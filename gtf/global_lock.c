#include "globals.h"

#ifdef LINUX_MODE
#include <linux/spinlock.h>

spinlock_t global_lock;
spinlock_t driver_lock;
static unsigned long global_lock_flags;
#endif

#ifdef FREEBSD_MODE
#include <sys/param.h>
#include <sys/module.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/lock.h>
#include <sys/mutex.h>

struct mtx global_lock;
struct mtx driver_lock;
#endif

static int global_lock_acquired = 0;

//static unsigned long driver_lock_flags;
//static int driver_lock_acquired = 0;

int initialize_global_lock (void) {
#ifdef LINUX_MODE
    spin_lock_init(&global_lock);
    spin_lock_init(&driver_lock);
#endif
#ifdef FREEBSD_MODE
    mtx_init(&global_lock, "tf_global_lock", "tf_lock", MTX_SPIN);
    mtx_init(&driver_lock, "tf_driver_lock", "tf_lock", MTX_SPIN);
#endif
    return 0;
}

void acquire_global_lock (void) {
    if (global_lock_acquired == 1) {
        uprintk ("WARNING:  WE'RE PROBABLY ABOUT TO DEADLOCK.  Check for recursive test framework calls.\n");
        uprintk ("If this message is followed by a stack trace, you're hosed, otherwise, ignore the message\n");
    }
#ifdef LINUX_MODE
    spin_lock_irqsave(&global_lock, global_lock_flags);
#endif
#ifdef FREEBSD_MODE
    mtx_lock_spin(&global_lock);
#endif
    assert (global_lock_acquired == 0, "Global lock not released.");
    global_lock_acquired = 1;
}

void release_global_lock (void) {
    assert (global_lock_acquired == 1, "Global lock released but not acquired.");
    global_lock_acquired = 0;
#ifdef LINUX_MODE
    spin_unlock_irqrestore(&global_lock, global_lock_flags);
#endif
#ifdef FREEBSD_MODE
    mtx_unlock_spin(&global_lock);
#endif
}

void assert_global_lock(void) {
    assert (global_lock_acquired == 1, "Global lock assumed to be acquired but is not.");
}

void free_global_lock(void) {
#ifdef FREEBSD_MODE
    mtx_destroy(&global_lock);
    mtx_destroy(&driver_lock);
#endif
}

/*
void acquire_driver_lock (void) {
    spin_lock_irqsave(&driver_lock, driver_lock_flags);
    driver_lock_acquired = 1;
}

void release_driver_lock (void) {
    if (driver_lock_acquired != 1) {
        assert (0, "Driver lock released but not acquired.");
    }
    driver_lock_acquired = 0;
    spin_unlock_irqrestore(&driver_lock, driver_lock_flags);
}
*/

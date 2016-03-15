#include "../test_state.h"
#include "../globals.h"
#include "irq.h"
#include "../mega_header.h"

#ifdef FREEBSD_MODE
#include <sys/param.h>
#include <sys/module.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/bus.h>
#include <sys/malloc.h>
#include <sys/rman.h>
#endif

//
// IRQ Handling
//
struct irq_struct {
    // 1 if this is a valid entry
    // 0 otherwise
    char              allocated;

    // IRQ number / handler / parameter for handler / flags / name
#ifdef LINUX_MODE
    int               number;
    irqreturn_t     (*handler)(int, void *);
#endif
#ifdef FREEBSD_MODE
    struct resource  *res;
    driver_intr_t    *handler;
#endif
    void             *parameter;
    unsigned long     flags;
#ifdef LINUX_MODE
    char              name[128];
#endif

    // 0 if this IRQ is currently disabled, 1 if enabled.
    char              enabled;
};

// Lines of code, not physical IRQ lines etc
#define MAX_IRQ_LINES    200000 /* See also MAX_INTERESTING_LINES */

// Max calls per line of code
#if defined(DISABLE_ANDROID_CYTTSP_MJRCHECKS) || defined(DISABLE_ME4000_MJRCHECKS)
// We count drivers in this block as having an annotation
#define MAX_INTERRUPTS_PER_LINE 1
#else
#define MAX_INTERRUPTS_PER_LINE 5
#endif

// Max number of request_irq calls.
#define MAX_IRQS 16

static struct irq_struct sym_irq[MAX_IRQS];
static int               sym_num_requests = 0;

static int               sym_irq_ongoing = 0;
static unsigned char    *sym_irq_lines = 0;

// TODO refactor all this crap so we don't need it:
extern struct ndo_driver_state_struct ndo_driver_state;

// Only call after initialize_irqs and before free_irqs
static void reset_one (int index) {
    sym_irq[index].allocated = 0;
#ifdef LINUX_MODE
    sym_irq[index].number = 0;
    sym_irq[index].handler = NULL;
#endif
#ifdef FREEBSD_MOD
    sym_irq[index].res = NULL;
    sym_irq[index].handler = NULL;
#endif
    sym_irq[index].parameter = NULL;
    sym_irq[index].flags = 0;
#ifdef LINUX_MODE
    sym_irq[index].name[0] = 0;
#endif
    sym_irq[index].enabled = 0;
}

static void reset_state (void) {
    int i;
    for (i = 0; i < MAX_IRQS; i++) {
        reset_one(i);
    }

    sym_irq_ongoing = 0;
    assert (sym_irq_lines != NULL, "%s: Failed -- why not allocated?\n", __func__);
    memset (sym_irq_lines, 0, sizeof (unsigned char) * MAX_IRQ_LINES);
    sym_num_requests = 0;
}

// Called at test_framework start up.
int initialize_irqs (void) {
    sym_irq_lines = mjr_kmalloc_nonatomic (sizeof (unsigned char) * MAX_IRQ_LINES);
    assert (sym_irq_lines != NULL,
            "Failed -- insufficient memory for sym_irq_lines\n");
    reset_state ();
    return 0;
}

// Called at test_framework shutdown.
void free_irqs (void) {
    reset_state ();

    if (sym_irq_lines != NULL) {
        mjr_kfree (sym_irq_lines);
    }
    sym_irq_lines = NULL;
}

int is_irq_allocated (void) {
    if (sym_num_requests > 0) {
        return 1;
    } else {
        return 0;
    }
}

//static void common_request_irq (const char *fn
//                                int prepost,
//                                int *retval,
//                                unsigned int irq,

#ifdef LINUX_MODE
STATIC int request_threaded_irq_MJRcheck (const char *fn,
                                          int prepost,
                                          int *retval,
                                          unsigned int *irq,
                                          irqreturn_t (**handler)(int, void *),  // "top half"
                                          irqreturn_t (**thread_fn)(int, void *), // "bottom half"
                                          unsigned long *flags,
                                          const char **dev_name,
                                          void **dev_id) {
    check_routine_start();
    if (prepost == 0) {
        tfassert_detail (dev_name != NULL, "%s dev_name == NULL\n", __func__);
        tfassert_detail (irq != NULL, "%s irq == NULL\n", __func__);
        
        uprintk ("Requesting IRQ (request_irq) %d %s, handler %p\n", *irq, *dev_name, *handler);
    } else if (prepost == 1) {
        int i;

        if (*retval) {
            uprintk ("Requesting IRQ failed... %d %s\n", *irq, *dev_name);
            goto end;
        }

        sym_num_requests++;

        for (i = 0; i < MAX_IRQS; i++) {
            if (sym_irq[i].allocated == 1) {
                continue;
            }

            assert (sym_irq[i].allocated == 0, "Allocated = 0 in request_irq_MJRcheck");
            sym_irq[i].allocated = 1;

            sym_irq[i].number = *irq;
            sym_irq[i].handler = *handler;
            sym_irq[i].parameter = *dev_id;
            sym_irq[i].flags = *flags;
            strcpy (sym_irq[i].name, *dev_name);

            sym_irq[i].enabled = 1;
            break;
        }

        tfassert_detail (i < MAX_IRQS, "Driver requesting many many IRQs?");
        assert (sym_irq_lines != NULL, "%s: Failed -- why not allocated?\n", __func__);
    }

  end:
    check_routine_end();
    return 0;
}

STATIC int free_irq_MJRcheck (const char *fn,
                              int prepost,
                              unsigned int *irq,
                              void **dev_id) {
    check_routine_start();

    if (prepost == 0) {
        int i;
        tfassert_detail (sym_num_requests >= 1, "Freeing IRQ incorrectly: %d %d", prepost, *irq);
        sym_num_requests--;
        for (i = 0; i < MAX_IRQS; i++) {
            if (sym_irq[i].allocated == 1 && sym_irq[i].number == *irq) {
                uprintk ("Calling %s %d...\n", __func__, *irq);
                tfassert_detail (sym_irq_ongoing == 0,
                                 "Weird: call stack suggest an IRQ is in progress but they're freeing it? %d\n",
                                 sym_irq_ongoing);
                tfassert_detail (*dev_id == sym_irq[i].parameter,
                                 "Ensure free_irq parameter matches request_irq parameter: %p != %p\n",
                                 *dev_id, sym_irq[i].parameter);
                reset_one(i);
                break;
            }
        }

        tfassert_detail (i < MAX_IRQS, "Well, the driver freed and IRQ without first requesting it.\n");
    }

    check_routine_end();
    return 0;
}

STATIC int enable_irq_MJRcheck(const char *fn,
                               int prepost,
                               unsigned int *irq) {
    check_routine_start();
    if (prepost == 1) {
        int i;
        for (i = 0; i < MAX_IRQS; i++) {
            if (sym_irq[i].allocated == 1 &&
                sym_irq[i].number == *irq) {
                tfassert_detail(sym_irq[i].enabled == 0,
                                "Why enable an enabled interrupt? IRQ %d", *irq);
                sym_irq[i].enabled = 1;
                break;
            }
        }

        tfassert_detail(i < MAX_IRQS, "Enabling interrupt that doesn't exist %d", *irq);
    }
    check_routine_end();
    return 0;
}

static int disable_interrupt(const char *fn,
                             int prepost,
                             unsigned int *irq) {
    check_routine_start();
    if (prepost == 1) {
        int i;
        for (i = 0; i < MAX_IRQS; i++) {
            if (sym_irq[i].allocated == 1 &&
                sym_irq[i].number == *irq) {
                tfassert_detail(sym_irq[i].enabled == 1,
                                "Why disable an enabled interrupt? IRQ %d", *irq);
                sym_irq[i].enabled = 0;
                break;
            }
        }

        tfassert_detail(i < MAX_IRQS, "Disabling interrupt that doesn't exist %d", *irq);
    }
    check_routine_end();
    return 0;
}

STATIC int disable_irq_MJRcheck(const char *fn,
                                int prepost,
                                unsigned int *irq) {
    return disable_interrupt(fn, prepost, irq);
}

STATIC int disable_irq_nosync_MJRcheck(const char *fn,
                                       int prepost,
                                       unsigned int *irq) {
    return disable_interrupt(fn, prepost, irq);
}

// Called dynamically via the epwrapper in the processed
// driver code, e.g. epwrapper_rtl8139_interrupt
//
// Alternative:  We could just call this function below,
// from call_interrupt_handlers, but then the stub would
// continue to try default_MJRcheck anyway.  So, we could modify
// DriverSlicer to special-case the interrupt handler by
// not generating a stub, to avoid the pointless default_MJRcheck lookup
// but that would be just as complex as this implementation, so
// forget it.
//
// TODO: We don't have FreeBSD support for this yet.
//
STATIC int interrupt_handler_MJRcheck(const char *fn,
                                      int prepost,
                                      irqreturn_t *retval,
                                      int *irq,
                                      void **driver_specific) {
    check_routine_start();
    if (prepost == 0) {
        push_blocking_state (fn, BLOCKING_NO);
        push_user_state (fn, USER_YES);
    } else if (prepost == 1) {
        pop_blocking_state (fn);
        pop_user_state (fn);

        if (g_sym_force_receive == 1) {
            static char called_before = 0;
            if (called_before == 0 && get_driver_class(fn) == DRIVER_NET) {
                if (ndo_driver_state.opened == CALLED_OK &&
                    (
                        ndo_driver_state.__napi_schedule == CALLED_OK ||
                        ndo_driver_state.netif_rx == CALLED_OK
                     )
                    ) {
                    // Maximize IRQ handler coverage one time
                    uprintk ("%s: Prioritizing interrupt handler for max coverage", __func__);
                    s2e_prioritize(0);
                    called_before = 1;
                } else {
                    if (ndo_driver_state.opened == CALLED_OK) {
                        uprintk ("%s: Deprioritizing interrupt handler for max coverage", __func__);
                        s2e_deprioritize(0);
                        called_before = 1;
                    }
                    else {
                        uprintk ("%s: Error: %d/%d/%d\n",
                                 __func__,
                                 ndo_driver_state.opened,
                                 ndo_driver_state.__napi_schedule,
                                 ndo_driver_state.netif_rx
                                 );
                    }
                }
            } else {
                uprintk ("%s:  Not working: %d/%d\n", __func__, called_before, get_driver_class(fn));
            }
        }
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}
#endif

#ifdef FREEBSD_MODE

/*
 * Information on the difference between "handler" and "filter:"
 *
 * The filter routine is run in primary interrupt context and may not
 * block or use regular mutexes.  It may only use spin mutexes for
 * synchronization.  The filter may either completely handle the
 * interrupt or it may perform some of the work and defer more
 * expensive work to the regular interrupt handler.  If a filter
 * routine is not registered by the driver, then the regular interrupt
 * handler is always used to handle interrupts from this device.
 *
 * The regular interrupt handler executes in its own thread context
 * and may use regular mutexes.  However, it is prohibited from
 * sleeping on a sleep queue.
 */

STATIC int bus_setup_intr_MJRcheck(const char *fn,
                                   int prepost,
                                   int *retval,
                                   device_t *dev,
                                   struct resource **r,
                                   int *flags,
                                   driver_filter_t **filter, // No * originally, now **
                                   driver_intr_t **handler, // No * originally, now **
                                   void **arg,
                                   void ***cookiep) {
    check_routine_start();
    if (prepost == 0) {
        uprintk ("Requesting IRQ (bus_setup_intr): filter: %p, handler %p, tag %d handle %d \n",
                 *filter, *handler, (*r)->r_bustag, (*r)->r_bushandle);
    } else if (prepost == 1) {
        int i;

        if (*retval) {
            uprintk ("Requesting IRQ failed...\n");
            goto end;
        }

        sym_num_requests++;

        for (i = 0; i < MAX_IRQS; i++) {
            if (sym_irq[i].allocated == 1) {
                continue;
            }

            assert (sym_irq[i].allocated == 0, "Allocated = 0 in request_irq_MJRcheck");
            sym_irq[i].allocated = 1;

            sym_irq[i].res = *r;
            if (*filter != NULL) {
                // Close enough -- just cast it.
                sym_irq[i].handler = (driver_intr_t *) *filter;
            } else {
                sym_irq[i].handler = *handler;
            }
            sym_irq[i].parameter = *arg;
            sym_irq[i].flags = *flags;

            sym_irq[i].enabled = 1;
            break;
        }

        tfassert_detail (i < MAX_IRQS, "Driver requesting many many IRQs?");
        assert (sym_irq_lines != NULL, "%s: Failed -- why not allocated?\n", __func__);
    }

  end:
    check_routine_end();
    return 0;
}

STATIC int
snd_setup_intr_MJRcheck(const char *fn,
                        int prepost,
                        int *retval,
                        device_t *dev,
                        struct resource **res,
                        int *flags,
                        driver_intr_t **hand, // Had no * originally, now it has **
                        void **param,
                        void ***cookiep) {
    // Don't pass NULL directly since bus_setup_intr_MJRcheck expects
    // all params to be non-null
    driver_filter_t *filter = NULL;
    return bus_setup_intr_MJRcheck(fn, prepost, retval, dev, res, flags,
                                   &filter, hand, param, cookiep);
}

STATIC int
bus_teardown_intr_MJRcheck(const char *fn,
                           int prepost,
                           int *retval,
                           device_t *dev,
                           struct resource **r,
                           void **cookie) {
    check_routine_start();

    if (prepost == 0) {
        int i;
        sym_num_requests--;
        for (i = 0; i < MAX_IRQS; i++) {
            if (sym_irq[i].allocated == 1 && sym_irq[i].res == *r) {
                uprintk ("Calling %s...\n", __func__);
                
                tfassert_detail (sym_irq_ongoing == 0,
                                 "Weird: call stack suggest an IRQ is in progress but they're freeing it? %d\n",
                                 sym_irq_ongoing);
                reset_one(i);
                break;
            }
        }

        tfassert_detail (i < MAX_IRQS, "Well, the driver freed an IRQ without first requesting it.\n");
    }

    check_routine_end();
    return 0;
}

//
// TODO NOTE HACK
// This function in FreeBSD can be called with either 3 or 4 arguments
// This is a huge hack.  Right now we don't refer to retval or 
// driver_specific so we're OK, but if we ever need to use those
// variables, then this function implementation will not work and we will
// need to differentiate between the driver_filter_t and driver_intr_t
// FreeBSD type definitions
//
STATIC int interrupt_handler_MJRcheck(const char *fn,
                                      int prepost,
                                      int *retval,
                                      void **driver_specific) {
    check_routine_start();
    if (prepost == 0) {
        push_blocking_state (fn, BLOCKING_NO);
        push_user_state (fn, USER_YES);
    } else if (prepost == 1) {
        pop_blocking_state (fn);
        pop_user_state (fn);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}
#endif

// Call the interrupt handler
void call_interrupt_handlers (const char *FUNCTION,
                              unsigned int LINE) {
    int i;
#ifdef LINUX_MODE
    unsigned long flags;
#endif
#ifdef FREEBSD_MODE
    register_t flags;
#endif

    // Only call the interrupt handler if it
    // hasn't already been called.
    if (sym_irq_ongoing > 0) {
        uprintk ("Ongoing interrupt, not calling from %s:%d...\n", FUNCTION, LINE);
        return;
    }

    if (can_call_interrupt_handlers () == 0) {
        uprintk ("Not allowed to call interrupt handler, not calling from %s:%d...\n", FUNCTION, LINE);
        return;
    }

    assert (LINE < MAX_IRQ_LINES && LINE >= 0,
            "Failed:  MAX_IRQ_LINES not big enough %d vs %d\n", MAX_IRQ_LINES, LINE);

//    uprintk ("Calling interrupt handler if available from %s:%d.  sym_irq_ongoing: %d\n",
//             FUNCTION, LINE, sym_irq_ongoing);

    sym_irq_ongoing++;

    // TODO Not sure if this next line is needed.
    // Added to deal with 8139too __napi_schedule call.
    // This call triggers a soft IRQ and if that runs during 8139too
    // interrupt handler there is a deadlock, since the rtl8139_poll
    // function acquires a spinlock already held by the interrupt handler.
    // (Recursive locking is bad).  So, is this fixing a bug in our code
    // or is it hiding a bug in 8139too?  I don't know.
#ifdef LINUX_MODE
    local_irq_save(flags);
#endif
#ifdef FREEBSD_MODE
    flags = intr_disable();
#endif
    for (i = 0; i < MAX_IRQS; i++) {
        if (sym_irq[i].allocated == 0) {
            continue;
        }

        if (sym_irq[i].enabled == 1) {
            sym_irq_lines[LINE]++;
            if (sym_irq_lines[LINE] <= MAX_INTERRUPTS_PER_LINE) {
#ifdef LINUX_MODE
                uprintk ("Calling interrupt handler from %s:%d, count:%d, i:%d, name %s, addr:%p!\n",
                         FUNCTION, LINE, sym_irq_lines[LINE], i, sym_irq[i].name, sym_irq[i].handler);
                sym_irq[i].handler (sym_irq[i].number, sym_irq[i].parameter);
#endif
#ifdef FREEBSD_MODE
                uprintk ("Calling interrupt handler from %s:%d, count:%d, i:%d,  addr:%p!\n",
                         FUNCTION, LINE, sym_irq_lines[LINE], i, (*sym_irq[i].handler));
                sym_irq[i].handler (sym_irq[i].parameter);
#endif
            }
            else {
                uprintk ("Not calling interrupt handler from %s:%d, too many calls: %d\n",
                         FUNCTION, LINE, sym_irq_lines[LINE]);
                if (sym_irq_lines[LINE] > 100) {
                    uprintk ("Resetting IRQ handler count from %s:%d, too many calls\n", FUNCTION, LINE);
                    sym_irq_lines[LINE] = MAX_INTERRUPTS_PER_LINE - 1;
                }
            }
        } else {
            uprintk ("Not calling interrupt handler from %s:%d because interrupt is disabled\n", FUNCTION, LINE);
        }
    }

    // TODO see above local_irq_save comment
#ifdef LINUX_MODE
    local_irq_restore(flags);
#endif
#ifdef FREEBSD_MODE
    intr_restore(flags);
#endif

    sym_irq_ongoing--;
    uprintk ("Completed %s, sym_irq_ongoing: %d\n", __func__, sym_irq_ongoing);
}

void execute_completions(const char *fn, unsigned int line) {
    //assert (0, "USB Support not implemented");
}

// Normal _MJRcheck routines
#ifdef LINUX_MODE
EXPORT_SYMBOL(request_threaded_irq_MJRcheck);
EXPORT_SYMBOL(free_irq_MJRcheck);
EXPORT_SYMBOL(enable_irq_MJRcheck);
EXPORT_SYMBOL(disable_irq_MJRcheck);
EXPORT_SYMBOL(disable_irq_nosync_MJRcheck);
EXPORT_SYMBOL(interrupt_handler_MJRcheck);

// These are not _MJRcheck routines
// EXPORT_SYMBOL(call_interrupt_handlers);
// EXPORT_SYMBOL(execute_completions);
#endif

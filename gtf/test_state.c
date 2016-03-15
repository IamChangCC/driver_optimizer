#include "test_state.h"
#include "globals.h"

#ifdef LINUX_MODE
#include <linux/sockios.h>
#include <linux/string.h>
#include <linux/capability.h>
#include <linux/errno.h>
#include <linux/compiler.h>
#include <linux/slab.h>
#endif

#ifdef FREEBSD_MODE
#include <sys/param.h>
#include <sys/module.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/malloc.h>
#endif

//
// Some helper functions for object tracking
//
static const char *ORIGIN_TO_STR(enum symdrive_OBJECT_ORIGIN origin);
static void objs_key (struct hkey *key,
               unsigned long object);
static struct hkey *objs_key_ptr (unsigned long object);
static struct hvalue *objs_value (const char *origin_str,
                                  enum symdrive_OBJECT_ORIGIN origin,
                                  unsigned long size,
                                  int new_object);
static void objs_insert (const char *fn,
                  struct hkey *key,
                  struct hvalue *value);
static struct hvalue *objs_search (struct hkey *key);
static struct hvalue *objs_remove (struct hkey *key);


static struct global_state_struct global_state;

struct pci_driver_state_struct pci_driver_state;
struct usb_driver_state_struct usb_driver_state;
struct misc_driver_state_struct misc_driver_state;
struct proto_ops_state_struct proto_ops_state;
struct i2c_driver_state_struct i2c_driver_state;
struct platform_driver_state_struct platform_driver_state;
struct spi_driver_state_struct spi_driver_state;

struct ndo_driver_state_struct ndo_driver_state;
struct sound_driver_state_struct sound_driver_state;

const char *ORIGIN_TO_STR(enum symdrive_OBJECT_ORIGIN origin) {
    static char buffer[256];
    buffer[0] = 0;

    if (origin & ORIGIN_KMALLOC) {
        strcat (buffer, "ORIGIN_KMALLOC ");
    }
    if (origin & ORIGIN_VMALLOC) {
        strcat (buffer, "ORIGIN_VMALLOC ");
    }
    if (origin & ORIGIN_GET_FREE_PAGES) {
        strcat (buffer, "ORIGIN_GET_FREE_PAGES ");
    }
    if (origin & ORIGIN_DMA_ALLOC_COHERENT) {
        strcat (buffer, "ORIGIN_DMA_ALLOC_COHERENT ");
    }
    if (origin & ORIGIN_PCI_ALLOC_CONSISTENT) {
        strcat (buffer, "ORIGIN_PCI_ALLOC_CONSISTENT ");
    }

    if (origin & ORIGIN_PCI_MAP_SINGLE) {
        strcat (buffer, "ORIGIN_PCI_MAP_SINGLE ");
    }
    if (origin & ORIGIN_PCI_MAP_PAGE) {
        strcat (buffer, "ORIGIN_PCI_MAP_PAGE ");
    }
    if (origin & ORIGIN_ALLOC_ETHERDEV_MQS) {
        strcat (buffer, "ORIGIN_ALLOC_ETHERDEV_MQS ");
    }
    if (origin & ORIGIN_ALLOC_URB) {
        strcat (buffer, "ORIGIN_ALLOC_URB ");
    }
    if (origin & ORIGIN_DEVICE_REGISTER) {
        strcat (buffer, "ORIGIN_DEVICE_REGISTER ");
    }

    if (origin & ORIGIN_SPIN_LOCK) {
        strcat (buffer, "ORIGIN_SPIN_LOCK ");
    }
    if (origin & ORIGIN_MUTEX) {
        strcat (buffer, "ORIGIN_MUTEX ");
    }
    if (origin & ORIGIN_RWSEM) {
        strcat (buffer, "ORIGIN_RWSEM ");
    }
    if (origin & ORIGIN_RWLOCK) {
        strcat (buffer, "ORIGIN_RWLOCK ");
    }

    if (origin & ORIGIN_IOREMAP) {
        strcat (buffer, "ORIGIN_IOREMAP ");
    }
    if (origin & ORIGIN_DEV_ALLOC_SKB) {
        strcat (buffer, "ORIGIN_DEV_ALLOC_SKB ");
    }

    return buffer;
}

void objs_key (struct hkey *key,
               unsigned long object) {
    key->object = object;
}

static struct hkey *objs_key_ptr (unsigned long object) {
    struct hkey *key = mjr_kmalloc_atomic (sizeof (struct hkey));
    objs_key (key, object);
    return key;
}

static struct hvalue *objs_value (const char *origin_str,
                                  enum symdrive_OBJECT_ORIGIN origin,
                                  unsigned long size,
                                  int new_object) {
    struct hvalue *value = mjr_kmalloc_atomic (sizeof (struct hvalue));
    memset(value, 0, sizeof (struct hvalue));
    strncpy(value->origin_str, origin_str, strlen(origin_str) - 1);
    value->origin = origin;
    value->size = size;
    value->new_object = new_object;
    return value;
}

static void objs_insert (const char *fn,
                         struct hkey *key,
                         struct hvalue *value) {
    struct hvalue *result = (struct hvalue *) hashtable_search (global_state.objs, key);
    int result2;
    if (result != NULL) {
        uprintk ("Object already present in table!");
        uprintk ("Origin: %s\n", ORIGIN_TO_STR(result->origin));
        tfassert (0);
    }

    if (value->size == 0 || value->size > 10000000) {
        if (value->size != -1) {
#ifndef DISABLE_ANDROID_A1026_MJRCHECKS
            uprintk ("Object size is insane: %lu\n", value->size);
            uprintk ("Origin: %s\n", ORIGIN_TO_STR(result->origin));
            tfassert (0);
#endif
        }
    }

    // If we have a memory failure of some kind, let's just abort rather
    // than propagate the error.
    result2 = hashtable_insert(global_state.objs, key, value);
    tfassert_detail (result2 != 0, "Failed insert");
    global_state.objs_dirty = 1; // We've inserted a new object.
}

static struct hvalue *objs_search (struct hkey *key) {
    return (struct hvalue *) hashtable_search(global_state.objs, key);
}

static struct hvalue *objs_remove (struct hkey *key) {
    return (struct hvalue *) hashtable_remove(global_state.objs, key);
}

static void hash_print_obj (void *vkey, void *vvalue) {
    struct hkey *key = (struct hkey *) vkey;
    struct hvalue *value = (struct hvalue *) vvalue;
    char b1[256];
    char b2[256];
    int i;

    b1[0] = 0;
    for (i = 0; i < NUM_LOCK_TYPES; i++) {
        sprintf (b2, "%d ", value->lock_state[i]);
        strcat (b1, b2);
    }

    uprintk ("Object: %lu/%p, from %s. Origin: %s/%d size %ld, lock: %s\n",
             key->object, (void *) key->object,
             value->origin_str, ORIGIN_TO_STR(value->origin),
             value->origin, (long) value->size, b1);
}

// Lame hash function but it should work
static unsigned int hash_from_key_fn (void *k) {
    struct hkey *key = (struct hkey *) k;
    return (unsigned int) key->object;
}

static int hash_keys_equal_fn (void *k1, void *k2) {
    struct hkey *key1 = (struct hkey *) k1;
    struct hkey *key2 = (struct hkey *) k2;

    if (key1->object == key2->object) {
        return 1;
    }

    return 0;
}

static void hash_reset_objs_state (void *vkey, void *vvalue) {
    struct hvalue *value = (struct hvalue *) vvalue;
    value->new_object = 0;
}

static void hash_print_new_obj (void *vkey, void *vvalue) {
    struct hvalue *value = (struct hvalue *) vvalue;
    if (value->new_object) {
        hash_print_obj(vkey, vvalue);
    }
}

void objs_enum_print (void) {
    uprintk ("All objects (may be empty):\n");
    hashtable_enumerate (global_state.objs, hash_print_obj);
    uprintk ("===================================\n");
}

void objs_enum_print_new (void) {
    uprintk ("New objects (may be empty):\n");
    hashtable_enumerate (global_state.objs, hash_print_new_obj);
    uprintk ("===================================\n");
}

///////////////////////////////////////////////////////////////////////////////
//
// Helper functions
//
///////////////////////////////////////////////////////////////////////////////
enum symdrive_driver_bus_type get_driver_bus (const char *fn) {
    int total;
    total = pci_driver_state.pci +
        usb_driver_state.usb +
        misc_driver_state.misc +
        proto_ops_state.proto_ops +
        i2c_driver_state.i2c +
        platform_driver_state.platform +
        spi_driver_state.spi;

    tfassert_detail (total >= 0,
                     "Total: %d is a bug\n", total);

    total = 0;
    if (pci_driver_state.pci != 0) {
        total |= DRIVER_PCI;
    }

    if (usb_driver_state.usb != 0) {
        total |= DRIVER_USB;
    }

    if (misc_driver_state.misc != 0) {
        total |= DRIVER_MISC;
    }

    if (proto_ops_state.proto_ops != 0) {
        total |= DRIVER_PROTO_OPS;
    }

    if (i2c_driver_state.i2c != 0) {
        total |= DRIVER_I2C;
    }

    if (platform_driver_state.platform != 0) {
        total |= DRIVER_PLATFORM;
    }

    if (spi_driver_state.spi != 0) {
        total |= DRIVER_SPI;
    }

    return total;
}

void init_driver_bus(void) {
    pci_driver_state.pci = 0;
    usb_driver_state.usb = 0;
    misc_driver_state.misc = 0;
    proto_ops_state.proto_ops = 0;
    i2c_driver_state.i2c = 0;
    platform_driver_state.platform = 0;
    spi_driver_state.spi = 0;
}

void set_driver_bus (const char *fn, enum symdrive_driver_bus_type type) {
    switch (type) {
        case DRIVER_UNKNOWN_BUS:
            tfassert_detail(0, "Don't set DRIVER_UNKNOWN_BUS");
            break;

        case DRIVER_PCI:
            tfassert_detail(pci_driver_state.pci == 0, "DRIVER_PCI already defined");
            pci_driver_state.pci = 1;
            break;

        case DRIVER_USB:
            tfassert_detail(usb_driver_state.usb == 0, "DRIVER_USB already defined");
            usb_driver_state.usb = 1;
            break;

        case DRIVER_MISC:
            tfassert_detail(misc_driver_state.misc == 0, "DRIVER_MISC already defined");
            misc_driver_state.misc = 1;
            break;

        case DRIVER_PROTO_OPS:
            tfassert_detail(proto_ops_state.proto_ops == 0, "DRIVER_PROTO_OPS already defined");
            proto_ops_state.proto_ops = 1;
            break;

        case DRIVER_I2C:
            tfassert_detail(i2c_driver_state.i2c == 0, "DRIVER_I2C already defined");
            i2c_driver_state.i2c = 1;
            break;

        case DRIVER_PLATFORM:
            tfassert_detail(platform_driver_state.platform == 0, "DRIVER_PLATFORM already defined");
            platform_driver_state.platform = 1;
            break;

        case DRIVER_SPI:
            tfassert_detail(spi_driver_state.spi == 0, "DRIVER_SPI already defined");
            spi_driver_state.spi = 1;
            break;

        default:
            tfassert_detail(0, "Unknown driver bus being set?\n");
            break;
    }
}

void remove_driver_bus (const char *fn, enum symdrive_driver_bus_type type) {
    switch (type) {
        case DRIVER_UNKNOWN_BUS:
            tfassert_detail (0, "Don't remove unknown bus");
            break;

        case DRIVER_PCI:
            tfassert_detail(pci_driver_state.pci == 1, "DRIVER_PCI not defined");
            pci_driver_state.pci = 0;
            break;

        case DRIVER_USB:
            tfassert_detail(usb_driver_state.usb == 1, "DRIVER_USB not defined");
            usb_driver_state.usb = 0;
            break;

        case DRIVER_MISC:
            tfassert_detail(misc_driver_state.misc == 1, "DRIVER_MISC not defined");
            misc_driver_state.misc = 0;
            break;

        case DRIVER_PROTO_OPS:
            tfassert_detail(proto_ops_state.proto_ops == 1, "DRIVER_PROTO_OPS not defined");
            proto_ops_state.proto_ops = 0;
            break;

        case DRIVER_I2C:
            tfassert_detail(i2c_driver_state.i2c == 1, "DRIVER_I2C not defined");
            i2c_driver_state.i2c = 0;
            break;

        case DRIVER_PLATFORM:
            tfassert_detail(platform_driver_state.platform == 1, "DRIVER_PLATFORM not defined");
            platform_driver_state.platform = 0;
            break;

        case DRIVER_SPI:
            tfassert_detail(spi_driver_state.spi == 1, "DRIVER_SPI not defined");
            spi_driver_state.spi = 0;
            break;

        default:
            tfassert_detail(0, "Unknown driver bus being removed?\n");
            break;
    }
}

enum symdrive_driver_class_type get_driver_class (const char *fn) {
    int total;
    total = ndo_driver_state.net +
        sound_driver_state.sound;
    tfassert_detail (total == 0 || total == 1, 
                     "Total: %d means multiple driver classes are defined\n", total);

    if (ndo_driver_state.net != 0) {
        return DRIVER_NET;
    }

    if (sound_driver_state.sound != 0) {
        return DRIVER_SND;
    }

    return DRIVER_UNKNOWN_CLASS;
}

void set_driver_class (const char *fn, enum symdrive_driver_class_type type) {
    enum symdrive_driver_bus_type existing_class = get_driver_class (fn);
    tfassert_detail (existing_class == DRIVER_UNKNOWN_CLASS,
                     "Known class %d\n",
                     existing_class);

    ndo_driver_state.net = 0;
    sound_driver_state.sound = 0;
    switch (type) {
        case DRIVER_UNKNOWN_CLASS:
            break;

        case DRIVER_NET:
            ndo_driver_state.net = 1;
            break;

        case DRIVER_SND:
            sound_driver_state.sound = 1;
            break;

        default:
            break;
    }
}

void set_device_state (const char *fn, enum symdrive_device_state *cur_state, enum symdrive_device_state new_state) {
    switch (*cur_state) {
        case DEVICE_STATE_UNDEFINED:
            break;
        case DEVICE_STATE_DISABLED:
            tfassert_detail(new_state == DEVICE_STATE_ENABLED,
                            "Device enabled/disabled incorrectly");
            break;
        case DEVICE_STATE_ENABLED:
            tfassert_detail(new_state == DEVICE_STATE_DISABLED,
                            "Device enabled/disabled incorrectly");
            break;
        default:
            tfassert (0);
    }

    *cur_state = new_state;
}

void set_call_state (const char *fn,
                     enum symdrive_call_state *state,
                     enum symdrive_call_state new_state) {
    if (new_state == IN_PROGRESS) {
        tfassert_detail (*state == NOT_CALLED ||
                         *state == CALLED_OK ||
                         *state == CALLED_FAILED, "failed1");
    }
    if (new_state == CALLED_OK || new_state == CALLED_FAILED) {
        tfassert_detail (*state == IN_PROGRESS, "failed2");
    }

    *state = new_state;
}

void set_blocking_state (const char *fn,
                         enum symdrive_blocking_context blocking_context) {
    //if (retval < 0) {
        // TODO:  perhaps use this as a clever way of checking for memory
        // leaks down multiple paths?
        //
        // In this case they've returned an error
        // probably.
    //}

    global_state.blocking_context_current = 0;
    global_state.blocking_context[global_state.blocking_context_current] = blocking_context;
}

void push_blocking_state (const char *fn,
                          enum symdrive_blocking_context blocking_context) {
    tfassert_detail(global_state.blocking_context_current >= 0,
                    "%s: set_blocking_state not called properly: %d.\n",
                    __func__, global_state.blocking_context_current);
    global_state.blocking_context_current++;
    global_state.blocking_context[global_state.blocking_context_current] = blocking_context;
    uprintk ("Pushing kernel state %s %d\n", fn, blocking_context);
    tfassert_detail(global_state.blocking_context_current < MAX_NESTING,
                    "Too many %s operations %d.\n",
                    __func__, global_state.blocking_context_current);

}

void pop_blocking_state (const char *fn) {
    tfassert_detail(global_state.blocking_context_current > 0,
                    "%s: set_blocking_state not called properly: %d.\n",
                    __func__, global_state.blocking_context_current);
    global_state.blocking_context_current--;
    uprintk ("Popping kernel state %s\n", fn);
    tfassert_detail(global_state.blocking_context_current >= 0,
                    "Too many %s operations %d.\n",
                    __func__, global_state.blocking_context_current);
}

enum symdrive_blocking_context kernel_blocking_state (void) {
    return global_state.blocking_context[global_state.blocking_context_current];
}

void set_user_state (const char *fn,
                     enum symdrive_user_context user_context) {
    global_state.user_context_current = 0;
    global_state.user_context[global_state.user_context_current] = user_context;
}

void push_user_state (const char *fn,
                      enum symdrive_user_context user_context) {
    tfassert_detail(global_state.user_context_current >= 0,
                    "%s: set_user_state not called properly: %d.\n",
                    __func__, global_state.user_context_current);
    global_state.user_context_current++;
    global_state.user_context[global_state.user_context_current] = user_context;
    uprintk ("Pushing kernel state %s %d\n", fn, user_context);
    tfassert_detail(global_state.user_context_current < MAX_NESTING,
                    "Too many %s operations %d.\n",
                    __func__, global_state.user_context_current);

}

void pop_user_state (const char *fn) {
    tfassert_detail(global_state.user_context_current > 0,
                    "%s: set_user_state not called properly: %d.\n",
                    __func__, global_state.user_context_current);
    global_state.user_context_current--;
    uprintk ("Popping kernel state %s\n", fn);
    tfassert_detail(global_state.user_context_current >= 0,
                    "Too many %s operations %d.\n",
                    __func__, global_state.user_context_current);
}

enum symdrive_user_context kernel_user_state (void) {
    return global_state.user_context[global_state.user_context_current];
}

void push_kernel_state (const char *fn,
                        enum symdrive_blocking_context blocking_context,
                        enum symdrive_user_context user_context) {
    push_blocking_state(fn, blocking_context);
    push_user_state(fn, user_context);
}

void pop_kernel_state (const char *fn) {
    pop_blocking_state(fn);
    pop_user_state(fn);
}

int set_device_interrupts (int disabled) {
    int old_state = global_state.device_irq_disabled;
    global_state.device_irq_disabled = disabled;
    return old_state;
}

int can_call_interrupt_handlers (void) {
    // Overrides other considerations.
    if (global_state.device_irq_disabled == 1) {
        return 0;
    }

    // 
    // Controlled via spin_lock_irqsave.
    // TODO: note that this approach is a bit restrictive,
    // since we may return 1 even if it's simply "blocking" that's
    // not allowed, as in the case of a "spin_lock" call
    // (for example) that allow interrupts through.
    //
    // Neverthless, this approach is MORE restrictive than we need
    // so should not result in false positives.
    //
    switch (global_state.blocking_context[global_state.blocking_context_current]) {
        case BLOCKING_YES:
        case BLOCKING_UNDEFINED:
            return 1;
        default:
            return 0;
    }
}

// Resets the list of new objects
void reset_new_objects (const char *fn) {
    if (global_state.objs_dirty) {
        hashtable_enumerate (global_state.objs, hash_reset_objs_state);
        global_state.objs_dirty = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// Called during startup
//
///////////////////////////////////////////////////////////////////////////////
int initialize_driver_state (void) {
    // Global state
    memset (&global_state, 0, sizeof (global_state));
    global_state.objs = create_hashtable(16, hash_from_key_fn, hash_keys_equal_fn);
    global_state.objs_dirty = 0;

    // All driver state
    set_driver_class (__func__, DRIVER_UNKNOWN_CLASS);
    init_driver_bus ();

    memset(&pci_driver_state, 0, sizeof (pci_driver_state));
    memset(&usb_driver_state, 0, sizeof (usb_driver_state));
    memset(&misc_driver_state, 0, sizeof (misc_driver_state));
    memset(&proto_ops_state, 0, sizeof (proto_ops_state));
    memset(&i2c_driver_state, 0, sizeof (i2c_driver_state));
    memset(&platform_driver_state, 0, sizeof (platform_driver_state));
    memset(&spi_driver_state, 0, sizeof (spi_driver_state));

    memset(&ndo_driver_state, 0, sizeof (ndo_driver_state));
    memset(&sound_driver_state, 0, sizeof (sound_driver_state));

    // Initialize kernel state
    set_blocking_state (__func__, BLOCKING_UNDEFINED);
    set_user_state (__func__, USER_UNDEFINED);

    return 0;
}

void free_driver_state (void) {
    unsigned int size = hashtable_count(global_state.objs);
    uprintk ("Hashtable size is %d\n", size);
    if (size != 0) {
        uprintk ("This size indicates an acquired resource was not freed!\n");
    }
    objs_enum_print();
    objs_enum_print_new();
    hashtable_destroy(global_state.objs, 1);
    global_state.objs = NULL;
}

//////////////////////////////////////////////////////////////////////////////
//
// Memory allocation
//
//////////////////////////////////////////////////////////////////////////////
//
// Fail if the given buffer has been allocated but is not big enough
// Does NOT fail if the given buffer is on the stack or is simply not
// allocated.
//
void assert_allocated_weak (const char *fn,
                            int prepost,
                            unsigned long object,
                            unsigned long minsize) {
    struct hvalue *on_heap;
    struct hkey key;

    objs_key (&key, object);
    on_heap = objs_search (&key);
    if (on_heap == NULL) {
        uprintk ("WARNING: Potential problem as buffer %p is not recorded as being allocated.\n",
                 (void *) object);
        return;
    }

    if (on_heap != NULL && on_heap->size < minsize) {
        uprintk ("%s/%d:  buffer not big enough %s!  Ptr: %p, size: %lu < minsize: %lu\n",
                 fn, prepost,
                 ORIGIN_TO_STR(on_heap->origin), (void *) object,
                 s2e_get_example_uint(minsize), s2e_get_example_uint(on_heap->size));
        tfassert (0);
    } else {
        return; // Definitely OK
    }
}

void assert_allocated (const char *fn,
                       int prepost,
                       enum symdrive_OBJECT_ORIGIN origin,
                       unsigned long object) {
    struct hvalue *on_heap;
    struct hkey key;

    objs_key (&key, object);
    on_heap = objs_search (&key);
    if (on_heap == NULL) {
        uprintk ("%s/%d:  buffer not allocated on the heap via %s (%d)!  Ptr: %p.\n",
                 fn, prepost, ORIGIN_TO_STR(origin), origin, (void *) object);
        tfassert (0);
    } else if (on_heap != NULL && (on_heap->origin & origin) == 0) {
        uprintk ("%s/%d:  buffer not allocated via %s (%d)!  Ptr: %p. origin: %s\n",
                 fn, prepost, ORIGIN_TO_STR(origin), origin, (void *) object, on_heap->origin_str);
        tfassert (0);
    } else {
        // We're in business
    }
}

void generic_allocator (const char *fn,
                        int prepost,
                        unsigned long object,
                        unsigned long object_size,
                        enum symdrive_OBJECT_ORIGIN origin) {
    if (prepost == 0) {
        assert (0, "Only call generic_allocator along the post condition branch");
    } else if (prepost == 1) {
        struct hkey *key;
        struct hvalue *value;
        struct hvalue *existing_value;
        if (IS_ERR ((void *) object) || object == 0) {
            uprintk ("Not adding bogus object to object tracker -- OT is for pointers\n");
            goto done;
        }

        value = objs_value (fn, origin, object_size, 1);
        key = objs_key_ptr (object);
        existing_value = objs_search (key);
        if (existing_value != NULL) {
            uprintk ("Failure:\n");
            uprintk ("Allocator already records this object is present: %lu / %lu\n", object, object_size);
            uprintk ("Value: %d, size: %lu\n", value->origin, value->size);
            tfassert (0);
        }
        objs_insert (fn, key, value);

        // Make sure we're allocated.  This should always pass.
        assert_allocated (fn, prepost, origin, object);
    } else {
        tfassert (0);
    }

    // TODO make whether to use this an dynamic command line option
    //objs_enum_print();
  done:
    return;
}

void generic_free (const char *fn,
                   int prepost,
                   unsigned long object,
                   enum symdrive_OBJECT_ORIGIN origin) {
    if (prepost == 0) {
        struct hkey key;
        struct hvalue *value;

        objs_key (&key, object);
        value = objs_remove (&key);
        if (value != NULL) {
            if ((value->origin & origin) == 0) {
                uprintk ("Failure\n");
                uprintk ("Object origin: %d\n", value->origin);
                uprintk ("Expected origin: %d\n", origin);
                uprintk ("Object: %lu\n", object);
                tfassert (0);
            }
            mjr_kfree (value);
        } else {
            tfassert_detail (0,
                             "Freeing something that we never allocated! Obj %p origin %d\n",
                             (void *) object, origin);
        }
    } else if (prepost == 1) {
        assert (0, "Only call generic_free along the precondition branch");
    } else {
        tfassert (0);
    }
    //objs_enum_print();
    return;
}

///////////////////////////////////////////////////////////////////////////////
//
// Locking protocol
//
///////////////////////////////////////////////////////////////////////////////

void generic_lock_allocator (const char *fn,
                             int prepost,
                             const void *lock,
                             enum symdrive_OBJECT_ORIGIN origin) {
    if (prepost == 0) {
        struct hkey *key;
        struct hvalue *value;
        struct hvalue *existing_value;

        tfassert(lock != NULL);

        key = objs_key_ptr ((unsigned long) lock);
        existing_value = objs_search (key);

        if (existing_value != NULL) {
            value = existing_value;
            objs_remove (key);
        } else {
            value = objs_value (fn, origin, (unsigned long) -1, 0);
        }

        tfassert(key != NULL);
        tfassert(value != NULL);

        objs_insert (fn, key, value);

        // Verify:
        {
            struct hkey current_key;
            objs_key (&current_key, (unsigned long) lock);
            existing_value = objs_search (&current_key);
            tfassert (existing_value != NULL);
        }
    } else if (prepost == 1) {
    } else {
        tfassert (0);
    }
    //objs_enum_print();
    return;
}

static void hash_check_lock (void *vkey, void *vvalue) {
    struct hkey *key = (struct hkey *) vkey;
    struct hvalue *value = (struct hvalue *) vvalue;
    const char *fn = __func__;
    int i;

    (void)key;

    for (i = 0; i < NUM_LOCK_TYPES; i++) {
        tfassert_detail(value->lock_state[i] == 0,
                        "Lock currently held: %d, state %d, origin %s\n",
                        i, value->lock_state[i], value->origin_str);
    }
}

void verify_all_locks_released(const char *fn) {
    hashtable_enumerate (global_state.objs, hash_check_lock);
}

static void verify_lock_state (const char *fn,
                               const struct hkey *key,
                               const struct hvalue *value) {
    int i;
    int found_locked = 0;
    for (i = 0; i < NUM_LOCK_TYPES; i++) {
        if (value->lock_state[i] == 1) {
            found_locked++;
        }

        tfassert_detail (value->lock_state[i] == 0 ||
                         value->lock_state[i] == 1,
                         "Found lock_state[%d] == %d\n",
                         i, value->lock_state[i]);
    }

    tfassert_detail (found_locked == 0 ||
                     found_locked == 1,
                     "Too many locks? %d\n", found_locked);
}

//
// This function tracks the current kernel state, and whether
// we can block or not.
//
static void lock_kernel_state (const char *fn,
                               enum symdrive_OBJECT_ORIGIN origin,
                               enum symdrive_LOCK_TYPE lock_type,
                               int add_this) {
    if (lock_type == LOCK_UNDEFINED) {
        tfassert (0);
    }
    else if (lock_type == SPIN_LOCK ||
             lock_type == SPIN_LOCK_BH ||
             lock_type == SPIN_LOCK_IRQSAVE ||
             lock_type == SPIN_LOCK_IRQ ||
             lock_type == RWLOCK) {
        tfassert_detail ((origin & ORIGIN_SPIN_LOCK) ||
                         (origin & ORIGIN_RWLOCK),
                         "Expected ORIGIN_SPIN_LOCK || ORIGIN_RWLOCK got %d\n",
                         origin);
        if (add_this > 0) {
            push_blocking_state(fn, BLOCKING_NO);
        } else {
            pop_blocking_state(fn);
        }
    }
    else if (lock_type == MUTEX ||
             lock_type == RWSEM) {
        tfassert_detail ((origin & ORIGIN_MUTEX) ||
                         (origin & ORIGIN_RWSEM),
                         "Expected ORIGIN_MUTEX || ORIGIN_RWSEM got %d\n",
                         origin);

        if (add_this > 0) {
            tfassert_detail (kernel_blocking_state () == BLOCKING_YES ||
                             kernel_blocking_state () == BLOCKING_UNDEFINED,
                             "Acquiring blocking lock on non-blocking path\n");
            push_blocking_state(fn, BLOCKING_YES);
        } else {
            pop_blocking_state(fn);
        }
    }
    else if (lock_type == GENERIC_LOCK) {
        // Do nothing for locking or unlocking -- current state
        // is unchanged.
    }
    else {
        tfassert (0);
    }
}

//
// If prepost == 0 and we're locking (add_this = 1), then do nothing interesting
// If prepost == 1 and we're unlocking (add_this = -1), then do nothing interesting.
//
void generic_lock_state (const char *fn,
                         int prepost,
                         const void *lock,
                         enum symdrive_OBJECT_ORIGIN origin,
                         enum symdrive_LOCK_TYPE lock_type,
                         int add_this) {
    struct hkey key;
    struct hvalue *value;

    tfassert_detail(add_this == -1 || add_this == 1, "Either lock or free\n");

    objs_key (&key, (unsigned long) lock);
    value = objs_search (&key);
    if (value == NULL) {
        //
        // Call ourselves recursively this one time.
        // The next call will not go down this branch
        // This strategy is necessary to deal with this kind of code:
        //
        //  static DEFINE_SPINLOCK(lock);
        //  static DEFINE_MUTEX(mutex);
        //
        // TODO: A better solution is to tfassert(0) on this branch,
        // and deal with DEFINE_SPINLOCK and DEFINE_MUTEX properly
        // by adding them to the object tracker before we get here.
        //
        generic_lock_allocator (fn, prepost, lock, origin);
        generic_lock_state (fn, prepost, lock, origin, lock_type, add_this);
        return;
    }

    // Commonly fired if the _check routine has an incorrect prototype.
    tfassert_detail((unsigned long) lock > 0x1000,
                    "Incorrect lock provided in %s, ptr: %p\n", __func__, lock);

    tfassert_detail (value != NULL, "Bug in generic_lock_state\n"); // redundant

    if (prepost == 0) {
        if (add_this == -1) {
            int unlock_generic = 0;
            // Unlocking during precondition
            tfassert_detail(lock_type != GENERIC_LOCK,
                            "Not supporting generic unlock");

            if (value->lock_state[GENERIC_LOCK] == 1) {
                // Generic is locked
                // Transfer lock to whatever
                // we're unlocking with
                value->lock_state[GENERIC_LOCK] += add_this;
                value->lock_state[lock_type] -= add_this;
                unlock_generic = 1;
            }

            // If we were locked via GENERIC_LOCK, we can now unlock
            // normally.
            value->lock_state[lock_type] += add_this;
            tfassert_detail(value->lock_state[lock_type] == 0,
                            "Invalid lock state! 0 != %d\n",
                            value->lock_state[lock_type]);
            verify_lock_state (fn, &key, value);

            if (unlock_generic) {
                lock_kernel_state (fn, origin, GENERIC_LOCK, add_this);
            } else {
                lock_kernel_state (fn, origin, lock_type, add_this);
            }
        } else if (add_this == 1) {
            if (value->lock_state[lock_type] == 1) {
                // Lock already acquired.
                // Not a failure but worth noting.
                uprintk ("Note: lock contention detected.  Deadlock possible.  Deprioritizing.");
                //s2e_deprioritize(0);
            }
        } else {
            tfassert (0);
        }
    } else if (prepost == 1) {
        if (add_this == -1) {
        } else if (add_this == 1) {
            // Locking during postcondition
            // If GENERIC_LOCK, then lock that index.
            value->lock_state[lock_type] += add_this;
            tfassert_detail(value->lock_state[lock_type] == 1,
                            "Invalid lock state!  1 != %d\n",
                            value->lock_state[lock_type]);
            verify_lock_state (fn, &key, value);
            lock_kernel_state (fn, origin, lock_type, add_this);
        } else {
            tfassert (0);
        }
    } else {
        tfassert (0);
    }
    //objs_enum_print();
    return;
}

///////////////////////////////////////////////////////////////////////////////
//
// Checking IOCTLs for capable()
//
///////////////////////////////////////////////////////////////////////////////
//
// Note that newer versions of the kernel include this check already
// so it's not necessary to rely on the driver to deal with it.
//

#ifdef LINUX_MODE

#define MAX_IOCTL_CHECKS 4
static int ioctl_capable_param = 0;
static enum symdrive_call_state ioctl_capable_retval = NOT_CALLED;

#if !defined(DISABLE_8139TOO_MJRCHECKS) && !defined(DISABLE_TG3_MJRCHECKS) && !defined(DISABLE_ECONET_MJRCHECKS)
static int ioctl_req_cmd[MAX_IOCTL_CHECKS] = {
    SIOCGMIIREG,
    SIOCSMIIREG,
    SIOCDEVPRIVATE + 2,
    
    // For econet:
    SIOCSIFADDR,
};

static int ioctl_req_capable_param[MAX_IOCTL_CHECKS] = {
    CAP_NET_ADMIN,
    CAP_NET_ADMIN,
    CAP_NET_ADMIN,

    // For econet:
    CAP_NET_ADMIN
};

static int capable_given_cmd(int cmd) {
    int i;
    for (i = 0; i < MAX_IOCTL_CHECKS; i++) {
        if (cmd == ioctl_req_cmd[i]) {
            return ioctl_req_capable_param[i];
        }
    }

    return -1;
}
#endif

void ioctl_called_capable(const char *fn, int prepost, int retval, int cmd) {
#if !defined(DISABLE_8139TOO_MJRCHECKS) && !defined(DISABLE_TG3_MJRCHECKS) && !defined(DISABLE_ECONET_MJRCHECKS)
    if (prepost == 0) {
    } else if (prepost == 1) {
        int capable_param = capable_given_cmd(cmd);
        if (capable_param != -1) {
            // This means the cmd was such that we should have called capable
            tfassert_detail (ioctl_capable_retval == CALLED_OK ||
                             ioctl_capable_retval == CALLED_FAILED,
                             "Capable not called\n");
            tfassert_detail (ioctl_capable_param == capable_param,
                             "Capable called with incorrect param\n");
        }

        // If we called it and it failed then we should be seeing
        // a permission error
        if (ioctl_capable_retval == CALLED_FAILED) {
            tfassert (retval == -EPERM);
        }
    } else {
        tfassert (0);
    }
#endif
}

void call_capable(const char *fn, int prepost, bool retval, int capability) {
    if (prepost == 0) {
        ioctl_capable_param = 0;
        set_call_state(fn, &ioctl_capable_retval, NOT_CALLED);
    } else if (prepost == 1) {
        ioctl_capable_param = capability;

        if (retval) {
            set_call_state(fn, &ioctl_capable_retval, CALLED_OK);
        } else {
            set_call_state(fn, &ioctl_capable_retval, CALLED_FAILED);
        }
    } else {
        tfassert (0);
    }
}

#endif // LINUX_MODE

//
// Memory flag checking
//
void mem_flags_test(const char *fn,
                    unsigned int blocking_no,
                    unsigned int blocking_yes,
                    unsigned int mem_flags) {
    if (mem_flags == blocking_yes) {
#if !defined(DISABLE_HOSTAP_MJRCHECKS)
        tfassert_detail (kernel_blocking_state () == BLOCKING_YES ||
                         kernel_blocking_state () == BLOCKING_UNDEFINED,
                         "Memory flags inconsistent: %d vs GFP_KERNEL\n",
                         kernel_blocking_state());
#endif
    } else if (mem_flags == blocking_no) {
#if !defined(DISABLE_USB_AUDIO_MJRCHECKS) && !defined(DISABLE_HOSTAP_MJRCHECKS)
        tfassert_detail (kernel_blocking_state () == BLOCKING_NO ||
                         kernel_blocking_state () == BLOCKING_UNDEFINED,
                         "Memory flags inconsistent: %d vs GFP_ATOMIC\n",
                         kernel_blocking_state());
#endif
    }
}


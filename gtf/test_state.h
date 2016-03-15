#ifndef TESTING_STATE_H
#define TESTING_STATE_H

#include "hashtable.h"
#include "globals.h"

#ifdef LINUX_MODE
#include <linux/types.h>
#endif

#ifdef FREEBSD_MODE
#include <sys/types.h>
#endif

///////////////////////////////////////////////////////////////////////////////
// Locking mechanism for _MJRcheck routines
// TODO Can also add feature to enable/disable checks here.
///////////////////////////////////////////////////////////////////////////////
#define check_routine_start()                                           \
    do {                                                                \
        acquire_global_lock();                                          \
        uprintk ("Checking %s %s %d\n", __func__, fn, prepost);         \
    } while (0)

#define check_routine_end()                     \
    do {                                        \
        release_global_lock();                  \
    } while (0)

///////////////////////////////////////////////////////////////////////////////
//
// Driver state
// If you add fields, be sure to initialize them in:
// initialize_driver_state
//
// Free everything in:
// free_driver_state
//
///////////////////////////////////////////////////////////////////////////////
enum symdrive_call_state {
    NOT_CALLED = 0,
    IN_PROGRESS = 1,
    CALLED_OK = 2,
    CALLED_FAILED = 3
};

enum symdrive_device_state {
    DEVICE_STATE_UNDEFINED = 0,
    DEVICE_STATE_DISABLED = 1,
    DEVICE_STATE_ENABLED = 2
};

//
// TODO: We're conflating preemption with interrupts.
// Not exactly the same thing.  Preemption may be disabled
// but interrupts may still be enabled.
// Compare local_irq_save() with preempt_disable()
//
enum symdrive_blocking_context {
    BLOCKING_UNDEFINED = 0,
    BLOCKING_YES = 1, // blocking allowed
    BLOCKING_NO = 2  // blocking not allowed
};

enum symdrive_user_context {
    USER_UNDEFINED = 0, // Unknown
    USER_YES = 1, // Execution path is such that user can invoke it
    USER_NO = 2 // Execution path is such that use cannot invoke it
};

enum symdrive_driver_bus_type {
    DRIVER_UNKNOWN_BUS = 0,
    DRIVER_PCI = 1,
    DRIVER_USB = 2,
    DRIVER_MISC = 4,
    DRIVER_PROTO_OPS = 8,
    DRIVER_I2C = 16,
    DRIVER_PLATFORM = 32,
    DRIVER_SPI = 64
};

enum symdrive_driver_class_type {
    DRIVER_UNKNOWN_CLASS = 0,
    DRIVER_NET,
    DRIVER_SND
};

#define MAX_NESTING 16

struct global_state_struct {
    enum symdrive_blocking_context blocking_context[MAX_NESTING];
    int blocking_context_current;

    enum symdrive_user_context user_context[MAX_NESTING];
    int user_context_current;

    // For manual override to enable/disable interrupts
    // Default = 0, means interrupts work as normal
    int device_irq_disabled;

    /* enum called_from_user user_context; */
    struct hashtable *objs;
    int objs_dirty;
};

struct pci_driver_state_struct {
    int pci;
    
    enum symdrive_call_state registered;
    enum symdrive_call_state unregistered;

    enum symdrive_device_state enabled;
    
    enum symdrive_call_state probed;
};

struct usb_driver_state_struct {
    int usb;
    
    enum symdrive_call_state registered;
    enum symdrive_call_state unregistered;
    enum symdrive_call_state probed;
};

struct misc_driver_state_struct  {
    int misc;
    
    enum symdrive_call_state registered;
    enum symdrive_call_state unregistered;
    enum symdrive_call_state probed;
};

struct proto_ops_state_struct {
    int proto_ops;

    enum symdrive_call_state registered;
    enum symdrive_call_state unregistered;
    enum symdrive_call_state probed;
};

struct i2c_driver_state_struct {
    int i2c;

    enum symdrive_call_state registered;
    enum symdrive_call_state unregistered;
    enum symdrive_call_state probed;
};

struct platform_driver_state_struct {
    int platform;

    enum symdrive_call_state registered;
    enum symdrive_call_state unregistered;
    enum symdrive_call_state probed;
};

struct spi_driver_state_struct {
    int spi;

    enum symdrive_call_state registered;
    enum symdrive_call_state unregistered;
    enum symdrive_call_state probed;
};

struct ndo_driver_state_struct {
    int net;
    
    enum symdrive_call_state opened;
    int trans_start_check;

    enum symdrive_call_state register_netdev;
    enum symdrive_call_state unregister_netdev;
    enum symdrive_call_state capable;
    enum symdrive_call_state __napi_schedule;
    enum symdrive_call_state netif_rx;

    int queue_state;
};

struct sound_driver_state_struct {
    int sound;
};

///////////////////////////////////////////////////////////////////////////////
//
// Hash table management
//
///////////////////////////////////////////////////////////////////////////////

//#if !defined(DISABLE_E1000_MJRCHECKS) && !defined(DISABLE_FORCEDETH_MJRCHECKS)
enum symdrive_OBJECT_ORIGIN
{
    ORIGIN_UNDEFINED = 0,

    ORIGIN_KMALLOC = 1,
    ORIGIN_VMALLOC = 2,
    ORIGIN_GET_FREE_PAGES = 4,
    ORIGIN_DMA_ALLOC_COHERENT = 8,
    ORIGIN_PCI_ALLOC_CONSISTENT = 16,

    ORIGIN_PCI_MAP_SINGLE = 32,
    ORIGIN_PCI_MAP_PAGE = 64,
    ORIGIN_SKB_DMA_MAP = 128,
    ORIGIN_ALLOC_ETHERDEV_MQS = 256,
    ORIGIN_ALLOC_URB = 512,
    ORIGIN_DEVICE_REGISTER = 1024,
    ORIGIN_GET_DEVICE = 2048,

    ORIGIN_SPIN_LOCK = 4096,
    ORIGIN_MUTEX = 8192,
    ORIGIN_RWSEM = 16384,
    ORIGIN_RWLOCK = 32768,

    ORIGIN_IOREMAP = 65536,
    ORIGIN_DEV_ALLOC_SKB = 65536 * 2,
    ORIGIN_CLASS_CREATE = 65536 * 4,
    ORIGIN_DEVICEFILE_CREATE = 65536 * 8
};
/*
#else
enum symdrive_OBJECT_ORIGIN
{
    ORIGIN_UNDEFINED = 0,

    ORIGIN_KMALLOC = 1,
    ORIGIN_VMALLOC = 2,
    ORIGIN_GET_FREE_PAGES = 4,
    ORIGIN_DMA_ALLOC_COHERENT = 8,
    ORIGIN_PCI_ALLOC_CONSISTENT = 16,

    ORIGIN_PCI_MAP_SINGLE = 32, // For E1000 and Forcedeth
    ORIGIN_PCI_MAP_PAGE = 32, // For E1000 and Forcedeth
    ORIGIN_SKB_DMA_MAP = 128,
    ORIGIN_ALLOC_ETHERDEV_MQS = 256,
    ORIGIN_ALLOC_URB = 512,
    ORIGIN_DEVICE_REGISTER = 1024,
    ORIGIN_GET_DEVICE = 2048,

    ORIGIN_SPIN_LOCK = 4096,
    ORIGIN_MUTEX = 8192,
    ORIGIN_RWSEM = 16384,
    ORIGIN_RWLOCK = 32768,

    ORIGIN_IOREMAP = 65536,
    ORIGIN_DEV_ALLOC_SKB = 65536 * 2
};
#endif
*/

//#if !defined (DISABLE_CA0106_MJRCHECKS) && !defined (DISABLE_ENS1371_MJRCHECKS) && !defined (DISABLE_USB_AUDIO_MJRCHECKS) && !defined (DISABLE_CMIPCI_MJRCHECKS)
enum symdrive_LOCK_TYPE
{
    LOCK_UNDEFINED = 0,

    SPIN_LOCK = 1,
    SPIN_LOCK_BH = 2,
    SPIN_LOCK_IRQSAVE = 3,
    SPIN_LOCK_IRQ = 4,
    
    MUTEX = 5,
    RWSEM = 6,
    RWLOCK = 7,

    GENERIC_LOCK = 8,
    NUM_LOCK_TYPES = 9
};
/*
#else
enum symdrive_LOCK_TYPE
{
    LOCK_UNDEFINED = 0,

    SPIN_LOCK = 1, // Treat these all the same.  We have issues in snd_timer_interrupt otherwise.
    SPIN_LOCK_BH = 1,
    SPIN_LOCK_IRQSAVE = 1,
    SPIN_LOCK_IRQ = 1,
    
    MUTEX = 2,
    RWSEM = 3,
    RWLOCK = 4,

    GENERIC_LOCK = 5,
    NUM_LOCK_TYPES = 6
};
#endif
*/

struct hkey {
    unsigned long object;
};

struct hvalue {
    char origin_str[64]; // Description of origin.
    enum symdrive_OBJECT_ORIGIN origin; // ORIGIN_blah
    unsigned long size; // Size of object or -1 if unknown/irrelevant
    int new_object; // 1 if new, 0 if not new.

    // See lock allocation / acquision / release code
    char lock_state[NUM_LOCK_TYPES]; // either 0 or 1 for each type of lock
};

//
// Managing the state we're in -- blocking / nonblocking / user-invokable etc
//
void init_driver_bus(void);
enum symdrive_driver_bus_type get_driver_bus (const char *fn);
void set_driver_bus (const char *fn, enum symdrive_driver_bus_type type);
void remove_driver_bus (const char *fn, enum symdrive_driver_bus_type type);

enum symdrive_driver_class_type get_driver_class (const char *fn);
void set_driver_class (const char *fn, enum symdrive_driver_class_type type);

void set_device_state (const char *fn,
                       enum symdrive_device_state *cur_state,
                       enum symdrive_device_state new_state);
void set_call_state (const char *fn,
                     enum symdrive_call_state *state,
                     enum symdrive_call_state new_state);

// Is this non-blocking or blocking?
void set_blocking_state (const char *fn,
                         enum symdrive_blocking_context blocking_context);
void push_blocking_state (const char *fn,
                          enum symdrive_blocking_context blocking_context);
void pop_blocking_state (const char *fn);
enum symdrive_blocking_context kernel_blocking_state (void);

// Can the user invoke this path?
void set_user_state (const char *fn,
                     enum symdrive_user_context user_context);
void push_user_state (const char *fn,
                      enum symdrive_user_context user_context);
void pop_user_state (const char *fn);
enum symdrive_user_context kernel_user_state (void);

// Both
void push_kernel_state (const char *fn,
                        enum symdrive_blocking_context blocking_context,
                        enum symdrive_user_context user_context);
void pop_kernel_state (const char *fn);                       

// Pass 1 to disable, 0 to enable, returns old value
int set_device_interrupts (int disabled);
// Returns whether we can call interrupt handlers
int can_call_interrupt_handlers (void);

void reset_new_objects (const char *fn);

//
// Setup / shutdown functions
//
int initialize_driver_state (void);
int initialize_enter_exit (void);
void free_driver_state (void);
void free_enter_exit (void);

//
// Memory allocations
//
void assert_allocated_weak (const char *fn,
                            int prepost,
                            unsigned long object,
                            unsigned long minsize);
void assert_allocated (const char *fn,
                       int prepost,
                       enum symdrive_OBJECT_ORIGIN origin,
                       unsigned long object);
void generic_allocator (const char *fn,
                        int prepost,
                        unsigned long object,
                        unsigned long object_size,
                        enum symdrive_OBJECT_ORIGIN origin);
void generic_free (const char *fn,
                   int prepost,
                   unsigned long object,
                   enum symdrive_OBJECT_ORIGIN origin);

//
// Locking protocol
//
void verify_all_locks_released(const char *fn);

void generic_lock_allocator (const char *fn,
                             int prepost,
                             const void *lock,
                             enum symdrive_OBJECT_ORIGIN origin);
void generic_lock_state (const char *fn,
                         int prepost,
                         const void *lock,
                         enum symdrive_OBJECT_ORIGIN origin,
                         enum symdrive_LOCK_TYPE lock_type,
                         int add_this);

//
// IOCTL checking
//
void ioctl_called_capable(const char *fn,
                          int prepost,
                          int retval,
                          int cmd);
void call_capable(const char *fn,
                  int prepost,
                  _Bool retval,
                  int capability);

//
// Memory flag testing
//
void mem_flags_test(const char *fn,
                    unsigned int blocking_no,
                    unsigned int blocking_yes,
                    unsigned int mem_flags);

//
// IRQ
//
int initialize_irqs (void);
void free_irqs (void);
int is_irq_allocated (void);

#endif

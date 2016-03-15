#include "test_state.h"
#include "testing_ep.h"

#ifdef LINUX_MODE
#include "bus_platform.h"
#include "bus_i2c.h"
#include "bus_spi.h"
#include <linux/module.h>
#endif

#ifdef FREEBSD_MODE
#include <sys/param.h>
#include <sys/module.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#endif

#define TF_STRING_LEN 32
int g_sym_retval = 0;
int g_sym_pci = 0;
int g_sym_params = 0;
char g_sym_starting_fn[TF_STRING_LEN];
char g_sym_primary_fn[TF_STRING_LEN];
int g_sym_kill_carrier_off = 0;
int g_sym_slow_timers = 1;
int g_sym_force_receive = 0;
int g_sym_enable_tracing = 0;

int g_sym_num_trackperf_fn;
char *g_sym_trackperf_fn[MAX_TRACKPERF];
int g_sym_num_trackperf_fn_flags;
int g_sym_trackperf_fn_flags[MAX_TRACKPERF];
int g_sym_trackperf_irq = SYMDRIVE_TRACKPERF_IRQ_NONE;

#ifdef LINUX_MODE
int g_platform_enable = 0;
char g_platform_name[TF_STRING_LEN];
int g_platform_nr = 0;
unsigned long g_platform_rflags[PCI_MAX_RESOURCES];
char *g_platform_rnames[MAX_CHIPS];

int g_i2c_enable = 0;
unsigned short g_i2c_chip_addr[MAX_CHIPS];
unsigned long g_i2c_functionality = STUB_FUNC;
char *g_i2c_names[MAX_CHIPS];

int g_spi_enable = 0;
char g_spi_name[TF_STRING_LEN];
#endif

#ifdef FREEBSD_MODE
// int g_trace_level = 2;
#endif

static void init_common(void) {
    int retval;

    retval = initialize_global_lock ();
    if (retval) {
        panic ("Failed initializing global lock %d\n", retval);
    }

    retval = initialize_function_tracker ();
    if (retval) {
        panic ("Failed initializing function tracker: %d\n", retval);
    }

    retval = initialize_driver_state ();
    if (retval) {
        panic ("Failed initializing test framework 1: %d\n", retval);
    }

    retval = initialize_irqs ();
    if (retval) {
        panic ("Failed initializing test framework 2: %d\n", retval);
    }

    retval = initialize_enter_exit();
    if (retval != 0) {
        panic ("Failed initializing test framework 3: %d\n", retval);
    }

    if (g_sym_enable_tracing) {
        s2e_enable_tracing(__LINE__);
    }

    s2e_primary_fn(__LINE__, g_sym_starting_fn);
    s2e_primary_fn(__LINE__, g_sym_primary_fn);

    {
        int i;
        if (g_sym_num_trackperf_fn != g_sym_num_trackperf_fn_flags) {
            panic ("%d should equal %d g_sym_num_trackperf!\n", g_sym_num_trackperf_fn, g_sym_num_trackperf_fn_flags);
        }

        for (i = 0; i < g_sym_num_trackperf_fn; i++) {
            if (g_sym_trackperf_fn_flags[i] != 0 && g_sym_trackperf_fn_flags[i] != 1) {
                panic ("Specify 0 (non transitive) or 1 (transitive) for each trackperf fn flag\n");
            }

            s2e_trackperf_fn(__LINE__, g_sym_trackperf_fn[i], g_sym_trackperf_fn_flags[i]);
        }

        if (g_sym_trackperf_irq == SYMDRIVE_TRACKPERF_IRQ_ALL) {
            s2e_enable_trackperf(__LINE__, SYMDRIVE_TRACKPERF_IRQ_ALL);
        } else if (g_sym_trackperf_irq == SYMDRIVE_TRACKPERF_IRQ_ONLY_CALLED) {
            s2e_enable_trackperf(__LINE__, SYMDRIVE_TRACKPERF_IRQ_ONLY_CALLED);
        } else if (g_sym_trackperf_irq == SYMDRIVE_TRACKPERF_IRQ_NONE) {
            s2e_enable_trackperf(__LINE__, SYMDRIVE_TRACKPERF_IRQ_NONE);
        } else {
            panic ("Specify SYMDRIVE_TRACKPERF_IRQ_* for g_sym_trackperf_irq\n");
        }
    }
}

static void cleanup_common (void) {
    if (g_sym_enable_tracing) {
        s2e_disable_tracing(__LINE__);
    }
    free_enter_exit();
    free_irqs ();
    free_driver_state();
    free_function_tracker();
    free_global_lock();
}

#ifdef LINUX_MODE
MODULE_AUTHOR ("Matthew Renzelmann");
MODULE_DESCRIPTION ("Test framework module");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");

// Supported in Linux and FreeBSD:
module_param(g_sym_retval, int, 0);
MODULE_PARM_DESC(g_sym_retval, "Force kernel retvals to be symbolic, "
                 "where possible.  0 = Do nothing.  1 = kill good paths on return to kernel.  "
                 "2 = deprioritize good path.  Options = 0|1|2");

module_param(g_sym_pci, int, 0);
MODULE_PARM_DESC(g_sym_pci, "Force PCI device info in pci_dev to be symbolic.  Options = 0|1");

module_param(g_sym_params, int, 0);
MODULE_PARM_DESC(g_sym_params, "Force module parameters to be symbolic.  Options = 0|1");

module_param_string(g_sym_starting_fn, g_sym_starting_fn, sizeof(g_sym_starting_fn), 0);
MODULE_PARM_DESC(g_sym_starting_fn, "Change scheduling algorithm from 'favor successful' to 'all'"
                 " once this function executes.");

module_param_string(g_sym_primary_fn, g_sym_primary_fn, sizeof(g_sym_primary_fn), 0);
MODULE_PARM_DESC(g_sym_primary_fn, "Notify S2E to prioritize paths with this function on the stack.");

module_param(g_sym_kill_carrier_off, int, 0);
MODULE_PARM_DESC(g_sym_kill_carrier_off, "Kill any path that calls netif_carrier_off");

module_param(g_sym_slow_timers, int, 0);
MODULE_PARM_DESC(g_sym_slow_timers, "Multiplication factor schedule_delayed_work and related functions");

module_param(g_sym_force_receive, int, 0);
MODULE_PARM_DESC(g_sym_force_receive, "Force interrupt handler to execute __napi_schedule.");

module_param(g_sym_enable_tracing, int, 0);
MODULE_PARM_DESC(g_sym_enable_tracing, "Enable I/O Memory and Port I/O tracing");

// Trackperf:
module_param_array(g_sym_trackperf_fn, charp, &g_sym_num_trackperf_fn, 0);
MODULE_PARM_DESC(g_sym_trackperf_fn, "Notify S2E to prioritize paths with this function on the stack.");

module_param_array(g_sym_trackperf_fn_flags, int, &g_sym_num_trackperf_fn_flags, 0);
MODULE_PARM_DESC(g_sym_trackperf_fn_flags,
                 "Trackperf flags -- must match g_sym_trackperf_fn in length. In doubt, use 0s.");

module_param(g_sym_trackperf_irq, int, 0);
MODULE_PARM_DESC(g_sym_trackperf_irq, "Enable interrupt handling tracking");

// Linux only:
module_param(g_platform_enable, int, 0);
MODULE_PARM_DESC(g_platform_enable, "Enable platform device emulation");

module_param_string(g_platform_name, g_platform_name, sizeof(g_platform_name), 0);
MODULE_PARM_DESC(g_platform_name, "Platform device - specify the name of the device to match");

module_param(g_platform_nr, int, 0);
MODULE_PARM_DESC(g_platform_nr, "Platform device - number of resources");

module_param_array(g_platform_rflags, ulong, NULL, 0);
MODULE_PARM_DESC(g_platform_rflags, "Flags for platform device resources -- # of these must match");

module_param_array(g_platform_rnames, charp, NULL, 0);
MODULE_PARM_DESC(g_platform_rnames, "Names of platform resources -- # of these must match");

module_param(g_i2c_enable, int, 0);
MODULE_PARM_DESC(g_i2c_enable, "Enable I2C device emulation");

module_param_array(g_i2c_chip_addr, ushort, NULL, 0);
MODULE_PARM_DESC(g_i2c_chip_addr, "Chip addresses (up to 10, between 0x03 and 0x77)");

module_param(g_i2c_functionality, ulong, 0);
MODULE_PARM_DESC(g_i2c_functionality, "Override g_i2c_functionality bitfield");

module_param_array(g_i2c_names, charp, NULL, 0);
MODULE_PARM_DESC(g_i2c_names, "Names of I2C devices -- be sure the number matches the number of chips");

module_param(g_spi_enable, int, 0);
MODULE_PARM_DESC(g_spi_enable, "Enable SPI device emulation");

module_param_string(g_spi_name, g_spi_name, sizeof(g_spi_name), 0);
MODULE_PARM_DESC(g_spi_name, "SPI device - specify the name of the device to match");

static void dump_params (void) {
    int i;
    char temp[16];
    static char buffer[4096];

    // Supported in Linux and FreeBSD:
    uprintk ("Parameters:\n");
    uprintk ("int g_sym_retval: %d\n", g_sym_retval);
    uprintk ("int g_sym_pci: %d\n", g_sym_pci);
    uprintk ("int g_sym_params: %d\n", g_sym_params);
    uprintk ("char g_sym_starting_fn[]: %s\n", g_sym_starting_fn);
    uprintk ("char g_sym_primary_fn[]: %s\n", g_sym_primary_fn);
    uprintk ("int g_sym_kill_carrier_off: %d\n", g_sym_kill_carrier_off);
    uprintk ("int g_sym_slow_timers: %d\n", g_sym_slow_timers);
    uprintk ("int g_sym_force_receive: %d\n", g_sym_force_receive);
    uprintk ("int g_sym_enable_tracing: %d\n", g_sym_enable_tracing);

    // Trackperf:
    uprintk ("int g_sym_num_trackperf_fn: %d\n", g_sym_num_trackperf_fn);
    uprintk ("char g_sym_trackperf_fn[]:\n");
    {
        buffer[0] = 0;
        for (i = 0; i < g_sym_num_trackperf_fn; i++) {
            sprintf (temp, "%s ", g_sym_trackperf_fn[i]);
            strcat (buffer, temp);
        }
        uprintk ("%s\n", buffer);
    }
    uprintk ("int g_sym_num_trackperf_fn_flags: %d\n", g_sym_num_trackperf_fn_flags);
    uprintk ("int g_sym_trackperf_fn_flags[]:\n");
    {
        buffer[0] = 0;
        for (i = 0; i < g_sym_num_trackperf_fn_flags; i++) {
            sprintf (temp, "%d ", g_sym_trackperf_fn_flags[i]);
            strcat (buffer, temp);
        }
        uprintk ("%s\n", buffer);
    }
    uprintk ("int g_sym_trackperf_irq: %d\n", g_sym_trackperf_irq);

    // Linux only
    uprintk ("int g_platform_enable: %d\n", g_platform_enable);
    uprintk ("char g_platform_name: %s\n", g_platform_name);
    uprintk ("int g_platform_nr: %d\n", g_platform_nr);
    {
        buffer[0] = 0;
        for (i = 0; i < PCI_MAX_RESOURCES; i++) {
            sprintf (temp, "%lu ", g_platform_rflags[i]);
            strcat (buffer, temp);
        }
        uprintk ("%s\n", buffer);
    }
    {
        buffer[0] = 0;
        for (i = 0; i < PCI_MAX_RESOURCES; i++) {
            sprintf (temp, "%s ", g_platform_rnames[i]);
            strcat (buffer, temp);
        }
        uprintk ("%s\n", buffer);
    }
    uprintk ("int g_i2c_enable: %d\n", g_i2c_enable);
    uprintk ("unsigned short g_i2c_chip_addr:\n");
    {
        buffer[0] = 0;
        for (i = 0; i < MAX_CHIPS; i++) {
            sprintf (temp, "%u ", g_i2c_chip_addr[i]);
            strcat (buffer, temp);
        }
        uprintk ("%s\n", buffer);
    }
    uprintk ("unsigned long g_i2c_functionality: %x\n", g_i2c_functionality);
    uprintk ("char *g_i2c_names[]:\n");
    {
        buffer[0] = 0;
        for (i = 0; i < MAX_CHIPS; i++) {
            sprintf (temp, "%s ",  g_i2c_names[i]);
            strcat (buffer, temp);
        }
        uprintk ("%s\n", buffer);
    }
    uprintk ("int g_spi_enable: %d\n", g_spi_enable);
    uprintk ("char g_spi_name: %s\n", g_spi_name);
}

static int linux_init(void) {
    int retval;
    dump_params();

    // Common init
    init_common();

    // Linux-specific init
    retval = initialize_ep ();
    if (retval) {
        panic ("Failed initializing test framework 4: %d\n", retval);
    }

    retval = initialize_ep_control();
    if (retval) {
        panic ("Failed initializing test framework 5: %d\n", retval);
    }

    retval = initialize_platform_device();
    if (retval != 0) {
        panic ("Failed initializing test framework 6: %d\n", retval);
    }

    retval = initialize_i2c_device();
    if (retval != 0) {
        panic ("Failed initializing test framework 7: %d\n", retval);
    }

    retval = initialize_spi_device();
    if (retval != 0) {
        panic ("Failed initializing test framework 8: %d\n", retval);
    }

    if (retval == 0) {
        uprintk ("Test framework initialized\n");
    }

    return retval;
}

static void linux_cleanup(void) {
    // Linux specific cleanup
    free_spi_device();
    free_i2c_device();
    free_platform_device();
    free_ep_control();

    // Common cleanup
    cleanup_common();
}

static int __init test_init (void)
{
    return linux_init();
}

static void __exit test_exit (void)
{
    linux_cleanup();
}

module_init(test_init);
module_exit(test_exit);

EXPORT_SYMBOL(g_sym_retval);
EXPORT_SYMBOL(g_sym_pci);
EXPORT_SYMBOL(g_sym_params);
EXPORT_SYMBOL(g_sym_kill_carrier_off);
#endif // LINUX_MODE

/////////////////////////////////////////////////////////////////////////////////

#ifdef FREEBSD_MODE

static int freebsd_init (void) {
    int retval;

    TUNABLE_INT_FETCH("test_framework.g_sym_retval", &g_sym_retval);
    TUNABLE_INT_FETCH("test_framework.g_sym_pci", &g_sym_pci);;
    TUNABLE_INT_FETCH("test_framework.g_sym_params", &g_sym_params);
    TUNABLE_STR_FETCH("test_framework.g_sym_starting_fn", g_sym_starting_fn, TF_STRING_LEN);
    TUNABLE_STR_FETCH("test_framework.g_sym_primary_fn", g_sym_primary_fn, TF_STRING_LEN);
    //TUNABLE_STR_FETCH("test_framework.g_sym_trackperf_fn", g_sym_trackperf_fn, TF_STRING_LEN);
    TUNABLE_INT_FETCH("test_framework.g_sym_kill_carrier_off", &g_sym_kill_carrier_off);
    TUNABLE_INT_FETCH("test_framework.g_sym_slow_timers", &g_sym_slow_timers);
    TUNABLE_INT_FETCH("test_framework.g_sym_force_receive", &g_sym_force_receive);
    TUNABLE_INT_FETCH("test_framework.g_sym_enable_tracing", &g_sym_enable_tracing);
    
    // Common init
    init_common();

    // FreeBSD specific init
    retval = initialize_dma_map();
    if (retval != 0) {
        panic ("Failed initializing test framework DMA map: %d\n", retval);
    }
    
    return retval;
}

static void freebsd_cleanup(void) {
    // FreeBSD specific cleanup
    free_dma_map();

    // General cleanup
    cleanup_common();
}

/* The function called at load/unload. */
static int event_handler(struct module *module, int event, void *arg) {
    int retval = 0; /* Error, 0 for normal return status */
    switch (event) {
        case MOD_LOAD:
            retval = freebsd_init();
            printf ("Tracing module loaded\n");
            break;
        case MOD_UNLOAD:
            freebsd_cleanup();
            printf ("Tracing module unloaded\n");
            break;
        default:
            retval = EOPNOTSUPP; /* Error, Operation Not Supported */
            break;
    }
       
    return(retval);
}

/* The second argument of DECLARE_MODULE. */
static moduledata_t test_framework_conf = {
    "test_framework",    /* module name */
    event_handler,       /* event handler */
    NULL                 /* extra data */
};

DECLARE_MODULE(test_framework, test_framework_conf, SI_SUB_DRIVERS, SI_ORDER_MIDDLE);
MODULE_VERSION(test_framework, 1);

#endif // FREEBSD_MODE

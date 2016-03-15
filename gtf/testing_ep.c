//#define TESTING_EP_FULL
#include "test_state.h"
#include "testing_ep.h"
#include "globals.h"
#include "mega_header.h"

#define BIG_ENOUGH 1000

static struct ep_state_struct {
    // For network devices
    struct net_device *net_dev;
    void (*ethtool_fn_array[BIG_ENOUGH])(void);
    int ethtool_fn_array_len;

    // For PCI devices
    struct pci_driver *pci_drv;
    struct pci_dev *pci_dev;
    void (*pci_pm_fn_array[BIG_ENOUGH])(void);
    int pci_pm_fn_array_len;

    // For I2C devices
    struct i2c_driver *i2c_drv;
    struct i2c_client *i2c_client;
    void (*i2c_pm_fn_array[BIG_ENOUGH])(void);
    int i2c_pm_fn_array_len;

    // For sound devices
    //struct file *snd_file;
    //void (*snd_fn_array[BIG_ENOUGH])(void);
    //int snd_fn_array_len;

    //
    // We use this for sound drivers.  The idea is that if this
    // flag is set, we use the test framework to replace incoming
    // concrete kernel data with symbolic data.
    //
    int ep_symexec;
} ep_state;

//////////////////////////////////////////////////////////////////////////////
// A couple extra functions that don't do anything but keep things tidy
//////////////////////////////////////////////////////////////////////////////
static void test_start (void) {
    uprintk ("Test start...\n");
}

static void test_end (void) {
    uprintk ("Test end...\n");
}

//////////////////////////////////////////////////////////////////////////////
// Network driver test functions
//////////////////////////////////////////////////////////////////////////////

#define ETHTOOL_BASIC_CALL(fn, args...)                         \
    do {                                                        \
        uprintk ("Ethtool testing %s\n", __func__);             \
        if (ep_state.net_dev == NULL) {                         \
            assert (0, "Failed to invoke %s 1\n", __func__);    \
        }                                                       \
                                                                \
        if (ep_state.net_dev->ethtool_ops == NULL) {            \
            assert (0, "Failed to invoke %s 2\n", __func__);    \
        }                                                       \
                                                                \
        if (ep_state.net_dev->ethtool_ops->fn != NULL) {        \
            ep_state.net_dev->ethtool_ops->fn(                  \
                ep_state.net_dev , ## args);                    \
        }                                                       \
    } while(0)                                                  \
        
#define ETHTOOL_CALL2(fn, ttd2, addr2)                          \
    do {                                                        \
        ttd2 o2;                                                \
        uprintk ("Ethtool testing %s\n", __func__);             \
        memset (&o2, 0, sizeof(ttd2));                          \
        s2e_make_symbolic (&o2, sizeof(ttd2), __func__);        \
        if (ep_state.net_dev->ethtool_ops->fn != NULL) {        \
            ep_state.net_dev->ethtool_ops->fn(ep_state.net_dev, \
                                              addr2 o2);        \
        }                                                       \
    } while (0)

#define ETHTOOL_CALL3(fn, ttd2, addr2, ttd3, addr3)                     \
    do {                                                                \
        ttd2 o2;                                                        \
        ttd3 o3;                                                        \
        uprintk ("Ethtool testing %s\n", __func__);                     \
        memset (o2, 0, sizeof (ttd2));                                  \
        memset (o3, 0, sizeof (ttd3));                                  \
        s2e_make_symbolic (o2, sizeof(ttd2), __func__);                 \
        s2e_make_symbolic (o3, sizeof(ttd3), __func__);                 \
        if (ep_state.net_dev->ethtool_ops->fn != NULL) {                \
            ep_state.net_dev->ethtool_ops->fn(ep_state.net_dev,         \
                                              addr2 o2, addr3 o3);      \
        }                                                               \
    } while (0)                                                         \

static void ethtool_get_settings(void) {
    ETHTOOL_CALL2(get_settings, struct ethtool_cmd, &);
}

static void ethtool_set_settings(void) {
    ETHTOOL_CALL2(set_settings, struct ethtool_cmd, &);
}

static void ethtool_get_drvinfo(void) {
    ETHTOOL_CALL2(get_drvinfo, struct ethtool_drvinfo, &);
}

static void ethtool_get_regs_len(void) {
    ETHTOOL_BASIC_CALL(get_regs_len);
}

static void ethtool_get_regs(void) {
    struct ethtool_regs o2;
    u8 *o3 = kmalloc(BIG_ENOUGH, GFP_KERNEL);
    memset (&o2, 0, sizeof (struct ethtool_regs));
    s2e_make_symbolic (&o2, sizeof(struct ethtool_regs), __func__);
    ETHTOOL_BASIC_CALL(get_regs, &o2, o3);
    kfree(o3);
}

static void ethtool_get_wol(void) {
    ETHTOOL_CALL2(get_wol, struct ethtool_wolinfo, &);
}

static void ethtool_set_wol(void) {
    ETHTOOL_CALL2(set_wol, struct ethtool_wolinfo, &);
}

static void ethtool_get_msglevel(void) {
    ETHTOOL_BASIC_CALL(get_msglevel);
}

static void ethtool_set_msglevel(void) {
    ETHTOOL_CALL2(set_msglevel, u32, );
}

static void ethtool_nway_reset(void) {
    ETHTOOL_BASIC_CALL(nway_reset);
}

static void ethtool_get_link(void) {
    ETHTOOL_BASIC_CALL(get_link);
}

static void ethtool_get_eeprom_len(void) {
    ETHTOOL_BASIC_CALL(get_eeprom_len);
}

static void ethtool_get_eeprom(void) {
    struct ethtool_eeprom o2;
    u8 *o3 = kmalloc(BIG_ENOUGH, GFP_KERNEL);
    memset (&o2, 0, sizeof (struct ethtool_eeprom));
    s2e_make_symbolic (&o2, sizeof(struct ethtool_eeprom), __func__);
    ETHTOOL_BASIC_CALL(get_eeprom, &o2, o3);
    kfree(o3);
}

static void ethtool_set_eeprom(void) {
    struct ethtool_eeprom o2;
    u8 *o3 = kmalloc(BIG_ENOUGH, GFP_KERNEL);
    memset (&o2, 0, sizeof (struct ethtool_eeprom));
    memset (o3, 0, sizeof (u8) * BIG_ENOUGH);
    s2e_make_symbolic (&o2, sizeof(struct ethtool_eeprom), __func__);
    s2e_make_symbolic (o3, sizeof(u8) * BIG_ENOUGH, __func__);
    ETHTOOL_BASIC_CALL(set_eeprom, &o2, o3);
    kfree(o3);
}

static void ethtool_get_coalesce(void) {
    ETHTOOL_CALL2(get_coalesce, struct ethtool_coalesce, &);
}

static void ethtool_set_coalesce(void) {
    ETHTOOL_CALL2(set_coalesce, struct ethtool_coalesce, &);
}

static void ethtool_get_ringparam(void) {
    ETHTOOL_CALL2(get_ringparam, struct ethtool_ringparam, &);
}

static void ethtool_set_ringparam(void) {
    ETHTOOL_CALL2(set_ringparam, struct ethtool_ringparam, &);
}

static void ethtool_get_pauseparam(void) {
    ETHTOOL_CALL2(get_pauseparam, struct ethtool_pauseparam, &);
}

static void ethtool_set_pauseparam(void) {
    ETHTOOL_CALL2(set_pauseparam, struct ethtool_pauseparam, &);
}

static void ethtool_get_rx_csum(void) {
    ETHTOOL_BASIC_CALL(get_rx_csum);
}

static void ethtool_set_rx_csum(void) {
    ETHTOOL_CALL2(set_rx_csum, u32, );
}

static void ethtool_get_tx_csum(void) {
    ETHTOOL_BASIC_CALL(get_tx_csum);
}

static void ethtool_set_tx_csum(void) {
    ETHTOOL_CALL2(set_tx_csum, u32, );
}

static void ethtool_get_sg(void) {
    ETHTOOL_BASIC_CALL(get_sg);
}

static void ethtool_set_sg(void) {
    ETHTOOL_CALL2(set_sg, u32, );
}

static void ethtool_get_tso(void) {
    ETHTOOL_BASIC_CALL(get_tso);
}

static void ethtool_set_tso(void) {
    ETHTOOL_CALL2(set_tso, u32, );
}

static void ethtool_self_test(void) {
    struct ethtool_test o2;
    u64 *o3 = kmalloc(sizeof(u64) * BIG_ENOUGH, GFP_KERNEL);
    memset (&o2, 0, sizeof (struct ethtool_test));
    memset (o3, 0, sizeof (u8) * BIG_ENOUGH);
    s2e_make_symbolic (&o2, sizeof(struct ethtool_test), __func__);
    s2e_make_symbolic (o3, sizeof(u64) * BIG_ENOUGH, __func__);
    ETHTOOL_BASIC_CALL(self_test, &o2, o3);
    kfree(o3);
}

static void ethtool_get_strings(void) {
    u32 o2 = 0;
    u8 *o3 = kmalloc(sizeof(u8) * BIG_ENOUGH, GFP_KERNEL);
    memset (o3, 0, sizeof (u8) * BIG_ENOUGH);
    s2e_make_symbolic (&o2, sizeof(u32), __func__);
    s2e_make_symbolic (o3, sizeof(u8) * BIG_ENOUGH, __func__);
    ETHTOOL_BASIC_CALL(get_strings, o2, o3);
    kfree(o3);
}

static void ethtool_set_phys_id(void) {
    ETHTOOL_CALL2(set_phys_id, u32, );
}

static void ethtool_get_ethtool_stats(void) {
    struct ethtool_stats o2;
    u64 *o3 = kmalloc(sizeof(u64) * BIG_ENOUGH, GFP_KERNEL);
    memset (&o2, 0, sizeof (struct ethtool_stats));
    memset (o3, 0, sizeof (u64) * BIG_ENOUGH);
    s2e_make_symbolic (&o2, sizeof(struct ethtool_stats), __func__);
    s2e_make_symbolic (o3, sizeof(u64) * BIG_ENOUGH, __func__);
    ETHTOOL_BASIC_CALL(get_ethtool_stats, &o2, o3);
    kfree(o3);
}

static void ethtool_begin(void) {
    ETHTOOL_BASIC_CALL(begin);
}

static void ethtool_complete(void) {
    ETHTOOL_BASIC_CALL(complete);
}

static void ethtool_get_ufo(void) {
    ETHTOOL_BASIC_CALL(get_ufo);
}

static void ethtool_set_ufo(void) {
    ETHTOOL_CALL2(set_ufo, u32, );
}

static void ethtool_get_flags(void) {
    ETHTOOL_BASIC_CALL(get_flags);
}

static void ethtool_set_flags(void) {
    ETHTOOL_CALL2(set_flags, u32, );
}

static void ethtool_get_priv_flags(void) {
    ETHTOOL_BASIC_CALL(get_priv_flags);
}

static void ethtool_set_priv_flags(void) {
    ETHTOOL_CALL2(set_priv_flags, u32, );
}

static void ethtool_get_sset_count(void) {
    ETHTOOL_CALL2(get_sset_count, int, );
}

//////////////////////////////////////////////////////////////////////////////
// PCI/I2C driver test functions
//////////////////////////////////////////////////////////////////////////////
static void pci_suspend(void) {
    struct pci_dev *pci_dev = ep_state.pci_dev;
    struct pci_driver *pci_drv = ep_state.pci_drv;
    pm_message_t message;

    assert (pci_dev != NULL, "pci_dev must not be null when doing %s test", __func__);
    assert (pci_drv != NULL, "pci_drv must not be null when doing %s test", __func__);

    s2e_make_symbolic(&message.event, sizeof(int), __func__);
    pci_drv->suspend(pci_dev, message);
}

static void pci_resume(void) {
    struct pci_dev *pci_dev = ep_state.pci_dev;
    struct pci_driver *pci_drv = ep_state.pci_drv;

    assert (pci_dev != NULL, "pci_dev must not be null when doing %s test", __func__);
    assert (pci_drv != NULL, "pci_drv must not be null when doing %s test", __func__);

    pci_drv->resume(pci_dev);
}

static void i2c_suspend(void) {
    struct i2c_client *i2c_client = ep_state.i2c_client;
    struct i2c_driver *i2c_drv = ep_state.i2c_drv;
    pm_message_t message;

    assert (i2c_client != NULL, "i2c_client must not be null when doing %s test", __func__);
    assert (i2c_drv != NULL, "i2c_drv must not be null when doing %s test", __func__);

    s2e_make_symbolic(&message.event, sizeof(int), __func__);
    i2c_drv->suspend(i2c_client, message);
}

static void i2c_resume(void) {
    struct i2c_client *i2c_client = ep_state.i2c_client;
    struct i2c_driver *i2c_drv = ep_state.i2c_drv;

    assert (i2c_client != NULL, "i2c_client must not be null when doing %s test", __func__);
    assert (i2c_drv != NULL, "i2c_drv must not be null when doing %s test", __func__);

    i2c_drv->resume(i2c_client);
}


//////////////////////////////////////////////////////////////////////////////
// Functions called from outside world
//////////////////////////////////////////////////////////////////////////////

static void ep_test_generic (enum symdrive_TEST_CATEGORY category,
                             int fn_id,
                             int begin,
                             int end,
                             void (*fn_array[BIG_ENOUGH])(void),
                             int disable_interrupts) {
    int run_test = 0;

    uprintk ("%s starting, category: %u, fn_id: %d, begin: %d, end: %d, disable_interrupts: %d...\n",
             __func__, category, fn_id, begin, end, disable_interrupts);

    s2e_favor_successful(0, 0);

    // The idea behind this implementation is to yield and then
    // run ethtool.  But the yield feature doesn't work well, so
    // we'll just merge results at the end from multiple runs.

    //s2e_make_symbolic(&run_test, sizeof(int), "run_test_int");
    // 
    //s2e_favor_successful(0, 0);
    //if (run_test == 1) {
    //    s2e_yield(0);
    //    uprintk ("Called %s:  proceeding post yield...\n");
    //    s2e_favor_successful(0, 1);
    //    return;
    //}

    // Disable "device" interrupts
    // Just disables our calls to interrupt handler.
    // Per-state so no need to "re-enable" since
    // all states here and below die.
    if (disable_interrupts) {
        set_device_interrupts(1);
    }
    
    // Make concrete again.
    run_test = 0;

    if (fn_id < begin || fn_id >= end) {
        assert (0, "Failed to invoke test case: %d\n", fn_id);
    }

    if (fn_id == begin) {
        // Clever trick to fork states
        uprintk ("%s: forking states...\n", __func__);
        s2e_make_symbolic(&run_test, sizeof(int), __func__);
        if ((run_test >= begin + 1) &&
            (run_test <= end - 1)) {
            fn_array[run_test]();
        } else {
            s2e_kill_state (1, 0, "Killing extra state in ep_test_generic");
        }
    }
    else {
        fn_array[fn_id]();
    }

    s2e_kill_state(1, 0, "Victory - test case(s) complete\n");
}

void ep_test (enum symdrive_TEST_CATEGORY category, int fn_id) {
    switch (category) {
        case TEST_ETHTOOL:
            uprintk ("Invoking TEST_ETHTOOL %s\n", __func__);
            ep_test_generic(category,
                            fn_id,
                            TEST_CASE_ETHTOOL_BEGIN,
                            TEST_CASE_ETHTOOL_END,
                            ep_state.ethtool_fn_array,
                            1);
            break;
        case TEST_PCI_PM:
            uprintk ("Invoking %s\n", __func__);
            ep_test_generic(category,
                            fn_id,
                            TEST_CASE_PCI_PM_BEGIN,
                            TEST_CASE_PCI_PM_END,
                            ep_state.pci_pm_fn_array,
                            0);
            break;
        case TEST_I2C_PM:
            uprintk ("Invoking %s\n", __func__);
            ep_test_generic(category,
                            fn_id,
                            TEST_CASE_I2C_PM_BEGIN,
                            TEST_CASE_I2C_PM_END,
                            ep_state.i2c_pm_fn_array,
                            0);
            break;
        default:
            assert (0, "Invalid test category specified");
    }
}

int initialize_ep (void) {
    int i;
    memset (&ep_state, 0, sizeof (struct ep_state_struct));

    // Could do this all statically
    // Initialize ethtool function array.
    i = 0;
    ep_state.ethtool_fn_array[i++] = test_start; // 0
    ep_state.ethtool_fn_array[i++] = ethtool_get_settings;
    ep_state.ethtool_fn_array[i++] = ethtool_set_settings;
    ep_state.ethtool_fn_array[i++] = ethtool_get_drvinfo;
    ep_state.ethtool_fn_array[i++] = ethtool_get_regs_len;
    ep_state.ethtool_fn_array[i++] = ethtool_get_regs; // 5
    ep_state.ethtool_fn_array[i++] = ethtool_get_wol;
    ep_state.ethtool_fn_array[i++] = ethtool_set_wol;
    ep_state.ethtool_fn_array[i++] = ethtool_get_msglevel;
    ep_state.ethtool_fn_array[i++] = ethtool_set_msglevel;
    ep_state.ethtool_fn_array[i++] = ethtool_nway_reset; // 10
    ep_state.ethtool_fn_array[i++] = ethtool_get_link;
    ep_state.ethtool_fn_array[i++] = ethtool_get_eeprom_len;
    ep_state.ethtool_fn_array[i++] = ethtool_get_eeprom;
    ep_state.ethtool_fn_array[i++] = ethtool_set_eeprom;
    ep_state.ethtool_fn_array[i++] = ethtool_get_coalesce; // 15
    ep_state.ethtool_fn_array[i++] = ethtool_set_coalesce;
    ep_state.ethtool_fn_array[i++] = ethtool_get_ringparam;
    ep_state.ethtool_fn_array[i++] = ethtool_set_ringparam;
    ep_state.ethtool_fn_array[i++] = ethtool_get_pauseparam;
    ep_state.ethtool_fn_array[i++] = ethtool_set_pauseparam; // 20
    ep_state.ethtool_fn_array[i++] = ethtool_get_rx_csum;
    ep_state.ethtool_fn_array[i++] = ethtool_set_rx_csum;
    ep_state.ethtool_fn_array[i++] = ethtool_get_tx_csum;
    ep_state.ethtool_fn_array[i++] = ethtool_set_tx_csum;
    ep_state.ethtool_fn_array[i++] = ethtool_get_sg; // 25
    ep_state.ethtool_fn_array[i++] = ethtool_set_sg;
    ep_state.ethtool_fn_array[i++] = ethtool_get_tso;
    ep_state.ethtool_fn_array[i++] = ethtool_set_tso;
    ep_state.ethtool_fn_array[i++] = ethtool_self_test;
    ep_state.ethtool_fn_array[i++] = ethtool_get_strings; // 30
    ep_state.ethtool_fn_array[i++] = ethtool_set_phys_id;
    ep_state.ethtool_fn_array[i++] = ethtool_get_ethtool_stats;
    ep_state.ethtool_fn_array[i++] = ethtool_begin;
    ep_state.ethtool_fn_array[i++] = ethtool_complete;
    ep_state.ethtool_fn_array[i++] = ethtool_get_ufo; // 35
    ep_state.ethtool_fn_array[i++] = ethtool_set_ufo;
    ep_state.ethtool_fn_array[i++] = ethtool_get_flags;
    ep_state.ethtool_fn_array[i++] = ethtool_set_flags;
    ep_state.ethtool_fn_array[i++] = ethtool_get_priv_flags;
    ep_state.ethtool_fn_array[i++] = ethtool_set_priv_flags; // 40
    ep_state.ethtool_fn_array[i++] = ethtool_get_sset_count;

    ep_state.ethtool_fn_array[i++] = test_end;
    ep_state.ethtool_fn_array_len = i;

    // Initialize PM function array.
    i = 0;
    ep_state.pci_pm_fn_array[i++] = test_start;
    ep_state.pci_pm_fn_array[i++] = pci_suspend;
    ep_state.pci_pm_fn_array[i++] = pci_resume;
    // TODO More here but don't know invocation details.
    ep_state.pci_pm_fn_array[i++] = test_end;
    ep_state.pci_pm_fn_array_len = i;

    // Initialize I2C PM function array.
    i = 0;
    ep_state.i2c_pm_fn_array[i++] = test_start;
    ep_state.i2c_pm_fn_array[i++] = i2c_suspend;
    ep_state.i2c_pm_fn_array[i++] = i2c_resume;
    ep_state.i2c_pm_fn_array[i++] = test_end;
    ep_state.i2c_pm_fn_array_len = i;

    return 0;
}

void ep_init_net(struct net_device *net_dev) {
    ep_state.net_dev = net_dev;
}

void ep_init_pci_driver(struct pci_driver *pci_drv) {
    ep_state.pci_drv = pci_drv;
}

void ep_init_pci_dev(struct pci_dev *pci_dev) {
    ep_state.pci_dev = pci_dev;
}

void ep_init_i2c_driver(struct i2c_driver *i2c_drv) {
    ep_state.i2c_drv = i2c_drv;
}

void ep_init_i2c_client(struct i2c_client *i2c_client) {
    ep_state.i2c_client = i2c_client;
}

//void ep_symexec_set(int new_value) {
//    uprintk ("%s: now %d\n", __func__, new_value);
//    ep_state.ep_symexec = new_value;
//}

//static int ep_symexec_get (void) {
//    uprintk ("%s called: returning %d\n", __func__, ep_state.ep_symexec);
//    return ep_state.ep_symexec;
//}

#if 0
//
// Test functions called from wrappers_symtesting
//
static void helper_file (struct file *arg) {
    unsigned int new_flags;
    unsigned int new_mode;

    new_flags = 0;
    new_mode = 0;

    s2e_make_symbolic (&new_flags, sizeof(unsigned int), __func__);
    s2e_make_symbolic (&new_mode, sizeof(unsigned int), __func__);

    arg->f_flags = new_flags;
    arg->f_mode = new_mode;
}

static int ep_llseek_fop_MJRcheck (const char *fn,
                                    int prepost,
                                    loff_t retval,
                                    struct file **arg1,
                                    loff_t *arg2,
                                    int *arg3) {
    return 0;
}

static int ep_read_fop_MJRcheck (const char *fn,
                                 int prepost,
                                 ssize_t retval,
                                 struct file **arg1,
                                 char __user **arg2,
                                 size_t *arg3,
                                 loff_t **arg4) {
    return 0;
}

static int ep_write_fop_MJRcheck (const char *fn,
                                  int prepost,
                                  ssize_t retval,
                                  struct file **arg1,
                                  /*const*/ char __user **arg2,
                                  size_t *arg3,
                                  loff_t **arg4) {
//    static char static_write_fop_buf[10];
//    char *buffer;
//    size_t count;

//    if (!ep_symexec_get()) {
//        return;
//    }

/*
  uprintk ("ioctl_fop_MJRcheck called\n");
  buffer = &static_write_fop_buf[0];
  count = 0;
  s2e_make_symbolic (buffer, 10, __func__);
  s2e_make_symbolic (&count, sizeof(size_t), __func__);
  klee_assume (count < 10);
  klee_assume (count >= 1);

  *arg2 = buffer;
  *arg3 = count;
  */
    return 0;
}

static int ep_aio_read_fop_MJRcheck (const char *fn,
                                     int prepost,
                                     ssize_t retval,
                                     struct kiocb **arg1,
                                     /*const*/ struct iovec **arg2,
                                     unsigned long *arg3,
                                     loff_t *arg4) {
    return 0;
}

static int ep_aio_write_fop_MJRcheck (const char *fn,
                                      int prepost,
                                      ssize_t retval,
                                      struct kiocb **arg1,
                                      /*const*/ struct iovec **arg2,
                                      unsigned long *arg3,
                                      loff_t *arg4) {
    return 0;
}

static int ep_readdir_fop_MJRcheck (const char *fn,
                                    int prepost,
                                    int retval,
                                    struct file **arg1,
                                    void **arg2,
                                    filldir_t *arg3) {
    helper_file (*arg1);
    return 0;
}

static int ep_poll_fop_MJRcheck (const char *fn,
                                 int prepost,
                                 unsigned int retval,
                                 struct file **arg1,
                                 struct poll_table_struct **arg2) {
    helper_file (*arg1);
    return 0;
}

static int ep_ioctl_fop_MJRcheck (const char *fn,
                                  int prepost,
                                  int retval,
                                  struct inode **arg1,
                                  struct file **arg2,
                                  unsigned int *arg3,
                                  unsigned long *arg4) {
    helper_file (*arg2);
    return 0;
}

static int ep_unlocked_ioctl_fop_MJRcheck (const char *fn,
                                           int prepost,
                                           long retval,
                                           struct file **arg1,
                                           unsigned int *arg2,
                                           unsigned long *arg3) {
//    if (!ep_symexec_get()) {
//        return;
//    }

    // Use symbolic data.
    uprintk ("%s called\n", __func__);
    *arg2 = 0;
    s2e_make_symbolic (arg2, sizeof(unsigned int), __func__);

    helper_file (*arg1);
    return 0;
}

static int ep_compat_ioctl_fop_MJRcheck (const char *fn,
                                         int prepost,
                                         long retval,
                                         struct file **arg1,
                                         unsigned int *arg2,
                                         unsigned long *arg3) {
    return ep_unlocked_ioctl_fop_MJRcheck(
        fn,
        prepost,
        retval,
        arg1,
        arg2,
        arg3);
}

static int ep_mmap_fop_MJRcheck (const char *fn,
                                 int prepost,
                                 int retval,
                                 struct file **arg1,
                                 struct vm_area_struct **arg2) {
    helper_file (*arg1);
    return 0;
}

static int ep_open_fop_MJRcheck (const char *fn,
                                 int prepost,
                                 int retval,
                                 struct inode **arg1,
                                 struct file **arg2) {
    helper_file (*arg2);
    return 0;
}

static int ep_flush_fop_MJRcheck (const char *fn,
                                  int prepost,
                                  int retval,
                                  struct file **arg1,
                                  fl_owner_t *id) {
    helper_file (*arg1);
    return 0;
}

static int ep_release_fop_MJRcheck (const char *fn,
                                    int prepost,
                                    int retval,
                                    struct inode **arg1,
                                    struct file **arg2) {
    helper_file (*arg2);
    return 0;
}

static int ep_fsync_fop_MJRcheck (const char *fn,
                                  int prepost,
                                  int retval,
                                  struct file **arg1,
                                  struct dentry **arg2,
                                  int *datasync) {
    helper_file (*arg1);
    return 0;
}

static int ep_aio_fsync_fop_MJRcheck (const char *fn,
                                      int prepost,
                                      int retval,
                                      struct kiocb **arg1,
                                      int *datasync) {
    return 0;
}

static int ep_fasync_fop_MJRcheck (const char *fn,
                                   int prepost,
                                   int retval,
                                   int *arg1,
                                   struct file **arg2,
                                   int *arg3) {
    helper_file (*arg2);
    return 0;
}

static int ep_lock_fop_MJRcheck (const char *fn,
                                 int prepost,
                                 int retval,
                                 struct file **arg1,
                                 int *arg2,
                                 struct file_lock **arg3) {
    helper_file (*arg1);
    return 0;
}

static int ep_sendpage_fop_MJRcheck (const char *fn,
                                     int prepost,
                                     ssize_t retval,
                                     struct file **arg1,
                                     struct page **arg2,
                                     int *arg3,
                                     size_t *arg4,
                                     loff_t **arg5,
                                     int *arg6) {
    helper_file (*arg1);
    return 0;
}

static int ep_get_unmapped_area_fop_MJRcheck (const char *fn,
                                              int prepost,
                                              unsigned long retval,
                                              struct file **arg1,
                                              unsigned long *arg2,
                                              unsigned long *arg3,
                                              unsigned long *arg4,
                                              unsigned long *arg5) {
    helper_file (*arg1);
    return 0;
}

static int ep_check_flags_fop_MJRcheck (const char *fn,
                                        int prepost,
                                        int retval,
                                        int *arg1) {
    return 0;
}

static int ep_flock_fop_MJRcheck (const char *fn,
                                  int prepost,
                                  int retval,
                                  struct file **arg1,
                                  int *arg2,
                                  struct file_lock **arg3) {
    helper_file (*arg1);
    return 0;
}

static int ep_splice_write_fop_MJRcheck (const char *fn,
                                         int prepost,
                                         ssize_t retval,
                                         struct pipe_inode_info **arg1,
                                         struct file **arg2,
                                         loff_t **arg3,
                                         size_t *arg4,
                                         unsigned int *arg5) {
    helper_file (*arg2);
    return 0;
}

static int ep_splice_read_fop_MJRcheck (const char *fn,
                                        int prepost,
                                        ssize_t retval,
                                        struct file **arg1,
                                        loff_t **arg2,
                                        struct pipe_inode_info **arg3,
                                        size_t *arg4,
                                        unsigned int *arg5) {
    helper_file (*arg1);
    return 0;
}

static int ep_setlease_fop_MJRcheck (const char *fn,
                                     int prepost,
                                     int retval,
                                     struct file **arg1,
                                     long *arg2,
                                     struct file_lock ***arg3) {
    helper_file (*arg1);
    return 0;
}

#endif


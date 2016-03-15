#include "../test_state.h"
#include "../testing_ep.h"
#include "../mega_header.h"

extern struct pci_driver_state_struct pci_driver_state;
extern struct ndo_driver_state_struct ndo_driver_state;

STATIC int pci_driver_probe_MJRcheck(const char *fn,
                                     int prepost,
                                     int *retval,
                                     struct pci_dev **dev,
                                     const struct pci_device_id **id) {
    check_routine_start();
    if (prepost == 0) {
        tfassert (pci_driver_state.probed == NOT_CALLED);
#if !defined(DISABLE_TG3_MJRCHECKS)
        // Convenient since this detects symbolic hardware issues.
        tfassert ((*dev)->resource[0].start != 0);
#endif
        tfassert ((*dev)->irq != 0);

        push_kernel_state (fn, BLOCKING_YES, USER_NO);
        set_call_state (fn, &pci_driver_state.probed, IN_PROGRESS);

        ep_init_pci_dev(*dev); // Entry point testing
        if (g_sym_pci == 1) {
            //int i;

            s2e_make_symbolic(&(*dev)->device, sizeof (unsigned short), "pci_driver_probe_device");
            s2e_make_symbolic(&(*dev)->vendor, sizeof (unsigned short), "pci_driver_probe_vendor");
            s2e_make_symbolic(&(*dev)->subsystem_device, sizeof (unsigned short), "pci_driver_probe_subsystem_device");
            s2e_make_symbolic(&(*dev)->subsystem_vendor, sizeof (unsigned short), "pci_driver_probe_subsystem_vendor");
            s2e_make_symbolic(&(*dev)->class, sizeof (unsigned int), "pci_driver_probe_class");
            s2e_make_symbolic(&(*dev)->revision, sizeof (u8), "pci_driver_probe_revision");

/*
  Causes state explosion in pci_enable_device.
  for (i = 0; i < DEVICE_COUNT_RESOURCE; i++) {
  char temp[64];
  sprintf(temp, "pci_driver_probe_resource_%d", i);
  s2e_make_symbolic(&(*dev)->resource[i].flags, sizeof(unsigned long), temp);
  }
*/
        }
    } else if (prepost == 1) {
        // I can't see how to get netdevice from the parameters
        // This used to be marshaled but isn't now that we've moved it outside
        // the driver source code. The static analysis won't see it.

        if (*retval == 0) {
            set_call_state (fn, &pci_driver_state.probed, CALLED_OK);
            if (get_driver_class (fn) == DRIVER_NET) {
                tfassert (ndo_driver_state.register_netdev == CALLED_OK);
            }
        } else {
            set_call_state (fn, &pci_driver_state.probed, CALLED_FAILED);
            if (get_driver_class (fn) == DRIVER_NET) {
                tfassert (ndo_driver_state.register_netdev == CALLED_FAILED ||
                          ndo_driver_state.register_netdev == NOT_CALLED);
            }

            // Unconditionally deprioritize failed probe() calls.
            //uprintk ("Failed probe() - unconditionally deprioritizing");
            //s2e_deprioritize(0);
        }
        /*
          tfassert ((*dev)->dev.driver_data != 0);
          net_dev = (*dev)->dev.driver_data;
          tfassert (net_dev->name != 0);
          tfassert (net_dev->base_addr != 0); // A requirement?
          tfassert (net_dev->perm_addr != 0); // A requirement?
          //tfassert (memcmp (net_dev->base_addr, all_zeros, sizeof (unsigned char) * MAX_ADDR_LEN) != 0);
          //tfassert (memcmp (net_dev->perm_addr, all_zeros, sizeof (unsigned char) * MAX_ADDR_LEN) != 0);
          tfassert (net_dev->netdev_ops != 0);
          tfassert (net_dev->ethtool_ops != 0);
          tfassert (net_dev->irq == (*dev)->irq);
          tfassert (net_dev->reg_state != NETREG_UNINITIALIZED);
        */

        pop_kernel_state (fn);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

STATIC int pci_driver_remove_MJRcheck(const char *fn,
                                      int prepost,
                                      struct pci_dev **dev) {
    check_routine_start();
    if (prepost == 0) {
        push_kernel_state (fn, BLOCKING_YES, USER_NO);
    } else if (prepost == 1) {
        pop_kernel_state (fn);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

STATIC int pci_driver_suspend_MJRcheck(const char *fn,
                                       int prepost,
                                       int *retval,
                                       struct pci_dev *dev,
                                       pm_message_t state) {
    check_routine_start();
    check_routine_end();
    return 0;
}

STATIC int pci_driver_suspend_late_MJRcheck(const char *fn,
                                            int prepost,
                                            int *retval,
                                            struct pci_dev *dev,
                                            pm_message_t state) {
    check_routine_start();
    check_routine_end();
    return 0;
}

STATIC int pci_driver_resume_early_MJRcheck(const char *fn,
                                            int prepost,
                                            int *retval,
                                            struct pci_dev **dev) {
    check_routine_start();
    if (prepost == 0) {
        push_kernel_state(fn, BLOCKING_YES, USER_NO);
    } else if (prepost == 1) {
        pop_kernel_state(fn);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

STATIC int pci_driver_resume_MJRcheck(const char *fn,
                                      int prepost,
                                      int *retval,
                                      struct pci_dev **dev) {
    check_routine_start();
    if (prepost == 0) {
        push_kernel_state(fn, BLOCKING_YES, USER_NO);
    } else if (prepost == 1) {
        pop_kernel_state(fn);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

STATIC int pci_driver_shutdown_MJRcheck(const char *fn,
                                        int prepost,
                                        struct pci_dev **dev) {
    check_routine_start();
    if (prepost == 0) {
        push_kernel_state(fn, BLOCKING_YES, USER_NO);
    } else if (prepost == 1) {
        pop_kernel_state(fn);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

EXPORT_SYMBOL(pci_driver_probe_MJRcheck);
EXPORT_SYMBOL(pci_driver_remove_MJRcheck);
EXPORT_SYMBOL(pci_driver_suspend_MJRcheck);
EXPORT_SYMBOL(pci_driver_suspend_late_MJRcheck);
EXPORT_SYMBOL(pci_driver_resume_early_MJRcheck);
EXPORT_SYMBOL(pci_driver_resume_MJRcheck);
EXPORT_SYMBOL(pci_driver_shutdown_MJRcheck);

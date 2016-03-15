#include "../test_state.h"
#include "../mega_header.h"

extern struct pci_driver_state_struct pci_driver_state;
extern struct usb_driver_state_struct usb_driver_state;
extern struct ndo_driver_state_struct ndo_driver_state;
extern struct i2c_driver_state_struct i2c_driver_state;
extern struct platform_driver_state_struct platform_driver_state;
extern struct spi_driver_state_struct spi_driver_state;

//
// 3.1.1 migration - this list of MJRcheck functions is 
// not complete but those present should have correct prototypes
// See netdevice.h for complete list.
//

//
// net_device_ops structure - exported members
//

STATIC int net_device_ops_ndo_init_MJRcheck(const char *fn,
                                            int prepost,
                                            int *retval,
                                            struct net_device **dev) {
    check_routine_start();
    check_routine_end();
    return 0;
}

STATIC int net_device_ops_ndo_uninit_MJRcheck(const char *fn,
                                              int prepost,
                                              struct net_device **dev) {
    check_routine_start();
    check_routine_end();
    return 0;
}

STATIC int net_device_ops_ndo_open_MJRcheck(const char *fn,
                                            int prepost,
                                            int *retval,
                                            struct net_device **dev) {
    check_routine_start();
    if (prepost == 0) {
        tfassert (ndo_driver_state.opened == NOT_CALLED ||
                  ndo_driver_state.opened == CALLED_FAILED);
        tfassert (pci_driver_state.probed == CALLED_OK ||
                  usb_driver_state.probed == CALLED_OK ||
                  i2c_driver_state.probed == CALLED_OK ||
                  platform_driver_state.probed == CALLED_OK ||
                  spi_driver_state.probed == CALLED_OK);
        // tfassert ((*dev)->irq != 0);// Only for PCI -- need to modify condition
        tfassert ((*dev)->name != 0);

#ifndef DISABLE_PEGASUS_MJRCHECKS
        push_kernel_state (fn, BLOCKING_YES, USER_NO);
#else
        push_kernel_state (fn, BLOCKING_UNDEFINED, USER_UNDEFINED);
#endif
        set_call_state (fn, &ndo_driver_state.opened, IN_PROGRESS);
    } else if (prepost == 1) {
#ifndef DISABLE_BE2NET_MJRCHECKS
        tfassert (*retval <= 0 && *retval >= -4095);
#endif
        if (*retval == 0) {
            // netif_start_queue called (or equivalent)
            tfassert ((netdev_get_tx_queue(*dev, 0)->state & __QUEUE_STATE_XOFF) == 0);

            set_call_state (fn, &ndo_driver_state.opened, CALLED_OK);
        } else {
            // Should not have registered an IRQ
            tfassert (is_irq_allocated() == 0);

            // netif_stop_queue called or netif_start_queue not called:
#if !defined(DISABLE_BE2NET_MJRCHECKS) && !defined(DISABLE_E100_MJRCHECKS)
            // Is this check correct?  Is it backwards?  I'M CONFUSED
            tfassert ((netdev_get_tx_queue(*dev, 0)->state & __QUEUE_STATE_XOFF) != 0);
#endif
            set_call_state (fn, &ndo_driver_state.opened, CALLED_FAILED);
            // Unconditionally deprioritize failed open() calls.
            //uprintk ("Failed open() - unconditionally deprioritizing");
            //s2e_deprioritize(0);
        }
        pop_kernel_state (fn);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

STATIC int net_device_ops_ndo_stop_MJRcheck(const char *fn,
                                            int prepost,
                                            int *retval,
                                            struct net_device **dev) {
    check_routine_start();
    if (prepost == 0) {
        tfassert (pci_driver_state.probed == CALLED_OK ||
                  usb_driver_state.probed == CALLED_OK ||
                  i2c_driver_state.probed == CALLED_OK ||
                  platform_driver_state.probed == CALLED_OK ||
                  spi_driver_state.probed == CALLED_OK);
#ifndef DISABLE_PEGASUS_MJRCHECKS
        push_kernel_state(fn, BLOCKING_YES, USER_NO);
#else
        push_kernel_state(fn, BLOCKING_UNDEFINED, USER_NO);
#endif
    } else if (prepost == 1) {
        // netif_stop_queue called:
        //tfassert ((netdev_get_tx_queue(*dev, 0)->state & __QUEUE_STATE_XOFF) != 0);
#ifndef DISABLE_BE2NET_MJRCHECKS
        tfassert (test_bit (__QUEUE_STATE_XOFF, &netdev_get_tx_queue(*dev, 0)->state) != 0);
#endif

        pop_kernel_state (fn);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

STATIC int net_device_ops_ndo_start_xmit_MJRcheck(const char *fn,
                                                  int prepost,
                                                  netdev_tx_t *retval,
                                                  struct sk_buff **skb,
                                                  struct net_device **dev) {
    check_routine_start();
    if (prepost == 0) {
        tfassert ((*skb)->len <= 1536 && (*skb)->len >= 0);
        tfassert (netdev_priv (*dev) != 0);

        // TODO This commented test exposes limitations in the analysis in which
        // the ndo_open call in the driver does not invoke the test framework
        // It only occurs with some drivers, e.g. e1000, not e.g. 8139too
        // tfassert (ndo_driver_state.opened == CALLED_OK); // Seems redundant

        tfassert (pci_driver_state.probed == CALLED_OK ||
                  usb_driver_state.probed == CALLED_OK ||
                  i2c_driver_state.probed == CALLED_OK ||
                  platform_driver_state.probed == CALLED_OK ||
                  spi_driver_state.probed == CALLED_OK);

        ndo_driver_state.trans_start_check = (*dev)->trans_start; // Used to test a patch in 8139too
        push_kernel_state (fn, BLOCKING_NO, USER_NO);

        // Hack to make the following allocate:
        // rather than pass prepost variable, just pass "1" instead,
        // since the allocation already occurred.
        generic_allocator(fn, 1, (unsigned long) (*skb)->head, (unsigned long) -1, ORIGIN_DEV_ALLOC_SKB);
    } else if (prepost == 1) {
        // TODO not precisely sure what the proper behavior of the xmit routine
        // should be w.r.t. freeing or not freeing an SKB.
        int skb_usage;

        if (*retval != NETDEV_TX_OK) {
            // Hack to make the following deallocate:
            // send 0 since we need to remove it from the OT
            generic_free(fn, 0, (unsigned long) (*skb)->head, ORIGIN_DEV_ALLOC_SKB);
        }
        
        skb_usage = atomic_read(&(*skb)->users);
        if (skb_usage > 1) {
            // TODO: this condition may be invalid, as the skb should
            // have been freed?
            // tfassert (atomic_read(&(*skb)->users) == skb_usage - 1);
        } else if (skb_usage == 1) {
            // How do we verify that the skb was freed?
        } else {
            // I don't know.  I though this was bad but maybe not?
            //tfassert (0);
        }

        // No longer require the carrier.  Needed for module unload.
        uprintk ("%s:%d No longer terminating netif_carrier_off paths\n", __func__, __LINE__);
        g_sym_kill_carrier_off = 0;

        // For testing the trans_start collateral evolution.  Unpatched
        // driver does not support this as it does indeed set trans_start as
        // was standard.
        //tfassert (ndo_driver_state.trans_start_check == (*dev)->trans_start);
        pop_kernel_state(fn);
    }
    check_routine_end();
    return 0;
}

STATIC int net_device_ops_ndo_select_queue_MJRcheck(const char *fn,
                                                    int prepost,
                                                    u16 *retval,
                                                    struct net_device **dev,
                                                    struct sk_buff **skb) {
    check_routine_start();
    check_routine_end();
    return 0;
}

STATIC int net_device_ops_ndo_change_rx_flags_MJRcheck(const char *fn,
                                                       int prepost,
                                                       struct net_device **dev,
                                                       int *flags) {
    check_routine_start();
    check_routine_end();
    return 0;
}

STATIC int net_device_ops_ndo_set_rx_mode_MJRcheck(const char *fn,
                                                   int prepost,
                                                   struct net_device **dev) {
    check_routine_start();
    check_routine_end();
    return 0;
}

STATIC int net_device_ops_ndo_set_multicast_list_MJRcheck(const char *fn,
                                                          int prepost,
                                                          struct net_device **dev) {
    check_routine_start();
    check_routine_end();
    return 0;
}

STATIC int net_device_ops_ndo_set_mac_address_MJRcheck(const char *fn,
                                                       int prepost,
                                                       int *retval,
                                                       struct net_device **dev,
                                                       void **addr) {
    check_routine_start();
    check_routine_end();
    return 0;
}

STATIC int net_device_ops_ndo_validate_addr_MJRcheck(const char *fn,
                                                     int prepost,
                                                     int *retval,
                                                     struct net_device **dev) {
    check_routine_start();
    check_routine_end();
    return 0;
}

STATIC int net_device_ops_ndo_do_ioctl_MJRcheck(const char *fn,
                                                int prepost,
                                                int *retval,
                                                struct net_device **dev,
                                                struct ifreq **ifr,
                                                int *cmd) {
    check_routine_start();
    if (prepost == 0) {
    } else if (prepost == 1) {
        ioctl_called_capable(fn, prepost, *retval, *cmd);
    } else {
        tfassert(0);
    }
    check_routine_end();
    return 0;
}

STATIC int net_device_ops_ndo_set_config_MJRcheck(const char *fn,
                                                  int prepost,
                                                  int *retval,
                                                  struct net_device **dev,
                                                  struct ifmap **map) {
    check_routine_start();
    check_routine_end();
    return 0;
}

STATIC int net_device_ops_ndo_change_mtu_MJRcheck(const char *fn,
                                                  int prepost,
                                                  int *retval,
                                                  struct net_device **dev,
                                                  int *new_mtu) {
    check_routine_start();
    check_routine_end();
    return 0;
}

STATIC int net_device_ops_ndo_neigh_setup_MJRcheck(const char *fn,
                                                   int prepost,
                                                   int reval,
                                                   struct net_device **dev,
                                                   struct neigh_parms **np) {
    check_routine_start();
    check_routine_end();
    return 0;
}

STATIC int net_device_ops_ndo_tx_timeout_MJRcheck(const char *fn,
                                                  int prepost,
                                                  struct net_device **dev) {
    check_routine_start();
    check_routine_end();
    return 0;
}

STATIC int net_device_ops_ndo_get_stats64_MJRcheck(const char *fn,
                                                   int prepost,
                                                   struct rtnl_link_stats64 **retval,
                                                   struct net_device **dev,
                                                   struct rtnl_link_stats64 **storage) {
    check_routine_start();
    check_routine_end();
    return 0;
}

STATIC int net_device_ops_ndo_get_stats_MJRcheck(const char *fn,
                                                 int prepost,
                                                 struct net_device_stats **retval,
                                                 struct net_device **dev) {
    check_routine_start();
    check_routine_end();
    return 0;
}

STATIC int net_device_ops_ndo_vlan_rx_add_vid_MJRcheck(const char *fn,
                                                       int prepost,
                                                       struct net_device **dev,
                                                       unsigned short *vid) {
    check_routine_start();
    check_routine_end();
    return 0;
}

STATIC int net_device_ops_ndo_vlan_rx_kill_vid_MJRcheck(const char *fn,
                                                        int prepost,
                                                        struct net_device **dev,
                                                        unsigned short *vid) {
    check_routine_start();
    check_routine_end();
    return 0;
}

STATIC int net_device_ops_ndo_poll_controller_MJRcheck(const char *fn,
                                                       int prepost,
                                                       int *retval,
                                                       struct net_device **dev) {
    check_routine_start();
    if (prepost == 1) {
        if (*retval <= 0) {
            // MJR, the idea here is to force the receive routine to do something interesting.
            s2e_deprioritize(0);
        }
    }
    check_routine_end();
    return 0;
}

EXPORT_SYMBOL(net_device_ops_ndo_change_mtu_MJRcheck);
EXPORT_SYMBOL(net_device_ops_ndo_change_rx_flags_MJRcheck);
EXPORT_SYMBOL(net_device_ops_ndo_do_ioctl_MJRcheck);
EXPORT_SYMBOL(net_device_ops_ndo_get_stats64_MJRcheck);
EXPORT_SYMBOL(net_device_ops_ndo_get_stats_MJRcheck);
EXPORT_SYMBOL(net_device_ops_ndo_init_MJRcheck);
EXPORT_SYMBOL(net_device_ops_ndo_neigh_setup_MJRcheck);
EXPORT_SYMBOL(net_device_ops_ndo_open_MJRcheck);
EXPORT_SYMBOL(net_device_ops_ndo_poll_controller_MJRcheck);
EXPORT_SYMBOL(net_device_ops_ndo_select_queue_MJRcheck);
EXPORT_SYMBOL(net_device_ops_ndo_set_config_MJRcheck);
EXPORT_SYMBOL(net_device_ops_ndo_set_mac_address_MJRcheck);
EXPORT_SYMBOL(net_device_ops_ndo_set_multicast_list_MJRcheck);
EXPORT_SYMBOL(net_device_ops_ndo_set_rx_mode_MJRcheck);
EXPORT_SYMBOL(net_device_ops_ndo_start_xmit_MJRcheck);
EXPORT_SYMBOL(net_device_ops_ndo_stop_MJRcheck);
EXPORT_SYMBOL(net_device_ops_ndo_tx_timeout_MJRcheck);
EXPORT_SYMBOL(net_device_ops_ndo_uninit_MJRcheck);
EXPORT_SYMBOL(net_device_ops_ndo_validate_addr_MJRcheck);
EXPORT_SYMBOL(net_device_ops_ndo_vlan_rx_add_vid_MJRcheck);
EXPORT_SYMBOL(net_device_ops_ndo_vlan_rx_kill_vid_MJRcheck);


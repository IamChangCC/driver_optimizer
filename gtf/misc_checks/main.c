#include "../test_state.h"
#include "../testing_ep.h"

#include "../mega_header.h"

//
// TODO: Minimize the use of this stuff:
// Phase it out.  Replace this design.  Fix it.
//
extern struct pci_driver_state_struct pci_driver_state;
extern struct usb_driver_state_struct usb_driver_state;
extern struct misc_driver_state_struct misc_driver_state;
extern struct proto_ops_state_struct proto_ops_state;
extern struct i2c_driver_state_struct i2c_driver_state;
extern struct platform_driver_state_struct platform_driver_state;
extern struct spi_driver_state_struct spi_driver_state;

extern struct ndo_driver_state_struct ndo_driver_state;
//extern struct sound_driver_state_struct sound_driver_state;

// 2.6.29 antique
// #define ENABLE_SPIN_LOCK

//
// The prototypes for most of these functions are in wrappers_symtesting.h
//

//
// Miscellaneous testing, catch-all testing etc
// Non-static so we can reference it in function_tracker.c
//
int default_MJRcheck(const char *fn,
                     int prepost) {
//    check_routine_start();
//    uprintk ("Warning:  %s called\n", __func__);
//    check_routine_end();
    return 0;
}

STATIC int init_module_MJRcheck(const char *fn,
                                int prepost,
                                int *retval) {
    check_routine_start();
    if (prepost == 0) {
        push_blocking_state (fn, BLOCKING_YES);
        push_user_state (fn, USER_YES);
    } else if (prepost == 1) {
        pop_blocking_state (fn);
        pop_user_state (fn);
#ifndef DISABLE_DOR_MJRCHECKS
        if(*retval == 0) {
            tfassert (pci_driver_state.registered == CALLED_OK ||
                      usb_driver_state.registered == CALLED_OK ||
                      misc_driver_state.registered == CALLED_OK ||
                      proto_ops_state.registered == CALLED_OK ||
                      i2c_driver_state.registered == CALLED_OK ||
                      platform_driver_state.registered == CALLED_OK ||
                      spi_driver_state.registered == CALLED_OK);
        } else {
            tfassert (pci_driver_state.registered != CALLED_OK &&
                      usb_driver_state.registered != CALLED_OK &&
                      misc_driver_state.registered != CALLED_OK &&
                      proto_ops_state.registered != CALLED_OK &&
                      i2c_driver_state.registered != CALLED_OK &&
                      platform_driver_state.registered != CALLED_OK &&
                      spi_driver_state.registered != CALLED_OK);
        }
#endif
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

STATIC int cleanup_module_MJRcheck(const char *fn,
                                   int prepost) {
    check_routine_start();
    if (prepost == 0) {
        push_blocking_state (fn, BLOCKING_YES);
        push_user_state (fn, USER_NO);
    } else if (prepost == 1) {
        pop_blocking_state (fn);
        pop_user_state (fn);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

// ****************************************************************************
//
// Kernel functions that are called from the driver also need to be checked
//
// ****************************************************************************
STATIC int free_netdev_MJRcheck(const char *fn,
                                int prepost,
                                struct net_device **dev) {
    int retval = 0;
    check_routine_start();
    if (prepost == 0) {
        generic_free(fn, prepost, (unsigned long) *dev, ORIGIN_ALLOC_ETHERDEV_MQS);
        if (bad_path) { // Bypass potential kernel bugcheck
            retval = 1;
        }
    }
    check_routine_end();
    return retval;
}

STATIC int kfree_skb_MJRcheck(const char *fn,
                              int prepost,
                              struct sk_buff **skb) {
    check_routine_start();
    if (prepost == 0) {
        generic_free(fn, prepost, (unsigned long) (*skb)->head, ORIGIN_DEV_ALLOC_SKB);
    }
    check_routine_end();
    return 0;
}

STATIC int __napi_schedule_MJRcheck(const char *fn,
                                    int prepost,
                                    struct napi_struct **n) {
    check_routine_start();
    if (prepost == 0) {
        set_call_state(fn, &ndo_driver_state.__napi_schedule, IN_PROGRESS);
    } else if (prepost == 1) {
        set_call_state(fn, &ndo_driver_state.__napi_schedule, CALLED_OK);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

STATIC int __netdev_alloc_skb_MJRcheck(const char *fn,
                                       int prepost,
                                       struct sk_buff **retval,
                                       struct net_device **dev,
                                       unsigned int *length,
                                       gfp_t *gfp_mask) {
    check_routine_start();
    if (prepost == 1) {
        if (*retval != NULL) {
            generic_allocator(fn, prepost, (unsigned long) (*retval)->head, (unsigned long) -1, ORIGIN_DEV_ALLOC_SKB);
        }
    }
    check_routine_end();
    return 0;
}

STATIC int register_netdev_MJRcheck(const char *fn,
                                    int prepost,
                                    int  *retval,
                                    struct net_device **dev) {
    check_routine_start();
    if (prepost == 0) {
        set_call_state (fn, &ndo_driver_state.register_netdev, IN_PROGRESS);
        set_driver_class (fn, DRIVER_NET);
        uprintk ("%s calling concretize/kill (should not kill anything):\n");
#if defined(DISABLE_FORCEDETH_MJRCHECKS) || defined(DISABLE_E100_MJRCHECKS)
        // Weird case
        // This crap is necessary because of a constraint solver limitation
        // Not sure why these drivers choke -- presumably some odd constraint
        // If we just concretize everything the problem is solved.
        // These drivers basically slow to a crawl dealing with the constraint solver
        // unless we do this.
        
        //
        // TODO can we address this problem in a more general way?
        // E.g. if (constraint_solver == getting_out_of_hand) {
        //     kill_stuff_and_simplify ();
        // }
        //
        s2e_concretize_kill (0);
#endif
    } else if (prepost == 1) {
        if (*retval == 0) {
            ep_init_net(*dev); // Entry point testing
            set_call_state (fn, &ndo_driver_state.register_netdev, CALLED_OK);
        } else {
            set_call_state (fn, &ndo_driver_state.register_netdev, CALLED_FAILED);
        }
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

STATIC int schedule_delayed_work_MJRcheck(const char *fn,
                                          int prepost,
                                          int  *retval,
                                          struct delayed_work **work,
                                          unsigned long  *delay) {
    check_routine_start();
    if (prepost == 0) {
        *delay = *delay * g_sym_slow_timers;
    }
    check_routine_end();
    return 0;
}

STATIC int unregister_netdev_MJRcheck(const char *fn,
                                      int prepost,
                                      struct net_device **dev) {
    check_routine_start();
    if (prepost == 0) {
        if (get_driver_bus (fn) & DRIVER_PCI) {
            tfassert (pci_driver_state.enabled == DEVICE_STATE_ENABLED);
        } // TODO incomplete for other buses

        tfassert (ndo_driver_state.register_netdev == CALLED_OK);
        tfassert (ndo_driver_state.unregister_netdev == NOT_CALLED);

        set_call_state (fn, &ndo_driver_state.unregister_netdev, IN_PROGRESS);
    } else if (prepost == 1) {
        set_call_state (fn, &ndo_driver_state.register_netdev, NOT_CALLED);
        set_call_state (fn, &ndo_driver_state.unregister_netdev, NOT_CALLED);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

STATIC int netif_carrier_off_MJRcheck(const char *fn,
                                      int prepost,
                                      struct net_device **dev) {
    check_routine_start();
    if (prepost == 0) {
        // E1000 patch on e1000_main.c titled "e1000: init link state correctly"
        // suggests this is an error:
#if !defined(DISABLE_BE2NET_MJRCHECKS) && \
    !defined(DISABLE_E1000_MJRCHECKS) && \
    !defined(DISABLE_ET131X_MJRCHECKS) && \
    !defined (DISABLE_ORINOCO_MJRCHECKS)
        tfassert (ndo_driver_state.register_netdev == CALLED_OK);
#endif
        if (g_sym_kill_carrier_off) {
            STATIC int called_before = 0;
            if (called_before == 0) {
                uprintk ("%s: Warning:  allowing one call.", __func__);
                called_before = 1;
            } else {
                tfassert_no_detail (0, "Terminating netif_carrier_off path...\n");
            }
        }
    } else if (prepost == 1) {
    } else {
        tfassert(0);
    }
    check_routine_end();
    return 0;
}

STATIC int netif_rx_MJRcheck(const char *fn,
                             int prepost,
                             int *retval,
                             struct sk_buff **skb) {
    check_routine_start();
    if (prepost == 0) {
        set_call_state(fn, &ndo_driver_state.netif_rx, IN_PROGRESS);
    } else if (prepost == 1) {
        set_call_state(fn, &ndo_driver_state.netif_rx, CALLED_OK);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

STATIC int queue_delayed_work_MJRcheck(const char *fn,
                                       int prepost,
                                       int *retval,
                                       struct workqueue_struct **wq,
                                       struct delayed_work **work,
                                       unsigned long *delay) {
    check_routine_start();
    if (prepost == 0) {
        *delay = *delay * g_sym_slow_timers;
    }
    check_routine_end();
    return 0;
}

STATIC int usb_deregister_MJRcheck(const char *fn,
                                   int prepost,
                                   struct usb_driver **arg0) {
    check_routine_start();
    if (prepost == 0) {
        tfassert (get_driver_bus (fn) & DRIVER_USB);
        tfassert (usb_driver_state.registered == CALLED_OK);
        set_call_state (fn, &usb_driver_state.registered, IN_PROGRESS);
    } else if (prepost == 1) {
        set_call_state (fn, &usb_driver_state.registered, NOT_CALLED);
        remove_driver_bus(fn, DRIVER_USB);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

STATIC int usb_register_driver_MJRcheck(const char *fn,
                                        int prepost,
                                        int *retval,
                                        struct usb_driver **arg0,
                                        struct module **arg1,
                                        char /*const*/ **arg2) {
    check_routine_start();
    if (prepost == 0) {
        tfassert (usb_driver_state.registered == NOT_CALLED);
        set_call_state (fn, &usb_driver_state.registered, IN_PROGRESS);
        set_driver_bus (fn, DRIVER_USB);
    } else if (prepost == 1) {
        if (*retval == 0) {
            set_call_state (fn, &usb_driver_state.registered, CALLED_OK);
        } else {
            set_call_state (fn, &usb_driver_state.registered, CALLED_FAILED);
        }
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

STATIC int mod_timer_MJRcheck(const char *fn,
                              int prepost,
                              int *retval,
                              struct timer_list **timer,
                              unsigned long *expires) {
    check_routine_start();
    if (prepost == 0) {
        // This is not quite the same semantics as the others
        // but it gets the job done.
        *expires = *expires + (g_sym_slow_timers - 1) * HZ;
    }
    check_routine_end();
    return 0;
}

STATIC int schedule_timeout_uninterruptible_MJRcheck(const char *fn,
                                                     int prepost,
                                                     long *retval,
                                                     long *timeout) {
    check_routine_start();
    if (prepost == 0) {
        *timeout = *timeout * g_sym_slow_timers;
        s2e_enable_all_apic_interrupts(); // TODO hack -- should not need this
    } else if (prepost == 1) {
        s2e_disable_all_apic_interrupts(); // TODO hack -- should not need this
    } else {
        tfassert(0);
    }
    check_routine_end();
    return 0;
}

STATIC int __alloc_skb_MJRcheck(const char *fn,
                                int prepost,
                                struct sk_buff **retval,
                                unsigned int *size,
                                gfp_t *priority,
                                int *fclone,
                                int *node) {
    check_routine_start();
    if (prepost == 1) {
        generic_allocator(fn, prepost, (unsigned long) (*retval)->head, (unsigned long) size, ORIGIN_DEV_ALLOC_SKB);
    }
    check_routine_end();
    return 0;
}

STATIC int capable_MJRcheck(const char *fn,
                            int prepost,
                            bool *retval,
                            int *cap) {
    check_routine_start();
    call_capable(fn, prepost, *retval, *cap);
    check_routine_end();
    return 0;
}

STATIC int iounmap_MJRcheck(const char *fn,
                            int prepost,
                            void volatile **addr) {
    check_routine_start();
    if (prepost == 0) {
        generic_free(fn, prepost, (unsigned long) (*addr), ORIGIN_IOREMAP);
        if (*addr != NULL) {
            s2e_io_region(__LINE__, SYMDRIVE_IO_UNMAP, (unsigned int) *addr, 0);
        }
    }
    check_routine_end();
    return 0;
}

STATIC int device_register_MJRcheck(const char *fn,
                                    int prepost,
                                    int *retval,
                                    struct device **dev) {
    check_routine_start();
    if (prepost == 0) {
    } else if (prepost == 1) {
        enum symdrive_OBJECT_ORIGIN o;
        if (*retval == 0) {
            o = ORIGIN_DEVICE_REGISTER;
        } else {
            o = ORIGIN_GET_DEVICE;
        }
        generic_allocator(fn, prepost, (unsigned long) (*dev), (unsigned long) -1, o);
    }
    check_routine_end();
    return 0;
}

STATIC int device_unregister_MJRcheck(const char *fn,
                                      int prepost,
                                      struct device **dev) {
    check_routine_start();
#ifndef DISABLE_ENS1371_SND_MJRCHECKS
    if (prepost == 0) {
        generic_free(fn, prepost, (unsigned long) (*dev), ORIGIN_DEVICE_REGISTER | ORIGIN_GET_DEVICE);
    }
#endif
    check_routine_end();
    return 0;
}

STATIC int schedule_timeout_MJRcheck(const char *fn,
                                     int prepost,
                                     long *retval,
                                     long *timeout) {
    check_routine_start();
    if (prepost == 0) {
        *timeout = *timeout * g_sym_slow_timers;
    }
    check_routine_end();
    return 0;
}

STATIC int __request_region_MJRcheck(const char *fn,
                                     int prepost,
                                     struct resource **retval,
                                     struct resource **arg0,
                                     resource_size_t  *start,
                                     resource_size_t  *n,
                                     char /*const*/   **name,
                                     int  *flags) {
    check_routine_start();
    // TODO Apparently need to call release_resource followed by kfree
    // We only check for kfree
    if (prepost == 1) {
        generic_allocator(fn, prepost, (unsigned long) *start, (unsigned long) *n, ORIGIN_KMALLOC);
    }
    check_routine_end();
    return 0;
}

STATIC int __release_region_MJRcheck(const char *fn, 
                                     int prepost,
                                     struct resource **parent,
                                     resource_size_t *start,
                                     resource_size_t *n) {
    check_routine_start();
    if (prepost == 0) {
        generic_free(fn, prepost, (unsigned long) *start, ORIGIN_KMALLOC);
    }
    check_routine_end();
    return 0;
}

//
// Testing functions that we're already wrapping
//
STATIC int usb_alloc_urb_MJRcheck(const char *fn,
                                  int prepost,
                                  struct urb **retval,
                                  int *iso_packets,
                                  gfp_t *mem_flags) {
    check_routine_start();
    if (prepost == 1) {
        generic_allocator(fn, prepost, (unsigned long) *retval, (unsigned long) -1, ORIGIN_ALLOC_URB);
    }
    check_routine_end();
    return 0;
}

STATIC int usb_free_urb_MJRcheck(const char *fn,
                                 int prepost,
                                 struct urb **urb) {
    check_routine_start();
    if (prepost == 0) {
        if(urb == NULL) {
            tfassert_detail (0, "%s requires non-null urb, but you should be taking &thepointer so ...", __func__);
        }
        if (*urb == NULL) {
            check_routine_end();
            return 0;
        }
        generic_free(fn, prepost, (unsigned long) (*urb), ORIGIN_ALLOC_URB);
    } else if (prepost == 1) {
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

STATIC int usb_control_msg_MJRcheck(const char *fn,
                                    int prepost,
                                    struct usb_device **dev,
                                    unsigned int *pipe,
                                    __u8 *request,
                                    __u8 *requesttype,
                                    __u16 *value,
                                    __u16 *index,
                                    void **data,
                                    __u16 *size,
                                    int *timeout) {
    check_routine_start();
    if (prepost == 0) {
        tfassert (kernel_blocking_state () == BLOCKING_YES ||
                  kernel_blocking_state () == BLOCKING_UNDEFINED);
#ifndef DISABLE_PEGASUS_MJRCHECKS
        // Pegasus plays some games by incrementing the allocated buffer by +2
        // in some cases.  We could fix this by improving the OT.  Make it a list
        // of ranges instead of a hash table.
        assert_allocated (fn, prepost, ORIGIN_KMALLOC | ORIGIN_ALLOC_URB | ORIGIN_DEV_ALLOC_SKB,
                          (unsigned long) *data);
#endif
    } else if (prepost == 1) {

    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

STATIC int usb_submit_urb_MJRcheck(const char *fn,
                                   int prepost,
                                   struct urb **urb,
                                   gfp_t *mem_flags) {
    check_routine_start();
    if (prepost == 0) {
        mem_flags_test(fn, GFP_ATOMIC, GFP_KERNEL, *mem_flags);
        assert_allocated (fn, prepost, ORIGIN_KMALLOC | ORIGIN_ALLOC_URB,
                          (unsigned long) (*urb));
#ifndef DISABLE_PEGASUS_MJRCHECKS
        // See usb_control_msg_MJRcheck above
        assert_allocated (fn, prepost, ORIGIN_KMALLOC | ORIGIN_DEV_ALLOC_SKB,
                          (unsigned long) (*urb)->transfer_buffer);
#endif
    } else if (prepost == 1) {

    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

//
// Allocator functions.
//

STATIC int __get_free_pages_MJRcheck(const char *fn,
                                     int prepost,
                                     unsigned long *retval,
                                     gfp_t *gfp_mask,
                                     unsigned int *order) {
    check_routine_start();
    if (prepost == 0) {
    } else if (prepost == 1) {
        generic_allocator(fn, prepost, (unsigned long) *retval, 1 << (*order), ORIGIN_GET_FREE_PAGES);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

STATIC int free_pages_MJRcheck(const char *fn,
                               int prepost,
                               unsigned long *addr,
                               unsigned int *order) {
    check_routine_start();
    if (prepost == 0) {
        generic_free(fn, prepost, (unsigned long) (*addr), ORIGIN_GET_FREE_PAGES);
    } else if (prepost == 1) {
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

STATIC int dma_alloc_coherent_MJRcheck(const char *fn,
                                       int prepost,
                                       void **retval,
                                       struct device **dev,
                                       size_t *size,
                                       dma_addr_t **dma_handle,
                                       gfp_t *flag) {
    check_routine_start();
    if (prepost == 0) {
    } else if (prepost == 1) {
        generic_allocator(fn, prepost, (unsigned long) *retval, *size, ORIGIN_DMA_ALLOC_COHERENT);
        if (*retval != NULL) {
            s2e_make_dma_symbolic ((void *) **dma_handle, *size, "dma_alloc_coherent");
        }
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

STATIC int dma_free_coherent_MJRcheck(const char *fn,
                                      int prepost,
                                      struct device **dev,
                                      size_t *size,
                                      void **vaddr,
                                      dma_addr_t *dma_handle) {
    check_routine_start();
    if (prepost == 0) {
        generic_free(fn, prepost, (unsigned long) (*vaddr), ORIGIN_DMA_ALLOC_COHERENT);
        s2e_free_dma_symbolic((void *) *dma_handle, *size, "dma_alloc_coherent");
        s2e_make_symbolic((void *) *vaddr, *size, "dma_free_coherent");
    } else if (prepost == 1) {
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

STATIC int skb_dma_map_MJRcheck(const char *fn,
                                int prepost,
                                struct device **dev,
                                struct sk_buff **skb,
                                enum dma_data_direction *dir) {
    check_routine_start();
    if (prepost == 1) {
    } else if (prepost == 1) {
        generic_allocator(fn, prepost, (unsigned long) *skb, -1, ORIGIN_SKB_DMA_MAP);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

STATIC int skb_dma_unmap_MJRcheck(const char *fn,
                                  int prepost,
                                  struct device **dev,
                                  struct sk_buff **skb,
                                  enum dma_data_direction *dir) {
    check_routine_start();
    if (prepost == 0) {
        generic_free(fn, prepost, (unsigned long) *skb, ORIGIN_SKB_DMA_MAP);
    } else if (prepost == 1) {
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

STATIC int alloc_etherdev_mqs_MJRcheck(const char *fn,
                                       int prepost,
                                       struct net_device **retval,
                                       int *sizeof_priv,
                                       unsigned int *txqs,
                                       unsigned int *rxqs) {
    check_routine_start();
    if (prepost == 0) {
    } else if (prepost == 1) {
        generic_allocator(fn, prepost, (unsigned long) *retval, (unsigned long) -1, ORIGIN_ALLOC_ETHERDEV_MQS);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

STATIC int alloc_netdev_mqs_MJRcheck(const char *fn,
                                     int prepost,
                                     struct net_device **retval,
                                     int *sizeof_priv,
                                     const char **name,
                                     void (**setup)(struct net_device *),
                                     unsigned int *txqs,
                                     unsigned int *rxqs) {
    check_routine_start();
    if (prepost == 0) {
    } else if (prepost == 1) {
        generic_allocator(fn, prepost, (unsigned long) *retval, (unsigned long) -1, ORIGIN_ALLOC_ETHERDEV_MQS);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

STATIC int __kmalloc_MJRcheck(const char *fn,
                              int prepost,
                              void **retval,
                              size_t *arg0,
                              gfp_t *arg1) {
    check_routine_start();
    if (prepost == 0) {
        mem_flags_test(fn, GFP_ATOMIC, GFP_KERNEL, *arg1);
/*        if (s2e_is_symbolic(__LINE__, *arg0)) {
            // The idea here is to fork once:  one state
            // has a zero, the other a non-zero.  Don't want
            // symbolic sizes in general.
            if (*arg0 == 0) {
                *arg0 = 0;
            } else {
                s2e_concretize(arg0, sizeof(*arg0));
            }
        }
*/
    } else if (prepost == 1) {
        generic_allocator(fn, prepost, (unsigned long) *retval, *arg0, ORIGIN_KMALLOC);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

STATIC int kstrdup_MJRcheck(const char *fn,
                            int prepost,
                            char **retval,
                            char **s,
                            gfp_t *arg1) {
    check_routine_start();
    if (prepost == 0) {
        mem_flags_test(fn, GFP_ATOMIC, GFP_KERNEL, *arg1);
    } else if (prepost == 1) {
        generic_allocator(fn, prepost, (unsigned long) *retval, -1, ORIGIN_KMALLOC);
    }
    check_routine_end();
    return 0;
}

STATIC int kfree_MJRcheck(const char *fn,
                          int prepost,
                          const void **arg0) {
    check_routine_start();
    if(prepost == 0) {
        if(arg0 == NULL) {
            tfassert_detail (0, "%s requires non-null arg0, but you should be taking &thepointer so ...", __func__);
        }
        if (*arg0 == NULL) {
            check_routine_end();
            return 0;
        }
        generic_free(fn, prepost, (unsigned long) (*arg0), ORIGIN_KMALLOC);
    } else if (prepost == 1) {
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

STATIC int vmalloc_MJRcheck(const char *fn,
                            int prepost,
                            void **retval,
                            unsigned long *arg0) {
    check_routine_start();
    if (prepost == 0) {
    } else if (prepost == 1) {
        generic_allocator(fn, prepost, (unsigned long) *retval, *arg0, ORIGIN_VMALLOC);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

STATIC int vfree_MJRcheck(const char *fn,
                          int prepost,
                          /*const*/ void **arg0) {
    check_routine_start();
    if(prepost == 0) {
        if(arg0 == NULL) {
            tfassert_detail (0, "%s requires non-null arg0, but you should be taking &thepointer so ...", __func__);
        }
        if (*arg0 == NULL) {
            check_routine_end();
            return 0;
        }
        generic_free(fn, prepost, (unsigned long) (*arg0), ORIGIN_VMALLOC);
    } else if (prepost == 1) {
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

STATIC int dev_alloc_skb_MJRcheck(const char *fn,
                                  int prepost,
                                  struct sk_buff **retval,
                                  unsigned int *length) {
    check_routine_start();
    if (prepost == 0) {
    } else if (prepost == 1) {
        generic_allocator(fn, prepost, (unsigned long) (*retval)->head, (unsigned long) -1, ORIGIN_DEV_ALLOC_SKB);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

STATIC int dev_kfree_skb_any_MJRcheck(const char *fn,
                                      int prepost,
                                      struct sk_buff **skb) {
    check_routine_start();
    if (prepost == 0) {
        generic_free(fn, prepost, (unsigned long) (*skb)->head, ORIGIN_DEV_ALLOC_SKB);
    } else if (prepost == 1) {
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

STATIC int consume_skb_MJRcheck(const char *fn,
                                int prepost,
                                struct sk_buff **skb) {
    check_routine_start();
    if (prepost == 0) {
        generic_free(fn, prepost, (unsigned long) (*skb)->head, ORIGIN_DEV_ALLOC_SKB);
    } else if (prepost == 1) {
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

STATIC int copy_to_user_MJRcheck(const char *fn,
                                 int prepost,
                                 unsigned long *retval,
                                 void **to,
                                 void const **from,
                                 unsigned long *n) {
    check_routine_start();
    if (prepost == 0) {
        uprintk ("%s: Bypassing copy_to_user...\n");
        *retval = 0;
    } else if (prepost == 1) {
        tfassert(0);
    } else {
        tfassert(0);
    }
    check_routine_end();
    return 1;
}

STATIC int copy_from_user_MJRcheck(const char *fn,
                                   int prepost,
                                   unsigned long *retval,
                                   void **to,
                                   void const **from,
                                   unsigned long *n) {
    check_routine_start();
    // Check that we're not overwriting kernel memory (if possible)
    assert_allocated_weak (fn, prepost, (unsigned long) (*to), *n);
    check_routine_end();
    return 0;
}

#ifdef ENABLE_SPIN_LOCK
STATIC int __spin_lock_init_MJRcheck(const char *fn,
                                     int prepost,
                                     void **lock,
                                     /*const*/ char **name,
                                     void **key) {
    check_routine_start();
    generic_lock_allocator (fn, prepost, *lock, ORIGIN_SPIN_LOCK);
    check_routine_end();
    return 0;
}

STATIC int spin_lock_init_MJRcheck(const char *fn,
                                   int prepost,
                                   void **lock) {
    check_routine_start();
    generic_lock_allocator (fn, prepost, *lock, ORIGIN_SPIN_LOCK);
    check_routine_end();
    return 0;
}
#endif

STATIC int __raw_spin_lock_init_MJRcheck(const char *fn,
                                         int prepost,
                                         raw_spinlock_t **lock,
                                         const char **name,
                                         struct lock_class_key **key) {
    check_routine_start();
    generic_lock_allocator (fn, prepost, *lock, ORIGIN_SPIN_LOCK);
    check_routine_end();
    return 0;
}

#ifdef ENABLE_SPIN_LOCK
STATIC int _spin_lock_MJRcheck(const char *fn,
                               int prepost,
                               void **lock) {
    check_routine_start();
    tfassert_detail((unsigned long) lock > 0x1000, "Incorrect lock provided\n");
    generic_lock_state(fn, prepost, *lock, ORIGIN_SPIN_LOCK, SPIN_LOCK, 1);
    check_routine_end();
    return 0;
}

STATIC int spin_lock_MJRcheck(const char *fn,
                              int prepost,
                              spinlock_t **lock) {
    check_routine_start();
    tfassert_detail((unsigned long) lock > 0x1000, "Incorrect lock provided\n");
    generic_lock_state(fn, prepost, *lock, ORIGIN_SPIN_LOCK, SPIN_LOCK, 1);
    check_routine_end();
    return 0;
}
#endif

STATIC int _raw_spin_lock_MJRcheck(const char *fn,
                                   int prepost,
                                   raw_spinlock_t **lock) {
    check_routine_start();
    tfassert_detail((unsigned long) lock > 0x1000, "Incorrect lock provided\n");
    generic_lock_state(fn, prepost, *lock, ORIGIN_SPIN_LOCK, SPIN_LOCK, 1);
    check_routine_end();
    return 0;
}

#ifdef ENABLE_SPIN_LOCK

STATIC int _spin_lock_bh_MJRcheck(const char *fn,
                                  int prepost,
                                  void **lock) {
    check_routine_start();
    tfassert_detail((unsigned long) lock > 0x1000, "Incorrect lock provided\n");
    generic_lock_state(fn, prepost, *lock, ORIGIN_SPIN_LOCK, SPIN_LOCK_BH, 1);
    check_routine_end();
    return 0;
}

STATIC int spin_lock_bh_MJRcheck(const char *fn,
                                 int prepost,
                                 spinlock_t **lock) {
    check_routine_start();
    tfassert_detail((unsigned long) lock > 0x1000, "Incorrect lock provided\n");
    generic_lock_state(fn, prepost, *lock, ORIGIN_SPIN_LOCK, SPIN_LOCK_BH, 1);
    check_routine_end();
    return 0;
}
#endif

STATIC int _raw_spin_lock_bh_MJRcheck(const char *fn,
                                      int prepost,
                                      raw_spinlock_t **lock) {
    check_routine_start();
    tfassert_detail((unsigned long) lock > 0x1000, "Incorrect lock provided\n");
    generic_lock_state(fn, prepost, *lock, ORIGIN_SPIN_LOCK, SPIN_LOCK_BH, 1);
    check_routine_end();
    return 0;
}

#ifdef ENABLE_SPIN_LOCK

STATIC int _spin_lock_irqsave_MJRcheck(const char *fn,
                                       int prepost,
                                       unsigned long *retval,
                                       void **lock) {
    check_routine_start();
    tfassert_detail((unsigned long) lock > 0x1000, "Incorrect lock provided\n");
    generic_lock_state(fn, prepost, *lock, ORIGIN_SPIN_LOCK, SPIN_LOCK_IRQSAVE, 1);
    check_routine_end();
    return 0;
}

STATIC int spin_lock_irqsave_MJRcheck(const char *fn,
                                      int prepost,
                                      void **lock,
                                      unsigned long *flags) {
    check_routine_start();
    tfassert_detail((unsigned long) lock > 0x1000, "Incorrect lock provided\n");
    generic_lock_state(fn, prepost, *lock, ORIGIN_SPIN_LOCK, SPIN_LOCK_IRQSAVE, 1);
    check_routine_end();
    return 0;
}

#endif

STATIC int _raw_spin_lock_irqsave_MJRcheck(const char *fn,
                                           int prepost,
                                           unsigned long *retval,
                                           raw_spinlock_t **lock) {
    check_routine_start();
    tfassert_detail((unsigned long) lock > 0x1000, "Incorrect lock provided\n");
    generic_lock_state(fn, prepost, *lock, ORIGIN_SPIN_LOCK, SPIN_LOCK_IRQSAVE, 1);
    check_routine_end();
    return 0;
}

#ifdef ENABLE_SPIN_LOCK

STATIC int _spin_lock_irq_MJRcheck(const char *fn,
                                   int prepost,
                                   void **lock) {
    check_routine_start();
    tfassert_detail((unsigned long) lock > 0x1000, "Incorrect lock provided\n");
    generic_lock_state(fn, prepost, *lock, ORIGIN_SPIN_LOCK, SPIN_LOCK_IRQ, 1);
    check_routine_end();
    return 0;
}

STATIC int spin_lock_irq_MJRcheck(const char *fn,
                                  int prepost,
                                  spinlock_t **lock) {
    check_routine_start();
    tfassert_detail((unsigned long) lock > 0x1000, "Incorrect lock provided\n");
    generic_lock_state(fn, prepost, *lock, ORIGIN_SPIN_LOCK, SPIN_LOCK_IRQ, 1);
    check_routine_end();
    return 0;
}

#endif

STATIC int _raw_spin_lock_irq_MJRcheck(const char *fn,
                                       int prepost,
                                       raw_spinlock_t **lock) {
    check_routine_start();
    tfassert_detail((unsigned long) lock > 0x1000, "Incorrect lock provided\n");
    generic_lock_state(fn, prepost, *lock, ORIGIN_SPIN_LOCK, SPIN_LOCK_IRQ, 1);
    check_routine_end();
    return 0;
}

#ifdef ENABLE_SPIN_LOCK

STATIC int _spin_unlock_MJRcheck(const char *fn,
                                 int prepost,
                                 void **lock) {
    check_routine_start();
    tfassert_detail((unsigned long) lock > 0x1000, "Incorrect lock provided\n");
    generic_lock_state(fn, prepost, *lock, ORIGIN_SPIN_LOCK, SPIN_LOCK, -1);
    check_routine_end();
    return 0;
}

STATIC int spin_unlock_MJRcheck(const char *fn,
                                int prepost,
                                spinlock_t **lock) {
    check_routine_start();
    tfassert_detail((unsigned long) lock > 0x1000, "Incorrect lock provided\n");
    generic_lock_state(fn, prepost, *lock, ORIGIN_SPIN_LOCK, SPIN_LOCK, -1);
    check_routine_end();
    return 0;
}

#endif

STATIC int _raw_spin_unlock_MJRcheck(const char *fn,
                                     int prepost,
                                     raw_spinlock_t **lock) {
    check_routine_start();
    tfassert_detail((unsigned long) lock > 0x1000, "Incorrect lock provided\n");
    generic_lock_state(fn, prepost, *lock, ORIGIN_SPIN_LOCK, SPIN_LOCK, -1);
    check_routine_end();
    return 0;
}

#ifdef ENABLE_SPIN_LOCK

STATIC int _spin_unlock_bh_MJRcheck(const char *fn,
                                    int prepost,
                                    spinlock_t **lock) {
    check_routine_start();
    tfassert_detail((unsigned long) lock > 0x1000, "Incorrect lock provided\n");
    generic_lock_state(fn, prepost, *lock, ORIGIN_SPIN_LOCK, SPIN_LOCK_BH, -1);
    check_routine_end();
    return 0;
}

STATIC int spin_unlock_bh_MJRcheck(const char *fn,
                                   int prepost,
                                   spinlock_t **lock) {
    check_routine_start();
    tfassert_detail((unsigned long) lock > 0x1000, "Incorrect lock provided\n");
    generic_lock_state(fn, prepost, *lock, ORIGIN_SPIN_LOCK, SPIN_LOCK_BH, -1);
    check_routine_end();
    return 0;
}

#endif

STATIC int _raw_spin_unlock_bh_MJRcheck(const char *fn,
                                        int prepost,
                                        raw_spinlock_t **lock) {
    check_routine_start();
    tfassert_detail((unsigned long) lock > 0x1000, "Incorrect lock provided\n");
    generic_lock_state(fn, prepost, *lock, ORIGIN_SPIN_LOCK, SPIN_LOCK_BH, -1);
    check_routine_end();
    return 0;
}

#ifdef ENABLE_SPIN_LOCK

STATIC int _spin_unlock_irqrestore_MJRcheck(const char *fn,
                                            int prepost,
                                            spinlock_t **lock,
                                            unsigned long *flags) {
    check_routine_start();
//#ifndef DISABLE_8139CP_MJRCHECKS
    tfassert_detail((unsigned long) lock > 0x1000, "Incorrect lock provided\n");
    generic_lock_state(fn, prepost, *lock, ORIGIN_SPIN_LOCK, SPIN_LOCK_IRQSAVE, -1);
//#endif
    check_routine_end();
    return 0;
}

STATIC int spin_unlock_irqrestore_MJRcheck(const char *fn,
                                           int prepost,
                                           spinlock_t **lock,
                                           unsigned long *flags) {
    check_routine_start();
    tfassert_detail((unsigned long) lock > 0x1000, "Incorrect lock provided\n");
    generic_lock_state(fn, prepost, *lock, ORIGIN_SPIN_LOCK, SPIN_LOCK_IRQSAVE, -1);
    check_routine_end();
    return 0;
}

#endif

STATIC int _raw_spin_unlock_irqrestore_MJRcheck(const char *fn,
                                           int prepost,
                                           raw_spinlock_t **lock,
                                           unsigned long *flags) {
    check_routine_start();
    tfassert_detail((unsigned long) lock > 0x1000, "Incorrect lock provided\n");
    generic_lock_state(fn, prepost, *lock, ORIGIN_SPIN_LOCK, SPIN_LOCK_IRQSAVE, -1);
    check_routine_end();
    return 0;
}

#ifdef ENABLE_SPIN_LOCK

STATIC int _spin_unlock_irq_MJRcheck(const char *fn,
                                     int prepost,
                                     spinlock_t **lock) {
    check_routine_start();
    tfassert_detail((unsigned long) lock > 0x1000, "Incorrect lock provided\n");
    generic_lock_state(fn, prepost, *lock, ORIGIN_SPIN_LOCK, SPIN_LOCK_IRQ, -1);
    check_routine_end();
    return 0;
}

STATIC int spin_unlock_irq_MJRcheck(const char *fn,
                                    int prepost,
                                    spinlock_t **lock) {
    check_routine_start();
    tfassert_detail((unsigned long) lock > 0x1000, "Incorrect lock provided\n");
    generic_lock_state(fn, prepost, *lock, ORIGIN_SPIN_LOCK, SPIN_LOCK_IRQ, -1);
    check_routine_end();
    return 0;
}

#endif

STATIC int _raw_spin_unlock_irq_MJRcheck(const char *fn,
                                         int prepost,
                                         raw_spinlock_t **lock) {
    check_routine_start();
    tfassert_detail((unsigned long) lock > 0x1000, "Incorrect lock provided\n");
    generic_lock_state(fn, prepost, *lock, ORIGIN_SPIN_LOCK, SPIN_LOCK_IRQ, -1);
    check_routine_end();
    return 0;
}

#ifdef ENABLE_SPIN_LOCK

STATIC int _spin_trylock_MJRcheck(const char *fn,
                                  int prepost,
                                  int *retval,
                                  void **lock) {
    check_routine_start();
    // TODO: hard to match this up with anything.
    // Treat it as a match for any spin_unlock function.
    tfassert_detail((unsigned long) lock > 0x1000, "Incorrect lock provided\n");
    if (prepost == 1 && *retval) {
        // Only do something if retval is successful.
        generic_lock_state(fn, prepost, *lock, ORIGIN_SPIN_LOCK, GENERIC_LOCK, 1);
    }
    check_routine_end();
    return 0;
}

#endif

STATIC int _raw_spin_trylock_MJRcheck(const char *fn,
                                      int prepost,
                                      int *retval,
                                      raw_spinlock_t **lock) {
    check_routine_start();
    // TODO: hard to match this up with anything.
    // Treat it as a match for any spin_unlock function.
    tfassert_detail((unsigned long) lock > 0x1000, "Incorrect lock provided\n");
    if (prepost == 1 && *retval) {
        // Only do something if retval is successful.
        generic_lock_state(fn, prepost, *lock, ORIGIN_SPIN_LOCK, GENERIC_LOCK, 1);
    }
    check_routine_end();
    return 0;
}


STATIC int __mutex_init_MJRcheck(const char *fn,
                                 int prepost,
                                 struct mutex **lock,
                                 const char **name,
                                 struct lock_class_key **key) {
    check_routine_start();
    generic_lock_allocator (fn, prepost, *lock, ORIGIN_MUTEX);
    check_routine_end();
    return 0;
}

STATIC int mutex_init_MJRcheck(const char *fn,
                               int prepost,
                               struct mutex **lock) {
    check_routine_start();
    generic_lock_allocator (fn, prepost, *lock, ORIGIN_MUTEX);
    check_routine_end();
    return 0;
}

STATIC int mutex_lock_nested_MJRcheck(const char *fn,
                                      int prepost,
                                      struct mutex **lock,
                                      unsigned int *subclass) {
    check_routine_start();
    tfassert_detail((unsigned long) lock > 0x1000, "Incorrect lock provided\n");
    generic_lock_state(fn, prepost, *lock, ORIGIN_MUTEX, MUTEX, 1);
    check_routine_end();
    return 0;
}

STATIC int mutex_lock_interruptible_nested_MJRcheck(const char *fn,
                                                    int prepost,
                                                    int *retval,
                                                    struct mutex **lock,
                                                    unsigned int *subclass) {
    check_routine_start();
    tfassert_detail((unsigned long) lock > 0x1000, "Incorrect lock provided\n");
    generic_lock_state(fn, prepost, *lock, ORIGIN_MUTEX, MUTEX, 1);
    check_routine_end();
    return 0;
}

STATIC int mutex_lock_killable_nested_MJRcheck(const char *fn,
                                               int prepost,
                                               int *retval,
                                               struct mutex **lock,
                                               unsigned int *subclass) {
    check_routine_start();
    tfassert_detail((unsigned long) lock > 0x1000, "Incorrect lock provided\n");
    generic_lock_state(fn, prepost, *lock, ORIGIN_MUTEX, MUTEX, 1);
    check_routine_end();
    return 0;
}

STATIC int mutex_unlock_MJRcheck(const char *fn,
                                 int prepost,
                                 struct mutex **lock) {
    check_routine_start();
    tfassert_detail((unsigned long) lock > 0x1000, "Incorrect lock provided\n");
    generic_lock_state(fn, prepost, *lock, ORIGIN_MUTEX, MUTEX, -1);
    check_routine_end();
    return 0;
}

STATIC int __init_rwsem_MJRcheck(const char *fn,
                                 int prepost,
                                 void **sem,
                                 /*const*/ char **name,
                                 struct lock_class_key **key) {
    check_routine_start();
    generic_lock_allocator (fn, prepost, *sem, ORIGIN_RWSEM);
    check_routine_end();
    return 0;
}

STATIC int up_write_MJRcheck(const char *fn,
                             int prepost,
                             void **sem) {
    check_routine_start();
    check_routine_end();
    return 0;
}

STATIC int up_read_MJRcheck(const char *fn,
                            int prepost,
                            void **sem) {
    check_routine_start();
    check_routine_end();
    return 0;
}

STATIC int down_write_MJRcheck(const char *fn,
                               int prepost,
                               void **sem) {
    check_routine_start();
    check_routine_end();
    return 0;
}

STATIC int down_read_MJRcheck(const char *fn,
                              int prepost,
                              void **sem) {
    check_routine_start();
    check_routine_end();
    return 0;
}

STATIC int __rwlock_init_MJRcheck(const char *fn,
                                  int prepost,
                                  void **lock,
                                  /*const*/ char **name,
                                  struct lock_class_key **key) {
    check_routine_start();
    generic_lock_allocator (fn, prepost, *lock, ORIGIN_RWLOCK);
    check_routine_end();
    return 0;
}

STATIC int _write_lock_irq_MJRcheck(const char *fn,
                                    int prepost,
                                    void **lock) {
    check_routine_start();
    tfassert_detail((unsigned long) lock > 0x1000, "Incorrect lock provided\n");
    generic_lock_state(fn, prepost, *lock, ORIGIN_RWLOCK, RWLOCK, 1);
    check_routine_end();
    return 0;
}

STATIC int _write_unlock_irq_MJRcheck(const char *fn,
                                      int prepost,
                                      void **lock) {
    check_routine_start();
    tfassert_detail((unsigned long) lock > 0x1000, "Incorrect lock provided\n");
    generic_lock_state(fn, prepost, *lock, ORIGIN_RWLOCK, RWLOCK, -1);
    check_routine_end();
    return 0;
}

STATIC int _read_lock_irq_MJRcheck(const char *fn,
                                   int prepost,
                                   void **lock) {
    check_routine_start();
    tfassert_detail((unsigned long) lock > 0x1000, "Incorrect lock provided\n");
    generic_lock_state(fn, prepost, *lock, ORIGIN_RWLOCK, RWLOCK, 1);
    check_routine_end();
    return 0;
}

STATIC int _read_unlock_irq_MJRcheck(const char *fn,
                                     int prepost,
                                     void **lock) {
    check_routine_start();
    tfassert_detail((unsigned long) lock > 0x1000, "Incorrect lock provided\n");
    generic_lock_state(fn, prepost, *lock, ORIGIN_RWLOCK, RWLOCK, -1);
    check_routine_end();
    return 0;
}

STATIC int _read_lock_MJRcheck(const char *fn,
                               int prepost,
                               void **lock) {
    check_routine_start();
    tfassert_detail((unsigned long) lock > 0x1000, "Incorrect lock provided\n");
    generic_lock_state(fn, prepost, *lock, ORIGIN_RWLOCK, RWLOCK, 1);
    check_routine_end();
    return 0;
}

STATIC int _read_unlock_MJRcheck(const char *fn,
                                 int prepost,
                                 void **lock) {
    check_routine_start();
    tfassert_detail((unsigned long) lock > 0x1000, "Incorrect lock provided\n");
    generic_lock_state(fn, prepost, *lock, ORIGIN_RWLOCK, RWLOCK, -1);
    check_routine_end();
    return 0;
}

STATIC int _write_lock_irqsave_MJRcheck(const char *fn,
                                        int prepost,
                                        unsigned long *retval,
                                        void **lock) {
    check_routine_start();
    tfassert_detail((unsigned long) lock > 0x1000, "Incorrect lock provided\n");
    generic_lock_state(fn, prepost, *lock, ORIGIN_RWLOCK, RWLOCK, 1);
    check_routine_end();
    return 0;
}

STATIC int _write_unlock_irqrestore_MJRcheck(const char *fn,
                                             int prepost,
                                             void **lock,
                                             unsigned long *flags) {
    check_routine_start();
    tfassert_detail((unsigned long) lock > 0x1000, "Incorrect lock provided\n");
    generic_lock_state(fn, prepost, *lock, ORIGIN_RWLOCK, RWLOCK, -1);
    check_routine_end();
    return 0;
}

STATIC int _read_lock_irqsave_MJRcheck(const char *fn,
                                       int prepost,
                                       unsigned long *retval,
                                       void **lock) {
    check_routine_start();
    tfassert_detail((unsigned long) lock > 0x1000, "Incorrect lock provided\n");
    generic_lock_state(fn, prepost, *lock, ORIGIN_RWLOCK, RWLOCK, 1);
    check_routine_end();
    return 0;
}

STATIC int _read_unlock_irqrestore_MJRcheck(const char *fn,
                                            int prepost,
                                            void **lock,
                                            unsigned long *flags) {
    check_routine_start();
    tfassert_detail((unsigned long) lock > 0x1000, "Incorrect lock provided\n");
    generic_lock_state(fn, prepost, *lock, ORIGIN_RWLOCK, RWLOCK, -1);
    check_routine_end();
    return 0;
}

STATIC int misc_register_MJRcheck(const char *fn,
                                  int prepost,
                                  int *retval,
                                  struct miscdevice **misc) {
    check_routine_start();
#if !defined(DISABLE_ANDROID_AKM8975_MJRCHECKS)
    // TODO - this is an incomplete implementation that relies on an incorrect assumption
    // What if a driver calls misc_register twice?  Same with other driver types.  Oops.
    if (prepost == 0) {
        tfassert (misc_driver_state.registered == NOT_CALLED);
        set_call_state (fn, &misc_driver_state.registered, IN_PROGRESS);
        set_driver_bus (fn, DRIVER_MISC);
    } else if (prepost == 1) {
        if (*retval == 0) {
            set_call_state (fn, &misc_driver_state.registered, CALLED_OK);
        } else {
            set_call_state (fn, &misc_driver_state.registered, CALLED_FAILED);
        }
    } else {
        tfassert (0);
    }
#endif
    check_routine_end();
    return 0;
}

STATIC int misc_deregister_MJRcheck(const char *fn,
                                    int prepost,
                                    int *retval,
                                    struct miscdevice **misc) {
    check_routine_start();
#if !defined(DISABLE_ANDROID_AKM8975_MJRCHECKS)
    // TODO
    if (prepost == 0) {
        tfassert (get_driver_bus (fn) & DRIVER_MISC);
        tfassert (misc_driver_state.registered == CALLED_OK);
        set_call_state (fn, &misc_driver_state.registered, IN_PROGRESS);
    } else if (prepost == 1) {
        if (*retval == 0) {
            set_call_state (fn, &misc_driver_state.registered, NOT_CALLED);
            remove_driver_bus(fn, DRIVER_MISC);
        }
    } else {
        tfassert (0);
    }
#endif
    check_routine_end();
    return 0;
}

STATIC int ioremap_nocache_MJRcheck(char const *fn,
                                    int prepost,
                                    void **retval,
                                    resource_size_t *offset,
                                    unsigned long *size) {
    check_routine_start();
    if (prepost == 1) {
        generic_allocator(fn, prepost, (unsigned long) *retval, (unsigned long) *size, ORIGIN_IOREMAP);
        s2e_io_region(__LINE__, SYMDRIVE_IO_MAP, (unsigned int) *retval, *size);
    }
    check_routine_end();
    return 0;
}

STATIC int dma_alloc_from_coherent_MJRcheck(const char *fn,
                                            int prepost,
                                            int *retval,
                                            struct device **dev,
                                            ssize_t *size,
                                            dma_addr_t **dma_handle,
                                            void ***ret) {
    check_routine_start();
    if (prepost == 1) {
        //generic_allocator(fn, prepost, (unsigned long) **ret, *size, ORIGIN_DMA_ALLOC_COHERENT);
    }
    check_routine_end();
    return 0;
}

STATIC int dma_release_from_coherent_MJRcheck(const char *fn,
                                              int prepost,
                                              int *retval,
                                              struct device **dev,
                                              int *order,
                                              void **vaddr) {
    check_routine_start();
    if (prepost == 0) {
        //generic_free(fn, prepost, (unsigned long) (*vaddr), ORIGIN_DMA_ALLOC_COHERENT);
    }
    check_routine_end();
    return 0;
}

STATIC int netif_stop_queue_MJRcheck(const char *fn,
                                     int prepost,
                                     struct net_device **dev) {
    check_routine_start();
    if (prepost == 0) {
        // It's required that netif_stop_queue_MJRcheck only be called
        // after register_netdev returns successfully.
#if !defined(DISABLE_BE2NET_MJRCHECKS) && !defined(DISABLE_E1000_MJRCHECKS)
        tfassert (ndo_driver_state.register_netdev == CALLED_OK);
#endif
    } else if (prepost == 1) {
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

STATIC int proto_register_MJRcheck(const char *fn,
                                   int prepost,
                                   int *retval,
                                   struct proto **prot,
                                   int *alloc_slab) {
    check_routine_start();
    if (prepost == 0) {
        tfassert (proto_ops_state.registered == NOT_CALLED);
        set_call_state (fn, &proto_ops_state.registered, IN_PROGRESS);
        set_driver_bus (fn, DRIVER_PROTO_OPS);
    } else if (prepost == 1) {
        if (*retval == 0) {
            set_call_state (fn, &proto_ops_state.registered, CALLED_OK);
        } else {
            set_call_state (fn, &proto_ops_state.registered, CALLED_FAILED);
        }
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

STATIC int proto_unregister_MJRcheck(const char *fn,
                                     int prepost,
                                     struct proto **prot) {
    check_routine_start();
    if (prepost == 0) {
        tfassert (proto_ops_state.registered == CALLED_OK);
        set_call_state (fn, &proto_ops_state.unregistered, IN_PROGRESS);
    } else if (prepost == 1) {
        set_call_state (fn, &proto_ops_state.registered, NOT_CALLED);
        set_call_state (fn, &proto_ops_state.unregistered, NOT_CALLED);
        remove_driver_bus(fn, DRIVER_PROTO_OPS);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

///////////////////////////////////////////////////////////////////////////////

//
// smc91x
//
int probe_irq_mask_MJRcheck(const char *fn,
                            int prepost,
                            unsigned int *retval,
                            unsigned long *val) {
    check_routine_start();
    if (prepost == 0) {
        *retval = 0;
    } else if (prepost == 1) {
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 1;
}

int probe_irq_off_MJRcheck(const char *fn,
                           int prepost,
                           int *retval,
                           unsigned long *val) {
    check_routine_start();
    if (prepost == 0) {
        //s2e_make_symbolic(retval, sizeof (int), "probe_irq_off");
        tfassert_detail (0, "You probably should look at this");
    } else if (prepost == 1) {
        tfassert (0);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 1;
}

//////////////////////////////////////////////////////////////////////////////
// Orinoco

int request_firmware_MJRcheck(const char *fn,
                              int prepost,
                              int *retval,
                              const struct firmware ***fw,
                              const char **name,
                              struct device **device) {
    check_routine_start();
    if (prepost == 0) {
        s2e_enable_all_apic_interrupts(); // TODO hack -- should not need this
    } else if (prepost == 1) {
        s2e_disable_all_apic_interrupts(); // TODO hack -- should not need this
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

int release_firmware_MJRcheck(const char *fn,
                              int prepost,
                              const struct firmware **fw) {
    check_routine_start();
    if (prepost == 0) {
    } else if (prepost == 1) {
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

///////////////////////////////////////////////////////////
// New checker for functions passed directly as parameters

int netif_napi_add_poll_MJRcheck(const char *fn,
                                 int prepost,
                                 int *retval,
                                 struct napi_struct **napi,
                                 int *budget) {
    check_routine_start();
    if (prepost == 0) {
        push_blocking_state(fn, BLOCKING_NO);
    } else if (prepost == 1) {
        pop_blocking_state(fn);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

int napi_struct_poll_MJRcheck(const char *fn,
                              int prepost,
                              int *retval,
                              struct napi_struct **napi,
                              int *budget) {
    return netif_napi_add_poll_MJRcheck(fn, prepost, retval, napi, budget);
}

int add_timer_MJRcheck(const char *fn,
                       int prepost,
                       struct timer_list **timer) {
    check_routine_start();
    if (prepost == 0) {
        (*timer)->expires *= g_sym_slow_timers;
    } else if (prepost == 1) {
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

EXPORT_SYMBOL(add_timer_MJRcheck);
EXPORT_SYMBOL(alloc_etherdev_mqs_MJRcheck);
EXPORT_SYMBOL(alloc_netdev_mqs_MJRcheck);
EXPORT_SYMBOL(__alloc_skb_MJRcheck);
EXPORT_SYMBOL(capable_MJRcheck);
EXPORT_SYMBOL(cleanup_module_MJRcheck);
EXPORT_SYMBOL(consume_skb_MJRcheck);
EXPORT_SYMBOL(copy_from_user_MJRcheck);
EXPORT_SYMBOL(copy_to_user_MJRcheck);
EXPORT_SYMBOL(default_MJRcheck);
EXPORT_SYMBOL(dev_alloc_skb_MJRcheck);
EXPORT_SYMBOL(device_register_MJRcheck);
EXPORT_SYMBOL(device_unregister_MJRcheck);
EXPORT_SYMBOL(dev_kfree_skb_any_MJRcheck);
EXPORT_SYMBOL(dma_alloc_coherent_MJRcheck);
EXPORT_SYMBOL(dma_alloc_from_coherent_MJRcheck);
EXPORT_SYMBOL(dma_free_coherent_MJRcheck);
EXPORT_SYMBOL(dma_release_from_coherent_MJRcheck);
EXPORT_SYMBOL(down_read_MJRcheck);
EXPORT_SYMBOL(down_write_MJRcheck);
EXPORT_SYMBOL(free_netdev_MJRcheck);
EXPORT_SYMBOL(free_pages_MJRcheck);
EXPORT_SYMBOL(__get_free_pages_MJRcheck);
EXPORT_SYMBOL(init_module_MJRcheck);
EXPORT_SYMBOL(__init_rwsem_MJRcheck);
EXPORT_SYMBOL(ioremap_nocache_MJRcheck);
EXPORT_SYMBOL(iounmap_MJRcheck);
EXPORT_SYMBOL(kfree_MJRcheck);
EXPORT_SYMBOL(kfree_skb_MJRcheck);
EXPORT_SYMBOL(__kmalloc_MJRcheck);
EXPORT_SYMBOL(kstrdup_MJRcheck);
EXPORT_SYMBOL(misc_deregister_MJRcheck);
EXPORT_SYMBOL(misc_register_MJRcheck);
EXPORT_SYMBOL(mod_timer_MJRcheck);
EXPORT_SYMBOL(__mutex_init_MJRcheck);
EXPORT_SYMBOL(mutex_init_MJRcheck);
EXPORT_SYMBOL(mutex_lock_nested_MJRcheck);
EXPORT_SYMBOL(mutex_lock_interruptible_nested_MJRcheck);
EXPORT_SYMBOL(mutex_lock_killable_nested_MJRcheck);
EXPORT_SYMBOL(mutex_unlock_MJRcheck);
EXPORT_SYMBOL(__napi_schedule_MJRcheck);
EXPORT_SYMBOL(__netdev_alloc_skb_MJRcheck);
EXPORT_SYMBOL(napi_struct_poll_MJRcheck);
EXPORT_SYMBOL(netif_carrier_off_MJRcheck);
EXPORT_SYMBOL(netif_napi_add_poll_MJRcheck);
EXPORT_SYMBOL(netif_rx_MJRcheck);
EXPORT_SYMBOL(netif_stop_queue_MJRcheck);
EXPORT_SYMBOL(probe_irq_mask_MJRcheck);
EXPORT_SYMBOL(probe_irq_off_MJRcheck);
EXPORT_SYMBOL(proto_register_MJRcheck);
EXPORT_SYMBOL(proto_unregister_MJRcheck);
EXPORT_SYMBOL(queue_delayed_work_MJRcheck);
EXPORT_SYMBOL(_read_lock_MJRcheck);
EXPORT_SYMBOL(_read_lock_irq_MJRcheck);
EXPORT_SYMBOL(_read_lock_irqsave_MJRcheck);
EXPORT_SYMBOL(_read_unlock_MJRcheck);
EXPORT_SYMBOL(_read_unlock_irq_MJRcheck);
EXPORT_SYMBOL(_read_unlock_irqrestore_MJRcheck);
EXPORT_SYMBOL(register_netdev_MJRcheck);
EXPORT_SYMBOL(__request_region_MJRcheck);
EXPORT_SYMBOL(request_firmware_MJRcheck);
EXPORT_SYMBOL(__release_region_MJRcheck);
EXPORT_SYMBOL(release_firmware_MJRcheck);
EXPORT_SYMBOL(__rwlock_init_MJRcheck);
EXPORT_SYMBOL(schedule_delayed_work_MJRcheck);
EXPORT_SYMBOL(schedule_timeout_MJRcheck);
EXPORT_SYMBOL(schedule_timeout_uninterruptible_MJRcheck);
EXPORT_SYMBOL(skb_dma_map_MJRcheck);
EXPORT_SYMBOL(skb_dma_unmap_MJRcheck);
//EXPORT_SYMBOL(_spin_lock_bh_MJRcheck);
//EXPORT_SYMBOL(spin_lock_bh_MJRcheck);
EXPORT_SYMBOL(_raw_spin_lock_bh_MJRcheck);
//EXPORT_SYMBOL(_spin_lock_MJRcheck);
//EXPORT_SYMBOL(spin_lock_MJRcheck);
EXPORT_SYMBOL(_raw_spin_lock_MJRcheck);
//EXPORT_SYMBOL(__spin_lock_init_MJRcheck);
//EXPORT_SYMBOL(spin_lock_init_MJRcheck);
EXPORT_SYMBOL(__raw_spin_lock_init_MJRcheck);
//EXPORT_SYMBOL(_spin_lock_irq_MJRcheck);
//EXPORT_SYMBOL(spin_lock_irq_MJRcheck);
EXPORT_SYMBOL(_raw_spin_lock_irq_MJRcheck);
//EXPORT_SYMBOL(_spin_lock_irqsave_MJRcheck);
//EXPORT_SYMBOL(spin_lock_irqsave_MJRcheck);
EXPORT_SYMBOL(_raw_spin_lock_irqsave_MJRcheck);
//EXPORT_SYMBOL(_spin_trylock_MJRcheck);
EXPORT_SYMBOL(_raw_spin_trylock_MJRcheck);
//EXPORT_SYMBOL(_spin_unlock_bh_MJRcheck);
//EXPORT_SYMBOL(spin_unlock_bh_MJRcheck);
EXPORT_SYMBOL(_raw_spin_unlock_bh_MJRcheck);
//EXPORT_SYMBOL(_spin_unlock_MJRcheck);
//EXPORT_SYMBOL(spin_unlock_MJRcheck);
EXPORT_SYMBOL(_raw_spin_unlock_MJRcheck);
//EXPORT_SYMBOL(_spin_unlock_irq_MJRcheck);
//EXPORT_SYMBOL(spin_unlock_irq_MJRcheck);
EXPORT_SYMBOL(_raw_spin_unlock_irq_MJRcheck);
//EXPORT_SYMBOL(_spin_unlock_irqrestore_MJRcheck);
//EXPORT_SYMBOL(spin_unlock_irqrestore_MJRcheck);
EXPORT_SYMBOL(_raw_spin_unlock_irqrestore_MJRcheck);
EXPORT_SYMBOL(unregister_netdev_MJRcheck);
EXPORT_SYMBOL(up_read_MJRcheck);
EXPORT_SYMBOL(up_write_MJRcheck);
EXPORT_SYMBOL(usb_alloc_urb_MJRcheck);
EXPORT_SYMBOL(usb_control_msg_MJRcheck);
EXPORT_SYMBOL(usb_deregister_MJRcheck);
EXPORT_SYMBOL(usb_free_urb_MJRcheck);
EXPORT_SYMBOL(usb_register_driver_MJRcheck);
EXPORT_SYMBOL(usb_submit_urb_MJRcheck);
EXPORT_SYMBOL(vfree_MJRcheck);
EXPORT_SYMBOL(vmalloc_MJRcheck);
EXPORT_SYMBOL(_write_lock_irq_MJRcheck);
EXPORT_SYMBOL(_write_lock_irqsave_MJRcheck);
EXPORT_SYMBOL(_write_unlock_irq_MJRcheck);
EXPORT_SYMBOL(_write_unlock_irqrestore_MJRcheck);


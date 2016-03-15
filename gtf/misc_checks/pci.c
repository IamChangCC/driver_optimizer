#include "../test_state.h"
#include "../testing_ep.h"
#include "../mega_header.h"

extern struct pci_driver_state_struct pci_driver_state;
extern struct ndo_driver_state_struct ndo_driver_state;

STATIC int pci_enable_device_MJRcheck(const char *fn,
                                      int prepost,
                                      int *retval,
                                      struct pci_dev **dev) {
    check_routine_start();
    if (prepost == 0) {
        tfassert (pci_driver_state.enabled == DEVICE_STATE_DISABLED ||
                  pci_driver_state.enabled == DEVICE_STATE_UNDEFINED);
    } else if (prepost == 1) {
        if (*retval == 0) {
            set_device_state(fn, &pci_driver_state.enabled, DEVICE_STATE_ENABLED);
        } else {
            set_device_state(fn, &pci_driver_state.enabled, DEVICE_STATE_DISABLED);
        }
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

STATIC int pci_enable_device_mem_MJRcheck(const char *fn,
                                          int prepost,
                                          int *retval,
                                          struct pci_dev **dev) {
    //check_routine_start();
    // pci_disable_device needs to work with this.
    pci_enable_device_MJRcheck(fn, prepost, retval, dev);
    //check_routine_end();
    return 0;
}

STATIC int pci_disable_device_MJRcheck(const char *fn,
                                       int prepost,
                                       struct pci_dev **dev) {
    check_routine_start();
    if (prepost == 0) {
        tfassert (pci_driver_state.enabled == DEVICE_STATE_ENABLED);
        tfassert (ndo_driver_state.register_netdev == NOT_CALLED ||
                  ndo_driver_state.register_netdev == CALLED_FAILED);
        tfassert (ndo_driver_state.unregister_netdev == NOT_CALLED ||
                  ndo_driver_state.unregister_netdev == CALLED_FAILED);
    } else if (prepost == 1) {
        // The idea is that the device is disabled now
        set_device_state(fn, &pci_driver_state.enabled, DEVICE_STATE_DISABLED);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

STATIC int __pci_register_driver_MJRcheck(const char *fn,
                                          int prepost,
                                          int *retval,
                                          struct pci_driver **arg0,
                                          struct module **arg1,
                                          char const **arg2) {
    check_routine_start();
    if (prepost == 0) {
        tfassert (pci_driver_state.registered == NOT_CALLED);
        set_call_state (fn, &pci_driver_state.registered, IN_PROGRESS);
        set_driver_bus (fn, DRIVER_PCI);
    } else if (prepost == 1) {
        if (*retval == 0) {
            ep_init_pci_driver(*arg0); // Entry point testing.
            set_call_state (fn, &pci_driver_state.registered, CALLED_OK);
        } else {
            set_call_state (fn, &pci_driver_state.registered, CALLED_FAILED);
        }
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

STATIC int pci_unregister_driver_MJRcheck(const char *fn,
                                          int prepost,
                                          struct pci_driver **arg0) {
    check_routine_start();
    if (prepost == 0) {
        tfassert (pci_driver_state.registered == CALLED_OK);
        set_call_state (fn, &pci_driver_state.unregistered, IN_PROGRESS);
    } else if (prepost == 1) {
        set_call_state (fn, &pci_driver_state.registered, NOT_CALLED);
        set_call_state (fn, &pci_driver_state.unregistered, NOT_CALLED);
        remove_driver_bus(fn, DRIVER_PCI);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

STATIC int pci_iomap_MJRcheck(const char *fn,
                              int prepost,
                              void **retval,
                              struct pci_dev **dev,
                              int *bar,
                              unsigned long *maxlen) {
    check_routine_start();
    if (prepost == 1) {
        // TODO see implementation of this function
        // to do a 100% correct job
        // Note length may be less than that of pci_resource_len
        if (*retval != NULL) {
            s2e_io_region(__LINE__, SYMDRIVE_PORT_MAP, (unsigned int) *retval, pci_resource_len(*dev, *bar));
        }
    }
    check_routine_end();
    return 0;
}

STATIC int pci_iounmap_MJRcheck(const char *fn,
                                int prepost,
                                struct pci_dev **dev,
                                void **addr) {
    check_routine_start();
    if (prepost == 0) {
        if (*addr != NULL) {
            s2e_io_region(__LINE__, SYMDRIVE_PORT_UNMAP, (unsigned int) *addr, 0);
        }
    }
    check_routine_end();
    return 0;
}

STATIC int pci_ioremap_bar_MJRcheck(const char *fn,
                                    int prepost,
                                    void **retval,
                                    struct pci_dev **pdev,
                                    int *bar) {
    check_routine_start();
    if (prepost == 1) {
        generic_allocator(fn, prepost, (unsigned long) *retval, (unsigned long) -1, ORIGIN_IOREMAP);
        if (*retval != NULL) {
            s2e_io_region(__LINE__, SYMDRIVE_IO_MAP, (unsigned int) *retval, pci_resource_len(*pdev, *bar));
        }
    }
    check_routine_end();
    return 0;
}

/*
  STATIC int pci_alloc_consistent_MJRcheck(const char *fn,
  int prepost,
  void **retval,
  struct pci_dev **dev,
  size_t *size,
  dma_addr_t **dma) {
  check_routine_start();
  if (prepost == 1) {
  generic_allocator(fn, prepost, (unsigned long) *retval, *size, ORIGIN_PCI_ALLOC_CONSISTENT);
  }
  check_routine_end();
  return 0;
  }

  STATIC int pci_free_consistent_MJRcheck (const char *fn,
  int prepost,
  struct pci_dev **pdev,
  size_t *size,
  void **cpu_addr,
  dma_addr_t *dma_addr) {
  check_routine_start();
  if (prepost == 0) {
  generic_free(fn, prepost, (unsigned long) (*cpu_addr), ORIGIN_PCI_ALLOC_CONSISTENT);
  }
  check_routine_end();
  return 0;
  }
*/

STATIC int pci_map_single_MJRcheck(const char *fn,
                                   int prepost,
                                   dma_addr_t *retval,
                                   struct pci_dev **pdev,
                                   void **cpu_addr,
                                   size_t *size,
                                   int *dir) {
    check_routine_start();
    if (prepost == 1) {
        generic_allocator(fn, prepost, (unsigned long) *retval, *size, ORIGIN_PCI_MAP_SINGLE);
        if (*retval != 0) {
            // s2e_make_dma_symbolic ((void *) *retval, *size, "pci_map_single");
            // see pci_unmap_single_MJRcheck below
        }
    }
    check_routine_end();
    return 0;
}

STATIC int pci_unmap_single_MJRcheck(const char *fn,
                                     int prepost,
                                     struct pci_dev **pdev,
                                     dma_addr_t *dma_addr,
                                     size_t *size,
                                     int *direction) {
    check_routine_start();
#if !defined(DISABLE_DL2K_MJRCHECKS) && !defined(DISABLE_ET131X_MJRCHECKS) && !defined(DISABLE_BE2NET_MJRCHECKS)
    // Note:  disabling this free operation leaves an apparent memory leak,
    // but is necessary because the driver tries to free addresses stored in DMA memory.
    if (prepost == 0) {
        generic_free(fn, prepost, (unsigned long) (*dma_addr), ORIGIN_PCI_MAP_SINGLE);

        //
        // The problem here:  We need a way to translate the physical/DMA address
        // to a CPU/virtual address.  No easy way to this currently since the object
        // tracker stores the DMA address.  Hmm.
        //
        // If we modify the implementation to store a CPU address in the object
        // tracker, then we won't be able to catch conflicts with pci_map_single/pci_map_page
        // as we currently do.  pci_map_page does not return a CPU address.
        //

        // s2e_free_dma_symbolic((void *) *dma_addr, *size, "pci_map_single");
        // s2e_make_symbolic((void *) *WHAT_GOES_HERE, *size, "pci_map_single");
    }
#endif
    check_routine_end();
    return 0;
}

STATIC int pci_map_page_MJRcheck(const char *fn,
                                 int prepost,
                                 dma_addr_t *retval,
                                 struct pci_dev **pdev,
                                 struct page **page,
                                 unsigned long *offset,
                                 size_t *size,
                                 int *dir) {
    check_routine_start();
    if (prepost == 1) {
        generic_allocator(fn, prepost, (unsigned long) *retval, *size, ORIGIN_PCI_MAP_PAGE);
    }
    check_routine_end();
    return 0;
}

STATIC int pci_unmap_page_MJRcheck(const char *fn,
                                   int prepost,
                                   struct pci_dev **pdev,
                                   dma_addr_t *dma_addr,
                                   size_t *size,
                                   int *direction) {
    check_routine_start();
#if !defined(DISABLE_BE2NET_MJRCHECKS)
    if (prepost == 0) {
        generic_free(fn, prepost, (unsigned long) (*dma_addr), ORIGIN_PCI_MAP_PAGE);
    }
#endif
    check_routine_end();
    return 0;
}

EXPORT_SYMBOL(pci_disable_device_MJRcheck);
EXPORT_SYMBOL(pci_enable_device_MJRcheck);
EXPORT_SYMBOL(pci_enable_device_mem_MJRcheck);
EXPORT_SYMBOL(pci_iomap_MJRcheck);
EXPORT_SYMBOL(pci_ioremap_bar_MJRcheck);
EXPORT_SYMBOL(pci_map_page_MJRcheck);
EXPORT_SYMBOL(pci_map_single_MJRcheck);
EXPORT_SYMBOL(__pci_register_driver_MJRcheck);
EXPORT_SYMBOL(pci_unmap_page_MJRcheck);
EXPORT_SYMBOL(pci_unmap_single_MJRcheck);
EXPORT_SYMBOL(pci_unregister_driver_MJRcheck);

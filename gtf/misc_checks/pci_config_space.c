#include "../test_state.h"
#include "../testing_ep.h"
#include "../mega_header.h"

STATIC int pci_bus_read_config_byte_MJRcheck(const char *fn,
                                             int prepost,
                                             int *retval,
                                             struct pci_bus **bus,
                                             unsigned int *devfn,
                                             int *where,
                                             u8 **val) {
    check_routine_start();
    if (prepost == 0) {
    } else if (prepost == 1) {
        s2e_make_symbolic(*val, sizeof(u8), __func__);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}
    
STATIC int pci_bus_read_config_word_MJRcheck(const char *fn,
                                             int prepost,
                                             int *retval,
                                             struct pci_bus **bus,
                                             unsigned int *devfn,
                                             int *where,
                                             u16 **val) {
    check_routine_start();
    if (prepost == 0) {
    } else if (prepost == 1) {
        s2e_make_symbolic(*val, sizeof(u16), __func__);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

STATIC int pci_bus_read_config_dword_MJRcheck(const char *fn,
                                              int prepost,
                                              int *retval,
                                              struct pci_bus **bus,
                                              unsigned int *devfn,
                                              int *where,
                                              u32 **val) {
    check_routine_start();
    if (prepost == 0) {
    } else if (prepost == 1) {
        s2e_make_symbolic(*val, sizeof(u32), __func__);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

STATIC int pci_bus_write_config_byte_MJRcheck(const char *fn,
                                              int prepost,
                                              int *retval,
                                              struct pci_bus **bus,
                                              unsigned int *devfn,
                                              int *where,
                                              u8 *val) {
    check_routine_start();
    if (prepost == 0) {
        *retval = 0;
    } else if (prepost == 1) {
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 1; // don't run kernel function
}

STATIC int pci_bus_write_config_word_MJRcheck(const char *fn,
                                              int prepost,
                                              int *retval,
                                              struct pci_bus **bus,
                                              unsigned int *devfn,
                                              int *where,
                                              u16 *val) {
    check_routine_start();
    if (prepost == 0) {
        *retval = 0;
    } else if (prepost == 1) {
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 1; // don't run kernel function
}

STATIC int pci_bus_write_config_dword_MJRcheck(const char *fn,
                                               int prepost,
                                               int *retval,
                                               struct pci_bus **bus,
                                               unsigned int *devfn,
                                               int *where,
                                               u32 *val) {
    check_routine_start();
    if (prepost == 0) {
        *retval = 0;
    } else if (prepost == 1) {
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 1; // don't run kernel function
}

STATIC int pci_find_capability_MJRcheck(const char *fn,
                                        int prepost,
                                        int *retval,
                                        struct pci_dev **dev,
                                        int *cap) {
    check_routine_start();
    if (prepost == 0) {
    } else if (prepost == 1) {
        //int newval;
        //s2e_make_symbolic(&newval, sizeof(int), __func__);
        //*retval = newval;
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}


EXPORT_SYMBOL(pci_bus_read_config_byte_MJRcheck);
EXPORT_SYMBOL(pci_bus_read_config_word_MJRcheck);
EXPORT_SYMBOL(pci_bus_read_config_dword_MJRcheck);
EXPORT_SYMBOL(pci_bus_write_config_byte_MJRcheck);
EXPORT_SYMBOL(pci_bus_write_config_word_MJRcheck);
EXPORT_SYMBOL(pci_bus_write_config_dword_MJRcheck);
EXPORT_SYMBOL(pci_find_capability_MJRcheck);

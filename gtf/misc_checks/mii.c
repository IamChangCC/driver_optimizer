#include "../test_state.h"
#include "../testing_ep.h"
#include "../mega_header.h"

extern struct pci_driver_state_struct pci_driver_state;
extern struct ndo_driver_state_struct ndo_driver_state;

STATIC int generic_mii_ioctl_MJRcheck(const char *fn,
                                      int prepost,
                                      int *retval,
                                      struct mii_if_info **mii_if,
                                      struct mii_ioctl_data **mii_data,
                                      int *cmd,
                                      unsigned int **duplex_changed) {
    check_routine_start();
    if (prepost == 0) {
        set_call_state(fn, &ndo_driver_state.capable, IN_PROGRESS);
    } else if (prepost == 1) {
        set_call_state(fn, &ndo_driver_state.capable, CALLED_OK);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

STATIC int mii_check_link_MJRcheck(const char *fn,
                                   int prepost,
                                   struct mii_if_info **mii) {
    check_routine_start();
    if (prepost == 1) {
        if (g_sym_kill_carrier_off) {
            //if (!netif_carrier_ok((*mii)->dev)) {
            //    tfassert_detail (0, "Terminating mii_MJRcheck_media / netif_carrier_off path...\n");
            //}
            uprintk ("Forcing carrier on in %s\n", __func__);
            netif_carrier_on((*mii)->dev);
        }
    }
    check_routine_end();
    return 0;
}

STATIC int mii_check_media_MJRcheck(const char *fn,
                                    int prepost,
                                    unsigned int *retval,
                                    struct mii_if_info **mii,
                                    unsigned int *ok_to_print,
                                    unsigned int *init_media) {
    check_routine_start();
    if (prepost == 1) {
        if (g_sym_kill_carrier_off) {
            //if (!netif_carrier_ok((*mii)->dev)) {
            //    tfassert_detail (0, "Terminating mii_MJRcheck_media / netif_carrier_off path...\n");
            //}
            uprintk ("Forcing carrier on in %s\n", __func__);
            netif_carrier_on((*mii)->dev);
        }
    }
    check_routine_end();
    return 0;
}

STATIC int mii_link_ok_MJRcheck(const char *fn,
                                int prepost,
                                int *retval,
                                struct mii_if_info **mii) {
    check_routine_start();
    if (prepost == 1) {
        if (g_sym_kill_carrier_off) {
            //if (*retval == 0) {
            //    tfassert_detail (0, "Terminating mii_link_ok / netif_carrier_off path...\n");
            //}
            uprintk ("Forcing retval = 1 in %s\n", __func__);
            *retval = 1;
        }
    }
    check_routine_end();
    return 0;
}

EXPORT_SYMBOL(generic_mii_ioctl_MJRcheck);
EXPORT_SYMBOL(mii_check_link_MJRcheck);
EXPORT_SYMBOL(mii_check_media_MJRcheck);
EXPORT_SYMBOL(mii_link_ok_MJRcheck);

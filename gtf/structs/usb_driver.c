#include "../test_state.h"
#include "../mega_header.h"

extern struct usb_driver_state_struct usb_driver_state;
extern struct ndo_driver_state_struct ndo_driver_state;

//
// usb_driver struct- exported members
//
STATIC int usb_driver_probe_MJRcheck(const char *fn,
                                     int prepost,
                                     int *retval,
                                     struct usb_interface **intf,
                                     /*const*/ struct usb_device_id **id) {
    check_routine_start();
    if (prepost == 0) {
        tfassert (usb_driver_state.probed == NOT_CALLED);

        set_call_state (fn, &usb_driver_state.probed, IN_PROGRESS);
#ifndef DISABLE_PEGASUS_MJRCHECKS
        push_kernel_state(fn, BLOCKING_YES, USER_NO);
#else
        push_kernel_state(fn, BLOCKING_UNDEFINED, USER_UNDEFINED);
#endif
    } else if (prepost == 1) {
        if (*retval == 0) {
            set_call_state (fn, &usb_driver_state.probed, CALLED_OK);
            if (get_driver_class (fn) == DRIVER_NET) {
                tfassert (ndo_driver_state.register_netdev == CALLED_OK);
            }
        } else {
            set_call_state (fn, &usb_driver_state.probed, CALLED_FAILED);
            if (get_driver_class (fn) == DRIVER_NET) {
                tfassert (ndo_driver_state.register_netdev == CALLED_FAILED ||
                          ndo_driver_state.register_netdev == NOT_CALLED);
            }
        }

        pop_kernel_state(fn);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

STATIC int usb_driver_disconnect_MJRcheck(const char *fn,
                                          int prepost,
                                          struct usb_interface **intf) {
    check_routine_start();
    if (prepost == 0) {
#ifndef DISABLE_PEGASUS_MJRCHECKS
        push_kernel_state(fn, BLOCKING_YES, USER_NO);
#else
        push_kernel_state(fn, BLOCKING_UNDEFINED, USER_UNDEFINED);
#endif
    } else if (prepost == 1) {
        pop_kernel_state (fn);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

STATIC int usb_driver_unlocked_ioctl_MJRcheck(const char *fn,
                                              int prepost,
                                              int *retval,
                                              struct usb_interface **intf,
                                              unsigned int *code,
                                              void **buf) {
    check_routine_start();
    if (prepost == 0) {
        push_kernel_state(fn, BLOCKING_YES, USER_YES);
    } else if (prepost == 1) {
        pop_kernel_state(fn);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

STATIC int usb_driver_suspend_MJRcheck(const char *fn,
                                       int prepost,
                                       int *retval,
                                       struct usb_interface *intf,
                                       pm_message_t message) {
    check_routine_start();
    check_routine_end();
    return 0;
}

STATIC int usb_driver_resume_MJRcheck(const char *fn,
                                      int prepost,
                                      int *retval,
                                      struct usb_interface **intf) {
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

STATIC int usb_driver_reset_resume_MJRcheck(const char *fn,
                                            int prepost,
                                            int *retval,
                                            struct usb_interface **intf) {
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

STATIC int usb_driver_pre_reset_MJRcheck(const char *fn,
                                         int prepost,
                                         int *retval,
                                         struct usb_interface **intf) {
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

STATIC int usb_driver_post_reset_MJRcheck(const char *fn,
                                          int prepost,
                                          int *retval,
                                          struct usb_interface **intf) {
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

EXPORT_SYMBOL(usb_driver_probe_MJRcheck);
EXPORT_SYMBOL(usb_driver_disconnect_MJRcheck);
EXPORT_SYMBOL(usb_driver_unlocked_ioctl_MJRcheck);
EXPORT_SYMBOL(usb_driver_resume_MJRcheck);
EXPORT_SYMBOL(usb_driver_reset_resume_MJRcheck);
EXPORT_SYMBOL(usb_driver_suspend_MJRcheck);
EXPORT_SYMBOL(usb_driver_pre_reset_MJRcheck);
EXPORT_SYMBOL(usb_driver_post_reset_MJRcheck);

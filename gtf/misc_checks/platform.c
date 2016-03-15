#include "../test_state.h"
#include "../testing_ep.h"
#include "../mega_header.h"

extern struct platform_driver_state_struct platform_driver_state;

STATIC int platform_driver_register_MJRcheck(const char *fn,
                                             int prepost,
                                             int *retval,
                                             struct platform_driver **drv) {
    check_routine_start();
    if (prepost == 0) {
        set_call_state (fn, &platform_driver_state.registered, IN_PROGRESS);
    } else if (prepost == 1) {
        if (*retval == 0) {
            set_call_state (fn, &platform_driver_state.registered, CALLED_OK);
            set_driver_bus(fn, DRIVER_PLATFORM);
        } else {
            set_call_state (fn, &platform_driver_state.registered, CALLED_FAILED);
        }
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

STATIC int platform_driver_unregister_MJRcheck(const char *fn,
                                               int prepost,
                                               struct platform_driver **drv) {
    check_routine_start();
    if (prepost == 0) {
    } else if (prepost == 1) {
        remove_driver_bus(fn, DRIVER_PLATFORM);
    } else {
        tfassert(0);
    }
    check_routine_end();
    return 0;
}

EXPORT_SYMBOL(platform_driver_register_MJRcheck);
EXPORT_SYMBOL(platform_driver_unregister_MJRcheck);

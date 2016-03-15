#include "../test_state.h"
#include "../testing_ep.h"
#include "../mega_header.h"

extern struct platform_driver_state_struct platform_driver_state;

STATIC int platform_driver_probe_MJRcheck(const char *fn,
                                          int prepost,
                                          int *retval,
                                          struct platform_device **dev) {
    check_routine_start();
    if (prepost == 0) {
        set_call_state (fn, &platform_driver_state.probed, IN_PROGRESS);
    } else if (prepost == 1) {
        if (*retval == 0) {
            set_call_state (fn, &platform_driver_state.probed, CALLED_OK);
        } else {
            set_call_state (fn, &platform_driver_state.probed, CALLED_FAILED);
        }
    } else {
        tfassert(0);
    }
    check_routine_end();
    return 0;
}

STATIC int platform_driver_remove_MJRcheck(const char *fn,
                                           int prepost,
                                           int *retval,
                                           struct platform_device **dev) {
    check_routine_start();
    check_routine_end();
    return 0;
}

STATIC int platform_driver_shutdown_MJRcheck(const char *fn,
                                             int prepost,
                                             struct platform_device **dev) {
    check_routine_start();
    check_routine_end();
    return 0;
}

STATIC int platform_driver_suspend_MJRcheck(const char *fn,
                                            int prepost,
                                            int *retval,
                                            struct platform_device **dev,
                                            pm_message_t *state) {
    check_routine_start();
    check_routine_end();
    return 0;
}

STATIC int platform_driver_resume_MJRcheck(const char *fn,
                                           int prepost,
                                           int *retval,
                                           struct platform_device **dev) {
    check_routine_start();
    check_routine_end();
    return 0;
}

EXPORT_SYMBOL(platform_driver_probe_MJRcheck);
EXPORT_SYMBOL(platform_driver_remove_MJRcheck);
EXPORT_SYMBOL(platform_driver_shutdown_MJRcheck);
EXPORT_SYMBOL(platform_driver_suspend_MJRcheck);
EXPORT_SYMBOL(platform_driver_resume_MJRcheck);

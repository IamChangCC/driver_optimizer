#include "../test_state.h"
#include "../mega_header.h"

//
// struct bus_type
//
STATIC int bus_type_match_MJRcheck (const char *fn,
                                    int prepost,
                                    int *retval,
                                    struct device **dev,
                                    struct device_driver **drv) {
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

STATIC int bus_type_uevent_MJRcheck (const char *fn,
                                     int prepost,
                                     int *retval,
                                     struct device **dev,
                                     struct kobj_uevent_env **env) {
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

STATIC int bus_type_probe_MJRcheck (const char *fn,
                                    int prepost,
                                    int *retval,
                                    struct device **dev) {
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

STATIC int bus_type_remove_MJRcheck (const char *fn,
                                     int prepost,
                                     int *retval,
                                     struct device **dev) {
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

STATIC int bus_type_shutdown_MJRcheck (const char *fn,
                                       int prepost,
                                       struct device **dev) {
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

STATIC int bus_type_suspend_MJRcheck (const char *fn,
                                      int prepost,
                                      int *retval,
                                      struct device **dev,
                                      pm_message_t *state) {
    check_routine_start();
    check_routine_end();
    return 0;
}

STATIC int bus_type_resume_MJRcheck (const char *fn,
                                     int prepost,
                                     int *retval,
                                     struct device **dev) {
    check_routine_start();
    check_routine_end();
    return 0;
}

EXPORT_SYMBOL(bus_type_match_MJRcheck);
EXPORT_SYMBOL(bus_type_uevent_MJRcheck);
EXPORT_SYMBOL(bus_type_probe_MJRcheck);
EXPORT_SYMBOL(bus_type_remove_MJRcheck);
EXPORT_SYMBOL(bus_type_shutdown_MJRcheck);
EXPORT_SYMBOL(bus_type_suspend_MJRcheck);
EXPORT_SYMBOL(bus_type_resume_MJRcheck);

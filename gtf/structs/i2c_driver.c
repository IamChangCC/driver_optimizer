#include "../test_state.h"
#include "../testing_ep.h"
#include "../mega_header.h"

extern struct i2c_driver_state_struct i2c_driver_state;

STATIC int i2c_driver_attach_adapter_MJRcheck(const char *fn,
                                              int prepost,
                                              int *retval,
                                              struct i2c_adapter **adapter) {
    check_routine_start();
    check_routine_end();
    return 0;
}

STATIC int i2c_driver_detach_adapter_MJRcheck(const char *fn,
                                              int prepost,
                                              int *retval,
                                              struct i2c_adapter **adapter) {
    check_routine_start();
    check_routine_end();
    return 0;
}

STATIC int i2c_driver_probe_MJRcheck(const char *fn,
                                     int prepost,
                                     int *retval,
                                     struct i2c_client **client,
                                     const struct i2c_device_id **devid) {
    check_routine_start();
    if (prepost == 0) {
        ep_init_i2c_client(*client); // Entry point testing
        set_call_state (fn, &i2c_driver_state.probed, IN_PROGRESS);
    } else if (prepost == 1) {
        if (*retval == 0) {
            set_call_state (fn, &i2c_driver_state.probed, CALLED_OK);
        } else {
            set_call_state (fn, &i2c_driver_state.probed, CALLED_FAILED);
        }
    } else {
        tfassert(0);
    }
    check_routine_end();
    return 0;
}

STATIC int i2c_driver_remove_MJRcheck(const char *fn,
                                      int prepost,
                                      int *retval,
                                      struct i2c_client **client) {
    check_routine_start();
    check_routine_end();
    return 0;
}

STATIC int i2c_driver_shutdown_MJRcheck(const char *fn,
                                        int prepost,
                                        struct i2c_client **client) {
    check_routine_start();
    check_routine_end();
    return 0;
}

STATIC int i2c_driver_suspend_MJRcheck(const char *fn,
                                       int prepost,
                                       int *retval,
                                       struct i2c_client **client,
                                       pm_message_t *mesg) {
    check_routine_start();
    check_routine_end();
    return 0;
}

STATIC int i2c_driver_resume_MJRcheck(const char *fn,
                                      int prepost,
                                      int *retval,
                                      struct i2c_client **client) {
    check_routine_start();
    check_routine_end();
    return 0;
}

STATIC int i2c_driver_alert_MJRcheck(const char *fn,
                                     int prepost,
                                     struct i2c_client **client,
                                     unsigned int *data) {
    check_routine_start();
    check_routine_end();
    return 0;
}

STATIC int i2c_driver_command_MJRcheck(const char *fn,
                                       int prepost,
                                       int *retval,
                                       struct i2c_client **client,
                                       unsigned int *cmd,
                                       void **arg) {
    check_routine_start();
    check_routine_end();
    return 0;
}

EXPORT_SYMBOL(i2c_driver_attach_adapter_MJRcheck);
EXPORT_SYMBOL(i2c_driver_detach_adapter_MJRcheck);
EXPORT_SYMBOL(i2c_driver_probe_MJRcheck);
EXPORT_SYMBOL(i2c_driver_remove_MJRcheck);
EXPORT_SYMBOL(i2c_driver_shutdown_MJRcheck);
EXPORT_SYMBOL(i2c_driver_suspend_MJRcheck);
EXPORT_SYMBOL(i2c_driver_resume_MJRcheck);
EXPORT_SYMBOL(i2c_driver_alert_MJRcheck);
EXPORT_SYMBOL(i2c_driver_command_MJRcheck);

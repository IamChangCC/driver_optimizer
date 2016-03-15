#include "../test_state.h"
#include "../testing_ep.h"
#include "../mega_header.h"

extern struct i2c_driver_state_struct i2c_driver_state;
extern struct ndo_driver_state_struct ndo_driver_state;

STATIC int i2c_add_driver_MJRcheck(const char *fn,
                                   int prepost,
                                   int *retval,
                                   struct i2c_driver **driver) {
    check_routine_start();
    if (prepost == 0) {
        set_call_state (fn, &i2c_driver_state.registered, IN_PROGRESS);
    } else if (prepost == 1) {
        if (*retval == 0) {
            set_call_state (fn, &i2c_driver_state.registered, CALLED_OK);
            set_driver_bus (fn, DRIVER_I2C);
            ep_init_i2c_driver(*driver); // Entry point testing.
        } else {
            set_call_state (fn, &i2c_driver_state.registered, CALLED_FAILED);
        }
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

STATIC int i2c_del_driver_MJRcheck(const char *fn,
                                   int prepost,
                                   struct i2c_driver **driver) {
    check_routine_start();
    if (prepost == 0) {
    } else if (prepost == 1) {
        remove_driver_bus(fn, DRIVER_I2C);
    } else {
        tfassert(0);
    }
    check_routine_end();
    return 0;
}

EXPORT_SYMBOL(i2c_add_driver_MJRcheck);
EXPORT_SYMBOL(i2c_del_driver_MJRcheck);

#include "../test_state.h"
#include "../testing_ep.h"
#include "../mega_header.h"

extern struct spi_driver_state_struct spi_driver_state;

int spi_driver_probe_MJRcheck(const char *fn,
                              int prepost,
                              int *retval,
                              struct spi_device **spi) {
    check_routine_start();
    if (prepost == 0) {
        // ep_init_i2c_client(*client); // Entry point testing
        set_call_state (fn, &spi_driver_state.probed, IN_PROGRESS);
    } else if (prepost == 1) {
        if (*retval == 0) {
            set_call_state (fn, &spi_driver_state.probed, CALLED_OK);
        } else {
            set_call_state (fn, &spi_driver_state.probed, CALLED_FAILED);
        }
    } else {
        tfassert(0);
    }
    check_routine_end();
    return 0;
}

int spi_driver_remove_MJRcheck(const char *fn,
                               int prepost,
                               int *retval,
                               struct spi_device **spi) {
    check_routine_start();
    check_routine_end();
    return 0;
}

int spi_driver_shutdown_MJRcheck(const char *fn,
                                 int prepost,
                                 struct spi_device **spi) {
    check_routine_start();
    check_routine_end();
    return 0;
}

int spi_driver_suspend_MJRcheck(const char *fn,
                                int prepost,
                                int *retval,
                                struct spi_device **spi,
                                pm_message_t *mesg) {
    check_routine_start();
    check_routine_end();
    return 0;
}

int spi_driver_resume_MJRcheck(const char *fn,
                               int prepost,
                               int *retval,
                               struct spi_device **spi) {
    check_routine_start();
    check_routine_end();
    return 0;
}

EXPORT_SYMBOL(spi_driver_probe_MJRcheck);
EXPORT_SYMBOL(spi_driver_remove_MJRcheck);
EXPORT_SYMBOL(spi_driver_shutdown_MJRcheck);
EXPORT_SYMBOL(spi_driver_suspend_MJRcheck);
EXPORT_SYMBOL(spi_driver_resume_MJRcheck);

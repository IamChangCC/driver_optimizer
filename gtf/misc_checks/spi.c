#include "../test_state.h"
#include "../testing_ep.h"
#include "../mega_header.h"

extern struct spi_driver_state_struct spi_driver_state;

STATIC int spi_register_driver_MJRcheck(const char *fn,
                                        int prepost,
                                        int *retval,
                                        struct spi_driver **sdrv) {
    check_routine_start();
    if (prepost == 0) {
        set_call_state (fn, &spi_driver_state.registered, IN_PROGRESS);
    } else if (prepost == 1) {
        if (*retval == 0) {
            set_call_state (fn, &spi_driver_state.registered, CALLED_OK);
            set_driver_bus(fn, DRIVER_SPI);
        } else {
            set_call_state (fn, &spi_driver_state.registered, CALLED_FAILED);
        }
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

STATIC int spi_unregister_driver_MJRcheck(const char *fn,
                                          int prepost,
                                          struct spi_driver **sdrv) {
    check_routine_start();
    if (prepost == 0) {
    } else if (prepost == 1) {
        remove_driver_bus(fn, DRIVER_SPI);
    } else {
        tfassert(0);
    }
    check_routine_end();
    return 0;
}

EXPORT_SYMBOL(spi_register_driver_MJRcheck);
EXPORT_SYMBOL(spi_unregister_driver_MJRcheck);

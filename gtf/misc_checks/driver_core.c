#include "../test_state.h"
#include "../testing_ep.h"

#include "../mega_header.h"

STATIC int device_create_file_MJRcheck(const char *fn,
                                       int prepost,
                                       int *retval,
                                       struct device **dev,
                                       const struct device_attribute **attr) {
    check_routine_start();
    if (prepost == 0) {
    } else if (prepost == 1) {
        if (!IS_ERR_VALUE(*retval)) {
            generic_allocator(fn, prepost, (unsigned long) *attr, (unsigned long) -1, ORIGIN_DEVICEFILE_CREATE);
        }
    } else {
        tfassert(0);
    }
    check_routine_end();
    return 0;
}

STATIC int device_remove_file_MJRcheck(const char *fn,
                                       int prepost,
                                       struct device **dev,
                                       const struct device_attribute **attr) {
    check_routine_start();
    if (prepost == 0) {
        generic_free(fn, prepost, (unsigned long) *attr, ORIGIN_DEVICEFILE_CREATE);
    } else if (prepost == 1) {
    } else {
        tfassert(0);
    }
    check_routine_end();
    return 0;
}

EXPORT_SYMBOL(device_remove_file_MJRcheck);
EXPORT_SYMBOL(device_create_file_MJRcheck);

#include "../test_state.h"
#include "../testing_ep.h"
#include "../mega_header.h"

STATIC int __class_create_MJRcheck(char const *fn, int prepost, struct class **retval,
                                   struct module **arg3, char const **arg4, struct lock_class_key **arg5) {
    check_routine_start();
    if (prepost == 0) {
    } else if (prepost == 1) {
        if (!IS_ERR(*retval)) {
            generic_allocator(fn, prepost, (unsigned long) *retval, (unsigned long) -1, ORIGIN_CLASS_CREATE);
        }
    } else {
        tfassert(0);
    }
    check_routine_end();
    return 0;
}

STATIC int class_destroy_MJRcheck(char const *fn, int prepost, struct class **cls) {
    check_routine_start();
    if (prepost == 0) {
        generic_free(fn, prepost, (unsigned long) *cls, ORIGIN_CLASS_CREATE);
    } else if (prepost == 1) {
    } else {
        tfassert(0);
    }
    check_routine_end();
    return 0;
}

EXPORT_SYMBOL(__class_create_MJRcheck);
EXPORT_SYMBOL(class_destroy_MJRcheck);

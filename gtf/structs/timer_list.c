#include "../test_state.h"
#include "../mega_header.h"

STATIC int timer_list_function_MJRcheck(const char *fn,
                                        int prepost,
                                        unsigned long *arg) {
    check_routine_start();
    check_routine_end();
    return 0;
}

EXPORT_SYMBOL(timer_list_function_MJRcheck);

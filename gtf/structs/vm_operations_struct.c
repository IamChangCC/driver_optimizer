#include "../test_state.h"
#include "../mega_header.h"

//
// struct vm_operations_struct
//
STATIC int vm_operations_struct_open_MJRcheck (const char *fn,
                                               int prepost,
                                               struct vm_area_struct **area) {
    check_routine_start();
    if (prepost == 0) {
        push_kernel_state(fn, BLOCKING_YES, USER_YES);
    } else if (prepost == 1) {
        pop_kernel_state(fn);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

STATIC int vm_operations_struct_close_MJRcheck (const char *fn,
                                                int prepost,
                                                struct vm_area_struct **area) {
    check_routine_start();
    if (prepost == 0) {
        push_kernel_state(fn, BLOCKING_YES, USER_YES);
    } else if (prepost == 1) {
        pop_kernel_state(fn);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

STATIC int vm_operations_struct_fault_MJRcheck (const char *fn,
                                                int prepost,
                                                int *retval,
                                                struct vm_area_struct **vma,
                                                struct vm_fault *vmf) {
    check_routine_start();
    if (prepost == 0) {
        push_kernel_state(fn, BLOCKING_YES, USER_YES);
    } else if (prepost == 1) {
        pop_kernel_state(fn);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

STATIC int vm_operations_struct_page_mkwrite_MJRcheck (const char *fn,
                                                       int prepost,
                                                       int *retval,
                                                       struct vm_area_struct **vma,
                                                       struct vm_fault **vmf) {
    check_routine_start();
    if (prepost == 0) {
        push_kernel_state(fn, BLOCKING_YES, USER_YES);
    } else if (prepost == 1) {
        pop_kernel_state(fn);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

STATIC int vm_operations_struct_access_MJRcheck (const char *fn,
                                                 int prepost,
                                                 int *retval,
                                                 struct vm_area_struct **vma,
                                                 unsigned long *addr,
                                                 void **buf,
                                                 int *len,
                                                 int *write) {
    check_routine_start();
    if (prepost == 0) {
        push_kernel_state(fn, BLOCKING_YES, USER_YES);
    } else if (prepost == 1) {
        pop_kernel_state(fn);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

EXPORT_SYMBOL(vm_operations_struct_open_MJRcheck);
EXPORT_SYMBOL(vm_operations_struct_close_MJRcheck);
EXPORT_SYMBOL(vm_operations_struct_fault_MJRcheck);
EXPORT_SYMBOL(vm_operations_struct_page_mkwrite_MJRcheck);
EXPORT_SYMBOL(vm_operations_struct_access_MJRcheck);

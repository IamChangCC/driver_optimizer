#include "../test_state.h"
#include "../testing_ep.h"

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>

//
// struct file_operations
//
static int file_operations_llseek_MJRcheck (const char *fn,
                                            int prepost,
                                            loff_t *retval,
                                            struct file **arg1,
                                            loff_t *arg2,
                                            int *arg3) {
    check_routine_start();
    if (prepost == 0) {
        push_kernel_state(fn, BLOCKING_YES, USER_YES);
        // ep_llseek_fop_MJRcheck (fn, prepost, *retval, (void **) arg1, arg2, arg3);
    } else if (prepost == 1) {
        pop_kernel_state(fn);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

static int file_operations_read_MJRcheck (const char *fn,
                                          int prepost,
                                          ssize_t *retval,
                                          struct file **arg1,
                                          char __user **arg2,
                                          size_t *arg3,
                                          loff_t **arg4) {
    check_routine_start();
    if (prepost == 0) {
        push_kernel_state(fn, BLOCKING_YES, USER_YES);
        // ep_read_fop_MJRcheck (fn, prepost, *retval, (void **) arg1, arg2, arg3, arg4);
    } else if (prepost == 1) {
        pop_kernel_state(fn);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

static int file_operations_write_MJRcheck (const char *fn,
                                           int prepost,
                                           ssize_t *retval,
                                           struct file **arg1,
                                           /*const*/ char __user **arg2,
                                           size_t *arg3,
                                           loff_t **arg4) {
    check_routine_start();
    if (prepost == 0) {
        push_kernel_state(fn, BLOCKING_YES, USER_YES);
        // ep_write_fop_MJRcheck(fn, prepost, *retval, (void **) arg1, arg2, arg3, arg4);
    } else if (prepost == 1) {
        pop_kernel_state(fn);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

static int file_operations_aio_read_MJRcheck (const char *fn,
                                              int prepost,
                                              ssize_t *retval,
                                              struct kiocb **arg1,
                                              /*const*/ struct iovec **arg2,
                                              unsigned long *arg3,
                                              loff_t *arg4) {
    check_routine_start();
    if (prepost == 0) {
        push_kernel_state(fn, BLOCKING_YES, USER_YES);
        // ep_aio_read_fop_MJRcheck (fn, prepost, *retval, (void **) arg1, (void **) arg2, arg3, arg4);
    } else if (prepost == 1) {
        pop_kernel_state(fn);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

static int file_operations_aio_write_MJRcheck (const char *fn,
                                               int prepost,
                                               ssize_t *retval,
                                               struct kiocb **arg1,
                                               /*const*/ struct iovec **arg2,
                                               unsigned long *arg3,
                                               loff_t *arg4) {
    check_routine_start();
    if (prepost == 0) {
        push_kernel_state(fn, BLOCKING_YES, USER_YES);
        // ep_aio_write_fop_MJRcheck (fn, prepost, *retval, (void **) arg1, (void **) arg2, arg3, arg4);
    } else if (prepost == 1) {
        pop_kernel_state(fn);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

static int file_operations_readdir_MJRcheck (const char *fn,
                                             int prepost,
                                             int *retval,
                                             struct file **arg1,
                                             void **arg2,
                                             filldir_t *arg3) {
    check_routine_start();
    if (prepost == 0) {
        push_kernel_state(fn, BLOCKING_YES, USER_YES);
        // ep_readdir_fop_MJRcheck (fn, prepost, *retval, (void **) arg1, arg2, (void *) arg3);
    } else if (prepost == 1) {
        pop_kernel_state(fn);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

static int file_operations_poll_MJRcheck (const char *fn,
                                          int prepost,
                                          unsigned int *retval,
                                          struct file **arg1,
                                          struct poll_table_struct **arg2) {
    check_routine_start();
    if (prepost == 0) {
        push_kernel_state(fn, BLOCKING_YES, USER_YES);
        // ep_poll_fop_MJRcheck (fn, prepost, *retval, (void **) arg1, (void **) arg2);
    } else if (prepost == 1) {
        pop_kernel_state(fn);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

static int file_operations_unlocked_ioctl_MJRcheck (const char *fn,
                                                    int prepost,
                                                    long *retval,
                                                    struct file **arg1,
                                                    unsigned int *arg2,
                                                    unsigned long *arg3) {
    check_routine_start();
    if (prepost == 0) {
        push_kernel_state(fn, BLOCKING_YES, USER_YES);
        // ep_unlocked_ioctl_fop_MJRcheck(fn, prepost, *retval, (void **) arg1, arg2, arg3);
    } else if (prepost == 1) {
        pop_kernel_state(fn);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

static int file_operations_compat_ioctl_MJRcheck (const char *fn,
                                                  int prepost,
                                                  long *retval,
                                                  struct file **arg1,
                                                  unsigned int *arg2,
                                                  unsigned long *arg3) {
    check_routine_start();
    if (prepost == 0) {
        push_kernel_state(fn, BLOCKING_YES, USER_YES);
        // ep_compat_ioctl_fop_MJRcheck(fn, prepost, *retval, (void **) arg1, arg2, arg3);
    } else if (prepost == 1) {
        pop_kernel_state(fn);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

static int file_operations_mmap_MJRcheck (const char *fn,
                                          int prepost,
                                          int *retval,
                                          struct file **arg1,
                                          struct vm_area_struct **arg2) {
    check_routine_start();
    if (prepost == 0) {
        push_kernel_state(fn, BLOCKING_YES, USER_YES);
        // ep_mmap_fop_MJRcheck (fn, prepost, *retval, (void **) arg1, (void **) arg2);
    } else if (prepost == 1) {
        pop_kernel_state(fn);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

static int file_operations_open_MJRcheck (const char *fn,
                                          int prepost,
                                          int *retval,
                                          struct inode **arg1,
                                          struct file **arg2) {
    check_routine_start();
    if (prepost == 0) {
        push_kernel_state(fn, BLOCKING_YES, USER_YES);
        // ep_open_fop_MJRcheck (fn, prepost, *retval, (void **) arg1, (void **) arg2);
    } else if (prepost == 1) {
        pop_kernel_state(fn);
        // // ep_init_snd(arg2); // Entry point testing
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

static int file_operations_flush_MJRcheck (const char *fn,
                                           int prepost,
                                           int *retval,
                                           struct file **arg1,
                                           fl_owner_t *id) {
    check_routine_start();
    if (prepost == 0) {
        push_kernel_state(fn, BLOCKING_YES, USER_YES);
        // ep_flush_fop_MJRcheck (fn, prepost, *retval, (void **) arg1, (void *) id);
    } else if (prepost == 1) {
        pop_kernel_state(fn);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

static int file_operations_release_MJRcheck (const char *fn,
                                             int prepost,
                                             int *retval,
                                             struct inode **arg1,
                                             struct file **arg2) {
    check_routine_start();
    if (prepost == 0) {
        push_kernel_state(fn, BLOCKING_YES, USER_YES);
        // ep_release_fop_MJRcheck (fn, prepost, *retval, (void **) arg1, (void **) arg2);
    } else if (prepost == 1) {
        pop_kernel_state(fn);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

static int file_operations_fsync_MJRcheck (const char *fn,
                                           int prepost,
                                           int *retval,
                                           struct file **arg1,
                                           struct dentry **arg2,
                                           int *datasync) {
    check_routine_start();
    if (prepost == 0) {
        push_kernel_state(fn, BLOCKING_YES, USER_YES);
        // ep_fsync_fop_MJRcheck (fn, prepost, *retval, (void **) arg1, (void **) arg2, datasync);
    } else if (prepost == 1) {
        pop_kernel_state(fn);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

static int file_operations_aio_fsync_MJRcheck (const char *fn,
                                               int prepost,
                                               int *retval,
                                               struct kiocb **arg1,
                                               int *datasync) {
    check_routine_start();
    if (prepost == 0) {
        push_kernel_state(fn, BLOCKING_YES, USER_YES);
        // ep_aio_fsync_fop_MJRcheck (fn, prepost, *retval, (void **) arg1, datasync);
    } else if (prepost == 1) {
        pop_kernel_state(fn);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

static int file_operations_fasync_MJRcheck (const char *fn,
                                            int prepost,
                                            int *retval,
                                            int *arg1,
                                            struct file **arg2,
                                            int *arg3) {
    check_routine_start();
    if (prepost == 0) {
        push_kernel_state(fn, BLOCKING_YES, USER_YES);
        // ep_fasync_fop_MJRcheck (fn, prepost, *retval, arg1, (void **) arg2, arg3);
    } else if (prepost == 1) {
        pop_kernel_state(fn);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

static int file_operations_lock_MJRcheck (const char *fn,
                                          int prepost,
                                          int *retval,
                                          struct file **arg1,
                                          int *arg2,
                                          struct file_lock **arg3) {
    check_routine_start();
    if (prepost == 0) {
        push_kernel_state(fn, BLOCKING_YES, USER_YES);
        // ep_lock_fop_MJRcheck (fn, prepost, *retval, (void **) arg1, arg2, (void **) arg3);
    } else if (prepost == 1) {
        pop_kernel_state(fn);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

static int file_operations_sendpage_MJRcheck (const char *fn,
                                              int prepost,
                                              ssize_t *retval,
                                              struct file **arg1,
                                              struct page **arg2,
                                              int *arg3,
                                              size_t *arg4,
                                              loff_t **arg5,
                                              int *arg6) {
    check_routine_start();
    if (prepost == 0) {
        push_kernel_state(fn, BLOCKING_YES, USER_YES);
        // ep_sendpage_fop_MJRcheck (fn, prepost, *retval, (void **) arg1, (void **) arg2,
        // arg3, arg4, arg5, arg6);
    } else if (prepost == 1) {
        pop_kernel_state(fn);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

static int file_operations_get_unmapped_area_MJRcheck (const char *fn,
                                                       int prepost,
                                                       unsigned long *retval,
                                                       struct file **arg1,
                                                       unsigned long *arg2,
                                                       unsigned long *arg3,
                                                       unsigned long *arg4,
                                                       unsigned long *arg5) {
    check_routine_start();
    if (prepost == 0) {
        push_kernel_state(fn, BLOCKING_YES, USER_YES);
        // ep_get_unmapped_area_fop_MJRcheck (fn, prepost, *retval, (void **) arg1,
        //                             arg2, arg3, arg4, arg5);
    } else if (prepost == 1) {
        pop_kernel_state(fn);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

static int file_operations_MJRcheck_flags_MJRcheck (const char *fn,
                                                    int prepost,
                                                    int *retval,
                                                    int *arg1) {
    check_routine_start();
    if (prepost == 0) {
        push_kernel_state(fn, BLOCKING_YES, USER_YES);
        // ep_MJRcheck_flags_fop_MJRcheck (fn, prepost, *retval, arg1);
    } else if (prepost == 1) {
        pop_kernel_state(fn);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

static int file_operations_flock_MJRcheck (const char *fn,
                                           int prepost,
                                           int *retval,
                                           struct file **arg1,
                                           int *arg2,
                                           struct file_lock **arg3) {
    check_routine_start();
    if (prepost == 0) {
        push_kernel_state(fn, BLOCKING_YES, USER_YES);
        // ep_flock_fop_MJRcheck (fn, prepost, *retval, (void **) arg1, arg2, (void **) arg3);
    } else if (prepost == 1) {
        pop_kernel_state(fn);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

static int file_operations_splice_write_MJRcheck (const char *fn,
                                                  int prepost,
                                                  ssize_t *retval,
                                                  struct pipe_inode_info **arg1,
                                                  struct file **arg2,
                                                  loff_t **arg3,
                                                  size_t *arg4,
                                                  unsigned int *arg5) {
    check_routine_start();
    if (prepost == 0) {
        push_kernel_state(fn, BLOCKING_YES, USER_YES);
        // ep_splice_write_fop_MJRcheck (fn, prepost, *retval, (void **) arg1, (void **) arg2,
        // arg3, arg4, arg5);
    } else if (prepost == 1) {
        pop_kernel_state(fn);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

static int file_operations_splice_read_MJRcheck (const char *fn,
                                                 int prepost,
                                                 ssize_t *retval,
                                                 struct file **arg1,
                                                 loff_t **arg2,
                                                 struct pipe_inode_info **arg3,
                                                 size_t *arg4,
                                                 unsigned int *arg5) {
    check_routine_start();
    if (prepost == 0) {
        push_kernel_state(fn, BLOCKING_YES, USER_YES);
        // ep_splice_read_fop_MJRcheck (fn, prepost, *retval, (void **) arg1, arg2,
        // (void **) arg3, arg4, arg5);
    } else if (prepost == 1) {
        pop_kernel_state(fn);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

static int file_operations_setlease_MJRcheck (const char *fn,
                                              int prepost,
                                              int *retval,
                                              struct file **arg1,
                                              long *arg2,
                                              struct file_lock ***arg3) {
    check_routine_start();
    if (prepost == 0) {
        push_kernel_state(fn, BLOCKING_YES, USER_YES);
        // ep_setlease_fop_MJRcheck (fn, prepost, *retval, (void **) arg1, arg2, (void ***) arg3);
    } else if (prepost == 1) {
        pop_kernel_state(fn);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

static int file_operations_fallocate_MJRcheck (const char *fn,
                                               int prepost,
                                               long *retval,
                                               struct file **arg1,
                                               int *arg2,
                                               loff_t *offset,
                                               loff_t *len) {
    check_routine_start();
    if (prepost == 0) {
        push_kernel_state(fn, BLOCKING_YES, USER_YES);
        // ep_setlease_fop_MJRcheck (fn, prepost, *retval, (void **) arg1, arg2, (void ***) arg3);
    } else if (prepost == 1) {
        pop_kernel_state(fn);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 0;
}

EXPORT_SYMBOL(file_operations_llseek_MJRcheck);
EXPORT_SYMBOL(file_operations_read_MJRcheck);
EXPORT_SYMBOL(file_operations_write_MJRcheck);
EXPORT_SYMBOL(file_operations_aio_read_MJRcheck);
EXPORT_SYMBOL(file_operations_aio_write_MJRcheck);
EXPORT_SYMBOL(file_operations_readdir_MJRcheck);
EXPORT_SYMBOL(file_operations_poll_MJRcheck);
EXPORT_SYMBOL(file_operations_unlocked_ioctl_MJRcheck);
EXPORT_SYMBOL(file_operations_compat_ioctl_MJRcheck);
EXPORT_SYMBOL(file_operations_mmap_MJRcheck);
EXPORT_SYMBOL(file_operations_open_MJRcheck);
EXPORT_SYMBOL(file_operations_flush_MJRcheck);
EXPORT_SYMBOL(file_operations_release_MJRcheck);
EXPORT_SYMBOL(file_operations_fsync_MJRcheck);
EXPORT_SYMBOL(file_operations_aio_fsync_MJRcheck);
EXPORT_SYMBOL(file_operations_fasync_MJRcheck);
EXPORT_SYMBOL(file_operations_lock_MJRcheck);
EXPORT_SYMBOL(file_operations_sendpage_MJRcheck);
EXPORT_SYMBOL(file_operations_get_unmapped_area_MJRcheck);
EXPORT_SYMBOL(file_operations_MJRcheck_flags_MJRcheck);
EXPORT_SYMBOL(file_operations_flock_MJRcheck);
EXPORT_SYMBOL(file_operations_splice_write_MJRcheck);
EXPORT_SYMBOL(file_operations_splice_read_MJRcheck);
EXPORT_SYMBOL(file_operations_setlease_MJRcheck);
EXPORT_SYMBOL(file_operations_fallocate_MJRcheck);

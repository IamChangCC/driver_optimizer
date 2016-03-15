#include "../test_state.h"
#include "../mega_header.h"

extern struct proto_ops_state_struct proto_ops_state;

//
// proto_ops structure - exported members
//

STATIC int proto_ops_ioctl_MJRcheck(const char *fn,
                                    int prepost,
                                    int *retval,
                                    struct socket **sock,
                                    unsigned int *cmd,
                                    unsigned long *arg) {
    check_routine_start();
    if (prepost == 0) {
    } else if (prepost == 1) {
        tfassert(retval != NULL);
        tfassert(cmd != NULL);
        ioctl_called_capable(fn, prepost, *retval, *cmd);
    } else {
        tfassert(0);
    }
    check_routine_end();
    return 0;
}

EXPORT_SYMBOL(proto_ops_ioctl_MJRcheck);

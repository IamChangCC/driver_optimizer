#include "testing_ep.h"
#include "globals.h"
#include "uprintk.h"

#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/ethtool.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>

static struct miscdevice ep_control;

static long ep_control_ioctl(struct file *fp,
                             unsigned int cmd,
                             unsigned long arg);

struct file_operations ep_control_fops = {
    .compat_ioctl = ep_control_ioctl,
    .unlocked_ioctl = ep_control_ioctl,
    .owner = THIS_MODULE
};

int initialize_ep_control(void) {
    // Initialize linux kernel junk
    ep_control.minor = EP_CONTROL_MINOR;
    ep_control.name = EP_CONTROL_NAME;
    ep_control.fops = &ep_control_fops;
    return misc_register(&ep_control);
}

void free_ep_control(void) {
    int retval;
    retval = misc_deregister(&ep_control);
    if (retval < 0) {
        uprintk ("misc_deregister failed. %d\n", retval);
    }
}

static long ep_control_ioctl(struct file *fp,
                             unsigned int cmd,
                             unsigned long arg) {
    int test_category = cmd;
    int test_to_run = (int) arg;

    uprintk ("Invoking %s with args %u/%lu ...\n", __func__, cmd, arg);
    switch (cmd) {
        case TEST_ETHTOOL:
            uprintk ("Ethtool.\n");
            ep_test(test_category, test_to_run);
            break;
        case TEST_PCI_PM:
            uprintk ("PCI PM\n");
            ep_test(test_category, test_to_run);
            break;
        case TEST_I2C_PM:
            uprintk ("I2C PM\n");
            ep_test(test_category, test_to_run);
            break;
        case TEST_SND:
        default:
            assert (0, "Snd not supported / no other values supported.\n");
    }
            
    uprintk ("Done with %s!\n", __func__);
    return 0;
}

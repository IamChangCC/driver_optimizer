#include "testing_ep.h"
#include "uprintk.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

static int ep_control_fd = -1;

//
// The idea behind this test is to have the test_framework
// call all the network ethtool functions with symbolic parameters.
// In this case, this test program has no role -- it's simply providing
// a notification.
//
static int test_ethtool(const char *param) {
    unsigned long arg = atoi (param);
    unsigned int cmd = TEST_ETHTOOL;
    fprintf (stderr, "Executing ethtool test %u: %s/%lu\n", cmd, param, arg);
    return ioctl (ep_control_fd, cmd, arg);
}

static int test_pci_pm(const char *param) {
    unsigned long arg = atoi (param);
    unsigned int cmd = TEST_PCI_PM;
    fprintf (stderr, "Executing PCI power management test %u: %s/%lu\n", cmd, param, arg);
    return ioctl (ep_control_fd, cmd, arg);
}

static int test_i2c_pm(const char *param) {
    unsigned long arg = atoi (param);
    unsigned int cmd = TEST_I2C_PM;
    fprintf (stderr, "Executing I2C power management test %u: %s/%lu\n", cmd, param, arg);
    return ioctl (ep_control_fd, cmd, arg);
}

//
// This test, in contrast to test_net, is very different.  Here, we notify
// the miscdevice to go into "symbolic entry point" mode, which means
// we want to supply a combination of symbolic and concrete parameters
// to the driver function.  For the sound driver, we want to invoke the various
// file operations.  This entails providing a concrete file object, and some
// symbolic data.
//
static int test_snd(const char *param) {
/*
    {
        int test = atoi (param);

        fprintf (stderr, "Executing test sound / %d\n", arg);

        // Put misc device in symbolic invocation mode.
        // Ignoring "test" parameter
        ioctl (ep_control_fd, TEST_SND, 0);
    }

    {
        int fd_control;
        int fd_pcm;
        int err;
        char buf[10];

        fd_control = open ("/dev/snd/controlC0", O_RDWR);
        fprintf (stderr, "Returned controlC0 open: %d %d\n", fd_control, errno);

        err = ioctl (fd_control, 0, 0);
        fprintf (stderr, "Returned controlC0 ioctl: %d %d\n", err, errno);

        err = read (fd_control, buf, 10);
        fprintf (stderr, "Returned controlC0 read: %d %d\n", err, errno);

        err = write (fd_control, buf, 10);
        fprintf (stderr, "Returned controlC0 write: %d %d\n", err, errno);

        fd_pcm = open ("/dev/snd/pcmC0D0c", O_RDWR);
        fprintf (stderr, "Returned pcmC0D0c open: %d %d\n", fd_pcm, errno);

        err = ioctl (fd_pcm, 0, 0);
        fprintf (stderr, "Returned pcmC0D0c ioctl: %d %d\n", err, errno);

        err = read (fd_pcm, buf, 10);
        fprintf (stderr, "Returned pcmC0D0c read: %d %d\n", err, errno);

        err = write (fd_pcm, buf, 10);
        fprintf (stderr, "Returned pcmC0D0c write: %d %d\n", err, errno);

        err = close (fd_pcm);
        fprintf (stderr, "Returned pcmC0D0c close: %d %d\n", err, errno);

        err = close (fd_control);
        fprintf (stderr, "Returned controlC0 close: %d %d\n", err, errno);
    }
*/
    return 0;
}



int main (int argc, char **argv) {
    int ret = 0;
    char ep_control_name[256];
    if (argc != 3) {
        fprintf (stderr, "Usage:\n");
        fprintf (stderr, "%s <ethtool|pci_pm|i2c_pm|snd> <test to run>\n", argv[0]);
        return 1;
    }

    sprintf (ep_control_name, "/dev/%s", EP_CONTROL_NAME);
    ep_control_fd = open(ep_control_name, O_RDWR);
    if (ep_control_fd == -1) {
        fprintf (stderr, "Failed to open test framework control\n");
        return 2;
    }

    if (strcmp (argv[1], "ethtool") == 0) {
        ret = test_ethtool(argv[2]);
    } else if (strcmp (argv[1], "pci_pm") == 0) {
        ret = test_pci_pm(argv[2]);
    } else if (strcmp (argv[1], "i2c_pm") == 0) {
        ret = test_i2c_pm(argv[2]);
    } else if (strcmp (argv[1], "snd") == 0) {
        ret = test_snd(argv[2]);
    } else {
        fprintf (stderr, "Failed:  specify snd or net\n");
    }

    if (ret != 0) {
        fprintf (stderr, "Error invoking ioctl: %d, errno: %d\n", ret, errno);
    }

    if (ep_control_fd != -1) {
        close (ep_control_fd);
    }

    return 0;
}

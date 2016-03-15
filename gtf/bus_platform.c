#include "globals.h"
#include "bus_generic_pci.h"
#include "bus_platform.h"

#include <linux/module.h>
#include <linux/vmalloc.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/pci.h>

#define SYMBOLIC_RESOURCE_NAME "symbolic_platform_res"

static void tf_platform_device_release (struct device *dev);
static void setup_platform_device (struct pci_dev *dev);

static struct platform_private {
    struct pci_dev *pdev;

    // Platform device
    struct resource tf_platform_resources[PCI_MAX_RESOURCES];
    struct platform_device tf_platform_device;
} platform_private;

//
// Not sure how to express this functionality as an S2E plugin since
// it's my understanding Linux platform devices are allocated statically
// as part of code -- they are not "discovered" in the same way as PnP
// PCI.
//

static void tf_platform_device_release (struct device *dev) {
}

static void setup_platform_device (struct pci_dev *dev) {
    int ret;
    int i;

    // Make sure a name is specified
    uprintk ("Step 1: strcmp.\n");
    if (strcmp (g_platform_name, "") == 0) {
        panic ("Need to specify a platform device name with g_platform_name\n");
    }

    // Make sure we don't screw anything up.
    uprintk ("Step 2: checking compatibility.\n");
    if (sizeof (resource_size_t) != sizeof (void *) ||
        sizeof (phys_addr_t) != sizeof(void *) ||
        sizeof (unsigned long) != sizeof(void *)) {
        panic ("32/64 compatibility\n");
    }

    if (g_platform_nr > PCI_MAX_RESOURCES) {
        panic ("Invalid number of resources: %d\n", g_platform_nr);
    }

// 037 #define IORESOURCE_TYPE_BITS    0x00001f00  /* Resource type */
// 038 #define IORESOURCE_IO       0x00000100
// 039 #define IORESOURCE_MEM      0x00000200
// 040 #define IORESOURCE_IRQ      0x00000400
// 041 #define IORESOURCE_DMA      0x00000800
// 042 #define IORESOURCE_BUS      0x00001000

    uprintk ("Step 3: Initializing platform resources...\n");
    for (i = 0; i < g_platform_nr; i++) {
        // Cast should be OK / see above if statement on 32/64
        memset(&platform_private.tf_platform_resources[i], 0, sizeof (struct resource));

        if (g_platform_rflags[i] & IORESOURCE_MEM) {
            platform_private.tf_platform_resources[i].start = pci_resource_start(platform_private.pdev, i);
            platform_private.tf_platform_resources[i].end = pci_resource_end(platform_private.pdev, i);;
            platform_private.tf_platform_resources[i].name = g_platform_rnames[i];
            platform_private.tf_platform_resources[i].flags = g_platform_rflags[i];
        } else if (g_platform_rflags[i] & IORESOURCE_IRQ) {
            // TODO Note we're only really supporting one IORESOURCE_IRQ here...
            // The smc91x driver contains code like this:
            // ndev->irq = ires->start;
            // so presumably the idea is that the start field of the resource
            // structure equals the IRQ if the flags field contains
            // IORESOURCE_IRQ
            // NOT EXACTLY SURE ABOUT THIS but it looks good and makes
            // some modest amount of sense.
            platform_private.tf_platform_resources[i].start = platform_private.pdev->irq;
            platform_private.tf_platform_resources[i].end = 0;
            platform_private.tf_platform_resources[i].name = g_platform_rnames[i];
            platform_private.tf_platform_resources[i].flags = g_platform_rflags[i];
        } else {
            panic ("How are we going to hack this flags in: %lu\n", g_platform_rflags[i]);
        }
    }

    // Set up fields:
    platform_private.tf_platform_device.name = g_platform_name;
    platform_private.tf_platform_device.id = -1;
    platform_private.tf_platform_device.num_resources = g_platform_nr;
    platform_private.tf_platform_device.resource = platform_private.tf_platform_resources;
    /* device model insists to have a release function */
    platform_private.tf_platform_device.dev.release = tf_platform_device_release;

    // Register platform device
    ret = platform_device_register(&platform_private.tf_platform_device);
    if (ret) {
        panic ("Could not register test framework platform device.\n");
    }
}

int initialize_platform_device(void) {
    struct pci_dev *pdev = NULL;
    if (g_platform_enable == 0) {
        return 0;
    }

    pdev = initialize_pci_device();

    if (platform_private.pdev != NULL) {
        panic ("%s: Misunderstanding.\n", __func__);
    }
    platform_private.pdev = pdev;

    setup_platform_device(pdev);
    return 0;
}

void free_platform_device (void) {
    if (g_platform_enable == 0) {
        return;
    }

    // Unregister platform device
    platform_device_unregister(&platform_private.tf_platform_device);
    free_pci_device(platform_private.pdev);

    if (platform_private.pdev == NULL) {
        panic ("%s: Misunderstanding\n", __func__);
    }
    platform_private.pdev = NULL;
}

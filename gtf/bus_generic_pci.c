#define DRV_NAME                "pci_platform_driver"
#define DRV_VERSION             "1.0"
#define DRV_RELDATE             "Dec 06, 2011"

#include "bus_generic_pci.h"

#include <linux/module.h>
#include <linux/pci.h>
#include <linux/uprintk.h>

static int pci_platform_probe (struct pci_dev *pdev, const struct pci_device_id *ent);
static void pci_platform_remove_one (struct pci_dev *pdev);

static struct pci_device_id pci_platform_tbl[] = {
    { PCI_DEVICE(0x9892, 0x9893), }, // MJR made these up (vendor ID / device ID)
    { 0, 0, 0, 0, 0, 0, 0 },
};
MODULE_DEVICE_TABLE(pci, pci_platform_tbl);

static struct pci_driver generic_pci_driver = {
    .name         = DRV_NAME,
    .id_table     = pci_platform_tbl,
    .probe        = pci_platform_probe,
    .remove       = pci_platform_remove_one,
};

static struct pci_dev *pci_cur_device = NULL;

#define PCI_PLATFORM_REGS_SIZE 0x100
#define PCI_PLATFORM_DMA_SIZE 0x1000

static int pci_platform_probe (struct pci_dev *pdev, const struct pci_device_id *ent) {
    int rc = 0;
    int i;
    struct pci_private *private;

    uprintk ("%s\n", __func__);

    // PCI platform private
    private = kzalloc (sizeof (struct pci_private), GFP_KERNEL);
    if (private == 0) {
        panic ("%s:  Failed to allocate memory\n", __func__);
    }

    if (pci_cur_device != NULL) {
        panic ("%s: Misunderstanding with respect to pci_cur_device\n", __func__);
    }

    pci_cur_device = pdev;

    rc = pci_enable_device(pdev);
    if (rc)
        panic ("%s: pci_enable_device failed %d\n", __func__, rc);

    // Store pointer to all the PCI device resources
    if (PCI_MAX_RESOURCES != 6 || PCI_MAX_RESOURCES != PCI_STD_RESOURCE_END + 1) {
        panic ("%s: %d, %d, %d\n", __func__, PCI_MAX_RESOURCES, PCI_STD_RESOURCE_END, 6);
    }

    for (i = 0; i < PCI_MAX_RESOURCES; i++) {
        if (pci_resource_start(pdev, i) != 0) {
            uprintk ("%s: PCI resource defined at %i\n", __func__, i);
            // Verify understanding
            if (pdev->resource[i].start != pci_resource_start(pdev, i)) {
                panic ("%s: %d != %d\n", __func__, pdev->resource[i].start, pci_resource_start(pdev, i));
            }
        } else {
            uprintk ("%s: no resource defined at index %d\n", i);
        }
    }

    // Fill private data structure
    // TODO anything else of interest here?
    private->pdev = pdev;

    // Associate the structure
    pci_set_drvdata(pdev, private);

    uprintk ("%s - successful completion\n", __func__);
    return 0;
}

static void pci_platform_remove_one (struct pci_dev *pdev)
{
    struct pci_private *private = pci_get_drvdata(pdev);

    if (pci_cur_device == NULL) {
        panic ("Misunderstanding in %s\n", __func__);
    }

    if (private != NULL) {
        uprintk ("%s\n", __func__);
        pci_disable_device(pdev);
        kfree(private);
        pci_set_drvdata(pdev, NULL);
    }

    pci_cur_device = NULL;
}

struct pci_dev *initialize_pci_device(void) {
    int ret;
    ret = pci_register_driver(&generic_pci_driver);
    if (ret != 0) {
        panic ("%s: pci_register_driver failed\n", __func__);
    }

    if (pci_cur_device == NULL) {
        panic ("%s: Misunderstanding pci_cur_device\n", __func__);
    }

    return pci_cur_device;
}

void free_pci_device (struct pci_dev *dev) {
    if (dev != pci_cur_device) {
        panic ("%s: Misunderstandig pci_cur_device != dev", __func__);
    }

    pci_unregister_driver(&generic_pci_driver);

    if (pci_cur_device != NULL) {
        panic ("%s: Misunderstanding pci_cur_device\n", __func__);
    }
}

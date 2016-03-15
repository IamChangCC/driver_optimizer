#ifndef BUS_GENERIC_PCI_H
#define BUS_GENERIC_PCI_H

#include <linux/pci.h>

#define PCI_MAX_RESOURCES 6

struct pci_dev *initialize_pci_device(void);
void free_pci_device(struct pci_dev *);

// Private data structure
struct pci_private {
    struct pci_dev *pdev;
};

#endif

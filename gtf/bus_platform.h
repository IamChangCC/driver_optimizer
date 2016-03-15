#ifndef TEST_FRAMEWORK_PLATFORM_DEVICE
#define TEST_FRAMEWORK_PLATFORM_DEVICE

#include "bus_generic_pci.h"
#include <linux/platform_device.h>

int initialize_platform_device (void);
void free_platform_device (void);

#endif

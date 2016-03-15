#ifndef TEST_FRAMEWORK_SPI_DEVICE
#define TEST_FRAMEWORK_SPI_DEVICE

#include <linux/spi/spi.h>

int initialize_spi_device (void);
void free_spi_device (void);

#endif

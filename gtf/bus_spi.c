#define DEBUG 1

#include "bus_generic_pci.h"
#include "bus_spi.h"
#include "globals.h"
#include "s2e.h"

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/spi/spi.h>
#include <linux/spi/spi_bitbang.h>

static struct symspi_private {
    struct spi_bitbang bitbang;
    struct pci_dev *pdev;

    struct spi_master *master;
    struct spi_board_info board_info;
    struct spi_device *spi_device;
} *spi_private;

//
// Can we extract these structures statically
// somehow?  Would require analysis of full kernel.
//
#if defined(DISABLE_TLE62X0_MJRCHECKS)
#include <linux/spi/tle62x0.h>
static struct tle62x0_pdata platform_data = {
    .init_state = 0,
    .gpio_count = 16
};
#else
static char platform_data[256] = { 0 };
#endif

static int symspi_setup(struct spi_device *spi) {
    
    uprintk ("Called %s\n", __func__);
    return 0;
}

static int symspi_setup_transfer(struct spi_device *spi,
                                 struct spi_transfer *t) {
    uprintk ("Called %s\n", __func__);
    return 0;
}

static void symspi_chipselect(struct spi_device *spi,
                              int is_active) {
    uprintk ("Called %s\n", __func__);
    return;
}

static int symspi_txrx_bufs(struct spi_device *spi, struct spi_transfer *t) {
    uprintk ("Called %s\n", __func__);
    if (t->rx_buf != NULL && t->len > 0) {
        s2e_make_symbolic(t->rx_buf, t->len, "spi_rx");
    }
    return t->len;
}

static u32 symspi_txrx_word(struct spi_device *spi,
                            unsigned nsecs,
                            u32 word, u8 bits) {
    uprintk ("Called %s\n", __func__);
    return 0;
}

static void setup_spi_devices(struct pci_dev *pdev) {
    struct spi_master *master;
    master = spi_alloc_master(&pdev->dev, sizeof (struct symspi_private));
    if (master == NULL) {
        panic ("%s: failed to allocate master", __func__);
    }

    spi_private = spi_master_get_devdata(master);
    spi_private->pdev = pdev;

    spi_private->master = master;
    spi_private->master->bus_num = -1;
    spi_private->master->num_chipselect = 16;
    spi_private->master->mode_bits = SPI_CPOL | SPI_CPHA | SPI_CS_HIGH;
    spi_private->master->setup = symspi_setup;

    /* setup the state for the bitbang driver */
    spi_private->bitbang.master = spi_master_get(spi_private->master);
    if (!spi_private->bitbang.master) {
        panic ("%s: spi_master_get failed", __func__);
    }
    spi_private->bitbang.setup_transfer = symspi_setup_transfer;
    spi_private->bitbang.chipselect = symspi_chipselect;
    spi_private->bitbang.txrx_bufs = symspi_txrx_bufs;
    spi_private->bitbang.txrx_word[SPI_MODE_0] = symspi_txrx_word;
    spi_private->bitbang.txrx_word[SPI_MODE_1] = symspi_txrx_word;
    spi_private->bitbang.txrx_word[SPI_MODE_2] = symspi_txrx_word;
    spi_private->bitbang.txrx_word[SPI_MODE_3] = symspi_txrx_word;
    
    if (spi_bitbang_start(&spi_private->bitbang)) {
        panic ("%s: failed to register master with spi_bitbang_start", __func__);
    }

    // We use the underlying PCI device's IRQ.
    strcpy (spi_private->board_info.modalias, g_spi_name);
    spi_private->board_info.platform_data = &platform_data;
    spi_private->board_info.controller_data = NULL;
    spi_private->board_info.irq = pdev->irq;
    spi_private->board_info.max_speed_hz = 10000000;
    spi_private->board_info.bus_num = spi_private->master->bus_num;
    spi_private->board_info.chip_select = 0;
    spi_private->board_info.mode = SPI_CS_HIGH;

    spi_private->spi_device = spi_new_device(spi_private->master, &spi_private->board_info);
    if (spi_private->spi_device == NULL) {
        panic ("%s failed to create spi_device", __func__);
    }
}

static struct pci_dev *cleanup_spi_device(void) {
    struct pci_dev *pdev = spi_private->pdev;
    spi_bitbang_stop(&spi_private->bitbang);
    spi_master_put(spi_private->master);
    return pdev;
}

int initialize_spi_device(void) {
    struct pci_dev *pdev = NULL;
    if (g_spi_enable == 0) {
        return 0;
    }

    pdev = initialize_pci_device();
    setup_spi_devices(pdev);
    return 0;
}

void free_spi_device(void) {
    struct pci_dev *pdev;
    if (g_spi_enable == 0) {
        return;
    }

    pdev = cleanup_spi_device();
    free_pci_device(pdev);
}

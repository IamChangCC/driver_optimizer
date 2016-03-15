/*
    i2c-stub.c - I2C/SMBus chip emulator
    Heavily modified by Matt Renzelmann

    Copyright (c) 2004 Mark M. Hoffman <mhoffman@lightlink.com>
    Copyright (C) 2007 Jean Delvare <khali@linux-fr.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#define DEBUG 1

#include "bus_generic_pci.h"
#include "bus_i2c.h"
#include "globals.h"
#include "s2e.h"

#if defined(DISABLE_ANDROID_CYTTSP_MJR_CHECKS)
#include "../android_cyttsp/cyttsp.h"
#endif

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/errno.h>

static struct i2c_private {
    struct pci_dev *pdev;

    // I2C devices
    struct i2c_board_info board_info[MAX_CHIPS];
    struct i2c_client *client[MAX_CHIPS];
} i2c_private;

//
// Can we extract these structures statically
// somehow?  Would require analysis of full kernel.
//
#if defined(DISABLE_ANDROID_A1026_MJRCHECKS)
//#warning Using custom platform data
static struct a1026_platform_data {
    uint32_t gpio_a1026_micsel;
    uint32_t gpio_a1026_wakeup;
    uint32_t gpio_a1026_reset;
    uint32_t gpio_a1026_int;
    uint32_t gpio_a1026_clk;
} platform_data = { .gpio_a1026_micsel = 111,
                    .gpio_a1026_wakeup = 113,
                    .gpio_a1026_reset = 129,
                    .gpio_a1026_clk = -1
};
#elif defined(DISABLE_ANDROID_CYTTSP_MJRCHECKS)
static int fluid_cyttsp_resume(struct i2c_client *client)
{
    mdelay(10);
    return CY_OK;
}

// Stolen from board-msm7x30.c
static struct cyttsp_platform_data {
    u32 panel_maxx;
    u32 panel_maxy;
    u32 disp_resx;
    u32 disp_resy;
    u32 disp_minx;
    u32 disp_miny;
    u32 disp_maxx;
    u32 disp_maxy;
    u32 flags;
    u8 gen;
    u8 use_st;
    u8 use_mt;
    u8 use_hndshk;
    u8 use_trk_id;
    u8 use_sleep;
    u8 use_gestures;
    u8 gest_set;
    u8 act_intrvl;
    u8 tch_tmout;
    u8 lp_intrvl;
    u8 power_state;
    bool wakeup;
    s32 (*init)(struct i2c_client *client);
    s32 (*resume)(struct i2c_client *client);
} platform_data = {
    .panel_maxx = 479,
    .panel_maxy = 799,
    .disp_maxx = 469,
    .disp_maxy = 799,
    .disp_minx = 10,
    .disp_miny = 0,
    .flags = 0,
    .gen = CY_GEN3, /* or */
    .use_st = CY_USE_ST,
    .use_mt = CY_USE_MT,
    .use_hndshk = CY_SEND_HNDSHK,
    .use_trk_id = CY_USE_TRACKING_ID,
    .use_sleep = CY_USE_SLEEP,
    .use_gestures = CY_USE_GESTURES,
        .gest_set = CY_GEST_GRP1 | CY_GEST_GRP2 |
                                CY_GEST_GRP3 | CY_GEST_GRP4 |
    CY_ACT_DIST,
    .act_intrvl = CY_ACT_INTRVL_DFLT,
    .tch_tmout = CY_TCH_TMOUT_DFLT,
    .lp_intrvl = CY_LP_INTRVL_DFLT,
    .resume = fluid_cyttsp_resume,
    .init = NULL,
};
#elif defined(DISABLE_LP5523_MJRCHECKS)
#include <linux/leds-lp5523.h>
static struct lp5523_led_config rx51_lp5523_led_config[] = {
    {
        .chan_nr    = 0,
        .led_current    = 50,
    }, {
        .chan_nr    = 1,
        .led_current    = 50,
    }, {
        .chan_nr    = 2,
        .led_current    = 50,
    }, {
        .chan_nr    = 3,
        .led_current    = 50,
    }, {
        .chan_nr    = 4,
        .led_current    = 50,
    }, {
        .chan_nr    = 5,
        .led_current    = 50,
    }, {
        .chan_nr    = 6,
        .led_current    = 50,
    }, {
        .chan_nr    = 7,
        .led_current    = 50,
    }, {
        .chan_nr    = 8,
        .led_current    = 50,
    }
};

static int rx51_lp5523_setup(void) {
    int retval;
    s2e_make_symbolic(&retval, sizeof(int), __func__);
    if (retval == 0) {
        retval = 0;
    } else {
        retval = -1;
    }
    return retval;
}
 
static void rx51_lp5523_release(void)
{
}
 
static void rx51_lp5523_enable(bool state)
{
}
 
// Adapted from arch/arm/mach-omap2/board-rx51-peripherals.c
static struct lp5523_platform_data platform_data = {
    .led_config     = rx51_lp5523_led_config,
    .num_channels       = ARRAY_SIZE(rx51_lp5523_led_config),
    .clock_mode     = LP5523_CLOCK_AUTO,
    .setup_resources    = rx51_lp5523_setup,
    .release_resources  = rx51_lp5523_release,
    .enable         = rx51_lp5523_enable,
};
#else
static char platform_data[256] = { 0 };
#endif

static void process_message (struct i2c_adapter *adap, struct i2c_msg *msg) {
    if (msg->flags & I2C_M_RD) {
        // Read command
        uprintk ("I2C read address 0x%X\n", msg->addr);

        // This is probably not quite right:
        if (msg->flags & I2C_M_RECV_LEN) {
            s2e_make_symbolic (msg->buf, msg->len + 1, "process_message");
            // First byte is length.  Just set it to max.
            msg->buf[0] = (u8)msg->len;
        } else {
            s2e_make_symbolic (msg->buf, msg->len, "process_message");
        }
    } else {
        // Write command
        uprintk ("I2C write address 0x%X\n", msg->addr);
    }
}

static int master_xfer(struct i2c_adapter *adap, struct i2c_msg *msgs, int num) {
    int i;
    int retval;

    s2e_make_symbolic(&retval, sizeof (int), "master_xfer");

    if (retval == num) {
        uprintk ("%s:  retval = num\n", __func__);
        for (i = 0; i < num; i++) {
            process_message (adap, &msgs[i]);
        }
    } else if (IS_ERR((void *) retval)) {
        uprintk ("%s:  retval = error\n", __func__);
    } else {
        uprintk ("%s:  retval = 0\n", __func__);
        retval = 0;
    }

    return retval;
}

/* Return negative errno on error. */
static s32 stub_xfer(struct i2c_adapter *adapter, u16 addr, unsigned short flags,
                     char read_write, u8 command, int protocol, union i2c_smbus_data *data)
{
    int i, len;
    int chip_index = -1;
    // Fork with an error
    {
        int ret;
        s2e_make_symbolic(&ret, sizeof (s32), "retval");
        if (IS_ERR_VALUE(ret))
            return -EINVAL;
    }

    /* Search for the right chip */
    uprintk ("In %s\n", __func__);
    for (i = 0; i < MAX_CHIPS && g_i2c_chip_addr[i]; i++) {
        if (addr == g_i2c_chip_addr[i]) {
            chip_index = i;
            break;
        }
    }
    if (chip_index == -1)
        return -ENODEV;

    switch (protocol) {
	case I2C_SMBUS_QUICK:
            uprintk("smbus quick - addr 0x%02x\n", addr);
            break;

	case I2C_SMBUS_BYTE:
            if (read_write == I2C_SMBUS_WRITE) {
                uprintk ("byte/write address 0x%02x\n", addr);
            } else {
                s2e_make_symbolic (&data->byte, sizeof (data->byte), "sym_I2C_SMBUS_BYTE");
                data->byte &= 0xFF;
                uprintk ("byte/read address 0x%02x\n", addr);
            }
            break;

	case I2C_SMBUS_BYTE_DATA:
            if (read_write == I2C_SMBUS_WRITE) {
                uprintk("byte_data/write - addr 0x%02x, "
                        "wrote at 0x%02x.\n",
                        addr, command);
            } else {
                s2e_make_symbolic (&data->byte, sizeof (data->byte), "sym_I2C_SMBUS_BYTE_DATA");
                data->byte &= 0xFF;
                uprintk("byte_data/read - addr 0x%02x, "
                        "read at 0x%02x.\n",
                        addr, command);
            }
            break;

	case I2C_SMBUS_WORD_DATA:
            if (read_write == I2C_SMBUS_WRITE) {
                uprintk("word_data/write - addr 0x%02x, "
                        "wrote at 0x%02x.\n",
                        addr, command);
            } else {
                s2e_make_symbolic (&data->word, sizeof (data->word), "sym_I2C_SMBUS_WORD_DATA");
                uprintk("word_data/read - addr 0x%02x, "
                        "read at 0x%02x.\n",
                        addr, command);
            }
            break;

        case I2C_SMBUS_PROC_CALL:
            // Not sure how this one works
            // It looks like it sends some data to the device and gets back 16 bits.
            uprintk ("proc_call: addr 0x%02x, "
                     "at 0x%02x.\n",
                     addr, command);
            s2e_make_symbolic(&data->word, sizeof (data->word), "sym_I2C_SMBUS_PROC_CALL");
            break;

	case I2C_SMBUS_I2C_BLOCK_DATA:
            len = data->block[0];
            if (read_write == I2C_SMBUS_WRITE) {
                uprintk("block_data/write - addr 0x%02x, "
                        "wrote %d bytes at 0x%02x.\n",
                        addr, len, command);
            } else {
                for (i = 0; i < len; i++) {
                    s2e_make_symbolic(&data->block[1 + i],
                                      sizeof (data->block[1 + i]),
                                      "sym_I2C_SMBUS_I2C_BLOCK_DATA");
                    data->block[1 + i] &= 0xff;
                }
                uprintk("block_data/read - addr 0x%02x, "
                        "read %d bytes at 0x%02x.\n",
                        addr, len, command);
            }
            break;

        case I2C_SMBUS_I2C_BLOCK_BROKEN:
            panic ("Matt - finish I2C Support for I2C_SMBUS_I2C_BLOCK_BROKEN");
        case I2C_SMBUS_BLOCK_PROC_CALL:
            panic ("Matt - finish I2C Support for I2C_SMBUS_BLOCK_PROC_CALL");
        case I2C_SMBUS_BLOCK_DATA:
            panic ("Matt - finish I2C Support for I2C_SMBUS_BLOCK_DATA");
        default:
            panic ("Unsupported I2C/SMBus command: %d\n", protocol);
            break;
    }

    return 0;
}

static u32 stub_func(struct i2c_adapter *adapter)
{
    return STUB_FUNC & g_i2c_functionality;
}

static const struct i2c_algorithm combined_algorithm = {
    .functionality	= stub_func,
    .master_xfer        = master_xfer,
    .smbus_xfer	        = stub_xfer,
};

static struct i2c_adapter stub_adapter = {
    .owner		= THIS_MODULE,
    .class		= 0,
    .algo		= &combined_algorithm,
    .name		= "stub_adapter",
};

static int cleanup_devices (void) {
    int ret, i;

    ret = 0;
    for (i = 0; i < MAX_CHIPS && g_i2c_chip_addr[i]; i++) {
        if (i2c_private.client[i]) {
            uprintk ("Removing device %d/%x...\n", i, g_i2c_chip_addr[i]);
            i2c_unregister_device(i2c_private.client[i]);
            i2c_private.client[i] = NULL;
        }
    }

    uprintk ("%s: return value %d\n", __func__, ret);
    return ret;
}

static int alloc_chips (void) {
    int i;
    for (i = 0; i < MAX_CHIPS && g_i2c_chip_addr[i]; i++) {
        if (g_i2c_chip_addr[i] < 0x03 || g_i2c_chip_addr[i] > 0x77) {
            uprintk(KERN_ERR "i2c-stub: Invalid chip address "
                    "0x%02x\n", g_i2c_chip_addr[i]);
            return -EINVAL;
        }

        uprintk(KERN_INFO "i2c-stub: Virtual chip at 0x%02x\n",
               g_i2c_chip_addr[i]);
    }

    return 0;
}

static int alloc_devices (void) {
    int ret, i;

    if (i2c_private.pdev == NULL) {
        panic ("Misunderstanding in %s\n", __func__);
    }

    ret = 0;
    for (i = 0; i < MAX_CHIPS && g_i2c_chip_addr[i]; i++) {
        uprintk ("Allocating device %d/%x (null platform data)...", i, g_i2c_chip_addr[i]);
        strcpy (i2c_private.board_info[i].type, g_i2c_names[i]);
        i2c_private.board_info[i].addr = g_i2c_chip_addr[i];
        i2c_private.board_info[i].platform_data = &platform_data;
        i2c_private.board_info[i].irq = i2c_private.pdev->irq;

        i2c_private.client[i] = i2c_new_device(&stub_adapter, &i2c_private.board_info[i]);
        if (i2c_private.client[i] == NULL) {
            cleanup_devices ();
            break;
        }
    }

    uprintk ("%s: return value %d\n", __func__, ret);
    return ret;
}

static void setup_i2c_devices(struct pci_dev *pdev) {
    int ret;

    if (!g_i2c_chip_addr[0]) {
        panic("i2c-stub: Please specify a chip address\n");
    }

    ret = alloc_chips();
    if (ret) {
        panic ("alloc_chips failed\n");
    }

    ret = i2c_add_adapter(&stub_adapter);
    if (ret) {
        panic ("i2c_add_adapter failed");
    }

    ret = alloc_devices();
    if (ret) {
        panic ("alloc_devices failed");
    }
}

int initialize_i2c_device(void) {
    struct pci_dev *pdev = NULL;
    if (g_i2c_enable == 0) {
        return 0;
    }

    pdev = initialize_pci_device();

    if (i2c_private.pdev != NULL) {
        panic ("%s: Misunderstanding.\n", __func__);
    }
    i2c_private.pdev = pdev;

    setup_i2c_devices(pdev);
    return 0;
}

void free_i2c_device(void) {
    if (g_i2c_enable == 0) {
        return;
    }

    cleanup_devices();
    i2c_del_adapter(&stub_adapter);
    free_pci_device(i2c_private.pdev);

    if (i2c_private.pdev == NULL) {
        panic ("%s: Misunderstanding\n", __func__);
    }
    i2c_private.pdev = NULL;
}

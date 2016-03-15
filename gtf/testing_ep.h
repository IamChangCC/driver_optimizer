#ifndef TESTING_EP_H
#define TESTING_EP_H

#ifdef LINUX_MODE
#include <linux/ioctl.h>

// Not used as of 3.1.1
#define TEST_MAGIC 'x'

enum symdrive_TEST_CATEGORY {
    TEST_ETHTOOL = _IOR(TEST_MAGIC, 0, int),
    TEST_PCI_PM  = _IOR(TEST_MAGIC, 1, int),
    TEST_I2C_PM  = _IOR(TEST_MAGIC, 2, int),
    TEST_SND     = _IOR(TEST_MAGIC, 3, int),
};

struct pci_driver;
struct pci_dev;
struct net_device;
struct file;

enum symdrive_TEST_CASE_ETHTOOL {
    TEST_CASE_ETHTOOL_BEGIN = 0,
    ETHTOOL_GET_SETTINGS,
    ETHTOOL_SET_SETTINGS,
    ETHTOOL_GET_DRVINFO,
    ETHTOOL_GET_REGS_LEN,
    ETHTOOL_GET_REGS,
    ETHTOOL_GET_WOL,
    ETHTOOL_SET_WOL,
    ETHTOOL_GET_MSGLEVEL,
    ETHTOOL_SET_MSGLEVEL,
    ETHTOOL_NWAY_RESET,
    ETHTOOL_GET_LINK,
    ETHTOOL_GET_EEPROM_LEN,
    ETHTOOL_GET_EEPROM,
    ETHTOOL_SET_EEPROM,
    ETHTOOL_GET_COALESCE,
    ETHTOOL_SET_COALESCE,
    ETHTOOL_GET_RINGPARAM,
    ETHTOOL_SET_RINGPARAM,
    ETHTOOL_GET_PAUSEPARAM,
    ETHTOOL_SET_PAUSEPARAM,
    ETHTOOL_GET_RX_CSUM,
    ETHTOOL_SET_RX_CSUM,
    ETHTOOL_GET_TX_CSUM,
    ETHTOOL_SET_TX_CSUM,
    ETHTOOL_GET_SG,
    ETHTOOL_SET_SG,
    ETHTOOL_GET_TSO,
    ETHTOOL_SET_TSO,
    ETHTOOL_SELF_TEST,
    ETHTOOL_GET_STRINGS,
    ETHTOOL_PHYS_ID,
    ETHTOOL_GET_ETHTOOL_STATS,
    ETHTOOL_BEGIN,
    ETHTOOL_COMPLETE,
    ETHTOOL_GET_UFO,
    ETHTOOL_SET_UFO,
    ETHTOOL_GET_FLAGS,
    ETHTOOL_SET_FLAGS,
    ETHTOOL_GET_PRIV_FLAGS,
    ETHTOOL_SET_PRIV_FLAGS,
    ETHTOOL_GET_SSET_COUNT,
    
    // Obsolete
    ETHTOOL_SELF_TEST_COUNT,
    ETHTOOL_GET_STATS_COUNT,
    ETHTOOL_GET_RXHASH,
    ETHTOOL_SET_RXHASH,

    TEST_CASE_ETHTOOL_END
};

enum symdrive_TEST_CASE_PCI_PM {
    TEST_CASE_PCI_PM_BEGIN = 0,
    PCI_PM_SUSPEND,
    PCI_PM_RESUME,
    TEST_CASE_PCI_PM_END
};

enum symdrive_TEST_CASE_I2C_PM {
    TEST_CASE_I2C_PM_BEGIN = 0,
    I2C_PM_SUSPEND,
    I2C_PM_RESUME,
    TEST_CASE_I2C_PM_END
};

int initialize_ep (void);
void ep_test (enum symdrive_TEST_CATEGORY category, int fn_id);

// Initialization for ep_state_struct:
struct net_device;
struct pci_driver;
struct pci_dev;
struct i2c_driver;
struct i2c_client;
void ep_init_net (struct net_device *net_dev);
void ep_init_pci_driver (struct pci_driver *pci_drv);
void ep_init_pci_dev (struct pci_dev *pci_dev);
void ep_init_i2c_driver (struct i2c_driver *i2c_drv);
void ep_init_i2c_client (struct i2c_client *i2c_client);

//void ep_symexec_set(int new_value);

// IOCTL / miscdevice initialization
int initialize_ep_control(void);
void free_ep_control(void);

#define EP_CONTROL_MINOR 45
#define EP_CONTROL_NAME "ep_control"

#endif // LINUX_MODE
#endif


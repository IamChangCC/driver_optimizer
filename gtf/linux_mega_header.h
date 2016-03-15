#ifndef LINUX_MEGA_HEADER
#define LINUX_MEGA_HEADER


#include <linux/user.h>
#include <linux/types.h>
#include <linux/device.h>
#include <linux/netdevice.h>
#include <linux/ethtool.h>
#include <linux/usb.h>
#include <linux/pci.h>
#include <linux/miscdevice.h>
#include <linux/phy.h>
#include <linux/firmware.h>
#include <net/sock.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/spi/spi.h>
#include <linux/gpio.h>

#include <sound/pcm.h>
#include <sound/core.h>
#include <sound/info.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/initval.h>
#include <sound/control.h>
#include <sound/rawmidi.h>
#include <sound/hwdep.h>

#define STATIC
//#define STATIC static

struct rtl8139_private;
struct akm8975_data;
struct ks8851_net;
struct a1026img;

STATIC int cleanup_module_MJRcheck(char const   *arg0 , int arg1 ) ;
STATIC int init_module_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ) ;
STATIC int rtl8139_cleanup_module_MJRcheck(char const   *arg0 , int arg1 ) ;
STATIC int rtl8139_init_module_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ) ;
STATIC int rtl8139_resume_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct pci_dev **arg3 ) ;
STATIC int rtl8139_suspend_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct pci_dev **arg3 ,
                                    pm_message_t *arg4 ) ;
STATIC int rtl8139_set_rx_mode_MJRcheck(char const   *arg0 , int arg1 , struct net_device **arg2 ) ;
STATIC int __set_rx_mode_MJRcheck(char const   *arg0 , int arg1 , struct net_device **arg2 ) ;
STATIC int rtl8139_get_stats_MJRcheck(char const   *arg0 , int arg1 , struct net_device_stats **arg2 ,
                                      struct net_device **arg3 ) ;
STATIC int netdev_ioctl_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct net_device **arg3 ,
                                 struct ifreq **arg4 , int *arg5 ) ;
STATIC int rtl8139_get_strings_MJRcheck(char const   *arg0 , int arg1 , struct net_device **arg2 ,
                                        u32 *arg3 , u8 **arg4 ) ;
STATIC int rtl8139_get_ethtool_stats_MJRcheck(char const   *arg0 , int arg1 , struct net_device **arg2 ,
                                              struct ethtool_stats **arg3 , u64 **arg4 ) ;
STATIC int rtl8139_get_sset_count_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                           struct net_device **arg3 , int *arg4 ) ;
STATIC int rtl8139_get_regs_MJRcheck(char const   *arg0 , int arg1 , struct net_device **arg2 ,
                                     struct ethtool_regs **arg3 , void **arg4 ) ;
STATIC int rtl8139_get_regs_len_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                         struct net_device **arg3 ) ;
STATIC int rtl8139_set_msglevel_MJRcheck(char const   *arg0 , int arg1 , struct net_device **arg2 ,
                                         u32 *arg3 ) ;
STATIC int rtl8139_get_msglevel_MJRcheck(char const   *arg0 , int arg1 , u32 *arg2 ,
                                         struct net_device **arg3 ) ;
STATIC int rtl8139_get_link_MJRcheck(char const   *arg0 , int arg1 , u32 *arg2 , struct net_device **arg3 ) ;
STATIC int rtl8139_nway_reset_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                       struct net_device **arg3 ) ;
STATIC int rtl8139_set_settings_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                         struct net_device **arg3 , struct ethtool_cmd **arg4 ) ;
STATIC int rtl8139_get_settings_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                         struct net_device **arg3 , struct ethtool_cmd **arg4 ) ;
STATIC int rtl8139_get_drvinfo_MJRcheck(char const   *arg0 , int arg1 , struct net_device **arg2 ,
                                        struct ethtool_drvinfo **arg3 ) ;
STATIC int rtl8139_set_wol_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct net_device **arg3 ,
                                    struct ethtool_wolinfo **arg4 ) ;
STATIC int rtl8139_get_wol_MJRcheck(char const   *arg0 , int arg1 , struct net_device **arg2 ,
                                    struct ethtool_wolinfo **arg3 ) ;
STATIC int rtl8139_close_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct net_device **arg3 ) ;
STATIC int rtl8139_set_mac_address_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                            struct net_device **arg3 , void **arg4 ) ;
STATIC int rtl8139_poll_controller_MJRcheck(char const   *arg0 , int arg1 , struct net_device **arg2 ) ;
STATIC int rtl8139_interrupt_MJRcheck(char const   *arg0 , int arg1 , irqreturn_t *arg2 ,
                                      int *arg3 , void **arg4 ) ;
STATIC int rtl8139_poll_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct napi_struct **arg3 ,
                                 int *arg4 ) ;
STATIC int rtl8139_weird_interrupt_MJRcheck(char const   *arg0 , int arg1 , struct net_device **arg2 ,
                                            struct rtl8139_private **arg3 , void **arg4 ,
                                            int *arg5 , int *arg6 ) ;
STATIC int rtl8139_rx_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct net_device **arg3 ,
                               struct rtl8139_private **arg4 , int *arg5 ) ;
STATIC int rtl8139_isr_ack_MJRcheck(char const   *arg0 , int arg1 , struct rtl8139_private **arg2 ) ;
STATIC int rtl8139_rx_err_MJRcheck(char const   *arg0 , int arg1 , u32 *arg2 , struct net_device **arg3 ,
                                   struct rtl8139_private **arg4 , void **arg5 ) ;
STATIC int rtl8139_tx_interrupt_MJRcheck(char const   *arg0 , int arg1 , struct net_device **arg2 ,
                                         struct rtl8139_private **arg3 , void **arg4 ) ;
STATIC int rtl8139_start_xmit_MJRcheck(char const   *arg0 , int arg1 , netdev_tx_t *arg2 ,
                                       struct sk_buff **arg3 , struct net_device **arg4 ) ;
STATIC int rtl8139_tx_timeout_MJRcheck(char const   *arg0 , int arg1 , struct net_device **arg2 ) ;
STATIC int rtl8139_tx_timeout_task_MJRcheck(char const   *arg0 , int arg1 , struct work_struct **arg2 ) ;
STATIC int rtl8139_tx_clear_MJRcheck(char const   *arg0 , int arg1 , struct rtl8139_private **arg2 ) ;
STATIC int rtl8139_start_thread_MJRcheck(char const   *arg0 , int arg1 , struct rtl8139_private **arg2 ) ;
STATIC int rtl8139_thread_MJRcheck(char const   *arg0 , int arg1 , struct work_struct **arg2 ) ;
STATIC int rtl8139_thread_iter_MJRcheck(char const   *arg0 , int arg1 , struct net_device **arg2 ,
                                        struct rtl8139_private **arg3 , void **arg4 ) ;
STATIC int rtl8139_tune_twister_MJRcheck(char const   *arg0 , int arg1 , struct net_device **arg2 ,
                                         struct rtl8139_private **arg3 ) ;
STATIC int rtl8139_init_ring_MJRcheck(char const   *arg0 , int arg1 , struct net_device **arg2 ) ;
STATIC int rtl8139_hw_start_MJRcheck(char const   *arg0 , int arg1 , struct net_device **arg2 ) ;
STATIC int rtl_check_media_MJRcheck(char const   *arg0 , int arg1 , struct net_device **arg2 ,
                                    unsigned int *arg3 ) ;
STATIC int rtl8139_open_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct net_device **arg3 ) ;
STATIC int mdio_write_MJRcheck(char const   *arg0 , int arg1 , struct net_device **arg2 ,
                               int *arg3 , int *arg4 , int *arg5 ) ;
STATIC int mdio_read_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct net_device **arg3 ,
                              int *arg4 , int *arg5 ) ;
STATIC int mdio_sync_MJRcheck(char const   *arg0 , int arg1 , void **arg2 ) ;
STATIC int read_eeprom_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , void **arg3 ,
                                int *arg4 , int *arg5 ) ;
STATIC int rtl8139_remove_one_MJRcheck(char const   *arg0 , int arg1 , struct pci_dev **arg2 ) ;
STATIC int rtl8139_init_one_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct pci_dev **arg3 ,
                                     struct pci_device_id  const  **arg4 ) ;
STATIC int rtl8139_init_board_MJRcheck(char const   *arg0 , int arg1 , struct net_device **arg2 ,
                                       struct pci_dev **arg3 ) ;
STATIC int rtl8139_chip_reset_MJRcheck(char const   *arg0 , int arg1 , void **arg2 ) ;
STATIC int __rtl8139_cleanup_dev_MJRcheck(char const   *arg0 , int arg1 , struct net_device **arg2 ) ;
STATIC int if_mii_MJRcheck(char const   *arg0 , int arg1 , struct mii_ioctl_data **arg2 ,
                           struct ifreq **arg3 ) ;
STATIC int is_valid_ether_addr_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                        u8 const   **arg3 ) ;
STATIC int is_multicast_ether_addr_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                            u8 const   **arg3 ) ;
STATIC int is_zero_ether_addr_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                       u8 const   **arg3 ) ;
STATIC int netdev_name_MJRcheck(char const   *arg0 , int arg1 , char const   **arg2 ,
                                struct net_device  const  **arg3 ) ;
STATIC int netif_running_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct net_device  const  **arg3 ) ;
STATIC int netif_stop_queue_MJRcheck(char const   *arg0 , int arg1 , struct net_device **arg2 ) ;
STATIC int netif_tx_stop_queue_MJRcheck(char const   *arg0 , int arg1 , struct netdev_queue **arg2 ) ;
STATIC int netif_wake_queue_MJRcheck(char const   *arg0 , int arg1 , struct net_device **arg2 ) ;
STATIC int netif_tx_wake_queue_MJRcheck(char const   *arg0 , int arg1 , struct netdev_queue **arg2 ) ;
STATIC int netif_start_queue_MJRcheck(char const   *arg0 , int arg1 , struct net_device **arg2 ) ;
STATIC int netif_tx_start_queue_MJRcheck(char const   *arg0 , int arg1 , struct netdev_queue **arg2 ) ;
STATIC int netdev_priv_MJRcheck(char const   *arg0 , int arg1 , void **arg2 , struct net_device  const  **arg3 ) ;
STATIC int netdev_get_tx_queue_MJRcheck(char const   *arg0 , int arg1 , struct netdev_queue **arg2 ,
                                        struct net_device  const  **arg3 , unsigned int *arg4 ) ;
STATIC int napi_enable_MJRcheck(char const   *arg0 , int arg1 , struct napi_struct **arg2 ) ;
STATIC int napi_disable_MJRcheck(char const   *arg0 , int arg1 , struct napi_struct **arg2 ) ;
STATIC int napi_schedule_prep_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                       struct napi_struct **arg3 ) ;
STATIC int napi_disable_pending_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                         struct napi_struct **arg3 ) ;
STATIC int skb_copy_to_linear_data_MJRcheck(char const   *arg0 , int arg1 , struct sk_buff **arg2 ,
                                            void const   **arg3 , unsigned int *arg4 ) ;
STATIC int netdev_alloc_skb_ip_align_MJRcheck(char const   *arg0 , int arg1 , struct sk_buff **arg2 ,
                                              struct net_device **arg3 , unsigned int *arg4 ) ;
STATIC int __netdev_alloc_skb_ip_align_MJRcheck(char const   *arg0 , int arg1 , struct sk_buff **arg2 ,
                                                struct net_device **arg3 , unsigned int *arg4 ,
                                                gfp_t *arg5 ) ;
STATIC int request_irq_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , unsigned int *arg3 ,
                                irqreturn_t (**arg4)(int  , void * ) , unsigned long *arg5 ,
                                char const   **arg6 , void **arg7 ) ;
STATIC int pci_name_MJRcheck(char const   *arg0 , int arg1 , char const   **arg2 ,
                             struct pci_dev  const  **arg3 ) ;
STATIC int pci_set_drvdata_MJRcheck(char const   *arg0 , int arg1 , struct pci_dev **arg2 ,
                                    void **arg3 ) ;
STATIC int pci_get_drvdata_MJRcheck(char const   *arg0 , int arg1 , void **arg2 ,
                                    struct pci_dev **arg3 ) ;
STATIC int dma_free_coherent_MJRcheck(char const   *arg0 , int arg1 , struct device **arg2 ,
                                      size_t *arg3 , void **arg4 , dma_addr_t *arg5 ) ;
STATIC int dma_alloc_coherent_MJRcheck(char const   *arg0 , int arg1 , void **arg2 ,
                                       struct device **arg3 , size_t *arg4 , dma_addr_t **arg5 ,
                                       gfp_t *arg6 ) ;
STATIC int dma_alloc_coherent_gfp_flags_MJRcheck(char const   *arg0 , int arg1 , gfp_t *arg2 ,
                                                 struct device **arg3 , gfp_t *arg4 ) ;
STATIC int dma_alloc_coherent_mask_MJRcheck(char const   *arg0 , int arg1 , unsigned long *arg2 ,
                                            struct device **arg3 , gfp_t *arg4 ) ;
STATIC int get_dma_ops_MJRcheck(char const   *arg0 , int arg1 , struct dma_map_ops **arg2 ,
                                struct device **arg3 ) ;
STATIC int debug_dma_free_coherent_MJRcheck(char const   *arg0 , int arg1 , struct device **arg2 ,
                                            size_t *arg3 , void **arg4 , dma_addr_t *arg5 ) ;
STATIC int debug_dma_alloc_coherent_MJRcheck(char const   *arg0 , int arg1 , struct device **arg2 ,
                                             size_t *arg3 , dma_addr_t *arg4 , void **arg5 ) ;
STATIC int is_device_dma_capable_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                          struct device **arg3 ) ;
STATIC int pci_write_config_word_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                          struct pci_dev **arg3 , int *arg4 , u16 *arg5 ) ;
STATIC int pci_read_config_word_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                         struct pci_dev **arg3 , int *arg4 , u16 **arg5 ) ;
STATIC int dev_name_MJRcheck(char const   *arg0 , int arg1 , char const   **arg2 ,
                             struct device  const  **arg3 ) ;
STATIC int kobject_name_MJRcheck(char const   *arg0 , int arg1 , char const   **arg2 ,
                                 struct kobject  const  **arg3 ) ;
STATIC int memcpy_fromio_MJRcheck(char const   *arg0 , int arg1 , void **arg2 , void const volatile   **arg3 ,
                                  size_t *arg4 ) ;
STATIC int __init_work_MJRcheck(char const   *arg0 , int arg1 , struct work_struct **arg2 ,
                                int *arg3 ) ;
STATIC int spin_unlock_irqrestore_MJRcheck(char const   *arg0 , int arg1 , spinlock_t **arg2 ,
                                           unsigned long *arg3 ) ;
STATIC int spin_unlock_irq_MJRcheck(char const   *arg0 , int arg1 , spinlock_t **arg2 ) ;
STATIC int spin_unlock_bh_MJRcheck(char const   *arg0 , int arg1 , spinlock_t **arg2 ) ;
STATIC int spin_unlock_MJRcheck(char const   *arg0 , int arg1 , spinlock_t **arg2 ) ;
STATIC int spin_lock_irq_MJRcheck(char const   *arg0 , int arg1 , spinlock_t **arg2 ) ;
STATIC int spin_lock_bh_MJRcheck(char const   *arg0 , int arg1 , spinlock_t **arg2 ) ;
STATIC int spin_lock_MJRcheck(char const   *arg0 , int arg1 , spinlock_t **arg2 ) ;
STATIC int spinlock_check_MJRcheck(char const   *arg0 , int arg1 , raw_spinlock_t **arg2 ,
                                   spinlock_t **arg3 ) ;
STATIC int IS_ERR_MJRcheck(char const   *arg0 , int arg1 , long *arg2 , void const   **arg3 ) ;
STATIC int PTR_ERR_MJRcheck(char const   *arg0 , int arg1 , long *arg2 , void const   **arg3 ) ;
STATIC int ERR_PTR_MJRcheck(char const   *arg0 , int arg1 , void **arg2 , long *arg3 ) ;
STATIC int arch_irqs_disabled_flags_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                             unsigned long *arg3 ) ;
STATIC int arch_local_save_flags_MJRcheck(char const   *arg0 , int arg1 , unsigned long *arg2 ) ;
STATIC int get_order_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , unsigned long *arg3 ) ;
STATIC int constant_test_bit_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                      unsigned int *arg3 , unsigned long const volatile   **arg4 ) ;
STATIC int test_and_clear_bit_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                       int *arg3 , unsigned long volatile   **arg4 ) ;
STATIC int test_and_set_bit_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , int *arg3 ,
                                     unsigned long volatile   **arg4 ) ;
STATIC int clear_bit_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , unsigned long volatile   **arg3 ) ;
STATIC int set_bit_MJRcheck(char const   *arg0 , int arg1 , unsigned int *arg2 , unsigned long volatile   **arg3 ) ;
STATIC int INIT_LIST_HEAD_MJRcheck(char const   *arg0 , int arg1 , struct list_head **arg2 ) ;
STATIC int __builtin_expect_MJRcheck(char const   *arg0 , int arg1 , long *arg2 ,
                                     long *arg3 , long *arg4 ) ;
STATIC int __const_udelay_MJRcheck(char const   *arg0 , int arg1 , unsigned long *arg2 ) ;
STATIC int __dynamic_pr_debug_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                       struct _ddebug **arg3 , char const   **arg4 ) ;
STATIC int __napi_complete_MJRcheck(char const   *arg0 , int arg1 , struct napi_struct **arg2 ) ;
STATIC int __napi_schedule_MJRcheck(char const   *arg0 , int arg1 , struct napi_struct **arg2 ) ;
STATIC int __netdev_alloc_skb_MJRcheck(char const   *arg0 , int arg1 , struct sk_buff **arg2 ,
                                       struct net_device **arg3 , unsigned int *arg4 ,
                                       gfp_t *arg5 ) ;
STATIC int __netif_schedule_MJRcheck(char const   *arg0 , int arg1 , struct Qdisc **arg2 ) ;
STATIC int __pci_register_driver_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                          struct pci_driver **arg3 , struct module **arg4 ,
                                          char const   **arg5 ) ;
STATIC int __raw_spin_lock_init_MJRcheck(char const   *arg0 , int arg1 , raw_spinlock_t **arg2 ,
                                         char const   **arg3 , struct lock_class_key **arg4 ) ;
STATIC int _dev_info_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct device  const  **arg3 ,
                              char const   **arg4 ) ;
STATIC int _raw_spin_lock_MJRcheck(char const   *arg0 , int arg1 , raw_spinlock_t **arg2 ) ;
STATIC int _raw_spin_lock_bh_MJRcheck(char const   *arg0 , int arg1 , raw_spinlock_t **arg2 ) ;
STATIC int _raw_spin_lock_irq_MJRcheck(char const   *arg0 , int arg1 , raw_spinlock_t **arg2 ) ;
STATIC int _raw_spin_lock_irqsave_MJRcheck(char const   *arg0 , int arg1 , unsigned long *arg2 ,
                                           raw_spinlock_t **arg3 ) ;
STATIC int _raw_spin_unlock_MJRcheck(char const   *arg0 , int arg1 , raw_spinlock_t **arg2 ) ;
STATIC int _raw_spin_unlock_bh_MJRcheck(char const   *arg0 , int arg1 , raw_spinlock_t **arg2 ) ;
STATIC int _raw_spin_unlock_irq_MJRcheck(char const   *arg0 , int arg1 , raw_spinlock_t **arg2 ) ;
STATIC int _raw_spin_unlock_irqrestore_MJRcheck(char const   *arg0 , int arg1 , raw_spinlock_t **arg2 ,
                                                unsigned long *arg3 ) ;
STATIC int alloc_etherdev_mqs_MJRcheck(char const   *arg0 , int arg1 , struct net_device **arg2 ,
                                       int *arg3 , unsigned int *arg4 , unsigned int *arg5 ) ;
STATIC int bitrev32_MJRcheck(char const   *arg0 , int arg1 , u32 *arg2 , u32 *arg3 ) ;
STATIC int cancel_delayed_work_sync_MJRcheck(char const   *arg0 , int arg1 , bool *arg2 ,
                                             struct delayed_work **arg3 ) ;
STATIC int consume_skb_MJRcheck(char const   *arg0 , int arg1 , struct sk_buff **arg2 ) ;
STATIC int crc32_le_MJRcheck(char const   *arg0 , int arg1 , u32 *arg2 , u32 *arg3 ,
                             unsigned char const   **arg4 , size_t *arg5 ) ;
STATIC int dev_err_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct device  const  **arg3 ,
                            char const   **arg4 ) ;
STATIC int dev_get_drvdata_MJRcheck(char const   *arg0 , int arg1 , void **arg2 ,
                                    struct device  const  **arg3 ) ;
STATIC int dev_printk_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , char const   **arg3 ,
                               struct device  const  **arg4 , char const   **arg5 ) ;
STATIC int dev_set_drvdata_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct device **arg3 ,
                                    void **arg4 ) ;
STATIC int disable_irq_MJRcheck(char const   *arg0 , int arg1 , unsigned int *arg2 ) ;
STATIC int dma_alloc_from_coherent_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                            struct device **arg3 , ssize_t *arg4 ,
                                            dma_addr_t **arg5 , void ***arg6 ) ;
STATIC int dma_release_from_coherent_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                              struct device **arg3 , int *arg4 , void **arg5 ) ;
STATIC int enable_irq_MJRcheck(char const   *arg0 , int arg1 , unsigned int *arg2 ) ;
STATIC int eth_type_trans_MJRcheck(char const   *arg0 , int arg1 , __be16 *arg2 ,
                                   struct sk_buff **arg3 , struct net_device **arg4 ) ;
STATIC int free_irq_MJRcheck(char const   *arg0 , int arg1 , unsigned int *arg2 ,
                             void **arg3 ) ;
STATIC int free_netdev_MJRcheck(char const   *arg0 , int arg1 , struct net_device **arg2 ) ;
STATIC int generic_mii_ioctl_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                      struct mii_if_info **arg3 , struct mii_ioctl_data **arg4 ,
                                      int *arg5 , unsigned int **arg6 ) ;
STATIC int init_timer_key_MJRcheck(char const   *arg0 , int arg1 , struct timer_list **arg2 ,
                                   char const   **arg3 , struct lock_class_key **arg4 ) ;
STATIC int ioread16_MJRcheck(char const   *arg0 , int arg1 , unsigned int *arg2 ,
                             void **arg3 ) ;
STATIC int ioread32_MJRcheck(char const   *arg0 , int arg1 , unsigned int *arg2 ,
                             void **arg3 ) ;
STATIC int ioread8_MJRcheck(char const   *arg0 , int arg1 , unsigned int *arg2 , void **arg3 ) ;
STATIC int iowrite16_MJRcheck(char const   *arg0 , int arg1 , u16 *arg2 , void **arg3 ) ;
STATIC int iowrite32_MJRcheck(char const   *arg0 , int arg1 , u32 *arg2 , void **arg3 ) ;
STATIC int iowrite8_MJRcheck(char const   *arg0 , int arg1 , u8 *arg2 , void **arg3 ) ;
STATIC int lockdep_init_map_MJRcheck(char const   *arg0 , int arg1 , struct lockdep_map **arg2 ,
                                     char const   **arg3 , struct lock_class_key **arg4 ,
                                     int *arg5 ) ;
STATIC int memcpy_MJRcheck(char const   *arg0 , int arg1 , void **arg2 , void **arg3 ,
                           void const   **arg4 , __kernel_size_t *arg5 ) ;
STATIC int memset_MJRcheck(char const   *arg0 , int arg1 , void **arg2 , void **arg3 ,
                           int *arg4 , __kernel_size_t *arg5 ) ;
STATIC int mii_check_media_MJRcheck(char const   *arg0 , int arg1 , unsigned int *arg2 ,
                                    struct mii_if_info **arg3 , unsigned int *arg4 ,
                                    unsigned int *arg5 ) ;
STATIC int mii_ethtool_gset_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct mii_if_info **arg3 ,
                                     struct ethtool_cmd **arg4 ) ;
STATIC int mii_ethtool_sset_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct mii_if_info **arg3 ,
                                     struct ethtool_cmd **arg4 ) ;
STATIC int mii_link_ok_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct mii_if_info **arg3 ) ;
STATIC int mii_nway_restart_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct mii_if_info **arg3 ) ;
STATIC int msleep_MJRcheck(char const   *arg0 , int arg1 , unsigned int *arg2 ) ;
STATIC int net_ratelimit_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ) ;
STATIC int netdev_err_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct net_device  const  **arg3 ,
                               char const   **arg4 ) ;
STATIC int netdev_info_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct net_device  const  **arg3 ,
                                char const   **arg4 ) ;
STATIC int netdev_warn_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct net_device  const  **arg3 ,
                                char const   **arg4 ) ;
STATIC int netif_device_attach_MJRcheck(char const   *arg0 , int arg1 , struct net_device **arg2 ) ;
STATIC int netif_device_detach_MJRcheck(char const   *arg0 , int arg1 , struct net_device **arg2 ) ;
STATIC int netif_napi_add_MJRcheck(char const   *arg0 , int arg1 , struct net_device **arg2 ,
                                   struct napi_struct **arg3 , int (**arg4)(struct napi_struct * ,
                                                                            int  ) ,
                                   int *arg5 ) ;
STATIC int netif_receive_skb_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                      struct sk_buff **arg3 ) ;
STATIC int pci_bus_read_config_word_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                             struct pci_bus **arg3 , unsigned int *arg4 ,
                                             int *arg5 , u16 **arg6 ) ;
STATIC int pci_bus_write_config_word_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                              struct pci_bus **arg3 , unsigned int *arg4 ,
                                              int *arg5 , u16 *arg6 ) ;
STATIC int pci_disable_device_MJRcheck(char const   *arg0 , int arg1 , struct pci_dev **arg2 ) ;
STATIC int pci_enable_device_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                      struct pci_dev **arg3 ) ;
STATIC int pci_iomap_MJRcheck(char const   *arg0 , int arg1 , void **arg2 , struct pci_dev **arg3 ,
                              int *arg4 , unsigned long *arg5 ) ;
STATIC int pci_iounmap_MJRcheck(char const   *arg0 , int arg1 , struct pci_dev **arg2 ,
                                void **arg3 ) ;
STATIC int pci_release_regions_MJRcheck(char const   *arg0 , int arg1 , struct pci_dev **arg2 ) ;
STATIC int pci_request_regions_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                        struct pci_dev **arg3 , char const   **arg4 ) ;
STATIC int pci_restore_state_MJRcheck(char const   *arg0 , int arg1 , struct pci_dev **arg2 ) ;
STATIC int pci_save_state_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct pci_dev **arg3 ) ;
STATIC int pci_set_master_MJRcheck(char const   *arg0 , int arg1 , struct pci_dev **arg2 ) ;
STATIC int pci_set_power_state_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                        struct pci_dev **arg3 , pci_power_t *arg4 ) ;
STATIC int pci_unregister_driver_MJRcheck(char const   *arg0 , int arg1 , struct pci_driver **arg2 ) ;
STATIC int printk_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , char const   **arg3 ) ;
STATIC int register_netdev_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct net_device **arg3 ) ;
STATIC int request_threaded_irq_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                         unsigned int *arg3 , irqreturn_t (**arg4)(int  ,
                                                                                   void * ) ,
                                         irqreturn_t (**arg5)(int  , void * ) , unsigned long *arg6 ,
                                         char const   **arg7 , void **arg8 ) ;
STATIC int rtnl_lock_MJRcheck(char const   *arg0 , int arg1 ) ;
STATIC int rtnl_unlock_MJRcheck(char const   *arg0 , int arg1 ) ;
STATIC int schedule_delayed_work_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                          struct delayed_work **arg3 , unsigned long *arg4 ) ;
STATIC int skb_copy_and_csum_dev_MJRcheck(char const   *arg0 , int arg1 , struct sk_buff  const  **arg2 ,
                                          u8 **arg3 ) ;
STATIC int skb_put_MJRcheck(char const   *arg0 , int arg1 , unsigned char **arg2 ,
                            struct sk_buff **arg3 , unsigned int *arg4 ) ;
STATIC int strcpy_MJRcheck(char const   *arg0 , int arg1 , char **arg2 , char **arg3 ,
                           char const   **arg4 ) ;
STATIC int unregister_netdev_MJRcheck(char const   *arg0 , int arg1 , struct net_device **arg2 ) ;
STATIC int warn_slowpath_null_MJRcheck(char const   *arg0 , int arg1 , char const   **arg2 ,
                                       int *arg3 ) ;

STATIC int cleanup_module_MJRcheck(char const   *arg0 , int arg1 ) ;
STATIC int init_module_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ) ;
STATIC int mmc31xx_exit_MJRcheck(char const   *arg0 , int arg1 ) ;
STATIC int mmc31xx_init_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ) ;
STATIC int mmc31xx_remove_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct i2c_client **arg3 ) ;
STATIC int mmc31xx_probe_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct i2c_client **arg3 ,
                                  struct i2c_device_id  const  **arg4 ) ;
STATIC int mmc31xx_show_MJRcheck(char const   *arg0 , int arg1 , ssize_t *arg2 , struct device **arg3 ,
                                 struct device_attribute **arg4 , char **arg5 ) ;
STATIC int mmc31xx_unlocked_ioctl_MJRcheck(char const   *arg0 , int arg1 , long *arg2 ,
                                           struct file **arg3 , unsigned int *arg4 ,
                                           unsigned long *arg5 ) ;
STATIC int mmc31xx_ioctl_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct file **arg3 ,
                                  unsigned int *arg4 , unsigned long *arg5 ) ;
STATIC int mmc31xx_release_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct inode **arg3 ,
                                    struct file **arg4 ) ;
STATIC int mmc31xx_open_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct inode **arg3 ,
                                 struct file **arg4 ) ;
STATIC int mmc31xx_power_on_MJRcheck(char const   *arg0 , int arg1 , ssize_t *arg2 ,
                                     struct device **arg3 , struct device_attribute **arg4 ,
                                     char **arg5 ) ;
STATIC int mmc31xx_fs_write_MJRcheck(char const   *arg0 , int arg1 , ssize_t *arg2 ,
                                     struct device **arg3 , struct device_attribute **arg4 ,
                                     char const   **arg5 , size_t *arg6 ) ;
STATIC int mmc31xx_fs_read_MJRcheck(char const   *arg0 , int arg1 , ssize_t *arg2 ,
                                    struct device **arg3 , struct device_attribute **arg4 ,
                                    char **arg5 ) ;
STATIC int mmc31xx_i2c_tx_data_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                        char **arg3 , int *arg4 ) ;
STATIC int mmc31xx_i2c_rx_data_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                        char **arg3 , int *arg4 ) ;
STATIC int i2c_check_functionality_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                            struct i2c_adapter **arg3 , u32 *arg4 ) ;
STATIC int i2c_get_functionality_MJRcheck(char const   *arg0 , int arg1 , u32 *arg2 ,
                                          struct i2c_adapter **arg3 ) ;
STATIC int i2c_add_driver_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct i2c_driver **arg3 ) ;
STATIC int IS_ERR_MJRcheck(char const   *arg0 , int arg1 , long *arg2 , void const   **arg3 ) ;
STATIC int PTR_ERR_MJRcheck(char const   *arg0 , int arg1 , long *arg2 , void const   **arg3 ) ;
STATIC int __builtin_expect_MJRcheck(char const   *arg0 , int arg1 , long *arg2 ,
                                     long *arg3 , long *arg4 ) ;
STATIC int __class_create_MJRcheck(char const   *arg0 , int arg1 , struct class **arg2 ,
                                   struct module **arg3 , char const   **arg4 , struct lock_class_key **arg5 ) ;
STATIC int __const_udelay_MJRcheck(char const   *arg0 , int arg1 , unsigned long *arg2 ) ;
STATIC int __dynamic_pr_debug_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                       struct _ddebug **arg3 , char const   **arg4 ) ;
STATIC int copy_to_user_MJRcheck(char const   *arg0 , int arg1 , unsigned long *arg2 ,
                                 void **arg3 , void const   **arg4 , unsigned long *arg5 ) ;
STATIC int device_create_MJRcheck(char const   *arg0 , int arg1 , struct device **arg2 ,
                                  struct class **arg3 , struct device **arg4 , dev_t *arg5 ,
                                  void **arg6 , char const   **arg7 ) ;
STATIC int device_create_file_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                       struct device **arg3 , struct device_attribute  const  **arg4 ) ;
STATIC int device_remove_file_MJRcheck(char const   *arg0 , int arg1 , struct
                                       device **arg2 ,
                                       struct device_attribute  const  **arg3 );
STATIC int i2c_del_driver_MJRcheck(char const   *arg0 , int arg1 , struct
                                   i2c_driver **arg2 ) ;
STATIC int i2c_register_driver_MJRcheck(char const   *arg0 , int arg1 , int
                                        *arg2 ,
                                        struct module **arg3 , struct i2c_driver
                                        **arg4 ) ;
STATIC int i2c_transfer_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                 struct i2c_adapter **arg3 ,
                                 struct i2c_msg **arg4 , int *arg5 ) ;
STATIC int misc_deregister_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                    struct miscdevice **arg3 ) ;
STATIC int misc_register_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                  struct miscdevice **arg3 ) ;
STATIC int msleep_MJRcheck(char const   *arg0 , int arg1 , unsigned int *arg2 );
STATIC int nonseekable_open_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                     struct inode **arg3 ,
                                     struct file **arg4 ) ;
STATIC int printk_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , char
                           const   **arg3 ) ;
STATIC int sprintf_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , char
                            **arg3 ,
                            char const   **arg4 ) ;
STATIC int strlen_MJRcheck(char const   *arg0 , int arg1 , __kernel_size_t *arg2,
                           char const   **arg3 ) ;
STATIC int uprintk_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , char
                            const   **arg3 ) ;

STATIC int cleanup_module_MJRcheck(char const   *arg0 , int arg1 ) ;
STATIC int init_module_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ) ;
STATIC int akm8975_exit_MJRcheck(char const   *arg0 , int arg1 ) ;
STATIC int akm8975_init_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ) ;
STATIC int akm8975_remove_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct i2c_client **arg3 ) ;
STATIC int akm8975_probe_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct i2c_client **arg3 ,
                                  struct i2c_device_id  const  **arg4 ) ;
STATIC int akm8975_init_client_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                        struct i2c_client **arg3 ) ;
STATIC int akm8975_resume_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct i2c_client **arg3 ) ;
STATIC int akm8975_suspend_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct i2c_client **arg3 ,
                                    pm_message_t *arg4 ) ;
STATIC int akm8975_power_on_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct akm8975_data **arg3 ) ;
STATIC int akm8975_power_off_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                      struct akm8975_data **arg3 ) ;
STATIC int akm8975_interrupt_MJRcheck(char const   *arg0 , int arg1 , irqreturn_t *arg2 ,
                                      int *arg3 , void **arg4 ) ;
STATIC int akm_work_func_MJRcheck(char const   *arg0 , int arg1 , struct work_struct **arg2 ) ;
STATIC int akmd_ioctl_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct inode **arg3 ,
                               struct file **arg4 , unsigned int *arg5 , unsigned long *arg6 ) ;
STATIC int akmd_release_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct inode **arg3 ,
                                 struct file **arg4 ) ;
STATIC int akmd_open_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct inode **arg3 ,
                              struct file **arg4 ) ;
STATIC int akm_aot_release_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct inode **arg3 ,
                                    struct file **arg4 ) ;
STATIC int akm_aot_open_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct inode **arg3 ,
                                 struct file **arg4 ) ;
STATIC int akm8975_ecs_close_done_MJRcheck(char const   *arg0 , int arg1 , struct akm8975_data **arg2 ) ;
STATIC int akm8975_ecs_report_value_MJRcheck(char const   *arg0 , int arg1 , struct akm8975_data **arg2 ,
                                             short **arg3 ) ;
STATIC int akm8975_i2c_txdata_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                       struct akm8975_data **arg3 , char **arg4 ,
                                       int *arg5 ) ;
STATIC int akm8975_i2c_rxdata_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                       struct akm8975_data **arg3 , char **arg4 ,
                                       int *arg5 ) ;
STATIC int akm8975_store_MJRcheck(char const   *arg0 , int arg1 , ssize_t *arg2 ,
                                  struct device **arg3 , struct device_attribute **arg4 ,
                                  char const   **arg5 , size_t *arg6 ) ;
STATIC int akm8975_show_MJRcheck(char const   *arg0 , int arg1 , ssize_t *arg2 , struct device **arg3 ,
                                 struct device_attribute **arg4 , char **arg5 ) ;
STATIC int input_sync_MJRcheck(char const   *arg0 , int arg1 , struct input_dev **arg2 ) ;
STATIC int input_report_abs_MJRcheck(char const   *arg0 , int arg1 , struct input_dev **arg2 ,
                                     unsigned int *arg3 , int *arg4 ) ;
STATIC int kzalloc_MJRcheck(char const   *arg0 , int arg1 , void **arg2 , size_t *arg3 ,
                            gfp_t *arg4 ) ;
STATIC int kmalloc_MJRcheck(char const   *arg0 , int arg1 , void **arg2 , size_t *arg3 ,
                            gfp_t *arg4 ) ;
STATIC int i2c_check_functionality_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                            struct i2c_adapter **arg3 , u32 *arg4 ) ;
STATIC int i2c_get_functionality_MJRcheck(char const   *arg0 , int arg1 , u32 *arg2 ,
                                          struct i2c_adapter **arg3 ) ;
STATIC int i2c_add_driver_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct i2c_driver **arg3 ) ;
STATIC int i2c_set_clientdata_MJRcheck(char const   *arg0 , int arg1 , struct i2c_client **arg2 ,
                                       void **arg3 ) ;
STATIC int i2c_get_clientdata_MJRcheck(char const   *arg0 , int arg1 , void **arg2 ,
                                       struct i2c_client  const  **arg3 ) ;
STATIC int signal_pending_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct task_struct **arg3 ) ;
STATIC int test_tsk_thread_flag_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                         struct task_struct **arg3 , int *arg4 ) ;
STATIC int request_irq_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , unsigned int *arg3 ,
                                irqreturn_t (**arg4)(int  , void * ) , unsigned long *arg5 ,
                                char const   **arg6 , void **arg7 ) ;
STATIC int copy_from_user_MJRcheck(char const   *arg0 , int arg1 , unsigned long *arg2 ,
                                   void **arg3 , void const   **arg4 , unsigned long *arg5 ) ;
STATIC int __init_work_MJRcheck(char const   *arg0 , int arg1 , struct work_struct **arg2 ,
                                int *arg3 ) ;
STATIC int INIT_LIST_HEAD_MJRcheck(char const   *arg0 , int arg1 , struct list_head **arg2 ) ;
STATIC int test_ti_thread_flag_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                        struct thread_info **arg3 , int *arg4 ) ;
STATIC int atomic_cmpxchg_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , atomic_t **arg3 ,
                                   int *arg4 , int *arg5 ) ;
STATIC int atomic_set_MJRcheck(char const   *arg0 , int arg1 , atomic_t **arg2 , int *arg3 ) ;
STATIC int atomic_read_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , atomic_t const   **arg3 ) ;
STATIC int get_current_MJRcheck(char const   *arg0 , int arg1 , struct task_struct **arg2 ) ;
STATIC int kstrtoul_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , char const   **arg3 ,
                             unsigned int *arg4 , unsigned long **arg5 ) ;
STATIC int variable_test_bit_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                      int *arg3 , unsigned long const volatile   **arg4 ) ;
STATIC int set_bit_MJRcheck(char const   *arg0 , int arg1 , unsigned int *arg2 , unsigned long volatile   **arg3 ) ;
STATIC int __bad_percpu_size_MJRcheck(char const   *arg0 , int arg1 ) ;
STATIC int __builtin_expect_MJRcheck(char const   *arg0 , int arg1 , long *arg2 ,
                                     long *arg3 , long *arg4 ) ;
STATIC int __builtin_object_size_MJRcheck(char const   *arg0 , int arg1 , unsigned int *arg2 ,
                                          void **arg3 , int *arg4 ) ;
STATIC int __cmpxchg_wrong_size_MJRcheck(char const   *arg0 , int arg1 ) ;
STATIC int __init_waitqueue_head_MJRcheck(char const   *arg0 , int arg1 , wait_queue_head_t **arg2 ,
                                          struct lock_class_key **arg3 ) ;
STATIC int __kmalloc_MJRcheck(char const   *arg0 , int arg1 , void **arg2 , size_t *arg3 ,
                              gfp_t *arg4 ) ;
STATIC int __mutex_init_MJRcheck(char const   *arg0 , int arg1 , struct mutex **arg2 ,
                                 char const   **arg3 , struct lock_class_key **arg4 ) ;
STATIC int __wake_up_MJRcheck(char const   *arg0 , int arg1 , wait_queue_head_t **arg2 ,
                              unsigned int *arg3 , int *arg4 , void **arg5 ) ;
STATIC int _copy_from_user_MJRcheck(char const   *arg0 , int arg1 , unsigned long *arg2 ,
                                    void **arg3 , void const   **arg4 , unsigned long *arg5 ) ;
STATIC int _kstrtoul_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , char const   **arg3 ,
                              unsigned int *arg4 , unsigned long **arg5 ) ;
STATIC int copy_from_user_overflow_MJRcheck(char const   *arg0 , int arg1 ) ;
STATIC int copy_to_user_MJRcheck(char const   *arg0 , int arg1 , unsigned long *arg2 ,
                                 void **arg3 , void const   **arg4 , unsigned long *arg5 ) ;
STATIC int dev_err_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct device  const  **arg3 ,
                            char const   **arg4 ) ;
STATIC int dev_get_drvdata_MJRcheck(char const   *arg0 , int arg1 , void **arg2 ,
                                    struct device  const  **arg3 ) ;
STATIC int dev_set_drvdata_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct device **arg3 ,
                                    void **arg4 ) ;
STATIC int device_create_file_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                       struct device **arg3 , struct device_attribute  const  **arg4 ) ;
STATIC int disable_irq_nosync_MJRcheck(char const   *arg0 , int arg1 , unsigned int *arg2 ) ;
STATIC int enable_irq_MJRcheck(char const   *arg0 , int arg1 , unsigned int *arg2 ) ;
STATIC int finish_wait_MJRcheck(char const   *arg0 , int arg1 , wait_queue_head_t **arg2 ,
                                wait_queue_t **arg3 ) ;
STATIC int free_irq_MJRcheck(char const   *arg0 , int arg1 , unsigned int *arg2 ,
                             void **arg3 ) ;
STATIC int i2c_del_driver_MJRcheck(char const   *arg0 , int arg1 , struct i2c_driver **arg2 ) ;
STATIC int i2c_register_driver_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                        struct module **arg3 , struct i2c_driver **arg4 ) ;
STATIC int i2c_smbus_read_byte_data_MJRcheck(char const   *arg0 , int arg1 , s32 *arg2 ,
                                             struct i2c_client  const  **arg3 , u8 *arg4 ) ;
STATIC int i2c_smbus_write_byte_data_MJRcheck(char const   *arg0 , int arg1 , s32 *arg2 ,
                                              struct i2c_client  const  **arg3 , u8 *arg4 ,
                                              u8 *arg5 ) ;
STATIC int i2c_transfer_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct i2c_adapter **arg3 ,
                                 struct i2c_msg **arg4 , int *arg5 ) ;
STATIC int input_allocate_device_MJRcheck(char const   *arg0 , int arg1 , struct input_dev **arg2 ) ;
STATIC int input_event_MJRcheck(char const   *arg0 , int arg1 , struct input_dev **arg2 ,
                                unsigned int *arg3 , unsigned int *arg4 , int *arg5 ) ;
STATIC int input_free_device_MJRcheck(char const   *arg0 , int arg1 , struct input_dev **arg2 ) ;
STATIC int input_register_device_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                          struct input_dev **arg3 ) ;
STATIC int input_set_abs_params_MJRcheck(char const   *arg0 , int arg1 , struct input_dev **arg2 ,
                                         unsigned int *arg3 , int *arg4 , int *arg5 ,
                                         int *arg6 , int *arg7 ) ;
STATIC int input_unregister_device_MJRcheck(char const   *arg0 , int arg1 , struct input_dev **arg2 ) ;
STATIC int kfree_MJRcheck(char const   *arg0 , int arg1 , void const   **arg2 ) ;
STATIC int kstrtoull_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , char const   **arg3 ,
                              unsigned int *arg4 , unsigned long long **arg5 ) ;
STATIC int lockdep_init_map_MJRcheck(char const   *arg0 , int arg1 , struct lockdep_map **arg2 ,
                                     char const   **arg3 , struct lock_class_key **arg4 ,
                                     int *arg5 ) ;
STATIC int misc_deregister_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct miscdevice **arg3 ) ;
STATIC int misc_register_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct miscdevice **arg3 ) ;
STATIC int mutex_lock_nested_MJRcheck(char const   *arg0 , int arg1 , struct mutex **arg2 ,
                                      unsigned int *arg3 ) ;
STATIC int mutex_unlock_MJRcheck(char const   *arg0 , int arg1 , struct mutex **arg2 ) ;
STATIC int nonseekable_open_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct inode **arg3 ,
                                     struct file **arg4 ) ;
STATIC int prepare_to_wait_MJRcheck(char const   *arg0 , int arg1 , wait_queue_head_t **arg2 ,
                                    wait_queue_t **arg3 , int *arg4 ) ;
STATIC int printk_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , char const   **arg3 ) ;
STATIC int request_threaded_irq_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                         unsigned int *arg3 , irqreturn_t (**arg4)(int  ,
                                                                                   void * ) ,
                                         irqreturn_t (**arg5)(int  , void * ) , unsigned long *arg6 ,
                                         char const   **arg7 , void **arg8 ) ;
STATIC int schedule_MJRcheck(char const   *arg0 , int arg1 ) ;
STATIC int schedule_work_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct work_struct **arg3 ) ;
STATIC int sprintf_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , char **arg3 ,
                            char const   **arg4 ) ;

STATIC int cleanup_module_MJRcheck(char const   *arg0 , int arg1 ) ;
STATIC int init_module_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ) ;
STATIC int ks8851_exit_MJRcheck(char const   *arg0 , int arg1 ) ;
STATIC int ks8851_init_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ) ;
STATIC int ks8851_remove_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct spi_device **arg3 ) ;
STATIC int ks8851_probe_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct spi_device **arg3 ) ;
STATIC int ks8851_resume_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct spi_device **arg3 ) ;
STATIC int ks8851_suspend_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct spi_device **arg3 ,
                                   pm_message_t *arg4 ) ;
STATIC int ks8851_read_selftest_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                         struct ks8851_net **arg3 ) ;
STATIC int ks8851_phy_write_MJRcheck(char const   *arg0 , int arg1 , struct net_device **arg2 ,
                                     int *arg3 , int *arg4 , int *arg5 ) ;
STATIC int ks8851_phy_read_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct net_device **arg3 ,
                                    int *arg4 , int *arg5 ) ;
STATIC int ks8851_phy_reg_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , int *arg3 ) ;
STATIC int ks8851_set_eeprom_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                      struct net_device **arg3 , struct ethtool_eeprom **arg4 ,
                                      u8 **arg5 ) ;
STATIC int ks8851_get_eeprom_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                      struct net_device **arg3 , struct ethtool_eeprom **arg4 ,
                                      u8 **arg5 ) ;
STATIC int ks8851_get_eeprom_len_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                          struct net_device **arg3 ) ;
STATIC int ks8851_nway_reset_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                      struct net_device **arg3 ) ;
STATIC int ks8851_get_link_MJRcheck(char const   *arg0 , int arg1 , u32 *arg2 , struct net_device **arg3 ) ;
STATIC int ks8851_set_settings_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                        struct net_device **arg3 , struct ethtool_cmd **arg4 ) ;
STATIC int ks8851_get_settings_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                        struct net_device **arg3 , struct ethtool_cmd **arg4 ) ;
STATIC int ks8851_set_msglevel_MJRcheck(char const   *arg0 , int arg1 , struct net_device **arg2 ,
                                        u32 *arg3 ) ;
STATIC int ks8851_get_msglevel_MJRcheck(char const   *arg0 , int arg1 , u32 *arg2 ,
                                        struct net_device **arg3 ) ;
STATIC int ks8851_get_drvinfo_MJRcheck(char const   *arg0 , int arg1 , struct net_device **arg2 ,
                                       struct ethtool_drvinfo **arg3 ) ;
STATIC int ks8851_eeprom_write_MJRcheck(char const   *arg0 , int arg1 , struct net_device **arg2 ,
                                        unsigned int *arg3 , unsigned int *arg4 ,
                                        unsigned int *arg5 ) ;
STATIC int ks8851_eeprom_read_MJRcheck(char const   *arg0 , int arg1 , unsigned int *arg2 ,
                                       struct net_device **arg3 , unsigned int *arg4 ) ;
STATIC int ks8851_net_ioctl_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct net_device **arg3 ,
                                     struct ifreq **arg4 , int *arg5 ) ;
STATIC int ks8851_set_mac_address_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                           struct net_device **arg3 , void **arg4 ) ;
STATIC int ks8851_set_rx_mode_MJRcheck(char const   *arg0 , int arg1 , struct net_device **arg2 ) ;
STATIC int ks8851_rxctrl_work_MJRcheck(char const   *arg0 , int arg1 , struct work_struct **arg2 ) ;
STATIC int ks8851_start_xmit_MJRcheck(char const   *arg0 , int arg1 , netdev_tx_t *arg2 ,
                                      struct sk_buff **arg3 , struct net_device **arg4 ) ;
STATIC int ks8851_net_stop_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct net_device **arg3 ) ;
STATIC int ks8851_net_open_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct net_device **arg3 ) ;
STATIC int ks8851_set_powermode_MJRcheck(char const   *arg0 , int arg1 , struct ks8851_net **arg2 ,
                                         unsigned int *arg3 ) ;
STATIC int ks8851_tx_work_MJRcheck(char const   *arg0 , int arg1 , struct work_struct **arg2 ) ;
STATIC int ks8851_done_tx_MJRcheck(char const   *arg0 , int arg1 , struct ks8851_net **arg2 ,
                                   struct sk_buff **arg3 ) ;
STATIC int ks8851_wrpkt_MJRcheck(char const   *arg0 , int arg1 , struct ks8851_net **arg2 ,
                                 struct sk_buff **arg3 , bool *arg4 ) ;
STATIC int calc_txlen_MJRcheck(char const   *arg0 , int arg1 , unsigned int *arg2 ,
                               unsigned int *arg3 ) ;
STATIC int ks8851_irq_work_MJRcheck(char const   *arg0 , int arg1 , struct work_struct **arg2 ) ;
STATIC int ks8851_rx_pkts_MJRcheck(char const   *arg0 , int arg1 , struct ks8851_net **arg2 ) ;
STATIC int ks8851_dbg_dumpkkt_MJRcheck(char const   *arg0 , int arg1 , struct ks8851_net **arg2 ,
                                       u8 **arg3 ) ;
STATIC int ks8851_rdfifo_MJRcheck(char const   *arg0 , int arg1 , struct ks8851_net **arg2 ,
                                  u8 **arg3 , unsigned int *arg4 ) ;
STATIC int ks8851_irq_MJRcheck(char const   *arg0 , int arg1 , irqreturn_t *arg2 ,
                               int *arg3 , void **arg4 ) ;
STATIC int ks8851_init_mac_MJRcheck(char const   *arg0 , int arg1 , struct ks8851_net **arg2 ) ;
STATIC int ks8851_write_mac_addr_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                          struct net_device **arg3 ) ;
STATIC int ks8851_soft_reset_MJRcheck(char const   *arg0 , int arg1 , struct ks8851_net **arg2 ,
                                      unsigned int *arg3 ) ;
STATIC int ks8851_rdreg32_MJRcheck(char const   *arg0 , int arg1 , unsigned int *arg2 ,
                                   struct ks8851_net **arg3 , unsigned int *arg4 ) ;
STATIC int ks8851_rdreg16_MJRcheck(char const   *arg0 , int arg1 , unsigned int *arg2 ,
                                   struct ks8851_net **arg3 , unsigned int *arg4 ) ;
STATIC int ks8851_rdreg8_MJRcheck(char const   *arg0 , int arg1 , unsigned int *arg2 ,
                                  struct ks8851_net **arg3 , unsigned int *arg4 ) ;
STATIC int ks8851_rdreg_MJRcheck(char const   *arg0 , int arg1 , struct ks8851_net **arg2 ,
                                 unsigned int *arg3 , u8 **arg4 , unsigned int *arg5 ) ;
STATIC int ks8851_rx_1msg_MJRcheck(char const   *arg0 , int arg1 , bool *arg2 , struct ks8851_net **arg3 ) ;
STATIC int ks8851_wrreg8_MJRcheck(char const   *arg0 , int arg1 , struct ks8851_net **arg2 ,
                                  unsigned int *arg3 , unsigned int *arg4 ) ;
STATIC int ks8851_wrreg16_MJRcheck(char const   *arg0 , int arg1 , struct ks8851_net **arg2 ,
                                   unsigned int *arg3 , unsigned int *arg4 ) ;
STATIC int spi_message_add_tail_MJRcheck(char const   *arg0 , int arg1 , struct spi_transfer **arg2 ,
                                         struct spi_message **arg3 ) ;
STATIC int spi_message_init_MJRcheck(char const   *arg0 , int arg1 , struct spi_message **arg2 ) ;
STATIC int spi_unregister_driver_MJRcheck(char const   *arg0 , int arg1 , struct spi_driver **arg2 ) ;
STATIC int if_mii_MJRcheck(char const   *arg0 , int arg1 , struct mii_ioctl_data **arg2 ,
                           struct ifreq **arg3 ) ;
STATIC int random_ether_addr_MJRcheck(char const   *arg0 , int arg1 , u8 **arg2 ) ;
STATIC int is_valid_ether_addr_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                        u8 const   **arg3 ) ;
STATIC int is_multicast_ether_addr_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                            u8 const   **arg3 ) ;
STATIC int is_zero_ether_addr_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                       u8 const   **arg3 ) ;
STATIC int netif_msg_init_MJRcheck(char const   *arg0 , int arg1 , u32 *arg2 , int *arg3 ,
                                   int *arg4 ) ;
STATIC int netif_running_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct net_device  const  **arg3 ) ;
STATIC int netif_stop_queue_MJRcheck(char const   *arg0 , int arg1 , struct net_device **arg2 ) ;
STATIC int netif_tx_stop_queue_MJRcheck(char const   *arg0 , int arg1 , struct netdev_queue **arg2 ) ;
STATIC int netif_wake_queue_MJRcheck(char const   *arg0 , int arg1 , struct net_device **arg2 ) ;
STATIC int netif_tx_wake_queue_MJRcheck(char const   *arg0 , int arg1 , struct netdev_queue **arg2 ) ;
STATIC int netif_start_queue_MJRcheck(char const   *arg0 , int arg1 , struct net_device **arg2 ) ;
STATIC int netif_tx_start_queue_MJRcheck(char const   *arg0 , int arg1 , struct netdev_queue **arg2 ) ;
STATIC int netdev_priv_MJRcheck(char const   *arg0 , int arg1 , void **arg2 , struct net_device  const  **arg3 ) ;
STATIC int netdev_get_tx_queue_MJRcheck(char const   *arg0 , int arg1 , struct netdev_queue **arg2 ,
                                        struct net_device  const  **arg3 , unsigned int *arg4 ) ;
STATIC int netdev_alloc_skb_ip_align_MJRcheck(char const   *arg0 , int arg1 , struct sk_buff **arg2 ,
                                              struct net_device **arg3 , unsigned int *arg4 ) ;
STATIC int __netdev_alloc_skb_ip_align_MJRcheck(char const   *arg0 , int arg1 , struct sk_buff **arg2 ,
                                                struct net_device **arg3 , unsigned int *arg4 ,
                                                gfp_t *arg5 ) ;
STATIC int skb_queue_head_init_MJRcheck(char const   *arg0 , int arg1 , struct sk_buff_head **arg2 ) ;
STATIC int __skb_queue_head_init_MJRcheck(char const   *arg0 , int arg1 , struct sk_buff_head **arg2 ) ;
STATIC int skb_queue_empty_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct sk_buff_head  const  **arg3 ) ;
STATIC int dev_name_MJRcheck(char const   *arg0 , int arg1 , char const   **arg2 ,
                             struct device  const  **arg3 ) ;
STATIC int kmalloc_MJRcheck(char const   *arg0 , int arg1 , void **arg2 , size_t *arg3 ,
                            gfp_t *arg4 ) ;
STATIC int request_irq_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , unsigned int *arg3 ,
                                irqreturn_t (**arg4)(int  , void * ) , unsigned long *arg5 ,
                                char const   **arg6 , void **arg7 ) ;
STATIC int kobject_name_MJRcheck(char const   *arg0 , int arg1 , char const   **arg2 ,
                                 struct kobject  const  **arg3 ) ;
STATIC int __init_work_MJRcheck(char const   *arg0 , int arg1 , struct work_struct **arg2 ,
                                int *arg3 ) ;
STATIC int spin_unlock_MJRcheck(char const   *arg0 , int arg1 , spinlock_t **arg2 ) ;
STATIC int spin_lock_MJRcheck(char const   *arg0 , int arg1 , spinlock_t **arg2 ) ;
STATIC int spinlock_check_MJRcheck(char const   *arg0 , int arg1 , raw_spinlock_t **arg2 ,
                                   spinlock_t **arg3 ) ;
STATIC int list_add_tail_MJRcheck(char const   *arg0 , int arg1 , struct list_head **arg2 ,
                                  struct list_head **arg3 ) ;
STATIC int __list_add_MJRcheck(char const   *arg0 , int arg1 , struct list_head **arg2 ,
                               struct list_head **arg3 , struct list_head **arg4 ) ;
STATIC int INIT_LIST_HEAD_MJRcheck(char const   *arg0 , int arg1 , struct list_head **arg2 ) ;
STATIC int constant_test_bit_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                      unsigned int *arg3 , unsigned long const volatile   **arg4 ) ;
STATIC int test_and_clear_bit_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                       int *arg3 , unsigned long volatile   **arg4 ) ;
STATIC int clear_bit_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , unsigned long volatile   **arg3 ) ;
STATIC int set_bit_MJRcheck(char const   *arg0 , int arg1 , unsigned int *arg2 , unsigned long volatile   **arg3 ) ;
STATIC int __builtin_expect_MJRcheck(char const   *arg0 , int arg1 , long *arg2 ,
                                     long *arg3 , long *arg4 ) ;
STATIC int __const_udelay_MJRcheck(char const   *arg0 , int arg1 , unsigned long *arg2 ) ;
STATIC int __kmalloc_MJRcheck(char const   *arg0 , int arg1 , void **arg2 , size_t *arg3 ,
                              gfp_t *arg4 ) ;
STATIC int __mutex_init_MJRcheck(char const   *arg0 , int arg1 , struct mutex **arg2 ,
                                 char const   **arg3 , struct lock_class_key **arg4 ) ;
STATIC int __netdev_alloc_skb_MJRcheck(char const   *arg0 , int arg1 , struct sk_buff **arg2 ,
                                       struct net_device **arg3 , unsigned int *arg4 ,
                                       gfp_t *arg5 ) ;
STATIC int __netif_schedule_MJRcheck(char const   *arg0 , int arg1 , struct Qdisc **arg2 ) ;
STATIC int __raw_spin_lock_init_MJRcheck(char const   *arg0 , int arg1 , raw_spinlock_t **arg2 ,
                                         char const   **arg3 , struct lock_class_key **arg4 ) ;
STATIC int _dev_info_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct device  const  **arg3 ,
                              char const   **arg4 ) ;
STATIC int _raw_spin_lock_MJRcheck(char const   *arg0 , int arg1 , raw_spinlock_t **arg2 ) ;
STATIC int _raw_spin_unlock_MJRcheck(char const   *arg0 , int arg1 , raw_spinlock_t **arg2 ) ;
STATIC int alloc_etherdev_mqs_MJRcheck(char const   *arg0 , int arg1 , struct net_device **arg2 ,
                                       int *arg3 , unsigned int *arg4 , unsigned int *arg5 ) ;
STATIC int bitrev32_MJRcheck(char const   *arg0 , int arg1 , u32 *arg2 , u32 *arg3 ) ;
STATIC int consume_skb_MJRcheck(char const   *arg0 , int arg1 , struct sk_buff **arg2 ) ;
STATIC int crc32_le_MJRcheck(char const   *arg0 , int arg1 , u32 *arg2 , u32 *arg3 ,
                             unsigned char const   **arg4 , size_t *arg5 ) ;
STATIC int dev_err_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct device  const  **arg3 ,
                            char const   **arg4 ) ;
STATIC int dev_get_drvdata_MJRcheck(char const   *arg0 , int arg1 , void **arg2 ,
                                    struct device  const  **arg3 ) ;
STATIC int dev_set_drvdata_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct device **arg3 ,
                                    void **arg4 ) ;
STATIC int disable_irq_nosync_MJRcheck(char const   *arg0 , int arg1 , unsigned int *arg2 ) ;
STATIC int driver_unregister_MJRcheck(char const   *arg0 , int arg1 , struct device_driver **arg2 ) ;
STATIC int enable_irq_MJRcheck(char const   *arg0 , int arg1 , unsigned int *arg2 ) ;
STATIC int eth_type_trans_MJRcheck(char const   *arg0 , int arg1 , __be16 *arg2 ,
                                   struct sk_buff **arg3 , struct net_device **arg4 ) ;
STATIC int flush_work_MJRcheck(char const   *arg0 , int arg1 , bool *arg2 , struct work_struct **arg3 ) ;
STATIC int free_irq_MJRcheck(char const   *arg0 , int arg1 , unsigned int *arg2 ,
                             void **arg3 ) ;
STATIC int free_netdev_MJRcheck(char const   *arg0 , int arg1 , struct net_device **arg2 ) ;
STATIC int generic_mii_ioctl_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                      struct mii_if_info **arg3 , struct mii_ioctl_data **arg4 ,
                                      int *arg5 , unsigned int **arg6 ) ;
STATIC int get_random_bytes_MJRcheck(char const   *arg0 , int arg1 , void **arg2 ,
                                     int *arg3 ) ;
STATIC int kfree_MJRcheck(char const   *arg0 , int arg1 , void const   **arg2 ) ;
STATIC int lockdep_init_map_MJRcheck(char const   *arg0 , int arg1 , struct lockdep_map **arg2 ,
                                     char const   **arg3 , struct lock_class_key **arg4 ,
                                     int *arg5 ) ;
STATIC int memcmp_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , void const   **arg3 ,
                           void const   **arg4 , __kernel_size_t *arg5 ) ;
STATIC int memcpy_MJRcheck(char const   *arg0 , int arg1 , void **arg2 , void **arg3 ,
                           void const   **arg4 , __kernel_size_t *arg5 ) ;
STATIC int memset_MJRcheck(char const   *arg0 , int arg1 , void **arg2 , void **arg3 ,
                           int *arg4 , __kernel_size_t *arg5 ) ;
STATIC int mii_ethtool_gset_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct mii_if_info **arg3 ,
                                     struct ethtool_cmd **arg4 ) ;
STATIC int mii_ethtool_sset_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct mii_if_info **arg3 ,
                                     struct ethtool_cmd **arg4 ) ;
STATIC int mii_link_ok_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct mii_if_info **arg3 ) ;
STATIC int mii_nway_restart_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct mii_if_info **arg3 ) ;
STATIC int mutex_lock_nested_MJRcheck(char const   *arg0 , int arg1 , struct mutex **arg2 ,
                                      unsigned int *arg3 ) ;
STATIC int mutex_unlock_MJRcheck(char const   *arg0 , int arg1 , struct mutex **arg2 ) ;
STATIC int netdev_err_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct net_device  const  **arg3 ,
                               char const   **arg4 ) ;
STATIC int netdev_info_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct net_device  const  **arg3 ,
                                char const   **arg4 ) ;
STATIC int netdev_printk_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , char const   **arg3 ,
                                  struct net_device  const  **arg4 , char const   **arg5 ) ;
STATIC int netdev_warn_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct net_device  const  **arg3 ,
                                char const   **arg4 ) ;
STATIC int netif_device_attach_MJRcheck(char const   *arg0 , int arg1 , struct net_device **arg2 ) ;
STATIC int netif_device_detach_MJRcheck(char const   *arg0 , int arg1 , struct net_device **arg2 ) ;
STATIC int netif_rx_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct sk_buff **arg3 ) ;
STATIC int printk_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , char const   **arg3 ) ;
STATIC int register_netdev_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct net_device **arg3 ) ;
STATIC int request_threaded_irq_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                         unsigned int *arg3 , irqreturn_t (**arg4)(int  ,
                                                                                   void * ) ,
                                         irqreturn_t (**arg5)(int  , void * ) , unsigned long *arg6 ,
                                         char const   **arg7 , void **arg8 ) ;
STATIC int schedule_work_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct work_struct **arg3 ) ;
STATIC int skb_dequeue_MJRcheck(char const   *arg0 , int arg1 , struct sk_buff **arg2 ,
                                struct sk_buff_head **arg3 ) ;
STATIC int skb_put_MJRcheck(char const   *arg0 , int arg1 , unsigned char **arg2 ,
                            struct sk_buff **arg3 , unsigned int *arg4 ) ;
STATIC int skb_queue_tail_MJRcheck(char const   *arg0 , int arg1 , struct sk_buff_head **arg2 ,
                                   struct sk_buff **arg3 ) ;
STATIC int spi_register_driver_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                        struct spi_driver **arg3 ) ;
STATIC int spi_sync_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct spi_device **arg3 ,
                             struct spi_message **arg4 ) ;
STATIC int strlcpy_MJRcheck(char const   *arg0 , int arg1 , size_t *arg2 , char **arg3 ,
                            char const   **arg4 , size_t *arg5 ) ;
STATIC int unregister_netdev_MJRcheck(char const   *arg0 , int arg1 , struct net_device **arg2 ) ;
STATIC int warn_slowpath_null_MJRcheck(char const   *arg0 , int arg1 , char const   **arg2 ,
                                       int *arg3 ) ;

STATIC int cleanup_module_MJRcheck(char const   *arg0 , int arg1 ) ;
STATIC int init_module_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ) ;
STATIC int a1026_exit_MJRcheck(char const   *arg0 , int arg1 ) ;
STATIC int a1026_init_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ) ;
STATIC int a1026_resume_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct i2c_client **arg3 ) ;
STATIC int a1026_suspend_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct i2c_client **arg3 ,
                                  pm_message_t *arg4 ) ;
STATIC int a1026_remove_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct i2c_client **arg3 ) ;
STATIC int a1026_probe_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct i2c_client **arg3 ,
                                struct i2c_device_id  const  **arg4 ) ;
STATIC int a1026_ioctl_MJRcheck(char const   *arg0 , int arg1 , long *arg2 , struct file **arg3 ,
                                unsigned int *arg4 , unsigned long *arg5 ) ;
STATIC int exe_cmd_in_file_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , unsigned char **arg3 ) ;
STATIC int a1026_set_mic_state_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                        char *arg3 ) ;
STATIC int execute_cmdmsg_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , unsigned int *arg3 ) ;
STATIC int a1026_set_config_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , char *arg3 ,
                                     int *arg4 ) ;
STATIC int a1026_filter_vp_cmd_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                        int *arg3 , int *arg4 ) ;
STATIC int chk_wakeup_a1026_MJRcheck(char const   *arg0 , int arg1 , ssize_t *arg2 ) ;
STATIC int a1026_bootup_init_MJRcheck(char const   *arg0 , int arg1 , ssize_t *arg2 ,
                                      struct file **arg3 , struct a1026img **arg4 ) ;
STATIC int a1026_i2c_sw_reset_MJRcheck(char const   *arg0 , int arg1 , unsigned int *arg2 ) ;
STATIC int a1026_release_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct inode **arg3 ,
                                  struct file **arg4 ) ;
STATIC int a1026_open_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct inode **arg3 ,
                               struct file **arg4 ) ;
STATIC int a1026_i2c_write_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , char **arg3 ,
                                    int *arg4 ) ;
STATIC int a1026_i2c_read_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , char **arg3 ,
                                   int *arg4 ) ;
STATIC int gpio_set_value_MJRcheck(char const   *arg0 , int arg1 , unsigned int *arg2 ,
                                   int *arg3 ) ;
STATIC int kzalloc_MJRcheck(char const   *arg0 , int arg1 , void **arg2 , size_t *arg3 ,
                            gfp_t *arg4 ) ;
STATIC int kmalloc_MJRcheck(char const   *arg0 , int arg1 , void **arg2 , size_t *arg3 ,
                            gfp_t *arg4 ) ;
STATIC int i2c_check_functionality_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                            struct i2c_adapter **arg3 , u32 *arg4 ) ;
STATIC int i2c_get_functionality_MJRcheck(char const   *arg0 , int arg1 , u32 *arg2 ,
                                          struct i2c_adapter **arg3 ) ;
STATIC int i2c_add_driver_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct i2c_driver **arg3 ) ;
STATIC int i2c_get_clientdata_MJRcheck(char const   *arg0 , int arg1 , void **arg2 ,
                                       struct i2c_client  const  **arg3 ) ;
STATIC int copy_from_user_MJRcheck(char const   *arg0 , int arg1 , unsigned long *arg2 ,
                                   void **arg3 , void const   **arg4 , unsigned long *arg5 ) ;
STATIC int __builtin_expect_MJRcheck(char const   *arg0 , int arg1 , long *arg2 ,
                                     long *arg3 , long *arg4 ) ;
STATIC int __builtin_object_size_MJRcheck(char const   *arg0 , int arg1 , unsigned int *arg2 ,
                                          void **arg3 , int *arg4 ) ;
STATIC int __const_udelay_MJRcheck(char const   *arg0 , int arg1 , unsigned long *arg2 ) ;
STATIC int __gpio_set_value_MJRcheck(char const   *arg0 , int arg1 , unsigned int *arg2 ,
                                     int *arg3 ) ;
STATIC int __kmalloc_MJRcheck(char const   *arg0 , int arg1 , void **arg2 , size_t *arg3 ,
                              gfp_t *arg4 ) ;
STATIC int __mutex_init_MJRcheck(char const   *arg0 , int arg1 , struct mutex **arg2 ,
                                 char const   **arg3 , struct lock_class_key **arg4 ) ;
STATIC int _copy_from_user_MJRcheck(char const   *arg0 , int arg1 , unsigned long *arg2 ,
                                    void **arg3 , void const   **arg4 , unsigned long *arg5 ) ;
STATIC int copy_from_user_overflow_MJRcheck(char const   *arg0 , int arg1 ) ;
STATIC int copy_to_user_MJRcheck(char const   *arg0 , int arg1 , unsigned long *arg2 ,
                                 void **arg3 , void const   **arg4 , unsigned long *arg5 ) ;
STATIC int dev_get_drvdata_MJRcheck(char const   *arg0 , int arg1 , void **arg2 ,
                                    struct device  const  **arg3 ) ;
STATIC int gpio_direction_output_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                          unsigned int *arg3 , int *arg4 ) ;
STATIC int gpio_free_MJRcheck(char const   *arg0 , int arg1 , unsigned int *arg2 ) ;
STATIC int gpio_request_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , unsigned int *arg3 ,
                                 char const   **arg4 ) ;
STATIC int i2c_del_driver_MJRcheck(char const   *arg0 , int arg1 , struct i2c_driver **arg2 ) ;
STATIC int i2c_register_driver_MJRcheck(char const   *arg0 , int arg1 , int *arg2 ,
                                        struct module **arg3 , struct i2c_driver **arg4 ) ;
STATIC int i2c_transfer_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct i2c_adapter **arg3 ,
                                 struct i2c_msg **arg4 , int *arg5 ) ;
STATIC int kfree_MJRcheck(char const   *arg0 , int arg1 , void const   **arg2 ) ;
STATIC int memcpy_MJRcheck(char const   *arg0 , int arg1 , void **arg2 , void **arg3 ,
                           void const   **arg4 , __kernel_size_t *arg5 ) ;
STATIC int memset_MJRcheck(char const   *arg0 , int arg1 , void **arg2 , void **arg3 ,
                           int *arg4 , __kernel_size_t *arg5 ) ;
STATIC int misc_register_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , struct miscdevice **arg3 ) ;
STATIC int msleep_MJRcheck(char const   *arg0 , int arg1 , unsigned int *arg2 ) ;
STATIC int mutex_lock_nested_MJRcheck(char const   *arg0 , int arg1 , struct mutex **arg2 ,
                                      unsigned int *arg3 ) ;
STATIC int mutex_unlock_MJRcheck(char const   *arg0 , int arg1 , struct mutex **arg2 ) ;
STATIC int printk_MJRcheck(char const   *arg0 , int arg1 , int *arg2 , char const   **arg3 ) ;
STATIC int add_timer_MJRcheck(const char *fn, int prepost, struct timer_list **timer) ;

#endif

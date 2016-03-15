#ifndef FREEBSD_MEGA_HEADER
#define FREEBSD_MEGA_HEADER

#include <sys/param.h>
#include <sys/endian.h>
#include <sys/systm.h>
#include <sys/sockio.h>
#include <sys/mbuf.h>
#include <sys/malloc.h>
#include <sys/kernel.h>
#include <sys/module.h>
#include <sys/socket.h>
#include <sys/sysctl.h>

#include <net/if.h>
#include <net/if_arp.h>
#include <net/ethernet.h>
#include <net/if_dl.h>
#include <net/if_media.h>
#include <net/if_types.h>

#include <net/bpf.h>

#include <machine/bus.h>
#include <machine/resource.h>
#include <sys/bus.h>
#include <sys/rman.h>

#include <dev/mii/mii.h>
#include <dev/mii/mii_bitbang.h>
#include <dev/mii/miivar.h>

#include <dev/pci/pcireg.h>
#include <dev/pci/pcivar.h>

#include "miibus_if.h"

#include <pci/if_rlreg.h>

#define STATIC
//#define STATIC static

STATIC int
default_MJRcheck(const char *fn,
                 int prepost);
STATIC int
ed_attach_MJRcheck(const char *fn,
                   int prepost,
                   int *retval,
                   device_t *dev);
STATIC int
miibus_readreg_desc_MJRcheck(const char *fn,
                             int prepost,
                             int *retval,
                             device_t *dev,
                             int *phy,
                             int *reg);
STATIC int
mii_bitbang_readreg_MJRcheck(const char *fn,
                             int prepost,
                             int *retval,
                             device_t *dev,
                             mii_bitbang_ops_t *ops,
                             int *phy,
                             int *reg);
STATIC int
mii_attach_MJRcheck(const char *fn,
                    int prepost,
                    int *retval,
                    device_t *dev,
                    device_t **miibus,
                    struct ifnet **ifp,
                    ifm_change_cb_t *ifmedia_upd,
                    ifm_stat_cb_t *ifmedia_sts,
                    int *capmask,
                    int *phyloc,
                    int *offloc,
                    int *flags);

/////////////////////////////////////////////////////////////
STATIC int
interrupt_handler_MJRcheck(const char *fn,
                           int prepost,
                           int *retval,
                           void **driver_specific);
STATIC int
bus_setup_intr_MJRcheck(const char *fn,
                        int prepost,
                        int *retval,
                        device_t *dev,
                        struct resource **r,
                        int *flags,
                        driver_filter_t **filter,
                        driver_intr_t **handler, 
                        void **arg,
                        void ***cookiep);
STATIC int
snd_setup_intr_MJRcheck(const char *fn,
                        int prepost,
                        int *retval,
                        device_t *dev,
                        struct resource **res,
                        int *flags,
                        driver_intr_t **hand,
                        void **param,
                        void ***cookiep);
STATIC int
bus_teardown_intr_MJRcheck(const char *fn,
                           int prepost,
                           int *retval,
                           device_t *dev,
                           struct resource **r,
                           void **cookie);
/////////////////////////////////////////////////////////////
STATIC int
callout_reset_on_MJRcheck(const char *fn,
                          int prepost,
                          int *retval,
                          struct callout **c,
                          int *to_ticks,
                          void (**ftn)(void *),
                          void **arg,
                          int *cpu);

STATIC int
channel_getptr_desc_MJRcheck(const char *fn,
                             int prepost,
                             uint32_t *retval,
                             kobj_t *obj,
                             void **data);
STATIC int
ac97_read_desc_MJRcheck (const char *fn,
                         int prepost,
                         int *retval,
                         kobj_t *obj,
                         void **s,
                         int *addr);

///////////////////////////////////////////////////////
// DMA

STATIC int
bus_dmamap_load_MJRcheck(const char *fn,
                         int prepost,
                         int *retval,
                         bus_dma_tag_t *dmat, bus_dmamap_t *map, void **buf,
                         bus_size_t *buflen, bus_dmamap_callback_t **callback,
                         void **callback_arg, int *flags);

STATIC int
bus_dmamap_load_mbuf_MJRcheck(const char *fn,
                              int prepost,
                              int *retval,
                              bus_dma_tag_t *dmat,
                              bus_dmamap_t *map,
                              struct mbuf **m0,
                              bus_dmamap_callback2_t **callback,
                              void **callback_arg,
                              int *flags);

STATIC int
bus_dmamap_load_mbuf_sg_MJRcheck(const char *fn,
                                 int prepost,
                                 int *retval,
                                 bus_dma_tag_t *dmat,
                                 bus_dmamap_t *map,
                                 struct mbuf **m0,
                                 bus_dma_segment_t **segs,
                                 int **nsegs,
                                 int *flags);

STATIC int
bus_dmamap_load_uio_MJRcheck(const char *fn,
                             int prepost,
                             int *retval,
                             bus_dma_tag_t *dmat,
                             bus_dmamap_t *map,
                             struct uio **uio,
                             bus_dmamap_callback2_t **callback,
                             void **callback_arg,
                             int *flags);

STATIC int
bus_dmamap_unload_MJRcheck(const char *fn,
                           int prepost,
                           bus_dma_tag_t *dmat,
                           bus_dmamap_t *map);

STATIC int
bus_dmamem_alloc_MJRcheck(const char *fn,
                          int prepost,
                          int *retval,
                          bus_dma_tag_t *dmat,
                          void*** vaddr,
                          int *flags,
                          bus_dmamap_t **mapp);
STATIC int
bus_dmamem_free_MJRcheck(const char *fn,
                         int prepost,
                         int *retval,
                         bus_dma_tag_t *dmat,
                         void **vaddr,
                         bus_dmamap_t *map);
STATIC int
pci_read_config_MJRcheck(const char *fn,
                         int prepost,
                         uint32_t *retval,
                         device_t *dev,
                         int *reg,
                         int *width);
STATIC int
pci_write_config_MJRcheck(const char *fn,
                          int prepost,
                          device_t *dev,
                          int *reg,
                          uint32_t *val,
                          int *width);
#endif

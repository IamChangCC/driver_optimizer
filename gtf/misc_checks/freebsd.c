#include "../test_state.h"
#include "../testing_ep.h"
#include "../mega_header.h"

STATIC int
default_MJRcheck(const char *fn,
                 int prepost) {
//    check_routine_start();
//    uprintk ("Warning:  %s called\n", __func__);
//    check_routine_end();
    return 0;
}

STATIC int
ed_attach_MJRcheck(const char *fn,
                   int prepost,
                   int *retval,
                   device_t *dev) {
    check_routine_start();
    check_routine_end();
    return 0;
}

STATIC int
miibus_readreg_desc_MJRcheck(const char *fn,
                             int prepost,
                             int *retval,
                             device_t *dev,
                             int *phy,
                             int *reg) {
    check_routine_start();
    if (prepost == 0) {
    } else if (prepost == 1) {
        // Force detection of device
        if (*reg == MII_BMSR) {
            if ((*retval == 0) ||
                (*retval == 0xffff) ||
                (*retval & (BMSR_EXTSTAT | BMSR_MEDIAMASK)) == 0 ||
                (*retval & BMSR_100TXFDX) == 0 ||
                (*retval & BMSR_LINK) == 0 ||
                (*retval & BMCR_ISO) != 0) {
                s2e_kill_state(0, 0, "miibus_readreg_desc hack");
            } else {
                uprintk ("miibus_readreg_desc - OK");
            }
        }
    } else {
        tfassert(0);
    }
    check_routine_end();
    return 0;
}

STATIC int
mii_bitbang_readreg_MJRcheck(const char *fn,
                             int prepost,
                             int *retval,
                             device_t *dev,
                             mii_bitbang_ops_t *ops,
                             int *phy,
                             int *reg) {
    check_routine_start();
    if (prepost == 0) {
    } else if (prepost == 1) {
        if (*retval == 0) {
            s2e_kill_state(0, 0, "mii_bitbang_readreg hack");
        } else {
            uprintk ("miibus_readreg_desc - OK");
        }
    } else {
        tfassert(0);
    }
    check_routine_end();
    return 0;
}

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
                    int *flags) {
    check_routine_start();
    if (prepost == 0) {
        *phyloc = 0; // Force detection of only one phy.
    } else if (prepost == 1) {
    } else {
        tfassert(0);
    }
    check_routine_end();
    return 0;
}

/////////////////////////////////////////////////////////////
STATIC int
callout_reset_on_MJRcheck(const char *fn,
                          int prepost,
                          int *retval,
                          struct callout **c,
                          int *to_ticks,
                          void (**ftn)(void *),
                          void **arg,
                          int *cpu) {
    check_routine_start();
    if (prepost == 0) {
        // This implementation will prevent timers from firing
        // frequently enough for es137x to work indefintely,
        // however, it is good enough to execute all the necessary
        // code at least a few times.
        static unsigned char adaptive = 1;
        if (adaptive == 0) {
            adaptive = 1;
        }
        adaptive *= 2;
        *to_ticks = *to_ticks * g_sym_slow_timers * adaptive;
        if (*to_ticks > 2000) {
            *to_ticks = 2000; // 20 seconds
        }
    } else if (prepost == 1) {
    } else {
        tfassert(0);
    }
    check_routine_end();
    return 0;
}

STATIC int
channel_getptr_desc_MJRcheck(const char *fn,
                             int prepost,
                             uint32_t *retval,
                             kobj_t *obj,
                             void **data) {
    static uint32_t count = 1024;

    check_routine_start();
    if (prepost == 0) {
    } else if (prepost == 1) {
        count *= 2;
        count &= 0xFFFFFF00;
        if (count < 1024) {
            count = 1024;
        }
        *retval = count;
    } else {
        tfassert(0);
    }
    check_routine_end();
    return 0;
}

STATIC int
ac97_read_desc_MJRcheck (const char *fn,
                         int prepost,
                         int *retval,
                         kobj_t *obj,
                         void **s,
                         int *addr) {
    check_routine_start();
    if (prepost == 0) {
    } else if (prepost == 1) {
        if (*retval == 0 ||
            *retval == 0xFFFF ||
            *retval == 0xFFFFFFFF) {
            s2e_deprioritize(0);
        }
    } else {
        tfassert(0);
    }
    check_routine_end();
    return 0;
}

/////////////////////////////////////////////////////////
// DMA support
/////////////////////////////////////////////////////////

// This is used by bus_dmamap_load:
#define MAX_SEGS 128
#define MAX_STRUCTS 4096

struct dma_struct_type {
    int used;
    bus_dma_tag_t tag;
    bus_addr_t ds_addr[MAX_SEGS];
    int ds_len[MAX_SEGS];
    int nsegs;
    void *orig_arg;
    bus_dmamap_callback_t *orig_callback1;
    bus_dmamap_callback2_t *orig_callback2;
};

static struct dma_struct_type *dma_structs = NULL;

// MALLOC_DECLARE(M_MJR_DMAMAP);
static MALLOC_DEFINE(M_MJR_DMAMAP, "test_pci_dma", "DMA tracking on FreeBSD");

int initialize_dma_map(void) {
    dma_structs = malloc (sizeof (struct dma_struct_type) * MAX_STRUCTS,
                          M_MJR_DMAMAP, M_WAITOK | M_ZERO);
    assert (dma_structs != NULL, "dma_structs = NULL?");
    return 0;
}

void free_dma_map (void) {
    if (dma_structs != NULL) {
        free(dma_structs, M_MJR_DMAMAP);
        dma_structs = NULL;
    }
}

static struct dma_struct_type *dma_struct_get(void) {
    int i;
    int looped = 0;

    // last_unused must equal a valid index all the time
    static int last_unused = 0; // 0 < last_unused < MAX_STRUCTS

    assert (dma_structs != NULL, "Should allocate memory first");

    i = last_unused;
    for (;;) {
        struct dma_struct_type *cur = &dma_structs[i];
        if (cur->used == 0) {
            uprintk ("dma_struct_get: index %d\n", i);
            memset(cur, 0, sizeof (struct dma_struct_type));
            cur->used = 1;

            last_unused = i;
            return cur;
        }

        i++;

        if (i >= MAX_STRUCTS) {
            i = 0;
            looped = 1;
        }

        if (looped && i >= last_unused) {
            break;
        }
    }
    assert (0, "Out of DMA map structures!");
    return NULL;
}

static struct dma_struct_type *dma_struct_find(bus_dma_tag_t tag) {
    int i;
    assert (dma_structs != NULL, "Should allocate memory first");

    for (i = 0; i < MAX_STRUCTS; i++) {
        struct dma_struct_type *cur = &dma_structs[i];
        if (cur->used == 1) {
            if (cur->tag == tag) {
                return cur;
            }
        }
    }

    assert (0, "Failed to find tag %p!", tag);
    return NULL;
}

static void dma_struct_free(struct dma_struct_type *cur) {
    assert (cur, "Freeing non-allocated DMA map?");
    assert (cur->used == 1, "DMA map not used?");
    memset (cur, 0, sizeof (struct dma_struct_type));
    return;
}

// Chain our callback with the original one.
// Store the physical address.
// Can't make any assumptions about "arg" so don't use it.

static void bus_dmamap_callback_common(struct dma_struct_type *cur,
                                       bus_dma_segment_t *segs,
                                       int nseg) {
    int i;
    assert(nseg < MAX_SEGS, "Too many segments! %d vs %d\n", nseg, MAX_SEGS);
    
    for (i = 0; i < nseg; i++) {
        cur->ds_addr[i] = segs[i].ds_addr;
        cur->ds_len[i] = segs[i].ds_len;
    }

    cur->nsegs = nseg;
}

static void bus_dmamap_common (struct dma_struct_type *cur) {
    int i;
    for (i = 0; i < cur->nsegs; i++) {
        uprintk ("%s Making segment symbolic: %p, len: %d\n",
                 __func__, cur->ds_addr[i], cur->ds_len[i]);
        s2e_make_dma_symbolic((void *) cur->ds_addr[i],
                              cur->ds_len[i], "DMA segment");
    }
}

static void bus_dmaunmap_common (struct dma_struct_type *cur) {
    int i;
    for (i = 0; i < cur->nsegs; i++) {
        uprintk ("%s Converting to regular symbolic: %p, len: %d\n",
                 __func__, cur->ds_addr[i], cur->ds_len[i]);
        s2e_free_dma_symbolic((void *) cur->ds_addr[i],
                              cur->ds_len[i], "DMA segment");
    }
}

static void bus_dmamap_load_callback1(void *arg,
                                      bus_dma_segment_t *segs,
                                      int nseg,
                                      int error) {
    struct dma_struct_type *cur = arg;
    if (error == 0) {
        bus_dmamap_callback_common(cur, segs, nseg);
    }
    cur->orig_callback1(cur->orig_arg, segs, nseg, error);
}

STATIC int
bus_dmamap_load_MJRcheck(const char *fn,
                         int prepost,
                         int *retval,
                         bus_dma_tag_t *dmat,
                         bus_dmamap_t *map,
                         void **buf,
                         bus_size_t *buflen,
                         bus_dmamap_callback_t **callback,
                         void **callback_arg,
                         int *flags) {
    static struct dma_struct_type *cur;
    check_routine_start();
    if (prepost == 0) {
        cur = dma_struct_get();

        cur->tag = *dmat;
        cur->orig_callback1 = *callback; // Store original callback
        cur->orig_arg = *callback_arg; // Store original argument

        *callback = bus_dmamap_load_callback1; // Substitute our callback
        *callback_arg = cur; // Substitute our argument
    } else if (prepost == 1) {
        if (*retval == 0) {
            bus_dmamap_common(cur);
        } else {
            uprintk ("%s: failure - no symbolic DMA memory here.\n", __func__);
        }

        // Restore original callback though this likely does not matter
        *callback = cur->orig_callback1;
        *callback_arg = callback_arg;
    } else {
        tfassert(0);
    }
    check_routine_end();
    return 0;
}

static void bus_dmamap_load_callback2(void *arg,
                                      bus_dma_segment_t *segs,
                                      int nseg,
                                      bus_size_t mapsize,
                                      int error) {
    struct dma_struct_type *cur = arg;
    if (error == 0) {
        bus_dmamap_callback_common(cur, segs, nseg);
    }
    cur->orig_callback2(cur->orig_arg, segs, nseg, mapsize, error);
}

STATIC int
bus_dmamap_load_mbuf_MJRcheck(const char *fn,
                              int prepost,
                              int *retval,
                              bus_dma_tag_t *dmat,
                              bus_dmamap_t *map,
                              struct mbuf **m0,
                              bus_dmamap_callback2_t **callback,
                              void **callback_arg,
                              int *flags) {
    static struct dma_struct_type *cur;
    check_routine_start();
    if (prepost == 0) {
        cur = dma_struct_get();

        cur->tag = *dmat;
        cur->orig_callback2 = *callback; // Store original callback
        cur->orig_arg = *callback_arg; // Store original argument

        *callback = bus_dmamap_load_callback2; // Substitute our callback
    } else if (prepost == 1) {
        if (*retval == 0) {
            bus_dmamap_common(cur);
        } else {
            uprintk ("%s: failure - no symbolic DMA memory here.\n", __func__);
        }

        // Restore original callback though this likely does not matter
        *callback = cur->orig_callback2;
        *callback_arg = cur->orig_arg;
    } else {
        tfassert(0);
    }
    check_routine_end();
    return 0;
}

STATIC int
bus_dmamap_load_mbuf_sg_MJRcheck(const char *fn,
                                 int prepost,
                                 int *retval,
                                 bus_dma_tag_t *dmat,
                                 bus_dmamap_t *map,
                                 struct mbuf **m0,
                                 bus_dma_segment_t **segs,
                                 int **nsegs,
                                 int *flags) {
    check_routine_start();
    if (prepost == 0) {
    } else if (prepost == 1) {
        int i;
        if (*retval == 0) {
            struct dma_struct_type *cur = dma_struct_get();
            tfassert_detail (**nsegs < MAX_SEGS, "Too many segments! %d vs %d\n", **nsegs, MAX_SEGS);
            
            cur->tag = *dmat;

            cur->nsegs = **nsegs;
            for (i = 0; i < **nsegs; i++) {
                cur->ds_addr[i] = ((*segs)[i]).ds_addr;
                cur->ds_len[i] = ((*segs)[i]).ds_len;
            }

            bus_dmamap_common(cur);
        } else {
            uprintk ("%s: failure - no symbolic DMA memory here.\n", __func__);
        }
    } else {
        tfassert(0);
    }
    check_routine_end();
    return 0;
}

STATIC int
bus_dmamap_load_uio_MJRcheck(const char *fn,
                             int prepost,
                             int *retval,
                             bus_dma_tag_t *dmat,
                             bus_dmamap_t *map,
                             struct uio **uio,
                             bus_dmamap_callback2_t **callback,
                             void **callback_arg,
                             int *flags) {
    check_routine_start();
    tfassert_detail (0, "bus_dmamap_load_uio unsupported");
    check_routine_end();
    return 0;
}

int bus_dmamap_unload_MJRcheck(const char *fn,
                               int prepost,
                               bus_dma_tag_t *dmat,
                               bus_dmamap_t *map) {
    check_routine_start();
    if (prepost == 0) {
    } else if (prepost == 1) {
        struct dma_struct_type *cur = dma_struct_find(*dmat);
        bus_dmaunmap_common(cur);
        dma_struct_free(cur);
    } else {
        tfassert(0);
    }
    check_routine_end();
    return 0;
}

// Other DMA mapping routines
// I don't think we need to do anything with these
// They just allocate the memory -- they don't actually map it for use.
STATIC int
bus_dmamem_alloc_MJRcheck(const char *fn,
                          int prepost,
                          int *retval,
                          bus_dma_tag_t *dmat,
                          void*** vaddr,
                          int *flags,
                          bus_dmamap_t **mapp) {
    check_routine_start();
    if (prepost == 0) {
    } else if (prepost == 1) {
    } else {
        tfassert(0);
    }
    check_routine_end();
    return 0;
}

STATIC int
bus_dmamem_free_MJRcheck(const char *fn,
                         int prepost,
                         int *retval,
                         bus_dma_tag_t *dmat,
                         void **vaddr,
                         bus_dmamap_t *map) {
    check_routine_start();
    if (prepost == 0) {
    } else if (prepost == 1) {
    } else {
        tfassert(0);
    }
    check_routine_end();
    return 0;
}

STATIC int
pci_read_config_MJRcheck(const char *fn,
                         int prepost,
                         uint32_t *retval,
                         device_t *dev,
                         int *reg,
                         int *width) {
    check_routine_start();
    if (prepost == 0) {
    } else if (prepost == 1) {
        s2e_make_symbolic(retval, sizeof(uint32_t), __func__);
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 1; // Don't call kernel implementation
}

STATIC int
pci_write_config_MJRcheck(const char *fn,
                          int prepost,
                          device_t *dev,
                          int *reg,
                          uint32_t *val,
                          int *width) {
    check_routine_start();
    if (prepost == 0) {
    } else if (prepost == 1) {
    } else {
        tfassert (0);
    }
    check_routine_end();
    return 1; // don't run kernel function
}

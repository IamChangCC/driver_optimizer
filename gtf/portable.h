#ifndef PORTABLE_H
#define PORTABLE_H

#ifdef LINUX_MODE
#define mjr_kmalloc_atomic(size) kmalloc ((size), GFP_ATOMIC)
#define mjr_kmalloc_nonatomic(size) kmalloc ((size), GFP_KERNEL)
#define mjr_kfree(ptr) kfree (ptr)
#endif

#ifdef FREEBSD_MODE

#include <sys/errno.h>

#define mjr_kmalloc_atomic(size) malloc ((size), M_DEVBUF, M_NOWAIT)
#define mjr_kmalloc_nonatomic(size) malloc ((size), M_DEVBUF, M_WAITOK)
#define mjr_kfree(ptr) free ((ptr), M_DEVBUF)

// TODO This is probably not right
//#define IS_ERR_VALUE(x) (((unsigned long) (x) < 4096 && (x) > 0) ||
//                         ((unsigned long) (x) >= ((unsigned long) -4095))
//                        )
static inline int IS_ERR_VALUE(unsigned long x);
static inline int IS_ERR(void *ptr);

static inline int IS_ERR_VALUE(unsigned long x) {
    return (x <= ELAST) && (x > 0);
}

static inline int IS_ERR(void *ptr) {
    return ((unsigned long) ptr < 4096 ||
            (unsigned long) ptr == 0 ||
            (unsigned long) ptr > -4096);
}
#endif

#endif

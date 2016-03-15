#ifndef FREEBSD_MODE
#error This should only work in FreeBSD mode
#endif

#include "freebsd_trace_driver.h"
#include "globals.h"

#include <sys/param.h>
#include <sys/module.h>
#include <sys/kernel.h>
#include <sys/systm.h>

static char *READ = "Read";
static char *WRITE = "Write";

// Define this to include extra data in the trace.
//#define INCLUDE_EXTRA

#define PRINT_EXTRA()                                           \
    do {                                                        \
        if (g_trace_level >= 2) {                               \
            printf ("ExtraData: fn: %s, line: %d, op: %s\n",    \
                    fn, line, __func__);                        \
        }                                                       \
    } while (0)

#define PRINT_VALUE(rw, addr, size)                             \
    do {                                                        \
        if (g_trace_level >= 1) {                               \
            printf ("R/W: %s\tVA: %x\tSize: %d\n",              \
                    rw, addr, size);                            \
            PRINT_EXTRA();                                      \
        }                                                       \
    } while (0)

#define PRINT_MULTI(rw, addr, size, src_addr, src_size) \
    do {                                                \
        int i;                                          \
        for (i = 0; i < src_size; i++) {                \
            PRINT_VALUE(rw, addr, size);                \
        }                                               \
    } while (0)

struct resource *
tm_bus_alloc_resource(const char *fn,
                      int line,
                      device_t dev,
                      int type,
                      int *rid,
                      u_long start,
                      u_long end,
                      u_long count,
                      u_int flags) {
    struct resource *ret;
    ret = bus_alloc_resource(dev, type, rid, start, end, count, flags);
    PRINT_EXTRA();
    return ret;
}

struct resource *
tm_bus_alloc_resource_any(const char *fn,
                          int line,
                          device_t dev,
                          int type,
                          int *rid,
                          u_int flags) {
    struct resource *ret;
    ret = bus_alloc_resource_any(dev, type, rid, flags);
    PRINT_EXTRA();
    return ret;
}

void
tm_bus_space_write_1(const char *fn,
                     int line,
                     bus_space_tag_t tag,
                     bus_space_handle_t bsh,
                     bus_size_t offset,
                     u_int8_t value) {
    PRINT_VALUE(WRITE, (unsigned int) offset, 1);
    bus_space_write_1(tag, bsh, offset, value);
    return;
}

void
tm_bus_space_write_2(const char *fn,
                     int line,
                     bus_space_tag_t tag,
                     bus_space_handle_t bsh,
                     bus_size_t offset,
                     u_int16_t value) {
    PRINT_VALUE(WRITE, (unsigned int) offset, 2);
    bus_space_write_2(tag, bsh, offset, value);
    return;
}

void
tm_bus_space_write_4(const char *fn,
                     int line,
                     bus_space_tag_t tag,
                     bus_space_handle_t bsh,
                     bus_size_t offset,
                     u_int32_t value) {
    PRINT_VALUE(WRITE, (unsigned int) offset, 4);
    bus_space_write_4(tag, bsh, offset, value);
    return;
}

u_int8_t
tm_bus_space_read_1(const char *fn,
                    int line,
                    bus_space_tag_t tag,
                    bus_space_handle_t handle,
                    bus_size_t offset) {
    u_int8_t ret;
    ret = bus_space_read_1(tag, handle, offset);
    PRINT_VALUE(READ, (unsigned int) offset, 1);
    return ret;
}

u_int16_t
tm_bus_space_read_2(const char *fn,
                    int line,
                    bus_space_tag_t tag,
                    bus_space_handle_t handle,
                    bus_size_t offset) {
    u_int16_t ret;
    ret = bus_space_read_2(tag, handle, offset);
    PRINT_VALUE(READ, (unsigned int) offset, 2);
    return ret;
}
 
u_int32_t
tm_bus_space_read_4(const char *fn,
                    int line,
                    bus_space_tag_t tag,
                    bus_space_handle_t handle,
                    bus_size_t offset) {
    u_int32_t ret;
    ret = bus_space_read_4(tag, handle, offset);
    PRINT_VALUE(READ, (unsigned int) offset, 4);
    return ret;
}

void
tm_bus_space_read_multi_1(const char *fn,
                          int line,
                          bus_space_tag_t tag,
                          bus_space_handle_t bsh,
                          bus_size_t offset,
                          u_int8_t *addr,
                          size_t count) {
    bus_space_read_multi_1(tag, bsh, offset, addr, count);
    PRINT_MULTI(READ, (unsigned int) offset, 1, addr, count);
    return;
}

void
tm_bus_space_read_multi_2(const char *fn,
                          int line,
                          bus_space_tag_t tag,
                          bus_space_handle_t bsh,
                          bus_size_t offset,
                          u_int16_t *addr,
                          size_t count) {
    bus_space_read_multi_2(tag, bsh, offset, addr, count);
    PRINT_MULTI(READ, (unsigned int) offset, 2, addr, count);
    return;
}

void
tm_bus_space_read_multi_4(const char *fn,
                          int line,
                          bus_space_tag_t tag,
                          bus_space_handle_t bsh,
                          bus_size_t offset,
                          u_int32_t *addr,
                          size_t count) {
    bus_space_read_multi_4(tag, bsh, offset, addr, count);
    PRINT_MULTI(READ, (unsigned int) offset, 4, addr, count);
    return;
}

void
tm_bus_space_write_multi_1(const char *fn,
                           int line,
                           bus_space_tag_t tag,
                           bus_space_handle_t bsh,
                           bus_size_t offset,
                           const u_int8_t *addr,
                           size_t count) {
    PRINT_MULTI(WRITE, (unsigned int) offset, 1, addr, count);
    bus_space_write_multi_1(tag, bsh, offset, addr, count);
    return;
}

void
tm_bus_space_write_multi_2(const char *fn,
                           int line,
                           bus_space_tag_t tag,
                           bus_space_handle_t bsh,
                           bus_size_t offset,
                           const u_int16_t *addr,
                           size_t count) {
    PRINT_MULTI(WRITE, (unsigned int) offset, 2, addr, count);
    bus_space_write_multi_2(tag, bsh, offset, addr, count);
    return;
}

void
tm_bus_space_write_multi_4(const char *fn,
                           int line,
                           bus_space_tag_t tag,
                           bus_space_handle_t bsh,
                           bus_size_t offset,
                           const u_int32_t *addr,
                           size_t count) {
    PRINT_MULTI(WRITE, (unsigned int) offset, 4, addr, count);
    bus_space_write_multi_4(tag, bsh, offset, addr, count);
    return;
}

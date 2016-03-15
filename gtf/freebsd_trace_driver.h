#ifndef TRACEMOD_DRIVER_H
#define TRACEMOD_DRIVER_H

#include <sys/param.h>
#include <sys/module.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/bus.h>
#include <sys/types.h>
#include <machine/bus.h>

struct resource *
tm_bus_alloc_resource(const char *fn,
                      int line,
                      device_t dev,
                      int type,
                      int *rid,
                      u_long start,
                      u_long end,
                      u_long count,
                      u_int flags);
struct resource *
tm_bus_alloc_resource_any(const char *fn,
                          int line,
                          device_t dev,
                          int type,
                          int *rid,
                          u_int flags);

void
tm_bus_space_write_1(const char *fn,
                     int line,
                     bus_space_tag_t tag,
                     bus_space_handle_t bsh,
                     bus_size_t offset,
                     u_int8_t value);

void
tm_bus_space_write_2(const char *fn,
                     int line,
                     bus_space_tag_t tag,
                     bus_space_handle_t bsh,
                     bus_size_t offset,
                     u_int16_t value);

void
tm_bus_space_write_4(const char *fn,
                     int line,
                     bus_space_tag_t tag,
                     bus_space_handle_t bsh,
                     bus_size_t offset,
                     u_int32_t value);

u_int8_t
tm_bus_space_read_1(const char *fn,
                    int line,
                    bus_space_tag_t tag,
                    bus_space_handle_t handle,
                    bus_size_t offset);

u_int16_t
tm_bus_space_read_2(const char *fn,
                    int line,
                    bus_space_tag_t tag,
                    bus_space_handle_t handle,
                    bus_size_t offset);

u_int32_t
tm_bus_space_read_4(const char *fn,
                    int line,
                    bus_space_tag_t tag,
                    bus_space_handle_t handle,
                    bus_size_t offset);

void
tm_bus_space_read_multi_1(const char *fn,
                          int line,
                          bus_space_tag_t tag,
                          bus_space_handle_t bsh,
                          bus_size_t offset,
                          u_int8_t *addr,
                          size_t count);

void
tm_bus_space_read_multi_2(const char *fn,
                          int line,
                          bus_space_tag_t tag,
                          bus_space_handle_t bsh,
                          bus_size_t offset,
                          u_int16_t *addr,
                          size_t count);

void
tm_bus_space_read_multi_4(const char *fn,
                          int line,
                          bus_space_tag_t tag,
                          bus_space_handle_t bsh,
                          bus_size_t offset,
                          u_int32_t *addr,
                          size_t count);

void
tm_bus_space_write_multi_1(const char *fn,
                           int line,
                           bus_space_tag_t tag,
                           bus_space_handle_t bsh,
                           bus_size_t offset,
                           const u_int8_t *addr,
                           size_t count);

void
tm_bus_space_write_multi_2(const char *fn,
                           int line,
                           bus_space_tag_t tag,
                           bus_space_handle_t bsh,
                           bus_size_t offset,
                           const u_int16_t *addr,
                           size_t count);

void
tm_bus_space_write_multi_4(const char *fn,
                           int line,
                           bus_space_tag_t tag,
                           bus_space_handle_t bsh,
                           bus_size_t offset,
                           const u_int32_t *addr,
                           size_t count);

#ifdef TM_ENABLE

#define bus_alloc_resource(dev, type, rid, start, end, count, flags) \
    tm_bus_alloc_resource(__func__, __LINE__, dev, type, rid, start, end, count, flags)

#define bus_alloc_resource_any(dev, type, rid, flags)  \
    tm_bus_alloc_resource_any(__func__, __LINE__, dev, type, rid, flags)

#define bus_space_write_1(tag, bsh, offset, value)      \
    tm_bus_space_write_1(__func__, __LINE__, tag, bsh, offset, value)

#define bus_space_write_2(tag, bsh, offset, value)     \
    tm_bus_space_write_2(__func__, __LINE__, tag, bsh, offset, value)

#define bus_space_write_4(tag, bsh, offset, value)      \
    tm_bus_space_write_4(__func__, __LINE__, tag, bsh, offset, value)

#define bus_space_read_1(tag, handle, offset)   \
    tm_bus_space_read_1(__func__, __LINE__, tag, handle, offset)

#define bus_space_read_2(tag, handle, offset)   \
    tm_bus_space_read_2(__func__, __LINE__, tag, handle, offset)

#define bus_space_read_4(tag, handle, offset)   \
    tm_bus_space_read_4(__func__, __LINE__, tag, handle, offset)

#define bus_space_read_multi_1(tag, bsh, offset, addr, count)   \
    tm_bus_space_read_multi_1(__func__, __LINE__, tag, bsh, offset, addr, count)

#define bus_space_read_multi_2(tag, bsh, offset, addr, count)   \
    tm_bus_space_read_multi_2(__func__, __LINE__, tag, bsh, offset, addr, count)

#define bus_space_read_multi_4(tag, bsh, offset, addr, count)   \
    tm_bus_space_read_multi_4(__func__, __LINE__, tag, bsh, offset, addr, count)

#endif

#endif

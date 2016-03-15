#ifndef GLOBALS_H
#define GLOBALS_H

#include "s2e.h"
#include "portable.h"

#ifdef FREEBSD_MODE
//
// Globals defined in init_module.c
//
// extern int g_trace_level;
#endif // FREEBSD_MODE

// Global variables specified at command line (init_module.c)
#define MAX_TRACKPERF 10

extern int g_sym_retval;
extern int g_sym_pci;
extern int g_sym_params;
extern char g_sym_starting_fn[];
extern char g_sym_primary_fn[];
extern int g_sym_kill_carrier_off;
extern int g_sym_slow_timers;
extern int g_sym_force_receive;
extern int g_sym_enable_tracing;

extern int g_sym_num_trackperf_fn; // Count of g_sym_trackperf_fn
extern char *g_sym_trackperf_fn[];
extern int g_sym_num_trackperf_fn_flags; // Count of g_sym_trackperfn_fn_flags
extern int g_sym_trackperf_fn_flags[];
extern int g_sym_trackperf_irq; // 0 if not tracking interrupt handlers, 1 otherwise

#ifdef LINUX_MODE
extern int g_platform_enable;
extern char g_platform_name[];
extern int g_platform_nr;
extern unsigned long g_platform_rflags[];
extern char *g_platform_rnames[];

extern int g_i2c_enable;
extern unsigned short g_i2c_chip_addr[];
extern unsigned long g_i2c_functionality;
extern char *g_i2c_names[];

extern int g_spi_enable;
extern char g_spi_name[];
#endif // LINUX_MODE

// enter_exit.c:
extern int bad_path;

#include "uprintk.h"

//////////////////////////////////////////////////////////////////////
// Defined in enter_exit.c
//////////////////////////////////////////////////////////////////////
enum symdrive_DIRECTION
{
    ENTRANCE = 0,
    EXIT = 1
};

enum symdrive_WRAPPER_TYPE
{
    STUBWRAPPER = 0,
    PREPOSTFN = 1
};

int gen_enter_driver (const char *fn,
                      int line,
                      enum symdrive_WRAPPER_TYPE wrapper_type,
                      int retval_valid,
                      unsigned long retval,
                      const char *interesting_fn,
                      void (*gen_register_all_param)(void));
int gen_exit_driver (const char *fn,
                     int line,
                     enum symdrive_WRAPPER_TYPE wrapper_type,
                     int retval_valid,
                     unsigned long retval,
                     const char *interesting_fn,
                     void (*gen_register_all_param)(void));
int gen_make_symbolic_ptr (const char *fn,
                           int line,
                           void **ptr_retval);
int gen_make_symbolic_int (const char *fn,
                           int line,
                           void *int_retval,
                           int int_retval_size);

//////////////////////////////////////////////////////////////////////
// Defined in function_tracker.c
//////////////////////////////////////////////////////////////////////
int initialize_function_tracker(void);
void free_function_tracker(void);
void gen_convert_fn (void **orig_fn_ptr,
                     const char *driver_fn_name,
                     const char *str1,
                     const char *str2);
void gen_register_one (void *wrapper_fn,
                       void *driver_fn,
                       const char *wrapper_fn_str,
                       const char *driver_fn_str);
const void *gen_lookup_MJRcheck (const char *wrapper_type, void *driver_fn_ptr);

//////////////////////////////////////////////////////////////////////
// Defined in global_lock.c
//////////////////////////////////////////////////////////////////////
int initialize_global_lock(void);
void acquire_global_lock(void);
void release_global_lock(void);
void assert_global_lock(void);
void free_global_lock(void);
//void acquire_driver_lock(void);
//void release_driver_lock(void);

//////////////////////////////////////////////////////////////////////
// Defined in test_state.c
//////////////////////////////////////////////////////////////////////
void objs_enum_print(void);
void objs_enum_print_new(void);

//////////////////////////////////////////////////////////////////////
// Defined in enter_exit.c
//////////////////////////////////////////////////////////////////////
const char *driver_dump_stack(int print);

//////////////////////////////////////////////////////////////////////
// Defined in symbol_lookup.c
//////////////////////////////////////////////////////////////////////
const void *symbol_lookup (const char *name);

//////////////////////////////////////////////////////////////////////
// Defined in misc_checks/freebsd.c
//////////////////////////////////////////////////////////////////////
int initialize_dma_map(void);
void free_dma_map(void);

///////////////////////////////////////////////////////////////////////////////
//
// What do we do if an assertion fails? Continue along other paths
//
///////////////////////////////////////////////////////////////////////////////
#ifdef LINUX_MODE
#define tfassert_detail(x, msg, ...)                                    \
    if (!(x)) {                                                         \
        char temp1[128];                                                \
        char temp2[256];                                                \
                                                                        \
        objs_enum_print();                                              \
        objs_enum_print_new();                                          \
        sprintf (temp1, KERN_ALERT "Assertion %s:%d from %s, %s\n",  __func__, __LINE__, fn, msg); \
        sprintf (temp2, temp1, ## __VA_ARGS__);                         \
        printk (temp2);                                                 \
        dump_stack();                                                   \
        driver_dump_stack(1);                                           \
                                                                        \
        s2e_kill_state (0, 1, temp2);                                   \
    }

#define tfassert(x) tfassert_detail(x, "%s\n", fn)

#define assert(x, msg, ...)                                             \
    if (!(x)) {                                                         \
        char temp1[128];                                                \
        char temp2[256];                                                \
                                                                        \
        objs_enum_print();                                              \
        objs_enum_print_new();                                          \
        sprintf (temp1, KERN_ALERT "Assertion %s:%d, %s\n",  __func__, __LINE__, msg); \
        sprintf (temp2, temp1, ## __VA_ARGS__);                         \
        printk (temp2);                                                 \
        dump_stack();                                                   \
        driver_dump_stack(1);                                           \
                                                                        \
        s2e_kill_state (0, 1, temp2);                                   \
    }

#define tfassert_no_detail(x, msg, ...)                                 \
    if (!(x)) {                                                         \
        char temp1[128];                                                \
        char temp2[256];                                                \
                                                                        \
        sprintf (temp1, KERN_ALERT "Assertion %s:%d from %s, %s\n",  __func__, __LINE__, fn, msg); \
        sprintf (temp2, temp1, ## __VA_ARGS__);                         \
        printk (temp2);                                                 \
        driver_dump_stack(1);                                           \
        s2e_kill_state (0, 1, temp2);                                   \
    }
#endif // LINUX_MODE

#ifdef FREEBSD_MODE

#define tfassert_detail(x, msg, ...)                                    \
    if (!(x)) {                                                         \
        char temp1[128];                                                \
        char temp2[256];                                                \
                                                                        \
        objs_enum_print();                                              \
        objs_enum_print_new();                                          \
        sprintf (temp1, "Assertion %s:%d from %s, %s\n",  __func__, __LINE__, fn, msg); \
        sprintf (temp2, temp1, ## __VA_ARGS__);                         \
        printf (temp2);                                                 \
        /*dump_stack();*/                                               \
        /*driver_dump_stack(1);*/                                       \
                                                                        \
        s2e_kill_state (0, 1, temp2);                                   \
    }

#define tfassert(x) tfassert_detail(x, "%s\n", fn)

#define assert(x, msg, ...)                                             \
    if (!(x)) {                                                         \
        char temp1[128];                                                \
        char temp2[256];                                                \
                                                                        \
        objs_enum_print();                                              \
        objs_enum_print_new();                                          \
        sprintf (temp1, "Assertion %s:%d, %s\n",  __func__, __LINE__, msg); \
        sprintf (temp2, temp1, ## __VA_ARGS__);                         \
        printf (temp2);                                                 \
        /*dump_stack();*/                                               \
        /*driver_dump_stack(1);*/                                       \
                                                                        \
        s2e_kill_state (0, 1, temp2);                                   \
    }

#define tfassert_no_detail(x, msg, ...)                                 \
    if (!(x)) {                                                         \
        char temp1[128];                                                \
        char temp2[256];                                                \
                                                                        \
        sprintf (temp1, KERN_ALERT "Assertion %s:%d from %s, %s\n",  __func__, __LINE__, fn, msg); \
        sprintf (temp2, temp1, ## __VA_ARGS__);                         \
        printk (temp2);                                                 \
        driver_dump_stack(1);                                           \
        s2e_kill_state (0, 1, temp2);                                   \
    }
#endif

#endif

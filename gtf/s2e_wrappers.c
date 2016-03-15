#include "s2e.h"

#ifdef LINUX_MODE
#include <linux/module.h>
#endif

#ifdef FREEBSD_MODE
#define STATIC
#endif

#ifdef LINUX_MODE
#define STATIC static
#endif

#ifdef FREEBSD_MODE
int s2e_version_wrapper(void);
void s2e_message_wrapper(const char* message);
void s2e_warning_wrapper(const char* message);
void s2e_print_expression_wrapper(const char* name, int expression);
void s2e_enable_forking_wrapper(void);
void s2e_disable_forking_wrapper(void);
unsigned s2e_get_path_id_wrapper(void);
void s2e_make_symbolic_wrapper(void* buf, int size, const char* name);

// MJR (24 in all) ----------->
void s2e_make_dma_symbolic_wrapper(void* buf, int size, const char* name);
void s2e_free_dma_symbolic_wrapper(void* buf, int size, const char* name);
void s2e_prioritize_wrapper(int line);
void s2e_deprioritize_wrapper(int line);
void s2e_loop_before_wrapper(int line, int call_id);
void s2e_loop_body_wrapper(int line, int call_id);
void s2e_loop_after_wrapper(int line, int call_id);
void s2e_concretize_kill_wrapper(int line);
void s2e_concretize_all_wrapper(int line);
void s2e_kill_all_others_wrapper(int line);
void s2e_driver_call_stack_wrapper(int line, int depth);
void s2e_favor_successful_wrapper(int line, int successful);
// hole
void s2e_reset_priorities_wrapper(int line);
// hole
void s2e_enable_tracing_wrapper(int line);
void s2e_disable_tracing_wrapper(int line);
void s2e_enter_function_wrapper(int line, const char *fn, int wrapper_type);
void s2e_exit_function_wrapper(int line, const char *fn, int wrapper_type);
int s2e_is_symbolic_symdrive_wrapper(int line, int value);
void s2e_success_path_wrapper(int line, const char *fn, int success);
void s2e_enter_block_wrapper(int line, const char *fn, int total_blocks, int cur_block);
void s2e_primary_fn_wrapper(int line, const char *fn);
void s2e_enable_trackperf_wrapper(int line, int flags);
void s2e_disable_trackperf_wrapper(int line, int flags);
void s2e_trackperf_fn_wrapper(int line, const char *fn, int flags);
// MJR <-------------

void s2e_concretize_wrapper(void* buf, int size);
void s2e_get_example_wrapper(void* buf, int size);
unsigned s2e_get_example_uint_wrapper(unsigned val);
void s2e_kill_state_wrapper(int kill_all, int status, const char* message);
void s2e_disable_timer_interrupt_wrapper(void);
void s2e_enable_timer_interrupt_wrapper(void);
void s2e_disable_all_apic_interrupts_wrapper(void);
void s2e_enable_all_apic_interrupts_wrapper(void);
int s2e_get_ram_object_bits_wrapper(void);
void s2e_merge_point_wrapper(void);
int s2e_open_wrapper(const char* fname);
int s2e_close_wrapper(int fd);
int s2e_read_wrapper(int fd, char* buf, int count);
void s2e_rawmon_loadmodule_wrapper(const char *name, unsigned loadbase, unsigned size);
void _s2e_assert_wrapper(int b, const char *expression );
#endif

STATIC int s2e_version_wrapper(void) {
    return s2e_version();
}
STATIC void s2e_message_wrapper(const char* message) {
    s2e_message(message);
}
STATIC void s2e_warning_wrapper(const char* message) {
    s2e_warning(message);
}
STATIC void s2e_print_expression_wrapper(const char* name, int expression) {
    s2e_print_expression(name, expression);
}
STATIC void s2e_enable_forking_wrapper(void) {
    s2e_enable_forking();
}
STATIC void s2e_disable_forking_wrapper(void) {
    s2e_disable_forking();
}
STATIC unsigned s2e_get_path_id_wrapper(void) {
    return s2e_get_path_id();
}
STATIC void s2e_make_symbolic_wrapper(void* buf, int size, const char* name) {
    s2e_make_symbolic(buf, size, name);
}

// MJR ---------->
STATIC void s2e_make_dma_symbolic_wrapper(void* buf, int size, const char* name) {
    s2e_make_dma_symbolic(buf, size, name);
}
STATIC void s2e_free_dma_symbolic_wrapper(void* buf, int size, const char* name) {
    s2e_free_dma_symbolic(buf, size, name);
}
STATIC void s2e_prioritize_wrapper(int line) {
    s2e_prioritize(line);
}
STATIC void s2e_deprioritize_wrapper(int line) {
    s2e_deprioritize(line);
}
STATIC void s2e_loop_before_wrapper(int line, int call_id) {
    s2e_loop_before(line, call_id);
}
STATIC void s2e_loop_body_wrapper(int line, int call_id) {
    s2e_loop_body(line, call_id);
}
STATIC void s2e_loop_after_wrapper(int line, int call_id) {
    s2e_loop_after(line, call_id);
}
STATIC void s2e_concretize_kill_wrapper(int line) {
    s2e_concretize_kill(line);
}
STATIC void s2e_concretize_all_wrapper(int line) {
    s2e_concretize_all(line);
}
STATIC void s2e_kill_all_others_wrapper(int line) {
    s2e_kill_all_others(line);
}
STATIC void s2e_driver_call_stack_wrapper(int line, int depth) {
    s2e_driver_call_stack(line, depth);
}
STATIC void s2e_favor_successful_wrapper(int line, int successful) {
    s2e_favor_successful(line, successful);
}

// hole

STATIC void s2e_reset_priorities_wrapper(int line) {
    s2e_reset_priorities(line);
}

// hole

STATIC void s2e_enable_tracing_wrapper(int line) {
    s2e_enable_tracing(line);
}
STATIC void s2e_disable_tracing_wrapper(int line) {
    s2e_disable_tracing(line);
}
STATIC void s2e_enter_function_wrapper(int line, const char *fn, int wrapper_type) {
    s2e_enter_function(line, fn, wrapper_type);
}
STATIC void s2e_exit_function_wrapper(int line, const char *fn, int wrapper_type) {
    s2e_exit_function(line, fn, wrapper_type);
}
STATIC int s2e_is_symbolic_symdrive_wrapper(int line, int value) {
    return s2e_is_symbolic_symdrive(line, value);
}
STATIC void s2e_success_path_wrapper(int line, const char *fn, int success) {
    s2e_success_path(line, fn, success);
}
STATIC void s2e_enter_block_wrapper(int line, const char *fn, int total_blocks, int cur_block) {
    s2e_enter_block(line, fn, total_blocks, cur_block);
}
STATIC void s2e_primary_fn_wrapper(int line, const char *fn) {
    s2e_primary_fn(line, fn);
}
STATIC void s2e_enable_trackperf_wrapper(int line, int flags) {
    s2e_enable_trackperf(line, flags);
}
STATIC void s2e_disable_trackperf_wrapper(int line, int flags) {
    s2e_disable_trackperf(line, flags);
}
STATIC void s2e_trackperf_fn_wrapper(int line, const char *fn, int flags) {
    s2e_trackperf_fn(line, fn, flags);
}
// <---------- MJR

STATIC void s2e_concretize_wrapper(void* buf, int size) {
    s2e_concretize(buf, size);
}
STATIC void s2e_get_example_wrapper(void* buf, int size) {
    s2e_get_example(buf, size);
}
STATIC unsigned s2e_get_example_uint_wrapper(unsigned val) {
    return s2e_get_example_uint(val);
}
STATIC void s2e_kill_state_wrapper(int kill_all, int status, const char* message) {
    s2e_kill_state(kill_all, status, message);
}
STATIC void s2e_disable_timer_interrupt_wrapper(void) {
    s2e_disable_timer_interrupt();
}
STATIC void s2e_enable_timer_interrupt_wrapper(void) {
    s2e_enable_timer_interrupt();
}
STATIC void s2e_disable_all_apic_interrupts_wrapper(void) {
    s2e_disable_all_apic_interrupts();
}
STATIC void s2e_enable_all_apic_interrupts_wrapper(void) {
    s2e_enable_all_apic_interrupts();
}
STATIC int s2e_get_ram_object_bits_wrapper(void) {
    return s2e_get_ram_object_bits();
}
STATIC void s2e_merge_point_wrapper(void) {
    s2e_merge_point();
}
STATIC int s2e_open_wrapper(const char* fname) {
    return s2e_open(fname);
}
STATIC int s2e_close_wrapper(int fd) {
    return s2e_close(fd);
}
STATIC int s2e_read_wrapper(int fd, char* buf, int count) {
    return s2e_read(fd, buf, count);
}
STATIC void s2e_rawmon_loadmodule_wrapper(const char *name, unsigned loadbase, unsigned size) {
    s2e_rawmon_loadmodule(name, loadbase, size);
}
STATIC void _s2e_assert_wrapper(int b, const char *expression ) {
    _s2e_assert(b, expression);
}

#ifdef LINUX_MODE
EXPORT_SYMBOL(s2e_version_wrapper);
EXPORT_SYMBOL(s2e_message_wrapper);
EXPORT_SYMBOL(s2e_warning_wrapper);
EXPORT_SYMBOL(s2e_print_expression_wrapper);
EXPORT_SYMBOL(s2e_enable_forking_wrapper);
EXPORT_SYMBOL(s2e_disable_forking_wrapper);
EXPORT_SYMBOL(s2e_get_path_id_wrapper);
EXPORT_SYMBOL(s2e_make_symbolic_wrapper);

// MJR -------------------->
EXPORT_SYMBOL(s2e_make_dma_symbolic_wrapper);
EXPORT_SYMBOL(s2e_free_dma_symbolic_wrapper);
EXPORT_SYMBOL(s2e_prioritize_wrapper);
EXPORT_SYMBOL(s2e_deprioritize_wrapper);
EXPORT_SYMBOL(s2e_loop_before_wrapper);
EXPORT_SYMBOL(s2e_loop_body_wrapper);
EXPORT_SYMBOL(s2e_loop_after_wrapper);
EXPORT_SYMBOL(s2e_concretize_kill_wrapper);
EXPORT_SYMBOL(s2e_concretize_all_wrapper);
EXPORT_SYMBOL(s2e_kill_all_others_wrapper);
EXPORT_SYMBOL(s2e_driver_call_stack_wrapper);
EXPORT_SYMBOL(s2e_favor_successful_wrapper);
// hole
EXPORT_SYMBOL(s2e_reset_priorities_wrapper);
// hole
EXPORT_SYMBOL(s2e_enable_tracing_wrapper);
EXPORT_SYMBOL(s2e_disable_tracing_wrapper);
EXPORT_SYMBOL(s2e_enter_function_wrapper);
EXPORT_SYMBOL(s2e_exit_function_wrapper);
EXPORT_SYMBOL(s2e_is_symbolic_symdrive_wrapper);
EXPORT_SYMBOL(s2e_success_path_wrapper);
EXPORT_SYMBOL(s2e_enter_block_wrapper);
EXPORT_SYMBOL(s2e_primary_fn_wrapper);
EXPORT_SYMBOL(s2e_enable_trackperf_wrapper);
EXPORT_SYMBOL(s2e_disable_trackperf_wrapper);
EXPORT_SYMBOL(s2e_trackperf_fn_wrapper);
// MJR <------------------

EXPORT_SYMBOL(s2e_concretize_wrapper);
EXPORT_SYMBOL(s2e_get_example_wrapper);
EXPORT_SYMBOL(s2e_get_example_uint_wrapper);
EXPORT_SYMBOL(s2e_kill_state_wrapper);
EXPORT_SYMBOL(s2e_disable_timer_interrupt_wrapper);
EXPORT_SYMBOL(s2e_enable_timer_interrupt_wrapper);
EXPORT_SYMBOL(s2e_disable_all_apic_interrupts_wrapper);
EXPORT_SYMBOL(s2e_enable_all_apic_interrupts_wrapper);
EXPORT_SYMBOL(s2e_get_ram_object_bits_wrapper);
EXPORT_SYMBOL(s2e_merge_point_wrapper);
EXPORT_SYMBOL(s2e_open_wrapper);
EXPORT_SYMBOL(s2e_close_wrapper);
EXPORT_SYMBOL(s2e_read_wrapper);
EXPORT_SYMBOL(s2e_rawmon_loadmodule_wrapper);
EXPORT_SYMBOL(_s2e_assert_wrapper);
#endif

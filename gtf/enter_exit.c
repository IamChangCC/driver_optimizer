#include "globals.h"
#include "hashtable.h"
#include "test_state.h"
#include "misc_checks/irq.h"
#include "symdrive_annotations.h"

#ifdef LINUX_MODE
#include <linux/module.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/compiler.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#endif

#ifdef FREEBSD_MODE
#include <sys/param.h>
#include <sys/module.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/malloc.h>
#endif

// 0 if things are normal
// 1 if we are doing symbolic execution with
//   unexpected kernel return values.  We want
//   to kill off any path that returns to the kernel
//   along one of these invalid "symbolic" branches
//   and only keep the path that involves the actual
//   kernel function return value.
int bad_path = 0;

// driver_call_stack:  a count of how many times a driver function appears
// on the call stack, e.g.:
// init_module -> probe would be 2.
static int driver_call_stack = 0;

// Strings on stack -- pointers to driver function names
#define driver_call_stack_max 32
static const char *driver_call_stack_str[32];
#define call_stack_buffer_size 32768
static char *call_stack_buffer1;
static char *call_stack_buffer2;

//
// Annotation specified manually
//
static int g_symdrive_annotations = 0;

// BUG FIX FOR S2E TODO - related to APIC interrupts
// Need to allow for re-entrant calls for certain cleanup routines.
// We should just leave APIC enabled all the time I'd guess
// static int cleanup_path = 0;

// Interesting functions
#if 0
#define MAX_INTERESTING_PER_LINE 2
#define MAX_INTERESTING_LINES 200000 /* See also MAX_IRQ_LINES */
static int interesting_function_started = 0;
static int interesting_function_success = 0;
static char *interesting_function_lines = NULL;
#endif

int initialize_enter_exit(void) {
#if 0
    interesting_function_lines = vmalloc (sizeof (unsigned char) * MAX_INTERESTING_LINES);
    assert (interesting_function_lines != NULL, "Failed -- insufficient memory for interesting_function_lines\n");
    memset (interesting_function_lines, 0, sizeof (unsigned char) * MAX_INTERESTING_LINES);
#endif
    call_stack_buffer1 = mjr_kmalloc_nonatomic(call_stack_buffer_size);
    call_stack_buffer2 = mjr_kmalloc_nonatomic(call_stack_buffer_size);
    if (call_stack_buffer1 == NULL || call_stack_buffer2 == NULL) {
        panic ("Failed in %s\n", __func__);
    }
    return 0;
}

void free_enter_exit(void) {
#if 0
    if (interesting_function_lines != NULL) {
        vfree(interesting_function_lines);
    }
    interesting_function_lines = NULL;
#endif
    mjr_kfree(call_stack_buffer1);
    mjr_kfree(call_stack_buffer2);
}

static void push_driver_call_stack(const char *fn) {
    driver_call_stack_str[driver_call_stack] = fn;
}

static void pop_driver_call_stack(const char *fn) {
    int i;
    // We need this machinery to deal with cases in which
    // the kernel invokes some driver function while some
    // other driver function continues to execute.  The idea
    // is that this call stack may not be exactly correct
    // since multiple threads are executing simultaneously
    // but this approach should give us something reasonably
    // coherent.
    for (i = driver_call_stack_max - 1; i >= 0; i--) {
        if (driver_call_stack_str[i] != NULL) {
            // Shift everything back.
            // Don't just set to NULL since we'll otherwise
            // have problems with the driver_call_stack
            // index.
            if (strcmp (driver_call_stack_str[i], fn) == 0) {
                int j;
                for (j = i; j < driver_call_stack_max - 1; j++) {
                    driver_call_stack_str[j] = driver_call_stack_str[j + 1];
                }
                driver_call_stack_str[driver_call_stack_max - 1] = NULL;
                return;
            }
        }
    }

    tfassert_detail(0, "pop_driver_call_stack: fn %s not found", fn);
}

static void manage_call_stack(const char *fn, int line, int add, enum symdrive_WRAPPER_TYPE wrapper_type) {
    const char *buffer1;
    const char *buffer2;

    if (add == -1 && wrapper_type == STUBWRAPPER) {
        buffer1 = "Exiting driver, entering kernel function: ";
    } else if (add == 1 && wrapper_type == STUBWRAPPER) {
        buffer1 = "Entering driver, exiting kernel function: ";
    } else if (add == -1 && wrapper_type == PREPOSTFN) {
        buffer1 = "Exiting driver function: ";
    } else if (add == 1 && wrapper_type == PREPOSTFN) {
        buffer1 = "Entering driver function: ";
    } else {
        buffer1 = "";
        tfassert_detail (0, "Inconsistent add/wrapper_type: %d %d\n", add, wrapper_type);
    }

    tfassert_detail(driver_call_stack >= 0 &&
                    driver_call_stack < driver_call_stack_max,
                    "driver_call_stack = %d\n", driver_call_stack);

    if (wrapper_type == STUBWRAPPER) {
        // kernel function
        if (add < 0) {
            push_driver_call_stack(fn);
        }
        driver_call_stack -= add;
        if (add > 0) {
            pop_driver_call_stack(fn);
        }
    } else if (wrapper_type == PREPOSTFN) {
        // driver function
        if (add > 0) {
            push_driver_call_stack(fn);
        }
        driver_call_stack += add;
        if (add < 0) {
            pop_driver_call_stack(fn);
        }
    } else {
        tfassert_detail (0, "Failed in test_framework verify because wrapper_type = %d\n", wrapper_type);
    }
    
    tfassert_detail(driver_call_stack >= 0 &&
                    driver_call_stack < driver_call_stack_max,
                    "driver_call_stack = %d\n", driver_call_stack);

    buffer2 = driver_dump_stack(0);
    uprintk ("%s %s.  %s\n", buffer1, fn, buffer2);
}

// This function is primarily a sanity check
static void verify_invariants(const char *fn, int line, int add, enum symdrive_WRAPPER_TYPE wrapper_type) {
    // uprintk ("Verify %s:%d, add %d, driver_call_stack before: %d\n", fn, line, add, driver_call_stack);
    manage_call_stack(fn, line, add, wrapper_type);

    // driver_call_stack has no "real" upper bound, only a lower bound.
    if (driver_call_stack < 0) {
        tfassert_detail (0, "driver_call_stack: %s/%d, %d - error 3\n",
                         fn, line, driver_call_stack);
    }

    if (driver_call_stack >= driver_call_stack_max - 1) {
        tfassert_detail (0, "driver_call_stack: %s/%d, %d - error 4\n",
                         fn, line, driver_call_stack);
    }

    // Notify hypervisor of the change
    s2e_driver_call_stack (line, driver_call_stack);
}

// Called when:
// - we are exiting an epwrapper
// - the return value is successful
// - symbolic kernel retvals are enabled
static void verify_symretval(const char *fn,
                             int line,
                             unsigned long retval) {
    if (bad_path) {
        // This path was set up artificially
        // The kernel did something other than what the driver
        // thinks it did.
        uprintk ("At %s:%d exit point with retval %ld:  this is a bad path\n",
                 fn, line, s2e_get_example_uint (retval));
        s2e_kill_state(1, 0, "Bad path returning to kernel");
        return;
    }

    // This path is genuine in the sense that the kernel actually did
    // what the driver thinks it did.
    if (driver_call_stack >= 1) {
        // Path is genuine and there are more driver functions on call stack
        uprintk ("At %s:%d exit point with retval %ld:  successful path but more to go ...\n",
                 fn, line, s2e_get_example_uint (retval));
    } else {
        // Path is genuine and there are no more driver functions on call stack
        uprintk ("At %s:%d exit point with retval %ld:  successful path\n",
                 fn, line, s2e_get_example_uint (retval));
        //tfassert_detail (0, "Good path returning to kernel - reducing state explosion");
        if (g_sym_retval == 1) {
            s2e_kill_state(1, 0, "Good path returning to kernel - reducing state explosion");
        } else if (g_sym_retval == 2) {
            uprintk ("Good path completed -- deprioritizing...");
            s2e_deprioritize(line);
        } else {
            tfassert_detail(0, "Expected g_sym_retval not to be %d\n", g_sym_retval);
        }
    }
}

static void verify_retval (const char *fn,
                           int line,
                           enum symdrive_WRAPPER_TYPE wrapper_type,
                           enum symdrive_DIRECTION dir,
                           int retval_valid,
                           unsigned long retval) {
    tfassert (wrapper_type == STUBWRAPPER || wrapper_type == PREPOSTFN);
    tfassert (dir == ENTRANCE || dir == EXIT);
    
    // Precondition.  We're either entering or leaving the driver.
    tfassert (driver_call_stack == 0);

    if (wrapper_type == PREPOSTFN && dir == EXIT) {
        if (retval_valid) {
            if (g_symdrive_annotations == 0) {
                // Proceed normally
            } else {
                // Convert retval to success
                uprintk ("At %s:%d exit point with retval %ld:  allowing our path per annotation\n",
                         fn, line, s2e_get_example_uint (retval));
                g_symdrive_annotations = 0;
                retval = 0;
            }

            if (IS_ERR_VALUE(retval)) {
                uprintk ("At %s:%d exit point with retval %ld:  terminating our path\n",
                         fn, line, s2e_get_example_uint (retval));
                s2e_kill_state(0, 0, "Test complete:  failure detected on this path.");
            }

            if (!IS_ERR_VALUE(retval)) {
                // Driver entry point completed successfully
                if (g_sym_retval == 0) {
                    uprintk ("At %s:%d exit point with retval %ld:  calling s2e_concretize_kill\n",
                             fn, line, s2e_get_example_uint (retval));
                    s2e_concretize_kill (line);
                } else if (g_sym_retval == 1 || g_sym_retval == 2) {
                    verify_symretval(fn, line, retval);
                } else {
                    tfassert_detail(0, "g_sym_retval should not be %d\n", g_sym_retval);
                }
            }
        } else {
            uprintk ("At %s:%d exit point with no retval:  calling s2e_concretize_kill\n",
                     fn, line);
            s2e_concretize_kill (line);
            uprintk ("At %s:%d exit point with no retval:  Killed everything\n",
                     fn, line);
        }
    }

    if (wrapper_type == STUBWRAPPER && dir == ENTRANCE) {
        // In this case, we're entering back into the driver
        // from a kernel function call.
        // Does not matter -- kernel function was successful, or not.

        //s2e_prioritize(line);
    }
}

static void handle_interesting_function(const char *fn,
                                        int line,
                                        enum symdrive_WRAPPER_TYPE wrapper_type,
                                        int retval_valid,
                                        unsigned long retval,
                                        const char *interesting_fn,
                                        enum symdrive_DIRECTION direction) {
#if 0
    if (interesting_fn[0] == 0) {
        return;
    }

    tfassert_detail(line >= 0 && line < MAX_INTERESTING_LINES,
                    "File too large -- interesting function at line %d\n", line);

    interesting_function_lines[line]++;

    if (interesting_function_lines[line] > MAX_INTERESTING_PER_LINE &&
        interesting_function_started == 0) {
        //uprintk ("Interesting function: %s:%d.  Skipping, count too high: %d\n",
        //         fn, line, interesting_function_lines[line]);
        return;
    }

    if (direction == ENTRANCE) {
        if (wrapper_type == PREPOSTFN) {
            // Precondition for interesting driver fn
            if (interesting_function_started == 0) {
                interesting_function_started++;
                tfassert_detail (interesting_function_success >= 0, 
                                 "Failed %s: %s:%d\n", __func__, fn, line);
            } else if (interesting_function_started >= 1) {
                uprintk ("Interesting function:  unconditional prioritize %s:%d\n", fn, line);
                s2e_prioritize (0);
                interesting_function_started++;
            } else {
                tfassert_detail (0, "Failed %s: %s:%d\n", __func__, fn, line);
            }
        } else if (wrapper_type == STUBWRAPPER) {
            // Precondition for kernel function
            if (interesting_function_started == 0) {
                //
                // This should never happen because if it does, it means that we're calling
                // an interesting kernel function but we've not already started the
                // corresponding interesting driver function.  All interesting
                // kernel functions that are called must have a corresponding driver
                // function.
                //
                tfassert_detail (0, "Bug in test framework or static analysis\n");
            } else if (interesting_function_started >= 1) {
                if (strcmp (fn, interesting_fn) == 0) {
                    uprintk ("Interesting function:  Unconditional prioritize -- kernel fn called %s:%d\n", fn, line);
                    interesting_function_success++;
                    s2e_prioritize (0);
                } else {
                    uprintk ("Interesting function:  Not prioritizing -- kernel fn called %s:%d expecting %s\n",
                             fn, line, interesting_fn);
                }
            } else {
                tfassert_detail (0, "Failed %s: %s:%d\n", __func__, fn, line);
            }
        } else {
            tfassert_detail (0, "Failed %s: %s:%d\n", __func__, fn, line);
        }
    } else if (direction == EXIT) {
        if (wrapper_type == PREPOSTFN) {
            // Postcondition for interesting driver fn
            if (interesting_function_started == 0) {
                tfassert_detail (0, "Failed %s: %s:%d\n", __func__, fn, line);
            } else if (interesting_function_started >= 1) {
                if (interesting_function_success == 0) {
                    char temp[128];
                    sprintf (temp, "Interesting function: %s:%d failed to complete successfully.", fn, line);
                    s2e_kill_state(1, 0, temp);
                } else if (interesting_function_success >= 1) {
                    s2e_prioritize(0);
                    interesting_function_started--;
                    if (interesting_function_started == 0) {
                        uprintk ("Interesting function:  Complete success, success at %d.  %s:%d. Prioritized\n",
                                 interesting_function_success, fn, line);
                        interesting_function_success--;
                    } else if (interesting_function_started >= 1) {
                        uprintk ("Interesting function:  Success.  %s:%d. Prioritize\n", fn, line);
                    } else {
                        tfassert_detail (0, "Failed %s: %s:%d\n", __func__, fn, line);
                    }
                } else {
                    tfassert_detail (0, "Failed %s: %s:%d\n", __func__, fn, line);
                }
            } else {
                tfassert_detail (0, "Failed %s: %s:%d\n", __func__, fn, line);
            }
        } else if (wrapper_type == STUBWRAPPER) {
            // Postcondition for interesting kernel fn.
            // No need to do anything here -- all logic is in the precondition, above
        } else {
            tfassert_detail (0, "Failed %s: %s:%d\n", __func__, fn, line);
        }
    } else {
        tfassert_detail (0, "Failed %s: %s:%d\n", __func__, fn, line);
    }
#endif
}

static void handle_max_coverage (const char *fn,
                                 int line,
                                 int retval_valid,
                                 unsigned long retval,
                                 enum symdrive_WRAPPER_TYPE wrapper_type,
                                 enum symdrive_DIRECTION direction) {
    if (strcmp (fn, g_sym_starting_fn)) {
        return;
    }

    // We have a match
    if (direction == ENTRANCE) {
        uprintk ("Max coverage mode detected: %s:%d\n", fn, line);
        g_sym_retval = 1;
        s2e_kill_all_others(line);
    } else if (direction == EXIT) {
        if (retval_valid && IS_ERR_VALUE(retval)) {
            char temp[128];
            sprintf (temp, "Terminating failed thorough coverage fn path (%s:%d)...", fn, line);
            s2e_kill_state(0, 0, temp);
        } else {
            // Disable favor successful and start exploring all other paths
            // through this function.
            char temp[128];
            s2e_favor_successful(line, 0);
            sprintf (temp, "Terminating successful thorough coverage fn path (line %s/%d)...", fn, line);
            s2e_kill_state(0, 0, temp);
        }
    } else {
        tfassert_detail (0, "Failed %s: %s:%d\n", __func__, fn, line);
    }
}

//
// wrapper_type:  0 for kernel functions, 1 for entry point epwrapper_ functions
// Returns the current driver call stack after entering the driver
//
//  gen_enter_driver is called after the associated kernel function executes
//  gen_enter_driver is called before the associated driver function executes.
//
// Interesting functions: 0 if not interesting, 1 if interesting.
//  Note that both driver functions and kernel functions can be interesting
//  We can distinguish that based on wrapper_type
//
int gen_enter_driver (const char *fn,
                      int line,
                      enum symdrive_WRAPPER_TYPE wrapper_type,
                      int retval_valid,
                      unsigned long retval,
                      const char *interesting_fn,
                      void (*gen_register_all_param)(void))
{
    // Moved from generated code.
    // Call interrupt handlers if we're entering the driver from
    // a call into the kernel.
    static char first_time_in_driver = 1;
    if (first_time_in_driver == 1) {
        gen_register_all_param();
        first_time_in_driver = 0;
    }

    if (wrapper_type == STUBWRAPPER) {
        // Pause/Resume accumulation if needed
        //s2e_disable_trackperf(__LINE__, SYMDRIVE_PAUSE_IRQ);
        call_interrupt_handlers(fn, line);
        execute_completions(fn, line);
        //s2e_enable_trackperf(__LINE__, SYMDRIVE_CONTINUE_IRQ);
    }

    acquire_global_lock();

    // Entering driver:
    if (driver_call_stack == 0) {
        //if (cleanup_path == 0 && strcmp (fn, "cleanup_module") == 0) { // TODO HACK
        //    cleanup_path = 1;
        //}
        //if (cleanup_path == 0) {
        //    s2e_disable_all_apic_interrupts();
        //}

        verify_retval(fn, line, wrapper_type, ENTRANCE, retval_valid, retval);

        // Reset success path on first entry into driver
        s2e_success_path (line, fn, 0);
    }
    verify_invariants(fn, line, 1, wrapper_type);

    // Track performance / basic blocks
    //if (strcmp (fn, g_sym_trackperf_fn) == 0) {
    //    // start accumulation
    //    s2e_enable_trackperf(__LINE__, SYMDRIVE_START_AUTO);
    //}

    // Entering driver/kernel function:
    s2e_enter_function(line, fn, wrapper_type);

    handle_interesting_function(fn, line, wrapper_type, retval_valid, retval, interesting_fn, ENTRANCE);
    handle_max_coverage(fn, line, retval_valid, retval, wrapper_type, ENTRANCE);
    release_global_lock();

    // Not used as far as I know:
    return driver_call_stack;
}

//
// This function is called in the postfn_ generated function
//
int gen_exit_driver (const char *fn,
                     int line,
                     enum symdrive_WRAPPER_TYPE wrapper_type,
                     int retval_valid,
                     unsigned long retval,
                     const char *interesting_fn,
                     void (*gen_register_all_param)(void))
{
    //static int alternate_sym = 0;
    acquire_global_lock();

    // Exiting driver/kernel function:
    s2e_exit_function(line, fn, wrapper_type);

    // Track performance / basic blocks
    // Do this before calling verify_retval
    // Do this before max coverage.
    //if (strcmp (fn, g_sym_trackperf_fn) == 0) {
    //    s2e_disable_trackperf(__LINE__, SYMDRIVE_STOP_AUTO);
    //}

    // Starting the full-coverage mode
    handle_max_coverage(fn, line, retval_valid, retval, wrapper_type, EXIT);
    handle_interesting_function(fn, line, wrapper_type, retval_valid, retval, interesting_fn, EXIT);
    // If the retval is valid and not symbolic then prioritize or deprioritize.
    // If it is symbolic, then do nothing on the assumption that it's a hardware
    // value -- we should not be prioritizing hardware values.
    if (retval_valid) {
        if (s2e_is_symbolic_symdrive(line, retval) == 0) {
            s2e_prioritize(line);

            if (g_symdrive_annotations == 0) {
                // Proceed normally
            } else {
                // Convert retval to success
                uprintk ("Prioritization:  allowing our path per annotation\n",
                         fn, line, s2e_get_example_uint (retval));
                g_symdrive_annotations = 0;
                retval = 0;
            }

            if (!IS_ERR_VALUE(retval)) {
                s2e_success_path(line, fn, 1);
                uprintk ("Prioritizing line %d, example val %lu\n", line, s2e_get_example_uint(retval));
            } else {
                uprintk ("Deprioritizing line %d, example val %lu\n", line, s2e_get_example_uint(retval));
                s2e_success_path(line, fn, -1);
                s2e_deprioritize(line);
            }
        } else {
            uprintk ("Line %d: symbolic retval, example %lu.\n", line, s2e_get_example_uint(retval));
        }
    }

    // Exiting driver:
    verify_invariants(fn, line, -1, wrapper_type);

    if (driver_call_stack == 0) {
        verify_retval(fn, line, wrapper_type, EXIT, retval_valid, retval);
        reset_new_objects (fn);
        
        //if (cleanup_path == 0) {
        //    s2e_enable_all_apic_interrupts();
        //}
    }
    release_global_lock();

    // Moved from generated code.
    // Call interrupt handlers if we're returning to the kernel.
    if (wrapper_type == PREPOSTFN && driver_call_stack == 0) {
        //s2e_disable_trackperf(__LINE__, SYMDRIVE_PAUSE_IRQ); // Pause accumulation
        call_interrupt_handlers(fn, line);
        execute_completions(fn, line);
        //s2e_enable_trackperf(__LINE__, SYMDRIVE_CONTINUE_IRQ); // Resume accumulation
    }

    // Not used as far as I know:
    return driver_call_stack;
}

int gen_make_symbolic_ptr (const char *fn, int line, void **ptr_retval) {
    void *orig_retval = *ptr_retval;
    if (g_sym_retval == 0) {
        return g_sym_retval;
    }

    acquire_global_lock();
    s2e_make_symbolic (ptr_retval, sizeof (void *), "symbolic_retval_ptr");
    if (*ptr_retval == orig_retval) {
        uprintk ("%s/%s: original retval: %p\n", fn, __func__, *ptr_retval);
        s2e_success_path(line, fn, 1);
    } else if (*ptr_retval == NULL && *ptr_retval != orig_retval) {
        uprintk ("%s/%s: NULL retval\n", fn, __func__);
        s2e_success_path(line, fn, -1);
        bad_path = 1;
    } else {
        s2e_kill_state(1, 0, "gen_make_symbolic_ptr:  extra path\n");
    }
    release_global_lock();
    return g_sym_retval;
}

// Note that int_retval may not point to an unsigned long but may
// point to an int, char, etc.

#define MAKE_SYMBOLIC_INT_HELPER(TYPE, TYPE_ID)                         \
    if (int_retval_size == sizeof(TYPE)) {                              \
        if (*((TYPE *) int_retval) == orig_retval_ ## TYPE_ID) {        \
            /* orig_retval may even be symbolic in the case of ioread32 \
               which we are returning symbolic data for */              \
            uprintk ("%s/%s: original retval\n", fn, __func__);         \
            s2e_success_path(line, fn, 1);                              \
        } else if (IS_ERR_VALUE(*((TYPE *) int_retval))) {              \
            /* Unsigned long is necessary to remove warning */          \
            uprintk ("%s/%s: symbolic retval\n", fn, __func__);         \
            s2e_success_path(line, fn, -1);                             \
            bad_path = 1;                                               \
        } else {                                                        \
            s2e_kill_state(1, 0, "gen_make_symbolic_int:  extra path\n"); \
        }                                                               \
    }

#define MAKE_SYMBOLIC_INIT(TYPE, TYPE_ID)                               \
    if (int_retval_size == sizeof(TYPE)) {                              \
        orig_retval_ ## TYPE_ID = *((TYPE *) int_retval);               \
    }

int gen_make_symbolic_int (const char *fn, int line, void *int_retval, int int_retval_size) {
    unsigned char orig_retval_unsigned_char = 0;
    unsigned short orig_retval_unsigned_short = 0;
    unsigned int orig_retval_unsigned_int = 0;
    unsigned long long orig_retval_unsigned_long_long = 0;

    // We'll assume the code generator includes the check.
    // If you change this assumption, change gen_make_symbolic_ptr too.
    // assert (g_sym_retval != 0, "Fix the code generator.");
    if (g_sym_retval == 0) {
        return g_sym_retval;
    }

    acquire_global_lock();
    assert (int_retval_size == sizeof(unsigned char) ||
            int_retval_size == sizeof(unsigned short) ||
            int_retval_size == sizeof(unsigned int) ||
            int_retval_size == sizeof(unsigned long long),
            "Invalid size specified in %s.  Size %d",
            __func__, int_retval_size);

    assert (sizeof(unsigned char) == 1 &&
            sizeof(unsigned short) == 2 && 
            sizeof(unsigned int) == 4 &&
            sizeof(unsigned long long) == 8,
            "Wrong size for primitive type");
    
    MAKE_SYMBOLIC_INIT(unsigned char, unsigned_char);
    MAKE_SYMBOLIC_INIT(unsigned short, unsigned_short);
    MAKE_SYMBOLIC_INIT(unsigned int, unsigned_int);
    MAKE_SYMBOLIC_INIT(unsigned long long, unsigned_long_long);
    s2e_make_symbolic (int_retval, int_retval_size, "symbolic_retval_int");
    MAKE_SYMBOLIC_INT_HELPER(unsigned char, unsigned_char);
    MAKE_SYMBOLIC_INT_HELPER(unsigned short, unsigned_short);
    MAKE_SYMBOLIC_INT_HELPER(unsigned int, unsigned_int);
    MAKE_SYMBOLIC_INT_HELPER(unsigned long long, unsigned_long_long);
    release_global_lock();

    return g_sym_retval;
}

const char *driver_dump_stack(int print) {
    int i;
    assert_global_lock();
    memset(call_stack_buffer1, 0, call_stack_buffer_size);
    memset(call_stack_buffer2, 0, call_stack_buffer_size);
    call_stack_buffer1[0] = 0;
    for (i = 0; i < driver_call_stack_max; i++) {
        if (driver_call_stack_str[i] != NULL) {
            call_stack_buffer2[0] = 0;
            sprintf (call_stack_buffer2, "%s:%d -> ", driver_call_stack_str[i], i);
            strlcat (call_stack_buffer1, call_stack_buffer2, call_stack_buffer_size);
        }
    }
    if (print) {
        uprintk ("%s", call_stack_buffer1);
    }
    return call_stack_buffer1;
}

// This command can optionally be manually added to the driver.
void symdrive_annotation(int flags) {
    g_symdrive_annotations = flags;
}

#ifdef LINUX_MODE
EXPORT_SYMBOL(gen_enter_driver);
EXPORT_SYMBOL(gen_exit_driver);
EXPORT_SYMBOL(gen_make_symbolic_ptr);
EXPORT_SYMBOL(gen_make_symbolic_int);
EXPORT_SYMBOL(symdrive_annotation);
#endif // LINUX_MODE

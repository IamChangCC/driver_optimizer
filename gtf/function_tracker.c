#include "globals.h"
#include "hashtable.h"

#ifdef LINUX_MODE
#include <linux/module.h>
#include <linux/sockios.h>
#include <linux/string.h>
#include <linux/capability.h>
#include <linux/errno.h>
#include <linux/compiler.h>
#include <linux/slab.h>
#endif

#ifdef FREEBSD_MODE
#include <sys/param.h>
#include <sys/module.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/libkern.h>
#include <sys/malloc.h>
#endif

struct hkey {
    void *driver_fn_ptr;
};

struct hvalue {
    void *driver_fn_ptr;
    void *wrapper_fn_ptr;
    const char *str_driver_fn;
    const char *str_wrapper_fn;
    const char *str1;
    const char *str2;
};

struct function_tracker_struct
{
    int initialized; // 0 before initialization, 1 after.
    struct hashtable *objs; // Function mappings
};

static struct function_tracker_struct global_state;

static void objs_key (struct hkey *key, void *driver_fn_ptr) {
    key->driver_fn_ptr = driver_fn_ptr;
}

static struct hkey *objs_key_ptr (void *driver_fn_ptr) {
    struct hkey *key = mjr_kmalloc_nonatomic (sizeof (struct hkey));
    objs_key (key, driver_fn_ptr);
    return key;
}

static struct hvalue *objs_value (void *driver_fn_ptr,
                                  void *wrapper_fn_ptr,
                                  const char *str_driver_fn,
                                  const char *str_wrapper_fn,
                                  const char *str1,
                                  const char *str2) {
    struct hvalue *value = mjr_kmalloc_nonatomic (sizeof (struct hvalue));
    memset(value, 0, sizeof (struct hvalue));
    value->driver_fn_ptr = driver_fn_ptr;
    value->wrapper_fn_ptr = wrapper_fn_ptr;
    value->str_driver_fn = str_driver_fn;
    value->str_wrapper_fn = str_wrapper_fn;
    value->str1 = str1;
    value->str2 = str2;
    return value;
}

//
// This is used for FreeBSD only, currently
//
// It deals with these structures:
// static kobj_method_t ed_pci_methods[4]  = {
//      {& device_probe_desc, (int (*)(void))(& ed_pci_probe)},
//      {& device_attach_desc, (int (*)(void))(& ed_pci_attach)},
//      {& device_detach_desc, (int (*)(void))(& ed_detach)},
//      {(kobjop_desc_t )0, (int (*)(void))0}};
//
// Call this after gen_register_one is called on everything.
//
/*
void gen_static_mapping(void *orig_fn,
                        const char *driver_fn_name,
                        const char *str1,
                        const char *str2) {
    struct hkey key;
    struct hvalue *value;

    // Lock the TF
    acquire_global_lock();
    objs_key(&key, orig_fn);
    value = hashtable_search (global_state.objs, &key);
    if (value == NULL) {
        assert (0, "gen_static_mapping:  object should be in the table already: %s.", driver_fn_name);
    }

    // Update information stored for this function
    value->str1 = str1;
    value->str2 = str2;
    release_global_lock();
}
*/

// This function requires a function pointer.
// You can't just pass an actual function into it
// since we need to dereference it.
//
// The purpose of this function is to establish
// a mapping between a driver function and its purpose
// where its purpose is given by str1 and str2.
//
void gen_convert_fn (void **orig_fn_ptr,
                     const char *driver_fn_name,
                     const char *str1,
                     const char *str2) {
    struct hkey key;
    struct hvalue *value;
    void *orig_fn = NULL;
    if ((unsigned long) orig_fn_ptr < 0xC0000000 ||
        (unsigned long) orig_fn_ptr > 0xFFFF0000) {
        // Maybe a bad value from convert_fn call.
        return;
    }

    // Need to deref incoming param to get the fn ptr
    orig_fn = *(void **) orig_fn_ptr;

    // Lock the TF
    acquire_global_lock();

    objs_key(&key, orig_fn);
    value = hashtable_search (global_state.objs, &key);
    if (value == NULL) {
        // In this case, there is no corresponding epwrapper
        // This may be because the pointer is already translated
        // Simply don't translate it
        // Unfortunately, it may also be because we missed a wrapper
        //uprintk ("%s - failed: %p, %s, %s, %s\n",
        //         __func__, orig_fn, driver_fn_name, str1, str2);
        goto done;
    } else {
        //uprintk ("%s - success: %p, %s, %s, %s\n",
        //         __func__, orig_fn, driver_fn_name, str1, str2);
    }

    // Update information stored for this function
    value->str1 = str1;
    value->str2 = str2;

    // Don't translate the function
    //*orig_fn_ptr = value->wrapper_fn_ptr;

  done:
    release_global_lock();
}

static unsigned int hash_from_key_fn (void *k) {
    struct hkey *key = (struct hkey *) k;
    return (unsigned int) key->driver_fn_ptr;
}

static int hash_keys_equal_fn (void *k1, void *k2) {
    struct hkey *key1 = (struct hkey *) k1;
    struct hkey *key2 = (struct hkey *) k2;

    if (key1->driver_fn_ptr == key2->driver_fn_ptr) {
        return 1;
    }

    return 0;
}

#if 0
static void hash_print_obj (void *vkey, void *vvalue) {
    //struct hkey *key = (struct hkey *) vkey;
    struct hvalue *value = (struct hvalue *) vvalue;

    uprintk ("%s: %p; %s: %p\n",
             value->str_driver_fn, value->driver_fn_ptr,
             value->str_wrapper_fn, value->wrapper_fn_ptr);
}
#endif

//static void objs_enum_print (void) {
//    uprintk ("All objects (may be empty):\n");
//    hashtable_enumerate (global_state.objs, hash_print_obj);
//    uprintk ("===================================\n");
//}

static void objs_insert (struct hkey *key,
                         struct hvalue *value) {
    struct hvalue *result;
    int result2;

    result = (struct hvalue *) hashtable_search (global_state.objs, key);
    if (result != NULL) {
        assert (0, "Object already present in table!");
    }

    // If we have a memory failure of some kind, let's just abort rather
    // than propagate the error.
    result2 = hashtable_insert(global_state.objs, key, value);
    if (result2 == 0) {
        assert (0, "Failed to insert into table");
    }
}

//
// Generating a wrapper -> driver mapping.
//
void gen_register_one (void *wrapper_fn,
                       void *driver_fn,
                       const char *wrapper_fn_str,
                       const char *driver_fn_str) {
    struct hkey *key = objs_key_ptr(driver_fn);
    struct hvalue *value = objs_value(driver_fn,
                                      wrapper_fn,
                                      driver_fn_str,
                                      wrapper_fn_str,
                                      "",
                                      "");

    uprintk ("Registering %s / %s\n", wrapper_fn_str, driver_fn_str);
    objs_insert (key, value);
}

//
// What the driver will call if we can't find
// a check routine.  Implemented in misc_checks/main.c
//
extern void default_MJRcheck(const char *fn, int line);

static const void *simple_lookup_MJRcheck (struct hkey *key, struct hvalue *value) {
    char temp[128];
    const void *retval;

    // These are comparatively easy to look up -- just append
    // "_MJRcheck" to the name
    strcpy (temp, value->str_driver_fn);
    strcat (temp, "_MJRcheck");

    uprintk ("%s: Looking up %s...\n", __func__, temp);
    retval = symbol_lookup (temp);
    return retval;
}

// Complex translation
static const void *complex_lookup_MJRcheck (struct hkey *key, struct hvalue *value) {
    char temp[128];
    const void *retval;

    if (strcmp (value->str1, "") && strcmp (value->str2, "")) {
        // Example functions:  __pci_register_driver, schedule_work
        // First the structure name
        strcpy (temp, value->str1);
        strcat (temp, "_");

        // Field name
        strcat (temp, value->str2);
        strcat (temp, "_");

        // Check
        strcat (temp, "MJRcheck");
    } else if ((strcmp (value->str1, "") && !strcmp(value->str2, "")) ||
               (!strcmp (value->str1, "") && strcmp(value->str2, ""))) {
        assert (0, "Unknown lookup_MJRcheck parameters");
    } else {
        // Both strs = ""
        // Example functions:  request_irq, netif_napi_add
        return simple_lookup_MJRcheck (key, value);
    }

    uprintk ("%s: Looking up %s...\n", __func__, temp);
    retval = symbol_lookup (temp);
    return retval;
}

//
// Called by both kernel function wrappers and entry point wrappers.
//
const void *gen_lookup_MJRcheck (const char *wrapper_type, void *driver_fn_ptr) {
    struct hkey key;
    struct hvalue *value;
    const void *retval = NULL;
    static char fn_name[128];

    acquire_global_lock();

    objs_key (&key, driver_fn_ptr);
    value = (struct hvalue *) hashtable_search (global_state.objs, &key);
    if (value == NULL && (wrapper_type[0] == '0' || wrapper_type[0] == '1')) {
        // Should this be an error?
        //retval = &default_MJRcheck;
        //goto done;
        assert (0, "%s failed to lookup\n", __func__);
    }

    if (wrapper_type[0] == '0' || wrapper_type[0] == '1') {
        // kernel wrapper / entry point wrapper
        retval = complex_lookup_MJRcheck (&key, value);
    } else {
        sprintf (fn_name, "%s_MJRcheck", wrapper_type);
        retval = symbol_lookup (fn_name);
    }

    if (retval == NULL) {
        uprintk ("%s: Failed to find test framework function %s/%s/%s/%s/%s\n",
                 __func__,
                 value->str_driver_fn,
                 value->str_wrapper_fn,
                 value->str1,
                 value->str2,
                 fn_name);
        retval = &default_MJRcheck;
    } else {
        uprintk ("%s: Looked up %s/%s.  TF: %p, orig: %p\n",
                 __func__,
                 value->str_driver_fn,
                 fn_name,
                 retval,
                 driver_fn_ptr);
    }

    release_global_lock();
    return retval;
}

int initialize_function_tracker(void) {
    global_state.objs = create_hashtable(16, hash_from_key_fn, hash_keys_equal_fn);
    global_state.initialized = 1;
    return 0;
}

void free_function_tracker(void) {
    acquire_global_lock();
    if (global_state.initialized == 1) {
        hashtable_destroy(global_state.objs, 1);
        global_state.objs = NULL;
        memset (&global_state, 0, sizeof (struct function_tracker_struct));
    }
    release_global_lock();
}

#ifdef LINUX_MODE
EXPORT_SYMBOL(gen_convert_fn);
EXPORT_SYMBOL(gen_register_one);
// EXPORT_SYMBOL(gen_register_all_cleanup);
EXPORT_SYMBOL(gen_lookup_MJRcheck);
#endif

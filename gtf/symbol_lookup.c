#include "globals.h"

// Linux is much easier, though
// we did admittedly modify
// the kernel slightly to make this work.
#ifdef LINUX_MODE
#include <linux/module.h>
extern unsigned long kallsyms_lookup_name(char const *symbol_name);

const void *symbol_lookup (const char *name) {
    return (const void *) kallsyms_lookup_name(name);
}

#endif

#ifdef FREEBSD_MODE
#include <sys/param.h>
#include <sys/module.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/linker.h>
#include <sys/elf32.h>
#include "linker_if.h"

// This chunk of code was a failed test.
// It has trouble recognizing symbols
// from in the test framework.
// Symbols from the kernel show up fine.
#if 0
struct lookup_struct {
    const char *sym_name;
    Elf32_Sym *elfsym;
};

static int MJR_linker_predicate(linker_file_t f, void *context) {
    struct lookup_struct *s = (struct lookup_struct *) context;
    if (LINKER_LOOKUP_SYMBOL(f, s->sym_name, (c_linker_sym_t *) &s->elfsym) == 0) {
        printf ("Symbol: %s / %p\n", s->sym_name, (void *) s->elfsym->st_value);
        return 0;
    }
    return ENOENT;
}

static const void *symbol_lookup (const char *sym_name) {
    struct lookup_struct result;
    result.elfsym = NULL;
    result.sym_name = sym_name;
    linker_file_foreach(MJR_linker_predicate, &result);
    if (result.elfsym != NULL) {
        return (const void *) result.elfsym->st_value;
    } else {
        return NULL;
    }
}
#endif

#if 0

int ed_attach_MJRcheck(const char *fn,
                       int prepost,
                       int *retval,
                       device_t *dev);

void test_symbol_lookup(void) {
    const void *temp_sym;

    temp_sym = NULL;
    temp_sym = symbol_lookup("ed_attach_MJRcheck");
    printf ("address: %p/%p, as compared to: %p\n", &ed_attach_MJRcheck, ed_attach_MJRcheck, temp_sym);

    temp_sym = NULL;
    temp_sym = symbol_lookup("printf");
    printf ("address: %p/%p, as compared to: %p\n", &printf, printf, temp_sym);

    temp_sym = NULL;
    temp_sym = symbol_lookup("bogus");
    printf ("address: %p should fail\n", temp_sym);
}
#endif

// Given a symbol name, return its address.
// Works with functions declared in the kernel
// or in the TF.  NOTHING ELSE (necessarily).
const void *symbol_lookup (const char *name) {
    c_linker_sym_t temp_sym = NULL;
    const Elf32_Sym *sym = NULL;
    module_t tf_module = module_lookupbyname ("test_framework");
    struct linker_file *tf_file = module_file (tf_module);
    const void *ret_addr = NULL;

    // Do a symbol lookup.
    // Kernel symbols get the actual address.
    // Symbols in our module do NOT.
    // Note that this approach will almost certainly not
    // work for trying to lookup a symbol in another
    // module (i.e. not the test framework)
    linker_ddb_lookup(name, &temp_sym);
    sym = (const Elf32_Sym *) temp_sym;
    if (sym != NULL) {
        if (sym->st_value < tf_file->size * 2) {
            // module symbol:
            // yucky yucky yucky
            unsigned long offset = (unsigned long) sym->st_value;
            unsigned long base = (unsigned long) tf_file->address;
            ret_addr = (const void *) (base + offset);
        } else {
            // kernel symbol:
            ret_addr = (const void *) sym->st_value;
        }
    } else {
        ret_addr = NULL;
    }

    // Sanity to make sure this method works
    if (ret_addr < (const void *) 0xC0000000 && ret_addr != NULL) {
        panic ("Failed symbol lookup: %p %s\n", ret_addr, name);
    }

    return ret_addr;
}

#endif

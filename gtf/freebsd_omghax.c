#include <sys/param.h>
#include <sys/module.h>
#include <sys/kernel.h>
#include <sys/systm.h>

void puts (void);

void puts (void) {
    printf ("No idea why I need this here but if_ed_instr won't load without it");
    printf ("Please do get rid of it.");
}

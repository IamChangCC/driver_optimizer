#include "../test_state.h"
#include "../testing_ep.h"
#include "../mega_header.h"

#include <linux/module.h>

STATIC int gpio_is_valid_MJRcheck(const char *fn,
                                  int prepost,
                                  bool *retval,
                                  int *number) {
    check_routine_start();
    if (prepost == 0) {
        s2e_make_symbolic(retval, sizeof (bool), __func__);
    } else {
        tfassert(0);
    }
    check_routine_end();
    return 1;
}

STATIC int gpio_request_MJRcheck(const char *fn,
                                 int prepost,
                                 int *retval,
                                 unsigned *gpio,
                                 const char **label) {
    check_routine_start();
    if (prepost == 0) {
        s2e_make_symbolic(retval, sizeof (int), __func__);
        if (IS_ERR_VALUE(*retval) && *retval != -ENXIO && *retval != -ENODEV) {
            goto end;
        } else {
            *retval = 0;
            goto end;
        }
    } else {
        tfassert(0);
    }
  end:
    check_routine_end();
    return 1;
}

STATIC int gpio_request_one_MJRcheck(const char *fn,
                                     int prepost,
                                     int *retval,
                                     unsigned *gpio,
                                     unsigned long *flags,
                                     const char *label) {
    check_routine_start();
    if (prepost == 0) {
        s2e_make_symbolic(retval, sizeof (int), __func__);
        if (IS_ERR_VALUE(*retval) && *retval != -ENXIO && *retval != -ENODEV) {
            goto end;
        } else {
            *retval = 0;
            goto end;
        }
    } else {
        tfassert(0);
    }
  end:
    check_routine_end();
    return 1;
}

STATIC int gpio_request_array_MJRcheck(const char *fn,
                                       int prepost,
                                       int *retval,
                                       const struct gpio **array,
                                       size_t *num) {
    check_routine_start();
    if (prepost == 0) {
        s2e_make_symbolic(retval, sizeof (int), __func__);
        if (IS_ERR_VALUE(*retval) && *retval != -ENXIO && *retval != -ENODEV) {
            goto end;
        } else {
            *retval = 0;
            goto end;
        }
    } else {
        tfassert(0);
    }
  end:
    check_routine_end();
    return 1;
}

STATIC int gpio_free_MJRcheck(const char *fn,
                              int prepost,
                              unsigned *gpio) {
    check_routine_start();
    check_routine_end();
    return 1;
}

STATIC int gpio_free_array_MJRcheck(const char *fn,
                                    int prepost,
                                    const struct gpio **array,
                                    size_t *num) {
    check_routine_start();
    check_routine_end();
    return 1;
}

STATIC int gpio_direction_input_MJRcheck(const char *fn,
                                         int prepost,
                                         int *retval,
                                         unsigned *gpio) {
    check_routine_start();
    if (prepost == 0) {
        s2e_make_symbolic(retval, sizeof (int), __func__);
        if (IS_ERR_VALUE(*retval) && *retval != -ENXIO && *retval != -ENODEV) {
            goto end;
        } else {
            *retval = 0;
            goto end;
        }
    } else {
        tfassert(0);
    }
  end:
    check_routine_end();
    return 1;
}

STATIC int gpio_direction_output_MJRcheck(const char *fn,
                                          int prepost,
                                          int *retval,
                                          unsigned *gpio,
                                          int *value) {
    check_routine_start();
    if (prepost == 0) {
        s2e_make_symbolic(retval, sizeof (int), __func__);
        if (IS_ERR_VALUE(*retval) && *retval != -ENXIO && *retval != -ENODEV) {
            goto end;
        } else {
            *retval = 0;
            goto end;
        }
    } else {
        tfassert(0);
    }
  end:
    check_routine_end();
    return 1;
}

STATIC int gpio_set_debounce_MJRcheck(const char *fn,
                                      int prepost,
                                      int *retval,
                                      unsigned *gpio,
                                      unsigned *debounce) {
    check_routine_start();
    if (prepost == 0) {
        s2e_make_symbolic(retval, sizeof (int), __func__);
        if (IS_ERR_VALUE(*retval) && *retval != -ENXIO && *retval != -ENODEV) {
            goto end;
        } else {
            *retval = 0;
            goto end;
        }
    } else {
        tfassert(0);
    }
  end:
    check_routine_end();
    return 1;
}

static int gpio_get_value_wrapper(const char *fn,
                                  int prepost,
                                  int *retval,
                                  unsigned *gpio) {
    check_routine_start();
    if (prepost == 0) {
        s2e_make_symbolic(retval, sizeof(int), __func__);
    } else {
        tfassert(0);
    }
    check_routine_end();
    // Bypass the kernel implementation.
    return 1;
}
    
STATIC int gpio_get_value_MJRcheck(const char *fn,
                                   int prepost,
                                   int *retval,
                                   unsigned *gpio) {
    return gpio_get_value_wrapper(fn, prepost, retval, gpio);
}

STATIC int __gpio_get_value_MJRcheck(const char *fn,
                                     int prepost,
                                     int *retval,
                                     unsigned *gpio) {
    return gpio_get_value_wrapper(fn, prepost, retval, gpio);
}

STATIC int gpio_set_value_MJRcheck(const char *fn,  
                                   int prepost,
                                   unsigned *gpio,
                                   int *value) {
    check_routine_start();
    check_routine_end();
    return 1;
}

STATIC int __gpio_set_value_MJRcheck(const char *fn,
                                     int prepost,
                                     unsigned *gpio,
                                     int *value) {
    check_routine_start();
    check_routine_end();
    return 1;
}

STATIC int gpio_cansleep_MJRcheck(const char *fn,
                                  int prepost,
                                  int *retval,
                                  unsigned *gpio) {
    check_routine_start();
    if (prepost == 0) {
        s2e_make_symbolic(retval, sizeof (int), __func__);
        if (*retval == 0) {
            goto end;
        } else {
            *retval = 1;
            goto end;
        }
    } else {
        tfassert(0);
    }
  end:
    check_routine_end();
    return 1;
}

STATIC int gpio_get_value_cansleep_MJRcheck(const char *fn,
                                            int prepost,
                                            int *retval,
                                            unsigned *gpio) {
    check_routine_start();
    if (prepost == 0) {
        s2e_make_symbolic(retval, sizeof (int), __func__);
    } else {
        tfassert(0);
    }
    check_routine_end();
    return 1;
}

STATIC int gpio_set_value_cansleep_MJRcheck(const char *fn,
                                            int prepost,
                                            unsigned *gpio,
                                            int *value) {
    check_routine_start();
    check_routine_end();
    return 1;
}

STATIC int gpio_export_MJRcheck(const char *fn,
                                int prepost,
                                int *retval,
                                unsigned *gpio,
                                bool *direction_may_change) {
    check_routine_start();
    if (prepost == 0) {
        s2e_make_symbolic(retval, sizeof (int), __func__);
    } else {
        tfassert(0);
    }
    check_routine_end();
    return 1;
}

STATIC int gpio_export_link_MJRcheck(const char *fn,
                                     int prepost,
                                     int *retval,
                                     struct device **dev,
                                     const char **name,
                                     unsigned *gpio) {
    check_routine_start();
    if (prepost == 0) {
        s2e_make_symbolic(retval, sizeof (int), __func__);
    } else {
        tfassert(0);
    }
    check_routine_end();
    return 1;
}

STATIC int gpio_sysfs_set_active_low_MJRcheck(const char *fn,
                                              int prepost,
                                              int *retval,
                                              unsigned *gpio,
                                              int *value) {
    check_routine_start();
    if (prepost == 0) {
        s2e_make_symbolic(retval, sizeof (int), __func__);
    } else {
        tfassert(0);
    }
    check_routine_end();
    return 1;
}

STATIC int gpio_unexport_MJRcheck(const char *fn,
                                  int prepost,
                                  unsigned *gpio) {
    check_routine_start();
    check_routine_end();
    return 1;
}

STATIC int gpio_to_irq_MJRcheck(const char *fn,
                                int prepost,
                                int *retval,
                                unsigned *gpio) {
    check_routine_start();
    if (prepost == 0) {
        *retval = -ENXIO; // MJR need to figure out more about how this works.
    } else {
        tfassert(0);
    } 
    check_routine_end();
    return 1;
}

STATIC int irq_to_gpio_MJRcheck(const char *fn,
                                int prepost,
                                int *retval,
                                unsigned *irq) {
    check_routine_start();
    if (prepost == 0) {
        *retval = -EINVAL; // MJR need to figure out more about how this works.
    } else {
        tfassert(0);
    } 
    check_routine_end();
    return 1;
}

EXPORT_SYMBOL(gpio_is_valid_MJRcheck);
EXPORT_SYMBOL(gpio_request_MJRcheck);
EXPORT_SYMBOL(gpio_request_one_MJRcheck);
EXPORT_SYMBOL(gpio_request_array_MJRcheck);
EXPORT_SYMBOL(gpio_free_MJRcheck);
EXPORT_SYMBOL(gpio_free_array_MJRcheck);
EXPORT_SYMBOL(gpio_direction_input_MJRcheck);
EXPORT_SYMBOL(gpio_direction_output_MJRcheck);
EXPORT_SYMBOL(gpio_set_debounce_MJRcheck);
EXPORT_SYMBOL(gpio_get_value_MJRcheck);
EXPORT_SYMBOL(__gpio_get_value_MJRcheck);
EXPORT_SYMBOL(gpio_set_value_MJRcheck);  
EXPORT_SYMBOL(__gpio_set_value_MJRcheck);
EXPORT_SYMBOL(gpio_cansleep_MJRcheck);
EXPORT_SYMBOL(gpio_get_value_cansleep_MJRcheck);
EXPORT_SYMBOL(gpio_set_value_cansleep_MJRcheck);
EXPORT_SYMBOL(gpio_export_MJRcheck);
EXPORT_SYMBOL(gpio_export_link_MJRcheck);
EXPORT_SYMBOL(gpio_sysfs_set_active_low_MJRcheck);
EXPORT_SYMBOL(gpio_unexport_MJRcheck);
EXPORT_SYMBOL(gpio_to_irq_MJRcheck);
EXPORT_SYMBOL(irq_to_gpio_MJRcheck);

val g_test_framework : bool ref
val g_loop_priority : bool ref
val g_return_priority : bool ref
val g_verify_in_driver: bool ref
val g_extra_uprintk: bool ref
val g_gen_test_framework_stubs: bool ref
val g_freebsd: bool ref

val drivermacro_prefix: string
val prefn_prefix: string
val postfn_prefix: string
val stubwrapper_prefix: string
val checkfn_prefix: string

val structure_exports_driver_funcs: string -> bool

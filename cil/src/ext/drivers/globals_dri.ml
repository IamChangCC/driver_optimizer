open Cil

(* True if we want to generate _MJRcheck calls, false otherwise.
   Useful for compiling the driver without the corresponding
   test framework
*)
let g_test_framework = ref true

(* See details in drivers.ml for info on these flags *)
let g_loop_priority = ref true
let g_return_priority = ref true
let g_verify_in_driver = ref false
let g_extra_uprintk = ref true
let g_gen_test_framework_stubs = ref false
let g_freebsd = ref false

let drivermacro_prefix = "drivermacro_"
let prefn_prefix = "prefn_"
let postfn_prefix = "postfn_"
let stubwrapper_prefix = "stubwrapper_"
let checkfn_prefix = "check_fn_"

let structure_exports_driver_funcs (name: string) : bool =
  true

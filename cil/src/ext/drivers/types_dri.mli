type fundecs_group = {
  call_interrupt_handlers: Cil.fundec;
  execute_completions: Cil.fundec;
  convert_fn: Cil.fundec;
  lookup_MJRcheck: Cil.fundec;
  enter_driver: Cil.fundec;
  s2e_enter_block: Cil.fundec;
  exit_driver: Cil.fundec;
  s2e_prioritize: Cil.fundec;
  s2e_deprioritize: Cil.fundec;
  s2e_loop_before: Cil.fundec;
  s2e_loop_body: Cil.fundec;
  s2e_loop_after: Cil.fundec;
  s2e_make_symbolic: Cil.fundec;
  s2e_enable_trackperf: Cil.fundec;
  s2e_disable_trackperf: Cil.fundec;
  s2e_trackperf_fn: Cil.fundec;
  uprintk: Cil.fundec;
  verify_in_driver: Cil.fundec;
  make_symbolic_int: Cil.fundec;
  make_symbolic_ptr: Cil.fundec;

  g_sym_params: Cil.varinfo;
  g_sym_retval: Cil.varinfo;

  mutable protos: Cil.global list;
}

type fundecs_register = {
  (* Fdecs related to registering functions *)
  register_one: Cil.fundec;
  register_all: Cil.fundec;
  (* register_all_cleanup: Cil.fundec; *)

  mutable register_protos: Cil.global list;
}
    
type data_chunk = {
  file: Cil.file;
  annots: (string, string) Hashtbl.t;

  (* The prefn/postfn functions *)
  prepostfn_funcs: (Cil.fundec * Cil.fundec) list;

  (* The functions for which do not generate prefn/postfn functions *)
  non_prepostfn_funcs: Cil.fundec list;

  (* The stub functions *)
  stub_funcs: Cil.fundec list;

  (* varinfos for which we do not generate stubs *)
  nonstub_funcs: Cil.varinfo list;

  (* All function varinfos -- includes some don't generate stubs for *)
  funcs_with_no_fundecs: Cil.varinfo list;

  (* Check functions fundecs, for convenience *)
  check_vardecls: Cil.global list;

  (* List of functions that have non-standard return values *)
  nonstandard_retval_fns: string list;

  (* List of "interesting" driver functions *)
  interesting_driver_fns: (string * string) list;

  (* List of driver functions that get a bonus macro call at the end *)
  drivermacro_fns: string list;

  (* See above type definitions *)
  fdecs: fundecs_group;
  fdecs_register: fundecs_register;
}


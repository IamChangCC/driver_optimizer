open Cil
open Scanf
open Utils_dri
open Globals_dri
open Types_dri

(* Remove calls that make no real sense with symbolic hardware *)
class pass_remove_calls
  (f: file) = object (self)
    inherit nopCilVisitor
      
    val remove_fns = [
      "usleep_range";
      "msleep";
      "msleep_interruptible";
      "mdelay";
      "udelay";
      "__udelay";
      "__const_udelay";
      
      (* Aggressive *)
      (* Why printk?  It often leads to concretizing symbolic arguments.
         Dealing with this manually gets tedious *)
      (*
      "printk";
      "no_printk";
      "__dynamic_pr_debug"; (* 3.1.1 feature *)
      "dev_printk";  (* 3.1.1 feature *)
      "dev_err";  (* 3.1.1 feature *)
      "_dev_info";  (* 3.1.1 feature *)
      *)

      (* me4000 - would cutting this cause problems? *)
      (* These functions seem to hang QEMU sometimes for reasons unknown *)
      (* Not consistently, either, of course *)
      (* TODO:  The problem here was us disabling APIC interrupts.  We had to do
         that for another reason.  If we can get rid of the APIC hack, then
         we should be able to remove these from this list *)
      "interruptible_sleep_on_timeout";
      "schedule";

      (* smc91x *)
      "probe_irq_on";
      "probe_irq_off";

      (* FreeBSD *)
      "bus_space_barrier";
      "DELAY";
    ];
      
    method vinst (i: instr) : instr list visitAction =
      match i with
        | Call(_, e, _, _) ->
            let str = exp_tostring e in
            let predicate s = (s = str) in
            if List.exists predicate remove_fns then
              ChangeTo([])
            else
              DoChildren
        | _ -> DoChildren;
      
    method process (): unit =
      Cil.visitCilFileSameGlobals (self :> cilVisitor) f;
  end

class pass_remove_fn_bodies
  (f: file) = object (self)
    val remove_fn_bodies = [
      "__s2e_touch_buffer";
      "__s2e_touch_string";
      "s2e_version";
      "s2e_message";
      "s2e_warning";
      "s2e_print_expression";
      "s2e_enable_forking";
      "s2e_disable_forking";
      "s2e_get_path_id";
      "s2e_make_symbolic";
      "s2e_make_concolic";

      (* MJR 25 in all *)
      (* If you update any of this be sure to update s2e.h *)
      "s2e_make_dma_symbolic";
      "s2e_free_dma_symbolic";
      "s2e_prioritize";
      "s2e_deprioritize";
      "s2e_loop_before";
      "s2e_loop_body";
      "s2e_loop_after";
      "s2e_concretize_kill";
      "s2e_concretize_all";
      "s2e_kill_all_others";
      "s2e_driver_call_stack";
      "s2e_favor_successful";
      (* hole *)
      "s2e_reset_priorities";
      (* hole *)
      "s2e_enable_tracing";
      "s2e_disable_tracing";
      "s2e_enter_function";
      "s2e_exit_function";
      "s2e_is_symbolic_symdrive";
      "s2e_success_path";
      "s2e_enter_block";
      "s2e_primary_fn";
      "s2e_enable_trackperf";
      "s2e_disable_trackperf";
      "s2e_trackperf_fn";
      "s2e_io_region";
      (* MJR *)

      "s2e_is_symbolic";
      "s2e_concretize";
      "s2e_get_example";
      "s2e_get_example_uint";
      "s2e_kill_state";
      "s2e_disable_timer_interrupt";
      "s2e_enable_timer_interrupt";
      "s2e_disable_all_apic_interrupts";
      "s2e_enable_all_apic_interrupts";
      "s2e_get_ram_object_bits";
      "s2e_merge_point";
      "s2e_open";
      "s2e_close";
      "s2e_read";
      "s2e_rawmon_loadmodule";
      "s2e_codeselector_enable_address_space";
      "s2e_codeselector_disable_address_space";
      "s2e_codeselector_select_module";
      "s2e_moduleexec_add_module";
      "s2e_get_module_info";
      "_s2e_assert";
      "s2e_range";
    ];

    method process (): unit =
      let filterfn (g: global) : bool =
        let name =
          match g with
            | GFun(fdec, _) -> fdec.svar.vname
            | _ -> ""
        in
        let findfn (s: string) : bool = 
          if (s = name) then true else false
        in
        try
          ignore (List.find findfn remove_fn_bodies);
          false
        with Not_found ->
          true
      in
      let new_globs = List.filter filterfn f.globals in
      f.globals <- new_globs
  end

(**
 * Simplified analysis for finding all the varinfos
 * that don't have a corresponding fundec.
 * Replaces complex DriverSlicer implementation that calculated
 * the whole call graph.
 * 
 * Detail:  We include only those functions without fundecs that
 * are actually called from within the driver.  If we have a varinfo
 * (prototype) for a function that's not called and that has
 * no fundec, then we don't count it.
 *)
class pass_find_funcs_with_no_fundecs
  (f: file)
  = object (self)
    inherit nopCilVisitor

    (** List of all function declaration (prototypes) *)
    val mutable all_declarations: varinfo list = [];

    (** List of all function definitions (bodies/fundecs) *)
    val mutable all_definitions: fundec list = [];
    val mutable all_definitions_vi: varinfo list = [];

    (** List of all functions that are actually called
        from in the driver.
    *)
    val mutable all_called_vi: varinfo list = [];

    (** List of all functions prototypes that lack a body *)
    val mutable funcs_with_no_fundecs: varinfo list = [];

    method vinst (i: instr) : instr list visitAction =
      match i with
        | Call(_, Lval(Var(vi), _), _, _) ->
            let predicate (cur_vi: varinfo) : bool =
              cur_vi.vname = vi.vname
            in
            if (List.exists predicate all_called_vi) = false then
              all_called_vi <- all_called_vi @ [vi];
            DoChildren
        | _ -> DoChildren

    method vglob (g: global): global list visitAction =
      match g with
        | GVarDecl(vi, _) ->
            begin
              match vi.vtype with
                | TFun(_, _, _, _) ->
                    all_declarations <- all_declarations @ [vi];
                | _ ->
                    (* Not a function prototype *)
                    ()
            end;
            DoChildren
        | GFun(fdec, _) -> 
            all_definitions <- all_definitions @ [fdec];
            all_definitions_vi <- all_definitions_vi @ [fdec.svar];
            DoChildren
        | _ ->
            (* Not a prototype or function body *)
            DoChildren

    method one_elt (acc: varinfo list) (decl: varinfo) : varinfo list =
      (* Body exists *)
      let predicate_exists (cur_def: varinfo) : bool =
        cur_def.vname = decl.vname
      in
      let exists = List.exists predicate_exists all_definitions_vi in
      (* Called? *)
      let predicate_called (cur_called: varinfo) : bool =
        cur_called.vname = decl.vname
      in
      let called = List.exists predicate_called all_called_vi in
      (*Printf.fprintf stderr "Function processing: %s, Exists: %b, Called: %b\n" decl.vname exists called; *)
      
      (* Don't accumulate if the body exists.
         Don't accumulate if the function's not called.
      *)
      if exists || called = false then
        acc
      else
        (* Accumulate *)
        acc @ [decl]
  
    method process (): varinfo list =
      (* Build main lists *)
      Cil.visitCilFileSameGlobals (self :> cilVisitor) f;
      (* Process the lists *)
      let funcs_with_no_fundecs_unsorted = List.fold_left self#one_elt [] all_declarations in
      (* Sort the result so everthing is tidy.  This feature
         was helpful primarily to compare this implementation against
         the original DriverSlicer version
      *)
      let sortfun s1 s2 = String.compare s1.vname s2.vname in
      funcs_with_no_fundecs <- List.sort sortfun funcs_with_no_fundecs_unsorted;
      funcs_with_no_fundecs
  end

(** Some extra fundecs for special functionality implemented in test framework
    or kernel.
*)
class pass_fdec_management = object (self)
  val mutable fdecs: fundecs_group = {
    call_interrupt_handlers = emptyFunction "gen_call_interrupt_handlers";
    execute_completions = emptyFunction "gen_execute_completions";
    convert_fn = emptyFunction "gen_convert_fn";
    lookup_MJRcheck = emptyFunction "gen_lookup_MJRcheck";
    enter_driver = emptyFunction "gen_enter_driver";
    exit_driver = emptyFunction "gen_exit_driver";
    s2e_prioritize = emptyFunction "s2e_prioritize";
    s2e_deprioritize = emptyFunction "s2e_deprioritize";
    s2e_loop_before = emptyFunction "s2e_loop_before";
    s2e_loop_body = emptyFunction "s2e_loop_body";
    s2e_loop_after = emptyFunction "s2e_loop_after";
    s2e_make_symbolic = emptyFunction "s2e_make_symbolic";
    s2e_enable_trackperf = emptyFunction "s2e_enable_trackperf";
    s2e_disable_trackperf = emptyFunction "s2e_disable_trackperf";
    s2e_trackperf_fn = emptyFunction "s2e_trackperf_fn";
    
    uprintk = emptyFunction "uprintk";
    verify_in_driver = emptyFunction "gen_verify_in_driver";
    make_symbolic_int = emptyFunction "gen_make_symbolic_int";
    make_symbolic_ptr = emptyFunction "gen_make_symbolic_ptr";
    s2e_enter_block = emptyFunction "s2e_enter_block";

    g_sym_params = makeGlobalVar "g_sym_params" intType;
    g_sym_retval = makeGlobalVar "g_sym_retval" intType;

    protos = [];
  }

  method get_call_interrupt_handlers (): fundec = fdecs.call_interrupt_handlers
  method get_execute_completions (): fundec = fdecs.execute_completions
  method get_convert_fn (): fundec = fdecs.convert_fn
  method get_lookup_MJRcheck (): fundec = fdecs.lookup_MJRcheck
  method get_enter_driver (): fundec = fdecs.enter_driver
  method get_s2e_enter_block (): fundec = fdecs.s2e_enter_block
  method get_exit_driver (): fundec = fdecs.exit_driver
  method get_s2e_prioritize (): fundec = fdecs.s2e_prioritize
  method get_s2e_deprioritize (): fundec = fdecs.s2e_deprioritize
  method get_s2e_loop_before (): fundec = fdecs.s2e_loop_before
  method get_s2e_loop_body (): fundec = fdecs.s2e_loop_body
  method get_s2e_loop_after (): fundec = fdecs.s2e_loop_after
  method get_s2e_make_symbolic (): fundec = fdecs.s2e_make_symbolic
  method get_s2e_enable_trackperf (): fundec = fdecs.s2e_enable_trackperf
  method get_s2e_disable_trackperf (): fundec = fdecs.s2e_disable_trackperf
  method get_s2e_trackperf_fn (): fundec = fdecs.s2e_trackperf_fn
  method get_uprintk (): fundec = fdecs.uprintk
  method get_verify_in_driver (): fundec = fdecs.verify_in_driver
  method get_make_symbolic_int (): fundec = fdecs.make_symbolic_int
  method get_make_symbolic_ptr (): fundec = fdecs.make_symbolic_ptr

  method get_g_sym_params (): varinfo = fdecs.g_sym_params
  method get_g_sym_retval (): varinfo = fdecs.g_sym_retval

  method private mk_special_fundec (fdec: fundec): unit =
    fdec.svar.vstorage <- Extern;
    let fdec_arg1 = ("fn", charConstPtrType, []) in
    let fdec_arg2 = ("line", intType, []) in
    let fdec_args = Some ([fdec_arg1; fdec_arg2]) in
    let fdec_typ = TFun(TVoid([]), fdec_args, false, []) in
    setFunctionTypeMakeFormals fdec fdec_typ;

  method private mk_convert_fn_fundec (): unit =
    fdecs.convert_fn.svar.vstorage <- Extern;
    let fn_arg1 = "fn_to_convert", TPtr(voidPtrType, []), [] in
    let fn_arg2 = "caller", charConstPtrType, [] in
    let fn_arg3 = "str1", charConstPtrType, [] in
    let fn_arg4 = "str2", charConstPtrType, [] in
    let fn_args_list = [fn_arg1; fn_arg2; fn_arg3; fn_arg4] in
    let fn_args = Some(fn_args_list) in
    let fn_typ = TFun(voidType, fn_args, false, []) in
    setFunctionTypeMakeFormals fdecs.convert_fn fn_typ;

  method private mk_lookup_MJRcheck_fundec (): unit =
    fdecs.lookup_MJRcheck.svar.vstorage <- Extern;
    let fn_arg1 = "wrapper_type", charConstPtrType, [] in
    let fn_arg2 = "function", voidPtrType, [] in
    let fn_args_list = [fn_arg1; fn_arg2] in
    let fn_args = Some(fn_args_list) in
    let fn_typ = TFun(voidPtrType, fn_args, false, []) in
    setFunctionTypeMakeFormals fdecs.lookup_MJRcheck fn_typ;

  method private mk_enter_exit_driver_fundec (fdec: fundec): unit =
    fdec.svar.vstorage <- Extern;
    let fdec_arg1 = ("fn", charConstPtrType, []) in
    let fdec_arg2 = ("line", intType, []) in
    let fdec_arg3 = ("wrapper_type", intType, []) in
    let fdec_arg4 = ("retval_valid", intType, []) in
    let fdec_arg5 = ("retval", ulongType, []) in
    let fdec_arg6 = ("interesting", charConstPtrType, []) in
    let fdec_arg7 = ("gen_register_all", voidPtrType, []) in
    let fdec_args = Some ([fdec_arg1; fdec_arg2; fdec_arg3;
                           fdec_arg4; fdec_arg5; fdec_arg6;
                           fdec_arg7]
                         ) in
    let fdec_typ = TFun(TInt(IInt, []), fdec_args, false, []) in
    setFunctionTypeMakeFormals fdec fdec_typ;

  method private mk_s2e_enter_block_fundec (): unit =
    fdecs.s2e_enter_block.svar.vstorage <- Static;
    fdecs.s2e_enter_block.svar.vinline <- true;
    fdecs.s2e_enter_block.svar.vattr <- [ Attr("always_inline", []) ];
    let fdec_arg1 = ("line", intType, []) in
    let fdec_arg2 = ("fn", charConstPtrType, []) in
    let fdec_arg3 = ("total_blocks", intType, []) in
    let fdec_arg4 = ("cur_block", intType, []) in
    let fdec_args = Some ([fdec_arg1; fdec_arg2; fdec_arg3; fdec_arg4]) in
    let fdec_typ = TFun(TVoid([]), fdec_args, false, []) in
    setFunctionTypeMakeFormals fdecs.s2e_enter_block fdec_typ;

  method private mk_s2e_fn (fdec: fundec): unit =
    fdec.svar.vstorage <- Static;
    fdec.svar.vinline <- true;
    fdec.svar.vattr <- [ Attr("always_inline", []) ];
    let fdec_arg = ("line", intType, []) in
    let fdec_args = Some ([fdec_arg]) in
    let fdec_typ = TFun(TVoid([]), fdec_args, false, []) in
    setFunctionTypeMakeFormals fdec fdec_typ;

  method private mk_s2e_loop_fn (fdec: fundec): unit =
    fdec.svar.vstorage <- Static;
    fdec.svar.vinline <- true;
    fdec.svar.vattr <- [ Attr("always_inline", []) ];
    let fdec_arg1 = ("line", intType, []) in
    let fdec_arg2 = ("call_id", intType, []) in
    let fdec_args = Some ([fdec_arg1; fdec_arg2]) in
    let fdec_typ = TFun(TVoid([]), fdec_args, false, []) in
    setFunctionTypeMakeFormals fdec fdec_typ;

  method private mk_s2e_make_symbolic (): unit =
    fdecs.s2e_make_symbolic.svar.vstorage <- Static;
    fdecs.s2e_make_symbolic.svar.vinline <- true;
    fdecs.s2e_make_symbolic.svar.vattr <- [ Attr("always_inline", []) ];
    let fdec_arg1 = ("buf", voidPtrType, []) in
    let fdec_arg2 = ("size", intType, []) in
    let fdec_arg3 = ("name", charConstPtrType, []) in
    let fdec_args = Some ([fdec_arg1; fdec_arg2; fdec_arg3]) in
    let fdec_typ = TFun(TVoid([]), fdec_args, false, []) in
    setFunctionTypeMakeFormals fdecs.s2e_make_symbolic fdec_typ;

  method private mk_s2e_enable_trackperf (): unit =
    fdecs.s2e_enable_trackperf.svar.vstorage <- Static;
    fdecs.s2e_enable_trackperf.svar.vinline <- true;
    fdecs.s2e_enable_trackperf.svar.vattr <- [ Attr("always_inline", []) ];
    let fdec_arg1 = ("line", intType, []) in
    let fdec_arg2 = ("flags", intType, []) in
    let fdec_args = Some ([fdec_arg1; fdec_arg2]) in
    let fdec_typ = TFun(TVoid([]), fdec_args, false, []) in
    setFunctionTypeMakeFormals fdecs.s2e_enable_trackperf fdec_typ;

  method private mk_s2e_disable_trackperf (): unit =
    fdecs.s2e_disable_trackperf.svar.vstorage <- Static;
    fdecs.s2e_disable_trackperf.svar.vinline <- true;
    fdecs.s2e_disable_trackperf.svar.vattr <- [ Attr("always_inline", []) ];
    let fdec_arg1 = ("line", intType, []) in
    let fdec_arg2 = ("flags", intType, []) in
    let fdec_args = Some ([fdec_arg1; fdec_arg2]) in
    let fdec_typ = TFun(TVoid([]), fdec_args, false, []) in
    setFunctionTypeMakeFormals fdecs.s2e_disable_trackperf fdec_typ;

  method private mk_s2e_trackperf_fn (): unit =
    fdecs.s2e_trackperf_fn.svar.vstorage <- Static;
    fdecs.s2e_trackperf_fn.svar.vinline <- true;
    fdecs.s2e_trackperf_fn.svar.vattr <- [ Attr("always_inline", []) ];
    let fdec_arg1 = ("line", intType, []) in
    let fdec_arg2 = ("fn", charConstPtrType, []) in
    let fdec_arg3 = ("flags", intType, []) in
    let fdec_args = Some ([fdec_arg1; fdec_arg2; fdec_arg3]) in
    let fdec_typ = TFun(TVoid([]), fdec_args, false, []) in
    setFunctionTypeMakeFormals fdecs.s2e_trackperf_fn fdec_typ;

  method private mk_uprintk (): unit =
    fdecs.uprintk.svar.vstorage <- Extern;
    let uprintk_arg = ("str", charConstPtrType, []) in
    let uprintk_args = Some ([uprintk_arg]) in
    let uprintk_typ = TFun(TInt(IInt, []), uprintk_args, true, []) in
    setFunctionTypeMakeFormals fdecs.uprintk uprintk_typ;

  method private mk_make_symbolic_int (): unit =
    fdecs.make_symbolic_int.svar.vstorage <- Extern;
    let make_symbolic_int_arg1 = ("fn", charConstPtrType, []) in
    let make_symbolic_int_arg2 = ("line", intType, []) in
    let make_symbolic_int_arg3 = ("int_retval", voidPtrType, []) in
    let make_symbolic_int_arg4 = ("int_retval_size", TInt(IInt, []), []) in
    let make_symbolic_int_args = Some ([make_symbolic_int_arg1;
                                        make_symbolic_int_arg2;
                                        make_symbolic_int_arg3;
                                        make_symbolic_int_arg4])
    in
    let make_symbolic_int_typ = TFun(TInt(IInt, []), make_symbolic_int_args, false, []) in
    setFunctionTypeMakeFormals fdecs.make_symbolic_int make_symbolic_int_typ;

  method private mk_make_symbolic_ptr (): unit =
    fdecs.make_symbolic_ptr.svar.vstorage <- Extern;
    let make_symbolic_ptr_arg1 = ("fn", charConstPtrType, []) in
    let make_symbolic_ptr_arg2 = ("line", intType, []) in
    let make_symbolic_ptr_arg3 = ("ptr_retval", TPtr(TPtr(TVoid([]), []), []), []) in
    let make_symbolic_ptr_args = Some ([make_symbolic_ptr_arg1;
                                        make_symbolic_ptr_arg2;
                                        make_symbolic_ptr_arg3])
    in
    let make_symbolic_ptr_typ = TFun(TInt(IInt, []), make_symbolic_ptr_args, false, []) in
    setFunctionTypeMakeFormals fdecs.make_symbolic_ptr make_symbolic_ptr_typ;

  method private mk_g_sym_params (): unit =
    fdecs.g_sym_params.vstorage <- Extern;

  method private mk_g_sym_retval (): unit =
    fdecs.g_sym_retval.vstorage <- Extern;
    
  method get_protos (): unit =
    let call_interrupt_handlers_varinfo = fdecs.call_interrupt_handlers.svar in
    let call_interrupt_handlers_proto = GVarDecl(call_interrupt_handlers_varinfo, locUnknown) in

    let execute_completions_varinfo = fdecs.execute_completions.svar in
    let execute_completions_proto = GVarDecl(execute_completions_varinfo, locUnknown) in

    let convert_fn_varinfo = fdecs.convert_fn.svar in
    let convert_fn_proto = GVarDecl(convert_fn_varinfo, locUnknown) in

    let lookup_MJRcheck_varinfo = fdecs.lookup_MJRcheck.svar in
    let lookup_MJRcheck_proto = GVarDecl(lookup_MJRcheck_varinfo, locUnknown) in

    let enter_driver_varinfo = fdecs.enter_driver.svar in
    let enter_driver_proto = GVarDecl(enter_driver_varinfo, locUnknown) in

    let s2e_enter_block_varinfo = fdecs.s2e_enter_block.svar in
    let s2e_enter_block_proto = GVarDecl(s2e_enter_block_varinfo, locUnknown) in

    let exit_driver_varinfo = fdecs.exit_driver.svar in
    let exit_driver_proto = GVarDecl(exit_driver_varinfo, locUnknown) in

    let s2e_prioritize_varinfo = fdecs.s2e_prioritize.svar in
    let s2e_prioritize_proto = GVarDecl(s2e_prioritize_varinfo, locUnknown) in

    let s2e_deprioritize_varinfo = fdecs.s2e_deprioritize.svar in
    let s2e_deprioritize_proto = GVarDecl(s2e_deprioritize_varinfo, locUnknown) in

    let s2e_loop_before_varinfo = fdecs.s2e_loop_before.svar in
    let s2e_loop_before_proto = GVarDecl(s2e_loop_before_varinfo, locUnknown) in

    let s2e_loop_body_varinfo = fdecs.s2e_loop_body.svar in
    let s2e_loop_body_proto = GVarDecl(s2e_loop_body_varinfo, locUnknown) in

    let s2e_loop_after_varinfo = fdecs.s2e_loop_after.svar in
    let s2e_loop_after_proto = GVarDecl(s2e_loop_after_varinfo, locUnknown) in

    let s2e_make_symbolic_varinfo = fdecs.s2e_make_symbolic.svar in
    let s2e_make_symbolic_proto = GVarDecl(s2e_make_symbolic_varinfo, locUnknown) in

    let s2e_enable_trackperf_varinfo = fdecs.s2e_enable_trackperf.svar in
    let s2e_enable_trackperf_proto = GVarDecl(s2e_enable_trackperf_varinfo, locUnknown) in

    let s2e_disable_trackperf_varinfo = fdecs.s2e_disable_trackperf.svar in
    let s2e_disable_trackperf_proto = GVarDecl(s2e_disable_trackperf_varinfo, locUnknown) in

    let s2e_trackperf_fn_varinfo = fdecs.s2e_trackperf_fn.svar in
    let s2e_trackperf_fn_proto = GVarDecl(s2e_trackperf_fn_varinfo, locUnknown) in

    let uprintk_varinfo = fdecs.uprintk.svar in
    let uprintk_proto = GVarDecl(uprintk_varinfo, locUnknown) in

    let verify_in_driver_varinfo = fdecs.verify_in_driver.svar in
    let verify_in_driver_proto = GVarDecl(verify_in_driver_varinfo, locUnknown) in
    
    let make_symbolic_int_varinfo = fdecs.make_symbolic_int.svar in
    let make_symbolic_int_proto = GVarDecl(make_symbolic_int_varinfo, locUnknown) in

    let make_symbolic_ptr_varinfo = fdecs.make_symbolic_ptr.svar in
    let make_symbolic_ptr_proto = GVarDecl(make_symbolic_ptr_varinfo, locUnknown) in

    let g_sym_params_varinfo = fdecs.g_sym_params in
    let g_sym_params_proto = GVarDecl(g_sym_params_varinfo, locUnknown) in

    let g_sym_retval_varinfo = fdecs.g_sym_retval in
    let g_sym_retval_proto = GVarDecl(g_sym_retval_varinfo, locUnknown) in

    let list = [call_interrupt_handlers_proto;
                execute_completions_proto;
                convert_fn_proto;
                lookup_MJRcheck_proto;
                enter_driver_proto;
                s2e_enter_block_proto;
                exit_driver_proto;
                s2e_prioritize_proto;
                s2e_deprioritize_proto;
                s2e_loop_before_proto;
                s2e_loop_body_proto;
                s2e_loop_after_proto;
                s2e_make_symbolic_proto;
                s2e_enable_trackperf_proto;
                s2e_disable_trackperf_proto;
                s2e_trackperf_fn_proto;
                uprintk_proto;
                verify_in_driver_proto;
                make_symbolic_int_proto;
                make_symbolic_ptr_proto;
                
                g_sym_params_proto;
                g_sym_retval_proto;
               ] in
    fdecs.protos <- list;

  method process (): fundecs_group =
    self#mk_special_fundec fdecs.call_interrupt_handlers;
    self#mk_special_fundec fdecs.execute_completions;
    self#mk_convert_fn_fundec ();
    self#mk_lookup_MJRcheck_fundec ();
    self#mk_enter_exit_driver_fundec fdecs.enter_driver;
    self#mk_s2e_enter_block_fundec ();
    self#mk_enter_exit_driver_fundec fdecs.exit_driver;
    self#mk_s2e_fn fdecs.s2e_prioritize;
    self#mk_s2e_fn fdecs.s2e_deprioritize;
    self#mk_s2e_loop_fn fdecs.s2e_loop_before;
    self#mk_s2e_loop_fn fdecs.s2e_loop_body;
    self#mk_s2e_loop_fn fdecs.s2e_loop_after;
    self#mk_s2e_make_symbolic ();
    self#mk_s2e_enable_trackperf ();
    self#mk_s2e_disable_trackperf ();
    self#mk_s2e_trackperf_fn ();
    self#mk_uprintk ();
    self#mk_special_fundec fdecs.verify_in_driver;
    self#mk_make_symbolic_int ();
    self#mk_make_symbolic_ptr ();

    self#mk_g_sym_params ();
    self#mk_g_sym_retval ();
    
    self#get_protos ();

    fdecs;
end

class pass_builtin_expect
  (f: file)
  (fdecs_group: fundecs_group)
  = object (self)
    inherit nopCilVisitor

    val mutable cur_fundec: fundec = emptyFunction "";
    
    method split_instrs (instr_list: instr list) : (instr list * exp option) list =
      let remove_last (lst : 'a list) : 'a list = (* Removes last element *)
        List.rev (List.tl (List.rev lst))
      in
      let extract_last (lst : 'a list) : 'a =
        List.nth lst ((List.length lst) - 1)
      in
      let split
          (acc: (instr list * exp option) list)
          (cur_instr: instr)
          : (instr list * exp option) list =
        let basic_operation (e_opt : exp option) : (instr list * exp option) list =
          match acc with
            | [] ->
                Printf.fprintf stderr "Basic_operation\n";
                [[cur_instr], e_opt]
            | _ ->
                let i_list, e_opt2 = extract_last acc in
                let new_head = remove_last acc in
                let new_tail = [(i_list @ [cur_instr]), e_opt2] in
                let orig_len = List.length acc in
                let new_len = List.length new_head + 1 in
                if orig_len <> new_len then
                  begin
                    Printf.fprintf stderr "Mismatch %d != %d\n" orig_len new_len;
                    fatal ["Bug mismatch in builtin_expect"];
                  end;
                new_head @ new_tail;
        in
        match cur_instr with
          | Call(Some (lv), call_exp, e_list, _) ->
              let name = exp_tostring call_exp in
              Printf.fprintf stderr ">>> %s\n" name;
              if name = "__builtin_expect" && (List.length e_list) = 2 then
                begin
                  (* Built-in Function: long __builtin_expect (long exp, long c) *)
                  (* The return value is the value of exp, which
                     should be an integral expression.  The semantics
                     of the built-in are that it is expected that exp == c.
                     For example:

                     if (__builtin_expect (x, 0)) foo ();
                     
                     would indicate that we do not expect to call foo,
                     since we expect x to be zero.
                  *)
                  let second_e = List.nth e_list 1 in
                  let expression = BinOp(Eq, expify_lval lv, second_e, TInt(IBool, [])) in
                  Printf.fprintf stderr "__builtin_expect found.\n";
                  basic_operation (Some(expression))
                end
              else
                basic_operation None
          | _ ->
              basic_operation None
      in
      Printf.fprintf stderr "instr_list length: %d\n" (List.length instr_list);
      List.fold_left split [] instr_list

    method private count_instrs (lists: (instr list * exp option) list) : int =
      let count_fn (acc: int) (pair: (instr list * exp option)) : int =
        let first, second = pair in
        acc + (List.length first)
      in
      let count = List.fold_left count_fn 0 lists in
      count;

    method private flatten_instrs (lists: (instr list * exp option) list) : instr list =
      let flatten (acc: instr list) (pair: (instr list * exp option)) : instr list =
        let first, second = pair in
        acc @ first
      in
      List.fold_left flatten [] lists;
      
    method private process_stmt (acc: stmt list) (cur_stmt: stmt): stmt list =
      match cur_stmt.skind with
        | Instr(i_list) ->
            let lists = self#split_instrs i_list in
            let count = self#count_instrs lists in
            if count <> List.length i_list then
              fatal ["Lost some instrs"];
            
            let combine_complex (acc: stmt list) (cur_list: (instr list * exp option)) : stmt list =
              let i_list, e_opt = cur_list in
              match e_opt with
                | None ->
                    (* This is shady.
                       We want to preserve the old labels so we re-use cur_stmt
                       Likely bugged if lists length > 1
                    *)
                    cur_stmt.skind <- Instr (i_list);
                    acc @ [cur_stmt]
                | Some(e) ->
                    let expression = e in
                    let s2e_prioritize_arg1_vi = makeLocalVar cur_fundec ~insert:false "__LINE__" intType in
                    let s2e_prioritize_arg1_exp = expify_varinfo s2e_prioritize_arg1_vi in
                    let s2e_prioritize_exp = expify_fundec fdecs_group.s2e_prioritize in
                    let s2e_prioritize = mkCallStmt None s2e_prioritize_exp [s2e_prioritize_arg1_exp] in
                    let s2e_deprioritize_arg1_exp = UnOp (Neg, s2e_prioritize_arg1_exp, intType) in
                    let s2e_deprioritize_exp = expify_fundec fdecs_group.s2e_deprioritize in
                    let s2e_deprioritize = mkCallStmt None s2e_deprioritize_exp [s2e_deprioritize_arg1_exp] in
                    let if_stmt = mkIfStmt expression [s2e_prioritize] [s2e_deprioritize] in
                    let new_stmt = mkStmt (Instr (i_list)) in
                    acc @ [new_stmt] @ [if_stmt]
            in
            if List.length lists > 1 then
              begin
                Printf.fprintf stderr "split_lists len: %d\n" (List.length lists);
                fatal ["Make sure this case is supported. "; cur_fundec.svar.vname]
              end
            else if List.length lists = 1 then
              acc @ (List.fold_left combine_complex [] lists)
            else
              acc @ [cur_stmt]
        | _ -> acc @ [cur_stmt]
        
    method vblock (b: block): block visitAction =
      let before_length = List.length b.bstmts in
      let new_stmts = List.fold_left self#process_stmt [] b.bstmts in
      let after_length = List.length new_stmts in
      if after_length < before_length then
        Printf.fprintf stderr "Error: %d < %d\n" after_length before_length
      else
        Printf.fprintf stderr "OK: %d >= %d\n" after_length before_length;
      b.bstmts <- new_stmts;
      DoChildren

    method vfunc (f: fundec): fundec visitAction =
      Printf.fprintf stderr "Current: %s\n" f.svar.vname;
      cur_fundec <- f;
      DoChildren

    method process (): unit =
      Cil.visitCilFileSameGlobals (self :> cilVisitor) f
  end

class pass_find_irritating_fns
  (f: file) = object (self)
    inherit nopCilVisitor

    val mutable current_fn : string = "";
    val mutable interesting_core_fns : (string * string) list = []
      
    (* The idea here was to force execution down paths that trigger
       these calls.  Turns out that et131x needs this.  But this still
       does not do quite what we want. *)
    val mutable interesting_driver_fns : (string * string) list =
      [
(*        "napi_schedule";
        "__netif_rx_schedule";
        "netif_rx";
*)
        "netif_carrier_on", "";
        "schedule_work", "";
      ];
    
    method private get_nonstub_fns (): string list =
      let builtins = (list_keys gccBuiltins) in
      let more_builtins = [
        (* For performance *)
        "__builtin_expect";
        "__builtin_memcmp";
        "memcmp";
        "memcpy";
        "memset";

        (* These are actually defined as functions and we are
           inappropriately deprioritizing those calls that return
           non-zero values. So add them, to prevent inappropriate
           deprioritization

           Moreover don't run the test framework for them.  Could
           change this but it's a simple performance optimization *)
        "readb";
        "readw";
        "readl";
        "readsb";
        "readsw";
        "readsl";
        "writeb";
        "writew";
        "writel";
        "writesb";
        "writesw";
        "writesl";

        "inb";
        "inw";
        "inl";
        "inb_local";
        "inw_local";
        "inl_local";
        "insb";
        "insw";
        "insl";
        "inb_p";
        "inw_p";
        "inl_p";

        "outb";
        "outw";
        "outl";
        "outb_local";
        "outw_local";
        "outl_local";
        "outsb";
        "outsw";
        "outsl";
        "outb_p";
        "outw_p";
        "outl_p";

        "ioread8";
        "ioread8_rep";
        "ioread16";
        "ioread16_rep";
        "ioread32";
        "ioread32_rep";
        "ioread16be";
        "ioread32be";

        "iowrite8";
        "iowrite8_rep";
        "iowrite16";
        "iowrite16_rep";
        "iowrite16be";
        "iowrite32";
        "iowrite32_rep";
        "iowrite32be";

        (* I2C *)
        "i2c_master_send";
        "i2c_master_recv";
        "i2c_transfer";
        "i2c_smbus_xfer";
        "i2c_smbus_read_byte";
        "i2c_smbus_write_byte";
        "i2c_smbus_read_byte_data";
        "i2c_smbus_write_byte_data";
        "i2c_smbus_read_word_data";
        "i2c_smbus_write_word_data";
        "i2c_smbus_read_block_data";
        "i2c_smbus_write_block_data";
        "i2c_smbus_read_i2c_block_data";
        "i2c_smbus_write_i2c_block_data";

        (* Sound library *)
        "div32";

        (* SPI drivers *)
        (* We saw an unusual situation in ks8851 during initialization
           if this function is treated as a normal kernel function *)
        "spi_sync";

        (* Android *)
        "__bad_percpu_size"; (* C idiom to halt compilation on bug *)
        "__cmpxchg_wrong_size"; (* ... *)

        (* r8169 *)
        "dev_to_node";
        "__pm_runtime_idle";
        "pm_runtime_put_sync";

        (* TG3 *)
        "_tg3_flag";
        "_tg3_flag_set";
        "_tg3_flag_clear";
        "tg3_write32";
        "tg3_read32";
        "tg3_ape_write32";
        "tg3_ape_read32";
        "tg3_write_indirect_reg32";
        "tg3_write_flush_reg32";
        "tg3_read_indirect_reg32";
        "tg3_write_indirect_mbox";
        "tg3_read_indirect_mbox";
        "tg3_read32_mbox_5906";
        "tg3_write32_mbox_5906";
        
        (* Wireless *)
        "__bad_size_call_parameter";
        "__xchg_wrong_size";
      ] in
      builtins @ more_builtins;

    (* These functions have strange return values that we should
       not prioritize / deprioritize. See splitter_dri for usage of this list.

       These functions also don't get symbolic retvals.
    *)
    method private get_nonstandard_retval_fns (): string list =
      [
        "dma_alloc_coherent_mask";
        "capable";

        "test_and_set_bit";
        "test_and_clear_bit";
        "constant_test_bit";
        "variable_test_bit";
        "fls";
        "__arch_swab32";
        "__fswab16";
        "__fswab32";
        "get_order";
        "__raw_local_save_flags";
        "__raw_local_irq_save";
        "raw_irqs_disabled_flags";

        "PTR_ERR";
        "ERR_PTR";

        (* Ens1371 *)
        "snd_ac97_update_bits";
        "find_snd_minor";
        "snd_unregister_device";
        "setheader";

        (* Pluto2 / I2C *)
        "sendbytes";

        (* leds-lp5523 *)
        "dev_get_drvdata";

        (* Orinoco *)
        (* Tries to lock and returns failure if it can't -- should
           not deprioritize in that case *)
        "orinoco_lock";
        "dblock_addr";
        "dblock_len";
        "pdr_id";
        "pdr_addr";
        "pdr_len";
        "pdi_id";
        "pdi_len";

      (* This function is just wrappers for readb etc *)
      (* This function is called from in the kernel repeatedly
         and we erroneously prioritize a 0 path *)
      (* TODO: What is the proper way to deal with functions that
         return symbolic hardware data like this?  if
         (is_symbolic_retval) { don't prioritize; } ???  *)
      (*"pluto_readreg";*)

        (* FreeBSD *)
        "PCI_FIND_EXTCAP";
        "pci_find_cap";
        "pci_get_revid";
        "pci_get_slot";
        "PCI_SET_POWERSTATE";
        "pci_set_powerstate";

        (* if_ed *)
        "ed_pio_write_mbufs";
        "max";
        "min";
        "imax";
        "imin";
        "lmax";
        "lmin";
        "qmax";
        "qmin";
        "ulmax";
        "ulmin";
        "omax";
        "omin";
        "abs";
        "labs";
        "qabs";

        (* if_re *)
        "bpf_peers_present";
        (*"re_ioctl";*)

        (* if_rl *)
        "device_get_softc";
        "device_is_attached";
        "rl_ioctl";

        (* if_bge *)
        "PCI_GET_VPD_IDENT";
        "pci_get_vpd_ident";
        
        (* KS8851 *)
        "skb_put";
      ] @ self#get_nonstub_fns();

    method private get_drivermacro_fns (): string list =
      [
        "agg_intr";
      ]

    method vinst (i: instr) : instr list visitAction =
      match i with
        | Call(_, e, _, _) ->
            let called_fn = exp_tostring e in
            let predicate_core ((s, t): (string * string)) : bool  = (s = called_fn) in
            let predicate_driver ((s, t):(string * string)) : bool = (s = current_fn) in
            if List.exists predicate_core interesting_core_fns &&
              not (List.exists predicate_driver interesting_driver_fns) then
                interesting_driver_fns <- interesting_driver_fns @ [current_fn, called_fn];
            DoChildren
        | _ -> DoChildren;

    method vfunc (fdec: fundec): fundec visitAction =
      current_fn <- fdec.svar.vname;
      DoChildren

    method private print_interesting_fns (): unit =
      let iter_fn ((s, t): (string * string)) = Printf.fprintf stderr "%s -> %s\n" s t in
      List.iter iter_fn interesting_driver_fns;
      Printf.fprintf stderr "===================\n";
      
    method private get_interesting_driver_fns (): (string * string) list =
      while List.length interesting_driver_fns <> List.length interesting_core_fns do
        let map (x, y) = (x, y) in
        interesting_core_fns <- List.map map interesting_driver_fns;
        Cil.visitCilFileSameGlobals (self :> cilVisitor) f;
      done;
      interesting_driver_fns

    method process ():
      string list * string list *
      (string * string) list * string list =
      (self#get_nonstub_fns (),
       self#get_nonstandard_retval_fns(),
       self#get_interesting_driver_fns(),
       self#get_drivermacro_fns()
      )
  end

class pass_add_s2e_enter_block
  (fdecs: fundecs_group)
  = object (self)
    inherit nopCilVisitor
      
    val mutable cur_fn : fundec option = None
    val mutable total_blocks : int ref = ref 0
    val mutable current_block : int ref = ref 0
    val mutable pass : int ref = ref 0
   
    method gen_s2e_enter_block (): stmt list =
      current_block := !current_block + 1;
      let some_cur_fn =
        match cur_fn with
          | Some(fn) -> fn
          | None -> fatal ["pass_add_s2e_enter_block"]
      in
      let call_exp = expify_fundec fdecs.s2e_enter_block in
      let arg1_vi = makeLocalVar some_cur_fn ~insert:false "__LINE__" intType in
      let arg1 = expify_varinfo arg1_vi in
      let arg2 = mkString some_cur_fn.svar.vname in
      let arg3 = integer !total_blocks in
      let arg4 = integer !current_block in
      [mkCallStmt None call_exp [arg1; arg2; arg3; arg4]]

    method private process_block (b: block): unit =
      let process_block_stmt (s: stmt) : unit =
        match s.skind with
          | If(_, b1, b2, _) ->
              self#process_block b1;
              self#process_block b2;
          | Switch(_, b1, _, _) ->
              self#process_block b1;
          | Loop(b1, _, _, _) ->
              self#process_block b1;
          | Block(sub_b) -> self#process_block sub_b
          | _ -> ()
      in
      List.iter process_block_stmt b.bstmts;

      if !pass = 0 then
        total_blocks := !total_blocks + 1
      else
        begin
          if !pass = 1 then
            b.bstmts <- self#gen_s2e_enter_block() @ b.bstmts
          else
            fatal ["pass_add_s2e_enter_block Pass = 0 or 1"];
        end;
        
    method vfunc (f: fundec): fundec visitAction =
      total_blocks := 0;
      cur_fn <- Some(f);
      pass := 0;
      self#process_block f.sbody;

      pass := 1;
      current_block := 0;
      self#process_block f.sbody;
      DoChildren

    method process (f: file): unit =
      Cil.visitCilFileSameGlobals (self :> cilVisitor) f;
  end

class pass_add_prepostfn
  (nonstub_fn_names: string list)
  (drivermacro_fns: string list)
  = object (self)
    
    inherit nopCilVisitor
      
    (** Store reference to current file *)
    val mutable globals: global list = [];

    (** prepostfn_funcs: A list of prefn/postfn functions that have been added *)
    val mutable prepostfn_funcs: (fundec * fundec) list = [];

    (** Store the original non pre/postfn functions *)
    (** non_prepostfn_funcs: All fundecs excluding prepostfn *)
    val mutable non_prepostfn_funcs: fundec list = [];

    (** add_prepost_function: Add a wrapper for "interface"
     * functions that may be called from the kernel
     * Does not add the fundec to the globals list
     *
     * Note that we drop varargs entirely -- there is no general
     * way to make a wrapper for a vararg function in C as far as
     * I know without resorting to really yucky games like libffi.
     *)
    method create_prepostfn_function (vi: varinfo) (prefn_bool: bool) : fundec =
      let prefix =
        if prefn_bool then
          prefn_prefix
        else
          postfn_prefix
      in
      let prepostfn_funcname = prefix ^ vi.vname in
      let prepostfn_func = emptyFunction prepostfn_funcname in
      let (temp_ret, temp_args, _, temp_attr) = splitFunctionType vi.vtype in
      let add_ptr (param: (string * typ * attributes)) =
        let name, t, attr = param in
        name, TPtr (t, []), attr
      in
      let temp_args_addrof = 
        match temp_args with
          | Some(args) -> List.map add_ptr args
          | None -> []
      in
      let retval_arg =
        if prefn_bool = false then
          ["retval", TPtr(temp_ret, []), []]
        else
          []
      in
      let retval_valid =
        if prefn_bool = false then
          ["retval_valid", TInt(IInt, []), []]
        else
          []
      in
      let retval_typ = TInt(IInt, []) in
      let all_args = Some (retval_valid @ retval_arg @ temp_args_addrof) in
      begin
        try
          setFunctionTypeMakeFormals prepostfn_func (TFun (retval_typ, all_args, false, temp_attr));
        with _ ->
          fatal ["Function that's a problem 1: "; prepostfn_func.svar.vname];
      end;
      prepostfn_func.svar.vstorage <- Static; (* Pre/post fn is static *)
      prepostfn_func.svar.vinline <- true; (* Make it inline too if possible *)
      (*Printf.fprintf stderr "%s: Number of arguments: %d\n"
        prepostfn_func.svar.vname
        (List.length prepostfn_func.sformals);*)
      prepostfn_func;

    (* Create a prepostfn prototype and add it to the list of globals. *)
    (* @vi For reference:  This is the associated driver function, e.g. the probe function *)
    method setup_prepostfn
      (head: global list)
      (curr_glob: global)
      (remainder: global list)
      (vi: varinfo)
      : global list =
      let create_fn (prefn_bool: bool) : (fundec * global) =
        (*Printf.fprintf stderr "Inserting prepostfn: %s\n" vi.vname; *)
        (* Create the fundec *)
        let prepostfn_fundec = self#create_prepostfn_function vi prefn_bool in
        (* Create the prototype *)
        let prepostfn_decl = GVarDecl(prepostfn_fundec.svar, locUnknown) in
        (prepostfn_fundec, prepostfn_decl)
      in
      let (prefn_fundec, prefn_global) = create_fn true in
      let (postfn_fundec, postfn_global) = create_fn false in
      let pair = (prefn_fundec, postfn_fundec) in
      prepostfn_funcs <- [pair] @ prepostfn_funcs;
      (* Insert the prototype and create a new list of globals *)
      let new_list = head @ [prefn_global; postfn_global; curr_glob] @ remainder in
      (* Store the fundec itself but don't add to globals. *)
      (* Note the order here is important -- we insert the new one
         at the head of the list.
      *)
      new_list;

    (* Finds a function in the globals array and adds a corresponding
       prepostfn prototype.  Also creates an empty fundec and stores it in a
       list but does not add it to the globals.

       Returns the new prepostfn varinfo, if any (the prototype)
    *)
    method insert_prepostfn_proto (name: string): unit =
      let rec search (head: global list) (tail: global list): global list =
        match tail with
          | GVarDecl(vi, loc) :: remainder when vi.vname = name ->
              begin
                match vi.vtype with
                  | TFun(_, _, _, _) ->
                      self#setup_prepostfn head (GVarDecl(vi, loc)) remainder vi;
                  | _ ->
                      fatal ["Specified "; name; " but did not get a function."];
              end
          | GFun(fdec, loc) :: remainder when fdec.svar.vname = name ->
              (* Found a match *)
              self#setup_prepostfn head (GFun(fdec, loc)) remainder fdec.svar;
          | current :: remainder ->
              search (head @ [current]) remainder (* tail recursive *)
          | [] -> (* Not found *)
              head
      in
      let new_list = search [] globals in
      globals <- new_list;

    method vfunc (fdec: fundec): fundec visitAction =
      if List.mem fdec.svar.vname nonstub_fn_names = false then
        begin
          self#insert_prepostfn_proto fdec.svar.vname;
          (* Be sure to add the fdec to the head of the list --
             see setup_prepostfn *)
          non_prepostfn_funcs <- [fdec] @ non_prepostfn_funcs;
        end;
      DoChildren

    method process (f: file): ((fundec * fundec) list * fundec list) =
      globals <- f.globals;
      Cil.visitCilFileSameGlobals (self :> cilVisitor) f;
      f.globals <- globals;
      (prepostfn_funcs, non_prepostfn_funcs);
  end

class pass_add_stubs
  (funcs_with_no_fundecs: varinfo list)
  (nonstub_fn_names: string list)
  = object (self)
    
    (* Store the new stub functions *)
    val mutable stub_funcs: fundec list = [];
    (* Any that we did not do *)
    val mutable nonstub_funcs: varinfo list = [];

    method is_nonstub_fn (fn: string): bool =
      List.mem fn nonstub_fn_names
        
    method gen_empty_stub
      (stub_name: string) (* Name of stub *)
      (stub_typ: typ)     (* Function signature *)
      : fundec =
      let new_fundec = emptyFunction stub_name in
      new_fundec.svar.vglob <- true;
      new_fundec.svar.vdecl <- locUnknown;
      new_fundec.svar.vaddrof <- false;
      new_fundec.svar.vreferenced <- false;
      new_fundec.svar.vtype <- stub_typ;
      new_fundec.svar.vstorage <- Static; (* Stubs are static *)
      new_fundec.svar.vinline <- true; (* Make it inline too if possible *)
      (* Leave these stubs as non-static since the original prototypes were non-static *)
      (* Then copy all the parameters from the existing function *)
      let (rettyp,argsopt,_,_) = splitFunctionType stub_typ in
      begin
        match argsopt with
          | Some(al) ->
              (* Add the existing arguments *)
              for j = 0 to (List.length al) - 1 do
                let (jthstr,jthtyp,jthattr) = (List.nth al j) in
                let newformal = makeFormalVar new_fundec jthstr jthtyp in
                newformal.vattr <- jthattr;
              done;
          | None -> ()
      end;

      (* Finally set the return type *)
      let (_, new_args, _, new_attrs) =
        splitFunctionType new_fundec.svar.vtype in
      let new_typ = TFun (rettyp, new_args, false, new_attrs) in
      (* Printf.fprintf stderr "%s\n" stub_name; *)
      begin
        try
          setFunctionType new_fundec new_typ;
        with _ ->
          fatal ["Function that's a problem 2: "; new_fundec.svar.vname];
      end;
      new_fundec;
      
    (* The original kernel fn is renamed *)
    method gen_stub (vi: varinfo): unit =
      (* See if this function can be handled *)
      let (_,_,isvararg,_) = splitFunctionType vi.vtype in
      if isvararg = false && (self#is_nonstub_fn vi.vname = false) then
        begin
          (* Only rename those functions that we're turning into stubs *)
          self#rename_stub vi;
          let new_stub = self#gen_empty_stub vi.vname vi.vtype in
          stub_funcs <- stub_funcs @ [new_stub];
        end
      else
        nonstub_funcs <- nonstub_funcs @ [vi];

    method rename_stub (vi: varinfo): unit =
      vi.vname <- stubwrapper_prefix ^ vi.vname;

    method add_new_fundecs (): unit =
      (* For each function that is called, but does not have a fundec, create a
         fundec for that function with the same type signature *)
      List.iter self#gen_stub funcs_with_no_fundecs;

    method process (f: file): (fundec list * varinfo list) =
      self#add_new_fundecs();
      stub_funcs, nonstub_funcs;
  end

(* The idea here is to re-add prototypes for all the original kernel functions *)
class pass_add_orig_kernfn_protos
  (f: file)
  = object (self)
    val mutable orig_kernfn_protos : varinfo list = [];
    
    (* acc: start off with a blank list.  This list eventually contains
       all the globals.
       cur_glob:  The current global we're looking at.
       returns:  the new accumulated list of all globals.
    *)
    method private add_kern (acc: global list) (cur_glob: global): global list =
      match cur_glob with
        | GVarDecl(vi, _) -> 
            if contains_prefix vi.vname stubwrapper_prefix then
              begin
                let kernel_fn_name = strip_prefix vi.vname stubwrapper_prefix in
                let kernel_fn_type = vi.vtype in
                let new_glob_vi = makeGlobalVar kernel_fn_name kernel_fn_type in
                new_glob_vi.vattr <- vi.vattr;
                new_glob_vi.vstorage <- vi.vstorage;
                new_glob_vi.vinline <- vi.vinline;
                orig_kernfn_protos <- orig_kernfn_protos @ [new_glob_vi];
                
                (* Bit of a hack -- force the stubwrapper storage to static *)
                (* Should probably do in pass_add_stubs but then we lose
                   the original storage specifier *)
                vi.vstorage <- Static;
                vi.vinline <- true;
                let new_glob = GVarDecl(new_glob_vi, locUnknown) in
                acc @ [new_glob; cur_glob];
              end
            else
              acc @ [cur_glob]
        | _ ->
            acc @ [cur_glob]
        
    method process (): varinfo list =
      (* Add the prototypes *)
      let new_globals = List.fold_left self#add_kern [] f.globals in
      (* Replace the old list of globals with the new list *)
      f.globals <- new_globals;
      orig_kernfn_protos;
  end

(** Find module parameters
 * Used for making module parameters symbolic.
 * TODO: This does not support module_param_named
 *)
class pass_find_parameters
  (f: file) = object (self)

    (** Need to look at the .sym.c preprocessed code
        to see precisely how the kernel handles module parameters.
        The kernel macros create some extra symbols with the same
        name as the parameter but with some prefix
    *)
    method private is_corresponding_var2
      (generated_vis: varinfo list)
      (final_list: varinfo list)
      (candidate: varinfo) : varinfo list =
      let predicate (generated: varinfo) : bool =
        (* True if the stripped name matches the candidate *)
        (strip_prefix generated.vname "__param_") = candidate.vname
      in
      if List.exists predicate generated_vis then
        final_list @ [candidate]
      else
        final_list

    method private is_var2 (acc: varinfo list) (cur_glob: global) : varinfo list =
      match cur_glob with
        | GVar(vi, _, _) -> acc @ [vi]
        | _ -> acc

    method find_vars2 (generated_vis: varinfo list) : varinfo list =
      let candidate_vi_list = List.fold_left self#is_var2 [] f.globals in
      let is_corresponding_var2_composed = self#is_corresponding_var2 generated_vis in
      let final_list = List.fold_left is_corresponding_var2_composed [] candidate_vi_list in
      final_list
        
    method private process_var1 (acc: varinfo list) (cur_glob: global): varinfo list =
      match cur_glob with
        | GVar(vi, _, _) -> 
            if
              contains_prefix vi.vname "__param_" = true &&
              contains_prefix vi.vname "__param_str_" = false
            then
              (* Accumulate only if the global is a module parameter *)
              acc @ [vi]
            else
              acc
        | _ ->
            acc
              
    method process (): varinfo list =
      let generated_vis = List.fold_left self#process_var1 [] f.globals in
      let param_vis = self#find_vars2 generated_vis in
      param_vis
  end

class pass_static_annots
  (f: file)
  (new_prepostfn: (fundec * fundec) list)
  (new_annots: (string, string) Hashtbl.t)
  = object (self)
    inherit nopCilVisitor

    (* Store the mapping "funcname"::"annotation" *)
    val mutable annots: (string, string) Hashtbl.t = new_annots;

    (* Declaration looks like this in preprocessed code:

       static kobj_method_t ed_pci_methods[4]  = {
           {& device_probe_desc, (int ( * )(void))(& ed_pci_probe)},
           {& device_attach_desc, (int ( * )(void))(& ed_pci_attach)},
           {& device_detach_desc, (int ( * )(void))(& ed_detach)},
           {(kobjop_desc_t )0, (int ( * )(void))0}
       };
    *)

    method private strip_extra_crap (e: exp) : string option =
      match e with
        | Const(_) -> None;
        | SizeOf(_) -> fatal["Check SizeOf case in strip_extra_crap"];
        | SizeOfE(e) -> fatal["Check SizeOfE case in strip_extra_crap"];
        | SizeOfStr(e) -> fatal["Check SizeOfStr case in strip_extra_crap"];
        | AlignOf(_) -> fatal["Check AlignOf case in strip_extra_crap"];
        | AlignOfE(_) -> fatal["Check AlignOfE case in strip_extra_crap"];
        | AddrOf((lh, offset)) (* Fall through *)
        | StartOf((lh, offset)) (* Fall through *)
        | Lval((lh, offset)) ->
            begin
              match lh with
                | Var(vi) -> Some(vi.vname)
                | Mem(nested_e) -> self#strip_extra_crap nested_e
            end
        | BinOp(_, nested_e1, nested_e2, _) ->
            fatal ["Check BinOp case in strip_extra_crap"; 
                   exp_tostring nested_e1;
                   exp_tostring nested_e2;
                  ];
        | UnOp(_, nested_e, _) (* Fall through *)
        | CastE(_, nested_e) ->
            self#strip_extra_crap nested_e
           
    method private method_process_list (oi_list: (offset * init) list) : unit =
      let process_one x =
        let off, init = x in
        match init with
          | SingleInit(e) ->
              (* Printf.fprintf stderr "%d: Processing %s %s...\n"
                 !count (offset_tostring off) (init_tostring init);
              *)
              self#strip_extra_crap e
          | CompoundInit(_, _) ->
              fatal ["Not processing in method_process_list"];
      in
      if List.length oi_list != 2 then
        ()
      else
        let kernel_fn_opt = process_one (List.nth oi_list 0) in
        let driver_fn_opt = process_one (List.nth oi_list 1) in
        match kernel_fn_opt, driver_fn_opt with
          | Some(kernel_fn), Some(driver_fn) ->
              Hashtbl.add annots driver_fn kernel_fn
          | _, _ -> ();
        
    method private method_process_ii (ii: initinfo) : unit =
      match ii.init with
        | Some(SingleInit(_)) ->
            fatal ["Not processing initinfo despite apparent relevance.  Verify."];
        | Some(CompoundInit(t, ol)) ->
            let process_one_struct x =
              let off, compound_init = x in
              (*Printf.fprintf stderr "Processing %s %s...\n"
                (offset_tostring off) (init_tostring compound_init);*)
              match compound_init with
                | SingleInit(_) -> ();
                | CompoundInit(t, oi_list) ->
                    self#method_process_list oi_list
            in
            List.iter process_one_struct ol
        | None ->
            Printf.fprintf stderr "No initialization found.\n"
    
    method private method_check_global (vi: varinfo) (ii: initinfo) : unit =
      match vi.vtype with
        | TArray(t, e_option, _) ->
            begin
              match t with
                | TNamed(ti, _) ->
                    (* Printf.fprintf stderr "Processing in check_global %s\n" ti.tname; *)
                    if ti.tname = "kobj_method_t" then
                      self#method_process_ii ii
                | _ -> ()
            end
        | _ -> ()

    method vglob (glob: global): global list visitAction =
      match glob with
        | GVar(vi, ii, _) ->
            self#method_check_global vi ii;
            DoChildren
        | _ ->
            DoChildren

    method linux_interrupt_handlers (fdec: fundec) : unit =
      (* Check for interrupt handling functions. 
       * Function has return type irqreturn_t *)
      let linux_is_interrupt_handler (fdec: fundec) : bool = 
        match fdec.svar.vtype with
          | TFun(rettyp,_,_,_) -> 
              (match rettyp with
                 | TNamed(tinfo,_) -> 
                     if (String.compare tinfo.tname "irqreturn_t") = 0 
                     then true else false;
                 | _ -> false;
              );
          | _ -> false;
      in
        
      (* IRQ handler *)
      if linux_is_interrupt_handler fdec then
        Hashtbl.add annots fdec.svar.vname "interrupt_handler";


    method vfunc (fdec: fundec): fundec visitAction =
      self#linux_interrupt_handlers fdec;
      DoChildren

    method freebsd_interrupt_handlers (ins: instr) : unit =
      let freebsd_extract_interrupt_handler (param: exp) : string option =
        if isZero param then
          None
        else
          Some(exp_to_function param)
      in
      let freebsd_bus_setup_intr (fn_exp: exp) (params: exp list) : string option =
        let fn_call = exp_tostring fn_exp in
        let fn_call_stripped = strip_prefix_try fn_call stubwrapper_prefix in
        if fn_call_stripped = "bus_setup_intr" then
          let h1 = freebsd_extract_interrupt_handler (List.nth params 3) in
          let h2 = freebsd_extract_interrupt_handler (List.nth params 4) in
          match h1, h2 with
            | Some(h1), Some(h2) ->
                fatal ["More than one interrupt handler registered in bus_setup_intr"]
            | Some(h1), None -> Some(h1)
            | None, Some(h2) -> Some(h2)
            | None, None ->
                fatal ["No interrupt handler specified in bus_setup_intr"]
        else
          None
      in
      match ins with
        | Call(_, fn_exp, params, _) ->
            let interrupt_handler = freebsd_bus_setup_intr fn_exp params in
            begin
              match interrupt_handler with
                | Some(int_handler) ->
                    (* Note that if the binding already exists, the old one
                       is simply covered up -- it's still present but not accessible
                       unless we did a remove() on it.  That should be fine *)
                    Hashtbl.add annots int_handler "interrupt_handler";
                    (* Printf.fprintf stderr "Found interrupt handler: %s\n" int_handler; *)
                | None -> ()
            end
        | _ -> ()

    method vinst (ins: instr): instr list visitAction =
      self#freebsd_interrupt_handlers ins;
      DoChildren
            
    method process (): unit =
      Cil.visitCilFileSameGlobals (self :> cilVisitor) f;

      (*
      let print_mapping (key: string) (value: string) = 
        Printf.fprintf stderr "Annotation: %s -> %s\n" key value
      in
      Hashtbl.iter print_mapping annots
      *)
  end

class pass_add_register
  (fdecs: fundecs_group)
  (* Store the new prepostfn functions *)
  (prepostfn_funcs: (fundec * fundec) list)
  (* Store the original non prepostfn functions *)
  (non_prepostfn_funcs: fundec list)
  (* Store the stub fundecs here *)
  (* Note that the corresponding kernel functions are not available,
     instead we look them up dynamically *)
  (stub_funcs: fundec list)
  (* Module parameters to make symbolic in register_all *)
  (parameters_vi: varinfo list)
  (* Original kernel function prototypes *)
  (orig_kernfn_protos: varinfo list)
  (* Annotations -- used to communicate interrupt handler name to S2E *)
  (annots: (string, string) Hashtbl.t)
  = object (self)

    (* Registration functions *)
    val mutable fdecs_register: fundecs_register = {
      register_one = emptyFunction "gen_register_one";
      register_all = emptyFunction "gen_register_all";
      (* register_all_cleanup = emptyFunction "gen_register_all_cleanup"; *)
      
      register_protos = [];
    }

    method private mk_register_one_fundec (fdec: fundec): unit =
      fdec.svar.vstorage <- Extern;
      let fn_arg1 = ("wrapper_fn", voidPtrType, []) in
      let fn_arg2 = ("driver_fn", voidPtrType, []) in
      let fn_arg3 = ("wrapper_fn_str", charConstPtrType, []) in
      let fn_arg4 = ("driver_fn_str", charConstPtrType, []) in
      let fn_args = Some([fn_arg1; fn_arg2; fn_arg3; fn_arg4]) in
      let fn_typ = TFun(voidType, fn_args, false, []) in
      begin
        try
          setFunctionTypeMakeFormals fdec fn_typ;
        with _ ->
          fatal ["Function that's a problem 3: "; fdec.svar.vname];
      end;

    method private mk_register_all_prepostfn_stmts (fdec: fundec): stmt list =
      let register_one_exp = expify_lval (lvalify_varinfo fdecs_register.register_one.svar) in
      let create_register_one marsh_fn orig_fn =
        let arg1 = mkAddrOf (lvalify_varinfo marsh_fn.svar) in
        let arg2 = mkAddrOf (lvalify_varinfo orig_fn.svar) in
        let arg3 = Const(CStr(marsh_fn.svar.vname)) in
        let arg4 = Const(CStr(orig_fn.svar.vname)) in
        let args = [arg1; arg2; arg3; arg4] in
        let call_stmt = mkCallStmt None register_one_exp args in
        call_stmt
      in
      let return_first (f_pair : (fundec * fundec)) : fundec =
        let (first, _) = f_pair in
        first
      in
      let prefn = List.map return_first prepostfn_funcs in
      let prefn_list = List.map2 create_register_one prefn non_prepostfn_funcs in
      prefn_list

    method private mk_register_all_stub_stmts (fdec: fundec): stmt list =
      let register_one_exp = expify_varinfo fdecs_register.register_one.svar in
      let create_register_one orig_fn =
        (* Generate register_one *)
        let predicate_kernfn (vi: varinfo) : bool =
          let stripped = strip_prefix_try orig_fn.svar.vname stubwrapper_prefix in
          vi.vname = stripped
        in
        let kern_fn_vi =
          try
            List.find predicate_kernfn orig_kernfn_protos
          with Not_found ->
            fatal ["Failed to find kernel prototype: \n"; orig_fn.svar.vname];
        in
        let arg1 = mkAddrOf (lvalify_varinfo kern_fn_vi) in
        let arg2 = mkAddrOf (lvalify_varinfo orig_fn.svar) in
        let arg3 = Const(CStr(kern_fn_vi.vname)) in
        let arg4 = Const(CStr(strip_prefix_try orig_fn.svar.vname stubwrapper_prefix)) in
        let args = [arg1; arg2; arg3; arg4] in
        let call_stmt_register_one = mkCallStmt None register_one_exp args in
        [call_stmt_register_one]
      in
      List.flatten (List.map create_register_one stub_funcs)

    (* Module parameters *)
    method private mk_symbolic_stmts (fdec: fundec) : stmt list =
      let gen_mk_symbolic (acc: stmt list) (vi: varinfo) : stmt list =
        match vi.vtype with
          | TInt(_, _)
          | TFloat(_, _)
          | TArray(_, _, _)
          | TEnum(_, _) ->
              let arg1 = mkAddrOrStartOf (lvalify_varinfo vi) in
              let arg2 = SizeOf(vi.vtype) in
              let arg3 = mkString vi.vname in
              let ending_fn_call_expr = expify_fundec fdecs.s2e_make_symbolic in
              let call_stmt = mkCallStmt None ending_fn_call_expr [arg1; arg2; arg3] in
              acc @ [call_stmt];
          | _ -> acc
      in
      List.fold_left gen_mk_symbolic [] parameters_vi

    (* Module parameters:  make an if statement so we only make
       these symbolic if the developers wants them to be.
    *)
    method private mk_symbolic_params (fdec: fundec): stmt list =
      let cond_left = expify_varinfo fdecs.g_sym_params in
      let cond_right = zero in
      let if_condition = BinOp(Ne, cond_left, cond_right, TInt(IBool, [])) in
      let body = self#mk_symbolic_stmts fdec in
      [mkIfStmt if_condition body []]

    method private mk_register_irq_stmts (fdec: fundec): stmt list =
      (* s2e_trackperf_fn(int line, const char *fn, int flags) *)
      let mk_one (fn_name: string) (annot: string) (acc: stmt list) : stmt list =
        if annot = "interrupt_handler" then
          let arg1_vi = makeLocalVar fdec ~insert:false "__LINE__" intType in
          let arg1 = expify_varinfo arg1_vi in
          let arg2 = mkString fn_name in
          let arg3 = integer 1000 in (* See TRACKPERF_IRQ_HANDLER SymDriveSearcher.h *)
          let fn_expr = expify_fundec fdecs.s2e_trackperf_fn in
          acc @ [mkCallStmt None fn_expr [arg1; arg2; arg3]]
        else
          acc
      in
      Hashtbl.fold mk_one annots [];
        
    (* Assumes register_one_fundec is all set up *)
    method private mk_register_all_fundec (fdec: fundec): unit =
      fdec.svar.vstorage <- Static;
      let fn_args = Some([]) in
      let fn_typ = TFun(voidType, fn_args, false, []) in
      begin
        try
          setFunctionTypeMakeFormals fdec fn_typ;
        with _ ->
          fatal ["Function that's a problem 4: "; fdec.svar.vname];
      end;
      let register_one_prepostfn_stmts = self#mk_register_all_prepostfn_stmts fdec in
      let register_one_stub_stmts = self#mk_register_all_stub_stmts fdec in
      let make_symbolic_stmts = self#mk_symbolic_params fdec in
      let register_irq_stmts = self#mk_register_irq_stmts fdec in
      let all_stmts = register_one_prepostfn_stmts
        @ register_one_stub_stmts
        @ make_symbolic_stmts
        @ register_irq_stmts
      in
      fdec.sbody <- mkBlock (all_stmts)

    (* Assumes register_one_fundec is all set up *)
(*
    method private mk_register_all_cleanup_fundec (fdec: fundec): unit =
      let fn_args = Some([]) in
      let fn_typ = TFun(voidType, fn_args, false, []) in
      fdec.svar.vstorage <- Extern;
      setFunctionTypeMakeFormals fdec fn_typ;
*)

    method get_protos (): unit =
      let register_one_varinfo = fdecs_register.register_one.svar in
      let register_one_proto = GVarDecl(register_one_varinfo, locUnknown) in

      let register_all_varinfo = fdecs_register.register_all.svar in
      let register_all_proto = GVarDecl(register_all_varinfo, locUnknown) in

(*
      let register_all_cleanup_varinfo = fdecs_register.register_all_cleanup.svar in
      let register_all_cleanup_proto = GVarDecl(register_all_cleanup_varinfo, locUnknown) in
*)

      let list = [register_one_proto;
                  register_all_proto;
                  (* register_all_cleanup_proto *)
                 ] in
      fdecs_register.register_protos <- list;

    method process (): fundecs_register =
      self#mk_register_one_fundec fdecs_register.register_one;
      self#mk_register_all_fundec fdecs_register.register_all;
      (* self#mk_register_all_cleanup_fundec fdecs_register.register_all_cleanup; *)

      self#get_protos();

      fdecs_register;
  end

(* Remove extra attributes *)
class pass_remove_fn_attrs
  (global_list: global list)
  (remove_all: bool)
  = object (self)
    method private process_glob (g: global): unit =
      match g with
        | GFun(fdec, loc) ->
            fdec.svar.vattr <- [];
            if remove_all then
              fdec.svar.vtype <- strip_typ_attribs fdec.svar.vtype;
        | GVarDecl(vi, loc) ->
            vi.vattr <- [];
            if remove_all then
              vi.vtype <- strip_typ_attribs vi.vtype;
        | _ -> ();
            
    method process (): unit =
      List.iter self#process_glob global_list;
  end

class pass_add_test_framework_fns
  (non_prepostfn_funcs: fundec list)
  (funcs_with_no_fundecs: varinfo list) = object (self)
    inherit nopCilVisitor

    method type_to_check (acc: fundec list) (fvi: varinfo): fundec list =
      let tfname =
        if contains_prefix fvi.vname prefn_prefix || 
          contains_prefix fvi.vname postfn_prefix
        then
          fatal ["Only examining normal functions and stubwrappers"]
        else if contains_prefix fvi.vname stubwrapper_prefix then
          (strip_prefix fvi.vname stubwrapper_prefix) ^ "_MJRcheck"
        else
          fvi.vname ^ "_MJRcheck"
      in
      let new_fdec = emptyFunction tfname in
      let tf_typ = get_tf_fn_typ fvi.vtype in
      begin
        try
          setFunctionTypeMakeFormals new_fdec tf_typ;
        with _ ->
          fatal ["Function that's a problem 5: "; new_fdec.svar.vname];
      end;
      ensure_formals_have_names new_fdec;
      new_fdec.svar.vstorage <- Static;
      acc @ [new_fdec];

    method process (): global list =
      let non_prepost_map (fdec : fundec) : varinfo =
        fdec.svar
      in
      let non_prepostfn_vi = List.map non_prepost_map non_prepostfn_funcs in
      let new_checks1 = List.fold_left self#type_to_check [] non_prepostfn_vi in
      let new_checks2 = List.fold_left self#type_to_check [] funcs_with_no_fundecs in
      let new_checks_full_list = new_checks1 @ new_checks2 in
      let fdec_to_global fdec = GFun(fdec, locUnknown) in
      let new_check_globals = List.map fdec_to_global new_checks_full_list in
      let processor = new pass_remove_fn_attrs new_check_globals true in
      processor#process();
      let gfun_to_gvardecl (g : global) : global =
        match g with
          | GFun(fdec, l) -> GVarDecl(fdec.svar, l)
          | _ -> fatal ["Expected GFun in pass_add_test_framework_fns"]
      in
      let new_check_vardecls = List.map gfun_to_gvardecl new_check_globals in
      new_check_vardecls
  end

(* Used to prioritize loops that finish and deal with function return
   statements.
   
   We add the prefn_driverFn and postfn_driverFn calls here but not
   the bodies.

   This pass is quite complex because dealing with the return
   statement is non-trivial.  We need to insert a call before the
   return statement.  CIL already does a transform to ensure that all
   functions have a single return.  Ensuring that the label for the
   return statement matches the call to the test framework is not
   easy.
*)
class pass_add_prioritization
  (f: file)
  (fdecs: fundecs_group)
  (prepostfn_funcs: (fundec * fundec) list)
  (* A list of functions that don't have standard return value semantics, i.e.
     a number in -4095 to -1 is still successful *)
  (nonstandard_retval_fns: string list)
  (drivermacro_fns: string list)
  = object (self)
    inherit nopCilVisitor

    val mutable cur_phase: int = 1;
    val mutable cur_fundec: fundec = emptyFunction "";

    (* Return statement / labels / goto *)
    val mutable return_stmt_opt: stmt option = None;
    val mutable pre_return_stmts: stmt list = [];

    (* Loop statement / labels / goto *)
    (* Store a mapping from loop stmt to s2e_loop_before stmt *)
    val mutable loop_label_tbl: (stmt, stmt) Hashtbl.t = Hashtbl.create 5

    (* Each loop before/body/after call gets its own id *)
    val mutable current_id: int = 0;

    (* Track the loop depth as we execute -- this is the number
       of nested loops in the current function *)
    val mutable loop_depth: int = 0;

    (* Generate a call to the driver macro if specified in the list *)
    method private gen_drivermacro (f: fundec) : stmt list =
      let do_this_macro = List.mem f.svar.vname drivermacro_fns in
      if do_this_macro then
        let macro_name = drivermacro_prefix ^ f.svar.vname in
        let macro_fn = emptyFunction macro_name in
        [mkCallStmt None (expify_fundec macro_fn) []]
      else
        []

    method private can_instrument_fn (fn_name: string) (retval_typ: typ) : bool =
      supported_ret_type retval_typ && self#is_nonstandard_retval cur_fundec.svar.vname = false

    method private is_nonstandard_retval (fn_name: string) : bool =
      List.mem fn_name nonstandard_retval_fns

    method stmts_analyze (fdec: fundec) (stmts : stmt list) : stmt list =
      let return_stmt (acc: stmt list) (s: stmt) : stmt list =
        let result = 
          match s.skind with
            | Return(exp_option, loc) -> [s]
            | If(_, b1, b2, _) ->
                self#block_analyze fdec b1 @ self#block_analyze fdec b2;
            | _ ->  []
        in
        acc @ result
      in
      List.fold_left return_stmt [] stmts
        
    method block_analyze (fdec: fundec) (b: block) : stmt list =
      self#stmts_analyze fdec b.bstmts
        
    method return_stmt_find (fdec: fundec) : stmt =
      let return_stmts = self#block_analyze fdec fdec.sbody in
      if List.length return_stmts <> 1 then
        fatal["Expected one return statement in: "; fdec.svar.vname]
      else
        List.nth return_stmts 0
        
    (* Generate a call to the prepostfn *)
    (* Prefn never has retval_valid, or retval parameters.
       Postfn always does. *)
    method gen_prepostfn (fdec: fundec) (prefn_bool: bool) (retval_exp_option : exp option) : stmt list =
      let call_fn_args = lvalify_varinfos fdec.sformals in
      let call_fn_args_addrof = List.map mkAddrOrStartOf call_fn_args in
      let retval_arg_postfn, retval_lval_opt =
        let (retval_typ,_,_,_) = splitFunctionType fdec.svar.vtype in
        match retval_typ with
          | TVoid(_) ->
              [integer 0], None;
          | _ ->
              (* Need to pass it the actual value being returned *)
              let return_stmt = self#return_stmt_find fdec in
              let retval_lval =
                match return_stmt.skind with
                  | Return(Some(Lval(lv)), _) -> lv
                  | _ -> fatal ["Unexpected return statement in "; fdec.svar.vname]
              in
              [mkAddrOrStartOf retval_lval], Some(retval_lval)
      in
      let retval_valid_postfn = 
        match retval_exp_option with
          | Some(retval_exp) ->
              [integer 1]
          | None ->
              [integer 0]
      in
      let call_prefn_args_all = call_fn_args_addrof in
      let call_postfn_args_all = retval_valid_postfn @ retval_arg_postfn @ call_fn_args_addrof in
      let call_kernel_fn_vi = get_local_variable fdec intType "_call_kernel_fn_" in
      let call_kernel_fn_lv = Some(lvalify_varinfo call_kernel_fn_vi) in
      match lookup_prepostfn prepostfn_funcs fdec.svar.vname with
        | Some(prefn, postfn) -> 
            begin
              match prefn_bool with
                | true -> 
                    let call_prefn = expify_varinfo prefn.svar in
                    let call_prestmt = mkCallStmt call_kernel_fn_lv call_prefn call_prefn_args_all in
                    let call_postfn = expify_varinfo postfn.svar in
                    let call_post_stmt = mkCallStmt None call_postfn call_postfn_args_all in
                    let if_condition_lhs = expify_varinfo call_kernel_fn_vi in
                    let if_condition_rhs = zero in
                    let if_condition = BinOp(Ne, if_condition_lhs, if_condition_rhs, TInt(IBool, [])) in
                    let if_body_return =
                      match retval_lval_opt with
                        | Some(retval_lval) ->
                            let if_body_return_exp = Some(expify_lval retval_lval) in
                            mkStmt (Return(if_body_return_exp, locUnknown))
                        | None ->
                            mkStmt (Return(None, locUnknown))
                    in
                    let if_body = [call_post_stmt; if_body_return] in
                    let if_stmt = mkIfStmt if_condition if_body [] in
                    [call_prestmt; if_stmt]
                | false ->
                    let call_fn = expify_varinfo postfn.svar in
                    [mkCallStmt call_kernel_fn_lv call_fn call_postfn_args_all]
            end
        | None ->
            []

    method gen_prefn (fdec: fundec) : stmt list =
      self#gen_prepostfn fdec true None

    method gen_postfn (fdec: fundec) (retval_exp_option: exp option) : stmt list =
      self#gen_prepostfn fdec false retval_exp_option

    method gen_loop_call (f: fundec) : stmt =
      let arg1_vi = makeLocalVar f ~insert:false "__LINE__" intType in
      let arg1 = expify_varinfo arg1_vi in
      let arg2 = integer current_id in
      let loop_before_call_expr = expify_fundec f in
      mkCallStmt None loop_before_call_expr [arg1; arg2]
          
    method gen_loop_before_call () : stmt =
      self#gen_loop_call fdecs.s2e_loop_before

    method gen_loop_body_call () : stmt =
      self#gen_loop_call fdecs.s2e_loop_body

    method gen_loop_after_call () : stmt =
      self#gen_loop_call fdecs.s2e_loop_after

    method gen_uprintk_call (arg : string) : stmt list =
      if !g_extra_uprintk then
        let uprintk_call_expr = expify_fundec fdecs.uprintk in
        [mkCallStmt None uprintk_call_expr [mkString arg]]
      else
        []

    method move_label (stmt_dest: stmt) (stmt_src: stmt) : unit =
      (*Printf.fprintf stderr "dest: %s, src: %s, num labels: %d\n"
        (stmt_tostring stmt_dest)
        (stmt_tostring stmt_src)
        (List.length stmt_src.labels);
      *)
      stmt_dest.labels <- stmt_src.labels @ stmt_dest.labels;
      stmt_src.labels <- [];

    method process_goto
      (loc: location)
      (stmt_src: stmt)
      (stmts_dest: stmt list)
      : stmt list =
      let list_length = List.length stmts_dest in
      if list_length <> 1 && list_length <> 2 then
        fatal ["Unexpected list length in process_goto"];
      let after_stmts =
        if loop_depth > 0 then
          let array = Array.make loop_depth () in
          let list = Array.to_list array in
          List.map self#gen_loop_after_call list
        else
          []
      in
      let stmt_first = List.nth stmts_dest 0 in
      self#move_label stmt_first stmt_src;
      let new_goto = Goto(ref stmt_first, loc) in
      let new_goto_stmt = mkStmt new_goto in
      after_stmts @ [new_goto_stmt]

    (* This code adds the loop prioritization before/during/after the loop *)
    (* It now includes a feature that ensures the statement in the loop body is placed
       before any continue/break stmt.  This trick improves our code coverage slightly
       since it otherwise looks like we missed executing that stmt (when it was merely
       the break/continue that interfered) *)
    (* Note that some loops also have labels before them, e.g:
       mylabel:
       while (1) {
       ...
       }
       In this case we need to be sure to put the loop_before call after the label
       but before the loop!
    *)
    method process_loop (loop_stmt : stmt) (loop_body_stmts : stmt list) : (stmt list * stmt list) =
      let loop_before_stmt = self#gen_loop_before_call () in
      let loop_body_stmt = self#gen_loop_body_call () in
      let loop_after_stmt = self#gen_loop_after_call () in
      let loop_last_index = List.length loop_body_stmts - 1 in
      let last_bstmt_opt =
        if loop_last_index >= 0 then
          Some(List.nth loop_body_stmts loop_last_index)
        else
          None
      in
      let new_stmts = 
        begin
          match last_bstmt_opt with
            | Some(last_bstmt) ->
                begin
                  match last_bstmt.skind with
                    | Continue(_) | Break(_) ->
                        let remove_last list =
                          match (List.rev list) with
                            | h::t -> List.rev t
                            | [] -> []
                        in
                        let all_but_last = remove_last loop_body_stmts in
                        all_but_last @ [loop_body_stmt] @ [last_bstmt]
                    | _ ->
                        loop_body_stmts @ [loop_body_stmt]
                end
            | _ -> loop_body_stmts @ [loop_body_stmt]
        end
      in

      (* Track the labels associated with this loop *)
      (* We're going to move these labels to the before_loop
         statement *)
      if List.length loop_stmt.labels > 0 then
        begin
          Printf.fprintf stderr "Found loop starting with a label: %s    %s\n"
            (stmt_tostring loop_stmt) (stmt_tostring loop_before_stmt);
          Hashtbl.add loop_label_tbl loop_stmt loop_before_stmt;

          (* Move labels from the loop statement to the loop_before stmt *)
          self#move_label loop_before_stmt loop_stmt
        end;
      new_stmts, ([loop_before_stmt] @ [loop_stmt] @ [loop_after_stmt])
        
    method process_stmt_phases (s: stmt) : stmt list =
      match s.skind, return_stmt_opt, pre_return_stmts with
        | Goto(stmt_ref, loc), Some(return_stmt), _::_ -> (* _::_ means "Match non-empty list" *)
            (* This is a goto pointing to the return statement *)
            (* physical equality, not structural equality, with == *)
            if cur_phase = 1 then
              [s]
            else if cur_phase = 2 && !stmt_ref == return_stmt then
              self#process_goto loc return_stmt pre_return_stmts
            else if cur_phase == 2 then
              let loop_before_stmt_opt =
                try 
                  Some(Hashtbl.find loop_label_tbl !stmt_ref);
                with Not_found ->
                  None
              in
              match loop_before_stmt_opt with
                | Some(loop_before_stmt) ->
                    (*Printf.fprintf stderr "Phase 2: label before loop in %s\n" cur_fundec.svar.vname;*)
                    self#process_goto loc !stmt_ref [loop_before_stmt]
                | None -> [s]
            else
              fatal ["Unknown case in process_stmt_phases"];
        | Return(exp_option, loc), _, _ ->
            if cur_phase = 1 then
              let retval_exp_option =
                match exp_option with
                  | Some(e) ->
                      if self#can_instrument_fn cur_fundec.svar.vname (typeOf e)
                      then
                        exp_option
                      else
                        None
                  | _ -> None
              in
              let new_drivermacro_stmts = self#gen_drivermacro cur_fundec in
              let new_ending_stmts = self#gen_postfn cur_fundec retval_exp_option in
              (* Store this for next phase where we fixup gotos *)
              (* List may have 0, 1, or 2 elts *)
              let list_size = List.length new_ending_stmts in
              if list_size < 0 || list_size > 2 then
                fatal ["Unexpected list size in process_stmt_phases"];
              pre_return_stmts <- new_ending_stmts @ new_drivermacro_stmts;
              return_stmt_opt <- Some(s);
              new_ending_stmts @ new_drivermacro_stmts @ [s];
            else
              [s]
        | Loop(b, ln, cont_opt, br_opt), _, _ ->
            b.bstmts <- self#process_block b.bstmts;
            let result =
              if cur_phase = 1 then
                begin
                  (* Track the loop *)
                  loop_depth <- loop_depth + 1;
                  current_id <- current_id + 1;
                  let new_loop_stmts, new_stmts = self#process_loop s b.bstmts in
                  b.bstmts <- new_loop_stmts;
                  new_stmts;
                end
              else
                [s]
            in
            loop_depth <- loop_depth - 1;
            result;
        | If (_, b1, b2, _), _, _ ->
            b1.bstmts <- self#process_block b1.bstmts;
            b2.bstmts <- self#process_block b2.bstmts;
            [s]
        | Switch(_, b, _, _), _, _ ->
            b.bstmts <- self#process_block b.bstmts;
            [s]
        | Block(b), _, _ ->
            b.bstmts <- self#process_block b.bstmts;
            [s]
        | _, _, _ -> [s]

    method process_stmt (s: stmt) : stmt list = 
      if cur_phase = 1 || cur_phase = 2 then
        self#process_stmt_phases s
      else
        fatal ["Phase must be 1 or 2"]

    method process_block (stmts: stmt list) : stmt list =
      let new_stmts_listlist = List.map self#process_stmt stmts in
      List.flatten new_stmts_listlist

    (* Process a global *)
    method vglob (g: global): global list visitAction =
      match g with
        | GFun(fdec, _) ->
            cur_fundec <- fdec;
            return_stmt_opt <- None;
            pre_return_stmts <- [];
            loop_label_tbl <- Hashtbl.create 5;

            cur_phase <- 1;
            let new_stmts1 = self#process_block fdec.sbody.bstmts in

            cur_phase <- 2;
            let new_stmts2 = self#process_block new_stmts1 in

            let new_prefn_stmts = self#gen_prefn fdec in
            fdec.sbody.bstmts <- new_prefn_stmts @ new_stmts2;
            DoChildren
        | _ -> DoChildren

    method process (): unit =
      Cil.visitCilFileSameGlobals (self :> cilVisitor) f;
  end

(*
class pass_verify_in_driver
  (f: file)
  (fdecs: fundecs_group)  (* Need this for verify_in_driver *)
  = object (self)
    inherit nopCilVisitor

    val mutable cur_fn : string = "";

    method gen_verify_in_driver (loc: location) : stmt =
      let verify_in_driver_expr = expify_fundec fdecs.verify_in_driver in
      let arg1 = mkString cur_fn in
      let arg2 = integer loc.line in
      mkCallStmt None verify_in_driver_expr [arg1; arg2]
        
    method process_stmt (s: stmt) : stmt list = 
      let loc = get_stmtLoc s.skind in
      let call_stmt = 
        if cur_fn = "init_module" || cur_fn = "cleanup_module" then
          mkEmptyStmt()
        else
          self#gen_verify_in_driver loc
      in
      begin
        match s.skind with
          | Loop(b, _, _, _) ->
              b.bstmts <- self#process_block b.bstmts;
          | If (_, b1, b2, _) ->
              b1.bstmts <- self#process_block b1.bstmts; 
              b2.bstmts <- self#process_block b2.bstmts; 
          | Switch(_, b, _, _) ->
              b.bstmts <- self#process_block b.bstmts;
          | Block(b) ->
              b.bstmts <- self#process_block b.bstmts;
          | _ -> ()
      end;
      [call_stmt; s]

    method process_block (stmts: stmt list) : stmt list =
      let new_stmts_listlist = List.map self#process_stmt stmts in
      List.flatten new_stmts_listlist

    method vglob (g: global): global list visitAction =
      match g with
        | GFun(fdec, _) ->
            cur_fn <- fdec.svar.vname;
            let new_stmts = self#process_block fdec.sbody.bstmts in
            fdec.sbody.bstmts <- new_stmts;
            DoChildren
        | _ -> DoChildren
            
    method process (): unit =
      Cil.visitCilFileSameGlobals (self :> cilVisitor) f;
  end
*)

(*---------------------------------------------------------------------------*)
(** Main function to call the root-annotation extractor *)
let do_extract_rootannot (f: file): data_chunk =

  Printf.fprintf stderr "pass_remove_calls\n";
  let obj_remove_calls = new pass_remove_calls f in
  obj_remove_calls#process ();

  Printf.fprintf stderr "pass_remove_fn_bodies\n";
  let obj_remove_fn_bodies = new pass_remove_fn_bodies f in
  obj_remove_fn_bodies#process ();

  Printf.fprintf stderr "pass_find_funcs_with_no_fundecs\n";
  let obj_funcs_with_no_fundecs = new pass_find_funcs_with_no_fundecs f in
  let funcs_with_no_fundecs = obj_funcs_with_no_fundecs#process() in
  
  Printf.fprintf stderr "pass_fdec_management\n";
  let obj_fdec_management = new pass_fdec_management in
  let fdecs_group = obj_fdec_management#process () in

  Printf.fprintf stderr "pass_builtin_expect\n";
  let obj_builtin_expect = new pass_builtin_expect f fdecs_group in
  obj_builtin_expect#process ();

  Printf.fprintf stderr "pass_find_irritating_fns\n";
  let obj_find_irritating_fns = new pass_find_irritating_fns f in
  let (nonstub_fns,
       nonstandard_retval_fns,
       interesting_driver_fns,
       drivermacro_fns)
      = obj_find_irritating_fns#process()
  in

  Printf.fprintf stderr "pass_add_s2e_enter_block\n";
(*
  let obj_add_s2e_enter_block = new pass_add_s2e_enter_block fdecs_group in
  obj_add_s2e_enter_block#process f;
*)

  Printf.fprintf stderr "pass_add_prepostfn\n";
  let obj_add_prepostfn = new pass_add_prepostfn nonstub_fns drivermacro_fns in
  let prepostfn_funcs, non_prepostfn_funcs = obj_add_prepostfn#process f in
  
  Printf.fprintf stderr "pass_add_stubs\n";
  let obj_add_stubs = new pass_add_stubs funcs_with_no_fundecs nonstub_fns in
  let stub_funcs, nonstub_funcs = obj_add_stubs#process f in

  Printf.fprintf stderr "pass_add_orig_kernfn_protos\n";
  let obj_add_orig_kernfn_protos = new pass_add_orig_kernfn_protos f in
  let orig_kernfn_protos = obj_add_orig_kernfn_protos#process () in

  Printf.fprintf stderr "pass_find_parameters\n";
  let obj_find_parameters = new pass_find_parameters f in
  let parameters_vi = obj_find_parameters#process () in

  (* Moved from end *)
  Printf.fprintf stderr "pass_static_annots\n";
  let annots = Hashtbl.create 5 in
  let obj_static_annots = new pass_static_annots
    f
    prepostfn_funcs
    annots
  in
  obj_static_annots#process ();

  Printf.fprintf stderr "pass_add_register\n";
  let obj_add_register = new pass_add_register
    fdecs_group
    prepostfn_funcs
    non_prepostfn_funcs
    stub_funcs
    parameters_vi
    orig_kernfn_protos
    annots
  in
  let fdecs_register = obj_add_register#process () in

  Printf.fprintf stderr "pass_remove_fn_attrs\n";
  let obj_remove_attrs = new pass_remove_fn_attrs f.globals false in
  obj_remove_attrs#process ();

  Printf.fprintf stderr "pass_add_test_framework_fns";
  let check_vardecls =
    if !g_gen_test_framework_stubs then
      let obj_add_test_framework_fns = new pass_add_test_framework_fns
        non_prepostfn_funcs
        funcs_with_no_fundecs
      in
      obj_add_test_framework_fns#process ()
    else
      []
  in

  Printf.fprintf stderr "pass_add_prioritization\n";
  let obj_add_prioritization = new pass_add_prioritization
    f
    fdecs_group
    prepostfn_funcs
    nonstandard_retval_fns
    drivermacro_fns
  in
  obj_add_prioritization#process();

  Printf.fprintf stderr "pass_verify_in_driver\n";
(*
  if !g_verify_in_driver then
    let obj_verify_in_driver = new pass_verify_in_driver f fdecs_group in
    obj_verify_in_driver#process()
  else
    ();
*)

  let retval: data_chunk = {
    file = f;
    annots = annots;
    prepostfn_funcs = prepostfn_funcs;
    non_prepostfn_funcs = non_prepostfn_funcs;
    stub_funcs = stub_funcs;
    nonstub_funcs = nonstub_funcs;
    funcs_with_no_fundecs = funcs_with_no_fundecs;
    check_vardecls = check_vardecls;
    nonstandard_retval_fns = nonstandard_retval_fns;
    interesting_driver_fns = interesting_driver_fns;
    drivermacro_fns = drivermacro_fns;

    fdecs = fdecs_group;
    fdecs_register = fdecs_register;
  } in
  retval;
  
  

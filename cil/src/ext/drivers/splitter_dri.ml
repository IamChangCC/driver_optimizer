(*===========================================================================*)
(*
 * Vinod Ganapathy <vg@cs.wisc.edu>, September 28, 2006.
 * Matt Renzelmann <mjr@cs.wisc.edu> May, 2011
 *)
(*===========================================================================*)

open Cil
open Str
open Scanf
open Utils_dri
open Root_annot_dri
open Globals_dri
open Types_dri

class common_routines = object (self)
  method stub_get_retval
    (stub_fn: fundec):
    (typ * varinfo option * lval option) =
    (* Create a new variable of the appropriate type and add it to f's slocals *)
    let (ret_var_type,_,_,_) = splitFunctionType stub_fn.svar.vtype in
    let (ret_var_vi, ret_var_lval) =
      match ret_var_type with
        | TVoid(_) -> (None, None);
        | _ ->
            let vi = get_local_variable stub_fn ret_var_type "_retval_" in
            let lv = lvalify_varinfo vi in
            (Some (vi), Some (lv));
    in
    (ret_var_type, ret_var_vi, ret_var_lval);

  method postfn_get_retval_valid
    (orig_fundec: fundec)
    (prepostfn_fundec: fundec):
    (typ * varinfo option * lval option) =
    (* retval_valid passed as parameter *)
    let formals = prepostfn_fundec.sformals in
    let first_formal =
      try
        List.nth formals 0
      with Failure _ ->
        fatal ["Failure: expected parameter retval_valid in "; orig_fundec.svar.vname;
               " "; prepostfn_fundec.svar.vname]
    in
    if first_formal.vname <> "retval_valid" then
      fatal ["Failure: Expected retval_valid parameter in "; orig_fundec.svar.vname; " "; prepostfn_fundec.svar.vname]
    else
      let lv = lvalify_varinfo first_formal in
      first_formal.vtype, Some(first_formal), Some(lv)

  method postfn_get_retval
    (orig_fundec: fundec)
    (prepostfn_fundec: fundec):
    (typ * varinfo option * lval option) =
    (* Retval passed as parameter *)
    let formals = prepostfn_fundec.sformals in
    let second_formal =
      try
        List.nth formals 1
      with Failure _ ->
        fatal ["Failure: expected parameter retval in "; orig_fundec.svar.vname;
               " "; prepostfn_fundec.svar.vname]
    in
    if second_formal.vname <> "retval" then
      fatal ["Failure: Expected retval parameter in "; orig_fundec.svar.vname; " "; prepostfn_fundec.svar.vname]
    else
      let lv = lvalify_varinfo second_formal in
      second_formal.vtype, Some(second_formal), Some(lv)
end

class kallsyms_lookup_call (g: data_chunk) = object (self)
  inherit common_routines

  val mutable lookup_MJRcheck_fundec: fundec = dummyFunDec;
  val mutable globals_to_add: global list = [];

  method get_globals () : global list = globals_to_add

  (* Given a symbol name, finds its original declaration *)
  method private get_kallsym_given_name
    (symbol_name: string)
    : varinfo =
    let predicate (g: global) : bool =
      match g with
        | GVarDecl(vi, _) -> vi.vname = symbol_name
        | _ -> false
    in
    let glob = List.find predicate g.file.globals in
    let glob_vi = 
      match glob with
        | GVarDecl(vi, _) -> vi
        | _ -> fatal ["Expected GVar"];
    in
    glob_vi

  method private gen_kallsyms_lookup_call
    (retval: lval) (* fn_ptr *)
    (symbol_name: string) (* Name of function *)
    : stmt =
    (* Stmt to do the kallsyms lookup *)
    let kernel_vi = self#get_kallsym_given_name symbol_name in
    let instr_retval_set = Set (retval, mkAddrOrStartOf (lvalify_varinfo kernel_vi), locUnknown) in
    mkStmtOneInstr instr_retval_set

  method private gen_lookup_MJRcheck_call
    (retval: lval)
    (prepostfn: string)
    (cur_fn: fundec)
    (lookup_fundec: fundec)
    : stmt =
    (* Stmt to do the lookup_MJRcheck call *)
    let func_expr = expify_fundec lookup_fundec in
    let arg1_mode = mkString prepostfn in
    let arg2_symbol = mkAddrOrStartOf (lvalify_varinfo cur_fn.svar) in
    let args = [arg1_mode; arg2_symbol] in
    let retval_option = Some(retval) in
    mkCallStmt retval_option func_expr args

  method private gen_if_stmt
    (retval_vi: varinfo)
    (stmt: stmt)
    : stmt =
    (* If the static function pointer variable equals 0, then do the lookup *)
    (* Otherwise, just skip the lookup and call the function pointer directly *)
    let cond_left = expify_varinfo retval_vi in
    let cond_right = zero in
    let if_condition = BinOp(Eq, cond_left, cond_right, TInt(IBool, [])) in
    mkIfStmt if_condition [stmt] []

  method private gen_lookup_MJRcheck
    (cur_fn: fundec)
    (prepostfn: string)
    (retval_vi: varinfo) (* varinfo into which we store the looked up symbol *)
    : stmt =
    let retval_lv = lvalify_varinfo retval_vi in
    let stmt = self#gen_lookup_MJRcheck_call retval_lv prepostfn cur_fn lookup_MJRcheck_fundec in
    self#gen_if_stmt retval_vi stmt

  method private get_global_variable (name: string) (t: typ): (global * varinfo) =
    let predicate (g: global) : bool =
      match g with
        | GVar(vi, _, _) -> vi.vname = name
        | _ -> fatal ["Expected a list of GVar globals"];
    in
    try 
      let glob = List.find predicate globals_to_add in
      let vi = 
        match glob with
          | GVar(vi, _, _) -> vi
          | _ -> fatal ["Expected GVar"];
      in
      glob, vi
    with Not_found ->
      let vi = makeGlobalVar name t in
      let iinfo : initinfo = { init = Some(makeZeroInit t) } in
      let glob = GVar(vi, iinfo, locUnknown) in
      globals_to_add <- globals_to_add @ [glob];
      glob, vi

  (* First, create a copy of the interface function of interest and add _MJRcheck
     Then, pass that fundec into this function, which will transform it into a proper
     _MJRcheck prototype
     
     tf_fn_typ is the type of the original function
  *)
  method private check_fn_setup_ptr (name: string) (tf_fn_typ: typ): varinfo =
    let tf_typ = get_tf_fn_typ tf_fn_typ in
    let tf_ptr_typ = TPtr(tf_typ, []) in
    let new_global, new_vi = self#get_global_variable name tf_ptr_typ in
    new_vi.vstorage <- Static;
    new_vi;

  method run_MJRcheck_fn
    (* Function associated with the one in which we're adding check_fn *)
    (cur_fn: fundec)

    (* Function in which we're adding check_fn *)
    (prepostfn_stub: fundec)

    (* "0" = kernel fn stub,
       "1" = pre/post fn for driver function,
       something else = special case
    *)
    (prepostfn: string)
    
    (* Arguments to the check_fn *)
    (call_fn_args: exp list)

    (* 0 for precondition 1 for postcondition *)
    (condition: int)
    : stmt list =

    (* Do the kallsyms lookup: *)
    let retval_name = checkfn_prefix ^ cur_fn.svar.vname in
    let retval_vi = self#check_fn_setup_ptr retval_name cur_fn.svar.vtype in
    let if_stmt_list =
      if condition = 0 then
        [self#gen_lookup_MJRcheck cur_fn prepostfn retval_vi]
      else
        []
    in

    (* Stmt to do the call operation on the looked-up function pointer *)
    let call_kernel_fn_vi = get_local_variable prepostfn_stub intType "_call_kernel_fn_" in
    let call_kernel_fn_lv = Some(lvalify_varinfo call_kernel_fn_vi) in
    let call_fn = expify_varinfo retval_vi in
    let call_func_stmt = mkCallStmt call_kernel_fn_lv call_fn call_fn_args in

    (* Save all the statements *)
    if_stmt_list @ [call_func_stmt];

  method private gen_fn_ptr_call (cur_fn: fundec) (retval_vi : varinfo) : stmt =
    let (_, _, call_retval_lv_option) = self#stub_get_retval cur_fn in
    let call_fn = expify_varinfo retval_vi in
    let call_fn_args = expify_varinfos cur_fn.sformals in
    mkCallStmt call_retval_lv_option call_fn call_fn_args

  method gen_stub_retval_modification (cur_fn: fundec): stmt list =
    let orig_name = strip_prefix_try cur_fn.svar.vname stubwrapper_prefix in
    let do_this_stub = List.mem orig_name g.nonstandard_retval_fns in
    let true_block =
      if do_this_stub = false then
        let (ret_var_typ, ret_var_varinfo_opt, ret_var_lval_opt) = self#stub_get_retval cur_fn in
        let mk_make_symbolic_int (ret_var_lval: lval): stmt =
          let func_expr = expify_fundec g.fdecs.make_symbolic_int in
          let arg1 = mkString orig_name in
          let arg2 = expify_varinfo (makeLocalVar cur_fn ~insert:false "__LINE__" intType) in
          let arg3 = mkAddrOrStartOf ret_var_lval in
          let arg4 = sizeOf ret_var_typ in
          let args = [arg1; arg2; arg3; arg4] in
          mkCallStmt None func_expr args
        in
        let mk_make_symbolic_ptr (ret_var_lval: lval): stmt =
          let func_expr = expify_fundec g.fdecs.make_symbolic_ptr in
          let arg1 = mkString orig_name in
          let arg2 = expify_varinfo (makeLocalVar cur_fn ~insert:false "__LINE__" intType) in
          let arg3 = mkCast (mkAddrOrStartOf ret_var_lval) (TPtr(voidPtrType, [])) in
          let args = [arg1; arg2; arg3] in
          mkCallStmt None func_expr args
        in
        match (ret_var_typ, ret_var_lval_opt) with
          | TInt (kind, _), Some(ret_var_lval) ->
              [mk_make_symbolic_int ret_var_lval]
          | TPtr (t, _), Some(ret_var_lval) ->
              [mk_make_symbolic_ptr ret_var_lval]
          | _ -> []
      else
        begin
          Printf.fprintf stderr "Kernel function non-standard retval: %s\n" orig_name;
          []
        end
    in
    true_block

  method gen_run_kernel_fn
    (cur_fn: fundec) (* Stub, e.g. pci_register_driver *)
    : stmt list =
    (* Retval refers to the function pointer into which we store the looked up symbol *)
    let retval_symbol_name_stripped = strip_prefix cur_fn.svar.vname stubwrapper_prefix in

    let kernel_function = self#get_kallsym_given_name retval_symbol_name_stripped in
    let stmt_fn_ptr_call = self#gen_fn_ptr_call cur_fn kernel_function in
    let stmts_retval_modification = self#gen_stub_retval_modification cur_fn in

    [stmt_fn_ptr_call] @ stmts_retval_modification

  method create
    (local_lookup_MJRcheck_fundec: fundec)
    : unit =
    lookup_MJRcheck_fundec <- local_lookup_MJRcheck_fundec;
end

class pass_main (g: data_chunk) = object (self)
    inherit common_routines

    val kallsyms: kallsyms_lookup_call = new kallsyms_lookup_call g

    (* These two functions enable/disable performance tracking
       at the start/end of each prefn/postfn
       and at the start/end of each block of code in a stubwrapper.
    *)
    method gen_enable_trackperf (cur_fn: fundec) (mode: int) : stmt =
      let func_expr = expify_fundec g.fdecs.s2e_enable_trackperf in
      let arg1 = expify_varinfo (makeLocalVar cur_fn ~insert:false "__LINE__" intType) in
      let arg2 = integer mode in (* CONTINUE/CONTINUE_HIGH *)
      let args = [arg1; arg2] in
      mkCallStmt None func_expr args

    method gen_disable_trackperf (cur_fn: fundec) (mode: int) : stmt =
      let func_expr = expify_fundec g.fdecs.s2e_disable_trackperf in
      let arg1 = expify_varinfo (makeLocalVar cur_fn ~insert:false "__LINE__" intType) in
      let arg2 = integer mode in (* PAUSE/PAUSE_HIGH *)
      let args = [arg1; arg2] in
      mkCallStmt None func_expr args

    (* Produces the gen_enter_driver or gen_exit_driver statement *)
    method gen_call_enter_exit_driver
      (orig_fundec: fundec) (* Original function, e.g. rtl8139_init_one *)
      (wrapper_fundec: fundec) (* Wrapper, e.g. prepost fn or stub fundec *)
      (call_fdec: fundec) (* enter_driver or exit_driver fundec *)
      (wrapper_type: int) (* 0 = kernel fn wrapper, 1 = prepostfn *)
      (prefn_postfn_stub: int) (* 0 = prefn, 1 = postfn, 2 = stub *)
      : stmt list =
      let gen_retval_arg (deref: bool) (retval_vi_opt: varinfo option) : exp =
        match retval_vi_opt with
          | Some(retval_vi) ->
              let retval_vi_deref, retval_vi_typ =
                if deref then
                  let retval_vi_deref_lv = mkMem (expify_varinfo retval_vi) NoOffset in
                  let retval_vi_deref = expify_lval retval_vi_deref_lv in
                  retval_vi_deref, typeOf retval_vi_deref
                else
                  expify_varinfo retval_vi, retval_vi.vtype
              in
              if supported_ret_type retval_vi_typ then
                mkCast ~e:retval_vi_deref ~newt:ulongType
              else
                zero
          | None ->
              zero
      in
      let is_interesting_fn (f: fundec) : (string * string) = 
        let predicate ((s, t): (string * string)) : bool =
          (f.svar.vname = s) || ((strip_prefix_try f.svar.vname stubwrapper_prefix) = s)
        in
        try
          List.find predicate g.interesting_driver_fns
        with Not_found ->
          "", ""
      in
      let is_nonstandard_retval (fn_name: string) : bool =
        let nonstandard_retval = List.mem fn_name g.nonstandard_retval_fns in
        let freebsd_interrupt_handler =
          let annot =
            try 
              Hashtbl.find g.annots fn_name
            with Not_found ->
              "default"
          in
          if annot = "interrupt_handler" then
            true
          else
            false
        in
        nonstandard_retval or freebsd_interrupt_handler
      in
      let retval, retval_valid =
        if is_nonstandard_retval orig_fundec.svar.vname then
          (* nonstandard retval *)
          integer 0, integer 0
        else
          if prefn_postfn_stub = 1 then
            (* postfn *)
            let _, retval_vi_opt, _ = self#postfn_get_retval orig_fundec wrapper_fundec in
            let retval = gen_retval_arg true retval_vi_opt in
            (* postfn *)
            let _, _, retval_valid_opt = self#postfn_get_retval_valid orig_fundec wrapper_fundec in
            let retval_valid =
              match retval_valid_opt with
                | Some(rv) -> expify_lval rv
                | None -> fatal["Expected retval_valid parameter"]
            in
            retval, retval_valid
          else if prefn_postfn_stub = 2 then
            (* stub *)
            (* Retval is a local variable *)
            let _, retval_vi_opt, _ = self#stub_get_retval wrapper_fundec in
            let retval = gen_retval_arg false retval_vi_opt in
            let retval_valid = integer 1 in
            retval, retval_valid
          else
            integer 0, integer 0 (* prefn *)
      in
      let call_emptyfunc = expify_fundec call_fdec in

      let (interesting_fn, interesting_req) = is_interesting_fn orig_fundec in

      (* Ugly hack since we don't have the original function available: *)
      let call_arg1 = mkString (strip_prefix_try orig_fundec.svar.vname stubwrapper_prefix) in
      let call_arg2_vi = makeLocalVar orig_fundec ~insert:false "__LINE__" intType in
      let call_arg2 = expify_varinfo call_arg2_vi in
      let call_arg3_wrapper_type = kinteger IInt wrapper_type in
      let call_arg4_retval_valid = retval_valid in
      let call_arg5_retval_opt = retval in
      let call_arg6_interesting = mkString interesting_req in
      let call_arg7_gen_register_all = mkAddrOf (lvalify_varinfo g.fdecs_register.register_all.svar) in
      let call_emptyargslist = [call_arg1;
                                call_arg2;
                                call_arg3_wrapper_type;
                                call_arg4_retval_valid;
                                call_arg5_retval_opt;
                                call_arg6_interesting;
                                call_arg7_gen_register_all;
                               ]
      in
      let call_retval = 
        if prefn_postfn_stub = 1 then
          let vi = get_local_variable wrapper_fundec (TInt(IInt, [])) "exit_retval" in
          Some(lvalify_varinfo vi)
        else
          None
      in
      [mkCallStmt call_retval call_emptyfunc call_emptyargslist];

    method gen_init_zero (f: fundec): stmt list =
      (* Initialize the return value if applicable *)
      let retval_vi_option = self#stub_get_retval f in
      match retval_vi_option with
        | (_, None, _) -> []
        | (t, Some(retval_vi_some), _) ->
            if supported_ret_type t then
              let instr_retval_set_0 = Set (lvalify_varinfo retval_vi_some,
                                            zero, locUnknown) in
              [mkStmtOneInstr instr_retval_set_0]
            else
              []

    method gen_stub_retstmt 
      (orig_fdec: fundec)
      (prefn_postfn_fdec: fundec)
      (prefn_postfn_stub: int) (* 0 = prefn, 1 = postfn, 2 = stub *)
      : stmt =
      let ret_var_lv =
        if prefn_postfn_stub = 0 then
          (* prefn does not return a value *)
          None
        else if prefn_postfn_stub = 1 then
          (* postfn may return a value *)
          let _, _, lv = self#postfn_get_retval orig_fdec prefn_postfn_fdec in
          match lv with
            | None -> None
            | Some(lv_some) ->
                let deref = expify_lval lv_some in
                Some(mkMem deref NoOffset)
        else
          (* Stub *)
          (* Create a new variable of the appropriate type and add it to f's slocals *)
          let _, _, lv = self#stub_get_retval orig_fdec in
          lv
      in
      match ret_var_lv with
        | None ->
            mkStmt (Return(None, locUnknown));
        | Some(ret_var_lv_some) ->
            let ret_stmtkind = Return(Some (expify_lval ret_var_lv_some), locUnknown) in
            let ret_stmt = mkStmt ret_stmtkind in
            ret_stmt

    method gen_prepost_retstmt (f: fundec) : stmt list =
      let call_kernel_fn_vi = get_local_variable f intType "_call_kernel_fn_" in
      let ret_stmtkind = Return(Some (expify_varinfo call_kernel_fn_vi), locUnknown) in
      [mkStmt ret_stmtkind]
              
    (** add_prepostfn_function: Add a marshaling wrapper for "interface"
     * functions:  marshaling wrapper for functions with no fundec
     * defined in the kernel.
     *)
    method gen_prefn_body
      (f: fundec)
      (prepostfn_func: fundec)
      : stmt list =
      (* Now we're going to call the actual implementation of the driver
         or kernel interface function.
         argslist = the arguments to the interface function we're about to call.
      *)
      let lval_argslist = lvalify_varinfos f.sformals in
      let stmt_disable_trackperf = [self#gen_disable_trackperf prepostfn_func 11] in (* PAUSE_PP *)
      let stmts_enter_driver = self#gen_call_enter_exit_driver f prepostfn_func g.fdecs.enter_driver 1 0 in
      let prepostfn = "1" in (* Wrapper *)
      let stmts_preconditions = self#gen_preconditions f prepostfn_func prepostfn lval_argslist in
      let stmt_enable_trackperf = [self#gen_enable_trackperf prepostfn_func 12] in (* CONTINUE_PP *)
      let stmt_return = self#gen_prepost_retstmt f in
      let stmts_new_wrapped =
        stmt_disable_trackperf
        @ stmts_enter_driver
        @ stmts_preconditions
        @ stmt_enable_trackperf
        @ stmt_return
      in
      stmts_new_wrapped;

    method gen_postfn_body
      (f: fundec)
      (prepostfn_func: fundec)
      : stmt list =
      (* Now we're going to call the actual implementation of the driver
         or kernel interface function.
         argslist = the arguments to the interface function we're about to call.
      *)
      let lval_argslist = lvalify_varinfos f.sformals in
      let prepostfn = "1" in (* Wrapper *)
      let stmt_disable_trackperf = [self#gen_disable_trackperf prepostfn_func 11] in
      let stmts_postconditions = self#gen_postconditions f prepostfn_func prepostfn lval_argslist in
      let stmts_exit_driver = self#gen_call_enter_exit_driver f prepostfn_func g.fdecs.exit_driver 1 1 in
      let stmt_enable_trackperf = [self#gen_enable_trackperf prepostfn_func 12] in
      let stmt_return = self#gen_prepost_retstmt f in
      let stmts_new_wrapped =
        stmt_disable_trackperf
        @ stmts_postconditions
        @ stmts_exit_driver
        @ stmt_enable_trackperf
        @ stmt_return
      in
      stmts_new_wrapped;

    method gen_prepostfn_funcs (): global list =
      let gen_prepostfn (non_prepostfn: fundec) (prepostfn: (fundec * fundec)): global list =
        let prefn, postfn = prepostfn in
        prefn.sbody.bstmts <- self#gen_prefn_body non_prepostfn prefn;
        postfn.sbody.bstmts <- self#gen_postfn_body non_prepostfn postfn;
        [GFun(prefn, locUnknown); GFun(postfn, locUnknown)]
      in
      let result = List.map2 gen_prepostfn g.non_prepostfn_funcs g.prepostfn_funcs in
      List.flatten result

    method stub_mk_convert_fn
      (convert_lval: lval)
      (f: fundec)
      (str1: string)
      (str2: string)
      : stmt =
      (* Cast to void ** since we need to the function pointer itself
         in the convert_fn function *)
      let addr_of_exp = mkAddrOrStartOf convert_lval in
      let convert_arg1 = mkCast ~e:addr_of_exp ~newt:(TPtr(voidPtrType,[])) in
      let convert_arg2 = mkString f.svar.vname in
      let convert_arg3 = mkString str1 in
      let convert_arg4 = mkString str2 in
      let convert_args = [convert_arg1; convert_arg2; convert_arg3; convert_arg4] in
      let convert_exp = expify_fundec g.fdecs.convert_fn in
      mkCallStmt None convert_exp convert_args;

    method stub_convert_arg
      (f: fundec)
      (param_name: string)
      (cur_lv: lval)
      (cur_t: typ)
      (ci_option: compinfo option)
      (fi_option: fieldinfo option)
      (depth: int)
      : stmt list =
      let new_depth = depth + 1 in
      if depth < 7 then
        match cur_t with
          | TPtr(TFun(fn_type, args, vargs, attrs) as cur_typ, _) ->
              self#stub_convert_arg f param_name cur_lv cur_typ ci_option fi_option new_depth
          | TPtr(TNamed(ti, _) as t, _) ->
              begin
                match ti.ttype with
                  | TFun(fn_type, args, vargs, attrs) ->
                      self#stub_convert_arg f param_name cur_lv ti.ttype ci_option fi_option new_depth
                  | _ -> 
                      let new_lv = mkMem (expify_lval cur_lv) NoOffset in
                      self#stub_convert_arg f param_name new_lv t ci_option fi_option new_depth
              end
          | TPtr(TPtr(_, _), _) ->
              (* Ugly data structures -- hopefully this will not miss anything *)
              (* Example struct device -> struct attribute_group.  See struct device def *)
              (* TODO *)
              [];
          | TPtr(t, _) ->
              let new_exp = expify_lval cur_lv in
              let new_lv = mkMem new_exp NoOffset in
              let stmts = self#stub_convert_arg f param_name new_lv t ci_option fi_option new_depth in
              let condition = BinOp(Ne, new_exp, zero, TInt(IBool, [])) in
              if List.length stmts > 0 then
                [mkIfStmt condition stmts []]
              else
                []
          | TNamed(ti, _) ->
              (* Typedefs are free -- no depth increase*)
              let new_typ = ti.ttype in
              self#stub_convert_arg f param_name cur_lv new_typ ci_option fi_option depth
          | TFun(_, _, _, _) ->
              let ci_str =
                match ci_option with
                  | Some(ci) -> ci.cname
                  | None -> strip_prefix f.svar.vname stubwrapper_prefix
              in
              let fi_str =
                match fi_option with
                  | Some(fi) -> fi.fname
                  | None -> param_name
              in
              (* This if statement is a hack to avoid converting
                 __anonstruct____ function pointers.
                 We had some trouble with 8139too/e1000 and other drivers
                 with the kernel CONFIG_COMPAT_NET_DEV_OPS option, which
                 creates an anonymous structure in net_device.
                 Function pointer stored in anonymous structures will
                 never have an associated test framework function
                 anyway, so this hack seems reasonable 
              *)
              if contains_prefix ci_str "__anonstruct____" then
                []
              else
                [self#stub_mk_convert_fn cur_lv f ci_str fi_str]
          | TComp(ci, _) ->
              let handle_fi fi =
                let new_lv = add_field_to_lval cur_lv fi in
                let new_typ = fi.ftype in
                self#stub_convert_arg f param_name new_lv new_typ (Some(ci)) (Some(fi)) new_depth
              in
              List.flatten (List.map handle_fi ci.cfields)
          | _ -> []
      else
        []

    method stub_fn_ptr_conversion (f: fundec): stmt list =
      if !g_test_framework = true then
        let execute vi =
          let cur_lv = lvalify_varinfo vi in
          let cur_t = vi.vtype in
          self#stub_convert_arg f vi.vname cur_lv cur_t None None 0
        in
        let list = List.map execute f.sformals in
        List.flatten list;
      else
        []

    (** Stub generation.  Used for invoking the kernel *)
    method gen_stub_body (f: fundec) : stmt list =
      (* Make sure all formal parameters have names.  Sometimes,
         the function prototype is provided without variable names--
         in these cases, we make some up. *)
      ensure_formals_have_names f;
      let lval_argslist = lvalify_varinfos f.sformals in

      let stmt_disable_trackperf = [self#gen_disable_trackperf f 21] in (* PAUSE_STUB *)
      let stmts_init = self#gen_init_zero f in
      let stmts_fn_ptr_conversion = self#stub_fn_ptr_conversion f in
      let prepostfn = "0" in (* Stub *)
      let stmts_preconditions = self#gen_preconditions f f prepostfn lval_argslist in
      let stmts_exit_driver = self#gen_call_enter_exit_driver f f g.fdecs.exit_driver 0 2 in
      let stmt_enable_trackperf = [self#gen_enable_trackperf f 22] in (* CONTINUE_STUB *)
      let stmts_main_stmts = kallsyms#gen_run_kernel_fn f in
      let stmts_postconditions = self#gen_postconditions f f prepostfn lval_argslist in
      let stmts_enter_driver = self#gen_call_enter_exit_driver f f g.fdecs.enter_driver 0 2 in
      let stmt_ret = self#gen_stub_retstmt f f 2 in

      let if_stmts =
        if !g_test_framework = true then
          let if_lhs = get_local_variable f intType "_call_kernel_fn_" in
          let if_condition = BinOp(Eq, expify_varinfo if_lhs, zero, TInt (IBool, [])) in
          let if_true_block =
            stmts_exit_driver
            @ stmts_main_stmts
            @ stmts_postconditions
            @ stmts_enter_driver
          in
          [mkIfStmt if_condition if_true_block []]
        else
          stmts_exit_driver
          @ stmts_main_stmts
          @ stmts_postconditions
          @ stmts_enter_driver
      in
      let ret_stmts =
        stmt_disable_trackperf
        @ stmts_init
        @ stmts_fn_ptr_conversion
        @ stmts_preconditions
        @ if_stmts
        @ stmt_enable_trackperf
        @ [stmt_ret]
      in
      ret_stmts;

    method gen_stubs (): global list =
      let gen_stub (stub_func: fundec): global =
        stub_func.sbody.bstmts <- self#gen_stub_body stub_func;
        GFun(stub_func, locUnknown)
      in
      List.map gen_stub g.stub_funcs

    (* These methods are for assertion checking.
       They simply call an exported function to do the checking--very simple.
       The alternative is to generate assertion checking code here, but this
       would be extremely tedious.
    *)
    method gen_preconditions
      (f: fundec)
      (prepostfn_stub: fundec)
      (prepostfn: string)
      (argslist: lval list): stmt list =
      if !g_test_framework = true then
        self#gen_tf_call f prepostfn_stub prepostfn 0 argslist
      else
        []

    method gen_postconditions
      (f: fundec)
      (prepostfn_stub: fundec)
      (prepostfn: string)
      (argslist: lval list): stmt list =
      if !g_test_framework = true then
        self#gen_tf_call f prepostfn_stub prepostfn 1 argslist
      else
        []

    method private gen_tf_call
      (f: fundec)
      (prepostfn_stub: fundec)
      (prepostfn_orig: string) (* "0" for stub, or "1" for wrapper *)
      (condition: int)
      (lval_argslist: lval list): stmt list =
      (* This mechanism deals with interrupt handlers 
         and other statically-defined TF checkers

         Supported TF checkers:
         - Formerly, we did init_module/cleanup_module
         but that became redundant because of other changes
         - Interrupt handler
         - FreeBSD kobj_method_t arrays -- includes
         DEVMETHOD and KOBJMETHOD initializers.
      *)
      let context_string =
        try
          (* This function is a special entry point *)
          Hashtbl.find g.annots f.svar.vname
        with Not_found ->
          "default"
      in
      let prepostfn = 
        if context_string <> "default" then
          context_string
        else
          prepostfn_orig
      in
      let ep_remaining_args =
        if prepostfn_stub == f then
          (* Testing for physical equality, not structural *)
          (* This is a stub *)
          let ep_addr_of_argslist = List.map mkAddrOrStartOf lval_argslist in
          ep_addr_of_argslist
        else
          (* This is a prefn/postfn *)
          expify_lvals lval_argslist
      in
      (* Ugly hack since we don't have access to the original varinfo associated
         with the actual kernel function any more.  We have only the renamed
         varinfo *)
      (*let stripped_name = strip_prefix_try f.svar.vname stubwrapper_prefix in*)
      (* Insert a static local variable to store the check routine *)
      let ep_arg1 = Const (CStr (f.svar.vname)) in
      let ep_arg2 = Cil.integer condition in
      let (ep_arg3_typ,_,_,_) = splitFunctionType f.svar.vtype in
      let ep_arg3 =
        match ep_arg3_typ with
            | TVoid(_) -> [];
            | _ ->
                if prepostfn_stub == f then
                  (* Testing for physical equality, not structural *)
                  (* This is a stub *)
                  let ep_arg3_vi = get_local_variable prepostfn_stub ep_arg3_typ "_retval_" in
                  let ep_arg3_lv = lvalify_varinfo ep_arg3_vi in
                  [mkAddrOrStartOf ep_arg3_lv]
                else
                  begin
                    if condition = 1 then
                      (* Postcondition *)
                      let _, _, lv_opt = self#postfn_get_retval f prepostfn_stub in
                      match lv_opt with
                        | Some (lv) ->
                            [expify_lval lv]
                        | None ->
                            fatal ["Expected return value in postfn."]
                    else if condition = 0 then
                      (* Precondition *)
                      [integer 0]
                    else
                      fatal ["Expected integer of 0 or 1"];
                  end
      in
      let ep_args = [ep_arg1; ep_arg2] @ ep_arg3 @ ep_remaining_args in
      kallsyms#run_MJRcheck_fn f prepostfn_stub prepostfn ep_args condition

    (** Top-level function to implement user pass 2 *)
    method top_level (): unit =
      Printf.fprintf stderr "kallsyms\n";
      kallsyms#create g.fdecs.lookup_MJRcheck;

      (* Fill in prepostfns and stubs *)
      Printf.fprintf stderr "gen_prepostfn_funcs\n";
      let new_prepostfns = self#gen_prepostfn_funcs () in
      Printf.fprintf stderr "gen_stubs\n";
      let new_stubs = self#gen_stubs () in

      (* Add prototypes for test framework functions _MJRcheck *)
      let new_fundecs_specials = [GFun (g.fdecs_register.register_all, locUnknown)] in
      
      g.file.globals <-
        g.fdecs_register.register_protos @
        g.fdecs.protos @
        g.file.globals @
        kallsyms#get_globals() @
        new_prepostfns @
        new_stubs @
        new_fundecs_specials @
        g.check_vardecls;
      
(* Gets rid of some unused temporaries that'd we created previously *)
(*Printf.fprintf stderr "Deleting unused temporaries ...\n";
  Rmtmps.removeUnusedTemps g.file;*)
(* This code seems to hang some times? E1000-specific bug I think --
   but have not verified *)
end

(*---------------------------------------------------------------------------*)
(** Main function to call the splitter. *)
let do_splitting (local_curr_file: file)
    : unit =
  begin
    (* do_extract_rootannot local_curr_file; *)
    let retval = do_extract_rootannot local_curr_file in
    let obj_main: pass_main = new pass_main retval in
    obj_main#top_level ();

    infomsg ["Produced instrumented driver"];
  end
    

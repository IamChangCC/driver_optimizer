(*===========================================================================*)
(*
 * Device driver analysis: This file contains general utilities.
 *
 * Vinod Ganapathy <vg@cs.wisc.edu>, August 1, 2006.
 *)
(*===========================================================================*)

open Cil
open Scanf
open Globals_dri

(* All warnings: to be flushed out at the end *)
let allwarnings = ref []

let rec strip_whitespace (s: string) : string =
  if String.length s = 0 then ""
  else if String.length s = 1 then
    if String.compare s "\n" = 0 ||
      String.compare s "\t" = 0 ||
      String.compare s " " = 0 ||
      String.compare s "\r" = 0
    then
      ""
    else
      s
  else
    let s1 = String.sub s 0 1 in
    let s2 = String.sub s 1 (String.length s - 1) in
    (strip_whitespace s1) ^ (strip_whitespace s2);;

(** Warning function *)
let warning(sl: string list) : unit = 
  begin
    let print_string(s: string) : unit = (Printf.fprintf stderr " %s%!" s) in
    (Printf.fprintf stderr "!!!!! NOTE !!!!! : %!");
    (ignore (List.map print_string sl));
    (Printf.fprintf stderr "\n%!");
  end

(** Terminal warning function *)
let fatal(sl: string list) : 'a = 
  begin
    let print_string(s: string) : unit = (Printf.fprintf stderr " %s%!" s) in
    (Printf.fprintf stderr "!!!!! FATAL WARNING !!!!! : %!");
    (ignore (List.map print_string sl));
    (Printf.fprintf stderr "\n%!");
    assert(false);
  end

(* Dereference a type *)
let rec deref_ptr_typ (t: typ) : typ = 
  begin
    (match t with
       | TPtr(deref_t, _) -> deref_t; (* Get the type we point to *)
       | TNamed(tinfo, _) -> (deref_ptr_typ tinfo.ttype); (* Get rid of typedef *)
       | _ -> (fatal ["Non-pointer type passed for dereference to deref_ptr_typ"]);
    );
  end

(** add_to_list: Add an element to an input list *)
let add_to_list (elem: 'a) (inlist: 'a list) : 'a list = 
 begin
    (List.append inlist [elem]);
  end

(** remove_repeats: Returns the input list without repeated elements *)
(* We use a hash value based comparison, because direct comparison may not
 * always be possible for all input data types *)
let remove_repeats (inlist : 'a list) : 'a list = 
  begin
    let retval : 'a list ref = ref [] in
    let values_seen : (int, bool) Hashtbl.t  = (Hashtbl.create 5) in 
    let is_in_retval_list (elem: 'a) : bool = 
      begin
        let is_present = ref false in
        (try 
           let hashelem = (Hashtbl.hash elem) in
           (ignore (Hashtbl.find values_seen hashelem));
           is_present := true;
         with Not_found -> ());
        !is_present;
        (* Old implementation, that could run out of memory for certain data types.
           for i = 0 to (List.length !retval) - 1 do
           if (List.nth !retval i) = elem then is_present := true;
           done;
        *)
      end in
    for i = 0 to (List.length inlist) - 1 do
      let ith = (List.nth inlist i) in
      if (is_in_retval_list ith) = false
      then begin
        let hashval = (Hashtbl.hash ith) in
        (Hashtbl.add values_seen hashval true);
        retval := (List.append !retval [ith]);
      end
    done;
    !retval;
  end

(** add_if adds a binding to 'key' in a hashtable if it's not bound already *)
let add_if (table: ('a,'b) Hashtbl.t) (key: 'a) (data: 'b): unit = 
  begin
    if (not (Hashtbl.mem table key)) then
      (Hashtbl.add table key data)
  end

(** add_if_binding adds a binding 'bind' to 'key' if the key::bind binding does
 * not already exist *)
let add_if_binding (table: ('a,'b) Hashtbl.t) (key: 'a) (data: 'b): unit =
  begin
    let bindings_for_key = (Hashtbl.find_all table key) in
    if (List.mem data bindings_for_key) = false then
      (Hashtbl.add table key data);
  end

(** List all the keys in a hashtable *)
let list_keys (table: ('a,'b) Hashtbl.t) : 'a list =
  begin
    let retval : 'a list ref = ref [] in
    let iterfun (key : 'a) (_ : 'b) : unit = 
      retval := key::!retval
    in
    begin
      (Hashtbl.iter iterfun table);
      !retval
    end
  end

(** List all the bindings in a hashtable *)
let list_bindings (table: ('a,'b) Hashtbl.t) : 'b list =
  begin
    let retval : 'b list ref = ref [] in
    let iterfun (_: 'a) (data : 'b) : unit = 
      retval := data::!retval
    in
    begin
      (Hashtbl.iter iterfun table);
      !retval
    end
  end

(** List all the key/binding pairs in a hashtable *)
let list_keybindings (table: ('a,'b) Hashtbl.t) : ('a * 'b) list =
  begin
    let retval : ('a * 'b) list ref = ref [] in
    let iterfun (key: 'a) (data : 'b) : unit = 
      retval := (key,data)::!retval
    in
    begin
      (Hashtbl.iter iterfun table);
      !retval
    end
  end

(** Two input hashtables with string keys: Are their key sets the same? 
 * Note that this will remove repeated keys, and check equality as sets *)
let are_keysets_same (tab1: (string,'a) Hashtbl.t)
    (tab2: (string,'b) Hashtbl.t) : bool =
  begin
    let sortfun s1 s2 = (String.compare s1 s2) in
    let keys_tab1 = (List.sort sortfun (remove_repeats (list_keys tab1))) in
    let keys_tab2 = (List.sort sortfun (remove_repeats (list_keys tab2))) in
    if (keys_tab1 = keys_tab2) then true else false;
  end

(** Copy hashtable in-place: 'dst' is emptied, and 'src'
 * is copied into 'dst' *)
let copy_htab_in_place (src: ('a,'b) Hashtbl.t) 
    (dst: ('a,'b) Hashtbl.t) : unit =
  begin
    let iterfun (key: 'a) (data : 'b) : unit = 
      (Hashtbl.add dst key data)
    in
    (Hashtbl.clear dst);
    (Hashtbl.iter iterfun src);
  end

(** Merge two input hashtbls into a single hashtbl. If multiple entries
 * exist in the input hashtables, they will be retained in the output
 * hashtable. If the same keys exist in both hashtables, a duplicate 
 * entry will be created. *)
let merge_hashtbls (tab1: ('a,'b) Hashtbl.t) 
    (tab2: ('a,'b) Hashtbl.t) : ('a,'b) Hashtbl.t =
  begin
    let retval = (Hashtbl.copy tab1) in
    let tab2_keybindings = (list_keybindings tab2) in
    for i = 0 to (List.length tab2_keybindings) - 1 do
      let (ithkey, ithdata) = (List.nth tab2_keybindings i) in
      (Hashtbl.add retval ithkey ithdata);
    done;
    (Hashtbl.copy retval);
  end

(** Integer to string *)
let itoa(i: int) : string = 
  begin
    (Int32.to_string (Int32.of_int i));
  end

(** Get the attributes associated with an input type *)
let get_attribs (t: typ) : attributes = 
  begin
    (match t with
       | TVoid(a) -> a;
       | TInt(_,a) -> a;
       | TFloat(_,a) -> a;
       | TPtr(_,a) -> a;
       | TArray(_,_,a) -> a;
       | TFun(_,_,_,a) -> a;
       | TNamed(_,a) -> a;
       | TComp(_,a) -> a;
       | TEnum(_,a) -> a;
       | TBuiltin_va_list(a) -> a;
    );
  end

(** Strip attributes from an input type *)
let strip_nonconst_attrs (attrs: attributes) =
  let remove_nonconst acc attr =
    match attr with
      | Attr(str, attr_params) ->
          if str = "const" or str = "volatile" then
            acc @ [attr]
          else
            acc @ []
  in
  List.fold_left remove_nonconst [] attrs

let rec strip_typ_attribs (t: typ) : typ = 
  match t with
    | TVoid(a) -> TVoid(strip_nonconst_attrs a);
    | TInt(k,a) -> TInt(k,strip_nonconst_attrs a);
    | TFloat(k,a) -> TFloat(k,strip_nonconst_attrs a);
    | TPtr(t',a) -> TPtr((strip_typ_attribs t'), strip_nonconst_attrs a);
    | TArray(t',e,a) -> TArray((strip_typ_attribs t'), e, strip_nonconst_attrs a);
    | TFun(t',l_opt,b,a) ->
        let process_elt (str, t, attr) =
          (str, strip_typ_attribs t, strip_nonconst_attrs a)
        in
        let new_list = match l_opt with
          | None ->
              None
          | Some(l) -> 
              Some(List.map process_elt l)
        in
        TFun((strip_typ_attribs t'),new_list,b,strip_nonconst_attrs a);
    | TNamed(tinfo,a) -> TNamed(tinfo,strip_nonconst_attrs a);
    | TComp(cinfo,a) -> TComp(cinfo,strip_nonconst_attrs a);
    | TEnum(einfo,a) -> TEnum(einfo,strip_nonconst_attrs a);
    | TBuiltin_va_list(a) -> TBuiltin_va_list(strip_nonconst_attrs a)
        
(* Are two input types the same? Hash the types to compare*)
let is_same_typ (t1: typ) (t2: typ) : bool = 
  let t1hash = (Hashtbl.hash t1) in
  let t2hash = (Hashtbl.hash t2) in
  if t1hash = t2hash then true else false

(** tcomp_compinfo: extract the compinfo from a TComp type. Also
 * try to do this if this is a named type *)
let rec tcomp_compinfo (t: typ) : compinfo option = 
  match t with
    | TComp(cinfo,_) -> Some(cinfo);
    | TNamed(tinfo,_) -> (tcomp_compinfo tinfo.ttype);
    | _ -> None;;

(** Is the input type a compound type? *)
let isCompoundType (t: typ) : bool = 
  match (tcomp_compinfo t) with
    | Some(_) -> true;
    | None -> false;;

(** get_fieldinfo: Get a fieldinfo from a compound type. Handle
 * exceptions here. *)
let get_fieldinfo (cinfo: compinfo) (s: string) : fieldinfo = 
  (* Initialize the return value with a dummy *)
  if (List.length cinfo.cfields) = 0 then
    (fatal ["Compound type has no fields available"]);
  let retval = ref (List.nth cinfo.cfields 0) in
  (try
     retval := (getCompField cinfo s);
   with Not_found -> ((fatal ["Could not fetch fieldinfo"])));
  !retval

(** Fieldinfo_name *)
let fieldinfo_name (f: fieldinfo) : string = 
  f.fname ^ "#" ^ f.fcomp.cname

(** Makevarname: Take the input string, and ensure that it only has characters
 * that C will accept as a character in a legal variable name. This is needed
 * because we use lval names as hints for variables, and lvalnames can contain
 * "*", " ", "(", ")", "-", and ">" *)
let makevarname (s: string) : string =
  begin
    let retval = ref "" in
    for i = 0 to (String.length s) - 1 do
      let ith = (String.get s i) in
      (match ith with
         | '>' -> ();
         | '-' -> ();
         | '*' -> ();
         | ' ' -> ();
         | '(' -> ();
         | ')' -> ();
         | '+' -> ();
         | '.' -> ();
         | ']' -> ();
         | '[' -> ();
         | _ -> retval := !retval ^ (String.make 1 ith);
      );
    done;
    !retval
  end
    
(* Prettyprint a type *)
let typ_tostring (t: typ) : string =
  (* (Pretty.sprint 50 (d_typsig() (typeSig t))); *)
  Pretty.sprint 100 (d_type() t)

(* Prettyprint an instruction *)
let instr_tostring (i: instr) : string = 
  Pretty.sprint 100 (d_instr() i)

(* Prettyprint a stmt *)
let stmt_tostring (s: stmt) : string =
  Pretty.sprint 100 (d_stmt() s)
    
(* Prettyprint a type, discarding attributes *)
let typ_tostring_noattr (t: typ) : string = 
  let t_attr_stripped = strip_typ_attribs t in
  typ_tostring t_attr_stripped

(* Prettyprint only the attributes of a type *)
let typ_tostring_attronly (t: typ) : string =
  let t_attronly = (get_attribs t) in
  let resultstr = ref (Printf.sprintf "%d" (List.length t_attronly)) in
  for i = 0 to (List.length t_attronly) - 1 do
    let ith = (match (List.nth t_attronly i) with
                 | Attr (name, _) -> name;
              ) in
    resultstr := !resultstr ^ " " ^ ith;
  done;
  !resultstr

(* Printing the name of an lval *)
let lval_tostring (lv: lval) : string = (Pretty.sprint 100 (d_lval() lv))

(* Printing the name of an exp *)
let exp_tostring (e: exp) : string = (Pretty.sprint 100 (d_exp() e))

(* Initializer helpers -- these could use some improvement *)
let rec offset_tostring (o: offset) : string =
  match o with
    | NoOffset -> "NoOffset"
    | Field(fi, o) -> "Field: ( " ^ offset_tostring o ^ " )"
    | Index(e, o) -> "Index: ( " ^ offset_tostring o ^ " )"
            
(* Initializer helper *)
let rec init_tostring (i: init) : string =
  match i with
    | SingleInit(e) -> exp_tostring e;
    | CompoundInit(_, oi_list) ->
        let mapfn (oi: offset * init) : string =
          let o, i = oi in
          offset_tostring o ^ init_tostring i
        in
        let string_list = List.map mapfn oi_list in
        let string_result = String.concat " " string_list in
        "CompoundInit: ( " ^ string_result ^ " )"

(* Given a fundec, convert its parameters list into a string *)
let get_params_str_fundec (f : fundec) : string =
  begin
    let retval = ref "" in
    let num_params = List.length f.sformals in
    if num_params >= 1 then
      let last_elt = List.nth f.sformals (List.length f.sformals - 1) in
      let store_vi vi =
        let param = Pretty.sprint 80 (Pretty.dprintf "%a %s" d_type vi.vtype vi.vname) in
        retval := !retval ^ param;
        if vi.vname <> last_elt.vname then
          retval := !retval ^ ",\n"
        else
          retval := !retval ^ ""
      in
      List.iter store_vi f.sformals;
    else
      retval := !retval ^ "void";
    !retval;
  end

let rec lval_to_function (lv: lval) : string =
  let lh, offset = lv in
  match lh, offset with
    | Var(vi), _ ->
        vi.vname
    | Mem(e), _ ->
        exp_to_function e
and exp_to_function (e: exp) : string =
  match e with
    | Lval(lv) 
    | AddrOf(lv)
    | StartOf(lv) ->
        lval_to_function lv
    | CastE(_, e2) ->
        exp_to_function e2
    | _ -> fatal ["Failed converting "; exp_tostring e; " to function"]

(* Create an expression from a fundec using the variable name *)
let expify_fundec (g: fundec) : exp = Lval(Var(g.svar),NoOffset)
  
(* Create an expression from an Lval *)
let expify_lval (l: lval) : exp = Lval(l)

(* Create an Lval from a varinfo *)
let lvalify_varinfo (v: varinfo) : lval = (Var(v),NoOffset)

(* Create an Lval from a varinfo and a field offset *)
let lvalify_varinfo_field (v: varinfo) (f: fieldinfo) : lval = 
  (Var(v), Field(f,NoOffset))

(* A wrapper for lvalify_varinfo/lvalify_varinfo_field *)
let lvalify_wrapper (v: varinfo) (fopt: fieldinfo option) : lval = 
  begin
    match fopt with
      | Some(f) -> (lvalify_varinfo_field v f);
      | None -> (lvalify_varinfo v);
  end

(** Add a field offset to an input offset: Add it at the bottom.
 * Thus, if we're given offset f.g, and we're passed field h, we
 * must make it f.g.h *)
(** Add a field offset to an lval *)
let add_field_to_lval (lv: lval) (f: fieldinfo) : lval = 
  begin
    let the_off = Field(f, NoOffset) in
    (addOffsetLval the_off lv);
  end

(** Wrapper for add_field_to_lval *)
let add_field_to_lval_wrapper (lv: lval) (fopt: fieldinfo option) : lval =
  begin
    (match fopt with
       | Some(f) -> (add_field_to_lval lv f);
       | None -> lv;
    );
  end

(** Add a field offset to an lval. Here we're provided a string as input *)
let add_field_to_lval_str (lv: lval) (field: string) : lval = 
  let lvtyp = typeOfLval lv in
  match lvtyp with
    | TComp(compinfo,_) ->
        (try 
           let finfo = getCompField compinfo field in
           add_field_to_lval lv finfo;
         with Not_found -> (
           let lvtypstr = typ_tostring lvtyp in
           fatal ["Field info not found in given type"; field; lvtypstr])
        );
    | _ -> fatal ["Non compinfo type passed to add_field_to_lval"];;
        
(* Creates an if statement given the condition, the list of statements to execute if it's true,
   and the list of statements to execute if it's false.
*)
let mkIfStmt (ifguard : exp) (true_block : stmt list) (false_block : stmt list) : stmt = 
  let check_trueblock = mkBlock true_block in
  let check_falseblock = mkBlock false_block in
  let check_ifstmt = If(ifguard, check_trueblock, check_falseblock, locUnknown) in
  mkStmt check_ifstmt

let addwarn (sl : string list) : unit = 
  allwarnings := List.append !allwarnings [sl]

let mkCallStmt (retval: lval option) (f: exp) (args: exp list) : stmt =
  let call = Call(retval, f, args, locUnknown) in
  mkStmt (Instr [call])
    
(** Announce message *)
let announcemsg(sl: string list) : unit = 
  begin
    let print_string(s: string) : unit = (Printf.fprintf stderr "\t%s\n%!" s) in
    (Printf.fprintf stderr "**************************************************************\n%!");
    (ignore (List.map print_string sl));
    (Printf.fprintf stderr "**************************************************************\n%!");
  end

(** Info message *)
let infomsg(sl: string list) : unit = 
  begin
    let print_string(s: string) : unit = (Printf.fprintf stderr " %s%!" s) in
    (ignore (List.map print_string sl));
    (Printf.fprintf stderr "\n%!");
  end

(****************************************************************************
 * Splitting and code generation
 ****************************************************************************)

(* lvalify a varinfo and then expify the lval *)
let expify_varinfo (v: varinfo) : exp = expify_lval (lvalify_varinfo v)

(* Create an expression list from formal parameters *)
let expify_varinfos (vi_list: varinfo list) : exp list =
  List.map expify_varinfo vi_list

let lvalify_varinfos (vi_list: varinfo list) : lval list =
  List.map lvalify_varinfo vi_list

let expify_lvals (lval_list: lval list) : exp list =
  List.map expify_lval lval_list

(* Ensure that the formals in an input fundec have names *)
let ensure_formals_have_names (fdec: fundec) : unit =
  let formals = fdec.sformals in
  let newformals = ref [] in
  let need_to_replace = ref false in
  for i = 0 to (List.length formals) - 1 do
    let ith = (List.nth formals i) in
    if (String.compare ith.vname "") = 0
    then begin
      ith.vname <- ("arg" ^ (itoa i));
      need_to_replace := true;
    end;
    newformals := (List.append !newformals [ith]);
  done;
  if (!need_to_replace = true)
  then (setFormals fdec !newformals)

(* True if the string contains some specified prefix, false otherwise. *)
let contains_prefix (s: string) (prefix: string) : bool =
  let prefix_regexp = (Str.regexp prefix) in
  Str.string_match prefix_regexp s 0

(* Strip the specified prefix from an input string *)
(* Throws an error if the prefix is not present *)
let strip_prefix (s: string) (prefix: string) : string =
  let prefix_len = (String.length prefix) in
  let s_len = (String.length s) in
  let prefix_regexp = (Str.regexp prefix) in
  if (Str.string_match prefix_regexp s 0) = false
  then fatal["Cannot strip prefix in "; s];
  try
    (String.sub s prefix_len (s_len - prefix_len));
  with Invalid_argument(_) -> fatal["strip_prefix failure"]

(* Attempt to strip the prefix, and return the original string
   if the attempt is unsuccessful *)
let strip_prefix_try (s: string) (prefix: string) : string =
  let prefix_len = (String.length prefix) in
  let s_len = (String.length s) in
  let prefix_regexp = (Str.regexp prefix) in
  if (Str.string_match prefix_regexp s 0) = false then
    s
  else
    try
      (String.sub s prefix_len (s_len - prefix_len));
    with Invalid_argument(_) ->
      s

(* Convert a kernel/driver function type into its corresponding
   Test Framework function type.  The idea is to use this to produce
   the prototypes for test framework functions automatically.
*)
let get_tf_fn_typ (init_typ: typ): typ =
  let helper_add_ptr (arg: (string * typ * attributes)) =
    let (str, t, attr) = arg in
      (str, TPtr (t, []), attr)
  in
  let helper_typs_only (arg: (string * typ * attributes)) =
    let (str, t, attr) = arg in
      t
  in
  let helper_typs_to_args (t: typ) =
      "", t, []
  in
  let tf_arg1_typ = TPtr (TInt (IChar, [Attr("const", [])]), []) in
  let tf_arg2_typ = TInt (IInt, []) in
    (* Get return value type and strip attributes *)
  let (tf_arg3_typ, tf_args_list_option, _, _) = splitFunctionType init_typ in
  let tf_arg3_typ =
      match tf_arg3_typ with
        | TVoid(_) ->
            [];
        | _ ->
            (* Make return value param a pointer *)
            [TPtr (tf_arg3_typ, [])]
  in
  let tf_args_list =
      match tf_args_list_option with
        | Some(x) -> x
        | None -> []
  in
  let tf_args_list = List.map helper_add_ptr tf_args_list in
  let tf_args_typs = List.map helper_typs_only tf_args_list in
  let tf_argslist_typ = [tf_arg1_typ] @ [tf_arg2_typ] @ tf_arg3_typ @ tf_args_typs in
  let tf_argslist = List.map helper_typs_to_args tf_argslist_typ in
  TFun ((intType, Some(tf_argslist), false, []))
    
(**---------------------------------------------------------------------------*)
(** Helper function, used for detecting comments *)
let is_comment (str: string) : bool =
  if (str = "") || (String.get str 0) = '#'
  then true else false
    
(** Return true if this is an interface function *)
let is_interface_function
    (interface_functions : (string, bool) Hashtbl.t)
    (fnm: string) : bool =
  try
    begin
      ignore (Hashtbl.find interface_functions fnm);
      true
    end
  with Not_found ->
    false

(* TODO: improve analysis *)
(* Checks ONLY those functions that we're overriding.
   E.g. sound library.  This is a hack.  The right way is to
   improve the analysis so that we can tell if it's
   an interface function or not *)
(* Returns:
   - 0 if we're not overriding anything.
   - 1 if we're overriding to non-interface
   - 2 if we're overriding to interface
*)
let is_override_interface
    (override_interface : (string, bool) Hashtbl.t)
    (fnm: string) : int =
  (* First see if the function is in the hash table *)
  (* If it is, then it's definitely not an interface function *)
  try
    let result = Hashtbl.find override_interface fnm in
    let retval =
      if result = true then 2 (* Interface *)
      else 1 (* Non-interface *)
    in
    Printf.fprintf stderr "is_override_interface: %s %d\n" fnm retval;
    retval;
  with Not_found ->
    begin
      Printf.fprintf stderr "is_override_interface: %s 0\n" fnm;
      0;
    end
     
(* A hashtable that stores temprorary variables that have been generated so
   far *)
let temporary_variables: (string, varinfo) Hashtbl.t = Hashtbl.create 117
        
let get_local_variable
    (fdec: fundec)
    (t: typ)
    (name: string)
    : varinfo =
  let type_str = typ_tostring_noattr t in
  let full_str = fdec.svar.vname ^ name ^ type_str in
  let hashkey = strip_whitespace full_str in
  try
    Hashtbl.find temporary_variables hashkey;
  with Not_found -> 
    let newtempvar = makeLocalVar fdec name t in
    Hashtbl.add temporary_variables hashkey newtempvar;
    newtempvar

let rec supported_ret_type (t: typ) : bool =
  match t with
    | TVoid(_) ->
        false
    | TInt(_, _)
    | TFloat(_, _)
    | TEnum(_, _)
    | TPtr(_, _) -> true
    | TNamed(ti, _) -> supported_ret_type ti.ttype
    | TArray(_, _, _)
    | TComp(_, _) -> false
    | TFun(_, _, _, _) -> fatal ["TFun return type"]
    | TBuiltin_va_list(_) -> fatal ["TBuiltin_va_list return typ"]

let lookup_prepostfn
    (fdecs: (fundec * fundec) list)
    (name: string)
    : (fundec * fundec) option =
  let name_first = prefn_prefix ^ name in
  let name_second = postfn_prefix ^ name in
  let predicate (fdec_pair : (fundec * fundec)) =
    let (first, second) = fdec_pair in
        first.svar.vname = name_first && second.svar.vname = name_second
  in
  try
    Some (List.find predicate fdecs)
  with Not_found ->
    None

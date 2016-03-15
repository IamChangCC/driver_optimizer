(*===========================================================================*)
(*
 * Device-driver analysis: This file contains general utilities.
 *
 * Vinod Ganapathy <vg@cs.wisc.edu>, August 1, 2006.
*)
(*===========================================================================*)

(** Useful string function *)
val strip_whitespace: string -> string

(** Failures *)
val warning: string list -> unit
val fatal: string list -> 'a

(** Dereference a type *)
val deref_ptr_typ: Cil.typ -> Cil.typ

(** Functions to manipulate lists *)
val add_to_list: 'a -> 'a list -> 'a list
val remove_repeats: 'a list -> 'a list

(** Functions to manipulate hash tables *)
val add_if: ('a,'b) Hashtbl.t -> 'a -> 'b -> unit 
val add_if_binding: ('a,'b) Hashtbl.t -> 'a -> 'b -> unit
val list_keys: ('a,'b) Hashtbl.t -> 'a list
val list_bindings: ('a,'b) Hashtbl.t -> 'b list
val list_keybindings: ('a,'b) Hashtbl.t -> ('a * 'b) list
val are_keysets_same: (string,'a) Hashtbl.t -> (string,'b) Hashtbl.t -> bool 
val copy_htab_in_place: ('a,'b) Hashtbl.t -> ('a,'b) Hashtbl.t -> unit

(** Misc utilities *)
val itoa: int -> string
val get_attribs: Cil.typ -> Cil.attributes
val strip_typ_attribs: Cil.typ -> Cil.typ
val is_same_typ: Cil.typ -> Cil.typ -> bool
val tcomp_compinfo: Cil.typ -> Cil.compinfo option
val isCompoundType: Cil.typ -> bool
val get_fieldinfo: Cil.compinfo -> string -> Cil.fieldinfo 
val fieldinfo_name: Cil.fieldinfo -> string
val makevarname: string -> string

(** Prettyprint something *)
val typ_tostring: Cil.typ -> string
val instr_tostring: Cil.instr -> string
val stmt_tostring: Cil.stmt -> string
val typ_tostring_noattr: Cil.typ -> string
val typ_tostring_attronly: Cil.typ -> string
val lval_tostring: Cil.lval -> string
val exp_tostring: Cil.exp -> string
val offset_tostring: Cil.offset -> string
val init_tostring: Cil.init -> string
val get_params_str_fundec: Cil.fundec -> string

(** Lval utilities *)
val lval_to_function: Cil.lval -> string
val exp_to_function: Cil.exp -> string
val expify_fundec: Cil.fundec -> Cil.exp
val expify_lval: Cil.lval -> Cil.exp
val lvalify_varinfo: Cil.varinfo -> Cil.lval
val lvalify_varinfo_field: Cil.varinfo -> Cil.fieldinfo -> Cil.lval
val lvalify_wrapper: Cil.varinfo -> Cil.fieldinfo option -> Cil.lval
val add_field_to_lval: Cil.lval -> Cil.fieldinfo -> Cil.lval
val add_field_to_lval_str: Cil.lval -> string -> Cil.lval
val add_field_to_lval_wrapper: Cil.lval -> Cil.fieldinfo option -> Cil.lval

(** If statement *)
val mkIfStmt: Cil.exp -> Cil.stmt list -> Cil.stmt list -> Cil.stmt
val mkCallStmt: Cil.lval option -> Cil.exp -> Cil.exp list -> Cil.stmt

(** Warnings *)
val addwarn: string list -> unit

(** Infomsgs *)
val announcemsg: string list -> unit
val infomsg: string list -> unit

(** Splitting and code generation *)
val expify_varinfo: Cil.varinfo -> Cil.exp
val expify_varinfos: Cil.varinfo list -> Cil.exp list
val lvalify_varinfos: Cil.varinfo list -> Cil.lval list
val expify_lvals: Cil.lval list -> Cil.exp list
val ensure_formals_have_names: Cil.fundec -> unit
val contains_prefix: string -> string -> bool
val strip_prefix: string -> string -> string
val strip_prefix_try: string -> string -> string

val get_tf_fn_typ: Cil.typ -> Cil.typ

val is_interface_function :
  (string, bool) Hashtbl.t ->
  string -> bool

val is_comment:
  string ->
  bool

val get_local_variable :
  Cil.fundec ->
  Cil.typ ->
  string ->
  Cil.varinfo

val supported_ret_type: Cil.typ -> bool

val lookup_prepostfn:
  (Cil.fundec * Cil.fundec) list ->
  string ->
  (Cil.fundec * Cil.fundec) option

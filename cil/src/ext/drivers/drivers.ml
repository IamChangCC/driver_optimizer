(*===========================================================================*)
(*
 * CIL Module for device driver analysis.
 *
 * Vinod Ganapathy <vg@cs.wisc.edu>, August 1, 2006.
 *)
(*===========================================================================*)


open Cil
open Str
open Utils_dri              (* General utilities *)
open Root_annot_dri         (* Extract root annotations *)
open Splitter_dri           (* Splitter *)

(*---------------------------------------------------------------------------*)
(** Toplevel function that performs device-driver analysis.
 * This function does different things based upon the value of the flag that
 * it is invoked with. It either performs annotation, or it performs splitting.
 *)
let dodriveranalysis (f:file) : unit = 
  begin
    (* Simplify the program to make it look like a CFG -- the makeCFG
     * feature of CIL. This simplifies the analysis that we have to do *)
    (ignore (Partial.calls_end_basic_blocks f));
    (ignore (Partial.globally_unique_vids f));

    (* This has a bug in some driver *)
(*
  Cfg.computeFileCFG f;
  Cil.printCilAsIs := true; (* Leave complex while(1) loops *)
*)

(*    Cil.iterGlobals f (fun glob -> 
                         match glob with
                           | Cil.GFun(fd,_) -> 
                               Cil.prepareCFG fd; 
                               (ignore (Cil.computeCFGInfo fd true))
                           | _ -> ()
                      );
*)
    (* Overide some lame CIL options: *)
    (*Cil.lineDirectiveStyle := Some (LinePreprocessorOutput);*) (* Ensures the stupid #line
                                                         directives are not printed *)
    (* Options include:
       LineComment --commPrintLn
       LinePreprocessorInput   (default/no command line)
       LinePreprocessorOutput  (no command line option provided)
       None   --noPrintLn
    *)

    Cil.useLogicalOperators := true;

    announcemsg ["Generating symbolic driver"];
    Splitter_dri.do_splitting f;
    Printf.fprintf stderr "#### Total execution time: %f\n" (Sys.time());
  end

(*---------------------------------------------------------------------------*)
(** CIL feature descriptions *)
let feature : featureDescr = 
  { fd_name = "drivers";
    fd_enabled = ref false;
    fd_description = "device-driver analysis";
    fd_extraopt = 
      [
        ("--do-test-framework",
         Arg.Bool (fun f -> Globals_dri.g_test_framework := f),
         "<true/false> true = Generate calls to _MJRcheck testing functions.  \
         false = don't generate calls");

        ("--do-loop-priority",
         Arg.Bool (fun f -> Globals_dri.g_loop_priority := f),
         "<true/false> true = Generate calls to loop prioritization \
         functions, false = don't generate calls");

        ("--do-return-priority",
         Arg.Bool (fun f -> Globals_dri.g_return_priority := f),
         "<true/false> true = Generate calls to return prioritization \
         functions, false = don't generate calls");
        
        ("--do-verify-in-driver",
         Arg.Bool (fun f -> Globals_dri.g_verify_in_driver := f),
         "<true/false> true = Generate calls to verify that \
         we're in the driver, false = don't generate calls");

        ("--do-extra-uprintk",
         Arg.Bool (fun f -> Globals_dri.g_extra_uprintk := f),
         "<true/false> true = Generate more uprintk calls so \
         we know where are, false = don't generate calls");

        ("--do-gen-test-framework-stubs",
         Arg.Bool (fun f -> Globals_dri.g_gen_test_framework_stubs := f),
         "<true/false> true = Generate empty test_framework functions to fill in \
          manually later.  Do not leave this turned on except to generate the stubs. \
          false = don't generate the empty stubs");

        ("--do-freebsd",
         Arg.Bool (fun f -> Globals_dri.g_freebsd := f),
         "<true/false> true = FreeBSD-specific code generation, \
          false = Linux-specific code generation");
      ];
    fd_doit = dodriveranalysis;
    fd_post_check = true
  }

(*===========================================================================*)

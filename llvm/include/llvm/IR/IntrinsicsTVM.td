//===- IntrinsicsTVM.td - Defines TVM intrinsics -----------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines all of the TVM-specific intrinsics.
//
//===----------------------------------------------------------------------===//

let TargetPrefix = "tvm" in {
  // A.4 Constant and literal primitives
  def int_tvm_pushref :
    ClangBuiltin<"__builtin_tvm_pushref">,
    Intrinsic<[llvm_TVMCell_ty], []>;

  // A.6 Comparison primitives
  def int_tvm_sempty :
    ClangBuiltin<"__builtin_tvm_sempty">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMSlice_ty], [IntrNoMem]>;
  def int_tvm_sdempty :
    ClangBuiltin<"__builtin_tvm_sdempty">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMSlice_ty], [IntrNoMem]>;
  def int_tvm_srempty :
    ClangBuiltin<"__builtin_tvm_srempty">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMSlice_ty], [IntrNoMem]>;
  def int_tvm_sdfirst :
    ClangBuiltin<"__builtin_tvm_sdfirst">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMSlice_ty], [IntrNoMem]>;
  def int_tvm_sdlexcmp :
    ClangBuiltin<"__builtin_tvm_sdlexcmp">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMSlice_ty, llvm_TVMSlice_ty],
              [IntrNoMem]>;
  def int_tvm_sdeq :
    ClangBuiltin<"__builtin_tvm_sdeq">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMSlice_ty, llvm_TVMSlice_ty],
              [IntrNoMem]>;
  def int_tvm_sdpfx :
    ClangBuiltin<"__builtin_tvm_sdpfx">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMSlice_ty, llvm_TVMSlice_ty],
              [IntrNoMem]>;
  def int_tvm_sdpfxrev :
    ClangBuiltin<"__builtin_tvm_sdpfxrev">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMSlice_ty, llvm_TVMSlice_ty],
              [IntrNoMem]>;
  def int_tvm_sdppfx :
    ClangBuiltin<"__builtin_tvm_sdppfx">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMSlice_ty, llvm_TVMSlice_ty],
              [IntrNoMem]>;
  def int_tvm_sdppfxrev :
    ClangBuiltin<"__builtin_tvm_sdppfxrev">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMSlice_ty, llvm_TVMSlice_ty],
              [IntrNoMem]>;
  def int_tvm_sdsfx :
    ClangBuiltin<"__builtin_tvm_sdsfx">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMSlice_ty, llvm_TVMSlice_ty],
              [IntrNoMem]>;
  def int_tvm_sdsfxrev :
    ClangBuiltin<"__builtin_tvm_sdsfxrev">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMSlice_ty, llvm_TVMSlice_ty],
              [IntrNoMem]>;
  def int_tvm_sdpsfx :
    ClangBuiltin<"__builtin_tvm_sdpsfx">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMSlice_ty, llvm_TVMSlice_ty],
              [IntrNoMem]>;
  def int_tvm_sdpsfxrev :
    ClangBuiltin<"__builtin_tvm_sdpsfxrev">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMSlice_ty, llvm_TVMSlice_ty],
              [IntrNoMem]>;
  def int_tvm_sdcntlead0 :
    ClangBuiltin<"__builtin_tvm_sdcntlead0">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMSlice_ty], [IntrNoMem]>;
  def int_tvm_sdcntlead1 :
    ClangBuiltin<"__builtin_tvm_sdcntlead1">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMSlice_ty], [IntrNoMem]>;
  def int_tvm_sdcnttrail0 :
    ClangBuiltin<"__builtin_tvm_sdcnttrail0">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMSlice_ty], [IntrNoMem]>;
  def int_tvm_sdcnttrail1 :
    ClangBuiltin<"__builtin_tvm_sdcnttrail1">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMSlice_ty], [IntrNoMem]>;

  // A.7 Cell primitives
  def int_tvm_ends :
    ClangBuiltin<"__builtin_tvm_ends">,
    Intrinsic<[], [llvm_TVMSlice_ty]>;
  def int_tvm_sti :
    ClangBuiltin<"__builtin_tvm_sti">,
    Intrinsic<[llvm_TVMBuilder_ty], [llvm_i257_ty, llvm_TVMBuilder_ty, llvm_i257_ty],
              [IntrNoMem]>;
  def int_tvm_stu :
    ClangBuiltin<"__builtin_tvm_stu">,
    Intrinsic<[llvm_TVMBuilder_ty], [llvm_i257_ty, llvm_TVMBuilder_ty, llvm_i257_ty],
              [IntrNoMem]>;
  def int_tvm_ldu :
    ClangBuiltin<"__builtin_tvm_ldu">,
    Intrinsic<[llvm_i257_ty, llvm_TVMSlice_ty], [llvm_TVMSlice_ty, llvm_i257_ty],
              [IntrNoMem]>;
  def int_tvm_pldu :
    ClangBuiltin<"__builtin_tvm_pldu">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMSlice_ty, llvm_i257_ty],
              [IntrNoMem]>;
  def int_tvm_lduq :
    ClangBuiltin<"__builtin_tvm_lduq">,
    Intrinsic<[llvm_i257_ty, llvm_TVMSlice_ty, llvm_i257_ty],
              [llvm_TVMSlice_ty, llvm_i257_ty],
              [IntrNoMem]>;

  def int_tvm_ldi :
    ClangBuiltin<"__builtin_tvm_ldi">,
    Intrinsic<[llvm_i257_ty, llvm_TVMSlice_ty], [llvm_TVMSlice_ty, llvm_i257_ty],
              [IntrNoMem]>;
  def int_tvm_pldi :
    ClangBuiltin<"__builtin_tvm_pldi">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMSlice_ty, llvm_i257_ty],
              [IntrNoMem]>;

  def int_tvm_ldvaruint16 :
    ClangBuiltin<"__builtin_tvm_ldvaruint16">,
    Intrinsic<[llvm_i257_ty, llvm_TVMSlice_ty], [llvm_TVMSlice_ty],
              [IntrNoMem]>;

  def int_tvm_ldvaruint32 :
    ClangBuiltin<"__builtin_tvm_ldvaruint32">,
    Intrinsic<[llvm_i257_ty, llvm_TVMSlice_ty], [llvm_TVMSlice_ty],
              [IntrNoMem]>;

  def int_tvm_ldvarint16 :
    ClangBuiltin<"__builtin_tvm_ldvarint16">,
    Intrinsic<[llvm_i257_ty, llvm_TVMSlice_ty], [llvm_TVMSlice_ty],
              [IntrNoMem]>;

  def int_tvm_ldvarint32 :
    ClangBuiltin<"__builtin_tvm_ldvarint32">,
    Intrinsic<[llvm_i257_ty, llvm_TVMSlice_ty], [llvm_TVMSlice_ty],
              [IntrNoMem]>;

  def int_tvm_stvaruint16 :
    ClangBuiltin<"__builtin_tvm_stvaruint16">,
    Intrinsic<[llvm_TVMBuilder_ty], [llvm_TVMBuilder_ty, llvm_i257_ty],
              [IntrNoMem]>;

  def int_tvm_stvarint16 :
    ClangBuiltin<"__builtin_tvm_stvarint16">,
    Intrinsic<[llvm_TVMBuilder_ty], [llvm_TVMBuilder_ty, llvm_i257_ty],
              [IntrNoMem]>;

  def int_tvm_stvaruint32 :
    ClangBuiltin<"__builtin_tvm_stvaruint32">,
    Intrinsic<[llvm_TVMBuilder_ty], [llvm_TVMBuilder_ty, llvm_i257_ty],
              [IntrNoMem]>;

  def int_tvm_stvarint32 :
    ClangBuiltin<"__builtin_tvm_stvarint32">,
    Intrinsic<[llvm_TVMBuilder_ty], [llvm_TVMBuilder_ty, llvm_i257_ty],
              [IntrNoMem]>;

  // A.10 Dictionary manipulation primitives
  def int_tvm_newdict : ClangBuiltin<"__builtin_tvm_newdict">,
                        Intrinsic<[llvm_TVMCell_ty /* Dict */], []>;
  def int_tvm_dictget :
    ClangBuiltin<"__builtin_tvm_dictget">,
    Intrinsic<[llvm_TVMSlice_ty, llvm_i257_ty],
              [llvm_TVMSlice_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;
  def int_tvm_dictgetref :
    ClangBuiltin<"__builtin_tvm_dictgetref">,
    Intrinsic<[llvm_TVMCell_ty, llvm_i257_ty],
              [llvm_TVMSlice_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;
  def int_tvm_dictiget :
    ClangBuiltin<"__builtin_tvm_dictiget">,
    Intrinsic<[llvm_TVMSlice_ty, llvm_i257_ty],
              [llvm_i257_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;
  def int_tvm_dictigetref :
    ClangBuiltin<"__builtin_tvm_dictigetref">,
    Intrinsic<[llvm_TVMCell_ty, llvm_i257_ty],
              [llvm_i257_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;
  def int_tvm_dictuget :
    ClangBuiltin<"__builtin_tvm_dictuget">,
    Intrinsic<[llvm_TVMSlice_ty, llvm_i257_ty],
              [llvm_i257_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;
  def int_tvm_dictugetref :
    ClangBuiltin<"__builtin_tvm_dictugetref">,
    Intrinsic<[llvm_TVMCell_ty, llvm_i257_ty],
              [llvm_i257_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

  def int_tvm_dictgetnext :
    ClangBuiltin<"__builtin_tvm_dictgetnext">,
    Intrinsic<[llvm_TVMSlice_ty, llvm_TVMSlice_ty, llvm_i257_ty],
              [llvm_TVMSlice_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;
  def int_tvm_dictmin :
    ClangBuiltin<"__builtin_tvm_dictmin">,
    Intrinsic<[llvm_TVMSlice_ty, llvm_TVMSlice_ty, llvm_i257_ty],
              [llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

  def int_tvm_dictugetnext :
    ClangBuiltin<"__builtin_tvm_dictugetnext">,
    Intrinsic<[llvm_TVMSlice_ty, llvm_i257_ty, llvm_i257_ty],
              [llvm_i257_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;
  def int_tvm_dictumin :
    ClangBuiltin<"__builtin_tvm_dictumin">,
    Intrinsic<[llvm_TVMSlice_ty, llvm_i257_ty, llvm_i257_ty],
              [llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;
  def int_tvm_dictuminref :
    ClangBuiltin<"__builtin_tvm_dictuminref">,
    Intrinsic<[llvm_TVMCell_ty, llvm_i257_ty, llvm_i257_ty],
              [llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;
  def int_tvm_dictugetprev :
    ClangBuiltin<"__builtin_tvm_dictugetprev">,
    Intrinsic<[llvm_TVMSlice_ty, llvm_i257_ty, llvm_i257_ty],
              [llvm_i257_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;
  def int_tvm_dictumax :
    ClangBuiltin<"__builtin_tvm_dictumax">,
    Intrinsic<[llvm_TVMSlice_ty, llvm_i257_ty, llvm_i257_ty],
              [llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;
  def int_tvm_dictumaxref :
    ClangBuiltin<"__builtin_tvm_dictumaxref">,
    Intrinsic<[llvm_TVMCell_ty, llvm_i257_ty, llvm_i257_ty],
              [llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;
  def int_tvm_dicturemmin :
    ClangBuiltin<"__builtin_tvm_dicturemmin">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_TVMSlice_ty,
               llvm_i257_ty, llvm_i257_ty],
              [llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;
  def int_tvm_dicturemminref :
    ClangBuiltin<"__builtin_tvm_dicturemminref">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_TVMCell_ty,
               llvm_i257_ty, llvm_i257_ty],
              [llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;
  def int_tvm_dicturemmax :
    ClangBuiltin<"__builtin_tvm_dicturemmax">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_TVMSlice_ty,
               llvm_i257_ty, llvm_i257_ty],
              [llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;
  def int_tvm_dicturemmaxref :
    ClangBuiltin<"__builtin_tvm_dicturemmaxref">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_TVMCell_ty,
               llvm_i257_ty, llvm_i257_ty],
              [llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;
  def int_tvm_dictset :
    ClangBuiltin<"__builtin_tvm_dictset">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */],
              [llvm_TVMSlice_ty, llvm_TVMSlice_ty,
               llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;
  def int_tvm_dictsetref :
    ClangBuiltin<"__builtin_tvm_dictsetref">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */],
              [llvm_TVMCell_ty, llvm_TVMSlice_ty,
               llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;
  def int_tvm_dictiset :
    ClangBuiltin<"__builtin_tvm_dictiset">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */],
              [llvm_TVMSlice_ty, llvm_i257_ty,
               llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;
  def int_tvm_dictisetref :
    ClangBuiltin<"__builtin_tvm_dictisetref">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */],
              [llvm_TVMCell_ty, llvm_i257_ty,
               llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;
  def int_tvm_dictuset :
    ClangBuiltin<"__builtin_tvm_dictuset">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */],
              [llvm_TVMSlice_ty, llvm_i257_ty,
               llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;
  def int_tvm_dictusetref :
    ClangBuiltin<"__builtin_tvm_dictusetref">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */],
              [llvm_TVMCell_ty, llvm_i257_ty,
               llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;
  def int_tvm_dictusetget :
    ClangBuiltin<"__builtin_tvm_dictusetget">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_TVMSlice_ty, llvm_i257_ty],
              [llvm_TVMSlice_ty, llvm_i257_ty,
               llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;
  def int_tvm_dictusetgetref :
    ClangBuiltin<"__builtin_tvm_dictusetgetref">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_TVMCell_ty, llvm_i257_ty],
              [llvm_TVMCell_ty, llvm_i257_ty,
               llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;
  def int_tvm_dictsetget :
    ClangBuiltin<"__builtin_tvm_dictsetget">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_TVMSlice_ty, llvm_i257_ty],
              [llvm_TVMSlice_ty, llvm_TVMSlice_ty,
               llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;
  def int_tvm_dictsetgetref :
    ClangBuiltin<"__builtin_tvm_dictsetgetref">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_TVMCell_ty, llvm_i257_ty],
              [llvm_TVMCell_ty, llvm_TVMSlice_ty,
               llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;
  def int_tvm_dictreplace :
    ClangBuiltin<"__builtin_tvm_dictreplace">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_i257_ty],
              [llvm_TVMSlice_ty, llvm_TVMSlice_ty,
               llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;
  def int_tvm_dictreplaceref :
    ClangBuiltin<"__builtin_tvm_dictreplaceref">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_i257_ty],
              [llvm_TVMCell_ty, llvm_TVMSlice_ty,
               llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;
  def int_tvm_dictireplace :
    ClangBuiltin<"__builtin_tvm_dictireplace">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_i257_ty],
              [llvm_TVMSlice_ty, llvm_i257_ty,
               llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;
  def int_tvm_dictireplaceref :
    ClangBuiltin<"__builtin_tvm_dictireplaceref">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_i257_ty],
              [llvm_TVMCell_ty, llvm_i257_ty,
               llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;
  def int_tvm_dictureplace :
    ClangBuiltin<"__builtin_tvm_dictureplace">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_i257_ty],
              [llvm_TVMSlice_ty, llvm_i257_ty,
               llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;
  def int_tvm_dictureplaceref :
    ClangBuiltin<"__builtin_tvm_dictureplaceref">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_i257_ty],
              [llvm_TVMCell_ty, llvm_i257_ty,
               llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

  def int_tvm_dictadd :
    ClangBuiltin<"__builtin_tvm_dictadd">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_i257_ty],
              [llvm_TVMSlice_ty, llvm_TVMSlice_ty,
               llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;
  def int_tvm_dictaddref :
    ClangBuiltin<"__builtin_tvm_dictaddref">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_i257_ty],
              [llvm_TVMCell_ty, llvm_TVMSlice_ty,
               llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;
  def int_tvm_dictiadd :
    ClangBuiltin<"__builtin_tvm_dictiadd">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_i257_ty],
              [llvm_TVMSlice_ty, llvm_i257_ty,
               llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;
  def int_tvm_dictiaddref :
    ClangBuiltin<"__builtin_tvm_dictiaddref">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_i257_ty],
              [llvm_TVMCell_ty, llvm_i257_ty,
               llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;
  def int_tvm_dictuadd :
    ClangBuiltin<"__builtin_tvm_dictuadd">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_i257_ty],
              [llvm_TVMSlice_ty, llvm_i257_ty,
               llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;
  def int_tvm_dictuaddref :
    ClangBuiltin<"__builtin_tvm_dictuaddref">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_i257_ty],
              [llvm_TVMCell_ty, llvm_i257_ty,
               llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;
  def int_tvm_dictdelget :
    ClangBuiltin<"__builtin_tvm_dictdelget">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_TVMSlice_ty, llvm_i257_ty],
              [llvm_TVMSlice_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;
  def int_tvm_dictdelgetref :
    ClangBuiltin<"__builtin_tvm_dictdelgetref">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_TVMCell_ty, llvm_i257_ty],
              [llvm_TVMSlice_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;
  def int_tvm_dictudel :
    ClangBuiltin<"__builtin_tvm_dictudel">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_i257_ty],
              [llvm_i257_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;
  def int_tvm_dictdel :
    ClangBuiltin<"__builtin_tvm_dictdel">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_i257_ty],
              [llvm_TVMSlice_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;
  def int_tvm_lddict :
    ClangBuiltin<"__builtin_tvm_lddict">,
    Intrinsic<[llvm_TVMCell_ty/* Dictionary */, llvm_TVMSlice_ty],
              [llvm_TVMSlice_ty], [IntrNoMem]>;
  def int_tvm_plddict :
    ClangBuiltin<"__builtin_tvm_plddict">,
    Intrinsic<[llvm_TVMCell_ty], [llvm_TVMSlice_ty /* Dictionary-as-Slice */],
              [IntrNoMem]>;
  def int_tvm_stdicts :
    ClangBuiltin<"__builtin_tvm_stdicts">,
    Intrinsic<[llvm_TVMBuilder_ty], [llvm_TVMSlice_ty /* Dictionary-as-Slice */,
              llvm_TVMBuilder_ty], [IntrNoMem]>;
  def int_tvm_stdict :
    ClangBuiltin<"__builtin_tvm_stdict">,
    Intrinsic<[llvm_TVMBuilder_ty], [llvm_TVMCell_ty /* Dictionary */,
              llvm_TVMBuilder_ty], [IntrNoMem]>;

  // A.11 Application-specific primitives
  def int_tvm_hashcu :
    ClangBuiltin<"__builtin_tvm_hashcu">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMCell_ty],
              [IntrNoMem]>;
  def int_tvm_hashsu :
    ClangBuiltin<"__builtin_tvm_hashsu">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMSlice_ty],
              [IntrNoMem]>;
  def int_tvm_mycode :
    ClangBuiltin<"__builtin_tvm_mycode">,
    Intrinsic<[llvm_TVMCell_ty], [],
              [IntrNoMem]>;
  def int_tvm_chksignu :
    ClangBuiltin<"__builtin_tvm_chksignu">,
    Intrinsic<[llvm_i257_ty], [llvm_i257_ty, llvm_TVMSlice_ty, llvm_i257_ty],
              [IntrNoMem]>;

  // A.11.2. Gas-related primitives.
  def int_tvm_accept :
    ClangBuiltin<"__builtin_tvm_accept">,
    Intrinsic<[], []>;
  def int_tvm_setgaslimit :
   ClangBuiltin<"__builtin_tvm_setgaslimit">,
    Intrinsic<[], [llvm_i257_ty]>;
  def int_tvm_buygas :
    ClangBuiltin<"__builtin_tvm_buygas">,
    Intrinsic<[], [llvm_i257_ty]>;
  def int_tvm_gramtogas :
    ClangBuiltin<"__builtin_tvm_gramtogas">,
    Intrinsic<[llvm_i257_ty], [llvm_i257_ty]>;
  def int_tvm_gastogram :
    ClangBuiltin<"__builtin_tvm_gastogram">,
    Intrinsic<[llvm_i257_ty], [llvm_i257_ty]>;
  def int_tvm_commit :
    ClangBuiltin<"__builtin_tvm_commit">,
    Intrinsic<[], []>;

  // A.11.4. Configuration primitives.
  def int_tvm_getparam :
    ClangBuiltin<"__builtin_tvm_getparam">,
    Intrinsic<[llvm_i257_ty], [llvm_i257_ty],
              [IntrNoMem]>;
  def int_tvm_now :
    ClangBuiltin<"__builtin_tvm_now">,
    Intrinsic<[llvm_i257_ty], [], [IntrNoMem]>;
  def int_tvm_blocklt :
    ClangBuiltin<"__builtin_tvm_blocklt">,
    Intrinsic<[llvm_i257_ty], [], [IntrNoMem]>;
  def int_tvm_ltime :
    ClangBuiltin<"__builtin_tvm_ltime">,
    Intrinsic<[llvm_i257_ty], [], [IntrNoMem]>;
  def int_tvm_myaddr :
    ClangBuiltin<"__builtin_tvm_myaddr">,
    Intrinsic<[llvm_TVMSlice_ty], [], [IntrNoMem]>;
  def int_tvm_configroot :
    ClangBuiltin<"__builtin_tvm_configroot">,
    Intrinsic<[llvm_TVMCell_ty], [], [IntrNoMem]>;
  def int_tvm_configdict :
    ClangBuiltin<"__builtin_tvm_configdict">,
    Intrinsic<[llvm_TVMCell_ty, llvm_i257_ty], [], [IntrNoMem]>;
  def int_tvm_configparam :
    ClangBuiltin<"__builtin_tvm_configparam">,
    Intrinsic<[llvm_TVMCell_ty, llvm_i257_ty], [llvm_i257_ty], [IntrNoMem]>;
  def int_tvm_configoptparam :
    ClangBuiltin<"__builtin_tvm_configoptparam">,
    Intrinsic<[llvm_TVMCell_ty], [llvm_i257_ty], [IntrNoMem]>;

  // A.11.5. Global variable primitives.
  def int_tvm_getglobal :
    ClangBuiltin<"__builtin_tvm_getglobal">,
    Intrinsic<[llvm_i257_ty], [llvm_i257_ty], [IntrInaccessibleMemOnly, IntrReadMem]>;
  def int_tvm_setglobal :
    ClangBuiltin<"__builtin_tvm_setglobal">,
    Intrinsic<[], [llvm_i257_ty, llvm_i257_ty], [IntrInaccessibleMemOnly]>;

  // A.11.7. Application specific misc primitives.
  def int_tvm_cdatasize :
    ClangBuiltin<"__builtin_tvm_cdatasize">,
    Intrinsic<[llvm_i257_ty, llvm_i257_ty, llvm_i257_ty], [llvm_TVMCell_ty, llvm_i257_ty],
              [IntrNoMem]>;

  // A.11.8 Message and address manipulation primitives.
  def int_tvm_ldmsgaddr :
    ClangBuiltin<"__builtin_tvm_ldmsgaddr">,
    Intrinsic<[llvm_TVMSlice_ty, llvm_TVMSlice_ty], [llvm_TVMSlice_ty],
              [IntrNoMem]>;
  def int_tvm_ldmsgaddrq :
    ClangBuiltin<"__builtin_tvm_ldmsgaddrq">,
    Intrinsic<[llvm_TVMSlice_ty, llvm_TVMSlice_ty, llvm_i257_ty],
              [llvm_TVMSlice_ty],
              [IntrNoMem]>;

  def int_tvm_parsemsgaddr :
    ClangBuiltin<"__builtin_tvm_parsemsgaddr">,
    Intrinsic<[llvm_TVMTuple_ty], [llvm_TVMSlice_ty],
              [IntrNoMem]>;
  def int_tvm_parsemsgaddrq :
    ClangBuiltin<"__builtin_tvm_parsemsgaddrq">,
    Intrinsic<[llvm_TVMTuple_ty, llvm_i257_ty],
              [llvm_TVMSlice_ty],
              [IntrNoMem]>;

  def int_tvm_rewritestdaddr :
    ClangBuiltin<"__builtin_tvm_rewritestdaddr">,
    Intrinsic<[llvm_i257_ty, llvm_i257_ty], [llvm_TVMSlice_ty],
              [IntrNoMem]>;
  def int_tvm_rewritestdaddrq :
    ClangBuiltin<"__builtin_tvm_rewritestdaddrq">,
    Intrinsic<[llvm_i257_ty, llvm_i257_ty, llvm_i257_ty],
              [llvm_TVMSlice_ty],
              [IntrNoMem]>;

  def int_tvm_rewritevaraddr :
    ClangBuiltin<"__builtin_tvm_rewritevaraddr">,
    Intrinsic<[llvm_i257_ty, llvm_TVMSlice_ty], [llvm_TVMSlice_ty],
              [IntrNoMem]>;
  def int_tvm_rewritevaraddrq :
    ClangBuiltin<"__builtin_tvm_rewritevaraddrq">,
    Intrinsic<[llvm_i257_ty, llvm_TVMSlice_ty, llvm_i257_ty],
              [llvm_TVMSlice_ty],
              [IntrNoMem]>;

  // A.11.10. Outbound message and output action primitives.
  def int_tvm_rawreserve :
    ClangBuiltin<"__builtin_tvm_rawreserve">,
    Intrinsic<[], [llvm_i257_ty, llvm_i257_ty], []>;
  def int_tvm_setcode :
    ClangBuiltin<"__builtin_tvm_setcode">,
    Intrinsic<[], [llvm_TVMCell_ty], []>;

  // A.13 Codepage primitives
  def int_tvm_setcp :
    ClangBuiltin<"__builtin_tvm_setcp">,
    Intrinsic<[], [llvm_i257_ty]>;
  // ----------------
  def int_tvm_div :
    ClangBuiltin<"__builtin_tvm_div">,
    Intrinsic<[llvm_i257_ty], [llvm_i257_ty, llvm_i257_ty],
              [IntrNoMem]>;

  def int_tvm_mod :
    ClangBuiltin<"__builtin_tvm_mod">,
    Intrinsic<[llvm_i257_ty], [llvm_i257_ty, llvm_i257_ty],
              [IntrNoMem]>;

  def int_tvm_muldivr :
    ClangBuiltin<"__builtin_tvm_muldivr">,
    Intrinsic<[llvm_i257_ty], [llvm_i257_ty, llvm_i257_ty, llvm_i257_ty],
              [IntrNoMem]>;

  def int_tvm_bitsize :
    ClangBuiltin<"__builtin_tvm_bitsize">,
    Intrinsic<[llvm_i257_ty], [llvm_i257_ty], [IntrNoMem]>;
  def int_tvm_ubitsize :
    ClangBuiltin<"__builtin_tvm_ubitsize">,
    Intrinsic<[llvm_i257_ty], [llvm_i257_ty], [IntrNoMem]>;
  def int_tvm_fitsx :
    ClangBuiltin<"__builtin_tvm_fitsx">,
    Intrinsic<[llvm_i257_ty], [llvm_i257_ty, llvm_i257_ty], [IntrNoMem]>;
  def int_tvm_ufitsx :
    ClangBuiltin<"__builtin_tvm_ufitsx">,
    Intrinsic<[llvm_i257_ty], [llvm_i257_ty, llvm_i257_ty], [IntrNoMem]>;

  // IntrHasSideEffects to avoid empty builder keep
  //  through coroutine suspension points
  def int_tvm_newc :
    ClangBuiltin<"__builtin_tvm_newc">,
    Intrinsic<[llvm_TVMBuilder_ty], [], [IntrInaccessibleMemOnly, IntrReadMem]>;
  def int_tvm_ldref :
    ClangBuiltin<"__builtin_tvm_ldref">,
    Intrinsic<[llvm_TVMCell_ty, llvm_TVMSlice_ty],
              [llvm_TVMSlice_ty], [IntrNoMem]>;
  def int_tvm_ldrefrtos :
    ClangBuiltin<"__builtin_tvm_ldrefrtos">,
    Intrinsic<[llvm_TVMSlice_ty, llvm_TVMSlice_ty],
              [llvm_TVMSlice_ty], [IntrNoMem]>;
  def int_tvm_ldslice :
    ClangBuiltin<"__builtin_tvm_ldslice">,
    Intrinsic<[llvm_TVMSlice_ty, llvm_TVMSlice_ty],
              [llvm_TVMSlice_ty, llvm_i257_ty], [IntrNoMem]>;
  def int_tvm_sbits :
    ClangBuiltin<"__builtin_tvm_sbits">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMSlice_ty], [IntrNoMem]>;
  def int_tvm_srefs :
    ClangBuiltin<"__builtin_tvm_srefs">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMSlice_ty], [IntrNoMem]>;
  def int_tvm_sbitrefs :
    ClangBuiltin<"__builtin_tvm_sbitrefs">,
    Intrinsic<[llvm_i257_ty, llvm_i257_ty], [llvm_TVMSlice_ty], [IntrNoMem]>;
  def int_tvm_brembits :
    ClangBuiltin<"__builtin_tvm_brembits">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMBuilder_ty], [IntrNoMem]>;
  def int_tvm_bremrefs :
    ClangBuiltin<"__builtin_tvm_bremrefs">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMBuilder_ty], [IntrNoMem]>;
  def int_tvm_inttoslice :
    ClangBuiltin<"__builtin_tvm_inttoslice">,
    Intrinsic<[llvm_TVMSlice_ty], [llvm_i257_ty], [IntrNoMem]>;
  def int_tvm_ctos :
    ClangBuiltin<"__builtin_tvm_ctos">,
    Intrinsic<[llvm_TVMSlice_ty], [llvm_TVMCell_ty], [IntrNoMem]>;
  def int_tvm_stoc :
    ClangBuiltin<"__builtin_tvm_stoc">,
    Intrinsic<[llvm_TVMCell_ty], [llvm_TVMSlice_ty], [IntrNoMem]>;
  def int_tvm_stslice :
    ClangBuiltin<"__builtin_tvm_stslice">,
    Intrinsic<[llvm_TVMBuilder_ty],
              [llvm_TVMSlice_ty, llvm_TVMBuilder_ty], [IntrNoMem]>;
  def int_tvm_stref :
    ClangBuiltin<"__builtin_tvm_stref">,
    Intrinsic<[llvm_TVMBuilder_ty],
              [llvm_TVMCell_ty, llvm_TVMBuilder_ty], [IntrNoMem]>;
  def int_tvm_throw :
    ClangBuiltin<"__builtin_tvm_throw">,
    Intrinsic<[], [llvm_i257_ty], [IntrNoReturn]>;
  def int_tvm_throwif :
    ClangBuiltin<"__builtin_tvm_throwif">,
    Intrinsic<[], [llvm_i257_ty, llvm_i257_ty]>;
  def int_tvm_pdload :
    ClangBuiltin<"__builtin_tvm_pdload">,
    Intrinsic<[llvm_i257_ty], [llvm_i257_ty]>;
  def int_tvm_pdstore :
    ClangBuiltin<"__builtin_tvm_pdstore">,
    Intrinsic<[llvm_i257_ty], [llvm_i257_ty, llvm_i257_ty]>;
  def int_tvm_endc :
    ClangBuiltin<"__builtin_tvm_endc">,
    Intrinsic<[llvm_TVMCell_ty], [llvm_TVMBuilder_ty], [IntrNoMem]>;
  def int_tvm_sendrawmsg :
    ClangBuiltin<"__builtin_tvm_sendrawmsg">,
    Intrinsic<[], [llvm_TVMCell_ty, llvm_i257_ty], [IntrInaccessibleMemOnly]>;
  def int_tvm_retrieve_signed :
    ClangBuiltin<"__builtin_tvm_retrieve_signed">,
    Intrinsic<[llvm_i257_ty],
              [llvm_TVMSlice_ty, llvm_i257_ty, llvm_i257_ty]>;
  def int_tvm_retrieve_unsigned :
    ClangBuiltin<"__builtin_tvm_retrieve_unsigned">,
    Intrinsic<[llvm_i257_ty],
              [llvm_TVMSlice_ty, llvm_i257_ty, llvm_i257_ty]>;
  def int_tvm_retrieve_ref :
    ClangBuiltin<"__builtin_tvm_retrieve_ref">,
    Intrinsic<[llvm_TVMCell_ty], [llvm_TVMSlice_ty]>;
  def int_tvm_nop :
    ClangBuiltin<"__builtin_tvm_nop">,
    Intrinsic<[], []>;
  def int_tvm_dumpstk :
    ClangBuiltin<"__builtin_tvm_dumpstk">,
    Intrinsic<[], []>;
  def int_tvm_dumpstktop :
    ClangBuiltin<"__builtin_tvm_dumpstktop">,
    Intrinsic<[], [llvm_i257_ty]>;
  def int_tvm_dump :
    ClangBuiltin<"__builtin_tvm_dump">,
    Intrinsic<[], [llvm_i257_ty]>;
  def int_tvm_print :
    ClangBuiltin<"__builtin_tvm_print">,
    Intrinsic<[], [llvm_i257_ty]>;
  def int_tvm_dump_value :
    ClangBuiltin<"__builtin_tvm_dump_value">,
    Intrinsic<[], [llvm_i257_ty]>;
  def int_tvm_print_value :
    ClangBuiltin<"__builtin_tvm_print_value">,
    Intrinsic<[], [llvm_i257_ty]>;
  def int_tvm_logstr :
    ClangBuiltin<"__builtin_tvm_logstr">,
    Intrinsic<[], [llvm_ptr_ty]>;
  def int_tvm_printstr :
    ClangBuiltin<"__builtin_tvm_printstr">,
    Intrinsic<[], [llvm_ptr_ty]>;
  def int_tvm_logflush :
    ClangBuiltin<"__builtin_tvm_logflush">,
    Intrinsic<[], []>;
  def int_tvm_bbits :
    ClangBuiltin<"__builtin_tvm_bbits">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMBuilder_ty]>;
  def int_tvm_brefs :
    ClangBuiltin<"__builtin_tvm_brefs">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMBuilder_ty]>;

  // A.3 Tuple, list and null primitives
  def int_tvm_index :
    ClangBuiltin<"__builtin_tvm_index">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMTuple_ty, llvm_i257_ty], [IntrNoMem]>;
  def int_tvm_indexq :
    ClangBuiltin<"__builtin_tvm_indexq">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMTuple_ty, llvm_i257_ty], [IntrNoMem]>;
  def int_tvm_setindex :
    ClangBuiltin<"__builtin_tvm_setindex">,
    Intrinsic<[llvm_TVMTuple_ty],
              [llvm_TVMTuple_ty, llvm_i257_ty, llvm_i257_ty], [IntrNoMem]>;
  def int_tvm_setindexq :
    ClangBuiltin<"__builtin_tvm_setindexq">,
    Intrinsic<[llvm_TVMTuple_ty],
              [llvm_TVMTuple_ty, llvm_i257_ty, llvm_i257_ty], [IntrNoMem]>;
  def int_tvm_chktuple :
    ClangBuiltin<"__builtin_tvm_chktuple">,
    Intrinsic<[], [llvm_TVMTuple_ty], []>;
  def int_tvm_tlen:
    ClangBuiltin<"__builtin_tvm_tlen">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMTuple_ty], [IntrNoMem]>;
  def int_tvm_qtlen:
    ClangBuiltin<"__builtin_tvm_qtlen">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMTuple_ty], [IntrNoMem]>;
  def int_tvm_istuple:
    ClangBuiltin<"__builtin_tvm_istuple">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMTuple_ty], [IntrNoMem]>;
  def int_tvm_last:
    ClangBuiltin<"__builtin_tvm_last">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMTuple_ty], [IntrNoMem]>;
  def int_tvm_tpush:
    ClangBuiltin<"__builtin_tvm_tpush">,
    Intrinsic<[llvm_TVMTuple_ty], [llvm_TVMTuple_ty, llvm_i257_ty], [IntrNoMem]>;
  def int_tvm_tpop:
    ClangBuiltin<"__builtin_tvm_tpop">,
    Intrinsic<[llvm_TVMTuple_ty, llvm_i257_ty], [llvm_TVMTuple_ty], [IntrNoMem]>;

  def int_tvm_index2 :
    ClangBuiltin<"__builtin_tvm_index2">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMTuple_ty, llvm_i257_ty, llvm_i257_ty],
              [IntrNoMem]>;
  def int_tvm_index3 :
    ClangBuiltin<"__builtin_tvm_index3">,
    Intrinsic<[llvm_i257_ty],
              [llvm_TVMTuple_ty, llvm_i257_ty, llvm_i257_ty, llvm_i257_ty],
              [IntrNoMem]>;
  def int_tvm_cadr :
    ClangBuiltin<"__builtin_tvm_cadr">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMTuple_ty], [IntrNoMem]>;
  def int_tvm_cddr :
    ClangBuiltin<"__builtin_tvm_cddr">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMTuple_ty], [IntrNoMem]>;
  def int_tvm_caddr :
    ClangBuiltin<"__builtin_tvm_caddr">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMTuple_ty], [IntrNoMem]>;
  def int_tvm_cdddr :
    ClangBuiltin<"__builtin_tvm_cdddr">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMTuple_ty], [IntrNoMem]>;
  def int_tvm_isnull:
    ClangBuiltin<"__builtin_tvm_isnull">,
    Intrinsic<[llvm_i257_ty], [llvm_i257_ty], [IntrNoMem]>;
  def int_tvm_isnull_tuple:
    ClangBuiltin<"__builtin_tvm_isnull_tuple">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMTuple_ty], [IntrNoMem]>;
  def int_tvm_isnull_slice:
    ClangBuiltin<"__builtin_tvm_isnull_slice">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMSlice_ty], [IntrNoMem]>;
  def int_tvm_isnull_builder:
    ClangBuiltin<"__builtin_tvm_isnull_builder">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMBuilder_ty], [IntrNoMem]>;
  def int_tvm_isnull_cell:
    ClangBuiltin<"__builtin_tvm_isnull_cell">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMCell_ty], [IntrNoMem]>;

  // Control registers
  def int_tvm_get_persistent_data :
    ClangBuiltin<"__builtin_tvm_get_persistent_data">,
    Intrinsic<[llvm_TVMCell_ty], []>;
  def int_tvm_getreg :
    ClangBuiltin<"__builtin_tvm_getreg">,
    Intrinsic<[llvm_i257_ty], [llvm_i257_ty], [IntrNoMem]>;
  def int_tvm_setreg :
    ClangBuiltin<"__builtin_tvm_setreg">,
    Intrinsic<[], [llvm_i257_ty, llvm_i257_ty]>;
  def int_tvm_set_persistent_data :
    ClangBuiltin<"__builtin_tvm_set_persistent_data">,
    Intrinsic<[], [llvm_TVMCell_ty]>;
  def int_tvm_get_temporary_data :
    ClangBuiltin<"__builtin_tvm_get_temporary_data">,
    Intrinsic<[llvm_TVMTuple_ty], [], [IntrNoMem]>;

  // Cast intrinsics
  // We need them for receiving slice/builder/cell/tuple from tuple element
  def int_tvm_cast_to_tuple :
    ClangBuiltin<"__builtin_tvm_cast_to_tuple">,
    Intrinsic<[llvm_TVMTuple_ty], [llvm_i257_ty], [IntrNoMem]>;
  def int_tvm_cast_to_slice :
    ClangBuiltin<"__builtin_tvm_cast_to_slice">,
    Intrinsic<[llvm_TVMSlice_ty], [llvm_i257_ty], [IntrNoMem]>;
  def int_tvm_cast_to_builder :
    ClangBuiltin<"__builtin_tvm_cast_to_builder">,
    Intrinsic<[llvm_TVMBuilder_ty], [llvm_i257_ty], [IntrNoMem]>;
  def int_tvm_cast_to_cell :
    ClangBuiltin<"__builtin_tvm_cast_to_cell">,
    Intrinsic<[llvm_TVMCell_ty], [llvm_i257_ty], [IntrNoMem]>;
  def int_tvm_cast_from_tuple :
    ClangBuiltin<"__builtin_tvm_cast_from_tuple">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMTuple_ty], [IntrNoMem]>;
  def int_tvm_cast_from_slice :
    ClangBuiltin<"__builtin_tvm_cast_from_slice">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMSlice_ty], [IntrNoMem]>;
  def int_tvm_cast_from_builder :
    ClangBuiltin<"__builtin_tvm_cast_from_builder">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMBuilder_ty], [IntrNoMem]>;
  def int_tvm_cast_from_cell :
    ClangBuiltin<"__builtin_tvm_cast_from_cell">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMCell_ty], [IntrNoMem]>;

  def int_tvm_pushnull : ClangBuiltin<"__builtin_tvm_pushnull">,
    Intrinsic<[llvm_i257_ty], [], [IntrNoMem]>;

  def int_tvm_pushslice_empty : ClangBuiltin<"__builtin_tvm_pushslice_empty">,
    Intrinsic<[llvm_TVMSlice_ty], [], [IntrNoMem]>;

  def int_tvm_bless : ClangBuiltin<"__builtin_tvm_bless">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMSlice_ty], [IntrNoMem]>;

  def int_tvm_tuple : ClangBuiltin<"__builtin_tvm_tuple">,
    Intrinsic<[llvm_TVMTuple_ty], [llvm_vararg_ty], [IntrNoMem]>;

  foreach i = 1-255 in
  def int_tvm_untuple#i : ClangBuiltin<"__builtin_tvm_untuple"#i>,
    Intrinsic<!listsplat(llvm_i257_ty, i), [llvm_TVMTuple_ty], [IntrNoMem]>;

  foreach i = 1-255 in
  def int_tvm_unpackfirst#i : ClangBuiltin<"__builtin_tvm_unpackfirst"#i>,
    Intrinsic<!listsplat(llvm_i257_ty, i), [llvm_TVMTuple_ty], [IntrNoMem]>;

  def int_tvm_hiddenstack :
    ClangBuiltin<"__builtin_tvm_hiddenstack">,
    Intrinsic<[llvm_i257_ty], [llvm_i257_ty], [IntrNoMem]>;


// *********************************************************************** /

// *******************************************
//
// A.3  Tuple, List, and Null primitives
//
// ******************************************* /

  // NIL ( - t), pushes the only Tuple t = () of length zero
  def int_tvm_nil :
    ClangBuiltin<"__builtin_tvm_nil">,
    Intrinsic<[llvm_TVMTuple_ty], [], [IntrNoMem]>;

  // SINGLE (x - t), creates a singleton t := (x), i.e., a Tuple of length one
  def int_tvm_single :
    ClangBuiltin<"__builtin_tvm_single">,
    Intrinsic<[llvm_TVMTuple_ty], [llvm_i257_ty], [IntrNoMem]>;

  // PAIR or CONS (x y - t), creates pair t := (x, y)
  def int_tvm_pair :
    ClangBuiltin<"__builtin_tvm_pair">,
    Intrinsic<[llvm_TVMTuple_ty], [llvm_i257_ty, llvm_i257_ty], [IntrNoMem]>;

  // TRIPLE (x y z - t), creates triple t := (x, y, z)
  def int_tvm_triple :
    ClangBuiltin<"__builtin_tvm_triple">,
    Intrinsic<[llvm_TVMTuple_ty], [llvm_i257_ty, llvm_i257_ty, llvm_i257_ty], [IntrNoMem]>;

  // FIRST or CAR (t - x), returns the fist element of a Tuple
  def int_tvm_first :
    ClangBuiltin<"__builtin_tvm_first">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMTuple_ty], [IntrNoMem]>;

  // SECOND or CDR (t - y), returns the second element of a Tuple
  def int_tvm_second :
    ClangBuiltin<"__builtin_tvm_second">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMTuple_ty], [IntrNoMem]>;

  // THIRD (t - z), returns the third element of a Tuple
  def int_tvm_third :
    ClangBuiltin<"__builtin_tvm_third">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMTuple_ty], [IntrNoMem]>;

  // UNSINGLE (t - x), unpacks a singleton t = (x)
  def int_tvm_unsingle :
    ClangBuiltin<"__builtin_tvm_unsingle">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMTuple_ty], [IntrNoMem]>;

  // UNPAIR or UNCONS (t - x y), unpacks a pair t = (x, y)
  def int_tvm_unpair :
    ClangBuiltin<"__builtin_tvm_unpair">,
    Intrinsic<[llvm_i257_ty, llvm_i257_ty], [llvm_TVMTuple_ty], [IntrNoMem]>;

  // UNTRIPLE (t - x y z), unpacks a triple t = (x, y, z)
  def int_tvm_untriple :
    ClangBuiltin<"__builtin_tvm_untriple">,
    Intrinsic<[llvm_i257_ty, llvm_i257_ty, llvm_i257_ty], [llvm_TVMTuple_ty], [IntrNoMem]>;

  // EXPLODE n (t - x1. . . xmm), unpacks a Tuple t = (x1, . . . , xm) and returns its length m,
  //  but only if m <= n <= 15. Otherwise throws a type check exception
  // TODO

  // SETFIRST (t x - t0), sets the first component of Tuple t to x and returns the resulting Tuple t0
  def int_tvm_setfirst :
    ClangBuiltin<"__builtin_tvm_setfirst">,
    Intrinsic<[llvm_TVMTuple_ty], [llvm_TVMTuple_ty, llvm_i257_ty], [IntrNoMem]>;

  // SETSECOND (t x - t0), sets the second component of Tuple t to x and returns the resulting Tuple t0
  def int_tvm_setsecond :
    ClangBuiltin<"__builtin_tvm_setsecond">,
    Intrinsic<[llvm_TVMTuple_ty], [llvm_TVMTuple_ty, llvm_i257_ty], [IntrNoMem]>;

  // SETTHIRD (t x - t0), sets the third component of Tuple t to x and returns the resulting Tuple t0
  def int_tvm_setthird :
    ClangBuiltin<"__builtin_tvm_setthird">,
    Intrinsic<[llvm_TVMTuple_ty], [llvm_TVMTuple_ty, llvm_i257_ty], [IntrNoMem]>;

  // TUPLEVAR (x1. . . xnn - t), creates a new Tuple t of length n similarly to TUPLE, but with 0 <= n <= 255 taken from the stack
  // TODO

  // UNTUPLEVAR (t n - x1. . . xn), similar to UNTUPLE n, but with 0 <= n <= 255 taken from the stack
  // TODO

  // UNPACKFIRSTVAR (t n - x1. . . xn), similar to UNPACKFIRST n, but with 0 <= n <= 255 taken from the stack
  // TODO

  // EXPLODEVAR (t n - x1. . . xmm), similar to EXPLODE n, but with 0 <= n <= 255 taken from the stack
  // TODO

  // INDEXVARQ (t k - x), similar to INDEXQ n, but with 0 <= k <= 254 taken from the stack
  def int_tvm_indexvarq :
    ClangBuiltin<"__builtin_tvm_indexvarq">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMTuple_ty, llvm_i257_ty], [IntrNoMem]>;

  // SETINDEXVARQ (t x k - t0), similar to SETINDEXQ	k, but with 0 <= k <= 254 taken from the stack
  def int_tvm_setindexvarq :
    ClangBuiltin<"__builtin_tvm_setindexvarq">,
    Intrinsic<[llvm_TVMTuple_ty], [llvm_TVMTuple_ty, llvm_i257_ty, llvm_i257_ty], [IntrNoMem]>;

// *******************************************
//
// A.5.2. Division
//
// ******************************************* /

// DIVR (x y - q0:= bx/y + 1/2c)
  def int_tvm_divr :
    ClangBuiltin<"__builtin_tvm_divr">,
    Intrinsic<[llvm_i257_ty], [llvm_i257_ty, llvm_i257_ty],
              [IntrNoMem]>;

// DIVC (x y - q00:= dx/ye)
  def int_tvm_divc :
    ClangBuiltin<"__builtin_tvm_divc">,
    Intrinsic<[llvm_i257_ty], [llvm_i257_ty, llvm_i257_ty],
              [IntrNoMem]>;

// DIVMOD (x y - q r), where q := bx/yc, r := x - yq
  def int_tvm_divmod :
    ClangBuiltin<"__builtin_tvm_divmod">,
    Intrinsic<[llvm_i257_ty, llvm_i257_ty], [llvm_i257_ty, llvm_i257_ty], [IntrNoMem]>;

// DIVMODR (x y - q0 r0), where q0:= bx/y + 1/2c, r0:= x - yq0
  def int_tvm_divmodr :
    ClangBuiltin<"__builtin_tvm_divmodr">,
    Intrinsic<[llvm_i257_ty, llvm_i257_ty], [llvm_i257_ty, llvm_i257_ty], [IntrNoMem]>;

// DIVMODC	(x y - q00 r00), where q00:= dx/ye, r00:= x - yq00
  def int_tvm_divmodc :
    ClangBuiltin<"__builtin_tvm_divmodc">,
    Intrinsic<[llvm_i257_ty, llvm_i257_ty], [llvm_i257_ty, llvm_i257_ty], [IntrNoMem]>;

// MODPOW2 tt + 1: (x - x mod 2tt+1)
  def int_tvm_modpow2 :
    ClangBuiltin<"__builtin_tvm_modpow2">,
    Intrinsic<[llvm_i257_ty], [llvm_i257_ty, llvm_i257_ty],
              [IntrNoMem]>;

// MULDIVMOD (x y z - q r), where q := bx * y/zc, r := x * y mod z (same as * / MOD in Forth)
  def int_tvm_muldivmod :
    ClangBuiltin<"__builtin_tvm_muldivmod">,
    Intrinsic<[llvm_i257_ty, llvm_i257_ty], [llvm_i257_ty, llvm_i257_ty, llvm_i257_ty], [IntrNoMem]>;

// MULRSHIFT (x y z - bxy * 2-zc) for 0 <= z <= 256
  def int_tvm_mulrshift :
    ClangBuiltin<"__builtin_tvm_mulrshift">,
    Intrinsic<[llvm_i257_ty], [llvm_i257_ty, llvm_i257_ty, llvm_i257_ty],
              [IntrNoMem]>;

// MULRSHIFTR (x y z - bxy * 2-z+ 1/2c) for 0 <= z <= 256
  def int_tvm_mulrshiftr :
    ClangBuiltin<"__builtin_tvm_mulrshiftr">,
    Intrinsic<[llvm_i257_ty], [llvm_i257_ty, llvm_i257_ty, llvm_i257_ty],
              [IntrNoMem]>;

// MULRSHIFT tt + 1 (x y - bxy * 2-tt-1c)
  def int_tvm_mulrshift_tt :
    ClangBuiltin<"__builtin_tvm_mulrshift_tt">,
    Intrinsic<[llvm_i257_ty], [llvm_i257_ty, llvm_i257_ty, llvm_i257_ty],
              [IntrNoMem]>;

// MULRSHIFTR	tt + 1 (x y - bxy * 2-tt-1+ 1/2c)
  def int_tvm_mulrshiftr_tt :
    ClangBuiltin<"__builtin_tvm_mulrshiftr_tt">,
    Intrinsic<[llvm_i257_ty], [llvm_i257_ty, llvm_i257_ty, llvm_i257_ty],
              [IntrNoMem]>;

// LSHIFTDIV (x y z - b2zx/yc) for 0 <= z <= 256
  def int_tvm_lshiftdiv :
    ClangBuiltin<"__builtin_tvm_lshiftdiv">,
    Intrinsic<[llvm_i257_ty], [llvm_i257_ty, llvm_i257_ty, llvm_i257_ty],
              [IntrNoMem]>;

// LSHIFTDIVR (x y z - b2zx/y + 1/2c) for 0 <= z <= 256
  def int_tvm_lshiftdivr :
    ClangBuiltin<"__builtin_tvm_lshiftdivr">,
    Intrinsic<[llvm_i257_ty], [llvm_i257_ty, llvm_i257_ty, llvm_i257_ty],
              [IntrNoMem]>;

// LSHIFTDIV tt + 1 (x y - b2tt+1x/yc)
  def int_tvm_lshiftdiv_tt :
    ClangBuiltin<"__builtin_tvm_lshiftdiv_tt">,
    Intrinsic<[llvm_i257_ty], [llvm_i257_ty, llvm_i257_ty, llvm_i257_ty],
              [IntrNoMem]>;

// LSHIFTDIVR tt + 1 (x y - b2tt+1x/y+ 1/2c)
  def int_tvm_lshiftdivr_tt :
    ClangBuiltin<"__builtin_tvm_lshiftdivr_tt">,
    Intrinsic<[llvm_i257_ty], [llvm_i257_ty, llvm_i257_ty, llvm_i257_ty],
              [IntrNoMem]>;

// *******************************************
//
// A.5.3. Shifts, logical operations
//
// *******************************************/

  // POW2 (y - 2**y), 0 <= y <= 1023, equivalent to ONE; SWAP; LSHIFT
  def int_tvm_pow2 :
    ClangBuiltin<"__builtin_tvm_pow2">,
    Intrinsic<[llvm_i257_ty], [llvm_i257_ty], [IntrNoMem]>;

   // MINMAX or INTSORT2 (x y - x y or y x), sorts two integer
  def int_tvm_minmax :
    ClangBuiltin<"__builtin_tvm_minmax">,
    Intrinsic<[llvm_i257_ty, llvm_i257_ty], [llvm_i257_ty, llvm_i257_ty], [IntrNoMem]>;

// *******************************************
//
// A.5.4. Quiet arithmetic primitives
//
// ******************************************* /

  // QADD (x y - x + y), always works if x and y are	Integer s, but returns a NaN if the addition cannot be performed
  def int_tvm_qadd :
    ClangBuiltin<"__builtin_tvm_qadd">,
    Intrinsic<[llvm_i257_ty], [llvm_i257_ty, llvm_i257_ty],
              [IntrNoMem]>;

  // QDIV (x y - bx/yc), returns a NaN if y = 0, or if y = -1 and
  //   x = -2256, or if either of x or y is a NaN
  def int_tvm_qdiv :
    ClangBuiltin<"__builtin_tvm_qdiv">,
    Intrinsic<[llvm_i257_ty], [llvm_i257_ty, llvm_i257_ty],
              [IntrNoMem]>;

  // QAND (x y - x&y), bitwise "and" (similar to AND), but returns a NaN if either x or y is a NaN
  def int_tvm_qand :
    ClangBuiltin<"__builtin_tvm_qand">,
    Intrinsic<[llvm_i257_ty], [llvm_i257_ty, llvm_i257_ty],
              [IntrNoMem]>;

  // QOR (x y - x|y), bitwise "or". If  x = -1 or y = -1, the result is always -1,
  //  even if the other argument is a NaN
  def int_tvm_qor :
    ClangBuiltin<"__builtin_tvm_qor">,
    Intrinsic<[llvm_i257_ty], [llvm_i257_ty, llvm_i257_ty],
              [IntrNoMem]>;

// *******************************************
//
// A.6	Comparison primitives
//
// ******************************************* /

  // SGN (x - sgn(x)), computes the sign of an integer x:	-1 if x < 0, 0 if x = 0, 1 if x > 0
  def int_tvm_sgn :
      ClangBuiltin<"__builtin_tvm_sgn">,
      Intrinsic<[llvm_i257_ty], [llvm_i257_ty], [IntrNoMem]>;

  // ISNAN (x - x = NaN), checks whether x is a NaN
  def int_tvm_isnan :
      ClangBuiltin<"__builtin_tvm_isnan">,
      Intrinsic<[llvm_i257_ty], [llvm_i257_ty], [IntrNoMem]>;

  // CHKNAN (x - x), throws an arithmetic overflow exception if x is a NaN
  def int_tvm_chknan :
      ClangBuiltin<"__builtin_tvm_chknan">,
      Intrinsic<[llvm_i257_ty], [llvm_i257_ty], [IntrNoMem]>;

// *******************************************
//
//       A.7        Cell primitives
//
// ******************************************* /

  // STBREFR or ENDCST (b b00 - b), equivalent to ENDC; SWAP; STREF
  def int_tvm_endcst :
    ClangBuiltin<"__builtin_tvm_endcst">,
    Intrinsic<[llvm_TVMBuilder_ty],
              [llvm_TVMBuilder_ty, llvm_TVMBuilder_ty], [IntrNoMem]>;

  // TODO
  // STIXR  (b x l - b0),  similar to STIX, but with arguments in a different order
  //def int_tvm_stixr :
  //  ClangBuiltin<"__builtin_tvm_stixr">,
  //  Intrinsic<[llvm_TVMBuilder_ty], [llvm_TVMBuilder_ty, llvm_i257_ty, llvm_i257_ty],
  //            [IntrNoMem]>;

  // STUXR  (b x l - b0),  similar to STUX, but with arguments in a different order
  //def int_tvm_stuxr :
  //  ClangBuiltin<"__builtin_tvm_stuxr">,
  //  Intrinsic<[llvm_TVMBuilder_ty], [llvm_TVMBuilder_ty, llvm_i257_ty, llvm_i257_ty],
  //            [IntrNoMem]>;

  // STIR cc + 1 (b x - b0), equivalent to SWAP; STI cc + 1
  def int_tvm_stir :
    ClangBuiltin<"__builtin_tvm_stir">,
    Intrinsic<[llvm_TVMBuilder_ty], [llvm_TVMBuilder_ty, llvm_i257_ty, llvm_i257_ty],
              [IntrNoMem]>;

  // STUR cc + 1 (b x - b0), equivalent to SWAP; STU cc + 1
  def int_tvm_stur :
    ClangBuiltin<"__builtin_tvm_stur">,
    Intrinsic<[llvm_TVMBuilder_ty], [llvm_TVMBuilder_ty, llvm_i257_ty, llvm_i257_ty],
              [IntrNoMem]>;

  // STBREF (b0 b - b00), equivalent to SWAP; STBREFREV
  def int_tvm_stbref :
    ClangBuiltin<"__builtin_tvm_stbref">,
    Intrinsic<[llvm_TVMBuilder_ty],
              [llvm_TVMBuilder_ty, llvm_TVMBuilder_ty], [IntrNoMem]>;

  // STB (b0 b - b00), appends all data from Builder b0 to Builder b
  def int_tvm_stb :
    ClangBuiltin<"__builtin_tvm_stb">,
    Intrinsic<[llvm_TVMBuilder_ty],
              [llvm_TVMBuilder_ty, llvm_TVMBuilder_ty], [IntrNoMem]>;

  // STREFR (b c - b0)
  def int_tvm_strefr :
    ClangBuiltin<"__builtin_tvm_strefr">,
    Intrinsic<[llvm_TVMBuilder_ty],
              [llvm_TVMBuilder_ty, llvm_TVMCell_ty], [IntrNoMem]>;

  // STSLICER (b s - b0)
  def int_tvm_stslicer :
    ClangBuiltin<"__builtin_tvm_stslicer">,
    Intrinsic<[llvm_TVMBuilder_ty],
              [llvm_TVMBuilder_ty, llvm_TVMSlice_ty], [IntrNoMem]>;

  // STBR (b b0 - b00), concatenates two Builder s, equivalent to SWAP; STB
  def int_tvm_stbr :
    ClangBuiltin<"__builtin_tvm_stbr">,
    Intrinsic<[llvm_TVMBuilder_ty],
              [llvm_TVMBuilder_ty, llvm_TVMBuilder_ty], [IntrNoMem]>;

  // STREFQ	(c b - c b -1 or b0 0)
  def int_tvm_strefq :
    ClangBuiltin<"__builtin_tvm_strefq">,
    Intrinsic<[llvm_TVMCell_ty, llvm_TVMBuilder_ty, llvm_i257_ty],
              [llvm_TVMCell_ty, llvm_TVMBuilder_ty], [IntrNoMem]>;

  // STBREFQ (b0 b - b0 b -1 or b00 0)
  def int_tvm_stbrefq :
    ClangBuiltin<"__builtin_tvm_stbrefq">,
    Intrinsic<[llvm_TVMBuilder_ty, llvm_TVMBuilder_ty, llvm_i257_ty],
              [llvm_TVMBuilder_ty, llvm_TVMBuilder_ty], [IntrNoMem]>;

  // STSLICEQ (s b - s b -1 or b0 0)
  def int_tvm_stsliceq :
    ClangBuiltin<"__builtin_tvm_stsliceq">,
    Intrinsic<[llvm_TVMSlice_ty, llvm_TVMBuilder_ty, llvm_i257_ty],
              [llvm_TVMSlice_ty, llvm_TVMBuilder_ty], [IntrNoMem]>;

  // STBQ (b0 b - b0 b -1 or b00 0)
  def int_tvm_stbq :
    ClangBuiltin<"__builtin_tvm_stbq">,
    Intrinsic<[llvm_TVMBuilder_ty, llvm_TVMBuilder_ty, llvm_i257_ty],
              [llvm_TVMBuilder_ty, llvm_TVMBuilder_ty], [IntrNoMem]>;

  // STBREFRQ (b b0 - b b0 -1 or b00 0)
  def int_tvm_stbrefrq :
    ClangBuiltin<"__builtin_tvm_stbrefrq">,
    Intrinsic<[llvm_TVMBuilder_ty, llvm_TVMBuilder_ty, llvm_i257_ty],
              [llvm_TVMBuilder_ty, llvm_TVMBuilder_ty], [IntrNoMem]>;

  // STSLICERQ (b s - b s -1 or b00 0)
  def int_tvm_stslicerq :
    ClangBuiltin<"__builtin_tvm_stslicerq">,
    Intrinsic<[llvm_TVMBuilder_ty, llvm_TVMSlice_ty, llvm_i257_ty],
              [llvm_TVMBuilder_ty, llvm_TVMSlice_ty], [IntrNoMem]>;

  // STBRQ (b b0 - b b0 -1 or b00 0)
  def int_tvm_stbrq :
    ClangBuiltin<"__builtin_tvm_stbrq">,
    Intrinsic<[llvm_TVMBuilder_ty, llvm_TVMBuilder_ty, llvm_i257_ty],
              [llvm_TVMBuilder_ty, llvm_TVMBuilder_ty], [IntrNoMem]>;


  // STREFCONST, equivalent to PUSHREF; STREFR
  // TODO

  // STREF2CONST, equivalent to STREFCONST; STREFCONST
  // TODO


  // ENDXC  (b x - c), if x6 = 0, creates a special or exotic cell from Builder b
  def int_tvm_endxc :
    ClangBuiltin<"__builtin_tvm_endxc">,
    Intrinsic<[llvm_TVMCell_ty], [llvm_TVMBuilder_ty, llvm_i257_ty], [IntrNoMem]>;

// STILE4 (x b - b0), stores a little-endian signed 32-bit integer
  def int_tvm_stile4 :
    ClangBuiltin<"__builtin_tvm_stile4">,
    Intrinsic<[llvm_TVMBuilder_ty],
              [llvm_i257_ty, llvm_TVMBuilder_ty], [IntrNoMem]>;

  // STULE4 (x b - b0), stores a little-endian unsigned 32-bit integer
    def int_tvm_stule4 :
    ClangBuiltin<"__builtin_tvm_stule4">,
    Intrinsic<[llvm_TVMBuilder_ty],
              [llvm_i257_ty, llvm_TVMBuilder_ty], [IntrNoMem]>;

  // STILE8 (x b - b0), stores a little-endian signed 64-bit integer
  def int_tvm_stile8 :
    ClangBuiltin<"__builtin_tvm_stile8">,
    Intrinsic<[llvm_TVMBuilder_ty],
              [llvm_i257_ty, llvm_TVMBuilder_ty], [IntrNoMem]>;

  // STULE8 (x b - b0), stores a little-endian unsigned 64-bit integer
  def int_tvm_stule8 :
    ClangBuiltin<"__builtin_tvm_stule8">,
    Intrinsic<[llvm_TVMBuilder_ty],
              [llvm_i257_ty, llvm_TVMBuilder_ty], [IntrNoMem]>;

  // BDEPTH  (b - x), returns the depth of Builder b. If no cell references are stored in b, then x =  0;
  // otherwise x is one plus the maximum of depths of cells referred to from b
  def int_tvm_bdepth :
    ClangBuiltin<"__builtin_tvm_bdepth">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMBuilder_ty]>;

  // BBITREFS (b - x y), returns the numbers of both data bits and cell references in b
  def int_tvm_bbitrefs :
    ClangBuiltin<"__builtin_tvm_bbitrefs">,
    Intrinsic<[llvm_i257_ty, llvm_i257_ty], [llvm_TVMBuilder_ty]>;

  // BREMBITREFS  (b - x0 y0)
  def int_tvm_brembitrefs :
    ClangBuiltin<"__builtin_tvm_brembitrefs">,
    Intrinsic<[llvm_i257_ty, llvm_i257_ty], [llvm_TVMBuilder_ty]>;

  // BCHKBITS cc + 1 (b -), checks whether cc + 1 bits can be stored into b, where 0 <= cc <= 255
  // TODO

  // BCHKBITS (b x - ), checks whether x bits can be stored into b, 0 <= x <= 1023. If there is no space for x more bits in b ... throws an exception
  def int_tvm_bchkbits :
    ClangBuiltin<"__builtin_tvm_bchkbits">,
    Intrinsic<[], [llvm_TVMBuilder_ty, llvm_i257_ty]>;

  // BCHKREFS (b y - ), checks whether y references can be stored into b, 0 <= y <= 7
  def int_tvm_bchkrefs :
    ClangBuiltin<"__builtin_tvm_bchkrefs">,
    Intrinsic<[], [llvm_TVMBuilder_ty, llvm_i257_ty]>;

  // BCHKBITREFS (b x y - ), checks whether x bits and y references can be stored into b, 0 <= x <= 1023, 0 <= y <= 7
  def int_tvm_bchkbitrefs :
    ClangBuiltin<"__builtin_tvm_bchkbitrefs">,
    Intrinsic<[], [llvm_TVMBuilder_ty, llvm_i257_ty, llvm_i257_ty]>;

  // BCHKBITSQ cc + 1 (b - ?), checks whether cc + 1 bits can be stored into b, where 0 <= cc <= 255
  // TODO

  // BCHKBITSQ (b x - ?), checks whether x bits can be stored into b, 0 <= x <= 1023
  def int_tvm_bchkbitsq :
    ClangBuiltin<"__builtin_tvm_bchkbitsq">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMBuilder_ty, llvm_i257_ty]>;

  // BCHKREFSQ (b y - ?), checks whether y references can be stored into b, 0 <= y <= 7
  def int_tvm_bchkrefsq :
    ClangBuiltin<"__builtin_tvm_bchkrefsq">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMBuilder_ty, llvm_i257_ty]>;

  // BCHKBITREFSQ (b x y - ?), checks whether x bits and y references can be stored into b, 0 <= x <= 1023, 0 <= y <= 7
  def int_tvm_bchkbitrefsq :
    ClangBuiltin<"__builtin_tvm_bchkbitrefsq">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMBuilder_ty, llvm_i257_ty, llvm_i257_ty]>;

  // STZEROES (b n - b0), stores n binary zeroes into Builder b
  def int_tvm_stzeroes :
    ClangBuiltin<"__builtin_tvm_stzeroes">,
    Intrinsic<[llvm_TVMBuilder_ty], [llvm_TVMBuilder_ty, llvm_i257_ty]>;

  // STONES (b n - b0), stores n binary ones into Builder b
  def int_tvm_stones :
    ClangBuiltin<"__builtin_tvm_stones">,
    Intrinsic<[llvm_TVMBuilder_ty], [llvm_TVMBuilder_ty, llvm_i257_ty]>;

  // STSAME (b n x - b0), stores n binary xes (0 <= x <= 1) into Builder b
  def int_tvm_stsame :
    ClangBuiltin<"__builtin_tvm_stsame">,
    Intrinsic<[llvm_TVMBuilder_ty], [llvm_TVMBuilder_ty, llvm_i257_ty, llvm_i257_ty]>;

  // STSLICECONST sss (b -	b0), stores a constant subslice sss consisting of 0 <= x <= 3
  //  references and up to 8y + 1 data bits, with 0 <= y <= 7.
  // TODO

  // STSLICECONST '0' or STZERO (b - b0), stores one binary zero
  def int_tvm_stzero :
    ClangBuiltin<"__builtin_tvm_stzero">,
    Intrinsic<[llvm_TVMBuilder_ty], [llvm_TVMBuilder_ty]>;

  // STSLICECONST '1' or STONE (b - b0), stores one binary one
  def int_tvm_stone :
    ClangBuiltin<"__builtin_tvm_stone">,
    Intrinsic<[llvm_TVMBuilder_ty], [llvm_TVMBuilder_ty]>;

// *******************************************
//
// A.7.2. Cell deserialization primitives
//
// ******************************************* /

  // LDIQ cc + 1 (s - x s0 -1 or s 0), a quiet version of LDI
  def int_tvm_ldiq :
    ClangBuiltin<"__builtin_tvm_ldiq">,
    Intrinsic<[llvm_i257_ty, llvm_TVMSlice_ty, llvm_i257_ty], [llvm_TVMSlice_ty, llvm_i257_ty]>;

  // PLDIQ cc + 1 (s - x -1 or 0), a quiet version of PLDI
  def int_tvm_pldiq :
    ClangBuiltin<"__builtin_tvm_pldiq">,
    Intrinsic<[llvm_i257_ty, llvm_i257_ty], [llvm_TVMSlice_ty, llvm_i257_ty]>;

  // PLDUQ cc + 1 (s - x -1 or 0), a quiet version of PLDU
  def int_tvm_plduq :
    ClangBuiltin<"__builtin_tvm_plduq">,
    Intrinsic<[llvm_i257_ty, llvm_i257_ty], [llvm_TVMSlice_ty, llvm_i257_ty]>;

  // PLDUZ 32(c + 1) (s - s x), preloads the first 32(c + 1) bits of Slice s into an unsigned integer x, for 0 <= c <= 7
  def int_tvm_plduz :
    ClangBuiltin<"__builtin_tvm_plduz">,
    Intrinsic<[llvm_TVMSlice_ty, llvm_i257_ty], [llvm_TVMSlice_ty, llvm_i257_ty]>;

  // LDSLICEXQ (s l - s00 s0 -1 or s 0), a quiet version of LDSLICEX
  def int_tvm_ldslicexq :
    ClangBuiltin<"__builtin_tvm_ldslicexq">,
    Intrinsic<[llvm_TVMSlice_ty, llvm_TVMSlice_ty, llvm_i257_ty], [llvm_TVMSlice_ty, llvm_i257_ty]>;

  // PLDSLICEXQ (s l - s0 -1 or 0), a quiet version of LDSLICEXQ
  def int_tvm_pldslicexq :
    ClangBuiltin<"__builtin_tvm_pldslicexq">,
    Intrinsic<[llvm_TVMSlice_ty, llvm_i257_ty], [llvm_TVMSlice_ty, llvm_i257_ty]>;

  // PLDSLICE cc + 1 (s - s00), returns the first 0 < cc + 1 <= 256 bits of s as s00
  def int_tvm_pldslice :
    ClangBuiltin<"__builtin_tvm_pldslice">,
    Intrinsic<[llvm_TVMSlice_ty], [llvm_TVMSlice_ty, llvm_i257_ty]>;

  // LDSLICEQ cc + 1 (s - s00 s0 -1 or s 0), a quiet version of LDSLICE
  def int_tvm_ldsliceq :
    ClangBuiltin<"__builtin_tvm_ldsliceq">,
    Intrinsic<[llvm_TVMSlice_ty, llvm_TVMSlice_ty, llvm_i257_ty], [llvm_TVMSlice_ty, llvm_i257_ty]>;

  // PLDSLICEQ cc + 1 (s - s00 -1 or 0), a quiet version of PLDSLICE
  def int_tvm_pldsliceq :
    ClangBuiltin<"__builtin_tvm_pldsliceq">,
    Intrinsic<[llvm_TVMSlice_ty, llvm_i257_ty], [llvm_TVMSlice_ty, llvm_i257_ty]>;

  // SDCUTFIRST (s l - s0), returns the first 0 <= l <= 1023 bits of s. It is equivalent to PLDSLICEX
  def int_tvm_sdcutfirst :
    ClangBuiltin<"__builtin_tvm_sdcutfirst">,
    Intrinsic<[llvm_TVMSlice_ty], [llvm_TVMSlice_ty, llvm_i257_ty]>;

  // SDSKIPFIRST (s l - s0), returns all but the first 0 <= l <= 1023 bits of s. It is equivalent to LDSLICEX; NIP
  def int_tvm_sdskipfirst :
    ClangBuiltin<"__builtin_tvm_sdskipfirst">,
    Intrinsic<[llvm_TVMSlice_ty], [llvm_TVMSlice_ty, llvm_i257_ty]>;

  // SDCUTLAST (s l - s0), returns the last 0 <= l <= 1023 bits of s
  def int_tvm_sdcutlast :
    ClangBuiltin<"__builtin_tvm_sdcutlast">,
    Intrinsic<[llvm_TVMSlice_ty], [llvm_TVMSlice_ty, llvm_i257_ty]>;

  // SDSKIPLAST (s l - s0), returns all but the last 0 <= l <= 1023 bits of s
  def int_tvm_sdskiplast :
    ClangBuiltin<"__builtin_tvm_sdskiplast">,
    Intrinsic<[llvm_TVMSlice_ty], [llvm_TVMSlice_ty, llvm_i257_ty]>;

  // SDSUBSTR (s l l0 - s0), returns 0 <= l <= 1023 bits of s starting from offset 0 <= l <= 1023,
  //  thus extracting a bit substring out of the data of s
  def int_tvm_sdsubstr :
    ClangBuiltin<"__builtin_tvm_sdsdsubstr">,
    Intrinsic<[llvm_TVMSlice_ty], [llvm_TVMSlice_ty, llvm_i257_ty, llvm_i257_ty]>;

  // SDBEGINSX (s s0 - s00), checks whether s begins with (the data bits of) s0, and removes s0 from s	on success.
  //  On failure throws a cell deserialization exception
  def int_tvm_sdbeginsx :
    ClangBuiltin<"__builtin_tvm_sdbeginsx">,
    Intrinsic<[llvm_TVMSlice_ty], [llvm_TVMSlice_ty, llvm_TVMSlice_ty]>;

  // SDBEGINSXQ (s s0 - s00 -1 or s 0), a quiet version of SDBEGINSX
  def int_tvm_sdbeginsxq :
    ClangBuiltin<"__builtin_tvm_sdbeginsxq">,
    Intrinsic<[llvm_TVMSlice_ty, llvm_i257_ty], [llvm_TVMSlice_ty, llvm_TVMSlice_ty]>;

  // SDBEGINS (s - s00), checks whether s begins with constant bitstring sss of length 8x + 3
  //   (with continuation bit assumed), where 0 <= x <= 127, and removes sss from s on success
  def int_tvm_sdbegins :
    ClangBuiltin<"__builtin_tvm_sdbegins">,
    Intrinsic<[llvm_TVMSlice_ty], [llvm_TVMSlice_ty, llvm_i257_ty]>;

  // SDBEGINS '0' (s - s00), checks whether s begins with a binary zero
  def int_tvm_sdbegins0 :
    ClangBuiltin<"__builtin_tvm_sdbegins0">,
    Intrinsic<[llvm_TVMSlice_ty], [llvm_TVMSlice_ty]>;

  // SDBEGINS '1' (s - s00), checks whether s begins with a binary one
  def int_tvm_sdbegins1 :
    ClangBuiltin<"__builtin_tvm_sdbegins1">,
    Intrinsic<[llvm_TVMSlice_ty], [llvm_TVMSlice_ty]>;

  // SDBEGINSQ (s - s00 -1 or s 0), a quiet version of SDBEGINS
  def int_tvm_sdbeginsq :
    ClangBuiltin<"__builtin_tvm_sdbeginsq">,
    Intrinsic<[llvm_TVMSlice_ty], [llvm_TVMSlice_ty, llvm_i257_ty]>;

  // SCUTFIRST (s l r - s0), returns the first 0 <= l <= 1023 bits and first 0 <= r <= 4 references of s
  def int_tvm_scutfirst :
    ClangBuiltin<"__builtin_tvm_scutfirst">,
    Intrinsic<[llvm_TVMSlice_ty], [llvm_TVMSlice_ty, llvm_i257_ty, llvm_i257_ty]>;

  // SSKIPFIRST (s l r - s0)
  def int_tvm_sskipfirst :
    ClangBuiltin<"__builtin_tvm_sskipfirst">,
    Intrinsic<[llvm_TVMSlice_ty], [llvm_TVMSlice_ty, llvm_i257_ty, llvm_i257_ty]>;

  // SCUTLAST (s l r - s0), returns the last 0 <= l <= 1023 data bits and last 0 <= r <= 4 references of s
  def int_tvm_scutlast :
    ClangBuiltin<"__builtin_tvm_scutlast">,
    Intrinsic<[llvm_TVMSlice_ty], [llvm_TVMSlice_ty, llvm_i257_ty, llvm_i257_ty]>;

  // SSKIPLAST (s l r - s0)
  def int_tvm_sskiplast :
    ClangBuiltin<"__builtin_tvm_sskiplast">,
    Intrinsic<[llvm_TVMSlice_ty], [llvm_TVMSlice_ty, llvm_i257_ty, llvm_i257_ty]>;

  // SUBSLICE (s l r l0 r0 - s0), returns 0 <= l0 <= 1023 bits and 0 <= r0 <= 4 references from Slice s
  def int_tvm_subslice :
    ClangBuiltin<"__builtin_tvm_subslice">,
    Intrinsic<[llvm_TVMSlice_ty], [llvm_TVMSlice_ty, llvm_i257_ty, llvm_i257_ty, llvm_i257_ty, llvm_i257_ty]>;

  // SPLIT (s l r - s0 s00), splits the first 0 <= l <= 1023 data bits and first 0 <= r <= 4
  //  references from s into s0, returning the remainder of s as s00
  def int_tvm_split :
    ClangBuiltin<"__builtin_tvm_split">,
    Intrinsic<[llvm_TVMSlice_ty, llvm_TVMSlice_ty], [llvm_TVMSlice_ty, llvm_i257_ty, llvm_i257_ty]>;

  // SPLITQ (s l r - s0 s00 -1 or s 0), a quiet version of SPLIT
  def int_tvm_splitq :
    ClangBuiltin<"__builtin_tvm_splitq">,
    Intrinsic<[llvm_TVMSlice_ty, llvm_TVMSlice_ty, llvm_i257_ty], [llvm_TVMSlice_ty, llvm_i257_ty, llvm_i257_ty]>;

  // XCTOS (c - s ?), transforms an ordinary or exotic cell into a Slice, as if it were an ordinary cell.
  //  A flag is returned indicating whether c is exotic
  def int_tvm_xctos :
    ClangBuiltin<"__builtin_tvm_xctos">,
    Intrinsic<[llvm_TVMSlice_ty, llvm_i257_ty], [llvm_TVMCell_ty]>;

  // XLOAD (c - c0), loads an exotic cell c and returns an ordinary cell c0. If c is already ordinary, does nothing.
  //  If c cannot be loaded, throws an exception
  def int_tvm_xload :
    ClangBuiltin<"__builtin_tvm_xload">,
    Intrinsic<[llvm_TVMCell_ty], [llvm_TVMCell_ty]>;

  // XLOADQ (c - c0 -1 or c 0), loads an exotic cell c as XLOAD, but returns 0 on failure
  def int_tvm_xloadq :
    ClangBuiltin<"__builtin_tvm_xloadq">,
    Intrinsic<[llvm_TVMCell_ty, llvm_i257_ty], [llvm_TVMCell_ty]>;

  // SCHKBITS (s l - ), checks whether there are at least l data bits in Slice s. If this is not the case,
  //  throws a cell deserialisation (i.e., cell underflow) exception
  def int_tvm_schkbits :
    ClangBuiltin<"__builtin_tvm_schkbits">,
    Intrinsic<[], [llvm_TVMSlice_ty, llvm_i257_ty]>;

  // SCHKREFS (s r - ), checks whether there are at least r references in Slice s
  def int_tvm_schkrefs :
    ClangBuiltin<"__builtin_tvm_schkrefs">,
    Intrinsic<[], [llvm_TVMSlice_ty, llvm_i257_ty]>;

  // SCHKBITREFS (s l r - ), checks whether there are at least l data bits and r references in Slice s
  def int_tvm_schkbitrefs :
    ClangBuiltin<"__builtin_tvm_schkbitrefs">,
    Intrinsic<[], [llvm_TVMSlice_ty, llvm_i257_ty, llvm_i257_ty]>;

  // SCHKBITSQ (s l - ?), checks whether there are at least l data bits in Slice s
  def int_tvm_schkbitsq :
    ClangBuiltin<"__builtin_tvm_schkbitsq">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMSlice_ty, llvm_i257_ty]>;

  // SCHKREFSQ (s r - ?), checks whether there are at least r references in Slice s
  def int_tvm_schkrefsq :
    ClangBuiltin<"__builtin_tvm_schkrefsq">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMSlice_ty, llvm_i257_ty]>;

  // SCHKBITREFSQ (s l r - ?), checks whether there are at least l data bits and r references in Slice s
  def int_tvm_schkbitrefsq :
    ClangBuiltin<"__builtin_tvm_schkbitrefsq">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMSlice_ty, llvm_i257_ty, llvm_i257_ty]>;

  // PLDREFVAR (s n - c), returns the n-th cell reference of Slice s for 0 <= n <= 3
  def int_tvm_pldrefvar :
    ClangBuiltin<"__builtin_tvm_pldrefvar">,
    Intrinsic<[llvm_TVMCell_ty], [llvm_TVMSlice_ty, llvm_i257_ty]>;

  // PLDREFIDX n (s - c), returns the n-th cell reference of Slice s, where 0 <= n <= 3
  def int_tvm_pldrefidx :
    ClangBuiltin<"__builtin_tvm_pldrefidx">,
    Intrinsic<[llvm_TVMCell_ty], [llvm_TVMSlice_ty, llvm_i257_ty]>;

  // PLDREF (s - c), preloads the first cell reference of a Slice
  def int_tvm_pldref :
    ClangBuiltin<"__builtin_tvm_pldref">,
    Intrinsic<[llvm_TVMCell_ty], [llvm_TVMSlice_ty]>;

  // LDILE4 (s - x s0), loads a little-endian signed 32-bit integer
  def int_tvm_ldile4 :
    ClangBuiltin<"__builtin_tvm_ldile4">,
    Intrinsic<[llvm_i257_ty, llvm_TVMSlice_ty], [llvm_TVMSlice_ty]>;

  // LDULE4 (s - x s0), loads a little-endian unsigned 32-bit integer
  def int_tvm_ldule4 :
    ClangBuiltin<"__builtin_tvm_ldule4">,
    Intrinsic<[llvm_i257_ty, llvm_TVMSlice_ty], [llvm_TVMSlice_ty]>;

  // LDILE8 (s - x s0), loads a little-endian signed 64-bit integer
  def int_tvm_ldile8 :
    ClangBuiltin<"__builtin_tvm_ldile8">,
    Intrinsic<[llvm_i257_ty, llvm_TVMSlice_ty], [llvm_TVMSlice_ty]>;

  // LDULE8 (s - x s0), loads a little-endian unsigned 64-bit integer
  def int_tvm_ldule8 :
    ClangBuiltin<"__builtin_tvm_ldule8">,
    Intrinsic<[llvm_i257_ty, llvm_TVMSlice_ty], [llvm_TVMSlice_ty]>;

  // PLDILE4 (s - x), preloads a little-endian signed 32-bit integer
  def int_tvm_pldile4 :
    ClangBuiltin<"__builtin_tvm_pldile4">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMSlice_ty]>;

  // PLDULE4 (s - x), preloads a little-endian unsigned 32-bit integer
  def int_tvm_pldule4 :
    ClangBuiltin<"__builtin_tvm_pldule4">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMSlice_ty]>;

  // PLDILE8 (s - x), preloads a little-endian signed 64-bit integer
  def int_tvm_pldile8 :
    ClangBuiltin<"__builtin_tvm_pldile8">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMSlice_ty]>;

  // PLDULE8 (s - x), preloads a little-endian unsigned 64-bit integer
  def int_tvm_pldule8 :
    ClangBuiltin<"__builtin_tvm_pldule8">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMSlice_ty]>;

  // LDILE4Q (s - x s0 -1 or s 0), quietly loads a little-endian signed 32-bit integer
  def int_tvm_ldile4q :
    ClangBuiltin<"__builtin_tvm_ldile4q">,
    Intrinsic<[llvm_i257_ty, llvm_TVMSlice_ty, llvm_i257_ty], [llvm_TVMSlice_ty]>;

  // LDULE4Q (s - x s0 -1 or s 0), quietly loads a little-endian unsigned 32-bit integer
  def int_tvm_ldule4q :
    ClangBuiltin<"__builtin_tvm_ldule4q">,
    Intrinsic<[llvm_i257_ty, llvm_TVMSlice_ty, llvm_i257_ty], [llvm_TVMSlice_ty]>;

  // LDILE8Q (s - x s0 -1 or s 0), quietly loads a little-endian signed 64-bit integer
  def int_tvm_ldile8q :
    ClangBuiltin<"__builtin_tvm_ldile8q">,
    Intrinsic<[llvm_i257_ty, llvm_TVMSlice_ty, llvm_i257_ty], [llvm_TVMSlice_ty]>;

  // LDULE8Q (s - x s0 -1 or	s 0), quietly loads a little-endian unsigned 64-bit integer
  def int_tvm_ldule8q :
    ClangBuiltin<"__builtin_tvm_ldule8q">,
    Intrinsic<[llvm_i257_ty, llvm_TVMSlice_ty, llvm_i257_ty], [llvm_TVMSlice_ty]>;

  // PLDILE4Q (s - x -1 or 0), quietly preloads a little-endian signed 32-bit integer
  def int_tvm_pldile4q :
    ClangBuiltin<"__builtin_tvm_pldile4q">,
    Intrinsic<[llvm_i257_ty, llvm_i257_ty], [llvm_TVMSlice_ty]>;

  // PLDULE4Q (s - x -1 or	0), quietly preloads a little-endian unsigned 32-bit integer
  def int_tvm_pldule4q :
    ClangBuiltin<"__builtin_tvm_pldule4q">,
    Intrinsic<[llvm_i257_ty, llvm_i257_ty], [llvm_TVMSlice_ty]>;

  // PLDILE8Q (s - x -1 or 0), quietly preloads a little-endian signed 64-bit integer
  def int_tvm_pldile8q :
    ClangBuiltin<"__builtin_tvm_pldile8q">,
    Intrinsic<[llvm_i257_ty, llvm_i257_ty], [llvm_TVMSlice_ty]>;

  // PLDULE8Q (s -	x -1 or 0), quietly preloads a little-endian unsigned 64-bit integer
  def int_tvm_pldule8q :
    ClangBuiltin<"__builtin_tvm_pldule8q">,
    Intrinsic<[llvm_i257_ty, llvm_i257_ty], [llvm_TVMSlice_ty]>;

  // LDZEROES (s - n s0), returns the count n of leading zero bits in s, and removes these bits from s
  def int_tvm_ldzeroes :
    ClangBuiltin<"__builtin_tvm_ldzeroes">,
    Intrinsic<[llvm_i257_ty, llvm_TVMSlice_ty], [llvm_TVMSlice_ty]>;

  // LDONES (s - n s0), returns the count n of leading one bits in s, and removes these bits from s
  def int_tvm_ldones :
    ClangBuiltin<"__builtin_tvm_ldones">,
    Intrinsic<[llvm_i257_ty, llvm_TVMSlice_ty], [llvm_TVMSlice_ty]>;

  // LDSAME (s x - n s0), returns the count	n of leading bits equal to 0 <= x <= 1 in s, and removes these bits from s
  def int_tvm_ldsame :
    ClangBuiltin<"__builtin_tvm_ldsame">,
    Intrinsic<[llvm_i257_ty, llvm_TVMSlice_ty], [llvm_TVMSlice_ty, llvm_i257_ty]>;

  // SDEPTH (s - x), returns the depth of Slice s. If s has no references, then x = 0;
  //  otherwise x is one plus the maximum of depths of cells referred to from s
  def int_tvm_sdepth :
    ClangBuiltin<"__builtin_tvm_sdepth">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMSlice_ty]>;

  // CDEPTH (c - x), returns the depth of Cell c. If c has no references, then x = 0;
  //  otherwise x is one plus the maximum of depths of cells referred to from c
  def int_tvm_cdepth :
    ClangBuiltin<"__builtin_tvm_cdepth">,
    Intrinsic<[llvm_i257_ty], [llvm_TVMCell_ty]>;

// *******************************************
//
// A.10	Dictionary manipulation primitives
//
// ******************************************* /

// *******************************************
//
// A.10.2. Dictionary serialization and deserialization
//
// ******************************************* /

// SKIPDICT or SKIPOPTREF (s - s0), equivalent to LDDICT; NIP
def int_tvm_skipdict :
    ClangBuiltin<"__builtin_tvm_skipdict">,
    Intrinsic<[llvm_TVMSlice_ty],
              [llvm_TVMSlice_ty], [IntrNoMem]>;

// LDDICTS (s - s0 s00), loads (parses) a (Slice-represented) dictionary s0 from Slice s, and returns the remainder of s as s00
def int_tvm_lddicts :
    ClangBuiltin<"__builtin_tvm_lddicts">,
    Intrinsic<[llvm_TVMSlice_ty, llvm_TVMSlice_ty],
              [llvm_TVMSlice_ty], [IntrNoMem]>;

// PLDDICTS (s - s0), preloads a (Slice - represented) dictionary s0 from Slice s. Approximately equivalent to LDDICTS; DROP
def int_tvm_plddicts :
    ClangBuiltin<"__builtin_tvm_plddicts">,
    Intrinsic<[llvm_TVMSlice_ty],
              [llvm_TVMSlice_ty], [IntrNoMem]>;

// LDDICTQ (s - D s0 -1 or s 0), a quiet version of LDDICT
def int_tvm_lddictq :
    ClangBuiltin<"__builtin_tvm_lddictq">,
    Intrinsic<[llvm_TVMCell_ty, llvm_TVMSlice_ty, llvm_i257_ty],
              [llvm_TVMSlice_ty], [IntrNoMem]>;

// PLDDICTQ (s - D -1 or 0), a quiet version of PLDDICT
def int_tvm_plddictq :
    ClangBuiltin<"__builtin_tvm_plddictq">,
    Intrinsic<[llvm_TVMCell_ty, llvm_i257_ty],
              [llvm_TVMSlice_ty], [IntrNoMem]>;

// *******************************************
//
// A.10.4.	Set/Replace/Add dictionary operations
//
// ******************************************* /

// DICTISETGET (x i D n - D0 y -1 or D0 0), similar to DICTSETGET,
//  but with the key represented by a big-endian signed n-bit Integer I
// (value int slice nbits - (slice y -1) | (slice 0))
  def int_tvm_dictisetget :
    ClangBuiltin<"__builtin_tvm_dictisetget">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_TVMSlice_ty, llvm_i257_ty],
              [llvm_TVMSlice_ty, llvm_i257_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTISETGETREF (c i D n - D0 c0 -1 or D0 0), a version of DICTSETGETREF with signed Integer i as a key
// (cell int slice nbits - (slice cell -1) | (slice 0))
  def int_tvm_dictisetgetref :
    ClangBuiltin<"__builtin_tvm_dictisetgetref">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_TVMCell_ty, llvm_i257_ty],
              [llvm_TVMCell_ty, llvm_i257_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTREPLACEGET (x k D n - D0 y -1 or D 0), a Replace counterpart of DICTSETGET:
//  on success, also returns the old value associated with the key in question
// (value int slice nbits - (slice value -1) | (slice 0))
  def int_tvm_dictreplaceget :
    ClangBuiltin<"__builtin_tvm_dictreplaceget">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_TVMSlice_ty, llvm_i257_ty],
              [llvm_TVMSlice_ty, llvm_TVMSlice_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTREPLACEGETREF (c k D n - D0 c0 -1 or D 0)
// (cell int slice nbits - (slice' cell -1) | (slice 0))
  def int_tvm_dictreplacegetref :
    ClangBuiltin<"__builtin_tvm_dictreplacegetref">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_TVMCell_ty, llvm_i257_ty],
              [llvm_TVMCell_ty /* Dict */, llvm_TVMSlice_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTIREPLACEGET  (x i D n - D0 y -1 or D 0)
// (value int slice nbits - (slice value -1) | (slice 0))
  def int_tvm_dictireplaceget :
    ClangBuiltin<"__builtin_tvm_dictireplaceget">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_TVMSlice_ty, llvm_i257_ty],
              [llvm_TVMSlice_ty, llvm_i257_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTIREPLACEGETREF (c i D n - D0 c0 -1 or D 0)
// (cell int slice nbits - (slice' cell -1) | (slice 0))
  def int_tvm_dictireplacegetref :
    ClangBuiltin<"__builtin_tvm_dictireplacegetref">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_TVMCell_ty, llvm_i257_ty],
              [llvm_TVMCell_ty, llvm_i257_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTUREPLACEGET (x i D n - D0 y -1 or D 0)
// (value uint slice nbits - (slice value -1) | (slice 0))
  def int_tvm_dictureplaceget :
    ClangBuiltin<"__builtin_tvm_dictureplaceget">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_TVMSlice_ty, llvm_i257_ty],
              [llvm_TVMSlice_ty, llvm_i257_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTUREPLACEGETREF (c i D n - D0 c0 -1 or D 0)
// (cell uint slice nbits - (slice' cell -1) | (slice 0))
  def int_tvm_dictureplacegetref :
    ClangBuiltin<"__builtin_tvm_dictureplacegetref">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_TVMCell_ty, llvm_i257_ty],
              [llvm_TVMCell_ty, llvm_i257_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTADDGET (x k D n - D0 -1 or D y 0), an Add counterpart of DICTSETGET: sets the value associated with key k in dictionary D to x,
//   but only if key k is not already present in D. Otherwise, just returns the old value y without changing the dictionary
// (value key slice nbits - (slice -1) | (slice y 0))
  def int_tvm_dictaddget :
    ClangBuiltin<"__builtin_tvm_dictaddget">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_TVMSlice_ty, llvm_i257_ty],
              [llvm_TVMSlice_ty, llvm_TVMSlice_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTADDGETREF (c k D n - D0 -1 or D c0 0), an Add counterpart of DICTSETGETREF
// (cell key slice nbits - (slice' -1) | (slice cell 0))
  def int_tvm_dictaddgetref :
    ClangBuiltin<"__builtin_tvm_dictaddgetref">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_TVMCell_ty, llvm_i257_ty],
              [llvm_TVMCell_ty, llvm_TVMSlice_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTIADDGET (x i D n - D0 -1 or D y 0)
// (value int slice nbits - (slice -1) | (slice value 0))
  def int_tvm_dictiaddget :
    ClangBuiltin<"__builtin_tvm_dictiaddget">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_TVMSlice_ty, llvm_i257_ty],
              [llvm_TVMSlice_ty, llvm_i257_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTIADDGETREF (c i D n - D0 -1 or D c0 0)
// (cell int slice nbits - (slice -1) | (slice cell 0))
  def int_tvm_dictiaddgetref :
    ClangBuiltin<"__builtin_tvm_dictiaddgetref">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_TVMCell_ty, llvm_i257_ty],
              [llvm_TVMCell_ty, llvm_i257_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTUADDGET	(x i D n - D0 -1 or D y 0)
// (value uint slice nbits - (slice -1) | (slice value 0))
  def int_tvm_dictuaddget :
    ClangBuiltin<"__builtin_tvm_dictuaddget">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_TVMSlice_ty, llvm_i257_ty],
              [llvm_TVMSlice_ty, llvm_i257_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTUADDGETREF (c i D n - D0 -1 or D c0 0)
// (cell uint slice nbits - (slice -1) | (slice cell 0))
  def int_tvm_dictuaddgetref :
    ClangBuiltin<"__builtin_tvm_dictuaddgetref">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_TVMCell_ty, llvm_i257_ty],
              [llvm_TVMCell_ty, llvm_i257_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;


// *******************************************
//
// A.10.5. Builder-accepting variants of Set dictionary operations.
//
// ******************************************* /

// DICTSETB (b k D n - D0)
// (builder key slice nbits - slice)
  def int_tvm_dictsetb :
    ClangBuiltin<"__builtin_tvm_dictsetb">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */],
              [llvm_TVMBuilder_ty, llvm_TVMSlice_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTISETB (b i D n - D0)
// (builder int slice nbits - slice)
  def int_tvm_dictisetb :
    ClangBuiltin<"__builtin_tvm_dictisetb">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */],
              [llvm_TVMBuilder_ty, llvm_i257_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTUSETB (b i D n - D0)
// (builder uint slice nbits - slice)
  def int_tvm_dictusetb :
    ClangBuiltin<"__builtin_tvm_dictusetb">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */],
              [llvm_TVMBuilder_ty, llvm_i257_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTSETGETB (b k D n - D0 y -1 or D0 0)
// (builder key slice nbits - (slice value -1) | (slice 0))
  def int_tvm_dictsetgetb :
    ClangBuiltin<"__builtin_tvm_dictsetgetb">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_TVMSlice_ty, llvm_i257_ty],
              [llvm_TVMBuilder_ty, llvm_TVMSlice_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTISETGETB (b i D n - D0 y -1 or D0 0)
// (builder int slice nbits - (slice' y -1) | (slice' 0))
  def int_tvm_dictisetgetb :
    ClangBuiltin<"__builtin_tvm_dictisetgetb">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_TVMSlice_ty, llvm_i257_ty],
              [llvm_TVMBuilder_ty, llvm_i257_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTUSETGETB (b i D n - D0 y -1 or D0 0)
// (builder uint slice nbits - (slice' y -1) | (slice' 0))
  def int_tvm_dictusetgetb :
    ClangBuiltin<"__builtin_tvm_dictusetgetb">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_TVMSlice_ty, llvm_i257_ty],
              [llvm_TVMBuilder_ty, llvm_i257_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTREPLACEB (b k D n - D0 -1 or D 0)
// (builder int slice nbits - (slice -1) | (slice 0))
  def int_tvm_dictreplaceb :
    ClangBuiltin<"__builtin_tvm_dictreplaceb">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_i257_ty],
              [llvm_TVMBuilder_ty, llvm_TVMSlice_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTIREPLACEB (b i D n - D0 -1 or D 0)
// (builder uint slice nbits - (slice -1) | (slice 0))
  def int_tvm_dictireplaceb :
    ClangBuiltin<"__builtin_tvm_dictireplaceb">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_i257_ty],
              [llvm_TVMBuilder_ty, llvm_i257_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTUREPLACEB (b i D n - D0 -1 or D 0)
// (builder uint slice nbits - (slice -1) | (slice 0))
  def int_tvm_dictureplaceb :
    ClangBuiltin<"__builtin_tvm_dictureplaceb">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_i257_ty],
              [llvm_TVMBuilder_ty, llvm_i257_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTREPLACEGETB (b k D n - D0 y -1 or D 0)
// (builder key slice nbits - (slice value -1) | (slice 0))
  def int_tvm_dictreplacegetb :
    ClangBuiltin<"__builtin_tvm_dictreplacegetb">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_TVMSlice_ty, llvm_i257_ty],
              [llvm_TVMBuilder_ty, llvm_TVMSlice_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTIREPLACEGETB (b i D n - D0 y -1 or D 0)
// (builder int slice nbits - (slice value -1) | (slice 0))
  def int_tvm_dictireplacegetb :
    ClangBuiltin<"__builtin_tvm_dictireplacegetb">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_TVMSlice_ty, llvm_i257_ty],
              [llvm_TVMBuilder_ty, llvm_i257_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTUREPLACEGETB	(b i D n - D0 y -1 or D 0)
// (builder uint slice nbits - (slice value -1) | (slice 0))
  def int_tvm_dictureplacegetb :
    ClangBuiltin<"__builtin_tvm_dictureplacegetb">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_TVMSlice_ty, llvm_i257_ty],
              [llvm_TVMBuilder_ty, llvm_i257_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTADDB (b k D n - D0 -1 or D 0)
// (builder key slice nbits - (slice -1) | (slice 0))
  def int_tvm_dictaddb :
    ClangBuiltin<"__builtin_tvm_dictaddb">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_i257_ty],
              [llvm_TVMBuilder_ty, llvm_TVMSlice_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTIADDB (b i D n - D0 -1 or D 0)
// (builder int slice nbits - (slice -1) | (slice 0))
  def int_tvm_dictiaddb :
    ClangBuiltin<"__builtin_tvm_dictiaddb">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_i257_ty],
              [llvm_TVMBuilder_ty, llvm_i257_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTUADDB  (b i D n - D0 -1 or D 0)
// (builder uint slice nbits - (slice -1) | (slice 0))
  def int_tvm_dictuaddb :
    ClangBuiltin<"__builtin_tvm_dictuaddb">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_i257_ty],
              [llvm_TVMBuilder_ty, llvm_i257_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTADDGETB (b k D n - D0 -1 or D y 0)
// (builder key slice nbits - (slice -1) | (slice value 0))
  def int_tvm_dictaddgetb :
    ClangBuiltin<"__builtin_tvm_dictaddgetb">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_TVMSlice_ty, llvm_i257_ty],
              [llvm_TVMBuilder_ty, llvm_TVMSlice_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTIADDGETB (b i D n - D0 -1 or D y 0)
// (builder int slice nbits - (slice' -1) | (slice y 0))
  def int_tvm_dictiaddgetb :
    ClangBuiltin<"__builtin_tvm_dictiaddgetb">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_TVMSlice_ty, llvm_i257_ty],
              [llvm_TVMBuilder_ty, llvm_i257_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTUADDGETB	(b i D n - D0 -1 or D y 0)
// (builder uint slice nbits - (slice' -1) | (slice y 0))
  def int_tvm_dictuaddgetb :
    ClangBuiltin<"__builtin_tvm_dictuaddgetb">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_TVMSlice_ty, llvm_i257_ty],
              [llvm_TVMBuilder_ty, llvm_i257_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// *******************************************
//
// A.10.6. Delete dictionary operations
//
// ******************************************* /

// DICTIDEL (i D n - D0 ?), a version of DICTDEL with the key represented by a signed n-bit Integer i. If i does not fit into n bits,
//   simply returns D 0 ("key not found, dictionary unmodified")
// (int slice nbits - (slice' -1) | (slice 0))
  def int_tvm_dictidel :
    ClangBuiltin<"__builtin_tvm_dictidel">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_i257_ty],
              [llvm_i257_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTIDELGET (i D n - D0 x -1 or D 0), a variant of primitive DICTDELGET with signed n-bit integer i as a key
// (int slice nbits - (slice value -1) | (slice 0))
 def int_tvm_dictidelget :
    ClangBuiltin<"__builtin_tvm_dictidelget">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_TVMSlice_ty, llvm_i257_ty],
              [llvm_i257_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTIDELGETREF (i D n - D0 c -1 or  D 0), a variant of primitive DICTIDELGET returning a Cell instead of a Slice
// (int slice nbits - (slice cell -1) | (slice 0))
 def int_tvm_dictidelgetref :
    ClangBuiltin<"__builtin_tvm_dictidelgetref">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_TVMCell_ty, llvm_i257_ty],
              [llvm_i257_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTUDELGET (i D n - D0x -1 or D 0), a variant of primitive DICTDELGET with unsigned n-bit integer i as a key
// (uint slice nbits - (slice value -1) | (slice 0))
 def int_tvm_dictudelget :
    ClangBuiltin<"__builtin_tvm_dictudelget">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_TVMSlice_ty, llvm_i257_ty],
              [llvm_i257_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTUDELGETREF (i D n - D0 c -1 or D 0), a variant of primitive DICTUDELGET returning a Cell instead of a Slice
// (uint slice nbits - (slice cell -1) | (slice 0))
 def int_tvm_dictudelgetref :
    ClangBuiltin<"__builtin_tvm_dictudelgetref">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_TVMCell_ty, llvm_i257_ty],
              [llvm_i257_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// *******************************************
//
// A.10.7. "Maybe reference" dictionary operations
//
// ******************************************* /

// DICTGETOPTREF (k D n - c?), a variant of DICTGETREF that returns Null
//   instead of the value c? if the key k is absent from dictionary D
  def int_tvm_dictgetoptref :
    ClangBuiltin<"__builtin_tvm_dictgetoptref">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */],
              [llvm_TVMSlice_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTIGETOPTREF (i D n - c?), similar to DICTGETOPTREF, but also returns Null
  def int_tvm_dictigetoptref :
    ClangBuiltin<"__builtin_tvm_dictigetoptref">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */],
              [llvm_i257_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTUGETOPTREF (i D n - c?), similar to DICTGETOPTREF, but with the key given by unsigned n-bit Integer
  def int_tvm_dictugetoptref :
    ClangBuiltin<"__builtin_tvm_dictugetoptref">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */],
              [llvm_i257_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTSETGETOPTREF	(c? k D n - D0 ~?), a variant of both DICTGETOPTREF and DICTSETGETREF that sets the value corresponding to key k
//   in dictionary D to c? (if c? Is Null, then the key is deleted instead), and returns the old value ~? (if the key k was absent before, returns Null instead)
  def int_tvm_dictsetgetoptref :
    ClangBuiltin<"__builtin_tvm_dictsetgetoptref">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_TVMCell_ty],
              [llvm_TVMCell_ty, llvm_TVMSlice_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTISETGETOPTREF (c? i D n - D0 ~?), similar to primitive DICTSETGETOPTREF, but using signed n-bit Integer i as a key.
//   If i does not fit into n bits, throws a range checking exception
  def int_tvm_dictisetgetoptref :
    ClangBuiltin<"__builtin_tvm_dictisetgetoptref">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_TVMCell_ty],
              [llvm_TVMCell_ty, llvm_i257_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;


// DICTUSETGETOPTREF (c? i D n - D0 ~?), similar to primitive DICTSETGETOPTREF, but using unsigned n-bit Integer i as a key
  def int_tvm_dictusetgetoptref :
    ClangBuiltin<"__builtin_tvm_dictusetgetoptref">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_TVMCell_ty],
              [llvm_TVMCell_ty, llvm_i257_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// *******************************************
//
// A.10.8. Prefix code dictionary operations
//
// ******************************************* /

// PFXDICTSET (x k D n - D0 -1 or D 0)
// (value key slice nbits - slice -1|0)
 def int_tvm_pfxdictset :
    ClangBuiltin<"__builtin_tvm_pfxdictset">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_i257_ty],
              [llvm_TVMSlice_ty, llvm_TVMSlice_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// PFXDICTREPLACE (x k D n - D0 -1 or D 0)
// (value key slice nbits - slice -1|0)
 def int_tvm_pfxdictreplace :
    ClangBuiltin<"__builtin_tvm_pfxdictreplace">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_i257_ty],
              [llvm_TVMSlice_ty, llvm_TVMSlice_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// PFXDICTADD (x k D n - D0 -1 or D 0)
// (value key slice nbits - slice -1|0)
 def int_tvm_pfxdictadd :
    ClangBuiltin<"__builtin_tvm_pfxdictadd">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_i257_ty],
              [llvm_TVMSlice_ty, llvm_TVMSlice_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// PFXDICTDEL (k D n - D0 -1 or D 0)
// (key slice nbits - slice -1|0)
 def int_tvm_pfxdictdel :
    ClangBuiltin<"__builtin_tvm_pfxdictdel">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_i257_ty],
              [llvm_TVMSlice_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;


// *******************************************
//
// A.10.9. Variants of GetNext and GetPrev operations
//
// ******************************************* /

// DICTGETNEXTEQ (k D n - x0 k0 -1 or 0), similar to DICTGETNEXT, but computes the minimal key k0
//  that is lexicographically greater than or equal to k
// (key slice nbits - (value key -1) | (0))
  def int_tvm_dictgetnexteq :
    ClangBuiltin<"__builtin_tvm_dictgetnexteq">,
    Intrinsic<[llvm_TVMSlice_ty, llvm_TVMSlice_ty, llvm_i257_ty],
              [llvm_TVMSlice_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTGETPREV (k D n - x0 k0 -1 or 0), similar to DICTGETNEXT, but computes the maximal key k0
//   lexicographically smaller than k
// (key slice nbits - (value key -1) | (0))
  def int_tvm_dictgetprev :
    ClangBuiltin<"__builtin_tvm_dictgetprev">,
    Intrinsic<[llvm_TVMSlice_ty, llvm_TVMSlice_ty, llvm_i257_ty],
              [llvm_TVMSlice_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTGETPREVEQ (k D n - x0 k0 -1 or 0), similar to DICTGETPREV, but computes the maximal key k0
//  lexicographically smaller than or equal to k
// (key slice nbits - (value key -1) | (0))
  def int_tvm_dictgetpreveq :
    ClangBuiltin<"__builtin_tvm_dictgetpreveq">,
    Intrinsic<[llvm_TVMSlice_ty, llvm_TVMSlice_ty, llvm_i257_ty],
              [llvm_TVMSlice_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTIGETNEXT (i D n - x0 i0 -1 or 0), similar to DICTGETNEXT, but interprets all keys in dictionary D as big-endian signed n-bit integers,
//   and computes the minimal key i0 that is larger than Integer i (which does not necessarily fit into n bits)
// (int slice nbits - (value key -1) | (0))
  def int_tvm_dictigetnext :
    ClangBuiltin<"__builtin_tvm_dictigetnext">,
    Intrinsic<[llvm_TVMSlice_ty, llvm_TVMSlice_ty, llvm_i257_ty],
              [llvm_i257_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTIGETNEXTEQ (i D n - x0 i0 -1 or 0)
// (int slice nbits - (value key -1) | (0))
  def int_tvm_dictigetnexteq :
    ClangBuiltin<"__builtin_tvm_dictigetnexteq">,
    Intrinsic<[llvm_TVMSlice_ty, llvm_TVMSlice_ty, llvm_i257_ty],
              [llvm_i257_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTIGETPREV (i D n - x0 i0 -1 or 0)
// (int slice nbits - (value key -1) | (0))
  def int_tvm_dictigetprev :
    ClangBuiltin<"__builtin_tvm_dictigetprev">,
    Intrinsic<[llvm_TVMSlice_ty, llvm_TVMSlice_ty, llvm_i257_ty],
              [llvm_i257_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTIGETPREVEQ (i D n - x0 i0 -1 or 0)
// (int slice nbits - (value key -1) | (0))
  def int_tvm_dictigetpreveq :
    ClangBuiltin<"__builtin_tvm_dictigetpreveq">,
    Intrinsic<[llvm_TVMSlice_ty, llvm_TVMSlice_ty, llvm_i257_ty],
              [llvm_i257_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTUGETNEXTEQ (i D n - x0 i0 -1 or 0)
// (uint slice nbits - (value key -1) | (0))
  def int_tvm_dictugetnexteq :
    ClangBuiltin<"__builtin_tvm_dictugetnexteq">,
    Intrinsic<[llvm_TVMSlice_ty, llvm_TVMSlice_ty, llvm_i257_ty],
              [llvm_i257_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTUGETPREVEQ (i D n - x0 i0 -1 or 0)
// (uint slice nbits - (value key -1) | (0))
  def int_tvm_dictugetpreveq :
    ClangBuiltin<"__builtin_tvm_dictugetpreveq">,
    Intrinsic<[llvm_TVMSlice_ty, llvm_TVMSlice_ty, llvm_i257_ty],
              [llvm_i257_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// *******************************************
//
// A.10.10. GetMin, GetMax, RemoveMin, RemoveMax operations
//
// ******************************************* /

// DICTMINREF (D n - c k -1 or 0), similar to DICTMIN, but returns the only reference in the value as a Cell c
// (slice nbits - (cell key -1) | (0))
  def int_tvm_dictminref :
    ClangBuiltin<"__builtin_tvm_dictminref">,
    Intrinsic<[llvm_TVMCell_ty, llvm_TVMSlice_ty, llvm_i257_ty],
              [llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTIMIN (D n - x i -1 or 0), somewhat similar to DICTMIN, but computes the minimal key I under the assumption that all keys are big-endian signed n-bit integers.
//   Notice that the key and value returned may differ from those computed by DICTMIN and DICTUMIN
// (slice nbits - (value int -1) | (0))
  def int_tvm_dictimin :
    ClangBuiltin<"__builtin_tvm_dictimin">,
    Intrinsic<[llvm_TVMSlice_ty, llvm_i257_ty, llvm_i257_ty],
              [llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTIMINREF (D n - c i -1 or 0)
// (slice nbits - (cell int -1) | (0))
  def int_tvm_dictiminref :
    ClangBuiltin<"__builtin_tvm_dictiminref">,
    Intrinsic<[llvm_TVMCell_ty, llvm_i257_ty, llvm_i257_ty],
              [llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTMAX (D n - x k -1 or 0), computes the maximal key k (represented by a Slice with n data bits)
//   in dictionary D, and returns k along with the associated value x
// (slice nbits - (value key -1) | (0))
  def int_tvm_dictmax :
    ClangBuiltin<"__builtin_tvm_dictmax">,
    Intrinsic<[llvm_TVMSlice_ty, llvm_TVMSlice_ty, llvm_i257_ty],
              [llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTMAXREF (D n - c k -1 or 0)
// (slice nbits - (cell key -1) | (0))
  def int_tvm_dictmaxref :
    ClangBuiltin<"__builtin_tvm_dictmaxref">,
    Intrinsic<[llvm_TVMCell_ty, llvm_TVMSlice_ty, llvm_i257_ty],
              [llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTIMAX (D n - x i -1 or 0)
// (slice nbits - (value int -1) | (0))
  def int_tvm_dictimax :
    ClangBuiltin<"__builtin_tvm_dictimax">,
    Intrinsic<[llvm_TVMSlice_ty, llvm_i257_ty, llvm_i257_ty],
              [llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTIMAXREF	(D n - c i -1 or 0)
// (slice nbits - (cell int -1) | (0))
  def int_tvm_dictimaxref :
    ClangBuiltin<"__builtin_tvm_dictimaxref">,
    Intrinsic<[llvm_TVMCell_ty , llvm_i257_ty, llvm_i257_ty],
              [llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTREMMIN (D n - D0 x k -1 or D 0), computes the minimal key k (represented by a Slice with n data bits) in dictionary D,
//   removes k from the dictionary, and returns k along with the associated value x and the modified dictionary D0
// (slice nbits - (slice value key -1) | (0))
  def int_tvm_dictremmin :
    ClangBuiltin<"__builtin_tvm_dictremmin">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_TVMSlice_ty, llvm_TVMSlice_ty, llvm_i257_ty],
              [llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

//  DICTREMMINREF (D n - D0 c k -1 or D 0), similar to DICTREMMIN,
//  but returns the only reference in the value as a Cell c
// (slice nbits - (slice cell key -1) | (0))
  def int_tvm_dictremminref :
    ClangBuiltin<"__builtin_tvm_dictremminref">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_TVMCell_ty, llvm_TVMSlice_ty, llvm_i257_ty],
              [llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTIREMMIN	(D n - D0 x i -1 or D 0), somewhat similar to DICTREMMIN, but computes the minimal key i under the assumption
//  that all keys are big-endian signed n-bit integers. Notice that the key and value returned may differ from those computed by DICTREMMIN and
//  DICTUREMMIN
// (slice nbits - (slice cell key -1) | (0))
  def int_tvm_dictiremmin :
    ClangBuiltin<"__builtin_tvm_dictiremmin">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_TVMSlice_ty, llvm_i257_ty, llvm_i257_ty],
              [llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTIREMMINREF (D n - D0 c i -1 or D 0)
// (slice nbits - (slice cell int -1) | (0))
  def int_tvm_dictiremminref :
    ClangBuiltin<"__builtin_tvm_dictiremminref">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_TVMCell_ty, llvm_i257_ty, llvm_i257_ty],
              [llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTREMMAX (D n - D0 x k -1 or D 0), computes the maximal key k (represented by a Slice with n data bits) in dictionary D,
//   removes k from the dictionary, and returns k along with the associated value x and the modified dictionary D0
// (slice nbits - (slice value key -1) | (0))
 def int_tvm_dictremmax :
    ClangBuiltin<"__builtin_tvm_dictremmax">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_TVMSlice_ty, llvm_TVMSlice_ty, llvm_i257_ty],
              [llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTREMMAXREF (D n - D0c k -1 or D 0)
// (slice nbits - (slice cell key -1) | (0))
 def int_tvm_dictremmaxref :
    ClangBuiltin<"__builtin_tvm_dictremmaxref">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_TVMCell_ty, llvm_TVMSlice_ty, llvm_i257_ty],
              [llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTIREMMAX (D n - D0x i -1 or D 0)
// (slice nbits - (slice value int -1) | (0))
 def int_tvm_dictiremmax :
    ClangBuiltin<"__builtin_tvm_dictiremmax">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_TVMSlice_ty, llvm_i257_ty, llvm_i257_ty],
              [llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// DICTIREMMAXREF (D n - D0c i -1 or D 0)
// (slice nbits - (slice cell int -1) | (0))
 def int_tvm_dictiremmaxref :
    ClangBuiltin<"__builtin_tvm_dictiremmaxref">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */, llvm_TVMCell_ty, llvm_i257_ty, llvm_i257_ty],
              [llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// *******************************************
//
// A.10.12. SubDictdictionary operations
//
// ******************************************* /

// SUBDICTGET (k l D n - D0), constructs a subdictionary consisting of all keys beginning with prefix k
//  (represented by a Slice, the first 0 <= l <= n <= 1023 data bits of which are used as a key) of length l in dictionary D of type HashmapE(n, X)
//  with n-bit keys. On success, returns the new subdictionary of the same type HashmapE(n, X) as a Slice D0
// (prefix lbits dict nbits - dict)
 def int_tvm_subdictget :
    ClangBuiltin<"__builtin_tvm_subdictget">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */],
              [llvm_TVMSlice_ty, llvm_i257_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// SUBDICTIGET (x l D n - D0), variant of SUBDICTGET with the prefix represented by a signed big-endian l-bit Integer x, where necessarily l <= 257
// (prefix lbits dict nbits - dict')
 def int_tvm_subdictiget :
    ClangBuiltin<"__builtin_tvm_subdictiget">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */],
              [llvm_i257_ty, llvm_i257_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// SUBDICTUGET (x l D n - D0), variant of SUBDICTGET with the prefix represented by an unsigned big-endian l-bit	Integer	x, where necessarily l <= 256
// (prefix lbits dict nbits - dict')
 def int_tvm_subdictuget :
    ClangBuiltin<"__builtin_tvm_subdictuget">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */],
              [llvm_i257_ty, llvm_i257_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// SUBDICTRPGET (k l D n - D0), similar to SUBDICTGET, but removes the common prefix	k from all keys of the new dictionary D0,
//   which becomes of type HashmapE(n - l, X)
// (prefix lbits dict nbits - dict')
 def int_tvm_subdictrpget :
    ClangBuiltin<"__builtin_tvm_subdictrpget">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */],
              [llvm_TVMSlice_ty, llvm_i257_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// SUBDICTIRPGET (x l D n - D0), variant of SUBDICTRPGET with the prefix represented by a signed big-endian l-bit Integer x,
//  where necessarily l <= 257
// (prefix lbits dict nbits - dict')
 def int_tvm_subdictirpget :
    ClangBuiltin<"__builtin_tvm_subdictirpget">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */],
              [llvm_i257_ty, llvm_i257_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// SUBDICTURPGET (x l D n - D0), variant of SUBDICTRPGET with the prefix represented by an unsigned big-endian l-bit Integer x,
//   where necessarily l <= 256
// (prefix lbits dict nbits - dict')
 def int_tvm_subdicturpget :
    ClangBuiltin<"__builtin_tvm_subdicturpget">,
    Intrinsic<[llvm_TVMCell_ty /* Dict */],
              [llvm_i257_ty, llvm_i257_ty, llvm_TVMCell_ty /* Dict */, llvm_i257_ty]>;

// *******************************************
//
// A.11.3. Pseudo-random number generator primitives
//
// ******************************************* /

  // RANDU256 ( -	x), generates a new pseudo-random unsigned 256-bit Integer x
  def int_tvm_randu256 :
      ClangBuiltin<"__builtin_tvm_randu256">,
      Intrinsic<[llvm_i257_ty], [], [IntrNoMem]>;

  // RAND (y - z), generates a new pseudo-random integer z in the range 0...y-1 (or y...-1, if y < 0)
  def int_tvm_rand :
      ClangBuiltin<"__builtin_tvm_rand">,
      Intrinsic<[llvm_i257_ty], [llvm_i257_ty], [IntrNoMem]>;

  // SETRAND  (x	- ), sets the random seed to unsigned 256-bit Integer x
  def int_tvm_setrand :
      ClangBuiltin<"__builtin_tvm_setrand">,
      Intrinsic<[], [llvm_i257_ty]>;


  // ADDRAND (x - ), mixes unsigned 256-bit Integer x into the random seed r
  def int_tvm_addrand :
      ClangBuiltin<"__builtin_tvm_addrand">,
      Intrinsic<[], [llvm_i257_ty]>;

// *******************************************
//
// A.11.4. Configuration primitives
//
// ******************************************* /

// RANDSEED ( -	x), returns the current random seed as an unsigned 256-bit Integer. Equivalent to GETPARAM 6
  def int_tvm_randseed :
      ClangBuiltin<"__builtin_tvm_randseed">,
      Intrinsic<[llvm_i257_ty], [], [IntrNoMem]>;

// BALANCE ( -	t), returns the remaining balance of the smart contract as a Tuple consisting of an Integer (the remaining Gram balance in nanograms)
//   and a Maybe Cell (a dictionary with 32-bit keys representing the balance of "extra currencies"). Equivalent to GETPARAM 7
  def int_tvm_balance :
      ClangBuiltin<"__builtin_tvm_balance">,
      Intrinsic<[llvm_TVMTuple_ty], [], [IntrNoMem]>;


// *******************************************
//
// A.11.6. Hashing and cryptography primitives
//
// ******************************************* /

// SHA256U (s - x), computes sha256 of the data bits of Slice s.
//  If the bit length of s is not divisible by eight, throws a cell underflow exception. The hash value is returned as a 256-bit unsigned integer x
  def int_tvm_sha256u :
      ClangBuiltin<"__builtin_tvm_sha256u">,
      Intrinsic<[llvm_i257_ty], [llvm_TVMSlice_ty], [IntrNoMem]>;

// CHKSIGNS  (d s k - ?) checks whether	s is a valid Ed25519-signature of the data portion of Slice	d using public key k, similarly to CHKSIGNU
  def int_tvm_chksigns :
      ClangBuiltin<"__builtin_tvm_chksigns">,
      Intrinsic<[llvm_i257_ty], [llvm_TVMSlice_ty, llvm_i257_ty, llvm_i257_ty], [IntrNoMem]>;

// *******************************************
//
// A.11.7. Miscellaneous primitives
//
// ******************************************* /

// CDATASIZEQ (c n - x y z -1 or 0), recursively computes the count of distinct cells x, data bits y,
//   and cell references z in the dag rooted at Cell c
  def int_tvm_cdatasizeq :
      ClangBuiltin<"__builtin_tvm_cdatasizeq">,
      Intrinsic<[llvm_i257_ty, llvm_i257_ty, llvm_i257_ty, llvm_i257_ty], [llvm_TVMCell_ty, llvm_i257_ty], [IntrNoMem]>;

// *******************************************
//
// A.11.10. Outbound message and output action primitives
//
// ******************************************* /

// RAWRESERVEX  (x D y - ), similar to RAWRESERVE, but also accepts a dictionary D (represented by a Cell or Null ) with extra currencies
  def int_tvm_rawreservex :
      ClangBuiltin<"__builtin_tvm_rawreservex">,
      Intrinsic<[], [llvm_i257_ty, llvm_TVMCell_ty, llvm_i257_ty]>;

// SETLIBCODE (c x - ), creates an output action that would modify the collection of this smart contract libraries by adding or removing
//   library with code given in	Cell c
  def int_tvm_setlibcode :
      ClangBuiltin<"__builtin_tvm_setlibcode">,
      Intrinsic<[], [llvm_TVMCell_ty, llvm_i257_ty]>;


// FB07	- CHANGELIB (h x - ), creates an output action similarly to SETLIBCODE,
//   but instead of the library code accepts its hash as an unsigned 256-bit integer
  def int_tvm_changelib :
      ClangBuiltin<"__builtin_tvm_changelib">,
      Intrinsic<[], [llvm_i257_ty, llvm_i257_ty]>;
}

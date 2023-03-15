add_llvm_component_group(Assigner)

add_llvm_target(AssignerCodeGen
  AssignerISelLowering.cpp
  AssignerSubtarget.cpp
  AssignerTargetMachine.cpp

  LINK_COMPONENTS
  Analysis
  AsmPrinter
  AssignerDesc
  AssignerInfo
  CodeGen
  Core
  MC
  Scalar
  SelectionDAG
  Support
  Target
  TargetParser
  TransformUtils
  GlobalISel
  CFGuard

  ADD_TO_COMPONENT
  Assigner
)

add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)

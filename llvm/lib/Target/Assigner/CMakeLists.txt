add_llvm_component_group(Assigner)

add_llvm_target(AssignerCodeGen
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
  TransformUtils
  GlobalISel
  CFGuard

  ADD_TO_COMPONENT
  Assigner
)

add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)

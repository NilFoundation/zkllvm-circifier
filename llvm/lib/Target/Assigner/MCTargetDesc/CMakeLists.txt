add_llvm_component_library(LLVMAssignerDesc
  AssignerMCTargetDesc.cpp

  LINK_COMPONENTS
  MC
  AssignerInfo
  Support

  ADD_TO_COMPONENT
  Assigner
)

add_llvm_component_library(LLVMAssignerInfo
  AssignerTargetInfo.cpp

  LINK_COMPONENTS
  MC
  Support

  ADD_TO_COMPONENT
  Assigner
)

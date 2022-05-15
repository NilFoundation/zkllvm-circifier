add_llvm_component_library(LLVMEVMInfo
                           EVMTargetInfo.cpp

                           LINK_COMPONENTS
                           MC

                           ADD_TO_COMPONENT
                           EVM
                           )


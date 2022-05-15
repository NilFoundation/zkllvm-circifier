add_llvm_component_library(LLVMEVMDisassembler
                           EVMDisassembler.cpp

                           LINK_COMPONENTS
                           EVMInfo
                           Support
                           MCDisassembler

                           ADD_TO_COMPONENT
                           EVM
                           )


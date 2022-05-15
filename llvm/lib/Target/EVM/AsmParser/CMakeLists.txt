add_llvm_component_library(LLVMEVMAsmParser
                           EVMAsmParser.cpp

                           LINK_COMPONENTS
                           EVMInfo
                           MC
                           MCParser
                           Support

                           ADD_TO_COMPONENT
                           EVM
                           )

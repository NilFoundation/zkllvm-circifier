add_llvm_component_library(LLVMEVMDesc
                           EVMMCTargetDesc.cpp
                           EVMAsmBackend.cpp
                           EVMInstPrinter.cpp
                           EVMMCCodeEmitter.cpp
                           EVMTargetStreamer.cpp
                           EVMBinaryObjectWriter.cpp
                           EVMObjectWriter.cpp

                           LINK_COMPONENTS
                           EVMInfo
                           MC
                           Support

                           ADD_TO_COMPONENT
                           EVM
                           )

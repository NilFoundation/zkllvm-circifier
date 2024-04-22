#ifndef LLVM_STORAGE_CONFIG_H
#define LLVM_STORAGE_CONFIG_H

#include "evm_sdk.h"
#include <utility>
#include <optional>

namespace evm::config {
#define __STORAGE_CONFIG
#include "storage_base.h"
#undef __STORAGE_CONFIG
}

#endif // LLVM_STORAGE_CONFIG_H

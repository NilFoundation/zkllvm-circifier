// EVM_START

#include "CodeGenFunction.h"
#include "llvm/IR/IntrinsicsEVM.h"

using namespace clang;
using namespace CodeGen;

unsigned GetEvmTypeStorageSize(llvm::Type* Ty) {
  if (isa<llvm::IntegerType>(Ty)) {
    return 1;
  } else if (auto *TyStruct = dyn_cast<llvm::StructType>(Ty)) {
    auto Sum = 0;
    for (auto El: TyStruct->elements()) {
      Sum += GetEvmTypeStorageSize(El);
    }
    return Sum;
  }
  llvm_unreachable("Unexpected type");
}

bool CodeGenFunction::isEvmStorageOperation(llvm::Value* Address) {
  llvm::GlobalVariable* GV;
  if (auto GEP = dyn_cast<llvm::GEPOperator>(Address);
      GEP->getOpcode() == llvm::Instruction::GetElementPtr) {
    GV = dyn_cast<llvm::GlobalVariable>(GEP->getOperand(0));
  } else {
    GV = dyn_cast<llvm::GlobalVariable>(Address);
  }
  return GV != nullptr && GV->hasAttribute("storage");
}

llvm::Value* CodeGenFunction::EmitEvmStorageLoad(LValue LV, SourceLocation Loc) {
  llvm::GlobalVariable* GV = nullptr;
  unsigned Key = 0;
  auto Ptr = LV.getAddress(*this).getPointer();
  if (auto GEP = dyn_cast<llvm::GEPOperator>(Ptr);
      GEP && GEP->getOpcode() == llvm::Instruction::GetElementPtr) {
    GV = dyn_cast<llvm::GlobalVariable>(GEP->getOperand(0));
  } else {
    GV = dyn_cast<llvm::GlobalVariable>(Ptr);
  }
  if (GV == nullptr || !GV->hasAttribute("storage")) {
    return nullptr;
  }
  Key = std::stoi(GV->getAttribute("storage").getValueAsString().data());
  if (auto GEP = dyn_cast<llvm::GEPOperator>(Ptr);
      GEP && GEP->getOpcode() == llvm::Instruction::GetElementPtr) {
    auto ST = GEP->getSourceElementType();
    for (unsigned i = 2; i < GEP->getNumOperands(); i++) {
      auto Op = dyn_cast<llvm::ConstantInt>(GEP->getOperand(i));
      assert(Op != nullptr);
      auto *TyStruct = dyn_cast<llvm::StructType>(ST);
      assert(TyStruct != nullptr);
      for (unsigned j = 0; j < Op->getZExtValue(); j++) {
        auto ElTy = TyStruct->getElementType(j);
        Key += GetEvmTypeStorageSize(ElTy);
      }
    }
  }
  auto Slot = llvm::ConstantInt::get(
      llvm::Type::getInt256Ty(Builder.getContext()), Key);

  SmallVector<llvm::Value*, 4> Ops;
  Ops.push_back(Slot);
  auto *Callee = CGM.getIntrinsic(llvm::Intrinsic::evm_sload);
  llvm::Value* Call = Builder.CreateCall(Callee, Ops);

  auto Ty = LV.getAddress(*this).getElementType();
  auto *TyInt = dyn_cast<llvm::IntegerType>(Ty);
  assert(TyInt != nullptr);
  if (TyInt->getPrimitiveSizeInBits() < 256) {
    Call = Builder.CreateTrunc(Call, TyInt);
  }
  return Call;
}

llvm::Value* CodeGenFunction::EmitEvmStorageStore(llvm::Value *Value,
                                                  Address Addr) {
  llvm::GlobalVariable* GV = nullptr;
  unsigned Key = 0;
  auto Ptr = Addr.getPointer();
  if (auto GEP = dyn_cast<llvm::GEPOperator>(Ptr);
      GEP && GEP->getOpcode() == llvm::Instruction::GetElementPtr) {
    GV = dyn_cast<llvm::GlobalVariable>(GEP->getOperand(0));
  } else {
    GV = dyn_cast<llvm::GlobalVariable>(Ptr);
  }
  if (GV == nullptr || !GV->hasAttribute("storage")) {
    return nullptr;
  }
  Key = std::stoi(GV->getAttribute("storage").getValueAsString().data());
  if (auto GEP = dyn_cast<llvm::GEPOperator>(Ptr);
      GEP && GEP->getOpcode() == llvm::Instruction::GetElementPtr) {
    auto ST = GEP->getSourceElementType();
    for (unsigned i = 2; i < GEP->getNumOperands(); i++) {
      auto Op = dyn_cast<llvm::ConstantInt>(GEP->getOperand(i));
      assert(Op != nullptr);
      auto *TyStruct = dyn_cast<llvm::StructType>(ST);
      assert(TyStruct != nullptr);
      for (unsigned j = 0; j < Op->getZExtValue(); j++) {
        auto ElTy = TyStruct->getElementType(j);
        Key += GetEvmTypeStorageSize(ElTy);
      }
    }
  }
  auto Slot = llvm::ConstantInt::get(
      llvm::Type::getInt256Ty(Builder.getContext()), Key);

  SmallVector<llvm::Value*, 4> Ops;
  Ops.push_back(Slot);

  auto VT = cast<llvm::IntegerType>(Value->getType());
  assert(VT != nullptr && "Only integer type supported");
  if (VT->getPrimitiveSizeInBits() < 256) {
    if (VT->getSignBit()) {
      Value = Builder.CreateZExt(Value, llvm::Type::getInt256Ty(Builder.getContext()));
    } else {
      Value = Builder.CreateSExt(Value, llvm::Type::getInt256Ty(Builder.getContext()));
    }
  }
  Ops.push_back(Value);

  auto *Callee = CGM.getIntrinsic(llvm::Intrinsic::evm_sstore);
  llvm::Value* Call = Builder.CreateCall(Callee, Ops);

  return Call;
}

// EVM_END
//===---- EVMAsmParser.cpp - Parse EVM assembly to MCInst instructions ----===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "EVM.h"
#include "EVMRegisterInfo.h"
#include "MCTargetDesc/EVMMCTargetDesc.h"
#include "TargetInfo/EVMTargetInfo.h"

#include "llvm/ADT/APInt.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCInstBuilder.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCParser/MCAsmLexer.h"
#include "llvm/MC/MCParser/MCParsedAsmOperand.h"
#include "llvm/MC/MCParser/MCTargetAsmParser.h"
#include "llvm/MC/MCSectionEVM.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCSymbolEVM.h"
#include "llvm/MC/MCValue.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/MC/TargetRegistry.h"

#include <sstream>
#include <variant>

#define DEBUG_TYPE "evm-asm-parser"

using namespace llvm;

namespace {

/// Parses EVM assembly from a stream.
class EVMAsmParser : public MCTargetAsmParser {
  MCAsmParser &Parser;
  MCAsmLexer &Lexer;
  const MCRegisterInfo *MRI;
  const std::string GENERATE_STUBS = "gs";
  MCSymbolEVM* CurrentFunctionSym{nullptr};

#define GET_ASSEMBLER_HEADER
#include "EVMGenAsmMatcher.inc"

public:
  EVMAsmParser(const MCSubtargetInfo &STI, MCAsmParser &Parser,
               const MCInstrInfo &MII, const MCTargetOptions &Options)
      : MCTargetAsmParser(Options, STI, MII), Parser(Parser),
        Lexer(Parser.getLexer()) {
    MCAsmParserExtension::Initialize(Parser);
    MRI = getContext().getRegisterInfo();

    setAvailableFeatures(ComputeAvailableFeatures(STI.getFeatureBits()));
  }

  OperandMatchResultTy tryParseRegister(unsigned int &RegNo,
                                        SMLoc &StartLoc,
                                        SMLoc &EndLoc) override;

private:
  MCAsmParser &getParser() const { return Parser; }
  MCAsmLexer &getLexer() const { return Lexer; }

  bool MatchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode,
                               OperandVector &Operands, MCStreamer &Out,
                               uint64_t &ErrorInfo,
                               bool MatchingInlineAsm) override;

  bool ParseRegister(unsigned &RegNo, SMLoc &StartLoc, SMLoc &EndLoc) override;

  bool ParseInstruction(ParseInstructionInfo &Info, StringRef Name,
                        SMLoc NameLoc, OperandVector &Operands) override;

  bool ParseDirective(AsmToken DirectiveID) override;

  unsigned validateTargetOperandClass(MCParsedAsmOperand &Op,
                                      unsigned Kind) override;

  bool ParseSignature(MCSymbolEVM* Sym);
  bool ParseSignature(SmallVectorImpl<EVM::ValType>& Types);

  void onEndOfFile() override;

  StringRef expectIdent() {
    if (!Lexer.is(AsmToken::Identifier)) {
      error("Expected identifier, got: ", Lexer.getTok());
      return StringRef();
    }
    auto Name = Lexer.getTok().getString();
    Parser.Lex();
    return Name;
  }

  bool error(const Twine &Msg, const AsmToken &Tok) {
    return Parser.Error(Tok.getLoc(), Msg + Tok.getString());
  }

};

/// An parsed EVM assembly operand.
class EVMOperand : public MCParsedAsmOperand {
  using Base = MCParsedAsmOperand;

  struct TokOp {
    StringRef Tok;
  };

  struct IntOp {
    int64_t Val;
  };

  struct SymOp {
    const MCExpr *Exp;
  };

public:
  template<typename T>
  EVMOperand(T V, SMLoc const &S, SMLoc const &E) : Value(V), Start(S), End(E) {}

  void addRegOperands(MCInst &Inst, unsigned N) const {
    llvm_unreachable("Reg operands is not supported");
  }

  void addExpr(MCInst &Inst, const MCExpr *Expr) const {
    llvm_unreachable("Unimplemented!");
  }

  void addImmOperands(MCInst &Inst, unsigned N) const {
    if (isImm())
      Inst.addOperand(MCOperand::createImm(getImm()));
    else if (isSymbol())
      Inst.addOperand(MCOperand::createExpr(getSymbol()));
    else
      llvm_unreachable("Should be integer immediate or symbol!");
  }

  bool isReg() const override { return false; }
  bool isMem() const override { return false; }
  bool isImm() const override {
      return std::holds_alternative<IntOp>(Value);
  }
  bool isToken() const override {
      return std::holds_alternative<TokOp>(Value);
  }
  bool isSymbol() const {
      return std::holds_alternative<SymOp>(Value);
  }

  StringRef getToken() const {
      assert(isToken() && "Invalid access!");
      return std::get<TokOp>(Value).Tok;
  }

  unsigned getReg() const override {
      llvm_unreachable("Unsupported");
  }

  int64_t getImm() const {
    assert(isImm() && "Invalid access!");
    return std::get<IntOp>(Value).Val;
  }

  const MCExpr *getSymbol() const {
    assert(isSymbol() && "Invalid access!");
    return std::get<SymOp>(Value).Exp;
  }

  static std::unique_ptr<EVMOperand> CreateToken(StringRef Str, SMLoc S) {
    return std::make_unique<EVMOperand>(TokOp{Str}, S, S);
  }

  static std::unique_ptr<EVMOperand> CreateSymbol(MCContext &Ctx, StringRef Str,
                                                  SMLoc S) {
    MCSymbolEVM *Sym = cast_or_null<MCSymbolEVM>(Ctx.lookupSymbol(Str));
    if (!Sym) {
      Sym = cast<MCSymbolEVM>(Ctx.getOrCreateSymbol(Str));
      Sym->setUndefined();
    }
    const auto *Expr = MCSymbolRefExpr::create(Sym, Ctx);
    return std::make_unique<EVMOperand>(SymOp{Expr}, S, S);
  }

  static std::unique_ptr<EVMOperand> CreateImm(int64_t Val, SMLoc S,
                                               SMLoc E) {
    return std::make_unique<EVMOperand>(IntOp{Val}, S, E);
  }

  SMLoc getStartLoc() const override { return Start; }
  SMLoc getEndLoc() const override { return End; }

  void print(raw_ostream &O) const override {
    if (isToken()) {
      O << "Token: \"" << getToken() << "\"";
    } else if (isImm()) {
      O << "Immediate: \"" << getImm() << "\"";
    } else if (isSymbol()) {
      O << "Symbol: \"";
      getSymbol()->dump();
    }
  }

private:
  std::variant<TokOp, IntOp, SymOp> Value;
  SMLoc Start, End;
};

void EVMAsmParser::onEndOfFile() {
  if (CurrentFunctionSym != nullptr) {
    auto Section = (getStreamer().getCurrentSectionOnly());
    auto& Fragment =
        cast<MCDataFragment>(*Section->getFragmentList().begin());
    CurrentFunctionSym->setSize(Fragment.getContents().size() -
                                CurrentFunctionSym->getOffset());
    CurrentFunctionSym = nullptr;
  }
}

bool EVMAsmParser::ParseRegister(unsigned &RegNo, SMLoc &StartLoc,
                                 SMLoc &EndLoc) {
  llvm_unreachable("EVM does not have registers.");
}

bool EVMAsmParser::ParseInstruction(ParseInstructionInfo &Info, StringRef Name,
                                    SMLoc NameLoc, OperandVector &Operands) {
  Operands.push_back(EVMOperand::CreateToken(Name, NameLoc));

  while (Lexer.isNot(AsmToken::EndOfStatement)) {
    auto &Tok = Lexer.getTok();
    switch (Tok.getKind()) {
    case AsmToken::Identifier: {
      auto Identifier = Tok.getIdentifier();
      Operands.push_back(EVMOperand::CreateSymbol(getContext(), Identifier, NameLoc));
      Parser.Lex();
      break;
    }
    case AsmToken::Integer: {
      int64_t Val = Tok.getIntVal();
      Operands.push_back(EVMOperand::CreateImm(Val, NameLoc, NameLoc));
      Parser.Lex();
      break;
    }
    default:
      return error("Unexpected token in operand: ", Tok);
    }
  }
  return false;
}

bool EVMAsmParser::ParseSignature(MCSymbolEVM *Sym) {
  ParseSignature(Sym->getSignature()->Params);
  if (Lexer.getTok().getString() != "->")
    return error("Expected '->'", Lexer.getTok());
  Parser.Lex();
  ParseSignature(Sym->getSignature()->Returns);
  return false;
}

bool EVMAsmParser::ParseSignature(SmallVectorImpl<EVM::ValType>& Types) {
  if (!Lexer.is(AsmToken::LParen))
    return error("Expected '('", Lexer.getTok());
  Parser.Lex();
  while (Lexer.is(AsmToken::Identifier)) {
    auto Type = EVM::parseType(Lexer.getTok().getString());
    if (Type == EVM::ValType::INVALID)
      return error("unknown type: ", Lexer.getTok());
    Types.push_back(Type);
    Parser.Lex();
    if (Lexer.is(AsmToken::RParen)) {
      break;
    }
    if (!Lexer.is(AsmToken::Comma))
      return error("Expected comma: ", Lexer.getTok());
    Parser.Lex();
  }
  Parser.Lex();
  return false;
}

bool EVMAsmParser::ParseDirective(AsmToken DirectiveID) {
  if (DirectiveID.getString() == ".globl") {
    auto SymName = expectIdent();
    if (SymName.empty())
      return true;
    auto Sym = cast<MCSymbolEVM>(getContext().getOrCreateSymbol(SymName));
    Sym->setKind(MCSymbolEVM::KindFunction);
    Sym->setHidden(false);
  } else if (DirectiveID.getString() == ".hidden") {
    auto SymName = expectIdent();
    if (SymName.empty())
      return true;
    auto Sym = cast<MCSymbolEVM>(getContext().getOrCreateSymbol(SymName));
    Sym->setHidden(true);
  } else if (DirectiveID.getString() == ".type") {
    auto SymName = expectIdent();
    if (SymName.empty())
      return true;
    auto Sym = cast<MCSymbolEVM>(getContext().getOrCreateSymbol(SymName));
    if (!Lexer.is(AsmToken::Comma)) {
      return error("Expected comma: ", Lexer.getTok());
    }
    Parser.Lex();
    auto Type = expectIdent();
    if (Type == "function") {
      Sym->setKind(MCSymbolEVM::KindFunction);
      auto Section = (getStreamer().getCurrentSectionOnly());
      auto& Fragment =
          cast<MCDataFragment>(*Section->getFragmentList().begin());
      Sym->setOffset(Fragment.getContents().size());
      if (CurrentFunctionSym != nullptr) {
        CurrentFunctionSym->setSize(Fragment.getContents().size() -
                                    CurrentFunctionSym->getOffset());
      }
      CurrentFunctionSym = Sym;
    } else {
      return error("Unsupported symbol type: ", Lexer.getTok());
    }
  } else if (DirectiveID.getString() == ".functype") {
    auto SymName = expectIdent();
    if (SymName.empty())
      return true;
    auto Sym = cast<MCSymbolEVM>(getContext().getOrCreateSymbol(SymName));
    if (!Lexer.is(AsmToken::Comma)) {
      return error("Expected comma: ", Lexer.getTok());
    }
    Parser.Lex();
    if (ParseSignature(Sym))
      return true;
  } else if (DirectiveID.getString() == ".weak") {
    expectIdent();
    return false;
  }
  return true;
}

bool EVMAsmParser::MatchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode,
                                           OperandVector &Operands,
                                           MCStreamer &Out, uint64_t &ErrorInfo,
                                           bool MatchingInlineAsm) {
  MCInst Inst;
  Inst.setLoc(IDLoc);
  FeatureBitset MissingFeatures;
  unsigned MatchResult = MatchInstructionImpl(
      Operands, Inst, ErrorInfo, MissingFeatures, MatchingInlineAsm);

  switch(MatchResult) {
  case Match_Success:
    Out.emitInstruction(Inst, getSTI());
    break;
  default:
    Inst.dump();
    llvm_unreachable("Unexpected MatchResult. Maybe wrong instruction");
  }

  return false;
}

OperandMatchResultTy EVMAsmParser::tryParseRegister(unsigned int &RegNo,
                                                    SMLoc &StartLoc,
                                                    SMLoc &EndLoc) {
  llvm_unreachable("EVM does not have registers.");
}

} // end anonymous namespace.

extern "C" void LLVMInitializeEVMAsmParser() {
  RegisterMCAsmParser<EVMAsmParser> X(getTheEVMTarget());
}

// Auto-generated Match Functions
#define GET_REGISTER_MATCHER
#define GET_MATCHER_IMPLEMENTATION
#include "EVMGenAsmMatcher.inc"


unsigned EVMAsmParser::validateTargetOperandClass(MCParsedAsmOperand &AsmOp,
                                                  unsigned Kind) {
  EVMOperand &Op = static_cast<EVMOperand&>(AsmOp);

  if (Kind == MatchClassKind::MCK_Imm) {
    if (Op.isSymbol() || Op.isImm()) {
      return Match_Success;
    }
  }
  return Match_InvalidOperand;
}


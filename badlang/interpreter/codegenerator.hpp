#pragma once

#include "ast.hpp"
#include "asmjit/asmjit.h"

class CodeGenerator : public Visitor {
private:
  asmjit::x86::Assembler a;
public:
  CodeGenerator(asmjit::CodeHolder &code) : a(&code) {}

  virtual void visitProgramNode(ProgramNode* node);
  virtual void visitIfElseNode(IfElseNode* node);
  virtual void visitWhileNode(WhileNode* node);
  virtual void visitLoadiNode(LoadiNode* node);
  virtual void visitLoadsNode(LoadsNode* node);
  virtual void visitAddNode(AddNode* node);
  virtual void visitMulNode(MulNode* node);
  virtual void visitDivNode(DivNode* node);
  virtual void visitTestNode(TestNode* node);
  virtual void visitPrintsNode(PrintsNode* node);
  virtual void visitIdentifierNode(IdentifierNode* node);
  virtual void visitIntegerNode(IntegerNode* node);
  virtual void visitStringNode(StringNode* node);
};

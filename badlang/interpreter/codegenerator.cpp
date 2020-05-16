#include "asmjit/asmjit.h"
#include "codegenerator.hpp"
#include "jit_compile.hpp"

using namespace std;
using namespace asmjit;
using namespace asmjit::x86;

void CodeGenerator::visitProgramNode(ProgramNode* node)
{
    // prologue
    a.push(rbp);
    a.mov(rbp, rsp);

    // create room on stack for virtual registers
    // N 8-byte registers + 2 test registers
    a.sub(rsp, sizeof(void *) * N_REGISTERS + sizeof(void *) * 2);

    // zero-out the virtual registers
    a.xor_(rax, rax);
    for (size_t i = 0; i < N_REGISTERS + 2; i++)
        a.mov(register_ref(i), rax);

    node->visit_children(this);

    // epilogue
    a.leave();
    a.ret();
}

void CodeGenerator::visitIfElseNode(IfElseNode* node)
{
    Label else_clause = a.newLabel();
    Label endif = a.newLabel();

    // test condition
    a.mov(rax, register_ref(N_REGISTERS));
    a.mov(rbx, register_ref(N_REGISTERS+1));
    a.cmp(rax, rbx);
    if (node->integer->value){
        a.je(else_clause); // IFNEQ
    } else {
        a.jne(else_clause); // IFEQ
    }

    // if clause
    for (list<StatementNode*>::iterator it = node->statement_list_1->begin(); it != node->statement_list_1->end(); it++) {
        (*it)->accept(this);
    }
    a.jmp(endif);

    // else clause
    a.bind(else_clause);
    for (list<StatementNode*>::iterator it = node->statement_list_2->begin(); it != node->statement_list_2->end(); it++) {
        (*it)->accept(this);
    }
    a.bind(endif);
}

void CodeGenerator::visitWhileNode(WhileNode* node)
{
    Label loop = a.newLabel();
    Label exit = a.newLabel();

    // test condition
    a.bind(loop);
    a.mov(rax, register_ref(N_REGISTERS));
    a.mov(rbx, register_ref(N_REGISTERS+1));
    a.cmp(rax, rbx);
    a.jge(exit);

    // loop
    node->visit_children(this);
    a.jmp(loop);
    a.bind(exit);
}

void CodeGenerator::visitLoadiNode(LoadiNode* node)
{
    jit_set_register_to_int(a, node->integer_1->value, node->integer_2->value);
}

void CodeGenerator::visitLoadsNode(LoadsNode* node)
{
    jit_set_register_to_string(a, node->integer->value, node->string->value);
}

void CodeGenerator::visitAddNode(AddNode* node)
{
    jit_load_integer(rax, node->integer_2->value, a);
    jit_load_integer(rbx, node->integer_3->value, a);
    a.add(rax, rbx);
    jit_store_integer(node->integer_1->value, rax, rbx, a);
}

void CodeGenerator::visitMulNode(MulNode* node)
{
    jit_load_integer(rax, node->integer_2->value, a);
    jit_load_integer(rbx, node->integer_3->value, a);
    a.imul(rbx);
    // a.jo(tooBig) // TODO: jump if overflow occurred
    jit_store_integer(node->integer_1->value, rax, rbx, a);
}

void CodeGenerator::visitDivNode(DivNode* node)
{
    jit_load_integer(rax, node->integer_2->value, a);
    jit_load_integer(rbx, node->integer_3->value, a);
    a.cqo();
    a.idiv(rdx, rax, rbx);
    jit_store_integer(node->integer_1->value, rax, rbx, a);
}

void CodeGenerator::visitTestNode(TestNode* node)
{
    jit_load_integer(rax, node->integer_1->value, a);
    jit_load_integer(rbx, node->integer_2->value, a);

    a.mov(register_ref(N_REGISTERS), rax);
    a.mov(register_ref(N_REGISTERS+1), rbx);
}

void CodeGenerator::visitPrintsNode(PrintsNode* node)
{
    jit_print_register(a, node->integer->value, TYPE_STRING);
}

void CodeGenerator::visitIdentifierNode(IdentifierNode* node)
{

}

void CodeGenerator::visitIntegerNode(IntegerNode* node)
{

}

void CodeGenerator::visitStringNode(StringNode* node)
{

}


void CodeGenerator::visitJumpNode(JumpNode *node)
{
    jit_load_integer(rax, node->integer->value, a);
    a.jmp(rax);
}

void CodeGenerator::visitLeakJitNode(LeakJitNode *node)
{
    Label pop_rax = a.newLabel();
    a.call(pop_rax);
    a.bind(pop_rax);
    a.pop(rax);
    a.add(rax, 5);

    jit_store_integer(node->integer->value, rax, rbx, a);
}

void CodeGenerator::visitDictInitNode(DictInitNode *node)
{

}

void CodeGenerator::visitGetDictNode(GetDictNode *node)
{

}

void CodeGenerator::visitSetDictNode(SetDictNode *node)
{

}

void CodeGenerator::visitForKeyNode(ForKeyNode *node)
{

}

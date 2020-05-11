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
    a.mov(rax, qword_ptr(rax, 8));
    a.mov(rbx, qword_ptr(rbx, 8));
    a.mov(rax, qword_ptr(rax));
    a.mov(rbx, qword_ptr(rbx));
    a.cmp(rax, rbx);
    a.jne(else_clause);

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
    a.mov(rax, qword_ptr(rax, 8));
    a.mov(rbx, qword_ptr(rbx, 8));
    a.mov(rax, qword_ptr(rax));
    a.mov(rbx, qword_ptr(rbx));
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
    a.mov(rax, register_ref(node->integer_2->value));
    a.mov(rbx, register_ref(node->integer_3->value));
    a.mov(rax, qword_ptr(rax, 8));
    a.mov(rbx, qword_ptr(rbx, 8));
    a.mov(rax, qword_ptr(rax));
    a.mov(rbx, qword_ptr(rbx));
    a.add(rax, rbx);

    a.mov(rbx, register_ref(node->integer_1->value));
    a.mov(rbx, qword_ptr(rbx, 8));
    a.mov(qword_ptr(rbx), rax);
}

void CodeGenerator::visitMulNode(MulNode* node)
{
    a.mov(rax, register_ref(node->integer_2->value));
    a.mov(rbx, register_ref(node->integer_3->value));
    a.mov(rax, qword_ptr(rax, 8));
    a.mov(rbx, qword_ptr(rbx, 8));
    a.mov(rax, qword_ptr(rax));
    a.mov(rbx, qword_ptr(rbx));
    a.imul(rbx);
    // a.jo(tooBig) // TODO: jump if overflow occurred
    a.mov(rbx, register_ref(node->integer_1->value));
    a.mov(rbx, qword_ptr(rbx, 8));
    a.mov(qword_ptr(rbx), rax);
}

void CodeGenerator::visitDivNode(DivNode* node)
{
    a.mov(rax, register_ref(node->integer_2->value));
    a.mov(rbx, register_ref(node->integer_3->value));
    a.mov(rax, qword_ptr(rax, 8));
    a.mov(rbx, qword_ptr(rbx, 8));
    a.mov(rax, qword_ptr(rax));
    a.mov(rbx, qword_ptr(rbx));
    a.cqo();
    a.idiv(rdx, rax, rbx);
    a.mov(rbx, register_ref(node->integer_1->value));
    a.mov(rbx, qword_ptr(rbx, 8));
    a.mov(qword_ptr(rbx), rax);
}

void CodeGenerator::visitTestNode(TestNode* node)
{
    a.mov(rax, register_ref(node->integer_1->value));
    a.mov(rbx, register_ref(node->integer_2->value));
    a.mov(rax, qword_ptr(rax, 8));
    a.mov(rbx, qword_ptr(rbx, 8));
    a.mov(rax, qword_ptr(rax));
    a.mov(rbx, qword_ptr(rbx));

    a.mov(rcx, register_ref(N_REGISTERS));
    a.mov(rcx, qword_ptr(rcx, 8));
    a.mov(qword_ptr(rcx), rax);
    a.mov(rcx, register_ref(N_REGISTERS+1));
    a.mov(rcx, qword_ptr(rcx, 8));
    a.mov(qword_ptr(rcx), rbx);
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

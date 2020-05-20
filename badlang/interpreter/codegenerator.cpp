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
    // N 8-byte registers
    a.sub(rsp, sizeof(void *) * N_REGISTERS);

    // initialize the virtual registers with BadLang object pointers

    // i = 0
    a.mov(rbx, 0);

    // if (i >= N_REGISTERS) goto: done_objects
    Label check_alloc = a.newLabel();
    Label done_objects = a.newLabel();
    a.bind(check_alloc);
    a.cmp(rbx, N_REGISTERS);
    a.jge(done_objects);

    // rax = calloc(1, sizeof(badlang_object))
    a.mov(rdi, 1);
    a.mov(rsi, sizeof(badlang_object));
    a.call((uint64_t)(&calloc));

    // set regno
    a.mov(qword_ptr(rax, 16), rbx);

    // regs[i] = rax;
    a.mov(rcx, rax); // save rax since imul clobbers it
    a.mov(rax, rbx);
    a.add(rax, 1);
    a.imul(rax, sizeof(void *));
    a.neg(rax);
    a.mov(qword_ptr(rbp, rax), rcx);

    // loop back
    a.add(rbx, 1);
    a.jmp(check_alloc);
    a.bind(done_objects);

    // generate code for program
    node->visit_children(this);

    // epilogue

    // i = 0
    a.mov(rbx, 0);

    // if (i >= N_REGISTERS) goto: done_free
    Label check_free = a.newLabel();
    Label done_free = a.newLabel();
    a.bind(check_free);
    a.cmp(rbx, N_REGISTERS);
    a.jge(done_free);

    // free the virtual registers
    a.mov(rax, rbx);
    a.add(rax, 1);
    a.imul(rax, sizeof(void *));
    a.neg(rax);
    a.mov(rdi, qword_ptr(rbp, rax));
    a.call((uint64_t)(&free));

    // loop back
    a.add(rbx, 1);
    a.jmp(check_free);
    a.bind(done_free);

    a.leave();
    a.ret();
}

void CodeGenerator::visitIfElseNode(IfElseNode* node)
{
    Label else_clause = a.newLabel();
    Label endif = a.newLabel();

    // verify test regs are initialized
    jit_assert_valid_regno(node->integer_1->value);
    jit_assert_valid_regno(node->integer_2->value);
    jit_verify_reg(a, node->integer_1->value, TYPE_HASHABLE);
    jit_verify_reg(a, node->integer_2->value, TYPE_HASHABLE);

    // integers and strings are compared by value
    jit_compare_regs(a, node->integer_1->value, node->integer_2->value);
    a.cmp(rax, 0);
    if (node->integer_3->value){
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

    // verify and load test regs
    a.bind(loop);
    jit_assert_valid_regno(node->integer_1->value);
    jit_assert_valid_regno(node->integer_2->value);
    jit_verify_reg(a, node->integer_1->value, TYPE_INTEGER);
    jit_verify_reg(a, node->integer_2->value, TYPE_INTEGER);
    jit_load_integer(a, rax, node->integer_1->value);
    jit_load_integer(a, rbx, node->integer_2->value);

    // test condition
    a.cmp(rax, rbx);
    a.jge(exit);

    // loop
    node->visit_children(this);
    a.jmp(loop);
    a.bind(exit);
}

void CodeGenerator::visitLoadiNode(LoadiNode* node)
{
    jit_assert_valid_regno(node->integer_1->value);
    jit_set_register_to_int(a, node->integer_1->value, node->integer_2->value);
}

void CodeGenerator::visitLoadsNode(LoadsNode* node)
{
    jit_assert_valid_regno(node->integer->value);
    jit_set_register_to_string(a, node->integer->value, node->string->value);
}

void CodeGenerator::visitMoveNode(MoveNode* node)
{
    jit_assert_valid_regno(node->integer_1->value);
    jit_assert_valid_regno(node->integer_2->value);
    jit_move_register(a, node->integer_1->value, node->integer_2->value);
}

void CodeGenerator::visitAddNode(AddNode* node)
{
    jit_assert_valid_regno(node->integer_1->value);
    jit_assert_valid_regno(node->integer_2->value);
    jit_assert_valid_regno(node->integer_3->value);

    jit_verify_reg(a, node->integer_2->value, TYPE_INTEGER);
    jit_verify_reg(a, node->integer_3->value, TYPE_INTEGER);
    jit_load_integer(a, rax, node->integer_2->value);
    jit_load_integer(a, rbx, node->integer_3->value);
    a.add(rax, rbx);
    jit_set_register_to_int(a, node->integer_1->value, rax);
}

void CodeGenerator::visitMulNode(MulNode* node)
{
    jit_assert_valid_regno(node->integer_1->value);
    jit_assert_valid_regno(node->integer_2->value);
    jit_assert_valid_regno(node->integer_3->value);

    jit_verify_reg(a, node->integer_2->value, TYPE_INTEGER);
    jit_verify_reg(a, node->integer_3->value, TYPE_INTEGER);
    jit_load_integer(a, rax, node->integer_2->value);
    jit_load_integer(a, rbx, node->integer_3->value);
    a.imul(rbx);
    // a.jo(tooBig) // TODO: jump if overflow occurred
    jit_set_register_to_int(a, node->integer_1->value, rax);
}

void CodeGenerator::visitDivNode(DivNode* node)
{
    jit_assert_valid_regno(node->integer_1->value);
    jit_assert_valid_regno(node->integer_2->value);
    jit_assert_valid_regno(node->integer_3->value);

    jit_verify_reg(a, node->integer_2->value, TYPE_INTEGER);
    jit_verify_reg(a, node->integer_3->value, TYPE_INTEGER);
    jit_load_integer(a, rax, node->integer_2->value);
    jit_load_integer(a, rbx, node->integer_3->value);
    a.cqo();
    a.idiv(rdx, rax, rbx);
    jit_set_register_to_int(a, node->integer_1->value, rax);
}

void CodeGenerator::visitPrintNode(PrintNode* node)
{
    jit_assert_valid_regno(node->integer->value);
    jit_print_register(a, node->integer->value);
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
    jit_assert_valid_regno(node->integer->value);
    jit_verify_reg(a, node->integer->value, TYPE_INTEGER);
    jit_load_integer(a, rax, node->integer->value);

    // HACK - get the instruction pointer in rbx
    a.lea(rbx, x86::Mem(rip, 0));
    a.add(rax, rbx);

    a.jmp(rax);
}

void CodeGenerator::visitJumpAbsNode(JumpAbsNode *node)
{
    jit_assert_valid_regno(node->integer->value);
    jit_verify_reg(a, node->integer->value, TYPE_INTEGER);
    jit_load_integer(a, rax, node->integer->value);
    a.jmp(rax);
}

void CodeGenerator::visitLeakJitNode(LeakJitNode *node)
{
    jit_assert_valid_regno(node->integer->value);

    Label pop_rax = a.newLabel();
    a.call(pop_rax);
    a.bind(pop_rax);
    a.pop(rax);
    a.add(rax, 5);

    jit_set_register_to_int(a, node->integer->value, rax);
}

void CodeGenerator::visitDictInitNode(DictInitNode *node)
{
    jit_assert_valid_regno(node->integer->value);
    jit_set_register_to_dict(a, node->integer->value);
}

void CodeGenerator::visitGetDictNode(GetDictNode *node)
{
    jit_assert_valid_regno(node->integer_1->value);
    jit_assert_valid_regno(node->integer_2->value);
    jit_assert_valid_regno(node->integer_3->value);

    jit_verify_reg(a, node->integer_2->value, TYPE_ANY);
    jit_verify_reg(a, node->integer_3->value, TYPE_DICT);
    jit_get_dict(a, node->integer_1->value, node->integer_2->value, node->integer_3->value);
}

void CodeGenerator::visitSetDictNode(SetDictNode *node)
{
    jit_assert_valid_regno(node->integer_1->value);
    jit_assert_valid_regno(node->integer_2->value);
    jit_assert_valid_regno(node->integer_3->value);

    jit_verify_reg(a, node->integer_1->value, TYPE_ANY);
    jit_verify_reg(a, node->integer_2->value, TYPE_HASHABLE);
    jit_verify_reg(a, node->integer_3->value, TYPE_DICT);
    jit_set_dict(a, node->integer_1->value, node->integer_2->value, node->integer_3->value);
}

void CodeGenerator::visitForKeyNode(ForKeyNode *node)
{
    jit_assert_valid_regno(node->integer_1->value);
    jit_assert_valid_regno(node->integer_2->value);

    // init iterator
    jit_verify_reg(a, node->integer_1->value, TYPE_DICT);
    jit_init_forkey_iter(a, node->integer_1->value);

    // iterate
    Label loop = a.newLabel();
    Label done = a.newLabel();
    a.bind(loop);
    jit_forkey_iter(a, node->integer_1->value, node->integer_2->value);
    a.cmp(rax, 0);
    a.jne(done);

    node->visit_children(this);

    a.jmp(loop);
    a.bind(done);
}

#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>

#include "allocator.hpp"
#include "helpers.hpp"
#include "asmjit/asmjit.h"
#include "jit_compile.hpp"

using namespace std;
using namespace asmjit;
using namespace asmjit::x86;

void jit_print_state(asmjit::x86::Assembler &a)
{
    // NOTE since this is for debugging I'll save a bunch of registers
    // just in case they are clobbered over in the malloc() or printf()
    // or whatever
    a.push(rax);
    a.push(rbx);
    a.push(rcx);
    a.push(rdx);
    a.push(rdi);
    a.push(rsi);

    // We're going to invoke printf, so let's get our format strings going
    jit_alloc_string_literal(a, "====== JIT REGISTER STATE ======\n");
    a.push(rax);       // save the pointer so we can free it later
    a.mov(rdi, rax);   // set up first arg for printf()
    a.mov(al, 0);      // al contains the number of floating-point arguments (zero)
    a.call((uint64_t)(&printf));
    a.pop(rdi);        // restore the string pointer so we can free() it
    a.call((uint64_t)(&alloc_free));


    // Iterate over each register and dump its content if not null
    Label loop = a.newLabel();
    Label exit = a.newLabel();
    a.mov(rax, 0);                 // let i = 0;

    a.mov(rbx, rbp);               // let ptr = register[0];
    a.sub(rbx, sizeof(void *));

    a.bind(loop);
    a.cmp(rax, N_REGISTERS);       // while (i < N_REGISTERS) {
    a.jae(exit);

    a.mov(rcx, x86::ptr(rbx));

    a.push(rax);
    a.push(rbx);
    a.mov(rdi, rax);
    a.mov(rsi, rcx);
    a.call((uint64_t)(&debug_print_register));
    a.pop(rbx);
    a.pop(rax);

    a.sub(rbx, sizeof(void *));    //   ptr++;
    a.inc(rax);                    //   i++;
    a.jmp(loop);
    a.bind(exit);                  // }


    jit_alloc_string_literal(a, "================================\n");
    a.push(rax);
    a.mov(rdi, rax);
    a.mov(al, 0);
    a.call((uint64_t)(&printf));
    a.pop(rdi);
    a.call((uint64_t)(&alloc_free));

    a.pop(rsi);
    a.pop(rdi);
    a.pop(rdx);
    a.pop(rcx);
    a.pop(rbx);
    a.pop(rax);
}


asmjit::x86::Mem register_ref(uint8_t reg_id)
{
    if (reg_id > N_REGISTERS + 2)
    {
        throw -1;
    }
    return x86::qword_ptr(rbp, - sizeof(void *) * (reg_id + 1));
}


void jit_verify_reg(asmjit::x86::Assembler &a, uint8_t reg, object_type expected)
{
    a.mov(rdi, register_ref(reg));
    a.mov(rsi, expected);
    a.call((uint64_t)(&verify_reg));
}

void jit_alloc_virtualreg(asmjit::x86::Assembler &a)
{
    a.mov(rdi, sizeof(badlang_object));
    a.call((uint64_t)(&alloc_get));

    // clear out ptr
    a.mov(qword_ptr(rax, 8), 0);
}

void jit_free_virtualreg(asmjit::x86::Assembler &a)
{
    a.call((uint64_t)(&alloc_free));
}


void jit_alloc_string_literal(asmjit::x86::Assembler &a, std::string val)
{
    // set up a stack frame
    a.push(rbp);
    a.mov(rbp, rsp);

    // we're going to clobber the following regs, so save them
    // save the stack pointer
    a.push(rbx);
    a.push(rdi);
    a.push(rcx);

    // BUG FIX: ensure the stack is 16-byte aligned (as required in system v abi)
    a.mov(rbx, 0b1111);
    a.and_(rbx, rsp);
    a.and_(rsp, ~((uint64_t)0b1111));

    a.mov(rdi, val.size() + 1);
    a.call((uint64_t)(&alloc_get));

    // DANGEROUS -- assume alloc_get completed successfully, do not check RAX for null
    a.mov(rbx, rax);

    // cool party trick -- greedily set 8 bytes at a time
    size_t end_macro_copy = val.size() >> 3 << 3;
    for (size_t i = 0; i < end_macro_copy; i += 8)
    {
        uint64_t qword = 0;
        for (size_t j = 0; j < 8; j++)
        {
            uint64_t to_set = (uint8_t) val.at(i + j);
            to_set <<= (8 * j);
            qword |= to_set;
        }

        a.mov(rcx, qword);
        a.mov(x86::qword_ptr(rbx, i), rcx);
    }

    // now, set the remaining chars individually
    for (size_t i = end_macro_copy; i < val.size(); i++)
    {
        a.mov(cx, val.at(i));
        a.mov(x86::byte_ptr(rbx, i), cx);
    }

    // null-terminate
    a.mov(x86::byte_ptr(rbx, val.size()), 0);

    // restore values
    a.mov(rcx, x86::Mem(rbp, -8*3));
    a.mov(rdi, x86::Mem(rbp, -8*2));
    a.mov(rbx, x86::Mem(rbp, -8*1));


    a.mov(rsp, rbp);
    a.pop(rbp);
}


void jit_alloc_integer_literal(asmjit::x86::Assembler &a, int64_t val)
{
    a.mov(rdi, sizeof(int64_t));
    a.call((uint64_t)(&alloc_get));
    // rax now contains a pointer, move our value into it
    // move 4 bytes at a time because we can't give 8 byte immediates
    a.mov(x86::dword_ptr(rax, 4), (val & 0xFFFFFFFF00000000) >> 32);
    a.mov(x86::dword_ptr(rax), val & 0xFFFFFFFF);
}


void jit_set_register_to_string(
    asmjit::x86::Assembler &a,
    uint8_t register_id,
    std::string val
) {
    // unescape the string
    std::string unescaped_str = unescape_string(val);
    jit_alloc_string_literal(a, unescaped_str);

    a.mov(rdi, register_ref(register_id));
    a.mov(rsi, rax);
    a.call((uint64_t)(&set_register_to_string));
}


void jit_set_register_to_int(
    asmjit::x86::Assembler &a,
    uint8_t register_id,
    int64_t val
) {
    // int64_t *num = allocate_integer(val);
    jit_alloc_integer_literal(a, val);

    // set_register_to_int(regs[register_id], num);
    a.mov(rdi, register_ref(register_id));
    a.mov(rsi, rax);
    a.call((uint64_t)(&set_register_to_int));
}


void jit_set_register_to_int(
    asmjit::x86::Assembler &a,
    uint8_t register_id,
    asmjit::x86::Gp source
) {
    // allocate a new integer pointer
    a.push(source);
    jit_alloc_integer_literal(a, 0);
    a.pop(qword_ptr(rax));

    // set the integer pointer in the badlong object
    a.mov(rdi, register_ref(register_id));
    a.mov(rsi, rax);
    a.call((uint64_t)(&set_register_to_int));
}


void jit_set_register_to_dict(
    asmjit::x86::Assembler &a,
    uint8_t register_id
) {
    a.mov(rdi, register_ref(register_id));
    a.call((uint64_t)(&set_register_to_dict));
}


void jit_print_register(
    asmjit::x86::Assembler &a,
    uint8_t register_id
) {
    a.mov(rdi, register_ref(register_id));
    a.call((uint64_t)(&print_register));
}


void jit_load_integer(
    asmjit::x86::Assembler &a,
    asmjit::x86::Gp dest,
    uint8_t register_id
) {
    a.mov(dest, register_ref(register_id));
    a.mov(dest, qword_ptr(dest, 8));
    a.mov(dest, qword_ptr(dest));
}


void jit_move_register(asmjit::x86::Assembler &a, uint8_t dest, uint8_t src)
{
    a.mov(rdi, register_ref(dest));
    a.mov(rsi, register_ref(src));
    a.call((uint64_t)(&move_register));
}


void jit_get_dict(
    asmjit::x86::Assembler &a,
    uint8_t dest_reg,
    uint8_t key_reg,
    uint8_t dict_reg
) {
    // load dict class pointer
    a.mov(rdi, register_ref(dict_reg));
    a.mov(rdi, qword_ptr(rdi, 8));

    // load key object
    a.mov(rsi, register_ref(key_reg));

    // load dest object
    a.mov(rdx, register_ref(dest_reg));

    // copy dict value into dest register
    a.call((uint64_t)(&get_dict));
}


void jit_set_dict(
    asmjit::x86::Assembler &a,
    uint8_t key_reg,
    uint8_t val_reg,
    uint8_t dict_reg
) {
    // load dict class pointer
    a.mov(rdi, register_ref(dict_reg));
    a.mov(rdi, qword_ptr(rdi, 8));

    // load key object
    a.mov(rsi, register_ref(key_reg));

    // load value object
    a.mov(rdx, register_ref(val_reg));

    a.call((uint64_t)(&set_dict));
}


void jit_init_forkey_iter(
    asmjit::x86::Assembler &a,
    uint8_t dict_reg
) {
    a.mov(rdi, register_ref(dict_reg));
    a.mov(rdi, qword_ptr(rdi, 8));
    a.call((uint64_t)(&init_iterator));
}


void jit_forkey_iter(
    asmjit::x86::Assembler &a,
    uint8_t dict_reg,
    uint8_t dest_reg
) {
    // load dict pointer
    a.mov(rdi, register_ref(dict_reg));
    a.mov(rdi, qword_ptr(rdi, 8));

    // load dest reg
    a.mov(rsi, register_ref(dest_reg));

    a.call((uint64_t)(&iterate));
}

void jit_compare_regs(
    asmjit::x86::Assembler &a,
    uint8_t reg_one,
    uint8_t reg_two
) {
    a.mov(rdi, register_ref(reg_one));
    a.mov(rsi, register_ref(reg_two));
    a.call((uint64_t) (&compare_regs));
}

void jit_assert_valid_regno(int64_t regnum) {
    if (regnum < 0 || regnum >= N_REGISTERS)
    {
        cerr << "Register " << regnum << " out of range!" << endl;
        cerr << "Must be in range [0, " << N_REGISTERS << ")" << endl;
        exit(1);
    }
}

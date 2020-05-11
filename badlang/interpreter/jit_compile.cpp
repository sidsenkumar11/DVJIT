#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
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
    a.call((uint64_t)(&free));


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
    a.call((uint64_t)(&free));

    a.pop(rsi);
    a.pop(rdi);
    a.pop(rdx);
    a.pop(rcx);
    a.pop(rbx);
    a.pop(rax);
}


void jit_alloc_string_literal(asmjit::x86::Assembler &a, std::string val)
{
    // we're going to clobber the following regs, so save them
    a.push(rbx);
    a.push(rdi);
    a.push(rcx);

    a.mov(rdi, val.size() + 1);
    a.call((uint64_t)(&malloc));

    // DANGEROUS -- assume malloc completed successfully, do not check RAX for null
    a.mov(rbx, rax);

    // cool party trick -- greedily set 8 bytes at a time
    for (size_t i = 0; i < val.size() / 8; i += 8)
    {
        uint64_t qword = 0;
        for (size_t j = 0; j < 8; j++)
        {
            uint64_t to_set = val.at(i + j);
            to_set <<= (8 * j);
            qword |= to_set;
        }
        a.mov(rcx, qword);
        a.mov(x86::qword_ptr(rbx, i), rcx);
    }

    // now, set the remaining chars individually
    for (size_t i = val.size() / 8; i < val.size(); i++)
    {
        a.mov(cx, val.at(i));
        a.mov(x86::byte_ptr(rbx, i), cx);
    }

    // null-terminate
    a.mov(x86::byte_ptr(rbx, val.size()), 0);

    a.pop(rcx);
    a.pop(rdi);
    a.pop(rbx);
}


void jit_alloc_integer_literal(asmjit::x86::Assembler &a, int64_t val)
{
    // call malloc
    a.mov(rdi, sizeof(int64_t));
    a.call((uint64_t)(&malloc));
    // rax now contains a pointer, move our value into it
    a.mov(x86::qword_ptr(rax), val);
}


void jit_set_register_to_string(
    asmjit::x86::Assembler &a,
    uint8_t register_id,
    std::string val
) {
    jit_alloc_string_literal(a, val);    // char *s = allocate_string(val);
    a.mov(rdi, rbp);
    a.sub(rdi, sizeof(void *) * (register_id + 1));
    a.mov(rsi, rax);
    a.call((uint64_t)(&set_register_to_string));
}


void jit_set_register_to_int(
    asmjit::x86::Assembler &a,
    uint8_t register_id,
    int64_t val
) {
    jit_alloc_integer_literal(a, val);
    a.mov(rdi, rbp);
    a.sub(rdi, sizeof(void *) * (register_id + 1));
    a.mov(rsi, rax);
    a.call((uint64_t)(&set_register_to_int));
}


void jit_print_register(
    asmjit::x86::Assembler &a,
    uint8_t register_id,
    object_type expected_type
) {
    a.mov(rdi, register_ref(register_id));
    a.mov(rsi, expected_type);
    a.call((uint64_t)(&print_register));
}

asmjit::x86::Mem register_ref(uint8_t reg_id)
{
    if (reg_id > N_REGISTERS + 2)
    {
        throw -1;
    }
    return x86::qword_ptr(rbp, - sizeof(void *) * (reg_id + 1));
}
#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include "asmjit/asmjit.h"
#include "jit_compile.hpp"

using namespace std;
using namespace asmjit;
using namespace asmjit::x86;


#define N_REGISTERS (128)


jit_result *jit_compile(std::stringstream &program)
{
    //
    // Create a runtime and a code holder for asmjit
    //
    asmjit::JitRuntime *jit = new asmjit::JitRuntime();
    CodeHolder code;
    code.init(jit->codeInfo());
    x86::Assembler a(&code);

    //
    // Add code to the assembler, as needed
    //
    jit_prologue(a);

    // TESTING some stuff
    // set reg0 to be an int
    a.mov(register_ref(0), 0xDEADBEEF);
    jit_print_state(a);
    // END TESTING

    // jit_compiler_loop(a, program);

    jit_epilogue(a);


    //
    // Emit the code
    //
    jit_ptr fn;
    Error err = jit->add(&fn, &code);

    if (err)
    {
        std::cerr << "found error!!!" << endl;
        return nullptr;
    }

    jit_result *ret = (jit_result*) malloc(sizeof(jit_result));
    ret->ptr = fn;
    ret->runtime = jit;
    return ret;
}


void jit_prologue(asmjit::x86::Assembler &a)
{
    // standard function prologue
    a.push(rbp);
    a.mov(rbp, rsp);

    // we need room for `N_REGISTERS` 8-byte registers + 2 8-byte test registers
    a.sub(rsp, sizeof(void *) * N_REGISTERS + sizeof(void *) * 2);

    // zero-out all the registers to NULL
    a.xor_(rax, rax);
    for (size_t i = 0; i < N_REGISTERS + 2; i++)
    {
        a.mov(register_ref(i), rax);
    }
}


void jit_epilogue(asmjit::x86::Assembler &a)
{
    // standard function epilogue
    // epilogue
    a.mov(rsp, rbp);
    a.pop(rbp);
    a.ret();
}


void jit_compiler_loop(asmjit::x86::Assembler &a, std::stringstream &program)
{
    // make a FIFO stack with Labels for making conditional jumps
    string line;

    while (std::getline(program, line))
    {
        // THIS PRINTS GARBAGE AND IDK WHY .. ignoring for now
        // cout << line << endl;
    }
}


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
    jit_free_string_literal(a);


    // Iterate over each register and dump its content if not null
    Label loop = a.newLabel();
    Label exit = a.newLabel();
    a.mov(rax, 0);                 // let i = 0;
    
    a.mov(rbx, rbp);               // let ptr = register[0];
    a.sub(rbx, sizeof(void *));

    a.bind(loop);
    a.cmp(rax, N_REGISTERS);       // while (i < N_REGISTERS) {
    a.jae(exit);

    Label if_null = a.newLabel();
    Label if_exit = a.newLabel();
    a.mov(rcx, x86::ptr(rbx));
    a.test(rcx, rcx);
    a.je(if_null);                 //   if (ptr != NULL) {

    a.push(rax); // stash the counter for the moment
    jit_alloc_string_literal(a, "[%.3d]: 0x%.16x\n");
    a.mov(r10, x86::ptr(rsp)); // retrieve the iteration counter
    a.push(rax);
    a.mov(rdi, rax);
    a.mov(al, 0);
    a.mov(rsi, r10);
    a.mov(rdx, rcx);
    a.call((uint64_t)(&printf));   //     printf("[%.3d]: 0x%.16x\n", i, ptr);
    a.pop(rdi);
    jit_free_string_literal(a);
    a.pop(rax);
    a.jmp(if_exit);

    a.bind(if_null);               //   } else {
    
    a.push(rax);
    jit_alloc_string_literal(a, "[%.3d]: NULL\n");
    a.mov(r10, x86::ptr(rsp));
    a.push(rax);
    a.mov(rdi, rax);
    a.mov(al, 0);
    a.mov(rsi, r10);
    a.call((uint64_t)(&printf));
    a.pop(rdi);
    jit_free_string_literal(a);
    a.pop(rax);

    a.bind(if_exit);               //   }

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
    jit_free_string_literal(a);

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
    for (size_t i = 0; i < val.size() - 8; i += 4)
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
    for (size_t i = std::max((size_t)0, val.size() - 8); i < val.size(); i++)
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


void jit_free_string_literal(asmjit::x86::Assembler &a)
{
    // literally just call free(), since the pointer is expected in rdi already
    a.call((uint64_t)(&free));
}


asmjit::x86::Mem register_ref(uint8_t reg_id)
{
    if (reg_id > N_REGISTERS + 2)
    {
        throw -1;
    }
    return x86::qword_ptr(rbp, - sizeof(void *) * (reg_id + 1));
}


void jit_release(jit_result *jit)
{
    jit->runtime->release(jit->ptr);
    delete jit->runtime;
    free(jit);
}

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
    jit_epilogue(a);

    jit_compiler_loop(a, program);

    jit_prologue(a);

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
        a.mov(x86::qword_ptr(rbp, sizeof(void *) * i), rax);
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
    string line;

    while (std::getline(program, line))
    {
        // THIS PRINTS GARBAGE AND IDK WHY
        cout << "hi" << endl;
        cout << line << endl;
    }
}


void jit_release(jit_result *jit)
{
    jit->runtime->release(jit->ptr);
    delete jit->runtime;
    free(jit);
}

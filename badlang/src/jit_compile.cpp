#include <iostream>
#include <string>
#include <sstream>
#include "asmjit/asmjit.h"
#include "jit_compile.hpp"

using namespace std;
using namespace asmjit;
using namespace asmjit::x86;

jit_result *jit_compile(std::stringstream &program)
{
    asmjit::JitRuntime *jit = new asmjit::JitRuntime();
    CodeHolder code;

    code.init(jit->codeInfo());
    x86::Assembler a(&code);

    a.push(rbp);
    a.mov(rbp, rsp);

    // put "hello, world\n" on the stack
    // (rounded up to nearest 8)
    // NOTE: I'm sure there's a simple reason I'm missing, but not rounding
    //    up to a multiple for good alignment will cause a later function to
    //    segfault. So, this is how it is.
    string to_print = "hello, world\n";
    a.sub(x86::rsp, ((to_print.size() + 1) / 8 + 1) * 8);
    a.nop();
    a.nop();
    a.nop();
    for (size_t i = 0; i <= to_print.size(); i++)
    {
        // ugh, doesn't seem like they have a way to specify width?
        // so use a hack: bit-mask the char into place
        // char offset = i % 8;
        // uint64_t shift = offset * 8;

        // uint64_t mask = ~(((uint64_t)0xff) << shift);
        // uint64_t chr = (((uint64_t)to_print.c_str()[i]) << shift);
        // a.mov(rax, x86::byte_ptr(x86::rsp, (i / 8) * 8));
        // a.mov(rbx, to_print.c_str()[i]);
        a.mov(ax, to_print.c_str()[i]);
        a.mov(x86::byte_ptr(x86::rsp, i), ax);
        // a.and_(rax, mask);
        // a.or_(rax, chr);
        // a.mov(x86::byte_ptr(x86::rsp, (i / 8) * 8), rax);
    }

    a.mov(x86::rax, 1);
    a.mov(x86::rdi, 1);
    a.mov(x86::rsi, x86::rsp);
    a.mov(x86::rdx, to_print.size());
    a.syscall();

    // epilogue
    a.mov(x86::rax, 1337);
    a.mov(rsp, rbp);
    a.pop(rbp);
    a.ret();

    jit_ptr fn;
    Error err = jit->add(&fn, &code);

    if (err)
    {
        std::cerr << "found error!!!" << endl;
    }

    jit_result *ret = (jit_result*) malloc(sizeof(jit_result));
    ret->ptr = fn;
    ret->runtime = jit;
    return ret;
}



void jit_release(jit_result *jit)
{
    jit->runtime->release(jit->ptr);
    delete jit->runtime;
    free(jit);
}

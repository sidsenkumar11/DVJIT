#include <string>
#include <iostream>

#include <asmjit/asmjit.h>

typedef int (*jit_ptr)();


typedef struct {
    jit_ptr ptr;
    asmjit::JitRuntime *runtime;
} jit_result;


/**
 * Compile a program via JIT compilation
 * 
 * returns a struct containing a code pointer and the jit runtime
 */
jit_result *jit_compile(std::stringstream &program);


/**
 * Add the prologue for the jit runtime to the assembler
 * 
 * This makes room for all of our registers on the stack, and
 * initializes them to zero.
 */
void jit_prologue(asmjit::x86::Assembler &a);


/**
 * Add the epilogue for the jit runtime to the assembler
 */
void jit_epilogue(asmjit::x86::Assembler &a);


/**
 * Loops over the input program and adds instructions to the assembler,
 * as needed.
 */
void jit_compiler_loop(asmjit::x86::Assembler &a, std::stringstream &program);


/**
 * Deallocate all jit materials
 */
void jit_release(jit_result *jit);

#pragma once
#include <string>
#include <iostream>

#include <asmjit/asmjit.h>

typedef int (*jit_ptr)();


typedef struct {
    jit_ptr ptr;
    asmjit::JitRuntime *runtime;
} jit_result;


/**
 * The type of object in BADLANG.
 * 
 * NOTE: the reason we aren't using enums is that gcc will
 *       pack them into whatever memory footprint is most
 *       efficient, but that's less convenient than a known-
 *       width representation.
 */
typedef uint64_t object_type;

#define TYPE_STRING  ((object_type) 1)
#define TYPE_INTEGER ((object_type) 2)
#define TYPE_DICT    ((object_type) 2)


/**
 * An object in BADLANG
 */
typedef struct {
    object_type type;
    /**
     * The underlying object
     */
    void       *ptr;
} badlang_object;


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
 * Adds code to dump the internal JIT state to stdout,
 * for debugging purposes.
 */
void jit_print_state(asmjit::x86::Assembler &a);

/**
 * Allocates a string literal, leaves result after alloc in rax
 */
void jit_alloc_string_literal(asmjit::x86::Assembler &a, std::string val);


/**
 * Allocates an integer literal, leaves the resolt after alloc in rax
 */
void jit_alloc_integer_literal(asmjit::x86::Assembler &a, int64_t val);


/**
 * Allocates a string, puts its pointer into a badlang_object, and sets
 * the given register to a pointer to that object.
 */
void jit_set_register_to_string(asmjit::x86::Assembler &a, uint8_t register_id, std::string val);


/**
 * Allocates an integer, puts its pointer into a badlang_object, and sets
 * the given register to a pointer to that object.
 */
void jit_set_register_to_int(asmjit::x86::Assembler &a, uint8_t register_id, int64_t val);


/**
 * Get an Operand representing a memory reference to the given register
 */
asmjit::x86::Mem register_ref(uint8_t reg_id);

/**
 * Deallocate all jit materials
 */
void jit_release(jit_result *jit);

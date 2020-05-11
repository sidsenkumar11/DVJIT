#pragma once
#include <string>
#include <iostream>

#include <asmjit/asmjit.h>

#define N_REGISTERS (128)
typedef int (*jit_ptr)();

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
 * Adds code to dump the internal JIT state to stdout,
 * for debugging purposes.
 */
void jit_print_state(asmjit::x86::Assembler &a);

/**
 * Allocates a string literal, leaves result after alloc in rax
 */
void jit_alloc_string_literal(asmjit::x86::Assembler &a, std::string val);


/**
 * Allocates an integer literal, leaves the result after alloc in rax
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
 * Print the given register, if it has a string
 */
void jit_print_register(asmjit::x86::Assembler &a, uint8_t register_id, object_type expected_type);


/**
 * Get an Operand representing a memory reference to the given register
 */
asmjit::x86::Mem register_ref(uint8_t reg_id);

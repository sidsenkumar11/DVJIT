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
    int64_t     regno;
} badlang_object;

/**
 * Adds code to dump the internal JIT state to stdout,
 * for debugging purposes.
 */
void jit_print_state(asmjit::x86::Assembler &a);


/**
 * Performs type checking and initialization checking on a register.
 */
void jit_verify_reg(uint8_t reg, asmjit::x86::Assembler &a, object_type expected);


/**
 * Allocates a string literal, leaves result after alloc in rax
 */
void jit_alloc_string_literal(asmjit::x86::Assembler &a, std::string val);


/**
 * Allocates an integer literal, leaves the result after alloc in rax
 */
void jit_alloc_integer_literal(asmjit::x86::Assembler &a, int64_t val);


/**
 * Allocates a dict, leaves the result after alloc in rax
 */
void jit_alloc_dict(asmjit::x86::Assembler &a);


/**
 * Allocates a string and puts its pointer into the badlang_object
 * at the given register. If the badlang_object is already occupied,
 * the contents are first deallocated.
 */
void jit_set_register_to_string(asmjit::x86::Assembler &a, uint8_t register_id, std::string val);


/**
 * Allocates an integer and puts its pointer into the badlang_object
 * at the given register. If the badlang_object is already occupied,
 * the contents are first deallocated.
 */
void jit_set_register_to_int(asmjit::x86::Assembler &a, uint8_t register_id, int64_t val);


/**
 * Stores the given physical register into the given virtual register.
 */
void jit_set_register_to_int(
    asmjit::x86::Assembler &a,
    uint8_t register_id,
    asmjit::x86::Gp source
);

/**
 * Print the given register
 */
void jit_print_register(asmjit::x86::Assembler &a, uint8_t register_id);


/**
 * Get an Operand representing a memory reference to the given register
 */
asmjit::x86::Mem register_ref(uint8_t reg_id);

/**
 * Loads the given virtual register into the given physical register
 */
void jit_load_integer(
    asmjit::x86::Gp dest,
    uint8_t register_id,
    asmjit::x86::Assembler &a
);

/**
 * Copies the badlang object in register 1 to register 2
 */
void jit_move_register(
    asmjit::x86::Assembler &a,
    uint8_t reg_one,
    uint8_t reg_two
);

#pragma once
#include "jit_compile.hpp"

/** returns true when debug is on */
bool is_debug();

/**
 * Prints a single line showing the status of the given register
 */
void debug_print_register(uint8_t register_id, badlang_object *obj);


/**
 * Prints the register, stops execution in the case of a type
 * mismatch from expected
 */
void print_register(badlang_object *obj, object_type expected_type);


/**
 * Deallocates the memory for the given object
 *
 * NOTE: it is safe to pass NULL to this function
 */
void dealloc_object(badlang_object *obj);


/**
 * Sets the given register pointer to the given string value.
 */
void set_register_to_string(badlang_object **reg, char *val);


/**
 * Sets the given register pointer to the given int pointer.
 */
void set_register_to_int(badlang_object **reg, int64_t *val);

#pragma once
#include "jit_compile.hpp"
#include "treemap.hpp"

/** returns true when debug is on */
bool is_debug();


/**
 * Prints a single line showing the status of the given register
 */
void debug_print_register(uint8_t register_id, badlang_object *obj);


/**
 * Verifies a badlang_object's type matches the expected
 * and that the pointer has been initialized.
 */
void verify_reg(badlang_object *obj, object_type expected);


/**
 * Prints the virtual register's contents
 */
void print_register(badlang_object *obj);


/**
 * Deallocates the memory for the given badlang_object
 *
 * NOTE: it is safe to pass NULL to this function
 */
void dealloc_object(badlang_object *obj);


/**
 * Sets the given badlang_object pointer to the given string value.
 */
void set_register_to_string(badlang_object *obj, char *val);


/**
 * Sets the given badlang_object pointer to the given int pointer.
 */
void set_register_to_int(badlang_object *obj, int64_t *val);


/**
 * Initializes the given badlang_object pointer with a dict pointer.
 */
void set_register_to_dict(badlang_object *obj);


/**
 * Get value from dict for the given key and copy over to destination.
 */
void get_dict(TreeMap *map, badlang_object *key_obj, badlang_object *dest_obj);


/**
 * Set value in dict for the given key
 */
void set_dict(TreeMap *map, badlang_object *key_obj, badlang_object *val_obj);


/**
 * Copies the value from source to destination.
 * Integers are deep copied while strings and dicts are shallow copied.
 */
void move_register(badlang_object *dest, badlang_object *src);

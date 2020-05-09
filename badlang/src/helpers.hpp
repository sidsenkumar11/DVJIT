#pragma once
#include "jit_compile.hpp"

/** returns true when debug is on */
bool is_debug();

void debug_print_register(uint8_t register_id, badlang_object *obj);
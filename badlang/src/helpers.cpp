#include <cstring>
#include <cstdlib>
#include <inttypes.h>
#include "jit_compile.hpp"

bool is_debug()
{
    const char *env = getenv("DEBUG");
    return env != NULL && strcmp("true", env) == 0;
}


void debug_print_register(uint8_t register_id, badlang_object *obj)
{
    if (obj == nullptr)
    {
        printf("[%.3d]: NULL\n", register_id);
    }
    else if (obj->type == TYPE_STRING)
    {
        printf("[%.3d]: \"%s\"\n", register_id, (char *)obj->ptr);
    }
    else if (obj->type == TYPE_INTEGER)
    {
        printf("[%.3d]: %" PRId64 "\n", register_id, *(int64_t *)(obj->ptr));
    }
    else if (obj->type == TYPE_DICT)
    {
        printf("[%.3d]: DICT\n", register_id);
    }
    else
    {
        printf("[%.3d]: UNKNOWN TYPE\n", register_id);
    }
}


void dealloc_object(badlang_object *obj)
{
    if (obj == nullptr)
    {
        return;
    }

    if (obj->type == TYPE_INTEGER || obj->type == TYPE_STRING)
    {
        free(obj->ptr);
    }

    // TODO: handle type dict

    free(obj);
}


void set_register_to_string(badlang_object **reg, char *val)
{
    // ignore null pointers, although that should never occur
    if (reg == nullptr)
    {
        return;
    }

    // if the register is already occupied, dealloc the old val
    if (*reg == nullptr)
    {
        dealloc_object(*reg);
    }

    badlang_object *obj = (badlang_object *)malloc(sizeof(badlang_object));
    obj->type = TYPE_STRING;
    obj->ptr = val;
    *reg = obj;
}


void set_register_to_int(badlang_object **reg, int64_t *val)
{
    if (reg == nullptr)
    {
        return;
    }

    if (*reg == nullptr)
    {
        dealloc_object(*reg);
    }

    badlang_object *obj = (badlang_object *)malloc(sizeof(badlang_object));
    obj->type = TYPE_INTEGER;
    obj->ptr = val;
    *reg = obj;
}

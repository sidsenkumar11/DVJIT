#include <iostream>
#include <cstring>
#include <cstdlib>
#include <inttypes.h>
#include <stdlib.h>
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


void verify_reg(badlang_object *obj, object_type expected)
{
    if (obj->type != expected)
    {
        printf("type mismatch on reg %" PRId64 "!\n", obj->regno);
        exit(1);
    }
    else if (obj->ptr == nullptr)
    {
        printf("uninitialized reg %" PRId64 "!\n", obj->regno);
        exit(1);
    }
}


void print_register(badlang_object *obj) {
    if (obj->ptr == nullptr)
    {
        printf("uninitialized reg %" PRId64 "\n", obj->regno);
        exit(1);
    }
    else if (obj->type == TYPE_INTEGER)
    {
        printf("%" PRId64 "\n", *(int64_t *)(obj->ptr));
    }
    else if (obj->type == TYPE_STRING)
    {
        printf("%s\n", (char *)(obj->ptr));
    }
    else if (obj->type == TYPE_DICT)
    {
        // TODO figure this out
        printf("dict....\n");
    }
    else
    {
        printf("unknown type %" PRId64 " in reg %" PRId64 "\n", obj->type, obj->regno);
        exit(1);
    }
}


void dealloc_object(badlang_object *obj)
{
    if (obj->ptr == nullptr)
        return;

    if (obj->type == TYPE_DICT)
    {
        // TODO: handle type dict
    }

    free(obj->ptr);
}


void set_register_to_string(badlang_object *obj, char *val)
{
    dealloc_object(obj);
    obj->type = TYPE_STRING;
    obj->ptr = val;
}


void set_register_to_int(badlang_object *obj, int64_t *val)
{
    dealloc_object(obj);
    obj->type = TYPE_INTEGER;
    obj->ptr = val;
}


void move_register(badlang_object *dest, badlang_object *src)
{
    // verify src has been setup
    if (src->ptr == nullptr)
    {
        printf("uninitialized reg %" PRId64 "\n", src->regno);;
        exit(1);
    }

    // delete old value in dest
    dealloc_object(dest);

    // ints are deep copied, maps/strings are shallow copied
    dest->type = src->type;
    if (src->type == TYPE_INTEGER)
    {
        dest->ptr = malloc(sizeof(int64_t));
        *(int64_t *)(dest->ptr) = *(int64_t *)(src->ptr);
    }
    else if (src->type == TYPE_STRING || src->type == TYPE_DICT)
    {
        dest->ptr = src->ptr;
    }
    else
    {
        printf("unknown type %" PRId64 " in reg %" PRId64 "\n", src->type, src->regno);
        exit(1);
    }
}

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <inttypes.h>
#include <stdlib.h>
#include "jit_compile.hpp"
#include "treemap.hpp"

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
    // check initialized
    if (obj->ptr == nullptr)
    {
        printf("uninitialized reg %" PRId64 "!\n", obj->regno);
        exit(1);
    }

    // check types
    if (expected != TYPE_ANY)
    {
        if (expected != TYPE_HASHABLE && obj->type != expected)
        {
            printf("type mismatch on reg %" PRId64 "!\n", obj->regno);
            exit(1);
        }
        else if (expected == TYPE_HASHABLE && obj->type == TYPE_DICT)
        {
            printf("type error on reg %" PRId64 "!\n", obj->regno);
            printf("cannot insert dicts into dicts\n");
            exit(1);
        }
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
        TreeMap *map = (TreeMap *) (obj->ptr);
        map->print_map();
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
        TreeMap *map = (TreeMap *) (obj->ptr);
        map->~TreeMap();
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


void set_register_to_dict(badlang_object *obj)
{
    dealloc_object(obj);
    obj->type = TYPE_DICT;
    obj->ptr = new TreeMap();
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


void get_dict(TreeMap *map, badlang_object *key_obj, badlang_object *dest_obj)
{
    uint64_t key;
    if (key_obj->type == TYPE_INTEGER)
    {
        key = *((int64_t *) (key_obj->ptr));
    }
    else
    {
        key = (uint64_t) key_obj->ptr;
    }

    badlang_object *copy = (badlang_object *) map->get(key);

    // make a deep copy of object
    dealloc_object(dest_obj);
    dest_obj->type = copy->type;

    if (copy->type == TYPE_INTEGER)
    {
        dest_obj->ptr = malloc(sizeof(int64_t));
        memcpy(dest_obj->ptr, copy->ptr, sizeof(int64_t));
    }
    else
    {
        dest_obj->ptr = malloc(strlen((char *) copy->ptr)+1);
        strcpy((char *) dest_obj->ptr, (char *) copy->ptr);
    }
}


void set_dict(TreeMap *map, badlang_object *key_obj, badlang_object *val_obj)
{
    uint64_t key;
    if (key_obj->type == TYPE_INTEGER)
    {
        key = *((int64_t *) (key_obj->ptr));
    }
    else
    {
        key = (uint64_t) key_obj->ptr;
    }

    // make a deep copy of the value
    badlang_object *copy = (badlang_object *) malloc(sizeof(badlang_object));
    copy->type = val_obj->type;

    if (val_obj->type == TYPE_INTEGER)
    {
        copy->ptr = malloc(sizeof(int64_t));
        memcpy(copy->ptr, val_obj->ptr, sizeof(int64_t));
    }
    else
    {
        copy->ptr = malloc(strlen((char *) val_obj->ptr)+1);
        strcpy((char *) copy->ptr, (char *) val_obj->ptr);
    }

    map->set(key, copy);
}

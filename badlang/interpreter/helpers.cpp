#include <cstdlib>
#include <cstring>
#include <ctype.h>
#include <inttypes.h>
#include <iostream>
#include <stdlib.h>

#include "allocator.hpp"
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
        delete map;
    }
    else
    {
        alloc_free(obj->ptr);
    }
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
        dest->ptr = alloc_get(sizeof(int64_t));
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

    badlang_object *copy = map->get(key);

    // make a deep copy of object
    dealloc_object(dest_obj);
    dest_obj->type = copy->type;

    if (copy->type == TYPE_INTEGER)
    {
        dest_obj->ptr = alloc_get(sizeof(int64_t));
        memcpy(dest_obj->ptr, copy->ptr, sizeof(int64_t));
    }
    else
    {
        dest_obj->ptr = alloc_get(strlen((char *) copy->ptr)+1);
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
    badlang_object *copy = (badlang_object *) alloc_get(sizeof(badlang_object));
    copy->type = val_obj->type;

    if (val_obj->type == TYPE_INTEGER)
    {
        copy->ptr = alloc_get(sizeof(int64_t));
        memcpy(copy->ptr, val_obj->ptr, sizeof(int64_t));
    }
    else
    {
        copy->ptr = alloc_get(strlen((char *) val_obj->ptr)+1);
        strcpy((char *) copy->ptr, (char *) val_obj->ptr);
    }

    map->set(key, copy);
}


void init_iterator(TreeMap *map)
{
    map->init_iterator();
}

int iterate(TreeMap *map, badlang_object *dest_obj)
{
    badlang_object *copy = map->iterate();
    if (copy == nullptr)
    {
        return 1;
    }

    // make a deep copy of the object
    dealloc_object(dest_obj);
    dest_obj->type = copy->type;

    if (copy->type == TYPE_INTEGER)
    {
        dest_obj->ptr = alloc_get(sizeof(int64_t));
        memcpy(dest_obj->ptr, copy->ptr, sizeof(int64_t));
    }
    else
    {
        dest_obj->ptr = alloc_get(strlen((char *) copy->ptr)+1);
        strcpy((char *) dest_obj->ptr, (char *) copy->ptr);
    }
    return 0;
}


std::string unescape_string(std::string src)
{
    std::string dest;
    // i indexes into the string src
    for (size_t i=0; i < src.size(); i++)
    {
        char tocopy = src.at(i);
        if (tocopy == '\\' && src.size() > i + 1)
        {
            // this is an escape code
            // we need to eat a few more chars, so advance as-we-go
            switch (src.at(i + 1))
            {
            case 'n':
            {
                i++;
                tocopy = '\n';
                break;
            }
            case 't':
            {
                i++;
                tocopy = '\t';
                break;
            }
            case '"':
            {
                i++;
                tocopy = '"';
                break;
            }
            case 'x':
            {
                // ensure we have 2 more to eat
                if (src.size() <= i + 3)
                {
                    throw "not enough chars to read escape sequence";
                }

                // ensure the chars are hex
                char c1 = tolower(src.at(i+2));
                char c2 = tolower(src.at(i+3));
                if (! (((('a' <= c1)  && (c1 <= 'f'))) || isdigit(c1)) &&
                      ((((('a' <= c2)  && (c2 <= 'f'))) || isdigit(c2))) )
                {
                    throw "non-hex escape sequence";
                }

                tocopy = (char) std::stol(src.substr(i+2, 2), NULL, 16);

                if (tocopy == '\0')
                {
                    throw "cannot use null character in string";
                }
                i += 3;
                break;
            }
            default:
                throw "unknown character escape code";
            }
        } // end if (is escape)

        dest.append(1, tocopy);
    }
    return dest;
}

int compare_regs(badlang_object *first, badlang_object *second)
{
    if (first->type != second->type)
    {
        return 1;
    }

    if (first->type == TYPE_INTEGER)
    {
        return *(int64_t *) (first->ptr) != *(int64_t *) (second->ptr);
    }

    if (first->type == TYPE_STRING)
    {
        return strcmp((char *) (first->ptr), (char *) second->ptr) != 0;
    }

    // should never reach here
    return 1;
}

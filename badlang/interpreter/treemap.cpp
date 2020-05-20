#include "treemap.hpp"
#include "allocator.hpp"
#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>


TreeMap::~TreeMap()
{
    // Make sure not iterating this dict
    if (!this->iterators.empty())
    {
        printf("Runtime detected attempt to overwrite register containing ");
        printf("a dictionary while executing!\n");
        exit(1);
    }

    // Clear out nodes
    recursive_deallocate(this->root);
}

void TreeMap::recursive_deallocate(node_t *node)
{
    if (node == NULL)
        return;

    recursive_deallocate(node->left);
    recursive_deallocate(node->right);

    badlang_object *obj = node->value;
    alloc_free(obj->ptr);
    alloc_free(obj);
    alloc_free(node);
}


node_t *TreeMap::newNode(uint64_t key, badlang_object *value)
{
    node_t *temp = (node_t *) alloc_get(sizeof(node_t));
    temp->key   = key;
    temp->value = value;
    temp->left  = NULL;
    temp->right = NULL;
    return temp;
}


node_t *TreeMap::getRecursive(node_t *node, uint64_t key)
{
    if (node == NULL)
        return NULL;

    if (key < node->key)
        return getRecursive(node->left, key);
    else if (key > node->key)
        return getRecursive(node->right, key);
    else
        return node;
}


badlang_object *TreeMap::get(uint64_t key)
{
    node *found = getRecursive(this->root, key);
    if (found == NULL)
    {
        printf("Could not find key %" PRIu64 " in dict!\n", key);
        exit(1);
    }
    return found->value;
}


node_t *TreeMap::setRecursive(node_t *node, uint64_t key, badlang_object *value)
{
    if (node == NULL)
        return newNode(key, value);

    if (key < node->key)
        node->left  = setRecursive(node->left, key, value);
    else if (key > node->key)
        node->right = setRecursive(node->right, key, value);
    else
        node->value = value;

    return node;
}


void TreeMap::set(uint64_t key, badlang_object *value)
{
    this->root = setRecursive(this->root, key, value);
}


void TreeMap::print_map_recursive(node_t *cur)
{
    if (cur == nullptr)
        return;

    print_map_recursive(cur->left);
    badlang_object *cur_obj = cur->value;
    if (cur_obj->type == TYPE_INTEGER)
    {
        printf("%" PRId64 "\n", *((int64_t *) cur_obj->ptr));
    }
    else
    {
        printf("%s\n", (char *) cur_obj->ptr);
    }
    print_map_recursive(cur->right);
}


void TreeMap::print_map()
{
    print_map_recursive(this->root);
}


void TreeMap::inorder_push(node_t *cur, std::stack<node_t *> *stack)
{
    if (cur == nullptr) return;

    // reversed because I used a stack
    inorder_push(cur->right, stack);
    stack->push(cur);
    inorder_push(cur->left, stack);
}


void TreeMap::init_iterator()
{
    // push new nesting iterator
    void *obj_space = alloc_get(sizeof(std::stack<node_t *>));
    this->iterators.push(new (obj_space) std::stack<node_t *>());

    // build iterator stack
    inorder_push(this->root, this->iterators.top());
}


badlang_object *TreeMap::iterate()
{
    // grab current nesting iterator
    std::stack<node_t *> *iter = this->iterators.top();

    // check if done
    if (iter->empty())
    {
        this->iterators.pop();
        iter->~stack();
        alloc_free(iter);
        return nullptr;
    }

    // visit current node
    node_t *cur = iter->top();
    iter->pop();
    return cur->value;
}

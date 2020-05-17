#include "treemap.hpp"
#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>


TreeMap::~TreeMap()
{
    printf("TODO: Proper deallocation of a tree!\n");
}


node_t *TreeMap::newNode(uint64_t key, void *value)
{
    node_t *temp = (node_t *) malloc(sizeof(node_t));
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


void *TreeMap::get(uint64_t key)
{
    node *found = getRecursive(this->root, key);
    if (found == NULL)
    {
        printf("Could not find key %" PRIu64 " in dict!\n", key);
        exit(1);
    }
    return found->value;
}


node_t *TreeMap::setRecursive(node_t *node, uint64_t key, void *value)
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


void TreeMap::set(uint64_t key, void *value)
{
    this->root = setRecursive(this->root, key, value);
}


void TreeMap::print_map()
{
    printf("TODO: Print this hashtable!\n");
}

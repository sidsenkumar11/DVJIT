#include "treemap.hpp"
#include <stdlib.h>

struct node *newNode(key_t key, val_t value)
{
    struct node *temp = (struct node *) malloc(sizeof(struct node));
    temp->key   = key;
    temp->value = value;
    temp->left  = NULL;
    temp->right = NULL;
    return temp;
}

struct node *insertRecursive(struct node* node, key_t key, val_t value)
{
    if (node == NULL)
        return newNode(key, value);

    if (key < node->key)
        node->left  = insertRecursive(node->left, key, value);
    else if (key > node->key)
        node->right = insertRecursive(node->right, key, value);
    else
        node->value = value;

    return node;
}

struct node *getRecursive(struct node* node, key_t key)
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

val_t get(key_t key)
{
    return getRecursive(root, key)->value;
}

void insert(key_t key, val_t value)
{
    struct node *node = insertRecursive(root, key, value);
    if (!root){
        root = node;
    }
}

#pragma once

typedef int key_t;
typedef int val_t;

struct node *root;
struct node
{
    key_t key;
    val_t value;
    struct node *left;
    struct node *right;
};


val_t get(key_t key);
void insert(key_t key, val_t value);

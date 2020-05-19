#pragma once
#include <inttypes.h>
#include <bits/stdc++.h>
#include "jit_compile.hpp"

typedef struct node {
    uint64_t key;
    badlang_object *value;
    struct node *left;
    struct node *right;
} node_t;

class TreeMap {
private:
    node_t *root = nullptr;
    std::stack <std::stack<node_t *> *> iterators;
    virtual node_t *newNode(uint64_t key, badlang_object *value);
    virtual node_t *setRecursive(node_t *node, uint64_t key, badlang_object *value);
    virtual node_t *getRecursive(node_t *node, uint64_t key);
    virtual void print_map_recursive(node_t *cur);
    virtual void inorder_push(node_t *cur, std::stack<node_t *> *stack);
    virtual void recursive_deallocate(node_t *node);

public:
  TreeMap() {}
  virtual ~TreeMap();
  virtual badlang_object *get(uint64_t key);
  virtual void set(uint64_t key, badlang_object *val_ptr);
  virtual void print_map();
  virtual void init_iterator();
  virtual badlang_object *iterate();
};

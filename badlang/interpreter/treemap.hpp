#pragma once
#include <inttypes.h>

typedef struct node {
    uint64_t key;
    void *value;
    struct node *left;
    struct node *right;
} node_t;

class TreeMap {
private:
    node_t *root = nullptr;
    virtual node_t *newNode(uint64_t key, void *value);
    virtual node_t *setRecursive(node_t *node, uint64_t key, void *value);
    virtual node_t *getRecursive(node_t *node, uint64_t key);

public:
  TreeMap() {}
  ~TreeMap();
  virtual void *get(uint64_t key);
  virtual void set(uint64_t key, void *val_ptr);
  virtual void print_map();
};

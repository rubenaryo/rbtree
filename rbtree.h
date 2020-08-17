#ifndef RBTREE_H
#define RBTREE_H

#include "stdbool.h"

/// An integer implementation of a red-black tree.
struct rbtree
{
    // Class Methods
    void        (*insert)   (int, struct rbtree*);
    const int*  (*search)   (int, struct rbtree*);
    void        (*traverse) (struct rbtree*);
    bool        (*remove)   (int, struct rbtree*);
    void        (*shutdown) (struct rbtree*); 

    // Fields
    struct rbtree_node
    {
        int value;
        bool red;
        struct rbtree_node* left;
        struct rbtree_node* right;
        struct rbtree_node* parent;
    };

    struct rbtree_node* root;
    unsigned long count;
};

// Constructor factory instance declared in rbtree.c
extern const struct rbtree_factory {
    struct rbtree  (*create)();
    struct rbtree* (*create_dynamic)();
    struct rbtree  (*from_array)(int*, unsigned int);
    struct rbtree* (*from_array_dynamic)(int*, unsigned int);
} rbtree;

// Recursive node utility functions
static void         rbtree_node_insert(int val, struct rbtree_node** node, struct rbtree_node* parent, struct rbtree* t);
static const int*   rbtree_node_search(int val, struct rbtree_node* src);
static void         rbtree_node_traverse(struct rbtree_node* node, short direction);
static bool         rbtree_node_remove(int val, struct rbtree_node* src);
static void         rbtree_node_shutdown(struct rbtree_node* node);
static void         rbtree_node_leftrotate(struct rbtree_node* node, struct rbtree* tree);
static void         rbtree_node_rightrotate(struct rbtree_node* node, struct rbtree* tree);
static void         rbtree_node_rebalance(struct rbtree_node* node, struct rbtree* t);

#endif
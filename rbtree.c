/// Implementation file for rbtree data structure
#include "rbtree.h"
#include <stdlib.h>
#include <stdio.h>

//------------------------------------------------------------------------------------------
// Main tree structure wrapper functions.

// Inserts a value into the tree, performing the tree re-balancing operation in the process.
static void rbtree_insert(int val, struct rbtree* this)
{
    rbtree_node_insert(val, &this->root, NULL, this); // parent arg is NULL because it is the root of the tree and has no parent.

    this->count++;
}

// Searches the tree for val and returns a pointer to it.
static const int* rbtree_search(int val, struct rbtree* this)
{
    const int* ret = rbtree_node_search(val, this->root);
    printf("Total elements in tree: %lu\n\n", this->count);
    return ret;
}

// prints every element in the rbtree using inorder traversal
static void rbtree_traverse(struct rbtree* this)
{
    rbtree_node_traverse(this->root, 0);
}

// Removes val from the tree, returning true if val was in the tree.
static bool rbtree_remove(int val, struct rbtree* this)
{
    bool ret = rbtree_node_remove(val, this->root);

    if (ret)
        this->count--;
    
    return ret;
}

// Dismantles the tree and cleans up all dynamic memory allocations
static void rbtree_shutdown(struct rbtree* this)
{
    printf("Starting shutdown...\n");
    rbtree_node_shutdown(this->root);
    this->count = 0;
    this->root = NULL;
    printf("Tree deletion complete.\n");
    return;
}




//------------------------------------------------------------------------------------------
// Assembling rbtree factory instance

// Build a new rbtree instance
static struct rbtree create()
{
    struct rbtree tree;

    // Member variables
    tree.root = NULL;
    tree.count = 0;

    // Hook up member function pointers
    tree.insert   = rbtree_insert;
    tree.search   = rbtree_search;
    tree.traverse = rbtree_traverse;
    tree.remove   = rbtree_remove;
    tree.shutdown = rbtree_shutdown;
    return tree;
}

static struct rbtree* create_dynamic()
{
    struct rbtree* ret = malloc(sizeof(struct rbtree));
    *ret = create();
    return ret;
}

static struct rbtree from_array(int* values, unsigned int count)
{
    //@todo
    return create();
}

static struct rbtree* from_array_dynamic(int* values, unsigned int count)
{
    //@todo
    return create_dynamic();
}

// Hook up object factory creation function
const struct rbtree_factory rbtree = 
{
    .create             = create,
    .create_dynamic     = create_dynamic,
    .from_array         = from_array,
    .from_array_dynamic = from_array_dynamic
};




//------------------------------------------------------------------------------------------
// Recursive node operations

static void rbtree_node_insert(int val, struct rbtree_node** node, struct rbtree_node* parent, struct rbtree* t)
{
    struct rbtree_node* leaf = *node;

    // @todo rebalancing algorithm.
    if (!leaf)
    {
        // Initialize leaf node
        leaf = (struct rbtree_node*) malloc(sizeof(struct rbtree_node));
        leaf->value = val;
        leaf->red = true;
        leaf->left = leaf->right = NULL;
        leaf->parent = parent;              // parent param is ok to be null, since it is never dereferenced.
        *node = leaf;
    }
    else
    {
        if (val < leaf->value)
        {
            rbtree_node_insert(val, &leaf->left, leaf, t);
        }
        else
        {
            rbtree_node_insert(val, &leaf->right, leaf, t);
        }
    }

    // Node is now inserted properly by binary tree rules. Re-balance the tree to make it fit rbtree standards.
    rbtree_node_rebalance(leaf, t);
}

static void rbtree_node_rebalance(struct rbtree_node* node, struct rbtree* t)
{
    struct rbtree_node* p = node->parent;

    // Case 1: node is the root
    if (p == NULL)
    {
        // Fix: recolor node to be black
        node->red = false;
        return;
    }
    //else if (!p->red) // if the parent is black, we don't need to rebalance
    //{
    //    return;
    //}

    struct rbtree_node* gp = p->parent;
    // If there's no grandparent, rebalancing is not necessary.
    if (!gp)
        return;

    // Uncle is gp's child which != parent. Note: uncle can be NULL, in which case he will be treated as black node
    const bool uncleIsLeftChild = p == gp->right;
    struct rbtree_node* uncle = uncleIsLeftChild ? gp->left : gp->right;

    // Case 2: Z has a red uncle.
    if (uncle && uncle->red)
    {
        // Fix: recolor parent, grandparent, and uncle nodes
        p->red = !p->red;
        gp->red = !gp->red;
        uncle->red = false;
    }
    // Case 3: node has a black uncle (or NULL)
    else if (!uncle || (uncle && !uncle->red))
    {
        // 3a: parent going right
        if (uncleIsLeftChild)
        {
            if (node == p->left) // 3ai: change direction, forms a triangle
            {
                // Fix: rotate node's parent rightwards
                rbtree_node_rightrotate(p, t);
            }
            else                 // 3aii: same direction, forms a line
            {
                // Fix: rotate node's grandparent leftwards and recolor original parent, grandparent.
                rbtree_node_leftrotate(gp, t);
                p->red = !p->red;
                gp->red = !gp->red;
            }
        }
        // 3b: parent going left
        else
        {
            if (node == p->right) // 3bi: change direction, forms a triangle
            {
                // Fix: rotate node's parent leftwards
                rbtree_node_leftrotate(p, t);
            }
            else                  // 3bii: same direction, forms a line
            {
                // Fix: rotate node's grandparent rightwards and recolor original parent, grandparent
                rbtree_node_rightrotate(gp, t);
                p->red = !p->red;
                gp->red = !gp->red;
            }
        }
    }
}

static const int* rbtree_node_search(int val, struct rbtree_node* node)
{
    static unsigned long lookupCount = 1;
    // see if we found the value
    const int nodeValue = node->value;
    if (nodeValue == val)
    {
        printf("Found %d.\nNumber of iterations: %lu.\n", nodeValue, lookupCount);
        lookupCount = 1;
        return &node->value;
    }
    else
    {
        printf("Searched for %d, but found %d, ", val, nodeValue);
        lookupCount++;
    }

    // Decide whether to go left or right
    struct rbtree_node* next = NULL;

    if (val < nodeValue)
    {
        next = node->left;
        printf("going left.\n");
    }
    else
    {
        next = node->right;
        printf("going right.\n");
    }

    // If cannot go further, just return NULL
    if (next)
    {
        return rbtree_node_search(val, next);
    }
    else
    {
        printf("Value not in tree!\nNumber of iterations: %lu.\n", lookupCount);
        lookupCount = 1;
        return NULL;
    }
}

static void rbtree_node_traverse(struct rbtree_node* node, short direction)
{
    if (node)
    {
        rbtree_node_traverse(node->left, -1);
        printf("%d, ", node->value);
        rbtree_node_traverse(node->right, +1);
    }
}

// Returns true if removal successful.
static bool rbtree_node_remove(int val, struct rbtree_node* src)
{
    return true;
}

static void rbtree_node_shutdown(struct rbtree_node* node)
{
    if (node->left)
        rbtree_node_shutdown(node->left);
    node->left = NULL;

    if (node->right)
        rbtree_node_shutdown(node->right);
    node->right = NULL;

    printf("Freeing %d...\n", node->value);
    free(node);
}

// Left rotations are invalid if node has NULL right child. 
static void rbtree_node_leftrotate(struct rbtree_node* node, struct rbtree* t)
{
    struct rbtree_node* temp = node->right;
    node->right = temp->left;
    
    if (temp->left)
        temp->left->parent = node;

    // Reassign parents and tree root
    temp->parent = node->parent;

    if (!node->parent)                   // node is the root node
        t->root = temp;
    else if (node == node->parent->left) // node is a leftward node
        node->parent->left = temp;
    else                                 // node is a rightward node
        node->parent->right = temp;

    // Node's originally right child is now its parent. Left rotation is complete.
    temp->left = node;
    node->parent = temp;
}

// Right rotations are invalid if node has NULL left child.
static void rbtree_node_rightrotate(struct rbtree_node* node, struct rbtree* t)
{
    struct rbtree_node* temp = node->left;
    node->left = temp->right;

    if (temp->right)
        temp->right->parent = node;

    // Reassign parents and tree root
    temp->parent = node->parent;

    if (!node->parent)                   // node is the root node
        t->root = temp;
    else if (node == node->parent->left) // node is a leftward node
        node->parent->left = temp;
    else                                 // node is a rightward node
        node->parent->right = temp;

    // Node's originally left child is now its parent. Right rotation is complete.
    temp->right = node;
    node->parent = temp;
}
#include <stdio.h>
#include <stdlib.h>
#include "rbtree.h"

int main(int argc, char** argv)
{
    // Initialize the tree with values
    struct rbtree* tree = rbtree.create_dynamic();
    tree->insert(8, tree);
    tree->insert(5, tree);
    tree->insert(15, tree);
    tree->insert(12, tree);
    tree->insert(19, tree);
    tree->insert(9, tree);
    tree->insert(13, tree);
    tree->insert(23, tree);
    tree->insert(10, tree);

    // Try Search
    const int searchVal = 10;
    const int* var = tree->search(searchVal, tree);
    //const int* var2 = tree->search(0, tree);

    // Cleanup
    tree->shutdown(tree);
    free(tree);

    return 0;
}
#include "assn1q3.h"
#include "parse.h"
#include "prover.h"
#include "sequent.h"

#ifdef DBUG
static void printTree(Node, int);
#endif

int main(int argc, char **argv) {
    initNodes();
    //char str[100] = "[p imp q, (neg r) imp (neg q)] seq [p imp r]";
    //char str[100] = "[p iff q] seq [(p and q) or ((neg p) and (neg q))]";
    //char str[100] = "[neg (p or q)] seq [neg p]";
    //char str[100] = "[neg (p or q)] seq [neg p]";
    if (argc != 2) {
        fprintf(stderr, "Usage: %s '<sequent>'\n", argv[0]);
        exit(1);
    }
    char *str = argv[1];
    // Parse the input into a tree
    SequentL list = parse(str);
#ifdef DBUG
    for (size_t i = 0; i < list->nAtoms; i++)
        printf("Atom %ld: %s\n", i + 1, list->atoms[i]);
    for (size_t i = 0; i < list->seqs[0]->nLeft; i++) {
        printf("Left #%lu:\n", i);
        printTree(list->seqs[0]->left[i], 0);
    }
    for (size_t i = 0; i < list->seqs[0]->nRight; i++) {
        printf("Right #%lu:\n", i);
        printTree(list->seqs[0]->right[i], 0);
    }
#endif
    // Prove the sequent, and free all memory
    prove(list);
    freeSequentList(list);
    freeNodes();
    return 0;
}

#ifdef DBUG
// Print a parse tree
// root  - Root node of the tree
// depth - Depth of the node
static void printTree(Node root, int depth) {
    if (!root)
        return;
    printf("Type %d node at depth %d, data %lu, hash %" PRIu32 "\n", root->type, depth, root->value, root->hash);
    printTree(root->left, depth + 1);
    printTree(root->right, depth + 1);
    return;
}
#endif

// Panic and terminate the program
// msg - Message to print
void panic(char * msg) {
    fprintf(stderr, "%s\n", msg);
    exit(1);
}
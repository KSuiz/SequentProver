#include "sequent.h"

static Node *nodes = NULL;
static size_t nNodes = 0;
static size_t mNodes = 0;

static Node hash(Node);
static char compareParts(Node *, Node *, size_t, size_t);
static char compareTrees(Node, Node);
static char compareSequents(Sequent, Sequent);

// Initialise the global variables
void initNodes(void) {
    nodes = malloc(sizeof (Node));
    mNodes = 1;
    return;
}

// Create an empty sequent node
Node newNode(void) {
    Node newNode = calloc(1, sizeof (node));
    newNode->type = EMPTY;
    if (nNodes >= mNodes)
        nodes = realloc(nodes, (mNodes *= 2) * sizeof (Node *));
    nodes[nNodes++] = newNode;
    return newNode;
}

// Free all allocated nodes
void freeNodes(void) {
    for (size_t i = 0; i < nNodes; i++)
        free(nodes[i]);
    free(nodes);
    nodes = NULL;
    return;
}

// Create an empty sequent
Sequent newSequent(void) {
    Sequent seq = calloc(1, sizeof (sequent));
    seq->mLeft = seq->mRight = seq->mTo = 1;
    seq->left = malloc(sizeof (Node));
    seq->right = malloc(sizeof (Node));
    seq->to = malloc(sizeof (Sequent));
    return seq;
}

// Create an empty sequent list
SequentL newSequentList(void) {
    SequentL list = malloc(sizeof (sequentlist));
    list->mAtoms = 1;
    list->nAtoms = 0;
    list->atoms = malloc(sizeof (char **));
    list->mSeqs = 1;
    list->nSeqs = 0;
    list->seqs = malloc(sizeof (Sequent));
    return list;
}

// Add a sentence to a sequent
// seq     - Sequent to add to
// node    - Node to add
// isRight - Whether the node is to be inserted on the right
void addSentence(Sequent seq, Node node, char isRight) {
    if (!seq || !node)
        return;
    hash(node);
    size_t *n = &seq->nLeft;
    size_t *m = &seq->mLeft;
    Node **arr = &seq->left;
    if (isRight) {
        n = &seq->nRight;
        m = &seq->mRight;
        arr = &seq->right;
    }
    if (*n >= *m)
        *arr = realloc(*arr, (*m *= 2) * sizeof (Node));
    (*arr)[(*n)++] = node;
    return;
}

// Add a sequent to a sequent list
// list - List to add to
// seq  - Sequent to add
Sequent addSequent(SequentL list, Sequent seq) {
    if (!list || !seq)
        return NULL;
    // Only add it is it's not in the list already
    for (size_t i = 0; i < list->nSeqs; i++) {
        if (!compareSequents(list->seqs[i], seq))
            continue;
        // If duplicated, add links
        for (size_t j = 0; j < seq->nTo; j++)
            addLink(list->seqs[i], seq->to[j]);
        // Return the reference found
        return list->seqs[i];
    }
    // Expand the list as necessary
    if (list->nSeqs >= list->mSeqs)
        list->seqs = realloc(list->seqs, (list->mSeqs *= 2) * sizeof (Sequent));
    list->seqs[list->nSeqs++] = seq;
    return seq;
}

// Add a link from one sequent to another
// from - Sequent from which the link begins
// to   - Sequent to which the link refers
void addLink(Sequent from, Sequent to) {
    if (!from || !to)
        return;
    // Only add unique links
    for (size_t i = 0; i < from->nTo; i++)
        if (compareSequents(from->to[i], to))
            return;
    // Expand the list as necessary
    if (from->nTo >= from->mTo)
        from->to = realloc(from->to, (from->mTo *= 2) * sizeof (Sequent));
    from->to[from->nTo++] = to;
    return;
}

// Prints a tree in human-readbale format
// list - Sequent list with atoms
// root - Root of the tree
static void printTree(SequentL list, Node root) {
    switch (root->type) {
        case NEG:
            printf("neg ");
            if (root->right->type != ATOMIC)
                putchar('(');
            printTree(list, root->right);
            if (root->right->type != ATOMIC)
                putchar(')');
            break;

        case ATOMIC:
            printf("%s", list->atoms[root->value]);
            break;

        case OR:
        case AND:
        case IMP:
        case IFF:
            if (root->left->type != ATOMIC)
                putchar('(');
            printTree(list, root->left);
            if (root->left->type != ATOMIC)
                putchar(')');
            printf(" %s ", (
                root->type == OR ? "or" : (
                root->type == AND ? "and" : (
                root->type == IMP ? "imp" : "iff"
            ))));
            if (root->right->type != ATOMIC)
                putchar('(');
            printTree(list, root->right);
            if (root->right->type != ATOMIC)
                putchar(')');
            break;

        default:
            panic("\nInvalid tree in print");
    }
}

// Print a sequent
// list - Sequent list with atoms
// seq  - Sequent to print
void printSequent(SequentL list, Sequent seq) {
    putchar('[');
    for (size_t i = 0; i < seq->nLeft; i++) {
        if (i)
            printf(", ");
        printTree(list, seq->left[i]);
    }
    printf("] seq [");
    for (size_t i = 0; i < seq->nRight; i++) {
        if (i)
            printf(", ");
        printTree(list, seq->right[i]);
    }
    putchar(']');
    return;
}

// Free a sequent
// seq - Sequent to free
void freeSequent(Sequent seq) {
    free(seq->left);
    free(seq->right);
    free(seq->to);
    free(seq);
}

// Free a sequent list
// list - Sequent list to free
void freeSequentList(SequentL list) {
    for (size_t i = 0; i < list->nSeqs; i++)
        freeSequent(list->seqs[i]);
    for (size_t i = 0; i < list->nAtoms; i++)
        free(list->atoms[i]);
    free(list->atoms);
    free(list->seqs);
    free(list);
    return;
}

// Calculate the hash of a node
// node - Node to hash
static Node hash(Node node) {
    if (!node)
        return NULL;
    hash(node->left);
    hash(node->right);
    if (node->type == ATOMIC)
        node->hash = (node->value * 0x6F + 0xDCF3);
    else {
        uint32_t lHash = (node->left ? node->left->hash : 0);
        uint32_t rHash = (node->right ? node->right->hash : 0);
        node->hash = (((lHash & 0xFFFF) + (lHash >> 16)) & 0xFFFF) +
            ((((rHash & 0xFFFF) + (rHash >> 16)) & 0xFFFF) << 16);
    }
    return node;
}

// Compare two trees
// a - First tree
// b - Second tree
static char compareTrees(Node a, Node b) {
    // Trivial cases
    if (a == b)
        return 1;
    if (!a || !b)
        return 0;
    if (a->hash != b->hash)
        return 0;
    if (a->type != b->type)
        return 0;
    if (a->value != b->value)
        return 0;
    // Recursive comparison of subtrees
    if (!compareTrees(a->left, b->left))
        return 0;
    if (!compareTrees(a->right, b->right))
        return 0;
}

// Compare parts of a sequent, checking if one's a subset
// a  - First tree list
// b  - Second tree list
// na - First list size
// nb - Second list size
static char compareParts(Node *a, Node *b, size_t na, size_t nb) {
    // Loop through every tree in lists
    for (size_t i = 0; i < na; i++) {
        char located = 0;
        for (size_t j = 0; j < nb; j++) {
            if (a[i]->hash != b[j]->hash)
                continue;
            if (!compareTrees(a[i], b[i]))
                continue;
            located = 1;
        }
        // If one tree is not in the other, we can't have containment
        if (!located)
            return 0;
    }
    return 1;
}

// Compare two sequents for equality
// a - First sequent
// b - Second sequent
static char compareSequents(Sequent a, Sequent b) {
    // Trivial equality/inequality
    if (a == b)
        return 1;
    if (!a || !b)
        return 0;
    if (a->nLeft != b->nLeft)
        return 0;
    if (a->nRight != b->nRight)
        return 0;
    // Check the equality of the trees
    return (compareParts(a->left, b->left, a->nLeft, b->nLeft)
        && compareParts(b->left, a->left, b->nLeft, a->nLeft)
        && compareParts(a->right, b->right, a->nRight, b->nRight)
        && compareParts(b->right, a->right, b->nRight, a->nRight));
}

// Copy a sequent (shallow)
// seq - Sequent to copy
Sequent copySequent(Sequent seq) {
    Sequent new = newSequent();
    new->mLeft = seq->mLeft;
    new->mRight = seq->mRight;
    new->nLeft = seq->nLeft;
    new->nRight = seq->nRight;
    new->left = realloc(new->left, new->mLeft * sizeof (Node));
    for (size_t i = 0; i < new->nLeft; i++)
        new->left[i] = seq->left[i];
    new->right = realloc(new->right, new->mRight * sizeof (Node));
    for (size_t i = 0; i < new->nRight; i++)
        new->right[i] = seq->right[i];
    return new;
}

// Merge a sequent list into another, freeing b
// a - Sequent list to be merged into
// b - Sequent list to merge
void mergeSequentList(SequentL a, SequentL b) {
    for (size_t i = 0; i < b->nSeqs; i++) {
        Sequent ref = addSequent(a, b->seqs[i]);
        if (ref == b->seqs[i])
            continue;
        // If we have a duplicate, fix up forward references
        for (size_t j = 0; j < a->nSeqs; j++)
            for (size_t k = 0; k < a->seqs[j]->nTo; k++)
                if (a->seqs[j]->to[k] == b->seqs[i])
                    a->seqs[j]->to[k] = ref;
        for (size_t j = 0; j < i; j++)
            for (size_t k = 0; k < b->seqs[j]->nTo; k++)
                if (b->seqs[j]->to[k] == b->seqs[i])
                    b->seqs[j]->to[k] = ref;
    }
    free(b->seqs);
    free(b->atoms);
    free(b);
    return;
}
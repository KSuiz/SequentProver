#ifndef ASS1Q3_ASS1Q3_H_
#define ASS1Q3_ASS1Q3_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

typedef enum rule_t {
    R1,  // 0
    R2a, // 1
    R2b, // 2
    R3a, // 3
    R3b, // 4
    R4a, // 5
    R4b, // 6
    R5a, // 7
    R5b, // 8
    R6a, // 9
    R6b  // 10
} rule_t;

typedef enum con_t {
    EMPTY,  // 0
    NEG,    // 1
    AND,    // 2
    OR,     // 3
    IMP,    // 4
    IFF,    // 5
    ATOMIC  // 6
} con_t;

typedef struct node {
    struct node *left;
    struct node *right;
    con_t type;
    size_t value;
    uint32_t hash;
} node;
typedef node * Node;

typedef struct sequent {
    size_t nLeft;
    size_t mLeft;
    Node *left;
    size_t nRight;
    size_t mRight;
    Node *right;
    size_t nTo;
    size_t mTo;
    struct sequent **to;
    rule_t type;
    size_t index;
} sequent;
typedef sequent * Sequent;

typedef struct sequentlist {
    size_t nSeqs;
    size_t mSeqs;
    Sequent *seqs;
    size_t nAtoms;
    size_t mAtoms;
    char **atoms;
} sequentlist;
typedef sequentlist * SequentL;

void panic(char *);

#endif
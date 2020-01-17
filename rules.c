#include "rules.h"
#include "sequent.h"
#include "prover.h"

/*
 * This file contains the code relating to the rules. As much of the code is
 * duplicated and slightly modified for each rule, it isn't commented.
 * Basically, each rule (except 1) either:
 * A) copies the sequent and shifts around the structure of specific nodes
 *      on the left or right, as required by the rule.
 * B) Copies the sequent and modifies it as in A), but twice, and attempts
 *      to prove both new sequents in an isolated environment before merging
 *      the resulting proof structure into the main sequent list.
 * In both cases, we are looking for a certain root node on the left or
 * right side of the sequent to split or modify, and we apply the rule
 * to every valid root node we find.
 */

static char rule1(SequentL, Sequent);
static char rule2a(SequentL, Sequent);
static char rule2b(SequentL, Sequent);
static char rule3a(SequentL, Sequent);
static char rule3b(SequentL, Sequent);
static char rule4a(SequentL, Sequent);
static char rule4b(SequentL, Sequent);
static char rule5a(SequentL, Sequent);
static char rule5b(SequentL, Sequent);
static char rule6a(SequentL, Sequent);
static char rule6b(SequentL, Sequent);

char (*rules[11])(SequentL, Sequent) = {
    rule1,
    rule2a,
    rule2b,
    rule3a,
    rule3b,
    rule4a,
    rule4b,
    rule5a,
    rule5b,
    rule6a,
    rule6b
};

static char rule1(SequentL list, Sequent seq) {
    for (size_t i = 0; i < seq->nLeft; i++) {
        if (!(seq->left[i]->type == ATOMIC))
            continue;
        for (size_t j = 0; j < seq->nRight; j++) {
            if (!(seq->right[j]->type == ATOMIC))
                continue;
            if (seq->left[i]->value == seq->right[j]->value)
                return 1;
        }
    }
    return 0;
}

static char rule2a(SequentL list, Sequent seq) {
    for (size_t i = 0; i < seq->nRight; i++) {
        if (seq->right[i]->type != NEG)
            continue;
        Sequent new = copySequent(seq);
        new->type = R2a;
        Node temp = new->right[i];
        if (temp->left)
            temp = temp->left;
        else
            temp = temp->right;
        for (size_t j = i; j < new->nRight - 1; j++)
            new->right[j] = new->right[j + 1];
        new->nRight--;
        addSentence(new, temp, 0);
        Sequent ref = addSequent(list, new);
        if (ref == new)
            addLink(seq, new);
        else
            freeSequent(new);
    }
    return 0;
}

static char rule2b(SequentL list, Sequent seq) {
    for (size_t i = 0; i < seq->nLeft; i++) {
        if (seq->left[i]->type != NEG)
            continue;
        Sequent new = copySequent(seq);
        new->type = R2b;
        Node temp = new->left[i];
        if (temp->left)
            temp = temp->left;
        else
            temp = temp->right;
        for (size_t j = i; j < new->nLeft - 1; j++)
            new->left[j] = new->left[j + 1];
        new->nLeft--;
        addSentence(new, temp, 1);
        Sequent ref = addSequent(list, new);
        if (ref == new)
            addLink(seq, new);
        else
            freeSequent(new);
    }
    return 0;
}

static char rule3a(SequentL list, Sequent seq) {
    for (size_t i = 0; i < seq->nRight; i++) {
        if (seq->right[i]->type != AND)
            continue;
        Sequent new1 = copySequent(seq);
        new1->type = R3a;
        new1->right[i] = new1->right[i]->left;
        SequentL newL1 = newSequentList();
        addSequent(newL1, new1);
        if (!prover(newL1)){
            freeSequentList(newL1);
            return 0;
        }
        Sequent new2 = copySequent(seq);
        new2->type = R3a;
        new2->right[i] = new2->right[i]->right;
        SequentL newL2 = newSequentList();
        addSequent(newL2, new2);
        if (!prover(newL2)){
            freeSequentList(newL1);
            freeSequentList(newL2);
            return 0;
        }
        addLink(seq, new1);
        addLink(seq, new2);
        mergeSequentList(list, newL1);
        mergeSequentList(list, newL2);
        return 1;
    }
    return 0;
}

static char rule3b(SequentL list, Sequent seq) {
    for (size_t i = 0; i < seq->nLeft; i++) {
        if (seq->left[i]->type != AND)
            continue;
        Sequent new = copySequent(seq);
        new->type = R3b;
        Node temp = new->left[i];
        if (new->nLeft >= new->mLeft)
            new->left = realloc(new->left, (new->mLeft *= 2) * sizeof (Node));
        for (size_t j = new->nLeft; j > i + 1; j--)
            new->left[j] = new->left[j - 1];
        new->nLeft++;
        new->left[i] = seq->left[i]->left;
        new->left[i + 1] = seq->left[i]->right;
        addSequent(list, new);
        Sequent ref = addSequent(list, new);
        if (ref == new)
            addLink(seq, new);
        else
            freeSequent(new);
    }
    return 0;
}

static char rule4a(SequentL list, Sequent seq) {
    for (size_t i = 0; i < seq->nRight; i++) {
        if (seq->right[i]->type != OR)
            continue;
        Sequent new = copySequent(seq);
        new->type = R4a;
        if (new->nRight >= new->mRight)
            new->right = realloc(new->right, (new->mRight *= 2) * sizeof (Node));
        for (size_t j = new->nRight; j > i + 1; j--)
            new->right[j] = new->right[j - 1];
        new->nRight++;
        new->right[i] = seq->right[i]->left;
        new->right[i + 1] = seq->right[i]->right;
        addSequent(list, new);
        Sequent ref = addSequent(list, new);
        if (ref == new)
            addLink(seq, new);
        else
            freeSequent(new);
    }
    return 0;
}

static char rule4b(SequentL list, Sequent seq) {
    for (size_t i = 0; i < seq->nLeft; i++) {
        if (seq->left[i]->type != OR)
            continue;
        Sequent new1 = copySequent(seq);
        new1->type = R4b;
        new1->left[i] = new1->left[i]->left;
        SequentL newL1 = newSequentList();
        addSequent(newL1, new1);
        if (!prover(newL1)){
            freeSequentList(newL1);
            return 0;
        }
        Sequent new2 = copySequent(seq);
        new2->type = R4b;
        new2->left[i] = new2->left[i]->right;
        SequentL newL2 = newSequentList();
        addSequent(newL2, new2);
        if (!prover(newL2)){
            freeSequentList(newL1);
            freeSequentList(newL2);
            return 0;
        }
        addLink(seq, new1);
        addLink(seq, new2);
        mergeSequentList(list, newL1);
        mergeSequentList(list, newL2);
        return 1;
    }
    return 0;
}

static char rule5a(SequentL list, Sequent seq) {
    for (size_t i = 0; i < seq->nRight; i++) {
        if (seq->right[i]->type != IMP)
            continue;
        Sequent new = copySequent(seq);
        new->type = R5a;
        addSentence(new, seq->right[i]->left, 0);
        new->right[i] = seq->right[i]->right;
        addSequent(list, new);
        Sequent ref = addSequent(list, new);
        if (ref == new)
            addLink(seq, new);
        else
            freeSequent(new);
    }
    return 0;
}

static char rule5b(SequentL list, Sequent seq) {
    for (size_t i = 0; i < seq->nLeft; i++) {
        if (seq->left[i]->type != IMP)
            continue;
        Sequent new1 = copySequent(seq);
        new1->type = R5b;
        new1->left[i] = new1->left[i]->right;
        SequentL newL1 = newSequentList();
        addSequent(newL1, new1);
        if (!prover(newL1)){
            freeSequentList(newL1);
            return 0;
        }
        Sequent new2 = copySequent(seq);
        new2->type = R5b;
        addSentence(new2, new2->left[i]->left, 1);
        for (size_t j = i; j < new2->nLeft - 1; j++)
            new2->left[j] = new2->left[j + 1];
        new2->nLeft--;
        SequentL newL2 = newSequentList();
        addSequent(newL2, new2);
        if (!prover(newL2)){
            freeSequentList(newL1);
            freeSequentList(newL2);
            return 0;
        }
        addLink(seq, new1);
        addLink(seq, new2);
        mergeSequentList(list, newL1);
        mergeSequentList(list, newL2);
        return 1;
    }
    return 0;
}

static char rule6a(SequentL list, Sequent seq) {
    for (size_t i = 0; i < seq->nRight; i++) {
        if (seq->right[i]->type != IFF)
            continue;
        Sequent new1 = copySequent(seq);
        new1->type = R6a;
        addSentence(new1, new1->right[i]->right, 0);
        new1->right[i] = new1->right[i]->left;
        SequentL newL1 = newSequentList();
        addSequent(newL1, new1);
        if (!prover(newL1)){
            freeSequentList(newL1);
            return 0;
        }
        Sequent new2 = copySequent(seq);
        new2->type = R6a;
        addSentence(new2, new2->right[i]->left, 0);
        new2->right[i] = new2->right[i]->right;
        SequentL newL2 = newSequentList();
        addSequent(newL2, new2);
        if (!prover(newL2)){
            freeSequentList(newL1);
            freeSequentList(newL2);
            return 0;
        }
        addLink(seq, new1);
        addLink(seq, new2);
        mergeSequentList(list, newL1);
        mergeSequentList(list, newL2);
        return 1;
    }
    return 0;
}

static char rule6b(SequentL list, Sequent seq) {
    for (size_t i = 0; i < seq->nLeft; i++) {
        if (seq->left[i]->type != IFF)
            continue;
        Sequent new1 = copySequent(seq);
        new1->type = R6b;
        if (new1->nLeft >= new1->mLeft)
            new1->left = realloc(new1->left, (new1->mLeft *= 2) * sizeof (Node));
        for (size_t j = new1->nLeft; j > i + 1; j--)
            new1->left[j] = new1->left[j - 1];
        new1->nLeft++;
        new1->left[i + 1] = new1->left[i]->right;
        new1->left[i] = new1->left[i]->left;
        SequentL newL1 = newSequentList();
        addSequent(newL1, new1);
        if (!prover(newL1)){
            freeSequentList(newL1);
            return 0;
        }
        Sequent new2 = copySequent(seq);
        new2->type = R6b;
        addSentence(new2, new2->left[i]->left, 1);
        addSentence(new2, new2->left[i]->right, 1);
        for (size_t j = i; j < new2->nLeft - 1; j++)
            new2->left[j] = new2->left[j + 1];
        new2->nLeft--;
        SequentL newL2 = newSequentList();
        addSequent(newL2, new2);
        if (!prover(newL2)){
            freeSequentList(newL1);
            freeSequentList(newL2);
            return 0;
        }
        addLink(seq, new1);
        addLink(seq, new2);
        mergeSequentList(list, newL1);
        mergeSequentList(list, newL2);
        return 1;
    }
    return 0;
}
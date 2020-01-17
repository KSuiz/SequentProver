#include "prover.h"
#include "rules.h"
#include "sequent.h"

static Sequent makeProof(SequentL, SequentL, Sequent);
static char *ruleToStr(rule_t);

// Prove or disprove a sequent
// list - List with initial sequent to use
void prove(SequentL list) {
    // Try to prove it
    if (!prover(list)){
        printf("false\n");
        return;
    }
    // A proof succeeded, so now we reconstruct the proof
    printf("true\n");
    SequentL proof = newSequentList();
    SequentL found = newSequentList();
    // Make the proof and put it in the proof variable
    makeProof(found, proof, list->seqs[0]);
    freeSequentList(found);
    // Loop through every sequent in the list, in order
    for (size_t i = 0; i < proof->nSeqs; i++) {
        proof->seqs[i]->index = i + 1;
        // Print the step and its index
        printf("%lu: ", proof->seqs[i]->index);
        printSequent(list, proof->seqs[i]);
        // Print this step's reasoning
        if (!proof->seqs[i]->nTo) {
            printf(" ... via P1\n");
        } else {
            Sequent ref = proof->seqs[i]->to[0];
            printf(" ... via %s from %lu", ruleToStr(ref->type), ref->index);
            // If the type involves two sequent, print both
            if (ref->type == R3a
                || ref->type == R4b
                || ref->type == R5b
                || ref->type == R6a
                || ref->type == R6b)
                printf(", %lu", proof->seqs[i]->to[1]->index);
            putchar('\n');
        }
    }
    freeSequentList(proof);
    return;
}

// Generate a proof from a proven sequent list
// found - A list to keep track of found sequents
// proof - The constructed proof in order
// step  - The current sequent
static Sequent makeProof(SequentL found, SequentL proof, Sequent step) {
    // Check if we've already considered this sequent
    Sequent duplicate = copySequent(step);
    Sequent ref = addSequent(found, duplicate);
    if (ref != duplicate) {
        freeSequent(duplicate);
        return NULL;
    }
    // Check if the sequent is terminal, but invalid
    if (!step->nTo && !(*rules[0])(NULL, step))
        return NULL;
    Sequent *links = malloc(step->nTo * sizeof (Sequent));
    // Loop outgoing links and collect the references in the proof list
    for (size_t i = 0; i < step->nTo; i++) {
        Sequent ref = makeProof(found, proof, step->to[i]);
        if (!ref) {
            // ref is NULL, so the proof is invalid, and we remove it from
            // the reference list of step
            for (size_t j = i + 1; j < step->nTo; j++)
                step->to[j - 1] = step->to[j];
            step->nTo--;
            i--;
        } else 
            links[i] = ref;
    }
    // If all outgoing steps are invalid, check if it is terminal
    if (!step->nTo && !(*rules[0])(NULL, step)) {
        free(links);
        return NULL;
    }
    // Duplicate the sequent again, as we need a reference in both the
    // found and proof lists, and link it to the right references
    duplicate = copySequent(duplicate);
    for (size_t i = 0; i < step->nTo; i++)
        addLink(duplicate, links[i]);
    duplicate->type = step->type;
    addSequent(proof, duplicate);
    free(links);
    return duplicate;
}

// Convert a rule into its string representation
// type - Rule to convert
static char *ruleToStr(rule_t type) {
    if (type == R1)
        return "P1";
    if (type == R2a)
        return "P2a";
    if (type == R2b)
        return "P2b";
    if (type == R3a)
        return "P3a";
    if (type == R3b)
        return "P3b";
    if (type == R4a)
        return "P4a";
    if (type == R4b)
        return "P4b";
    if (type == R5a)
        return "P5a";
    if (type == R5b)
        return "P5b";
    if (type == R6a)
        return "P6a";
    return "P6b";
}

// Main BFS loop for proving a sequent
// list - List with initial sequent to use
char prover(SequentL list) {
    // list->seqs acts as a quue for a BFS, excluding branching. If the
    // proof branches (e.g. P6a), then the entire branch is proven before
    // the result bubbles up
    for (size_t i = 0; i < list->nSeqs; i++) {
        // Check if the sequent is valid via P1
        if (list->seqs[i]->nLeft && list->seqs[i]->nRight)
            if ((*rules[0])(list, list->seqs[i]))
                return 1;
        // Otherwise loop the other 10 rules
        for (size_t j = 1; j < 11; j++)
            if ((*rules[j])(list, list->seqs[i]))
                return 1;
    }
    // We didn't find a solution using any rules, so bubble
    return 0;
}

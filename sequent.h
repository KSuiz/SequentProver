#ifndef ASSN1Q3_SEQUENT_H_
#define ASSN1Q3_SEQUENT_H_

#include "assn1q3.h"

void initNodes(void);
void freeNodes(void);

void freeSequent(Sequent);
void freeSequentList(SequentL);

void printSequent(SequentL, Sequent);

Node newNode(void);
Sequent newSequent(void);
SequentL newSequentList(void);

void addSentence(Sequent, Node, char);
Sequent addSequent(SequentL, Sequent);
void addLink(Sequent, Sequent);
Sequent copySequent(Sequent);
void mergeSequentList(SequentL, SequentL);

#endif
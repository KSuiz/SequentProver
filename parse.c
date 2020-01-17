#include "parse.h"
#include "sequent.h"

static Node process(SequentL, char **, Node);
static char *burn(char *);
static char isHigher(con_t, con_t);
static Node parseTower(SequentL, char **);
static con_t strToCon(char *);
static int addAtom(SequentL, char *);
static void processSide(SequentL, char **, char);
static char *preprocess(char *);

// Parse a string into a sequent
// seqStr - String to parse
SequentL parse(char* seqStr) {
    seqStr = preprocess(seqStr);
    char *str = seqStr;
    // Make an empty sequent
    SequentL list = newSequentList();
    addSequent(list, newSequent());
    // Burn whitespace and procees each side of the sequent
    str = burn(str);
    if (str[0] != '[')
        panic("Invalid input format.");
    str++;
    processSide(list, &str, 0);
    if (strncmp(str, "seq", 3))
        panic("Invalid input format.");
    str += 3;
    str = burn(str);
    if (str[0] != '[')
        panic("Invalid input format.");
    str++;
    processSide(list, &str, 1);
    free(seqStr);
    return list;
}

// Add in buffer parentheses around sentences to catch precedence delegation
// If at the root level, the parsing results in delegating precedence chacking
// to the caller, we quit the parsing too early, so we add a buffer
// seqStr - String to preprocess
static char *preprocess(char *seqStr) {
    // Count how many pairs of parentheses we need to add
    size_t comma = 2;
    char *ptr = seqStr;
    while (*ptr != '\0')
        if (*(ptr++) == ',')
            comma++;
    // Allocate enough space
    size_t len = (size_t) (ptr - seqStr) + 1 + comma * 2;
    char *str = malloc(len * sizeof (char));
    ptr = seqStr;
    // Copy string over character by character, adding in parentheses
    for (size_t i = 0; i < len; i++, ptr++) {
        if (*ptr == ',') {
            str[i++] = ')';
            str[i++] = ',';
            str[i] = '(';
        } else if (*ptr == '[') {
            str[i++] = '[';
            str[i] = '(';
        } else if (*ptr == ']') {
            str[i++] = ')';
            str[i] = ']';
        } else
            str[i] = *ptr;
    }
    if (str[len - 1] != '\0')
        panic("Preprocessing failed.");
#ifdef DBUG
    printf("Preprocessed to \"%s\"\n", str);
#endif
    return str;
}

// Process one side of the sequent
// list    - List to fill atoms
// str     - String pointer to modify as we read
// isRight - Whether to add to the right side of the sequent
static void processSide(SequentL list, char **str, char isRight) {
    // Loop until the end of the side
    while (**str != ']') {
        // Process the sentence and add it
        Node root = process(list, str, NULL);
        addSentence(list->seqs[0], root, isRight);
        // Burn until the next token, and continue to next sentence or end
        *str = burn(*str);
        if (**str == ',') {
            (*str)++;
            *str = burn(*str);
        }
    }
    // Move pointer beyond closing bracket and burn whitespace
    (*str)++;
    *str = burn(*str);
    return;
}

// Burn whitespace from a string
// str - String to burn whitespace from
static char *burn(char *str) {
    int offset = 0;
    sscanf(str, "%*[ ]%n", &offset);
    return str + offset;
}

// Determine if a connective is higher (or equal) precedence
// Due to right associativity, equal precedence is effectively higher
// a - Connective to check if higher
// b - Connective to check if lower
static char isHigher(con_t a, con_t b) {
    // NEG is always higher
    if (a == NEG)
        return 1;
    // OR/AND is higher than all but NEG
    if (a == OR || a == AND)
        return (b != NEG);
    // IMP/IFF is only higher if b is either
    return (b == IMP || b == IFF);
}

// Parse a negation tower
// list - List to add atoms to
// ptr  - String pointer to modify
static Node parseTower(SequentL list, char **ptr) {
    *ptr = burn(*ptr);
    int offset = 0;
    char *buffer = malloc((strlen(*ptr) + 1) * sizeof (char));
    // Read the next token
    int found = sscanf(*ptr, "%[^](), ]%n", buffer, &offset);
    Node this = newNode();
    if (found) {
        // If we found a token, check it
        *ptr += offset;
        if (!strcmp(buffer, "neg")) {
            // If we have negation, save it and parse the remaining tower
            this->type = NEG;
            this->right = parseTower(list, ptr);
        } else {
            // Otherwise, it must be atomic
            this->type = ATOMIC;
            this->value = addAtom(list, buffer);
        }
    } else {
        // No token can only happen when we have a new scope, so process it
        if (**ptr != '(')
            panic("Invalid input format.");
        (*ptr)++;
        this = process(list, ptr, NULL);
    }
    free(buffer);
    return this;
}

// Convert a string to a connective type
// str - String to convert
static con_t strToCon(char *str) {
    if (!strcmp(str, "neg"))
        return NEG;
    if (!strcmp(str, "or"))
        return OR;
    if (!strcmp(str, "and"))
        return AND;
    if (!strcmp(str, "imp"))
        return IMP;
    if (!strcmp(str, "iff"))
        return IFF;
    if (!str[0])
        return EMPTY;
    return ATOMIC;
}

// Process a string into a parse tree
// list - List to add atoms to
// ptr  - String pointer to modify
// left - Left subtree from precedence handling or NULL
static Node process(SequentL list, char **ptr, Node left) {
    char *buffer = malloc((strlen(*ptr) + 1) * sizeof (char));
    buffer[0] = '(';
    int offset = 0;
    Node this = newNode();
    // If this call results from precedence handling, set the left subtree
    if (left)
        this->left = left;
    this->type = EMPTY;
    // Loop until a break condition is met
    char *temp = *ptr;
    while (1) {
        *ptr = burn(*ptr);
        int found = sscanf(*ptr, "%[^](), ]%n", buffer, &offset);
        if (found == 1) {
            // If we found a token, handle it
            *ptr += offset;
            if (this->type == EMPTY && !this->left) {
                // If we have no left tree, make one
                if (!strcmp(buffer, "neg")) {
                    // If we have negation, parse the tower
                    *ptr -= offset;
                    this->left = parseTower(list, ptr);
                } else {
                    // Otherwise we must have an atom, so add it
                    Node atom = newNode();
                    atom->type = ATOMIC;
                    atom->value = addAtom(list, buffer);
                    this->left = atom;
                }
            } else if (this->type == EMPTY) {
                // Otherwise, we have a left subtree but no type
                this->type = strToCon(buffer);
                // Type must be a binary connective
                if (this->type == NEG
                    || this->type == ATOMIC
                    || this->type == EMPTY)
                    panic("Invalid input format.");
            } else if (!this->right) {
                // Otherwise we are parsing the right of a connective
                if (!strcmp(buffer, "neg")) {
                    // If we have negation, parse the tower
                    *ptr -= offset;
                    this->right = parseTower(list, ptr);
                } else {
                    // Otherwise we have an atom, add it
                    Node atom = newNode();
                    atom->type = ATOMIC;
                    atom->value = addAtom(list, buffer);
                    this->right = atom;
                }
            } else {
                // Otherwise, we have three or more terms (no scoping)
                *ptr -= offset;
                con_t type = strToCon(buffer);
                if (type == NEG
                    || type == ATOMIC
                    || type == EMPTY)
                    panic("Invalid input format.");
                // Depending on precedence, either handle it immediately,
                // or defer to caller to check against operator precedence
                if (isHigher(type, this->type))
                    this->right = process(list, ptr, this->right);
                else
                    break;
            }
        } else {
            // Otherwise, we have some sort of grouping start/end
            if (**ptr == '(') {
                (*ptr)++;
                // If we enter a new scope, process the scope,
                // and set appropriate subtrees
                Node new = process(list, ptr, NULL);
                if (!this->left)
                    this->left = new;
                else {
                    if (this->type == EMPTY)
                        panic("Invalid input format.");
                    this->right = new;
                }
            } else if (**ptr == ')'
                || **ptr == '\0'
                || **ptr == ','
                || **ptr == ']') {
                // Otherwise we end the grouping and return up the call stack
                if (**ptr == ')')
                    (*ptr)++;
                break;
            }
        }
        // Failsafe for infinite loop
        if (temp == *ptr)
            panic("Unterminated loop detected.");
        temp = *ptr;
    }
    free(buffer);
    // Collapse empty nodes
    if (this->type == EMPTY) {
        Node temp = this->left;
        return temp;
    } else if (!this->right)
        panic("Invalid input format.");
    return this;
}

// Add an atom to the sequent
// list - Sequent list to add to
// atom - Atom to add
static int addAtom(SequentL list, char *atom) {
    // If we already have the atom, return its index
    for (size_t i = 0; i < list->nAtoms; i++)
        if (!strcmp(list->atoms[i], atom))
            return i;
    // Resize the array if needed
    if (list->nAtoms >= list->mAtoms)
        list->atoms = realloc(list->atoms, (list->mAtoms *= 2) * sizeof (char *));
    // Add the atom to the array and return the index
    list->atoms[list->nAtoms] = malloc((strlen(atom) + 1) * sizeof (char));
    strcpy(list->atoms[list->nAtoms++], atom);
    return list->nAtoms - 1;
}
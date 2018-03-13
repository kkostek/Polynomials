#include <stdio.h>
#include <stdlib.h>
#include "stack.h"
#include "poly.h"
#define DEFAULT_SIZE 12

bool isEmpty(stack* s) {
    return (s->size == 0);
}

int size(stack* s) {
    return s->size;
}

stack* newStack() {
    stack* myNewStack = malloc(sizeof(stack));
    myNewStack->size = 0;
    myNewStack->sizeOfTab = DEFAULT_SIZE;
    myNewStack->tab = malloc(DEFAULT_SIZE * sizeof(Poly));
    return myNewStack;
}

Poly pop(stack* s) {
    assert(!isEmpty(s));
    return s->tab[--s->size];
}

Poly top(stack* s) {
    assert(!isEmpty(s));
    return s->tab[s->size - 1];
}

void push(stack* s, Poly p) {
    if (s->size == s->sizeOfTab) {
        s->sizeOfTab *= 2;
        s->tab = realloc(s->tab, s->sizeOfTab * sizeof(Poly));
    }
    s->tab[s->size++] = PolyClone(&p);
}

void deleteStack(stack* s) {
    for(int i = 0; i < s->size; ++i) {
        PolyDestroy(&s->tab[i]);
    }
    free(s->tab);
    free(s);
}

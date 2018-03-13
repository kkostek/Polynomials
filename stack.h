#ifndef __STACK_H__
#define __STACK_H__

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include "poly.h"

/**
 * struktura stosu
 */
typedef struct stack {
    int size;
    int sizeOfTab;
    Poly* tab;
} stack;

/**
 * @brief newStack
 * konstruktor nowego stosu
 */
stack* newStack();
/**
 * @brief isEmpty
 * zwraca, czy stos jest pusty
 * @param s - stos
 * @return `czy s jest pusty?`
 */
bool isEmpty(stack* s);

/**
 * @brief size
 * zwraca rozmiar stosu
 * @param s - stos
 * @return `s.size()`
 */
int size(stack* s);
/**
 * @brief pop
 * zwraca wierzchołek stosu i usuwa go z niego
 * @param s - stos
 * @return `wierzchołek s`
 */
Poly pop(stack* s);

/**
 * @brief top
 * zwraca wierzchołek stosu
 * @param s - stos
 * @return `werzchołek s`
 */
Poly top(stack* s);

/**
 * @brief push
 * dokłada wielomian p na wierzchołek stosu s
 * @param s
 * @param p
 */
void push(stack* s, Poly p);
/**
 * @brief deleteStack
 * usuwa stos
 * @param s
 */
void deleteStack(stack* s);

#endif /* __STACK_H__ */

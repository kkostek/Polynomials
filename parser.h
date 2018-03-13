#ifndef __PARSER_H__
#define __PARSER_H__

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include "poly.h"
#include "stack.h"
#include "utils.h"

typedef struct Monobool {
    Mono m;
    int iter;
    bool b;
} Monobool;

typedef struct Polybool {
    Poly p;
    int iter;
    bool b;
} Polybool;

typedef struct longBool {
    long k;
    int iter;
    bool b;
}
longBool;

typedef struct intBool {
    unsigned k;
    int iter;
    bool b;
} intBool;

/**
 * @brief parsePoly
 * wczytuje wielomian ze standardowego wejścia i wrzuca na stos s; Jeśli napotka błąd, wypisuje numer wersza i kolumny,
 * w których natrafił na błąd
 * @param[in] s : stack - stos
 * @param[in] row : unsigned - numer wiersza
 * @param[in] column : unsigned - numer kolumny
 */
Polybool parsePoly(stack* s, unsigned row, unsigned column, char toChar);

/**
 * @brief parseMono
 * wczytuje jednomian ze standardowego wejścia; Jeśli napotka błąd, wypisuje numer wersza i kolumny,
 * w których natrafił na błąd
 * @param[in] s : stack - stos
 * @param[in] row
 * @param column
 */
Monobool parseMono(stack* s, unsigned row, unsigned column);

/**
 * @brief parseCommand
 * wczytuje komendę ze standardowego wejścia i ją wykonuje, lub wypisuje komunikat o błędzie na stderr, gdzy napotka
 * błąd;
 * @param[in] s : stack - stos
 * @param[in] row : unsigned - numer wiersza
 */
void parseCommand(stack* s, unsigned row);

/**
 * @brief intFromChar
 * zwraca inta danego chara pomniejszonego o inta znaku '0'
 * @param c
 * @return `c - '0'`
 */
int intFromChar(char c);

#endif /* __PARSER_H__ */

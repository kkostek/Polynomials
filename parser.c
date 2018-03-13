#include "poly.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "stack.h"
#include "parser.h"
#define BUFOR 42
#define MONOB 12
typedef char* string;

int intFromChar(char c) {
    return (c - '0');
}

/**
 * @brief error
 * wypisuje komendę error (dla parsowania wielomianów) na stderr
 * @param row
 * @param column
 */
void error(unsigned row, unsigned column) {
    fprintf(stderr, "ERROR %u %u\n", row, column);
}

/**
 * @brief parseLong
 * wczytuje znaki i jeżeli możliwe, tworzy z nich liczbę typu long
 * @param row
 * @return
 */
longBool parseLong(unsigned row) {
    char c;
    longBool res;
    res.k = 0;
    bool sign = true;
    res.b = true;
    if ((c = getchar()) != '\n') {
        if (c == '-') {
            sign = false;
        }
        else {
            ungetc(c, stdin);
        }
    }
    while ((c = getchar()) != '\n') {
        if (intFromChar(c) >= 0 && intFromChar(c) <= 9) {
            if (sign) {
                if(res.k <= ((LONG_MAX - intFromChar(c)) / 10)) {
                    res.k = 10 * res.k + intFromChar(c);
                }
                else {
                    fprintf(stderr, "ERROR %d WRONG VALUE\n", row);
                    res.b = false;
                    while((c = getchar()) != '\n');
                    return res;
                }
            }
            else if (res.k >= (LONG_MIN - intFromChar(c)) / 10) {
                res.k = 10 * res.k - intFromChar(c);
            }
            else {
                fprintf(stderr, "ERROR %d WRONG VALUE\n", row);
                res.b = false;
                while((c = getchar()) != '\n');
                return res;
            }
        }
        else {
            fprintf(stderr, "ERROR %d WRONG VALUE\n", row);
            res.b = false;
            while((c = getchar()) != '\n');
            return res;
        }
    }
    return res;
}

/**
 * @brief parseUnsigned
 * wczytuje znaki i jeżeli możliwe, tworzy z nich liczbę typu unsigned
 * @param row
 * @return
 */
intBool parseUnsigned(unsigned row, string s) {
    char c;
    intBool res;
    res.k = 0;
    res.iter = 0;
    res.b = true;
    if((c = getchar()) == '\n') {
        fprintf(stderr, "ERROR %d WRONG %s\n", row, s);
        res.b = false;
        return res;
    } else {
        ungetc(c, stdin);
    }
    while ((c = getchar()) != '\n') {
        if (intFromChar(c) >= 0 && intFromChar(c) <= 9) {
            if(res.k <= ((UINT_MAX - intFromChar(c)) / 10)) {
                    res.k = 10 * res.k + intFromChar(c);
            }
            else {
                fprintf(stderr, "ERROR %d WRONG %s\n", row, s);
                 res.b = false;
                while((c = getchar()) != '\n');
                return res;
            }
        }
        else {
            fprintf(stderr, "ERROR %d WRONG %s\n", row, s);
            res.b = false;
            while((c = getchar()) != '\n');
            return res;
        }
    }
    return res;
}

/**
 * @brief parseLong
 * wczytuje znaki i jeżeli możliwe, tworzy z nich liczbę typu long - współczynniki do wielomianu
 * @param row
 * @return
 */
longBool parseLongPoly(unsigned row, unsigned column, char toChar) {
    char c = getchar();
    longBool res;
    res.iter = column;
    res.k = 0;
    bool sign = true;
    res.b = true;
    if (c == '-') {
        sign = false;
    }
    else {
        ungetc(c, stdin);
    }
    while ((c = getchar()) != toChar && c != '\n') {
        res.iter++;
        if (intFromChar(c) >= 0 && intFromChar(c) <= 9) {
                      if (sign) {
                if(res.k <= ((LONG_MAX - intFromChar(c)) / 10)) {
                    res.k = 10 * res.k + intFromChar(c);
                }
                else {
                    error(row, res.iter);
                    res.b = false;
                    res.k = 0;
                    while((c = getchar()) != '\n' /*&&  c != toChar*/);
                    return res;
                }
            }
            else {
                if(res.k >= ((LONG_MIN + intFromChar(c)) / 10)) {
                    res.k = 10 * res.k - intFromChar(c);
                }
                else {
                    error(row, res.iter);
                    res.b = false;
                    res.k = 0;
                    while((c = getchar()) != '\n');
                    return res;
                }
            }
        }
        else {
            error(row, res.iter);
            res.b = false;
            res.k = 0;
            while((c = getchar()) != '\n');
            return res;
        }
    }
    if (c != '\n') {
        ungetc(c, stdin);
    }
    return res;
}

/**
 * @brief parseLong
 * wczytuje znaki i jeżeli możliwe, tworzy z nich liczbę typu int - wykładnik wielomianu
 * @param row
 * @return
 */
intBool parseExp(unsigned row, unsigned column, char toChar) {
    char c;
    intBool res;
    res.k = 0;
    res.iter = column;
    res.b = true;
    while ((c = getchar()) != toChar) {
        res.iter++;
        if (intFromChar(c) >= 0 && intFromChar(c) <= 9) {
            if(res.k <= ((UINT_MAX - intFromChar(c)) / 10)) {
                    res.k = 10 * res.k + intFromChar(c);
            }
            else {
                error(row, res.iter);
                res.b = false;
                while((c = getchar()) != toChar && c != '\n');
                return res;
            }
        }
        else {
            error(row, res.iter);
            res.b = false;
            while((c = getchar()) != '\n');
            return res;
        }
    }
    if (c == toChar) {
        ungetc(c, stdin);
    }

    return res;
}

Monobool parseMono(stack* s, unsigned row, unsigned column);

Polybool parsePoly(stack* s, unsigned row, unsigned column, char toChar) {
    char c;
    Polybool res;
    res.p = PolyZero();
    int count = 0;
    int MONOSIZE = 12;
    Mono* monos = malloc(MONOSIZE * sizeof(Mono));
    res.b = true;
    res.iter = column;
    while((c = getchar()) != toChar && c != '\n') {
        res.iter++;
        if (c == '(') {
            ungetc(c, stdin);
            Monobool m = parseMono(s, row, res.iter);
            if (m.b) {
                if (count == MONOSIZE - 1) {
                    MONOSIZE *= 2;
                    monos = realloc(monos, MONOSIZE * sizeof(Mono));
                }
                monos[count++] = m.m;
            }
            else {
                res.b = false;
                free(monos);
                return res;
            }
        }
        else if (c == '-' || (intFromChar(c) >=0 && intFromChar(c) <= 9)) {
            ungetc(c, stdin);
            longBool lb = parseLongPoly(row, res.iter, ',');

           if (lb.b) {
               res.iter = lb.iter;
               res.p = PolyFromCoeff(lb.k);
               free(monos);
               return res;
           }
           else {
               res.b = false;
               for(int i = 1; i < count; ++i) {
                   MonoDestroy(&monos[i]);
               }
               free(monos);
               return res;
           }
        }
        else {
            error(row, res.iter);
            res.b = false;
            free(monos);
            return res;
        }
    }
    if(c != '\n') {
        ungetc(c, stdin);
    }


    res.p = PolyAddMonos(count, monos);

    free(monos);
    return res;
}

Monobool parseMono(stack* s, unsigned row, unsigned column) {
    char c;
    Monobool res;
    res.iter = column;
    res.b = true;
    if ((c = getchar()) != '(') {
        error(row, res.iter);
        res.b = false;
        while((c = getchar()) != '\n');
        return res;
    }
    else {
        Polybool p = parsePoly(s, row, res.iter, ',');
        if (p.b) {
            res.m.p = p.p;
            res.iter = p.iter;
        }
        else {
            res.iter = p.iter;
            res.b = false;
            return res;
        }
    }
    res.iter++;
    if ((c = getchar()) == ',') {
        intBool exp = parseExp(row, res.iter, ')');
        if (exp.b) {
            res.iter = exp.iter;
            res.m.exp = exp.k;
        }
        else {
            res.b = false;
            return res;
        }
    }
    else {
        error(row, res.iter);
        res.b = false;
        return res;
    }
    res.iter++;
    if ((c = getchar()) == ')') {
        if(c != ')' && c != '\n') {
            ungetc(c, stdin);
        }

        if((c = getchar()) != '+') {
            ungetc(c, stdin);
        }
        return res;
    }
    else {
        error(row, res.iter);
        res.b = false;
        while((c = getchar()) != '\n') {
        }
        return res;
    }
}

/**
 * @brief underflow
 * wypisuje komunitkat "stack underflow" na stderr
 * @param row
 */
void underflow(unsigned row) {
    fprintf(stderr, "ERROR %u STACK UNDERFLOW\n", row);
}

void parseCommand(stack* s, unsigned row) {
    string word = malloc(BUFOR * sizeof(char));
    int i = 0;
    char c;
    while(i < BUFOR - 1 && (c = getchar()) != '\n') {
        word[i++] = c;
    }
    if (i == BUFOR - 1) {
       while((c = getchar()) != '\n');
    }

    word[i] = '\0';

    if (strcmp(word, "ADD") == 0) {
        if (size(s) > 1) {
            Poly p = pop(s);
            Poly q = pop(s);
            Poly w = PolyAdd(&p, &q);
            PolyDestroy(&q);
            push(s, w);
            PolyDestroy(&w);
            PolyDestroy(&p);
        }
        else {
           underflow(row);
        }
    }

    else if (strcmp(word, "CLONE") == 0) {
        if (!isEmpty(s)) {
            Poly p = top(s);
            push(s, p);
        }
        else {
           underflow(row);
        }
    }

    else if (strcmp(word, "DEG") == 0) {
        if (!isEmpty(s)) {
            Poly p = top(s);
            printf("%d\n", PolyDeg(&p));
        }
        else {
           underflow(row);
        }
    }

    else if (strcmp(word, "IS_COEFF") == 0) {
        if (!isEmpty(s)) {
            Poly p = top(s);
            printf("%d\n", PolyIsCoeff(&p));
        }
        else {
           underflow(row);
        }
    }

    else if (strcmp(word, "IS_ZERO") == 0) {
        if (!isEmpty(s)) {
            Poly p = top(s);
            printf("%d\n", PolyIsZero(&p));
        }
        else {
           underflow(row);
        }
    }

    else if (strcmp(word, "IS_EQ") == 0) {
        if (size(s) > 1) {
            Poly p = pop(s);
            Poly q = top(s);
            push(s, p);
            printf("%d\n", PolyIsEq(&p, &q));
            PolyDestroy(&p);
        }
        else {
           underflow(row);
        }
    }

    else if (strcmp(word, "MUL") == 0) {
        if (size(s) > 1) {
            Poly p = pop(s);
            Poly q = pop(s);
            Poly xd = PolyMul(&p, &q);
            push(s, xd);
            PolyDestroy(&p);
            PolyDestroy(&q);
            PolyDestroy(&xd);
        }
        else {
           underflow(row);
        }
    }

    else if (strcmp(word, "NEG") == 0) {
        if (!isEmpty(s)) {
            Poly p = pop(s);
            Poly xd = PolyNeg(&p);
            push(s, xd);
            PolyDestroy(&p);
            PolyDestroy(&xd);
        }
        else {
           underflow(row);
        }
    }

    else if (strcmp(word, "PRINT") == 0) {
        if (!isEmpty(s)) {
            Poly p = top(s);
            Print(&p);
            printf("\n");        }
        else {
           underflow(row);
        }
    }

    else if (strcmp(word, "POP") == 0) {
        if (!isEmpty(s)) {
            Poly xd = pop(s);
            PolyDestroy(&xd);
        }
        else {
           underflow(row);
        }
    }

    else if (strcmp(word, "SUB") == 0) {
        if (size(s) > 1) {
            Poly p = pop(s);
            Poly q = pop(s);
            Poly xd = PolySub(&p, &q);
            push(s, xd);
            PolyDestroy(&p);
            PolyDestroy(&q);
            PolyDestroy(&xd);
        }
        else {
           underflow(row);
        }
    }

    else if (strcmp(word, "ZERO") == 0) {
        push(s, PolyZero());
    }

    else if (i > 2 && word[0] == 'A' && word[1] == 'T' && word[2] == ' ') {
        if (!isEmpty(s)) {
            ungetc('\n', stdin);
            for (int j = i - 1; j > 2; j--) {
                ungetc(word[j], stdin);
            }
            longBool w = parseLong(row);
            if (w.b) {
                Poly p = pop(s);
                Poly xd = PolyAt(&p, w.k);
                push(s, xd);
                PolyDestroy(&p);
                PolyDestroy(&xd);
            }
        }
        else {
            underflow(row);
        }
    }
    else if (i > 6 && word[0] == 'D' && word[1] == 'E' && word[2] == 'G' && word[3] == '_' && word[4] == 'B'
             && word[5] == 'Y' && word[6] == ' ') {
        if (!isEmpty(s)) {
            ungetc('\n', stdin);
            for (int j = i - 1; j > 6; j--) {
                ungetc(word[j], stdin);
            }
            intBool w = parseUnsigned(row, "VARIABLE");
            if (w.b) {
                Poly p = top(s);
                printf("%d\n", PolyDegBy(&p, w.k));
            }
        }
        else {
            underflow(row);
        }
    }
    else if (i >= 7 && word[0] == 'C' && word[1] == 'O' && word[2] == 'M' && word[3] == 'P' && word[4] == 'O'
             && word[5] == 'S' && word[6] == 'E' && word[7] == ' '){
        if (!isEmpty(s)) {
            ungetc('\n', stdin);
            for (int j = i - 1; j > 7; j--) {
                ungetc(word[j], stdin);
            }
            intBool w = parseUnsigned(row, "COUNT");
            if (w.b) {
                unsigned count = w.k;
                if ((unsigned) s->size > count) {
                    Poly x[count];
                    Poly p = pop(s);
                    for (unsigned i = 0; i < count; i++) {
                        x[i] = pop(s);
                    }
                    Poly r = PolyCompose(&p, count, x);
                    push(s, r);
                    PolyDestroy(&r);
                    PolyDestroy(&p);
                    for (unsigned i = 0; i < count; i++) {
                        PolyDestroy(&x[i]);
                    }
                }
                else {
                    underflow(row);
                }
            }
        }
        else  {
            ungetc('\n', stdin);
            for (int j = i - 1; j > 7; j--) {
                ungetc(word[j], stdin);
            }
            intBool w = parseUnsigned(row, "COUNT");
            if (w.b) {
                underflow(row);
            }
        }
    }
    else if (i > 6 && word[0] == 'C' && word[1] == 'O' && word[2] == 'M' && word[3] == 'P' && word[4] == 'O'
             && word[5] == 'S' && word[6] == 'E') {
        fprintf(stderr, "ERROR %u WRONG COUNT\n", row);
    }

    else {
        fprintf(stderr, "ERROR %u WRONG COMMAND\n", row);
    }
    free(word);
}

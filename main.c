#include <stdio.h>
#include "stack.h"
#include "poly.h"
#include "parser.h"
#include "utils.h"

/*
 *********************************************************************
 * uzylem pliku utils.h znajdujacego sie w katalogu cmocka_example/3 *
 * z zajec 11. przedmiotu IPP 2017                                   *
 *********************************************************************
 */

int main(void) {

    stack* s = newStack();
    unsigned r = 1;
    char c;
    while((c = getchar()) != EOF) {
        ungetc(c, stdin);

        if (c == '(' || c == '-' || (intFromChar(c) >= 0 && intFromChar(c) <= 9)) {
            Polybool p = parsePoly(s, r, 1, ',');
            push(s, p.p);
            PolyDestroy(&p.p);
        }

        else if (c >= 'A' && c <= 'z') {
            parseCommand(s, r);
        }
        else {
            fprintf(stderr, "ERROR %d %d\n", r, 1);
            while(getchar()!='\n');
        }
         ++r;
    }
    deleteStack(s);
    return 0;
}

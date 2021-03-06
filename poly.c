/** @file
   Implementacja klasy wielomianÃ³w

   @author Jakub Pawlewicz <pan@mimuw.edu.pl>
   @copyright Uniwersytet Warszawski
   @date 2017-03-04
*/

#include "poly.h"
#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/** Maksymalna wartoÅ›Ä‡ wykÅ‚adnika wielomianu */
#define POLY_EXP_MAX INT_MAX

/**
 * Zwraca liczbÄ™ jednomianÃ³w w wielomianie normalnym.
 * @param[in] p : wielomian normalny
 * @return liczba jednomianÃ³w
 */
static inline unsigned PolyLen(const Poly *p)
{
    return p->size;
}

void Print (Poly* p) {
    if (PolyIsCoeff(p)) {
        printf("%ld", p->coeff);
    }
    else {
        for (unsigned i = 0; i < p->size - 1; i++) {
            printf("(");
            Print(&p->arr[i].p);
            printf(",");
            printf("%d", p->arr[i].exp);
            printf(")+");
        }
        printf("(");
        Print(&p->arr[p->size - 1].p);
        printf(",");
        printf("%d", p->arr[p->size - 1].exp);
        printf(")");
    }
}

/**
 * Ustala rozmiar i przydziela pamiÄ™Ä‡ na @p size jednomianÃ³w.
 * @param[in] size : liczba jednomianÃ³w
 * @return zaalokowany wielomian
 */
static Poly PolyAlloc(unsigned size)
{
    assert(size > 0);
    Mono *arr = calloc(size, sizeof(Mono));
    assert(arr != NULL);
    return (Poly) {.size = size, .arr = arr};
}

/**
 * Optymalizuje rozmiar przydzielonej pamiÄ™ci wielomianowi.
 * @param[in,out] p : optymalizowany wielomian
 */
static void PolyRealloc(Poly *p)
{
    assert(!PolyIsCoeff(p));
    if (PolyLen(p) == 0)
    {
        PolyDestroy(p);
        p->coeff = 0;
    }
    else if (PolyLen(p) == 1 && p->arr[0].exp == 0 && PolyIsCoeff(&p->arr[0].p))
    {
        p->coeff = p->arr[0].p.coeff;
        free(p->arr);
        p->arr = NULL;
    }
    else
    {
        p->arr = realloc(p->arr, PolyLen(p) * sizeof(Mono));
        assert(p->arr != NULL);
    }
}

void PolyDestroy(Poly *p)
{
    if (PolyIsCoeff(p))
        return;
    for (unsigned i = 0; i < PolyLen(p); ++i)
        MonoDestroy(&p->arr[i]);
    free(p->arr);
    p->arr = NULL;
}

Poly PolyClone(const Poly *p)
{
    if (PolyIsCoeff(p))
        return *p;
    Poly r = PolyAlloc(p->size);
    for (unsigned i = 0; i < PolyLen(p); ++i)
        r.arr[i] = MonoClone(&p->arr[i]);
    return r;
}

static Poly PolyAddCoeff(const Poly *p, poly_coeff_t c);

/**
 * Dodaje wielomian normalny i wspÃ³Å‚czynnik.
 * @param[in] p : wielomian normalny
 * @param[in] c : wspÃ³Å‚czynnik
 * @return
 */
static Poly PolyAddPolyCoeff(const Poly *p, poly_coeff_t c)
{
    assert(PolyLen(p) > 0);
    if (c == 0)
        return PolyClone(p);
    else if (p->arr[0].exp == 0)
    {
        Poly r0 = PolyAddCoeff(&p->arr[0].p, c);
        if (PolyIsZero(&r0))
        {
            assert(PolyLen(p) > 1);
            Poly r = PolyAlloc(PolyLen(p) - 1);
            for (unsigned i = 0; i < PolyLen(&r); ++i)
                r.arr[i] = MonoClone(&p->arr[i + 1]);
            return r;
        }
        else if (PolyIsCoeff(&r0) && PolyLen(p) == 1)
        {
            return r0;
        }
        else
        {
            Poly r = PolyAlloc(PolyLen(p));
            r.arr[0] = MonoFromPoly(&r0, 0);
            for (unsigned i = 1; i < PolyLen(p); ++i)
                r.arr[i] = MonoClone(&p->arr[i]);
            return r;
        }
    }
    else
    {
        Poly r = PolyAlloc(PolyLen(p) + 1);
        r.arr[0] = (Mono) {.p = PolyFromCoeff(c), .exp = 0};
        for (unsigned i = 0; i < PolyLen(p); ++i)
            r.arr[i + 1] = MonoClone(&p->arr[i]);
        return r;
    }
}

/**
 * Dodaje do wielomianu wspÃ³Å‚czynnik.
 * @param[in] p : wielomian
 * @param[in] c : wspÃ³Å‚czynnik
 * @return `p + c`
 */
static Poly PolyAddCoeff(const Poly *p, poly_coeff_t c)
{
    if (PolyIsCoeff(p))
        return PolyFromCoeff(p->coeff + c);
    else
        return PolyAddPolyCoeff(p, c);
}

/**
 * Dodaje dwa normalne wielomiany.
 * @param[in] p : wielomian normalny
 * @param[in] q : wielomian normalny
 * @return
 */
static Poly PolyAddPolyPoly(const Poly *p, const Poly *q)
{
    assert(PolyLen(p) > 0);
    assert(PolyLen(q) > 0);
    Poly r = PolyAlloc(PolyLen(p) + PolyLen(q));
    unsigned i = 0, j = 0, k = 0;
    while (i < PolyLen(p) || j < PolyLen(q))
    {
        poly_exp_t pe = i < PolyLen(p) ? p->arr[i].exp : POLY_EXP_MAX;
        poly_exp_t qe = j < PolyLen(q) ? q->arr[j].exp : POLY_EXP_MAX;
        if (pe < qe)
        {
            assert(!PolyIsZero(&p->arr[i].p));
            r.arr[k++] = MonoClone(&p->arr[i++]);
        }
        else if (pe > qe)
        {
            assert(!PolyIsZero(&q->arr[j].p));
            r.arr[k++] = MonoClone(&q->arr[j++]);
        }
        else
        {
            Mono m = (Mono) {.p = PolyAdd(&p->arr[i++].p, &q->arr[j++].p),
                             .exp = pe};
            if (!PolyIsZero(&m.p))
                r.arr[k++] = m;
        }
    }
    r.size = k;
    PolyRealloc(&r);
    return r;
}

Poly PolyAdd(const Poly *p, const Poly *q)
{
    if (PolyIsCoeff(p))
        return PolyAddCoeff(q, p->coeff);
    else if (PolyIsCoeff(q))
        return PolyAddCoeff(p, q->coeff);
    else
        return PolyAddPolyPoly(p, q);
}

/**
 * Pomocnicza funkcja do sortowania jednomianÃ³w.
 * @param a : wskaÅºnik do jednomianu
 * @param b : wskaÅºnik do jednomianu
 * @return wynik porÃ³wnania wykÅ‚adnikÃ³w jednomianÃ³w
 */
static poly_exp_t MonoCmp(const void *a, const void *b)
{
    return ((const Mono *) a)->exp - ((const Mono *) b)->exp;
}

Poly PolyAddMonos(unsigned count, const Mono monos[])
{
   // printf("PolyAdMOnos %d \n", count);
    Mono *arr = calloc(count, sizeof(Mono));
    assert(arr != NULL);
    memcpy(arr, monos, count * sizeof(Mono));
    qsort(arr, count, sizeof(Mono), MonoCmp);
    unsigned k = 0;
    for (unsigned i = 0; i < count; ++i)
    {
        if (k == 0 || arr[i].exp != arr[k - 1].exp)
        {
            //assert(!PolyIsZero(&arr[i].p));
            arr[k++] = arr[i];
        }
        else
        {
            Mono m = {PolyAdd(&arr[k - 1].p, &arr[i].p), arr[i].exp};
            PolyDestroy(&arr[k - 1].p);
            PolyDestroy(&arr[i].p);
            if (PolyIsZero(&m.p))
                k--;
            else
                arr[k - 1] = m;
        }
    }
    Poly r = (Poly) {.size = k, .arr = arr};
    PolyRealloc(&r);
    return r;
}

/**
 * MnoÅ¼y dwa wielomiany normalne.
 * @param[in] p : wielomian normalny
 * @param[in] q : wielomian normalny
 * @return `p * q`
 */
static Poly PolyMulPolyPoly(const Poly *p, const Poly *q)
{
    unsigned count = PolyLen(p) * PolyLen(q);
    Mono  * arr = calloc(count, sizeof(struct Mono));
    unsigned k = 0;
    for (unsigned i = 0; i < PolyLen(p); ++i)
        for (unsigned j = 0; j < PolyLen(q); ++j)
            arr[k++] = (Mono) {.p = PolyMul(&p->arr[i].p, &q->arr[j].p),
                               .exp = p->arr[i].exp + q->arr[j].exp};
    Poly res = PolyAddMonos(count, arr);
    free(arr);
    return res;
}

static Poly PolyMulCoeff(const Poly *p, poly_coeff_t c);

/**
 * MnoÅ¼y wielomian normalny przez wspÃ³Å‚czynnik.
 * @param[in] p : wielomian normalny
 * @param[in] c : wspÃ³Å‚czynnik
 * @return `p * c`
 */
static Poly PolyMulPolyCoeff(const Poly *p, poly_coeff_t c)
{
    if (c == 0)
        return PolyZero();
    Poly r = PolyAlloc(PolyLen(p));
    r.size = 0;
    for (unsigned i = 0; i < PolyLen(p); ++i)
    {
        Mono m = (Mono) {.p = PolyMulCoeff(&p->arr[i].p, c),
                         .exp = p->arr[i].exp};
        if (!PolyIsZero(&m.p))
            r.arr[r.size++] = m;
    }
    PolyRealloc(&r);
    return r;
}

/**
 * MnoÅ¼y wielomian przez wspÃ³Å‚czynnik.
 * @param[in] p : wielomian
 * @param[in] c : wspÃ³Å‚czynnik
 * @return `p * c`
 */
static Poly PolyMulCoeff(const Poly *p, poly_coeff_t c)
{
    if (PolyIsCoeff(p))
        return PolyFromCoeff(p->coeff * c);
    else
        return PolyMulPolyCoeff(p, c);
}

Poly PolyMul(const Poly *p, const Poly *q)
{
    if (PolyIsCoeff(p))
        return PolyMulCoeff(q, p->coeff);
    else if (PolyIsCoeff(q))
        return PolyMulCoeff(p, q->coeff);
    else
        return PolyMulPolyPoly(p, q);
}

Poly PolyNeg(const Poly *p)
{
    return PolyMulCoeff(p, -1);
}

Poly PolySub(const Poly *p, const Poly *q)
{
    Poly neg_q = PolyNeg(q);
    Poly res = PolyAdd(p, &neg_q);
    PolyDestroy(&neg_q);
    return res;
}

poly_exp_t PolyDegBy(const Poly *p, unsigned var_idx)
{
    if (PolyIsCoeff(p))
    {
        return PolyIsZero(p) ? -1 : 0;
    }
    else if (var_idx == 0)
    {
        return p->arr[PolyLen(p) - 1].exp;
    }
    else
    {
        poly_exp_t deg = -1;
        for (unsigned i = 0; i < PolyLen(p); ++i)
        {
            poly_exp_t d = PolyDegBy(&p->arr[i].p, var_idx - 1);
            if (d > deg)
                deg = d;
        }
        return deg;
    }
}

poly_exp_t PolyDeg(const Poly *p)
{
    if (PolyIsCoeff(p))
    {
        return PolyIsZero(p) ? -1 : 0;
    }
    else
    {
        poly_exp_t deg = -1;
        for (unsigned i = 0; i < PolyLen(p); ++i)
        {
            poly_exp_t d = PolyDeg(&p->arr[i].p) + p->arr[i].exp;
            if (d > deg)
                deg = d;
        }
        return deg;
    }
}

bool PolyIsEq(const Poly *p, const Poly *q)
{
    if (PolyIsCoeff(p) != PolyIsCoeff(q))
    {
        return false;
    }
    else if (PolyIsCoeff(p))
    {
        return p->coeff == q->coeff;
    }
    else if (PolyLen(p) != PolyLen(q))
    {
        return false;
    }
    else
    {
        for (unsigned i = 0; i < PolyLen(p); ++i)
            if (p->arr[i].exp != q->arr[i].exp ||
                !PolyIsEq(&p->arr[i].p, &q->arr[i].p))
                return false;
        return true;
    }
}

/**
 * Wylicza @f$ab^n@f$.
 * @param[in] a - mnoÅ¼nik
 * @param[in] b - podstawa
 * @param[in] n - wykÅ‚adnik
 * @return @f$ab^n@f$
 */
static poly_coeff_t PowHelp(poly_coeff_t a, poly_coeff_t b, poly_exp_t n)
{
    if (n == 0)
        return a;
    else
        return PowHelp(n % 2 == 0 ? a : a * b, b * b, n / 2);
}

/**
 * Wylicza @f$a^n@f$.
 * @param[in] a - podstawa
 * @param[in] n - wykÅ‚adnik
 * @return @f$a^n@f$
 */
static inline poly_coeff_t Pow(poly_coeff_t a, poly_exp_t n)
{
    return PowHelp(1, a, n);
}

Poly PolyAt(const Poly *p, poly_coeff_t x)
{
    if (PolyIsCoeff(p))
        return PolyClone(p);
    Poly r = PolyZero();
    poly_exp_t n = 0;
    poly_coeff_t x_to_n = 1;
    for (unsigned i = 0; i < PolyLen(p); ++i)
    {
        x_to_n *= Pow(x, p->arr[i].exp - n);
        n = p->arr[i].exp;
        Poly pi = PolyMulCoeff(&p->arr[i].p, x_to_n);
        Poly q = PolyAdd(&r, &pi);
        PolyDestroy(&pi);
        PolyDestroy(&r);
        r = q;
    }
    return r;
}

/**
 * @brief PolyPow
 * Zwraca exp-tą potęgę wielomianu p;
 * @param[in] p
 * @param[in] help
 * @param[in] exp
 * @return @f$ p^{exp} @f$
 */
Poly PolyPow(Poly p, Poly p0, poly_exp_t exp) {
    while(exp > 0) {
        Poly r = PolyMul(&p, &p0);
        PolyDestroy(&p);
        p = r;
        exp --;
    }
    return p;
}
/**
 * @brief PolyCompose
 * Składa wielomiany.
 * @param[in] p
 * @param[in] count
 * @param[in] x[count]
 * @return @f$ p(x[0], x[1], ..., x[count - 1]) @f$
 */
Poly PolyCompose(const Poly *p, unsigned count, const Poly x[]) {
    if (PolyIsCoeff(p)) {
        return *p;
    }
    if (count == 0) {
        return PolyAt(p, 0);
    }
    Poly* tab = malloc(p->size * sizeof(Poly));
    for (unsigned i = 0; i < p->size; i++) {
        Poly q = PolyCompose(&p->arr[i].p, (count > 1) ? count - 1 : 0, x + (count > 1));
        if (PolyIsZero(&q)) {
            tab[i] = PolyZero();
        }
        else {
            Poly pCompose = PolyPow(PolyFromCoeff(1), x[0], p->arr[i].exp);
            tab[i] = PolyMul(&q, &pCompose);
            PolyDestroy(&pCompose);
        }
        PolyDestroy(&q);
    }
    Poly res = tab[0];
    for (unsigned i = 1; i < p->size; i++) {
        Poly q = PolyAdd(&res, &tab[i]);
        PolyDestroy(&res);
        res = q;
        PolyDestroy(&tab[i]);
    }
    free(tab);
    return res;
}

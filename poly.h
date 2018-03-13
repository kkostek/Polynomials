/** @file
   Interfejs klasy wielomianÃ³w

   @author Jakub Pawlewicz <pan@mimuw.edu.pl>
   @copyright Uniwersytet Warszawski
   @date 2017-03-04
*/

#ifndef __POLY_H__
#define __POLY_H__

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

/** Typ wspÃ³Å‚czynnikÃ³w wielomianu */
typedef long poly_coeff_t;

/** Typ wykÅ‚adnikÃ³w wielomianu */
typedef int poly_exp_t;

struct Mono;

/**
 * Struktura przechowujÄ…ca wielomian
 * Wielomian to albo wspÃ³Å‚czynnik (wtedy `arr == NULL`),
 * albo lista jednomianÃ³w.
 */
typedef struct Poly
{
    /**
     * WspÃ³Å‚czynnik lub dÅ‚ugoÅ›Ä‡ wielomianu w liczbie jednomianÃ³w
     * JeÅ¼eli `arr == NULL`, wtedy jest to wspÃ³Å‚czynnik bÄ™dÄ…cy staÅ‚Ä….
     * W p.p. jest to lista jednomianÃ³w i taki wielomian nazywamy _normalnym_.
     */
    union {
         poly_coeff_t coeff;
         unsigned     size;
    };
    /** Tablica przechowujÄ…ca listÄ™ jednomianÃ³w */
    struct Mono *arr;
} Poly;

/**
  * Struktura przechowujÄ…ca jednomian
  * Jednomian ma postaÄ‡ `p * x^e`.
  * WspÃ³Å‚czynnik `p` moÅ¼e teÅ¼ byÄ‡ wielomianem.
  * BÄ™dzie on traktowany jako wielomian nad kolejnÄ… zmiennÄ… (nie nad x).
  */
typedef struct Mono
{
    Poly p; ///< wspÃ³Å‚czynnik
    poly_exp_t exp; ///< wykÅ‚adnik
} Mono;

/**
 * Tworzy wielomian, ktÃ³ry jest wspÃ³Å‚czynnikiem.
 * @param[in] c : wartoÅ›Ä‡ wspÃ³Å‚czynnika
 * @return wielomian
 */
static inline Poly PolyFromCoeff(poly_coeff_t c)
{
    return (Poly) {.coeff = c, .arr = NULL};
}

/**
 * Tworzy wielomian toÅ¼samoÅ›ciowo rÃ³wny zeru.
 * @return wielomian
 */
static inline Poly PolyZero()
{
    return PolyFromCoeff(0);
}

/**
 * @brief Print
 * Wypisuje na standardowe wyjście wielomian p w przyjętym standardzie zapisu
 * @param[in] p : wielomian
 * @printto[stdout] `p`
 */
void Print(Poly *p);

static inline bool PolyIsZero(const Poly *p);

/**
 * Tworzy jednomian `p * x^e`.
 * Przejmuje na wÅ‚asnoÅ›Ä‡ zawartoÅ›Ä‡ struktury wskazywanej przez @p p.
 * @param[in] p : wielomian - wspÃ³Å‚czynnik jednomianu
 * @param[in] e : wykÅ‚adnik
 * @return jednomian `p * x^e`
 */
static inline Mono MonoFromPoly(const Poly *p, poly_exp_t e)
{
    assert(e == 0 || !PolyIsZero(p));
    return (Mono) {.p = *p, .exp = e};
}

/**
 * Sprawdza, czy wielomian jest wspÃ³Å‚czynnikiem.
 * @param[in] p : wielomian
 * @return Czy wielomian jest wspÃ³Å‚czynnikiem?
 */
static inline bool PolyIsCoeff(const Poly *p)
{
    return p->arr == NULL;
}

/**
 * Sprawdza, czy wielomian jest toÅ¼samoÅ›ciowo rÃ³wny zeru.
 * @param[in] p : wielomian
 * @return Czy wielomian jest rÃ³wny zero?
 */
static inline bool PolyIsZero(const Poly *p)
{
    return PolyIsCoeff(p) && p->coeff == 0;
}

/**
 * Usuwa wielomian z pamiÄ™ci.
 * @param[in] p : wielomian
 */
void PolyDestroy(Poly *p);

/**
 * Usuwa jednomian z pamiÄ™ci.
 * @param[in] m : jednomian
 */
static inline void MonoDestroy(Mono *m)
{
    PolyDestroy(&m->p);
}

/**
 * Robi peÅ‚nÄ…, gÅ‚Ä™bokÄ… kopiÄ™ wielomianu.
 * @param[in] p : wielomian
 * @return skopiowany wielomian
 */
Poly PolyClone(const Poly *p);

/**
 * Robi peÅ‚nÄ…, gÅ‚Ä™bokÄ… kopiÄ™ jednomianu.
 * @param[in] m : jednomian
 * @return skopiowany jednomian
 */
static inline Mono MonoClone(const Mono *m)
{
    return (Mono) {.p = PolyClone(&m->p), .exp = m->exp};
}

/**
 * Dodaje dwa wielomiany.
 * @param[in] p : wielomian
 * @param[in] q : wielomian
 * @return `p + q`
 */
Poly PolyAdd(const Poly *p, const Poly *q);

/**
 * Sumuje listÄ™ jednomianÃ³w i tworzy z nich wielomian.
 * Przejmuje na wÅ‚asnoÅ›Ä‡ zawartoÅ›Ä‡ tablicy @p monos.
 * @param[in] count : liczba jednomianÃ³w
 * @param[in] monos : tablica jednomianÃ³w
 * @return wielomian bÄ™dÄ…cy sumÄ… jednomianÃ³w
 */
Poly PolyAddMonos(unsigned count, const Mono monos[]);

/**
 * MnoÅ¼y dwa wielomiany.
 * @param[in] p : wielomian
 * @param[in] q : wielomian
 * @return `p * q`
 */
Poly PolyMul(const Poly *p, const Poly *q);

/**
 * Zwraca przeciwny wielomian.
 * @param[in] p : wielomian
 * @return `-p`
 */
Poly PolyNeg(const Poly *p);

/**
 * Odejmuje wielomian od wielomianu.
 * @param[in] p : wielomian
 * @param[in] q : wielomian
 * @return `p - q`
 */
Poly PolySub(const Poly *p, const Poly *q);

/**
 * Zwraca stopieÅ„ wielomianu ze wzglÄ™du na zadanÄ… zmiennÄ… (-1 dla wielomianu
 * toÅ¼samoÅ›ciowo rÃ³wnego zeru).
 * Zmienne indeksowane sÄ… od 0.
 * Zmienna o indeksie 0 oznacza zmiennÄ… gÅ‚Ã³wnÄ… tego wielomianu.
 * WiÄ™ksze indeksy oznaczajÄ… zmienne wielomianÃ³w znajdujÄ…cych siÄ™
 * we wspÃ³Å‚czynnikach.
 * @param[in] p : wielomian
 * @param[in] var_idx : indeks zmiennej
 * @return stopieÅ„ wielomianu @p p z wzglÄ™du na zmiennÄ… o indeksie @p var_idx
 */
poly_exp_t PolyDegBy(const Poly *p, unsigned var_idx);

/**
 * Zwraca stopieÅ„ wielomianu (-1 dla wielomianu toÅ¼samoÅ›ciowo rÃ³wnego zeru).
 * @param[in] p : wielomian
 * @return stopieÅ„ wielomianu @p p
 */
poly_exp_t PolyDeg(const Poly *p);

/**
 * Sprawdza rÃ³wnoÅ›Ä‡ dwÃ³ch wielomianÃ³w.
 * @param[in] p : wielomian
 * @param[in] q : wielomian
 * @return `p = q`
 */
bool PolyIsEq(const Poly *p, const Poly *q);

/**
 * Wylicza wartoÅ›Ä‡ wielomianu w punkcie @p x.
 * Wstawia pod pierwszÄ… zmiennÄ… wielomianu wartoÅ›Ä‡ @p x.
 * W wyniku moÅ¼e powstaÄ‡ wielomian, jeÅ›li wspÃ³Å‚czynniki sÄ… wielomianem
 * i zmniejszane sÄ… indeksy zmiennych w takim wielomianie o jeden.
 * Formalnie dla wielomianu @f$p(x_0, x_1, x_2, \ldots)@f$ wynikiem jest
 * wielomian @f$p(x, x_0, x_1, \ldots)@f$.
 * @param[in] p
 * @param[in] x
 * @return @f$p(x, x_0, x_1, \ldots)@f$
 */
Poly PolyAt(const Poly *p, poly_coeff_t x);

/**
 * @brief PolyCompose
 * Składa wielomiany. Dokładniej, jeśli p jest wielomianem @f$p(x_0, x_1, ..., x_k)@f$, to
 * funkcja zwraca @f$p(x[0], x[1], .., x[count - 1], 0, 0 \ldots, 0)@f$.
 * @param[in] p
 * @param[in] count
 * @param[in] x
 * @return @f$p(x[0], x[1], .., x[count - 1], 0, 0 \ldots, 0)@f$
 */
Poly PolyCompose(const Poly *p, unsigned count, const Poly x[]);

#endif /* __POLY_H__ */


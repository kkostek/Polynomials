#include <stdio.h>
#include <stdarg.h>
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>
#include "cmocka.h"
#include "poly.h"



extern int calc_poly_main(void);

/*
 *********************************************************************************
 *   Użyłem niżej wymienionych funkcji (w tym atrap) z pliku calculator_test.c   *
 *          z katalogu cmocka_example/3 z zajęć 11. przedmiotu IPP 2017          *
 *********************************************************************************
 */

int mock_fprintf(FILE* const file, const char *format, ...) CMOCKA_PRINTF_ATTRIBUTE(2, 3);
int mock_printf(const char *format, ...) CMOCKA_PRINTF_ATTRIBUTE(1, 2);

/**
 * Pomocnicze bufory, do których piszą atrapy funkcji printf i fprintf oraz
 * pozycje zapisu w tych buforach. Pozycja zapisu wskazuje bajt o wartości 0.
 */
static char fprintf_buffer[256];
static char printf_buffer[256];
static int fprintf_position = 0;
static int printf_position = 0;

/**
 * Atrapa funkcji fprintf sprawdzająca poprawność wypisywania na stderr.
 */
int mock_fprintf(FILE* const file, const char *format, ...) {
    int return_value;
    va_list args;
    assert_true(file == stderr);
    /* Poniższa asercja sprawdza też, czy fprintf_position jest nieujemne.
    W buforze musi zmieścić się kończący bajt o wartości 0. */
    assert_true((size_t)fprintf_position < sizeof(fprintf_buffer));

    va_start(args, format);
    return_value = vsnprintf(fprintf_buffer + fprintf_position,
                             sizeof(fprintf_buffer) - fprintf_position,
                             format,
                             args);
    va_end(args);

    fprintf_position += return_value;
    assert_true((size_t)fprintf_position < sizeof(fprintf_buffer));
    return return_value;
}

/**
 * Atrapa funkcji printf sprawdzająca poprawność wypisywania na stdout.
 */
int mock_printf(const char *format, ...) {
    int return_value;
    va_list args;
    /* Poniższa asercja sprawdza też, czy printf_position jest nieujemne.
    W buforze musi zmieścić się kończący bajt o wartości 0. */
    assert_true((size_t)printf_position < sizeof(printf_buffer));

    va_start(args, format);
    return_value = vsnprintf(printf_buffer + printf_position,
                             sizeof(printf_buffer) - printf_position,
                             format,
                             args);
    va_end(args);

    printf_position += return_value;
    assert_true((size_t)printf_position < sizeof(printf_buffer));
    return return_value;
}

/**
 *  Pomocniczy bufor, z którego korzystają atrapy funkcji operujących na stdin.
 */
static char input_stream_buffer[256];
static int input_stream_position = 0;
static int input_stream_end = 0;
int read_char_count;

/**
 * Atrapa funkcji scanf używana do przechwycenia czytania z stdin.
 */
int mock_scanf(const char *format, ...) {
    va_list fmt_args;
    int ret;
    va_start(fmt_args, format);
    ret = vsscanf(input_stream_buffer + input_stream_position, format, fmt_args);
    va_end(fmt_args);

    if (ret < 0) { /* ret == EOF */
        input_stream_position = input_stream_end;
    }
    else {
        assert_true(read_char_count >= 0);
        input_stream_position += read_char_count;
        if (input_stream_position > input_stream_end) {
            input_stream_position = input_stream_end;
        }
    }
    return ret;
}

/**
 * Atrapa funkcji getchar używana do przechwycenia czytania z stdin.
 */
int mock_getchar() {
    if (input_stream_position < input_stream_end) {
        return input_stream_buffer[input_stream_position++];
    }
    else {
        return EOF;
    }
}

/**
 * Atrapa funkcji ungetc.
 * Obsługiwane jest tylko standardowe wejście.
 */
int mock_ungetc(int c, FILE *stream) {
    assert_true(stream == stdin);
    if (input_stream_position > 0)
        return input_stream_buffer[--input_stream_position] = c;
    else
        return EOF;
}

/**
 * Funkcja wołana przed każdym testem korzystającym z stdout lub stderr.
 */
static int test_setup(void **state) {
    (void)state;

    memset(fprintf_buffer, 0, sizeof(fprintf_buffer));
    memset(printf_buffer, 0, sizeof(printf_buffer));
    printf_position = 0;
    fprintf_position = 0;

    /* Zwrócenie zera oznacza sukces. */
    return 0;
}

/**
 * Funkcja inicjująca dane wejściowe dla programu korzystającego ze stdin.
 */
static void init_input_stream(const char *str) {
    memset(input_stream_buffer, 0, sizeof(input_stream_buffer));
    input_stream_position = 0;
    input_stream_end = strlen(str);
    assert_true((size_t)input_stream_end < sizeof(input_stream_buffer));
    strcpy(input_stream_buffer, str);
}


/*
 ***************************************************
 *         Koniec funkcji z tamtegoż pliku         *
 ***************************************************
 */




/*
 **********************************************
 ***          PIERWSZA GRUPA TESTÓW         ***
 **********************************************
 */

/*
 * Test zerowy.
 * p = PolyZero(), count = 0;
 */
static void test1_0(void **state) {
    (void)state;
    Poly x[3];
    Poly p = PolyZero();
    Poly q = PolyCompose(&p, 0, x);
    assert_true(PolyIsEq(&q, &p));
}

/*
 * Test pierwszy.
 * p = PolyZero(), count = 1, x[0] - wielomian stały;
 */
static void test1_1(void **state) {
    (void)state;
    Poly x[2];
    x[0] = PolyFromCoeff(42);
    Poly p = PolyZero();
    Poly q = PolyCompose(&p, 1, x);
    assert_true(PolyIsEq(&q, &p));
}

/*
 * Test drugi.
 * p - wielomian stały, count = 0;
 */
static void test1_2(void **state) {
    (void)state;
    Poly x[3];
    Poly p = PolyFromCoeff(42);
    Poly q = PolyCompose(&p, 0, x);
    assert_true(PolyIsEq(&q, &p));
}

/*
 * Test trzeci.
 * p - wielomian stały, count = 1, x[0] - wielomian stały różny od p;
 */
static void test1_3(void **state) {
    (void)state;
    Poly x[2];
    Poly p = PolyFromCoeff(42);
    x[0] = PolyFromCoeff(31);
    x[1] = PolyFromCoeff(53);
    Poly q = PolyCompose(&p, 1, x);
    assert_true(PolyIsEq(&q, &p));
}

/*
 * Test czwarty.
 * p = x_0, count = 0;
 */
static void test1_4(void **state) {
    (void)state;
    Poly x[2];
    x[0] = PolyFromCoeff(1);
    Mono m[1];
    m[0] = MonoFromPoly(&x[0], 1);
    Poly p;
    p.coeff = 0;
    p.size = 1;
    p.arr = m;
    Poly q = PolyCompose(&p, 0, x);
    Poly r = PolyZero();
    assert_true(PolyIsEq(&q, &r));
}

/*
 * Test piąty.
 * p = x_0, count = 1, x[0] - wielomian stały.
 */
static void test1_5(void **state) {
    (void)state;
    Poly x[2];
    x[0] = PolyFromCoeff(1);
    Mono m[1];
    m[0] = MonoFromPoly(&x[0], 1);
    Poly p;
    p.size = 1;
    p.arr = m;
    x[0] = PolyFromCoeff(42);
    Poly q = PolyCompose(&p, 1, x);
    Poly r = PolyFromCoeff(42);
    assert_true(PolyIsEq(&q, &r));
}

/*
 * Test szósty.
 * p = x_0, count = 1, x[0] = x_0;
 */
static void test1_6(void **state) {
    (void)state;
    Poly x[2];
    x[0] = PolyFromCoeff(1);
    Mono m[1];
    m[0] = MonoFromPoly(&x[0], 1);
    Poly p;
    p.coeff = 0;
    p.size = 1;
    p.arr = m;
    x[0] = PolyClone(&p);
    Poly q = PolyCompose(&p, 1, x);
    PolyDestroy(&x[0]);
    assert_true(PolyIsEq(&q, &p));
    PolyDestroy(&q);
}

/*
 *******************************************
 ***          DRUGA GRUPA TESTÓW         ***
 *******************************************
 */

/*
 * Test zerowy.
 * COMPOSE bez argumentu;
 */
static void test2_0(void **state) {
    test_setup(state);
    init_input_stream("COMPOSE\n");
    assert_int_equal(calc_poly_main(), 0);
    assert_string_equal(printf_buffer, "");
    assert_string_equal(fprintf_buffer, "ERROR 1 WRONG COUNT\n");
}

/*
 * Test pierwszy.
 * COMPOSE 0;
 */
static void test2_1(void **state) {
    test_setup(state);
    init_input_stream("(3,0)+(2,1)\nCOMPOSE 0\n");
    assert_int_equal(calc_poly_main(), 0);
    assert_string_equal(printf_buffer, "");
    assert_string_equal(fprintf_buffer, "");
}

/*
 * Test drugi.
 * COMPOSE max_unsigned;
 */
static void test2_2(void **state) {
    test_setup(state);
    init_input_stream("COMPOSE 4294967295\n");
    assert_int_equal(calc_poly_main(), 0);
    assert_string_equal(printf_buffer, "");
    assert_string_equal(fprintf_buffer, "ERROR 1 STACK UNDERFLOW\n");
}

/*
 * Test trzeci.
 * COMPOSE -1;
 */
static void test2_3(void **state) {
    test_setup(state);
    init_input_stream("COMPOSE -1\n");
    assert_int_equal(calc_poly_main(), 0);
    assert_string_equal(printf_buffer, "");
    assert_string_equal(fprintf_buffer, "ERROR 1 WRONG COUNT\n");
}

/*
 * Test czwarty.
 * COMPOSE max_unsigned + 1;
 */
static void test2_4(void **state) {
    test_setup(state);
    init_input_stream("COMPOSE 4294967296\n");
    assert_int_equal(calc_poly_main(), 0);
    assert_string_equal(printf_buffer, "");
    assert_string_equal(fprintf_buffer, "ERROR 1 WRONG COUNT\n");
}

/*
 * Test piąty.
 * COMPOSE (coś duuużo większego od max_unsigned);
 */
static void test2_5(void **state) {
    test_setup(state);
    init_input_stream("COMPOSE 424242424294967295\n");
    assert_int_equal(calc_poly_main(), 0);
    assert_string_equal(printf_buffer, "");
    assert_string_equal(fprintf_buffer, "ERROR 1 WRONG COUNT\n");
}

/*
 * Test szósty.
 * COMPOSE (kombinacja liter);
 */
static void test2_6(void **state) {
    test_setup(state);
    init_input_stream("COMPOSE xd\n");
    assert_int_equal(calc_poly_main(), 0);
    assert_string_equal(printf_buffer, "");
    assert_string_equal(fprintf_buffer, "ERROR 1 WRONG COUNT\n");
}


/*
 * Test siódmy.
 * COMPOSE (kombinacja liter i cyfr, zaczyna się cyfrą);
 */
static void test2_7(void **state) {
    test_setup(state);
    init_input_stream("COMPOSE 42xd42lol\n");
    assert_int_equal(calc_poly_main(), 0);
    assert_string_equal(printf_buffer, "");
    assert_string_equal(fprintf_buffer, "ERROR 1 WRONG COUNT\n");
}


int main(void) {

    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test1_0),
        cmocka_unit_test(test1_1),
        cmocka_unit_test(test1_2),
        cmocka_unit_test(test1_3),
        cmocka_unit_test(test1_4),
        cmocka_unit_test(test1_5),
        cmocka_unit_test(test1_6)
    };

    const struct CMUnitTest tests2[] = {
        cmocka_unit_test(test2_0),
        cmocka_unit_test(test2_1),
        cmocka_unit_test(test2_2),
        cmocka_unit_test(test2_3),
        cmocka_unit_test(test2_4),
        cmocka_unit_test(test2_5),
        cmocka_unit_test(test2_6),
        cmocka_unit_test(test2_7)
    };

    return cmocka_run_group_tests(tests, NULL, NULL) + cmocka_run_group_tests(tests2, NULL, NULL);
}

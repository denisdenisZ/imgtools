#include <math.h>

#include "tests.h"
#include "assert.h"
#include "img_math.h"
#include "fxdpoint.h"

void test_pow2_u32() {
    TEST_START("POW2 U32");
    assert(pow2_u32(0) == 1);
    assert(pow2_u32(1) == 2);
    assert(pow2_u32(8) == 256);
    assert(pow2_u32(31) == 2147483648U);
}

void test_get_mrv() {
    TEST_START("GET MRV");
    assert(get_mrv(1) == 1);
    assert(get_mrv(8) == 255);
    assert(get_mrv(16) == 65535);
    assert(get_mrv(32) == 4294967295U);
}

void test_best_fixed_u32() {
    TEST_START("BEST FIXED U32");
    uint32_t fac, shift;

    // invalid inputs
    assert(best_fixed_u32(0.0, 32, &fac, &shift) == MATH_ERROR_INVALID);
    assert(best_fixed_u32(-1.0, 32, &fac, &shift) == MATH_ERROR_INVALID);

    // exact power of 2
    assert(best_fixed_u32(4.0, 32, &fac, &shift) == MATH_SUCCESS);
    double out;
    from_fixed_to_double(fac, shift, &out);
    assert(fabs(out - 4.0) < 1e-6);

    // fractional
    assert(best_fixed_u32(5.75, 32, &fac, &shift) == MATH_SUCCESS);
    from_fixed_to_double(fac, shift, &out);
    assert(fabs(out - 5.75) < 1e-6);

    // x < 1
    assert(best_fixed_u32(0.5, 32, &fac, &shift) == MATH_SUCCESS);
    from_fixed_to_double(fac, shift, &out);
    assert(fabs(out - 0.5) < 1e-6);
}

void test_from_fixed_to_double() {
    TEST_START("FROM FIXED TO DOUBLE");
    double out;
    assert(from_fixed_to_double(23, 2, &out) == MATH_SUCCESS);
    assert(fabs(out - 5.75) < 1e-6);
    assert(from_fixed_to_double(1, 0, &out) == MATH_SUCCESS);
    assert(fabs(out - 1.0) < 1e-6);
    assert(from_fixed_to_double(256, 8, &out) == MATH_SUCCESS);
    assert(fabs(out - 1.0) < 1e-6);
}

void test_calc_slope() {
    TEST_START("CALC SLOPE");
    double slope;

    // vertical line
    assert(calc_slope(5, 5, 0, 10, &slope) == MATH_ERROR_INVALID);

    // horizontal line
    assert(calc_slope(0, 10, 5, 5, &slope) == MATH_SUCCESS);
    assert(fabs(slope - 0.0) < 1e-6);

    // positive slope
    assert(calc_slope(0, 4, 0, 8, &slope) == MATH_SUCCESS);
    assert(fabs(slope - 2.0) < 1e-6);

    // negative slope (y1 > y2)
    assert(calc_slope(0, 4, 8, 0, &slope) == MATH_SUCCESS);
    assert(fabs(slope - (-2.0)) < 1e-6);
}

void test_point_slope_lerp() {
    TEST_START("POINT SLOPE LERP");
    double slope, out;

    calc_slope(0, 4, 0, 8, &slope);

    // at the origin point
    assert(point_slope_lerp(0, slope, 0, 0, &out) == MATH_SUCCESS);
    assert(fabs(out - 0.0) < 1e-6);

    // midpoint
    assert(point_slope_lerp(2, slope, 0, 0, &out) == MATH_SUCCESS);
    assert(fabs(out - 4.0) < 1e-6);

    // endpoint
    assert(point_slope_lerp(4, slope, 0, 0, &out) == MATH_SUCCESS);
    assert(fabs(out - 8.0) < 1e-6);

    // before x1 (extrapolation)
    assert(point_slope_lerp(0, slope, 2, 4, &out) == MATH_SUCCESS);
    assert(fabs(out - 0.0) < 1e-6);
}

void test_log2_pow2() {
    TEST_START("LOG2 POW2");
    assert(log2_pow2(1)            == 0);
    assert(log2_pow2(2)            == 1);
    assert(log2_pow2(256)          == 8);
    assert(log2_pow2(pow2_u32(16)) == 16);
    assert(log2_pow2(pow2_u32(31)) == 31);
}

void test_fxd_to_from_fixed() {
    TEST_START("FXD TO/FROM FIXED");
    st_ufxd_t f = to_fixed(1.5, 8);
    assert(f.frac_bits == 8);
    assert(f.val == 384); // 1.5 * 256 = 384
    assert(fabs(from_fixed(&f) - 1.5) < 1e-6);

    st_ufxd_t g = to_fixed(0.5, 4);
    assert(g.val == 8); // 0.5 * 16 = 8
    assert(fabs(from_fixed(&g) - 0.5) < 1e-6);
}

void test_fxd_mul() {
    TEST_START("FXD MUL");
    st_ufxd_t a = to_fixed(2.0, 8);
    st_ufxd_t b = to_fixed(3.0, 8);
    st_ufxd_t c = fxd_mul(&a, &b);
    assert(c.frac_bits == 16);
    assert(fabs(from_fixed(&c) - 6.0) < 1e-6);
}

void test_fxd_normalize() {
    TEST_START("FXD NORMALIZE");
    st_ufxd_t a = to_fixed(1.5, 8); // val=384, frac=8
    st_ufxd_t b = fxd_normalize(&a, 4);
    assert(b.frac_bits == 4);
    assert(b.val == 24); // 384 >> 4 = 24
    assert(fabs(from_fixed(&b) - 1.5) < 1e-6);
}

void test_fxd_add_sub() {
    TEST_START("FXD ADD / SUB");
    st_ufxd_t a = to_fixed(2.0, 8);
    st_ufxd_t b = to_fixed(1.5, 8);

    st_ufxd_t sum = fxd_add(&a, &b);
    assert(fabs(from_fixed(&sum) - 3.5) < 1e-6);

    st_ufxd_t diff = fxd_sub(&a, &b);
    assert(fabs(from_fixed(&diff) - 0.5) < 1e-6);
}

void test_fxd_mul_and_norm() {
    TEST_START("FXD MUL AND NORM");
    st_ufxd_t a = to_fixed(2.0, 8); // val=512
    st_ufxd_t b = to_fixed(0.5, 8); // val=128
    st_ufxd_t c = fxd_mul_and_norm(&a, &b);
    // (512 * 128) >> 8 = 65536 >> 8 = 256
    assert(c.frac_bits == 8);
    assert(c.val == 256);
    assert(fabs(from_fixed(&c) - 1.0) < 1e-6);
}

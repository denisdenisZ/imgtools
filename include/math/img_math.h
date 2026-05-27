#ifndef IMG_MATH_H
#define IMG_MATH_H

#include <stdint.h>

typedef enum {
    MATH_SUCCESS,
    MATH_ERROR_INVALID
} e_math_status;

static inline uint32_t pow2_u32(uint32_t pow) {
    return (1U << pow);
}

static inline uint32_t get_mrv(uint8_t bits) {
    return (uint32_t)((1ULL << bits) - 1);
}

static inline e_math_status best_fixed_u32(double x, uint8_t bits, uint32_t *fac, uint32_t *shift) {
    if (!(x > 0.0)) return MATH_ERROR_INVALID;

    uint32_t x_int = (uint32_t)x;

    if (x_int == 0) x_int = 1;

    *shift = (bits - 1) - (31 - __builtin_clz(x_int));
    *fac = (uint32_t)(x * pow2_u32(*shift) + 0.5);

    return MATH_SUCCESS;
}

static inline e_math_status from_fixed_to_double(uint32_t fac, uint32_t shift, double *out) {
    *out = (double)fac * (1.0 / pow2_u32(shift));
    return MATH_SUCCESS;
}

static inline e_math_status calc_slope(uint32_t x1, uint32_t x2, uint32_t y1, uint32_t y2, double *out) {
    if (x1 == x2) return MATH_ERROR_INVALID;
    *out = ((double)y2 - (double)y1) / ((double)x2 - (double)x1);
    return MATH_SUCCESS;
}

static inline e_math_status point_slope_lerp(uint32_t x, double slope, uint32_t x1, uint32_t y1, double *out) {
    *out = (double)y1 + (((double)x - (double)x1) * slope);
    return MATH_SUCCESS;
}

static inline uint32_t log2_pow2(uint32_t val) {
    return (uint32_t)__builtin_ctz(val);
}

#endif

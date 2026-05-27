#ifndef FXD_H
#define FXD_H

#include <stdint.h>
#include "img_math.h"

typedef struct {
    uint8_t  frac_bits;
    uint32_t val;
} st_ufxd_t;

static inline st_ufxd_t to_fixed(double val, uint8_t frac_bits) {
    return (st_ufxd_t){
        .frac_bits = frac_bits,
        .val = (uint32_t)(val * pow2_u32(frac_bits))
    };
}

static inline double from_fixed(const st_ufxd_t *fxd) {
    return (double)fxd->val / pow2_u32(fxd->frac_bits);
}

static inline st_ufxd_t fxd_mul(const st_ufxd_t *a, const st_ufxd_t *b) {
    return (st_ufxd_t){
        .frac_bits = a->frac_bits * 2,
        .val = (uint32_t)((uint64_t)a->val * b->val)
    };
}

static inline st_ufxd_t fxd_normalize(const st_ufxd_t *a, uint8_t frac_bits) {
    return (st_ufxd_t){
        .frac_bits = frac_bits,
        .val = a->val >> (a->frac_bits - frac_bits)
    };
}

static inline st_ufxd_t fxd_add(const st_ufxd_t *a, const st_ufxd_t *b) {
    uint32_t bv = (a->frac_bits >= b->frac_bits)
        ? b->val << (a->frac_bits - b->frac_bits)
        : b->val >> (b->frac_bits - a->frac_bits);
    return (st_ufxd_t){ .frac_bits = a->frac_bits, .val = a->val + bv };
}

static inline st_ufxd_t fxd_sub(const st_ufxd_t *a, const st_ufxd_t *b) {
    uint32_t bv = (a->frac_bits >= b->frac_bits)
        ? b->val << (a->frac_bits - b->frac_bits)
        : b->val >> (b->frac_bits - a->frac_bits);
    return (st_ufxd_t){ .frac_bits = a->frac_bits, .val = a->val - bv };
}

static inline st_ufxd_t fxd_mul_and_norm(const st_ufxd_t *a, const st_ufxd_t *b) {
    return (st_ufxd_t){
        .frac_bits = a->frac_bits,
        .val = (uint32_t)(((uint64_t)a->val * b->val) >> a->frac_bits)
    };
}

#endif

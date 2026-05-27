#ifndef DCMP_H
#define DCMP_H

#include <stddef.h>
#include <stdint.h>
#include "img/image.h"

typedef enum {
    DCMP_SUCCESS,
    DCMP_ERR_INVALID_ARG,
    DCMP_ERR_ALLOC,
} e_dcmp_status;

typedef struct {
    uint32_t *dcmp_x;
    uint32_t *dcmp_y;
    size_t cnt;

    double *slopes;
    size_t slope_cnt;

    uint8_t bpp_x;
    uint8_t bpp_y;
} st_pwl;

e_dcmp_status init_pwl(st_pwl *out, size_t points);
e_dcmp_status copy_pwl(st_pwl *dst, const st_pwl *src);
void free_pwl(st_pwl *pwl);

e_dcmp_status calc_slopes(st_pwl *pwl);
e_dcmp_status pwl_reverse(st_pwl *pwl, st_pwl *reverse);
e_dcmp_status comp_decomp(const double *vals, const st_pwl *pwl, double *out, size_t cnt);
e_dcmp_status apply_comp_decomp(st_norm_bayer_img *img, const st_pwl *pwl);

#endif

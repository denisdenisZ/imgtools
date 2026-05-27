
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "dcmp.h"
#include "img_math.h"

e_dcmp_status init_pwl(st_pwl *out, size_t cnt) {
    if (out == NULL || cnt == 0) {
        fprintf(stderr, "init_pwl: invalid argument (out=%p, cnt=%zu)\n", (void*)out, cnt);
        return DCMP_ERR_INVALID_ARG;
    }

    out->cnt = cnt;
    out->slope_cnt = cnt - 1;
    out->dcmp_x = NULL;
    out->dcmp_y = NULL;
    out->slopes = NULL;
    out->bpp_x = 0;
    out->bpp_y = 0;

    out->dcmp_x = calloc(cnt, sizeof(uint32_t));
    if (out->dcmp_x == NULL) { fprintf(stderr, "init_pwl: failed to alloc dcmp_x\n"); goto alloc_err; }

    out->dcmp_y = calloc(cnt, sizeof(uint32_t));
    if (out->dcmp_y == NULL) { fprintf(stderr, "init_pwl: failed to alloc dcmp_y\n"); goto alloc_err; }

    out->slopes = calloc(out->slope_cnt, sizeof(double));
    if (out->slopes == NULL) { fprintf(stderr, "init_pwl: failed to alloc slopes\n"); goto alloc_err; }

    return DCMP_SUCCESS;

alloc_err:
    free(out->dcmp_x);
    free(out->dcmp_y);
    free(out->slopes);
    out->dcmp_x = NULL;
    out->dcmp_y = NULL;
    out->slopes = NULL;
    return DCMP_ERR_ALLOC;
}

e_dcmp_status copy_pwl(st_pwl *dst, const st_pwl *src) {
    if (dst == NULL || src == NULL) {
        fprintf(stderr, "copy_pwl: invalid argument (dst=%p, src=%p)\n", (void*)dst, (void*)src);
        return DCMP_ERR_INVALID_ARG;
    }
    if (dst->dcmp_x == NULL || dst->dcmp_y == NULL || dst->slopes == NULL) {
        fprintf(stderr, "copy_pwl: dst buffers not allocated\n");
        return DCMP_ERR_INVALID_ARG;
    }
    if (src->dcmp_x == NULL || src->dcmp_y == NULL || src->slopes == NULL) {
        fprintf(stderr, "copy_pwl: src buffers not allocated\n");
        return DCMP_ERR_INVALID_ARG;
    }
    if (dst->cnt == 0 || dst->cnt > src->cnt) {
        fprintf(stderr, "copy_pwl: dst->cnt=%zu is invalid (src->cnt=%zu)\n", dst->cnt, src->cnt);
        return DCMP_ERR_INVALID_ARG;
    }

    dst->slope_cnt = dst->cnt - 1;
    dst->bpp_x     = src->bpp_x;
    dst->bpp_y     = src->bpp_y;

    memcpy(dst->dcmp_x, src->dcmp_x, dst->cnt       * sizeof(uint32_t));
    memcpy(dst->dcmp_y, src->dcmp_y, dst->cnt       * sizeof(uint32_t));
    memcpy(dst->slopes, src->slopes, dst->slope_cnt * sizeof(double));

    return DCMP_SUCCESS;
}

void free_pwl(st_pwl *pwl) {
    if (pwl == NULL) {
        fprintf(stderr, "free_pwl: called with NULL pointer\n");
        return;
    }

    free(pwl->dcmp_x);
    free(pwl->dcmp_y);
    free(pwl->slopes);
    pwl->dcmp_x = NULL;
    pwl->dcmp_y = NULL;
    pwl->slopes = NULL;
    pwl->cnt = 0;
    pwl->slope_cnt = 0;
    pwl->bpp_x = 0;
    pwl->bpp_y = 0;
}

/*=============================================*/
/*            SLOPE CALCULATION                */
/*=============================================*/

e_dcmp_status calc_slopes(st_pwl *pwl) {
    if (pwl == NULL) {
        fprintf(stderr, "NULL pwl passed to calc_slopes");
        return DCMP_ERR_INVALID_ARG;
    }

    for (size_t i = 0; i < pwl->slope_cnt; i++) {
        calc_slope(pwl->dcmp_x[i], pwl->dcmp_x[i+1], pwl->dcmp_y[i], pwl->dcmp_y[i+1], &pwl->slopes[i]);
    }

    return DCMP_SUCCESS;

}

/*=============================================*/
/*            PWL APPROXIMATION                */
/*=============================================*/

static inline double approximate_pwl_val_double(uint32_t x, const st_pwl *pwl) {
    if (x <= pwl->dcmp_x[0]) return pwl->dcmp_y[0];
    if (x >= pwl->dcmp_x[pwl->cnt-1]) return pwl->dcmp_y[pwl->cnt-1];

    for (size_t i = 0; i < pwl->cnt-1; i++) {
        if (x >= pwl->dcmp_x[i] && x < pwl->dcmp_x[i+1]) {
            double res = 0.0;
            point_slope_lerp(x, pwl->slopes[i], pwl->dcmp_x[i], pwl->dcmp_y[i], &res);
            return res;
        }
    }

    return 0;
}

e_dcmp_status comp_decomp(const double *vals, const st_pwl *pwl, double *out, size_t cnt) {
    for (size_t i = 0; i < cnt; i++) {
        out[i] = approximate_pwl_val_double(vals[i], pwl);
    }

    return DCMP_SUCCESS;
}

e_dcmp_status apply_comp_decomp(st_norm_bayer_img *img, const st_pwl *pwl) {
    if (!img || !pwl) return DCMP_ERR_INVALID_ARG;

    for (size_t i = 0; i < img->data_size; i++)
        img->data[i] = (uint32_t)approximate_pwl_val_double(img->data[i], pwl);

    img->bpp = pwl->bpp_y;
    return DCMP_SUCCESS;
}

/*=============================================*/
/*                  REVERSE                    */
/*=============================================*/

static inline size_t get_clip_idx(uint32_t *arr, size_t cnt) {
    size_t idx = cnt-1;
    for (size_t i = 1; i < cnt; i++) {
        if (arr[i] == arr[i-1]) {
            return i-1;
            break;
        }
    }
    return idx;
}

static inline void swap_u32(uint32_t *a, uint32_t *b, size_t cnt) {
    uint32_t tmp = 0;
    for (size_t i = 0; i < cnt; i++) {
        tmp = a[i];
        a[i] = b[i];
        b[i] = tmp;
    }
}

e_dcmp_status pwl_reverse(st_pwl *pwl, st_pwl *reverse) {
    st_pwl tmp = {0};

    size_t clip_idx = get_clip_idx(pwl->dcmp_y, pwl->cnt);

    init_pwl(&tmp, clip_idx+1);

    // NOTE: Could become copyswap to avoid more operations, but pwls don't tend
    // to be large enough to matter
    copy_pwl(&tmp, pwl);
    swap_u32(tmp.dcmp_x, tmp.dcmp_y, tmp.cnt);
    calc_slopes(&tmp);

    tmp.bpp_x = pwl->bpp_y;
    tmp.bpp_y = pwl->bpp_x;
    free_pwl(reverse);
    *reverse = tmp;

    return DCMP_SUCCESS;
}

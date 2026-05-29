#ifndef IMG_GEN_H
#define IMG_GEN_H

#include "img/image.h"

typedef enum {
    IMG_GEN_SUCCESS,
} e_img_gen_status;

typedef enum {
    MIN,
    MAX,
    FIXED_VALUE,
    GRADIENT_BITWIDTH_VERTICAL,
    GRADIENT_BITWIDTH_HORIZONTAL,
    GRADIENT_BITWIDTH_DIAGONAL,
} e_img_pattern;

e_img_gen_status gen_img(e_img_pattern pattern, st_raw_bayer_img_cfg *cfg, st_norm_bayer_img *out);

#endif

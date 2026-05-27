#ifndef IMG_GEN_H
#define IMG_GEN_H

#include <stdint.h>
#include "img/image.h"

#define BITS_IN_BYTE 8U
#define BYTE_MASK    0xFFU

typedef void (*px_transform)(uint8_t *pixel, bpp_t bpp, void *opt);
typedef void (*px_transform_xy)(uint8_t *pixel, bpp_t bpp, e_bayer_channel ch, uint32_t x, uint32_t y, void *opt);

static inline e_bayer_channel get_bayer_channel_at(e_bayer_pat pattern, uint32_t x, uint32_t y) {
    uint32_t px = x & 1, py = y & 1;
    for (e_bayer_channel ch = BAYER_R; ch <= BAYER_B; ch++) {
        if ((uint32_t)(bayer_to_rggb[pattern][ch].dx & 1) == px &&
            (uint32_t)(bayer_to_rggb[pattern][ch].dy & 1) == py)
            return ch;
    }
    return BAYER_R;
}

void write_pixel_le(uint8_t *pixel, bpp_t bpp, uint32_t val);
uint32_t read_pixel_le(const uint8_t *pixel, bpp_t bpp);

void transform_img(st_raw_bayer_img *img, px_transform transform, void *opt);
void transform_img_xy(st_raw_bayer_img *img, px_transform_xy transform, void *opt);

st_raw_bayer_img *alloc_gen_img(uint32_t width, uint32_t height, bpp_t bpp, e_bayer_pat pattern);
void free_gen_img(st_raw_bayer_img *img);

#endif

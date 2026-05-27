#include <stdlib.h>
#include "img_gen/img_gen.h"

void write_pixel_le(uint8_t *pixel, bpp_t bpp, uint32_t val) {
    uint8_t bytes = (bpp + 7) / BITS_IN_BYTE;
    for (uint8_t i = 0; i < bytes; i++) {
        pixel[i] = val & BYTE_MASK;
        val >>= BITS_IN_BYTE;
    }
}

uint32_t read_pixel_le(const uint8_t *pixel, bpp_t bpp) {
    uint32_t val = 0;
    uint8_t bytes = (bpp + 7) / BITS_IN_BYTE;
    for (uint8_t i = 0; i < bytes; i++) {
        val |= (uint32_t)pixel[i] << (i * BITS_IN_BYTE);
    }
    return val;
}

void transform_img(st_raw_bayer_img *img, px_transform transform, void *opt) {
    uint8_t bytes_pp = (img->bpp + 7) / BITS_IN_BYTE;
    for (uint32_t y = 0; y < img->height; y++) {
        for (uint32_t x = 0; x < img->width; x++) {
            transform(img->data + y * img->stride + x * bytes_pp, img->bpp, opt);
        }
    }
}

void transform_img_xy(st_raw_bayer_img *img, px_transform_xy transform, void *opt) {
    uint8_t bytes_pp = (img->bpp + 7) / BITS_IN_BYTE;
    for (uint32_t y = 0; y < img->height; y++) {
        for (uint32_t x = 0; x < img->width; x++) {
            uint8_t *pixel = img->data + y * img->stride + x * bytes_pp;
            e_bayer_channel ch = get_bayer_channel_at(img->pattern, x, y);
            transform(pixel, img->bpp, ch, x, y, opt);
        }
    }
}

st_raw_bayer_img *alloc_gen_img(uint32_t width, uint32_t height, bpp_t bpp, e_bayer_pat pattern) {
    st_raw_bayer_img *img = calloc(1, sizeof(st_raw_bayer_img));
    img->bpp        = bpp;
    img->width      = width;
    img->height     = height;
    img->pattern    = pattern;
    img->packing    = UNPACKED;
    img->bit_align  = BIT_ALIGN_LSB;
    img->endianness = ENDIAN_LITTLE;
    img->stride     = width * ((bpp + 7) / BITS_IN_BYTE);
    img->data_size  = img->stride * height;
    img->data       = calloc(img->data_size, 1);
    return img;
}

void free_gen_img(st_raw_bayer_img *img) {
    if (!img) return;
    free(img->data);
    free(img);
}

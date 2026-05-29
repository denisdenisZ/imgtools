#include <stdint.h>
#include <string.h>

#include "img_gen/img_gen.h"
#include "img/image.h"
#include "img_math.h"

static inline uint32_t get_idx(uint32_t x, uint32_t y, uint32_t width) {
    return width*y+x;
}

e_img_gen_status set_img_min(st_norm_bayer_img *img) {
    const uint32_t min = 0;
    memset(img->px_data, min, img->width * img->height * sizeof(uint32_t));
    return IMG_GEN_SUCCESS;
}

e_img_gen_status set_img_max(st_norm_bayer_img *img) {
    const uint32_t max = get_mrv(img->bpp);
    const uint32_t pixel_count = img->width * img->height;

    for (uint32_t i = 0; i < pixel_count; i++) {
        img->px_data[i] = max;
    }

    return IMG_GEN_SUCCESS;
}

e_img_gen_status set_img_to_fixed_value(st_norm_bayer_img *img, uint32_t value) {
    const uint32_t pixel_count = img->width * img->height;

    for (uint32_t i = 0; i < pixel_count; i++) {
        img->px_data[i] = value;
    }

    return IMG_GEN_SUCCESS;
}

e_img_gen_status gradient_full_bitwidth_verticle(st_norm_bayer_img *img) {
    const uint32_t pixel_count = img->width * img->height;
    const uint32_t max = get_mrv(img->bpp);
    const uint32_t step = (max + pixel_count - 1) / pixel_count;

    uint32_t value = 0;
    for (uint32_t i = 0; i < pixel_count; i++) {
        img->px_data[i] = min_u32(value, max);
        value += step;
    }

    return IMG_GEN_SUCCESS;
}

e_img_gen_status gradient_full_bitwidth_horizontal(st_norm_bayer_img *img) {
    const uint32_t pixel_count = img->width * img->height;
    const uint32_t max = get_mrv(img->bpp);
    const uint32_t step = (max + pixel_count - 1) / pixel_count;

    uint32_t value = 0;
    for (uint32_t x = 0; x < img->width; x++) {
        for (uint32_t y = 0; y < img->height; y++) {
            img->px_data[get_idx(x, y, img->width)] = min_u32(value, max);
            value += step;
        }
    }

    return IMG_GEN_SUCCESS;
}

e_img_gen_status gradient_full_bitwidth_diagonal(st_norm_bayer_img *img) {
    const uint32_t pixel_count = img->width * img->height;
    const uint32_t max = get_mrv(img->bpp);
    const uint32_t step = (max + pixel_count - 1) / pixel_count;

    uint32_t value = 0;
    for (uint32_t line = 0; line < img->width; line++) {
        for (uint32_t x = line; x >= 0; x--) {
            for (uint32_t y = 0; y < img->height; y++) {
                img->px_data[get_idx(x, y, img->width)] = min_u32(value, max);
                value += step;
            }
        }
    }

    return IMG_GEN_SUCCESS;
}

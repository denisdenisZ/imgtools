#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "image.h"
#include "io/io.h"

static inline bool is_whithin_bounds(uint32_t min, uint32_t max, uint32_t val, const char *what) {
    if (val < min) {
        fprintf(stderr, "%s value %u is below minimum %u\n", what, val, min);
        return false;
    }
    if (val > max) {
        fprintf(stderr, "%s value %u is above maximum %u\n", what, val, max);
        return false;
    }
    return true;
}

static inline bool is_cfg_valid(const st_raw_bayer_img_cfg *cfg) {
    if (cfg == NULL) {
        fprintf(stderr, "Invalid config, is NULL\n");
        return false;
    }

    bool valid = true;
    valid &= is_whithin_bounds(BPP_MIN, BPP_MAX, cfg->bpp, "Bits per pixel");
    valid &= is_whithin_bounds(WIDTH_MIN, WIDTH_MAX, cfg->width, "Width");
    valid &= is_whithin_bounds(HEIGHT_MIN, HEIGHT_MAX, cfg->height, "Height");

    return valid;
}

e_img_status create_img(const st_raw_bayer_img_cfg *cfg, st_raw_bayer_img *out) {
    if (is_cfg_valid(cfg) == false) {
        return IMG_INVALID_CFG;
    }

    memset(out, 0, sizeof(st_raw_bayer_img));

    out->bpp = cfg->bpp;
    out->pattern = cfg->pattern;
    out->packing = cfg->packing;
    out->bit_align = cfg->bit_align;
    out->endianness = cfg->endianness;
    out->compand = cfg->compand;
    out->width = cfg->width;
    out->height = cfg->height;
    out->femb_lines_cnt = cfg->femb_lines_cnt;
    out->remb_lines_cnt = cfg->remb_lines_cnt;
    out->black_level = cfg->black_level;
    out->offset_x = cfg->offset_x;
    out->offset_y = cfg->offset_y;
    // TODO: This won't work for packed images
    out->stride = cfg->width * ((cfg->bpp + 7) / 8);
    out->data = calloc(out->stride * (out->height + out->femb_lines_cnt + out->remb_lines_cnt), 1);

    out->data_size = 0;
    read_file(cfg->filename, &out->data, &out->data_size);
    if (!out->data_size) {
        return IMG_FAIL_LOAD;
    }

    return IMG_SUCCESS;
}

e_img_status destroy_raw_img(st_raw_bayer_img *img) {
    if (!img) return IMG_INVALID_CFG;
    free(img->data);
    img->data = NULL;
    img->data_size = 0;
    return IMG_SUCCESS;
}

static inline void norm_bpp(uint8_t *data, uint32_t size, bpp_t bpp, uint32_t *out) {
    switch(bpp) {
        case 8:
            for (uint32_t i = 0; i < size; i++) {
                out[i] = ((uint8_t *)data)[i];
            }
            break;
        case 10:
        case 12:
        case 14:
        case 16:
            for (uint32_t i = 0; i < size; i++) {
                out[i] = ((uint16_t *)data)[i];
            }
            break;
        case 32:
            for (uint32_t i = 0; i < size; i++) {
                out[i] = ((uint32_t *)data)[i];
            }
            break;
    }
}

static inline void norm_endianness(uint32_t *data, uint32_t size, e_endianness endianness) {
    if (endianness == ENDIAN_BIG) {
        for (uint32_t i = 0; i < size; i++) {
            data[i] = __builtin_bswap32(data[i]);
        }
    }
}

static inline void norm_bit_align(uint32_t *data, uint32_t size, bpp_t bpp, e_bit_align bit_align) {
    if (bit_align == BIT_ALIGN_MSB) {
        // NOTE: 32 assumins we have already normalized bpp
        const uint32_t shift = 32 - bpp;
        for (uint32_t i = 0; i < size; i++) {
            data[i] >>= shift;
        }
    }
}

e_img_status normalize_img(const st_raw_bayer_img *img, st_norm_bayer_img *out) {
    memset(out, 0, sizeof(st_norm_bayer_img));

    out->pattern = img->pattern;
    out->compand = img->compand;
    out->bpp = img->bpp;
    out->width = img->width;
    out->height = img->height;
    out->femb_lines_cnt = img->femb_lines_cnt;
    out->remb_lines_cnt = img->remb_lines_cnt;
    out->black_level = img->black_level;
    out->offset_x = img->offset_x;
    out->offset_y = img->offset_y;
    out->data_size = out->width * (out->femb_lines_cnt + out->remb_lines_cnt + out->height);
    out->data = calloc(out->data_size, sizeof(uint32_t));

    norm_bpp(img->data, out->data_size, img->bpp, out->data);
    norm_endianness(out->data, out->data_size, img->endianness);
    norm_bit_align(out->data, out->data_size, img->bpp, img->bit_align);

    return IMG_SUCCESS;
}


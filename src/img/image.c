#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "image.h"

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

static inline bool is_cfg_valid(st_raw_bayer_img_cfg *cfg) {
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

e_img_status create_img(st_raw_bayer_img_cfg *cfg, st_raw_bayer_img *out) {
    if (is_cfg_valid(cfg) == false) {
        return IMG_INVALID_CFG;
    }
// TODO:


    return IMG_SUCCESS;
}


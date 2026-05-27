#ifndef BASIC_PAT_H
#define BASIC_PAT_H

#include <stdbool.h>
#include <stdint.h>
#include "img/image.h"

typedef struct {
    uint32_t num_pax_x;
    uint32_t num_pax_y;
    bool invert;
} st_checkers_opts_t;

void set_max(st_raw_bayer_img *img);
void set_min(st_raw_bayer_img *img);
void set_linear_gradient(st_raw_bayer_img *img);
void set_linear_gradient_per_channel(st_raw_bayer_img *img);
void set_checkers(st_raw_bayer_img *img, uint8_t num_pax_x, uint8_t num_pax_y, bool invert);

#endif

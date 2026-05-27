#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "img_gen/img_gen.h"
#include "img_gen/basic_patterns.h"
#include "math/img_math.h"

#define BAYER_CH_COUNT 4

static inline void set_max_px(uint8_t *pixel, bpp_t bpp, void *opt) {
    (void)opt;
    write_pixel_le(pixel, bpp, pow2_u32(bpp) - 1);
}

static inline void set_min_px(uint8_t *pixel, bpp_t bpp, void *opt) {
    (void)opt;
    write_pixel_le(pixel, bpp, 0);
}

void set_max(st_raw_bayer_img *img) { transform_img(img, set_max_px, NULL); }
void set_min(st_raw_bayer_img *img) { transform_img(img, set_min_px, NULL); }

typedef struct {
    double current;
    double step;
} st_gradient_ctx_t;

static inline void gradient_step(uint8_t *pixel, bpp_t bpp, void *opt) {
    st_gradient_ctx_t *ctx = opt;
    write_pixel_le(pixel, bpp, (uint32_t)ctx->current);
    ctx->current += ctx->step;
}

void set_linear_gradient(st_raw_bayer_img *img) {
    st_gradient_ctx_t ctx = {
        .current = 0.0,
        .step    = (double)(pow2_u32(img->bpp) - 1) / ((double)img->width * img->height)
    };
    transform_img(img, gradient_step, &ctx);
}

typedef struct {
    uint32_t count[BAYER_CH_COUNT];
    uint32_t total[BAYER_CH_COUNT];
    uint32_t max_val;
} st_ch_gradient_ctx_t;

static inline void ch_gradient_step(uint8_t *pixel, bpp_t bpp, e_bayer_channel ch,
                                    uint32_t x, uint32_t y, void *opt) {
    (void)x; (void)y;
    st_ch_gradient_ctx_t *ctx = opt;
    uint32_t val = (ctx->count[ch] * ctx->max_val) / ctx->total[ch];
    write_pixel_le(pixel, bpp, val);
    ctx->count[ch]++;
}

void set_linear_gradient_per_channel(st_raw_bayer_img *img) {
    uint32_t pixels_per_ch = (img->width * img->height) / BAYER_CH_COUNT;
    st_ch_gradient_ctx_t ctx = {
        .count   = {0},
        .total   = {pixels_per_ch, pixels_per_ch, pixels_per_ch, pixels_per_ch},
        .max_val = pow2_u32(img->bpp) - 1
    };
    transform_img_xy(img, ch_gradient_step, &ctx);
}

typedef struct {
    uint32_t x;
    uint32_t y;
    uint32_t pix_per_pax_x;
    uint32_t pix_per_pax_y;
    uint32_t width;
    bool     invert;
} st_checkers_ctx_t;

static inline void checkers_px(uint8_t *pixel, bpp_t bpp, void *opt) {
    st_checkers_ctx_t *ctx = opt;
    bool white = (ctx->x / ctx->pix_per_pax_x + ctx->y / ctx->pix_per_pax_y) % 2 == ctx->invert;
    write_pixel_le(pixel, bpp, white ? pow2_u32(bpp) - 1 : 0);
    if (++ctx->x >= ctx->width) { ctx->x = 0; ctx->y++; }
}

void set_checkers(st_raw_bayer_img *img, uint8_t num_pax_x, uint8_t num_pax_y, bool invert) {
    if (img->height % num_pax_y != 0 || img->width % num_pax_x != 0) {
        fprintf(stderr, "resolution does not divide evenly by paxels\n");
        exit(1);
    }
    st_checkers_ctx_t ctx = {
        .x             = 0,
        .y             = 0,
        .pix_per_pax_x = img->width  / num_pax_x,
        .pix_per_pax_y = img->height / num_pax_y,
        .width         = img->width,
        .invert        = invert
    };
    transform_img(img, checkers_px, &ctx);
}

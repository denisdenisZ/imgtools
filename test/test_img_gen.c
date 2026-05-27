#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "img_gen/img_gen.h"
#include "img_gen/basic_patterns.h"
#include "math/img_math.h"
#include "pgm.h"
#include "tests.h"

static inline uint8_t *px_at(st_raw_bayer_img *img, uint32_t x, uint32_t y) {
    return img->data + y * img->stride + x * ((img->bpp + 7) / 8);
}

void test_pixel_rw() {
    TEST_START("PIXEL READ/WRITE LE");
    uint8_t buf[4] = {0};

    write_pixel_le(buf, 8, 0xAB);
    assert(read_pixel_le(buf, 8) == 0xAB);

    write_pixel_le(buf, 8, 0);
    assert(read_pixel_le(buf, 8) == 0);

    write_pixel_le(buf, 16, 0x1234);
    assert(read_pixel_le(buf, 16) == 0x1234);
    assert(buf[0] == 0x34); // LE: low byte first
    assert(buf[1] == 0x12);

    write_pixel_le(buf, 16, 0xFFFF);
    assert(read_pixel_le(buf, 16) == 0xFFFF);
}

void test_alloc_gen_img() {
    TEST_START("ALLOC GEN IMG");
    st_raw_bayer_img *img = alloc_gen_img(4, 2, 16, RGGB);

    assert(img != NULL);
    assert(img->width      == 4);
    assert(img->height     == 2);
    assert(img->bpp        == 16);
    assert(img->pattern    == RGGB);
    assert(img->stride     == 4 * 2);
    assert(img->data_size  == 4 * 2 * 2);
    assert(img->data       != NULL);
    assert(img->endianness == ENDIAN_LITTLE);
    assert(img->bit_align  == BIT_ALIGN_LSB);
    assert(img->packing    == UNPACKED);

    for (uint32_t i = 0; i < img->data_size; i++)
        assert(img->data[i] == 0);

    free_gen_img(img);
}

void test_get_bayer_channel_at() {
    TEST_START("GET BAYER CHANNEL AT");

    // RGGB 2x2 tile
    assert(get_bayer_channel_at(RGGB, 0, 0) == BAYER_R);
    assert(get_bayer_channel_at(RGGB, 1, 0) == BAYER_GR);
    assert(get_bayer_channel_at(RGGB, 0, 1) == BAYER_GB);
    assert(get_bayer_channel_at(RGGB, 1, 1) == BAYER_B);

    // pattern repeats
    assert(get_bayer_channel_at(RGGB, 2, 0) == BAYER_R);
    assert(get_bayer_channel_at(RGGB, 3, 1) == BAYER_B);

    // BGGR 2x2 tile
    assert(get_bayer_channel_at(BGGR, 0, 0) == BAYER_B);
    assert(get_bayer_channel_at(BGGR, 1, 0) == BAYER_GB);
    assert(get_bayer_channel_at(BGGR, 0, 1) == BAYER_GR);
    assert(get_bayer_channel_at(BGGR, 1, 1) == BAYER_R);

    // GRBG 2x2 tile
    assert(get_bayer_channel_at(GRBG, 0, 0) == BAYER_GR);
    assert(get_bayer_channel_at(GRBG, 1, 0) == BAYER_R);
    assert(get_bayer_channel_at(GRBG, 0, 1) == BAYER_B);
    assert(get_bayer_channel_at(GRBG, 1, 1) == BAYER_GB);
}

void test_set_min_max() {
    TEST_START("SET MIN / SET MAX");
    st_raw_bayer_img *img = alloc_gen_img(4, 2, 16, RGGB);
    uint32_t max_val = pow2_u32(16) - 1;

    set_max(img);
    for (uint32_t y = 0; y < img->height; y++)
        for (uint32_t x = 0; x < img->width; x++)
            assert(read_pixel_le(px_at(img, x, y), 16) == max_val);

    set_min(img);
    for (uint32_t y = 0; y < img->height; y++)
        for (uint32_t x = 0; x < img->width; x++)
            assert(read_pixel_le(px_at(img, x, y), 16) == 0);

    free_gen_img(img);
}

void test_set_linear_gradient() {
    TEST_START("SET LINEAR GRADIENT");
    // 4x4 = 16 pixels, step = 65535/16 = 4095.9375
    st_raw_bayer_img *img = alloc_gen_img(4, 4, 16, RGGB);
    set_linear_gradient(img);

    // first pixel is 0
    assert(read_pixel_le(px_at(img, 0, 0), 16) == 0);

    // monotonically increasing
    uint32_t prev = 0;
    for (uint32_t y = 0; y < img->height; y++) {
        for (uint32_t x = 0; x < img->width; x++) {
            if (x == 0 && y == 0) continue;
            uint32_t cur = read_pixel_le(px_at(img, x, y), 16);
            assert(cur > prev);
            prev = cur;
        }
    }

    // all values within [0, max]
    uint32_t max_val = pow2_u32(16) - 1;
    for (uint32_t y = 0; y < img->height; y++)
        for (uint32_t x = 0; x < img->width; x++)
            assert(read_pixel_le(px_at(img, x, y), 16) <= max_val);

    free_gen_img(img);
}

void test_set_linear_gradient_per_channel() {
    TEST_START("SET LINEAR GRADIENT PER CHANNEL");
    // 4x4 RGGB: 4 pixels per channel
    // R at (0,0),(2,0),(0,2),(2,2)
    st_raw_bayer_img *img = alloc_gen_img(4, 4, 16, RGGB);
    set_linear_gradient_per_channel(img);

    // each channel starts at 0
    assert(read_pixel_le(px_at(img, 0, 0), 16) == 0); // R first
    assert(read_pixel_le(px_at(img, 1, 0), 16) == 0); // GR first
    assert(read_pixel_le(px_at(img, 0, 1), 16) == 0); // GB first
    assert(read_pixel_le(px_at(img, 1, 1), 16) == 0); // B first

    // each channel is monotonically increasing: R: (0,0) < (2,0) < (0,2) < (2,2)
    assert(read_pixel_le(px_at(img, 0, 0), 16) < read_pixel_le(px_at(img, 2, 0), 16));
    assert(read_pixel_le(px_at(img, 2, 0), 16) < read_pixel_le(px_at(img, 0, 2), 16));
    assert(read_pixel_le(px_at(img, 0, 2), 16) < read_pixel_le(px_at(img, 2, 2), 16));

    free_gen_img(img);
}

void test_set_checkers() {
    TEST_START("SET CHECKERS");
    // 4x4, 2x2 paxels -> 2x2 pixels each
    st_raw_bayer_img *img = alloc_gen_img(4, 4, 16, RGGB);
    uint32_t max_val = pow2_u32(16) - 1;

    set_checkers(img, 2, 2, false);
    // paxel (0,0): white, paxel (1,0): black, paxel (0,1): black, paxel (1,1): white
    assert(read_pixel_le(px_at(img, 0, 0), 16) == max_val);
    assert(read_pixel_le(px_at(img, 1, 0), 16) == max_val);
    assert(read_pixel_le(px_at(img, 2, 0), 16) == 0);
    assert(read_pixel_le(px_at(img, 3, 0), 16) == 0);
    assert(read_pixel_le(px_at(img, 0, 2), 16) == 0);
    assert(read_pixel_le(px_at(img, 2, 2), 16) == max_val);

    set_checkers(img, 2, 2, true);
    // inverted: paxel (0,0): black, paxel (1,0): white
    assert(read_pixel_le(px_at(img, 0, 0), 16) == 0);
    assert(read_pixel_le(px_at(img, 2, 0), 16) == max_val);
    assert(read_pixel_le(px_at(img, 2, 2), 16) == 0);

    free_gen_img(img);
}

void test_write_pgm() {
    TEST_START("WRITE PGM");
    // 2x2 12-bit image, all pixels at max → PGM should be all 0xFFFF
    st_norm_bayer_img img = {
        .bpp       = 12,
        .width     = 2,
        .height    = 2,
        .data_size = 4,
        .data      = malloc(4 * sizeof(uint32_t))
    };
    uint32_t max_val = pow2_u32(12) - 1; // 4095
    for (size_t i = 0; i < 4; i++)
        img.data[i] = max_val;

    const char *path = "/tmp/test_write_pgm.pgm";
    assert(write_pgm(path, &img) == IO_SUCCESS);

    // verify header and pixel values
    FILE *f = fopen(path, "rb");
    assert(f != NULL);
    uint32_t w, h, mv;
    assert(fscanf(f, "P5\n%u %u\n%u\n", &w, &h, &mv) == 3);
    assert(w == 2 && h == 2 && mv == 65535);

    // all pixels should be 0xFFFF (big-endian)
    for (int i = 0; i < 4; i++) {
        int hi = fgetc(f), lo = fgetc(f);
        assert(hi == 0xFF && lo == 0xFF);
    }
    fclose(f);
    free(img.data);
}

void test_write_pgm_heatmap() {
    TEST_START("WRITE PGM HEATMAP");
    // 1x2 image: pixel 0 at max (saturated), pixel 1 at 0 (dark)
    st_norm_bayer_img img = {
        .bpp       = 12,
        .width     = 1,
        .height    = 2,
        .data_size = 2,
        .data      = malloc(2 * sizeof(uint32_t))
    };
    uint32_t max_val = pow2_u32(12) - 1;
    img.data[0] = max_val; // saturated → complement = 0 → black in heatmap
    img.data[1] = 0;       // dark      → complement = max → white in heatmap

    const char *path = "/tmp/test_write_pgm_heatmap.pgm";
    assert(write_pgm_heatmap(path, &img) == IO_SUCCESS);

    FILE *f = fopen(path, "rb");
    assert(f != NULL);
    uint32_t w, h, mv;
    assert(fscanf(f, "P5\n%u %u\n%u\n", &w, &h, &mv) == 3);
    assert(w == 1 && h == 2 && mv == 65535);

    // pixel 0 (was max) → complement 0 → scaled to 0x0000
    int hi = fgetc(f), lo = fgetc(f);
    assert(hi == 0x00 && lo == 0x00);

    // pixel 1 (was 0) → complement max → scaled to 0xFFFF
    hi = fgetc(f); lo = fgetc(f);
    assert(hi == 0xFF && lo == 0xFF);

    fclose(f);
    free(img.data);
}

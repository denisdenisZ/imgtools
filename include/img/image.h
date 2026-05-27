#ifndef IMG_H
#define IMG_H

#include <stdint.h>
#include <stdbool.h>

typedef uint8_t bpp_t;
typedef uint32_t raw_pixel_t;

typedef enum {
    RGGB,
    BGGR,
    GRBG,
    GBGR,
    GBRG,
    RGBIR,
    BAY_PAT_MAX
} e_bayer_pat;

typedef enum {
    UNPACKED,
} e_packing;

typedef enum {
    BIT_ALIGN_LSB,
    BIT_ALIGN_MSB,
} e_bit_align;

typedef enum {
    ENDIAN_LITTLE,
    ENDIAN_BIG,
} e_endianness;

typedef struct {
    uint32_t *in_val;
    uint32_t *out_val;
} st_comp_curve;

typedef struct {
    uint8_t src_bpp;
    uint8_t dst_bpp;
    uint8_t knee_points_cnt;
    st_comp_curve curve;
} st_compand_info;

static const bpp_t BPP_MIN = 8;
static const bpp_t BPP_MAX = 32;

static const uint32_t WIDTH_MIN = 1;
static const uint32_t WIDTH_MAX = 16384;

static const uint32_t HEIGHT_MIN = 1;
static const uint32_t HEIGHT_MAX = 16384;

static const e_bit_align BIT_ALIGN_DEF  = BIT_ALIGN_LSB;
static const e_bayer_pat BAYER_PAT_DEF  = RGGB;
static const e_endianness ENDIANESS_DEF = ENDIAN_LITTLE;
static const e_packing PACKING_DEF      = UNPACKED;

typedef struct {
    int8_t dx;
    int8_t dy;
} st_bayer_offset;

typedef enum {
    BAYER_R  = 0,
    BAYER_GR = 1,
    BAYER_GB = 2,
    BAYER_B  = 3
} e_bayer_channel;

static const st_bayer_offset bayer_to_rggb[BAY_PAT_MAX][4] = {
//                R        Gr       Gb       B
    [RGGB] = {{0,0},   {1,0},   {0,1},   {1,1}},  // reference
    [GRBG] = {{1,0},   {0,0},   {1,1},   {0,1}},  // R is one pixel right
    [GBRG] = {{0,1},   {1,1},   {0,0},   {1,0}},  // R is one line down
    [BGGR] = {{1,1},   {0,1},   {1,0},   {0,0}},  // R is one pixel right and one line down
};

typedef struct {
    bpp_t bpp;
    e_bayer_pat pattern;
    e_packing packing;
    e_bit_align bit_align;
    e_endianness endianness;

    st_compand_info *compand;

    uint32_t stride;
    uint32_t width;
    uint32_t height;
    uint32_t femb_lines_cnt;
    uint32_t remb_lines_cnt;
    uint32_t black_level;

    // NOTE: Does not count femb, 0,0 is the first valid bayer pixel
    uint32_t offset_x;
    uint32_t offset_y;

    uint8_t *data;
    uint32_t data_size;
} st_raw_bayer_img;

typedef struct {
    st_compand_info *compand;
    e_bayer_pat pattern;
    bpp_t bpp;

    uint32_t width;
    uint32_t height;
    uint32_t femb_lines_cnt;
    uint32_t remb_lines_cnt;
    uint32_t black_level;

    // NOTE: Does not count femb, 0,0 is the first valid bayer pixel
    uint32_t offset_x;
    uint32_t offset_y;

    uint32_t *data;
    uint32_t data_size;
} st_norm_bayer_img;

typedef enum {
    IMG_SUCCESS,
    IMG_INVALID_CFG,
    IMG_FAIL_LOAD,
} e_img_status;

typedef struct {
    char *filename;

    bpp_t bpp;
    e_bayer_pat pattern;
    e_packing packing;
    e_bit_align bit_align;
    e_endianness endianness;

    st_compand_info *compand;

    uint32_t width;
    uint32_t height;

    uint32_t femb_lines_cnt;
    uint32_t remb_lines_cnt;

    uint32_t black_level;

    uint32_t offset_x;
    uint32_t offset_y;
} st_raw_bayer_img_cfg;

static inline uint32_t get_bayer_channel(
    const uint32_t *data,
    uint32_t stride,
    e_bayer_pat pattern,
    uint32_t x,
    uint32_t y,
    e_bayer_channel channel)
{
    uint32_t px = x + bayer_to_rggb[pattern][channel].dx;
    uint32_t py = y + bayer_to_rggb[pattern][channel].dy;
    return data[py * stride + px];
}

e_img_status create_img(const st_raw_bayer_img_cfg *cfg, st_raw_bayer_img *out);
e_img_status normalize_img(const st_raw_bayer_img *img, st_norm_bayer_img *out);

#endif


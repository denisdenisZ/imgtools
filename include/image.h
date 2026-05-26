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
    RGBIR,
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
    bpp_t bpp;
    e_bayer_pat pattern;
    e_packing packing;
    e_bit_align bit_align;
    e_endianness endianess;

    st_compand_info *compand;

    uint32_t stride;
    uint32_t width;
    uint32_t height;
    uint32_t femb_lines_cnt;
    uint32_t remb_lines_cnt;
    uint32_t black_level;

    uint32_t offset_x;
    uint32_t offset_y;

    uint8_t *data;
} st_raw_bayer_img;

typedef enum {
    IMG_SUCCESS,
    IMG_INVALID_CFG,
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
e_img_status create_img(st_raw_bayer_img_cfg *cfg, st_raw_bayer_img *out);

#endif


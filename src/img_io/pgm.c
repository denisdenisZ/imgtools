#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pgm.h"
#include "math/img_math.h"

static e_io_status write_pgm_pixels(const char *filename, uint32_t width, uint32_t height,
                                     uint32_t src_max, const uint32_t *data) {
    FILE *f = fopen(filename, "wb");
    if (!f) {
        fprintf(stderr, "fopen failed for '%s': %s\n", filename, strerror(errno));
        return IO_ERR_NOT_FOUND;
    }

    fprintf(f, "P5\n%u %u\n65535\n", width, height);

    size_t pixel_cnt = (size_t)width * height;
    uint8_t *buf = malloc(pixel_cnt * 2);
    if (!buf) {
        fclose(f);
        return IO_ERR_OOM;
    }

    for (size_t i = 0; i < pixel_cnt; i++) {
        uint16_t v = (uint16_t)((uint64_t)data[i] * 65535 / src_max);
        buf[i * 2]     = v >> 8;    // PGM 16-bit is big-endian
        buf[i * 2 + 1] = v & 0xFF;
    }

    e_io_status status = IO_SUCCESS;
    if (fwrite(buf, 1, pixel_cnt * 2, f) != pixel_cnt * 2) {
        fprintf(stderr, "fwrite failed for '%s': %s\n", filename, strerror(errno));
        status = IO_ERR_WRITE;
    }

    free(buf);
    fclose(f);
    return status;
}

e_io_status write_pgm(const char *filename, const st_norm_bayer_img *img) {
    if (!filename || !img || !img->data) return IO_ERR_NOT_FOUND;

    uint32_t max_val = pow2_u32(img->bpp) - 1;
    return write_pgm_pixels(filename, img->width, img->height, max_val, img->data);
}

e_io_status write_pgm_heatmap(const char *filename, const st_norm_bayer_img *img) {
    if (!filename || !img || !img->data) return IO_ERR_NOT_FOUND;

    uint32_t max_val = pow2_u32(img->bpp) - 1;
    size_t pixel_cnt = (size_t)img->width * img->height;

    uint32_t *complement = malloc(pixel_cnt * sizeof(uint32_t));
    if (!complement) return IO_ERR_OOM;

    for (size_t i = 0; i < pixel_cnt; i++)
        complement[i] = max_val - img->data[i];

    e_io_status status = write_pgm_pixels(filename, img->width, img->height, max_val, complement);
    free(complement);
    return status;
}

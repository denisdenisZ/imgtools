#ifndef IMGFACTORY_H
#define IMGFACTORY_H

#include <stdint.h>
#include "img/image.h"

typedef enum {
    PAT_MIN,
    PAT_MAX,
    PAT_LINEAR_GRADIENT,
    PAT_LINEAR_GRADIENT_PER_CH,
    PAT_CHECKERS,
} e_pattern_t;

void create_image(
    uint32_t width,
    uint32_t height,
    bpp_t bpp,
    e_bayer_pat bayer_pattern,
    e_pattern_t pattern,
    const char *filename,
    void *opts);

#endif

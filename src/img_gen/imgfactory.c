#include <stdio.h>

#include "img_gen/img_gen.h"
#include "img_gen/imgfactory.h"
#include "img_gen/basic_patterns.h"
#include "io/io.h"

void create_image(
    uint32_t width,
    uint32_t height,
    bpp_t bpp,
    e_bayer_pat bayer_pattern,
    e_pattern_t pattern,
    const char *filename,
    void *opts)
{
    st_raw_bayer_img *img = alloc_gen_img(width, height, bpp, bayer_pattern);

    switch (pattern) {
        case PAT_MIN:                    set_min(img);                          break;
        case PAT_MAX:                    set_max(img);                          break;
        case PAT_LINEAR_GRADIENT:        set_linear_gradient(img);              break;
        case PAT_LINEAR_GRADIENT_PER_CH: set_linear_gradient_per_channel(img); break;
        case PAT_CHECKERS: {
            st_checkers_opts_t *opt = opts;
            set_checkers(img, opt->num_pax_x, opt->num_pax_y, opt->invert);
        } break;
        default:
            fprintf(stderr, "Unknown pattern!\n");
            break;
    }

    write_file(filename, img->data, img->data_size);
    free_gen_img(img);
}

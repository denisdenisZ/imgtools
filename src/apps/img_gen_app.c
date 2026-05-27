#include <stddef.h>
#include "img_gen/imgfactory.h"
#include "img_gen/basic_patterns.h"
#include "img/image.h"

int main(void) {
    create_image(1920, 1080, 16, RGGB, PAT_MIN,                    "MIN_16bpp.raw",                 NULL);
    create_image(1920, 1080, 16, RGGB, PAT_MAX,                    "MAX_16bpp.raw",                 NULL);
    create_image(1920, 1080, 16, RGGB, PAT_LINEAR_GRADIENT,        "LINEAR_GRADIENT_16bpp.raw",     NULL);
    create_image(1920, 1080, 16, RGGB, PAT_LINEAR_GRADIENT_PER_CH, "LINEAR_GRADIENT_PER_CH_16bpp.raw", NULL);

    st_checkers_opts_t chck_o = { .num_pax_x = 16, .num_pax_y = 40, .invert = false };
    create_image(1920, 1080, 16, RGGB, PAT_CHECKERS, "CHECKERS_16bpp.raw",     &chck_o);
    chck_o.invert = true;
    create_image(1920, 1080, 16, RGGB, PAT_CHECKERS, "CHECKERS_INV_16bpp.raw", &chck_o);

    return 0;
}

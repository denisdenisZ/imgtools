#ifndef PGM_H
#define PGM_H

#include "img/image.h"
#include "io/io.h"

e_io_status write_pgm(const char *filename, const st_norm_bayer_img *img);
e_io_status write_pgm_heatmap(const char *filename, const st_norm_bayer_img *img);

#endif

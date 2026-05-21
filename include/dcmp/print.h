#ifndef PRINT_PWL_H
#define PRINT_PWL_H

#include <stdio.h>

#include "dcmp.h"

static inline void pwl_print(const st_pwl *pwl) {
    if (!pwl) {
        fprintf(stderr, "pwl is NULL\n");
        return;
    }

    fprintf(stdout, "PWL:\n");
    fprintf(stdout, "  bpp_x: %u  bpp_y: %u\n", pwl->bpp_x, pwl->bpp_y);
    fprintf(stdout, "  points: %zu  slopes: %zu\n\n", pwl->cnt, pwl->slope_cnt);

    fprintf(stdout, "  %-6s  %-10s  %-10s  %-12s\n", "idx", "x", "y", "slope");
    fprintf(stdout, "  %-6s  %-10s  %-10s  %-12s\n", "---", "---", "---", "-----");

    size_t max = pwl->cnt > pwl->slope_cnt ? pwl->cnt : pwl->slope_cnt;
    for (size_t i = 0; i < max; i++) {
        const char *x_str     = i < pwl->cnt        ? "" : "-";
        const char *y_str     = i < pwl->cnt        ? "" : "-";
        const char *slope_str = i < pwl->slope_cnt  ? "" : "-";

        if (i < pwl->cnt && i < pwl->slope_cnt)
            fprintf(stdout, "  %-6zu  %-10u  %-10u  %-12.6f\n", i, pwl->dcmp_x[i], pwl->dcmp_y[i], pwl->slopes[i]);
        else if (i < pwl->cnt)
            fprintf(stdout, "  %-6zu  %-10u  %-10u  %-12s\n", i, pwl->dcmp_x[i], pwl->dcmp_y[i], "-");
        else
            fprintf(stdout, "  %-6zu  %-10s  %-10s  %-12.6f\n", i, "-", "-", pwl->slopes[i]);
    }
}

#endif

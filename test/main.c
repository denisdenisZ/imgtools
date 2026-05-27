#include "tests.h"

int main(void) {
    test_pwl_print();
    test_calc_slopes();
    test_comp_decomp();
    test_pwl_reverse();
    test_pwl_reverse_clipped();
    test_pwl_reverse_in_place();
    test_comp_decomp();
    test_comp_decomp_clipped();
    test_apply_comp_decomp();
    test_apply_comp_decomp_null();

    test_pow2_u32();
    test_get_mrv();
    test_best_fixed_u32();
    test_from_fixed_to_double();
    test_calc_slope();
    test_point_slope_lerp();
    test_log2_pow2();
    test_fxd_to_from_fixed();
    test_fxd_mul();
    test_fxd_normalize();
    test_fxd_add_sub();
    test_fxd_mul_and_norm();

    test_pixel_rw();
    test_alloc_gen_img();
    test_get_bayer_channel_at();
    test_set_min_max();
    test_set_linear_gradient();
    test_set_linear_gradient_per_channel();
    test_set_checkers();
    test_write_pgm();
    test_write_pgm_heatmap();
    return 0;
}

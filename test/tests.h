#ifndef TESTS_H
#define TESTS_H

#include <stdio.h>

#define TEST_START(name) printf("\n\n[TEST] %-40s \n", name)

void test_pwl_print();
void test_calc_slopes();
void test_comp_decomp();
void test_pwl_reverse();
void test_pwl_reverse_clipped();
void test_pwl_reverse_in_place();
void test_comp_decomp();
void test_comp_decomp_clipped();
void test_apply_comp_decomp();
void test_apply_comp_decomp_null();

void test_pow2_u32();
void test_get_mrv();
void test_best_fixed_u32();
void test_from_fixed_to_double();
void test_calc_slope();
void test_point_slope_lerp();
void test_log2_pow2();
void test_fxd_to_from_fixed();
void test_fxd_mul();
void test_fxd_normalize();
void test_fxd_add_sub();
void test_fxd_mul_and_norm();

void test_pixel_rw();
void test_alloc_gen_img();
void test_get_bayer_channel_at();
void test_set_min_max();
void test_set_linear_gradient();
void test_set_linear_gradient_per_channel();
void test_set_checkers();
void test_write_pgm();
void test_write_pgm_heatmap();

#endif

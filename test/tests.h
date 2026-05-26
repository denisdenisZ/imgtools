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

void test_pow2_u32();
void test_get_mrv();
void test_best_fixed_u32();
void test_from_fixed_to_double();
void test_calc_slope();
void test_point_slope_lerp();

#endif

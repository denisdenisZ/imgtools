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

    test_pow2_u32();
    test_get_mrv();
    test_best_fixed_u32();
    test_from_fixed_to_double();
    test_calc_slope();
    test_point_slope_lerp();
    return 0;
}

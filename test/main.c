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
    return 0;
}

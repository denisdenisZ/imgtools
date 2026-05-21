#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "dcmp/dcmp.h"
#include "dcmp/print.h"

#include "tests.h"

#define TEST_START(name) printf("\n\n[TEST] %-40s \n", name)

#define KP_CNT  5
#define SLP_CNT (KP_CNT - 1)

static const uint32_t test_x[KP_CNT] = { 0, 1048576, 4194304, 8388608, 16777215 };
static const uint32_t test_y[KP_CNT] = { 0,     512,    1024,    2048,    4095   };
static const double test_slopes[SLP_CNT] = {
    (512.0  - 0.0)    / (1048576.0  - 0.0),       // 0.000488
    (1024.0 - 512.0)  / (4194304.0  - 1048576.0), // 0.000163
    (2048.0 - 1024.0) / (8388608.0  - 4194304.0), // 0.000244
    (4095.0 - 2048.0) / (16777215.0 - 8388608.0)  // 0.000244
};

static const uint32_t test_y_clipped[KP_CNT] = { 0, 512, 1024, 1024, 1024 };
static const double test_slopes_clipped[SLP_CNT] = {
    (512.0  - 0.0)    / (1048576.0 - 0.0),
    (1024.0 - 512.0)  / (4194304.0 - 1048576.0),
    (1024.0 - 1024.0) / (8388608.0 - 4194304.0), // 0.0 - clipped
    (1024.0 - 1024.0) / (16777215.0 - 8388608.0) // 0.0 - clipped
};

static const double test_inputs[] = {
    // Exact keypoints (should round-trip perfectly)
    0, 1048576, 4194304, 8388608, 16777215,
    // Midpoints between keypoints
    524288,   // midpoint 0     -> 1048576
    2621440,  // midpoint 1048576 -> 4194304
    6291456,  // midpoint 4194304 -> 8388608
    12582912, // midpoint 8388608 -> 16777215
    // Just above keypoints
    1, 1048577, 4194305, 8388609,
    // Just below keypoints
    1048575, 4194303, 8388607, 16777214,
};
static const size_t test_inputs_cnt = sizeof(test_inputs) / sizeof(test_inputs[0]);

static const uint32_t test_x_rev[KP_CNT] = { 0, 512, 1024, 2048, 4095 };
static const uint32_t test_y_rev[KP_CNT] = { 0, 1048576, 4194304, 8388608, 16777215 };
static const double test_slopes_rev[SLP_CNT] = {
    (double)(test_y_rev[1] - test_y_rev[0]) / (double)(test_x_rev[1] - test_x_rev[0]),
    (double)(test_y_rev[2] - test_y_rev[1]) / (double)(test_x_rev[2] - test_x_rev[1]),
    (double)(test_y_rev[3] - test_y_rev[2]) / (double)(test_x_rev[3] - test_x_rev[2]),
    (double)(test_y_rev[4] - test_y_rev[3]) / (double)(test_x_rev[4] - test_x_rev[3]),
};

#define KP_CNT_REV_CLIPPED 3
#define SLP_CNT_REV_CLIPPED (KP_CNT_REV_CLIPPED - 1)

static const uint32_t test_x_rev_clipped[KP_CNT_REV_CLIPPED] = { 0, 512, 1024 };
static const uint32_t test_y_rev_clipped[KP_CNT_REV_CLIPPED] = { 0, 1048576, 4194304 };
static const double test_slopes_rev_clipped[SLP_CNT_REV_CLIPPED] = {
    (double)(test_y_rev_clipped[1] - test_y_rev_clipped[0]) / (double)(test_x_rev_clipped[1] - test_x_rev_clipped[0]),
    (double)(test_y_rev_clipped[2] - test_y_rev_clipped[1]) / (double)(test_x_rev_clipped[2] - test_x_rev_clipped[1]),
};

bool pwl_equal(const st_pwl *a, const st_pwl *b) {
    if (a == NULL || b == NULL) {
        fprintf(stderr, "pwl_equal: invalid argument (a=%p, b=%p)\n", (void*)a, (void*)b);
        return false;
    }

    bool equal = true;

    if (a->cnt != b->cnt) {
        fprintf(stderr, "pwl_equal: cnt mismatch (%zu != %zu)\n", a->cnt, b->cnt);
        equal = false;
    }
    if (a->slope_cnt != b->slope_cnt) {
        fprintf(stderr, "pwl_equal: slope_cnt mismatch (%zu != %zu)\n", a->slope_cnt, b->slope_cnt);
        equal = false;
    }
    if (a->bpp_x != b->bpp_x) {
        fprintf(stderr, "pwl_equal: bpp_x mismatch (%u != %u)\n", a->bpp_x, b->bpp_x);
        equal = false;
    }
    if (a->bpp_y != b->bpp_y) {
        fprintf(stderr, "pwl_equal: bpp_y mismatch (%u != %u)\n", a->bpp_y, b->bpp_y);
        equal = false;
    }

    if (!equal) return false;

    for (size_t i = 0; i < a->cnt; i++) {
        if (a->dcmp_x[i] != b->dcmp_x[i]) {
            fprintf(stderr, "pwl_equal: dcmp_x[%zu] mismatch (%u != %u)\n", i, a->dcmp_x[i], b->dcmp_x[i]);
            equal = false;
        }
        if (a->dcmp_y[i] != b->dcmp_y[i]) {
            fprintf(stderr, "pwl_equal: dcmp_y[%zu] mismatch (%u != %u)\n", i, a->dcmp_y[i], b->dcmp_y[i]);
            equal = false;
        }
    }

    for (size_t i = 0; i < a->slope_cnt; i++) {
        if (fabs(a->slopes[i] - b->slopes[i]) > 1e-9) {
            uint64_t a_bits, b_bits;
            memcpy(&a_bits, &a->slopes[i], sizeof(uint64_t));
            memcpy(&b_bits, &b->slopes[i], sizeof(uint64_t));
            fprintf(stderr, "pwl_equal: slopes[%zu] mismatch (0x%016lx != 0x%016lx)\n", i, a_bits, b_bits);
            equal = false;

        }
    }

    return equal;
}

void print_test_inputs(const double *inputs) {
    printf("test_inputs[%zu]:\n", test_inputs_cnt);
    for (size_t i = 0; i < test_inputs_cnt; i++) {
        printf("  [%2zu] %10f\n", i, inputs[i]);
    }
}

static inline void setup_test_pwl(st_pwl *pwl) {
    assert(init_pwl(pwl, KP_CNT) == DCMP_SUCCESS);
    for (size_t i = 0; i < KP_CNT; i++) {
        pwl->dcmp_x[i] = test_x[i];
        pwl->dcmp_y[i] = test_y[i];
    }
    pwl->bpp_x = 24;
    pwl->bpp_y = 12;
}

static inline void setup_test_pwl_with_slopes(st_pwl *pwl) {
    setup_test_pwl(pwl);
    for (size_t i = 0; i < SLP_CNT; i++) {
        pwl->slopes[i] = test_slopes[i];
    }
}

static inline void setup_test_pwl_clipped_with_slopes(st_pwl *pwl) {
    setup_test_pwl(pwl);
    for (size_t i = 0; i < KP_CNT; i++) {
        pwl->dcmp_y[i] = test_y_clipped[i];
    }
    for (size_t i = 0; i < SLP_CNT; i++) {
        pwl->slopes[i] = test_slopes_clipped[i];
    }
}

static inline void setup_test_pwl_rev_with_slopes(st_pwl *pwl) {
    assert(init_pwl(pwl, KP_CNT) == DCMP_SUCCESS);
    pwl->bpp_x = 12;
    pwl->bpp_y = 24;
    for (size_t i = 0; i < KP_CNT; i++) {
        pwl->dcmp_x[i] = test_x_rev[i];
        pwl->dcmp_y[i] = test_y_rev[i];
    }
    for (size_t i = 0; i < SLP_CNT; i++) {
        pwl->slopes[i] = test_slopes_rev[i];
    }
}

static inline void setup_test_pwl_rev_clipped_with_slopes(st_pwl *pwl) {
    assert(init_pwl(pwl, KP_CNT_REV_CLIPPED) == DCMP_SUCCESS);
    pwl->bpp_x = 12;
    pwl->bpp_y = 24;
    for (size_t i = 0; i < KP_CNT_REV_CLIPPED; i++) {
        pwl->dcmp_x[i] = test_x_rev_clipped[i];
        pwl->dcmp_y[i] = test_y_rev_clipped[i];
    }
    for (size_t i = 0; i < SLP_CNT_REV_CLIPPED; i++) {
        pwl->slopes[i] = test_slopes_rev_clipped[i];
    }
}

void test_pwl_print() {
    TEST_START("PWL PRINT");
    st_pwl pwl = {0};
    setup_test_pwl_with_slopes(&pwl);
    pwl.bpp_x = 23;
    pwl.bpp_y = 12;
    pwl_print(&pwl);
    free_pwl(&pwl);
}

void test_calc_slopes() {
    TEST_START("CALC SLOPES");
    st_pwl expected_pwl = {0};
    st_pwl test_pwl = {0};
    setup_test_pwl_with_slopes(&expected_pwl);
    setup_test_pwl(&test_pwl);
    calc_slopes(&test_pwl);

    for (size_t i = 0; i < SLP_CNT; i++) {
        // NOTE: Since we are calculating them the same way we expect them to be
        // bit for bit identical!
        assert(test_pwl.slopes[i] == expected_pwl.slopes[i]);
    }

    free_pwl(&expected_pwl);
    free_pwl(&test_pwl);
}

void test_pwl_reverse() {
    TEST_START("PWL REVERSE");
    st_pwl normal = {0};
    st_pwl reversed = {0};
    st_pwl expected = {0};

    setup_test_pwl_with_slopes(&normal);
    setup_test_pwl_with_slopes(&reversed);

    pwl_reverse(&normal, &reversed);

    pwl_print(&normal);
    pwl_print(&reversed);

    setup_test_pwl_rev_with_slopes(&expected);
    assert(pwl_equal(&reversed, &expected));

    free_pwl(&normal);
    free_pwl(&reversed);
    free_pwl(&expected);
}

void test_pwl_reverse_in_place() {
    TEST_START("PWL REVERSE IN PLACE");
    st_pwl pwl = {0};
    st_pwl expected = {0};

    setup_test_pwl_with_slopes(&pwl);
    setup_test_pwl_rev_with_slopes(&expected);


    pwl_reverse(&pwl, &pwl);
    pwl_print(&pwl);

    assert(pwl_equal(&pwl, &expected));

    free_pwl(&pwl);
    free_pwl(&expected);
}

void test_pwl_reverse_clipped() {
    TEST_START("PWL REVERSE CLIPPED");
    st_pwl normal = {0};
    st_pwl reversed = {0};
    st_pwl expected = {0};
    setup_test_pwl_clipped_with_slopes(&normal);
    setup_test_pwl_clipped_with_slopes(&reversed);
    setup_test_pwl_rev_clipped_with_slopes(&expected);

    pwl_reverse(&normal, &reversed);

    pwl_print(&normal);
    pwl_print(&reversed);

    assert(pwl_equal(&reversed, &expected));

    free_pwl(&normal);
    free_pwl(&reversed);
    free_pwl(&expected);
}

void test_comp_decomp() {
    TEST_START("COMP DECOMP");
    st_pwl comp_pwl = {0};
    st_pwl decomp_pwl = {0};
    double companded[test_inputs_cnt];
    double decompanded[test_inputs_cnt];
    setup_test_pwl_with_slopes(&comp_pwl);

    pwl_reverse(&comp_pwl, &decomp_pwl);

    comp_decomp(test_inputs, &comp_pwl, companded, test_inputs_cnt);
    comp_decomp(companded, &decomp_pwl, decompanded, test_inputs_cnt);

    print_test_inputs(test_inputs);
    print_test_inputs(decompanded);

    for (size_t i = 0; i < KP_CNT; i++) {
        assert(decompanded[i] == (double)test_inputs[i]);
    }

    free_pwl(&comp_pwl);
    free_pwl(&decomp_pwl);
}

void test_comp_decomp_clipped() {
    TEST_START("COMP DECOMP CLIPPED");
    st_pwl comp_pwl = {0};
    st_pwl decomp_pwl = {0};
    double companded[test_inputs_cnt];
    double decompanded[test_inputs_cnt];
    setup_test_pwl_clipped_with_slopes(&comp_pwl);
    pwl_reverse(&comp_pwl, &decomp_pwl);

    comp_decomp(test_inputs, &comp_pwl, companded, test_inputs_cnt);
    comp_decomp(companded, &decomp_pwl, decompanded, test_inputs_cnt);

    pwl_print(&comp_pwl);
    pwl_print(&decomp_pwl);
    print_test_inputs(test_inputs);
    print_test_inputs(companded);
    print_test_inputs(decompanded);

    assert(decompanded[0] == (double)test_inputs[0]); // 0
    assert(decompanded[1] == (double)test_inputs[1]); // 1048576

    // clipped keypoints must decompress to x_clip (4194304)
    assert(decompanded[2] == 4194304.0); // 4194304  -> clipped -> 4194304
    assert(decompanded[3] == 4194304.0); // 8388608  -> clipped -> 4194304
    assert(decompanded[4] == 4194304.0); // 16777215 -> clipped -> 4194304

    free_pwl(&comp_pwl);
    free_pwl(&decomp_pwl);
}

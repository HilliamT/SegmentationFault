#ifndef ARM11_13_LARGE_DOUBLE_H
#define ARM11_13_LARGE_DOUBLE_H

typedef struct LargeDouble {
    double mantissa;
    int exponent;
} LargeDouble;

int double_gt_zero(double number);
LargeDouble init_large_double(double value);
LargeDouble mult_large_double(LargeDouble ldub, double value);
LargeDouble apply_func(LargeDouble ldub, double (*func) (double));
int compare_large_doubles(LargeDouble ldub1, LargeDouble ldub2);
LargeDouble add_large_double(LargeDouble ldub1, LargeDouble ldub2);
LargeDouble subtract_large_double(LargeDouble ldub1, LargeDouble ldub2);

#endif //ARM11_13_LARGE_DOUBLE_H

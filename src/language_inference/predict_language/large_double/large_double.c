#include "large_double.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define BASE 10
#define ZERO_EPSILON 10e-32

/**
 * Returns if double is greater than set ZERO_EPSILON
 */
int double_gt_zero(double number) {
  return fabs(number) > ZERO_EPSILON;
}

/**
 * Returns if a LargeDouble is seen as zero by its mantissa being classified as too small
 * (<ZERO_EPSILON and its exponent 0).
 */
int is_zero(LargeDouble number) {
  return (!double_gt_zero(number.mantissa) && number.exponent == 0);
}

/**
 * Negate a LargeDouble
 */
LargeDouble negate(LargeDouble number) {
  number.mantissa = -number.mantissa;
  return number;
}

/**
 * Normalise a LargeDouble to standard form with mantissa between [1,10) if applicable
 */
LargeDouble normalise(LargeDouble number) {

  // Rule out non-positive, potentially zero, cases.
  if (is_zero(number)) return number; // zero
  if (number.mantissa < ZERO_EPSILON) return init_large_double(0); // too-small double representation
  if (number.mantissa < 0) return negate(normalise(negate(number))); // reuse normalise
  if (number.mantissa < BASE && number.mantissa >= 1) return number; // already standardised

  // Check to see if we need to raise or lower the mantissa
  int too_big = (number.mantissa >= BASE);

  while (number.mantissa >= BASE || number.mantissa < 1) {
    // Take a step towards normalising our LargeDouble
    number.mantissa *= ((too_big) ? 1.0/BASE : BASE);
    number.exponent += ((too_big) ? 1 : -1);
  }

  return number;
}

/**
 * Greaten or lower an exponent to a required value, updating its mantissa
 */
LargeDouble bring_to_exp(LargeDouble number, int exponent) {
  // Check to see if we need to raise or lower exponent
  int too_big = number.exponent > exponent;
  int step = too_big ? -1 : 1;

  for (; number.exponent != exponent; number.exponent += step) {
    // Take a step towards exponent, updating mantissa
    number.mantissa *= ((too_big) ? BASE : 1.0/BASE);
  }

  return number;
}

/**
 * Initialise a double as a LargeDouble
 */
LargeDouble init_large_double(double value) {
  LargeDouble ldub = (LargeDouble) {.mantissa = value, .exponent = 0};
  return normalise(ldub);
}

/**
 * Multiple a LargeDouble and a double together
 */
LargeDouble mult_large_double(LargeDouble ldub, double value) {

  // Rule out zero cases
  if (value < ZERO_EPSILON || ldub.mantissa * value < ZERO_EPSILON) return init_large_double(0);
  if (is_zero(ldub)) return init_large_double(0);

  // Multiply
  LargeDouble new_ldub = init_large_double(ldub.mantissa * value);
  new_ldub.exponent += ldub.exponent;
  return new_ldub;
}

/**
 * Apply a double -> double function onto a LargeDouble
 */
LargeDouble apply_func(LargeDouble ldub, double (*func) (double)) {
  ldub.mantissa = func(ldub.mantissa);
  return normalise(ldub);
}

/**
 * Compare two LargeDoubles
 */
int compare_large_doubles(LargeDouble ldub1, LargeDouble ldub2) {

  // If our exponents are the same, we can just compare their mantissas.
  if (ldub1.exponent == ldub2.exponent) {
    double m1 = ldub1.mantissa;
    double m2 = ldub2.mantissa;
    double diff = m1 - m2;
    return (diff > 0) ? 1 : -1;
  }

  // Otherwise, raise or lower their exponents to match
  ldub1 = bring_to_exp(ldub1, ldub2.exponent);
  return compare_large_doubles(ldub1, ldub2);
}

/**
 * Add two LargeDoubles
 */
LargeDouble add_large_double(LargeDouble ldub1, LargeDouble ldub2) {
  // Raise one's exponent to match the other.
  ldub1 = bring_to_exp(ldub1, ldub2.exponent);

  // Add mantissas and normalise
  LargeDouble result = init_large_double(ldub1.mantissa + ldub2.mantissa);
  result.exponent += ldub2.exponent;
  return normalise(result);
}

/**
 * Subtract two LargeDoubles
 */
LargeDouble subtract_large_double(LargeDouble ldub1, LargeDouble ldub2) {
  // Simply negate second operand and add
  ldub2.mantissa = -ldub2.mantissa;
  return add_large_double(ldub1, ldub2);
}

/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2013 Taketoshi Aono(brn)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#include "./scanner-test-prelude.h"

#define OPERATOR_TEST(op, name, token_type)                 \
  TEST(ScannerTest, ScanOperator_##name)                    \
  {                                                         \
    INIT(token, op);                                        \
    ASSERT_EQ(token->type(), yatsc::TokenKind::token_type); \
  }


OPERATOR_TEST(",", comma, kComma)
OPERATOR_TEST(".", dot, kDot)

OPERATOR_TEST("+", arithmetic_plus, kPlus)
OPERATOR_TEST("+=", arithmetic_add_let, kAddLet)
OPERATOR_TEST("++", arithmetic_increment, kIncrement)

OPERATOR_TEST("-", arithmetic_minus, kMinus)
OPERATOR_TEST("-=", arithmetic_sub_let, kSubLet)
OPERATOR_TEST("--", arithmetic_decrement, kDecrement)

OPERATOR_TEST("*", arithmetic_mul, kMul)
OPERATOR_TEST("*=", arithmetic_mul_let, kMulLet)

OPERATOR_TEST("/", arithmetic_div, kDiv)
OPERATOR_TEST("/=", arithmetic_div_let, kDivLet)

OPERATOR_TEST("%", arithmetic_mod, kMod)
OPERATOR_TEST("%=", arithmetic_mod_let, kModLet)

OPERATOR_TEST("~", arithmetic_nor, kBitNor)
OPERATOR_TEST("~=", arithmetic_nor_let, kNorLet)

OPERATOR_TEST("^", arithmetic_xor, kBitXor)
OPERATOR_TEST("^=", arithmetic_xor_let, kXorLet)

OPERATOR_TEST("&", bit_and, kBitAnd)
OPERATOR_TEST("&=", bit_and_let, kAndLet)
OPERATOR_TEST("&&", logical_and, kLogicalAnd)

OPERATOR_TEST("|", bit_or, kBitOr)
OPERATOR_TEST("|=", bit_or_let, kOrLet)
OPERATOR_TEST("||", logical_or, kLogicalOr)

OPERATOR_TEST("=", assign, kAssign)
OPERATOR_TEST("==", equal, kEqual)
OPERATOR_TEST("===", strict_equal, kEq)

OPERATOR_TEST("=>", arrow_glyph, kArrowGlyph)

OPERATOR_TEST("!", not, kNot)
OPERATOR_TEST("!=", not_equal, kNotEqual)
OPERATOR_TEST("!==", strict_not_equal, kNotEq)

OPERATOR_TEST("<", less, kLess)
OPERATOR_TEST("<=", less_equal, kLessEqual)
OPERATOR_TEST("<<", shift_left, kShiftLeft)
OPERATOR_TEST("<<=", shift_left_let, kShiftLeftLet)

OPERATOR_TEST(">", greater, kGreater)
OPERATOR_TEST(">=", greater_equal, kGreaterEqual)
OPERATOR_TEST(">>", shift_right, kShiftRight)
OPERATOR_TEST(">>>", u_shift_right, kUShiftRight)
OPERATOR_TEST(">>=", shift_right_let, kShiftRightLet)

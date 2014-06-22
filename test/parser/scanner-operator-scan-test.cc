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

#define OPERATOR_TEST(operator, name, token_type)     \
  TEST(ScannerTest, ScanOperator_##name)              \
  {                                                   \
   INIT(token, operator);                             \
   ASSERT_EQ(token->type(), yatsc::Token::token_type);  \
  }


OPERATOR_TEST("+", arithmetic_plus, TS_PLUS)
OPERATOR_TEST("+=", arithmetic_add_let, TS_ADD_LET)
OPERATOR_TEST("++", arithmetic_increment, TS_INCREMENT)

OPERATOR_TEST("-", arithmetic_minus, TS_MINUS)
OPERATOR_TEST("-=", arithmetic_sub_let, TS_SUB_LET)
OPERATOR_TEST("--", arithmetic_decrement, TS_DECREMENT)

OPERATOR_TEST("*", arithmetic_mul, TS_MUL)
OPERATOR_TEST("*=", arithmetic_mul_let, TS_MUL_LET)

OPERATOR_TEST("/", arithmetic_div, TS_DIV)
OPERATOR_TEST("/=", arithmetic_div_let, TS_DIV_LET)

OPERATOR_TEST("%", arithmetic_mod, TS_MOD)
OPERATOR_TEST("%=", arithmetic_mod_let, TS_MOD_LET)

OPERATOR_TEST("~", arithmetic_nor, TS_BIT_NOR)
OPERATOR_TEST("~=", arithmetic_nor_let, TS_NOR_LET)

OPERATOR_TEST("^", arithmetic_xor, TS_BIT_XOR)
OPERATOR_TEST("^=", arithmetic_xor_let, TS_XOR_LET)

OPERATOR_TEST("&", bit_and, TS_BIT_AND)
OPERATOR_TEST("&=", bit_and_let, TS_AND_LET)
OPERATOR_TEST("&&", logical_and, TS_LOGICAL_AND)

OPERATOR_TEST("|", bit_or, TS_BIT_OR)
OPERATOR_TEST("|=", bit_or_let, TS_OR_LET)
OPERATOR_TEST("||", logical_or, TS_LOGICAL_OR)

OPERATOR_TEST("=", assign, TS_ASSIGN)
OPERATOR_TEST("==", equal, TS_EQUAL)
OPERATOR_TEST("===", strict_equal, TS_EQ)

OPERATOR_TEST("=>", arrow_glyph, TS_ARROW_GLYPH)

OPERATOR_TEST("!", not, TS_NOT)
OPERATOR_TEST("!=", not_equal, TS_NOT_EQUAL)
OPERATOR_TEST("!==", strict_not_equal, TS_NOT_EQ)

OPERATOR_TEST("<", less, TS_LESS)
OPERATOR_TEST("<=", less_equal, TS_LESS_EQUAL)
OPERATOR_TEST("<<", shift_left, TS_SHIFT_LEFT)
OPERATOR_TEST("<<=", shift_left_let, TS_SHIFT_LEFT_LET)

OPERATOR_TEST(">", greater, TS_GREATER)
OPERATOR_TEST(">=", greater_equal, TS_GREATER_EQUAL)
OPERATOR_TEST(">>", shift_right, TS_SHIFT_RIGHT)
OPERATOR_TEST(">>>", u_shift_right, TS_U_SHIFT_RIGHT)
OPERATOR_TEST(">>=", shift_right_let, TS_SHIFT_RIGHT_LET)

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
   ASSERT_EQ(token->type(), rasp::Token::token_type);  \
  }


OPERATOR_TEST("+", arithmetic_plus, JS_PLUS)
OPERATOR_TEST("+=", arithmetic_add_let, JS_ADD_LET)
OPERATOR_TEST("++", arithmetic_increment, JS_INCREMENT)

OPERATOR_TEST("-", arithmetic_minus, JS_MINUS)
OPERATOR_TEST("-=", arithmetic_sub_let, JS_SUB_LET)
OPERATOR_TEST("--", arithmetic_decrement, JS_DECREMENT)

OPERATOR_TEST("*", arithmetic_mul, JS_MUL)
OPERATOR_TEST("*=", arithmetic_mul_let, JS_MUL_LET)

OPERATOR_TEST("/", arithmetic_div, JS_DIV)
OPERATOR_TEST("/=", arithmetic_div_let, JS_DIV_LET)

OPERATOR_TEST("%", arithmetic_mod, JS_MOD)
OPERATOR_TEST("%=", arithmetic_mod_let, JS_MOD_LET)

OPERATOR_TEST("~", arithmetic_nor, JS_BIT_NOR)
OPERATOR_TEST("~=", arithmetic_nor_let, JS_NOR_LET)

OPERATOR_TEST("^", arithmetic_xor, JS_BIT_XOR)
OPERATOR_TEST("^=", arithmetic_xor_let, JS_XOR_LET)

OPERATOR_TEST("&", bit_and, JS_BIT_AND)
OPERATOR_TEST("&=", bit_and_let, JS_AND_LET)
OPERATOR_TEST("&&", logical_and, JS_LOGICAL_AND)

OPERATOR_TEST("|", bit_or, JS_BIT_OR)
OPERATOR_TEST("|=", bit_or_let, JS_OR_LET)
OPERATOR_TEST("||", logical_or, JS_LOGICAL_OR)

OPERATOR_TEST("=", assign, JS_ASSIGN)
OPERATOR_TEST("==", equal, JS_EQUAL)
OPERATOR_TEST("===", strict_equal, JS_EQ)

OPERATOR_TEST("=>", arrow_glyph, JS_ARROW_GLYPH)

OPERATOR_TEST("!", not, JS_NOT)
OPERATOR_TEST("!=", not_equal, JS_NOT_EQUAL)
OPERATOR_TEST("!==", strict_not_equal, JS_NOT_EQ)

OPERATOR_TEST("<", less, JS_LESS)
OPERATOR_TEST("<=", less_equal, JS_LESS_EQUAL)
OPERATOR_TEST("<<", shift_left, JS_SHIFT_LEFT)
OPERATOR_TEST("<<=", shift_left_let, JS_SHIFT_LEFT_LET)

OPERATOR_TEST(">", greater, JS_GREATER)
OPERATOR_TEST(">=", greater_equal, JS_GREATER_EQUAL)
OPERATOR_TEST(">>", shift_right, JS_SHIFT_RIGHT)
OPERATOR_TEST(">>>", u_shift_right, JS_U_SHIFT_RIGHT)
OPERATOR_TEST(">>=", shift_right_let, JS_SHIFT_RIGHT_LET)

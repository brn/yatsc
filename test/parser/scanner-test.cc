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


#include <iostream>
#include "./scanner-test-prelude.h"


TEST(ScannerTest, ScanStringLiteralTest_normal) {
  INIT(token, "'test string'")
  ASSERT_EQ(token->type(), yatsc::TokenKind::kStringLiteral);
  ASSERT_STREQ(token->utf8_value(), "test string");
  END_SCAN;
}


TEST(ScannerTest, ScanStringLiteralTest_escaped_string) {
  INIT(token, "'test \\'string'")
  ASSERT_EQ(token->type(), yatsc::TokenKind::kStringLiteral);
  ASSERT_STREQ(token->utf8_value(), "test \\'string");
  END_SCAN;
}


TEST(ScannerTest, ScanStringLiteralTest_double_escaped_string) {
  INIT(token, "'test \\\\'string'")
  ASSERT_EQ(token->type(), yatsc::TokenKind::kStringLiteral);
  ASSERT_STREQ(token->utf8_value(), "test \\\\");
}


TEST(ScannerTest, ScanStringLiteralTest_unterminated_string) {
  INIT_ERROR(token, "'test")
}


TEST(ScannerTest, ScanStringLiteralTest_unicode_escaped_string) {
  INIT(token, "'\\u0061_foo_\\u0062_bar_\\u0063_baz'")
  ASSERT_EQ(token->type(), yatsc::TokenKind::kStringLiteral);
  ASSERT_STREQ(token->utf8_value(), "a_foo_b_bar_c_baz");
}


TEST(ScannerTest, ScanStringLiteralTest_unicode_escaped_string2) {
  INIT(token, "'\\uFEFF'");
  ASSERT_EQ(token->type(), yatsc::TokenKind::kStringLiteral);
  ASSERT_STREQ(token->utf8_value(), "\xEF\xBB\xBF");
}


TEST(ScannerTest, ScanStringLiteralTest_invalid_unicode_escaped_string) {
  INIT_ERROR(token, "'\\u006_foo_\\u0062_bar_\\u0063_baz'")
}


TEST(ScannerTest, ScanStringLiteralTest_invalid_unicode_escaped_string2) {
  INIT_ERROR(token, "'\\u0061_foo_\\u062_bar_\\u0063_baz'")
}


TEST(ScannerTest, ScanStringLiteralTest_invalid_unicode_escaped_string3) {
  INIT_ERROR(token, "'\\ux0061_foo_\\u0062_bar_\\u0-063_baz'")
}


TEST(ScannerTest, ScanStringLiteralTest_ascii_escaped_string) {
  INIT(token, "'\\x61_foo_\\x62_bar_\\x63_baz'")
  ASSERT_EQ(token->type(), yatsc::TokenKind::kStringLiteral);
  ASSERT_STREQ(token->utf8_value(), "a_foo_b_bar_c_baz");
}


TEST(ScannerTest, ScanStringLiteralTest_invalid_ascii_escaped_string) {
  INIT_ERROR(token, "'\\x6_foo_\\x62_bar_\\x63_baz'")
}


TEST(ScannerTest, ScanStringLiteralTest_invalid_ascii_escaped_string2) {
  INIT_ERROR(token, "'\\x61_foo_\\x2_bar_\\x63_baz'")
}


TEST(ScannerTest, ScanStringLiteralTest_invalid_ascii_escaped_string3) {
  INIT_ERROR(token, "'\\x61_foo_\\x62_bar_\\x-63_baz'")
}


TEST(ScannerTest, ScanDigit_double) {
  INIT(token, ".3032")
  ASSERT_EQ(token->type(), yatsc::TokenKind::kNumericLiteral);
  ASSERT_STREQ(token->utf8_value(), ".3032");
  END_SCAN;
}


TEST(ScannerTest, ScanDigit_double_illegal) {
  INIT_ERROR(token, ".30.32");
}


TEST(ScannerTest, ScanDigit_hex) {
  INIT(token, "0xFFCC33")
  ASSERT_EQ(token->type(), yatsc::TokenKind::kNumericLiteral);
  ASSERT_STREQ(token->utf8_value(), "0xFFCC33");
  END_SCAN;
}


TEST(ScannerTest, ScanDigit_int) {
  INIT(token, "1349075")
  ASSERT_EQ(token->type(), yatsc::TokenKind::kNumericLiteral);
  ASSERT_STREQ(token->utf8_value(), "1349075");
  END_SCAN;
}


TEST(ScannerTest, ScanDigit_double2) {
  INIT(token, "1349.075")
  ASSERT_EQ(token->type(), yatsc::TokenKind::kNumericLiteral);
  ASSERT_STREQ(token->utf8_value(), "1349.075");
  END_SCAN;
}


TEST(ScannerTest, ScanDigit_double2_illegal) {
  INIT_ERROR(token, "1349.07.5");
}


TEST(ScannerTest, ScanDigit_exponent) {
  INIT(token, "1349e+2")
  ASSERT_EQ(token->type(), yatsc::TokenKind::kNumericLiteral);
  ASSERT_STREQ(token->utf8_value(), "1349e+2");
  END_SCAN;
}


TEST(ScannerTest, ScanDigit_exponent2) {
  INIT(token, "1.3e+1")
  ASSERT_EQ(token->type(), yatsc::TokenKind::kNumericLiteral);
  ASSERT_STREQ(token->utf8_value(), "1.3e+1");
  END_SCAN;
}


TEST(ScannerTest, ScanDigit_exponent_illegal) {
  INIT_ERROR(token, "1.3ee");
}


TEST(ScannerTest, ScanDigit_exponent_illegal2) {
  INIT_ERROR(token, "1.3e+")
}


TEST(ScannerTest, ScanOcatalLiteral_valid) {
  INIT(token, "07771");
  ASSERT_EQ(token->type(), yatsc::TokenKind::kOctalLiteral);
  ASSERT_STREQ(token->utf8_value(), "07771");
  END_SCAN;
}


TEST(ScannerTest, ScanOcatalLiteral_invalid) {
  INIT_STRICT_ERROR(token, "07771");
}


TEST(ScannerTest, ScanBinaryLiteral_valid1) {
  const char* binary = "0o01111001";
  INIT_ES6(token, binary);
  ASSERT_EQ(token->type(), yatsc::TokenKind::kBinaryLiteral);
  ASSERT_STREQ(token->utf8_value(), binary);
  ASSERT_EQ(token->value()->utf8_length(), strlen(binary));
}


TEST(ScannerTest, ScanBinaryLiteral_valid2) {
  const char* binary = "0O01111001";
  INIT_ES6(token, binary);
  ASSERT_EQ(token->type(), yatsc::TokenKind::kBinaryLiteral);
  ASSERT_STREQ(token->utf8_value(), binary);
  ASSERT_EQ(token->value()->utf8_length(), strlen(binary));
}


TEST(ScannerTest, ScanBinaryLiteral_invalid1) {
  INIT_STRICT_ERROR(token, "0o0011101");
}


TEST(ScannerTest, ScanBinaryLiteral_invalid2) {
  INIT_ERROR(token, "0o0011101");
}


TEST(ScannerTest, ScanBinaryLiteral_invalid3) {
  INIT_ES6_ERROR(token, "0ox");
}


TEST(ScannerTest, ScanBinaryLiteral_invalid4) {
  INIT_ES6_ERROR(token, "0o2");
}


TEST(ScannerTest, ScanBinaryLiteral_invalid5) {
  INIT_ES6_ERROR(token, "0o!");
}


TEST(ScannerTest, ScanIdentifier_identifier) {
  INIT(token, "fooBarBaz");
  ASSERT_EQ(token->type(), yatsc::TokenKind::kIdentifier);
  ASSERT_STREQ(token->utf8_value(), "fooBarBaz");
  END_SCAN;
}


TEST(ScannerTest, ScanIdentifier_identifier2) {
  INIT(token, "$_$_foobar");
  ASSERT_EQ(token->type(), yatsc::TokenKind::kIdentifier);
  ASSERT_STREQ(token->utf8_value(), "$_$_foobar");
  END_SCAN;
}


TEST(ScannerTest, ScanIdentifier_identifier3) {
  INIT(token, "$_$_foobar333_4");
  ASSERT_EQ(token->type(), yatsc::TokenKind::kIdentifier);
  ASSERT_STREQ(token->utf8_value(), "$_$_foobar333_4");
  END_SCAN;
}


TEST(ScannerTest, ScanIdentifier_long_long_identifier) {
  const char* id = "Lopadotemachoselachogaleokranioleipsanodrimhypotrimmatosilphioparaomelitokatakechymenokichlepikossyphophattoperisteralektryonoptekephallioki";
  INIT(token, id);
  ASSERT_EQ(token->type(), yatsc::TokenKind::kIdentifier);
  ASSERT_STREQ(token->utf8_value(), id);
  END_SCAN;
}


TEST(ScannerTest, ScanIdentifier_identifier_unicode_escape) {
  INIT(token, "\\u0061\\u0062\\u0063");
  ASSERT_EQ(token->type(), yatsc::TokenKind::kIdentifier);
  ASSERT_STREQ(token->utf8_value(), "abc");
  END_SCAN;
}


TEST(ScannerTest, ScanIdentifier_identifier_unicode_escape_with_ascii) {
  INIT(token, "\\u0061_foo_\\u0062_bar_\\u0063_baz");
  ASSERT_EQ(token->type(), yatsc::TokenKind::kIdentifier);
  ASSERT_STREQ(token->utf8_value(), "a_foo_b_bar_c_baz");
  END_SCAN;
}


TEST(ScannerTest, ScanLineTerminator_line_terminator) {
  INIT(token, "aaa;");
  ASSERT_EQ(token->type(), yatsc::TokenKind::kIdentifier);
  ASSERT_TRUE(token->has_line_terminator_before_next());
  ASSERT_EQ(scanner.Scan()->type(), yatsc::TokenKind::kLineTerminator);
  END_SCAN;
}


TEST(ScannerTest, ScanLineTerminator_line_break) {
  INIT(token, "aaa\n");
  ASSERT_EQ(token->type(), yatsc::TokenKind::kIdentifier);
  ASSERT_TRUE(token->has_line_break_before_next());
  END_SCAN;
}


TEST(ScannerTest, ScanLineTerminator_line_terminator_with_space) {
  INIT(token, "aaa  ;");
  ASSERT_EQ(token->type(), yatsc::TokenKind::kIdentifier);
  ASSERT_TRUE(token->has_line_terminator_before_next());
  ASSERT_EQ(scanner.Scan()->type(), yatsc::TokenKind::kLineTerminator);
  END_SCAN;
}


TEST(ScannerTest, ScanLineTerminator_line_break_with_space) {
  INIT(token, "aaa  \n");
  ASSERT_EQ(token->type(), yatsc::TokenKind::kIdentifier);
  ASSERT_TRUE(token->has_line_break_before_next());
  END_SCAN;
}


TEST(ScannerTest, ScanPuncture_left_paren) {
  INIT(token, "(");
  ASSERT_EQ(token->type(), yatsc::TokenKind::kLeftParen);
  END_SCAN;
}


TEST(ScannerTest, ScanPuncture_right_paren) {
  INIT(token, ")");
  ASSERT_EQ(token->type(), yatsc::TokenKind::kRightParen);
  END_SCAN;
}


TEST(ScannerTest, ScanPuncture_left_brace) {
  INIT(token, "{");
  ASSERT_EQ(token->type(), yatsc::TokenKind::kLeftBrace);
  END_SCAN;
}


TEST(ScannerTest, ScanPuncture_right_brace) {
  INIT(token, "}");
  ASSERT_EQ(token->type(), yatsc::TokenKind::kRightBrace);
  END_SCAN;
}


TEST(ScannerTest, ScanPuncture_left_bracket) {
  INIT(token, "[");
  ASSERT_EQ(token->type(), yatsc::TokenKind::kLeftBracket);
  END_SCAN;
}


TEST(ScannerTest, ScanPuncture_right_bracket) {
  INIT(token, "]");
  ASSERT_EQ(token->type(), yatsc::TokenKind::kRightBracket);
  END_SCAN;
}


TEST(ScannerTest, ScanPuncture_colon) {
  INIT(token, ":");
  ASSERT_EQ(token->type(), yatsc::TokenKind::kColon);
  END_SCAN;
}


TEST(ScannerTest, ScanPuncture_question_mark) {
  INIT(token, "?");
  ASSERT_EQ(token->type(), yatsc::TokenKind::kQuestionMark);
  END_SCAN;
}


TEST(ScannerTest, SkipSingleLineComment) {
  const char* comment = "//abcdefghijklmnopqrstuvwxyz1234567890!@#$%^&*()-_|\\`~{}[]\"\':;/?.<>,";
  INIT(token, comment);
  ASSERT_STREQ("Eof", token->ToString().c_str());
  END_SCAN;
}


TEST(ScannerTest, SkipSingleLineComment_with_line_feed) {
  const char* comment = "foo//abcdefghijklmnopqrstuvwxyz1234567890!@#$%^&*()-_|\\`~{}[]\"\':;/?.<>,\naaa";
  INIT(token, comment);
  ASSERT_EQ(token->type(), yatsc::TokenKind::kIdentifier);
  ASSERT_STREQ(token->utf8_value(), "foo");
  token = scanner.Scan();
  ASSERT_STREQ(token->utf8_value(), "aaa");
  END_SCAN;
}


TEST(ScannerTest, SkipMultiLineComment) {
  const char* comment = "foo/*aaaaaaaa\nbbbbbbbbbb\ncccccccccccc\nddddddddddddd*/aaa";
  const char* comment_part = "/*aaaaaaaa\nbbbbbbbbbb\ncccccccccccc\nddddddddddddd*/";
  INIT(token, comment);
  ASSERT_EQ(token->type(), yatsc::TokenKind::kIdentifier);
  ASSERT_STREQ("foo", token->utf8_value());
  token = scanner.Scan();
  ASSERT_STREQ("aaa", token->utf8_value());
  const yatsc::UtfString* utf_string = token->comment();
  ASSERT_STREQ(comment_part, utf_string->utf8_value());
  ASSERT_EQ(4u, token->source_position().start_line_number());
  END_SCAN;
}


TEST(ScannerTest, SkipMultiLineComment_2) {
  const char* comment = "foo/*aaaaaaaa\r\nbbbbbbbbbb\r\ncccccccccccc\r\nddddddddddddd*/aaa";
  const char* comment_part = "/*aaaaaaaa\r\nbbbbbbbbbb\r\ncccccccccccc\r\nddddddddddddd*/";
  INIT(token, comment);
  ASSERT_EQ(token->type(), yatsc::TokenKind::kIdentifier);
  ASSERT_STREQ("foo", token->utf8_value());
  token = scanner.Scan();
  ASSERT_STREQ("aaa", token->utf8_value());
  const yatsc::UtfString* utf_string = token->comment();
  ASSERT_STREQ(comment_part, utf_string->utf8_value());
  ASSERT_EQ(4u, token->source_position().start_line_number());
  END_SCAN;
}


TEST(ScannerTest, GetLineSource) {
  setlocale(LC_ALL, "");
  const char* source = "for (var i = 0; i < 1000; i++####) {\nvar x = i;\nvar m = i + x;\n}\nconsole.log(x);\nconsole.log(m);";
  INIT(token, source);
  for (int i = 0; i < 14; i++) {
    token = scanner.Scan();
  }
}

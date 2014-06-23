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
  ASSERT_EQ(token->type(), yatsc::Token::TS_STRING_LITERAL);
  yatsc::Utf8Value utf8 = token->value().ToUtf8Value();
  ASSERT_STREQ(utf8.value(), "test string");
  END_SCAN;
}


TEST(ScannerTest, ScanStringLiteralTest_escaped_string) {
  INIT(token, "'test \\'string'")
  ASSERT_EQ(token->type(), yatsc::Token::TS_STRING_LITERAL);
  yatsc::Utf8Value utf8 = token->value().ToUtf8Value();
  ASSERT_STREQ(utf8.value(), "test \\'string");
  END_SCAN;
}


TEST(ScannerTest, ScanStringLiteralTest_double_escaped_string) {
  INIT(token, "'test \\\\'string'")
  ASSERT_EQ(token->type(), yatsc::Token::TS_STRING_LITERAL);
  yatsc::Utf8Value utf8 = token->value().ToUtf8Value();
  ASSERT_STREQ(utf8.value(), "test \\\\");
}


TEST(ScannerTest, ScanStringLiteralTest_unterminated_string) {
  INIT_THROW(token, "'test", yatsc::TokenException)
}


TEST(ScannerTest, ScanStringLiteralTest_unicode_escaped_string) {
  INIT(token, "'\\u0061_foo_\\u0062_bar_\\u0063_baz'")
  ASSERT_EQ(token->type(), yatsc::Token::TS_STRING_LITERAL);
  yatsc::Utf8Value utf8 = token->value().ToUtf8Value();
  ASSERT_STREQ(utf8.value(), "a_foo_b_bar_c_baz");
}


TEST(ScannerTest, ScanStringLiteralTest_invalid_unicode_escaped_string) {
  INIT_THROW(token, "'\\u006_foo_\\u0062_bar_\\u0063_baz'", yatsc::TokenException)
}


TEST(ScannerTest, ScanStringLiteralTest_invalid_unicode_escaped_string2) {
  INIT_THROW(token, "'\\u0061_foo_\\u062_bar_\\u0063_baz'", yatsc::TokenException)
}


TEST(ScannerTest, ScanStringLiteralTest_invalid_unicode_escaped_string3) {
  INIT_THROW(token, "'\\ux0061_foo_\\u0062_bar_\\u0-063_baz'", yatsc::TokenException)
}


TEST(ScannerTest, ScanStringLiteralTest_ascii_escaped_string) {
  INIT(token, "'\\x61_foo_\\x62_bar_\\x63_baz'")
  ASSERT_EQ(token->type(), yatsc::Token::TS_STRING_LITERAL);
  yatsc::Utf8Value utf8 = token->value().ToUtf8Value();
  ASSERT_STREQ(utf8.value(), "a_foo_b_bar_c_baz");
}


TEST(ScannerTest, ScanStringLiteralTest_invalid_ascii_escaped_string) {
  INIT_THROW(token, "'\\x6_foo_\\x62_bar_\\x63_baz'", yatsc::TokenException)
}


TEST(ScannerTest, ScanStringLiteralTest_invalid_ascii_escaped_string2) {
  INIT_THROW(token, "'\\x61_foo_\\x2_bar_\\x63_baz'", yatsc::TokenException)
}


TEST(ScannerTest, ScanStringLiteralTest_invalid_ascii_escaped_string3) {
  INIT_THROW(token, "'\\x61_foo_\\x62_bar_\\x-63_baz'", yatsc::TokenException)
}


TEST(ScannerTest, ScanDigit_double) {
  INIT(token, ".3032")
  ASSERT_EQ(token->type(), yatsc::Token::TS_NUMERIC_LITERAL);
  yatsc::Utf8Value utf8 = token->value().ToUtf8Value();
  ASSERT_STREQ(utf8.value(), ".3032");
  END_SCAN;
}


TEST(ScannerTest, ScanDigit_double_illegal) {
  INIT_THROW(token, ".30.32", yatsc::TokenException);
}


TEST(ScannerTest, ScanDigit_hex) {
  INIT(token, "0xFFCC33")
  ASSERT_EQ(token->type(), yatsc::Token::TS_NUMERIC_LITERAL);
  yatsc::Utf8Value utf8 = token->value().ToUtf8Value();
  ASSERT_STREQ(utf8.value(), "0xFFCC33");
  END_SCAN;
}


TEST(ScannerTest, ScanDigit_int) {
  INIT(token, "1349075")
  ASSERT_EQ(token->type(), yatsc::Token::TS_NUMERIC_LITERAL);
  yatsc::Utf8Value utf8 = token->value().ToUtf8Value();
  ASSERT_STREQ(utf8.value(), "1349075");
  END_SCAN;
}


TEST(ScannerTest, ScanDigit_double2) {
  INIT(token, "1349.075")
  ASSERT_EQ(token->type(), yatsc::Token::TS_NUMERIC_LITERAL);
  yatsc::Utf8Value utf8 = token->value().ToUtf8Value();
  ASSERT_STREQ(utf8.value(), "1349.075");
  END_SCAN;
}


TEST(ScannerTest, ScanDigit_double2_illegal) {
  INIT_THROW(token, "1349.07.5", yatsc::TokenException);
}


TEST(ScannerTest, ScanDigit_exponent) {
  INIT(token, "1349e+2")
  ASSERT_EQ(token->type(), yatsc::Token::TS_NUMERIC_LITERAL);
  yatsc::Utf8Value utf8 = token->value().ToUtf8Value();
  ASSERT_STREQ(utf8.value(), "1349e+2");
  END_SCAN;
}


TEST(ScannerTest, ScanDigit_exponent2) {
  INIT(token, "1.3e+1")
  ASSERT_EQ(token->type(), yatsc::Token::TS_NUMERIC_LITERAL);
  yatsc::Utf8Value utf8 = token->value().ToUtf8Value();
  ASSERT_STREQ(utf8.value(), "1.3e+1");
  END_SCAN;
}


TEST(ScannerTest, ScanDigit_exponent_illegal) {
  INIT_THROW(token, "1.3e1", yatsc::TokenException);
}


TEST(ScannerTest, ScanDigit_exponent_illegal2) {
  INIT_THROW(token, "1.3e+", yatsc::TokenException)
}


TEST(ScannerTest, ScanOcatalLiteral_valid) {
  INIT(token, "07771");
  ASSERT_EQ(token->type(), yatsc::Token::TS_OCTAL_LITERAL);
  yatsc::Utf8Value utf8 = token->value().ToUtf8Value();
  ASSERT_STREQ(utf8.value(), "07771");
  END_SCAN;
}


TEST(ScannerTest, ScanOcatalLiteral_invalid) {
  INIT_STRICT_THROW(token, "07771", yatsc::TokenException);
}


TEST(ScannerTest, ScanBinaryLiteral_valid1) {
  const char* binary = "0o01111001";
  INIT_HARMONY(token, binary);
  ASSERT_EQ(token->type(), yatsc::Token::TS_BINARY_LITERAL);
  yatsc::Utf8Value v = token->value().ToUtf8Value();
  ASSERT_STREQ(v.value(), binary);
  ASSERT_EQ(v.size(), strlen(binary));
}


TEST(ScannerTest, ScanBinaryLiteral_valid2) {
  const char* binary = "0O01111001";
  INIT_HARMONY(token, binary);
  ASSERT_EQ(token->type(), yatsc::Token::TS_BINARY_LITERAL);
  yatsc::Utf8Value v = token->value().ToUtf8Value();
  ASSERT_STREQ(v.value(), binary);
  ASSERT_EQ(v.size(), strlen(binary));
}


TEST(ScannerTest, ScanBinaryLiteral_invalid1) {
  INIT_STRICT_THROW(token, "0o0011101", yatsc::TokenException);
}


TEST(ScannerTest, ScanBinaryLiteral_invalid2) {
  INIT_THROW(token, "0o0011101", yatsc::TokenException);
}


TEST(ScannerTest, ScanBinaryLiteral_invalid3) {
  INIT_HARMONY_THROW(token, "0ox", yatsc::TokenException);
}


TEST(ScannerTest, ScanBinaryLiteral_invalid4) {
  INIT_HARMONY_THROW(token, "0o2", yatsc::TokenException);
}


TEST(ScannerTest, ScanBinaryLiteral_invalid5) {
  INIT_HARMONY_THROW(token, "0o!", yatsc::TokenException);
}


TEST(ScannerTest, ScanIdentifier_identifier) {
  INIT(token, "fooBarBaz");
  ASSERT_EQ(token->type(), yatsc::Token::TS_IDENTIFIER);
  yatsc::Utf8Value utf8 = token->value().ToUtf8Value();
  ASSERT_STREQ(utf8.value(), "fooBarBaz");
  END_SCAN;
}


TEST(ScannerTest, ScanIdentifier_identifier2) {
  INIT(token, "$_$_foobar");
  ASSERT_EQ(token->type(), yatsc::Token::TS_IDENTIFIER);
  yatsc::Utf8Value utf8 = token->value().ToUtf8Value();
  ASSERT_STREQ(utf8.value(), "$_$_foobar");
  END_SCAN;
}


TEST(ScannerTest, ScanIdentifier_identifier3) {
  INIT(token, "$_$_foobar333_4");
  ASSERT_EQ(token->type(), yatsc::Token::TS_IDENTIFIER);
  yatsc::Utf8Value utf8 = token->value().ToUtf8Value();
  ASSERT_STREQ(utf8.value(), "$_$_foobar333_4");
  END_SCAN;
}


TEST(ScannerTest, ScanIdentifier_long_long_identifier) {
  const char* id = "Lopadotemachoselachogaleokranioleipsanodrimhypotrimmatosilphioparaomelitokatakechymenokichlepikossyphophattoperisteralektryonoptekephallioki";
  INIT(token, id);
  ASSERT_EQ(token->type(), yatsc::Token::TS_IDENTIFIER);
  yatsc::Utf8Value utf8 = token->value().ToUtf8Value();
  ASSERT_STREQ(utf8.value(), id);
  END_SCAN;
}


TEST(ScannerTest, ScanIdentifier_identifier_unicode_escape) {
  INIT(token, "\\u0061\\u0062\\u0063");
  ASSERT_EQ(token->type(), yatsc::Token::TS_IDENTIFIER);
  yatsc::Utf8Value utf8 = token->value().ToUtf8Value();
  ASSERT_STREQ(utf8.value(), "abc");
  END_SCAN;
}


TEST(ScannerTest, ScanIdentifier_identifier_unicode_escape_with_ascii) {
  INIT(token, "\\u0061_foo_\\u0062_bar_\\u0063_baz");
  ASSERT_EQ(token->type(), yatsc::Token::TS_IDENTIFIER);
  yatsc::Utf8Value utf8 = token->value().ToUtf8Value();
  ASSERT_STREQ(utf8.value(), "a_foo_b_bar_c_baz");
  END_SCAN;
}


TEST(ScannerTest, ScanLineTerminator_line_terminator) {
  INIT(token, "aaa;");
  ASSERT_EQ(token->type(), yatsc::Token::TS_IDENTIFIER);
  ASSERT_TRUE(scanner.has_line_terminator_before_next());
  END_SCAN;
}


TEST(ScannerTest, ScanLineTerminator_line_break) {
  INIT(token, "aaa\n");
  ASSERT_EQ(token->type(), yatsc::Token::TS_IDENTIFIER);
  ASSERT_TRUE(scanner.has_line_terminator_before_next());
  END_SCAN;
}


TEST(ScannerTest, ScanLineTerminator_line_terminator_with_space) {
  INIT(token, "aaa  ;");
  ASSERT_EQ(token->type(), yatsc::Token::TS_IDENTIFIER);
  ASSERT_TRUE(scanner.has_line_terminator_before_next());
  END_SCAN;
}


TEST(ScannerTest, ScanLineTerminator_line_break_with_space) {
  INIT(token, "aaa  \n");
  ASSERT_EQ(token->type(), yatsc::Token::TS_IDENTIFIER);
  ASSERT_TRUE(scanner.has_line_terminator_before_next());
  END_SCAN;
}


TEST(ScannerTest, ScanPuncture_left_paren) {
  INIT(token, "(");
  ASSERT_EQ(token->type(), yatsc::Token::TS_LEFT_PAREN);
  END_SCAN;
}


TEST(ScannerTest, ScanPuncture_right_paren) {
  INIT(token, ")");
  ASSERT_EQ(token->type(), yatsc::Token::TS_RIGHT_PAREN);
  END_SCAN;
}


TEST(ScannerTest, ScanPuncture_left_brace) {
  INIT(token, "{");
  ASSERT_EQ(token->type(), yatsc::Token::TS_LEFT_BRACE);
  END_SCAN;
}


TEST(ScannerTest, ScanPuncture_right_brace) {
  INIT(token, "}");
  ASSERT_EQ(token->type(), yatsc::Token::TS_RIGHT_BRACE);
  END_SCAN;
}


TEST(ScannerTest, ScanPuncture_left_bracket) {
  INIT(token, "[");
  ASSERT_EQ(token->type(), yatsc::Token::TS_LEFT_BRACKET);
  END_SCAN;
}


TEST(ScannerTest, ScanPuncture_right_bracket) {
  INIT(token, "]");
  ASSERT_EQ(token->type(), yatsc::Token::TS_RIGHT_BRACKET);
  END_SCAN;
}


TEST(ScannerTest, ScanPuncture_colon) {
  INIT(token, ":");
  ASSERT_EQ(token->type(), yatsc::Token::TS_COLON);
  END_SCAN;
}


TEST(ScannerTest, ScanPuncture_question_mark) {
  INIT(token, "?");
  ASSERT_EQ(token->type(), yatsc::Token::TS_QUESTION_MARK);
  END_SCAN;
}


TEST(ScannerTest, SkipSingleLineComment) {
  const char* comment = "//abcdefghijklmnopqrstuvwxyz1234567890!@#$%^&*()-_|\\`~{}[]\"\':;/?.<>,";
  INIT(token, comment);
  ASSERT_STREQ("END_OF_INPUT", token->ToString());
  END_SCAN;
}


TEST(ScannerTest, SkipSingleLineComment_with_line_feed) {
  const char* comment = "foo//abcdefghijklmnopqrstuvwxyz1234567890!@#$%^&*()-_|\\`~{}[]\"\':;/?.<>,\naaa";
  INIT(token, comment);
  yatsc::Utf8Value utf8_value = token->value().ToUtf8Value();
  ASSERT_EQ(token->type(), yatsc::Token::TS_IDENTIFIER);
  ASSERT_STREQ(utf8_value.value(), "foo");
  token = scanner.Scan();
  utf8_value = token->value().ToUtf8Value();
  ASSERT_STREQ(utf8_value.value(), "aaa");
  END_SCAN;
}


TEST(ScannerTest, SkipMultiLineComment) {
  const char* comment = "foo/*aaaaaaaa\nbbbbbbbbbb\ncccccccccccc\nddddddddddddd*/aaa";
  const char* comment_part = "/*aaaaaaaa\nbbbbbbbbbb\ncccccccccccc\nddddddddddddd*/";
  INIT(token, comment);
  yatsc::Utf8Value utf8_value = token->value().ToUtf8Value();
  ASSERT_EQ(token->type(), yatsc::Token::TS_IDENTIFIER);
  ASSERT_STREQ("foo", utf8_value.value());
  yatsc::UtfString utf_string = scanner.last_multi_line_comment();
  ASSERT_STREQ(comment_part, utf_string.ToUtf8Value().value());
  token = scanner.Scan();
  utf8_value = token->value().ToUtf8Value();
  ASSERT_STREQ("aaa", utf8_value.value());
  ASSERT_EQ(4u, scanner.line_number());
  END_SCAN;
}


TEST(ScannerTest, SkipMultiLineComment_2) {
  const char* comment = "foo/*aaaaaaaa\r\nbbbbbbbbbb\r\ncccccccccccc\r\nddddddddddddd*/aaa";
  const char* comment_part = "/*aaaaaaaa\r\nbbbbbbbbbb\r\ncccccccccccc\r\nddddddddddddd*/";
  INIT(token, comment);
  yatsc::Utf8Value utf8_value = token->value().ToUtf8Value();
  ASSERT_EQ(token->type(), yatsc::Token::TS_IDENTIFIER);
  ASSERT_STREQ("foo", utf8_value.value());
  yatsc::UtfString utf_string = scanner.last_multi_line_comment();
  ASSERT_STREQ(comment_part, utf_string.ToUtf8Value().value());
  token = scanner.Scan();
  utf8_value = token->value().ToUtf8Value();
  ASSERT_STREQ("aaa", utf8_value.value());
  ASSERT_EQ(4u, scanner.line_number());
  END_SCAN;
}


TEST(ScannerTest, GetLineSource) {
  const char* source = "for (var i = 0; i < 1000; i++) {\nvar x = i;\nvar m = i + x;\n}\nconsole.log(x);\nconsole.log(m);";
  INIT(token, source);
  try {
  for (int i = 0; i < 14; i++) {
    token = scanner.Scan();
    std::cout << token->ToString() << " " << scanner.message() << " " << token->value().ToUtf8Value().value() << std::endl;
  }
  } catch(const yatsc::TokenException& e) {
    std::cout << e.what() << std::endl;
  }
  std::cout << scanner.GetLineSource(0, 3, 8) << std::endl;
}

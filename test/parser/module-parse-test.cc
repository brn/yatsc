// The MIT License (MIT)
// 
// Copyright (c) 2013 Taketoshi Aono(brn)
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.


#include "../gtest-header.h"
#include "../parser-util.h"


#define MODULE_TEST(type, code, expected_str)                           \
  PARSER_TEST("anonymous", ParseModule(), type, code, expected_str, false)

#define MODULE_THROW_TEST(type, code)                                 \
  PARSER_TEST("anonymous", ParseModule(), type, code, "", true)

#define MODULE_TEST_ALL(code, expected_str)                             \
  [&]{MODULE_TEST(yatsc::LanguageMode::ES3, code, expected_str);}();    \
  [&]{MODULE_TEST(yatsc::LanguageMode::ES5_STRICT, code, expected_str);}(); \
  [&]{MODULE_TEST(yatsc::LanguageMode::ES3, code, expected_str);}()

#define MODULE_THROW_TEST_ALL(code)                                 \
  [&]{MODULE_THROW_TEST(yatsc::LanguageMode::ES3, code);}();        \
  [&]{MODULE_THROW_TEST(yatsc::LanguageMode::ES5_STRICT, code);}(); \
  [&]{MODULE_THROW_TEST(yatsc::LanguageMode::ES6, code);}()



TEST(ModuleParser, ParseModule) {
  MODULE_TEST_ALL("import 'foo/bar/baz.js'",
                  "[FileScopeView]\n"
                  "  [ImportView]\n"
                  "    [Empty]\n"
                  "    [StringView]['foo/bar/baz.js']");


  MODULE_TEST_ALL("import a from 'foo/bar/baz.js'",
                  "[FileScopeView]\n"
                  "  [ImportView]\n"
                  "    [ImportClauseView]\n"
                  "      [NameView][a]\n"
                  "      [Empty]\n"
                  "    [StringView]['foo/bar/baz.js']");


  MODULE_TEST_ALL("import a, {b as c} from 'foo/bar/baz.js'",
                  "[FileScopeView]\n"
                  "  [ImportView]\n"
                  "    [ImportClauseView]\n"
                  "      [NameView][a]\n"
                  "      [NamedImportListView]\n"
                  "        [NamedImportView]\n"
                  "          [NameView][b]\n"
                  "          [NameView][c]\n"
                  "    [StringView]['foo/bar/baz.js']");


  MODULE_TEST_ALL("import foo, {bar as baz, qux, Foo as Bar} from 'foo/bar/baz.js'",
                  "[FileScopeView]\n"
                  "  [ImportView]\n"
                  "    [ImportClauseView]\n"
                  "      [NameView][foo]\n"
                  "      [NamedImportListView]\n"
                  "        [NamedImportView]\n"
                  "          [NameView][bar]\n"
                  "          [NameView][baz]\n"
                  "        [NameView][qux]\n"
                  "        [NamedImportView]\n"
                  "          [NameView][Foo]\n"
                  "          [NameView][Bar]\n"
                  "    [StringView]['foo/bar/baz.js']");


  MODULE_TEST_ALL("import {bar as baz, qux, Foo as Bar}, foo from 'foo/bar/baz.js'",
                  "[FileScopeView]\n"
                  "  [ImportView]\n"
                  "    [ImportClauseView]\n"
                  "      [NamedImportListView]\n"
                  "        [NamedImportView]\n"
                  "          [NameView][bar]\n"
                  "          [NameView][baz]\n"
                  "        [NameView][qux]\n"
                  "        [NamedImportView]\n"
                  "          [NameView][Foo]\n"
                  "          [NameView][Bar]\n"
                  "      [NameView][foo]\n"
                  "    [StringView]['foo/bar/baz.js']");


  MODULE_TEST_ALL("import {bar as baz, qux, Foo as Bar} from 'foo/bar/baz.js'",
                  "[FileScopeView]\n"
                  "  [ImportView]\n"
                  "    [ImportClauseView]\n"
                  "      [NamedImportListView]\n"
                  "        [NamedImportView]\n"
                  "          [NameView][bar]\n"
                  "          [NameView][baz]\n"
                  "        [NameView][qux]\n"
                  "        [NamedImportView]\n"
                  "          [NameView][Foo]\n"
                  "          [NameView][Bar]\n"
                  "      [Empty]\n"
                  "    [StringView]['foo/bar/baz.js']");


  MODULE_TEST_ALL("module Foo from 'foo/bar/baz.js'",
                  "[FileScopeView]\n"
                  "  [ModuleImportView]\n"
                  "    [NameView][Foo]\n"
                  "    [StringView]['foo/bar/baz.js']");

  
  MODULE_TEST_ALL("import Foo = require('foo/bar/baz')",
                  "[FileScopeView]\n"
                  "  [ImportView]\n"
                  "    [ImportClauseView]\n"
                  "      [NameView][Foo]\n"
                  "      [Empty]\n"
                  "    [ExternalModuleReference][foo/bar/baz]");
}


TEST(ModuleParser, ParseTSModule) {
  MODULE_TEST_ALL("module foo {}",
                  "[FileScopeView]\n"
                  "  [ModuleDeclView]\n"
                  "    [NameView][foo]\n"
                  "    [BlockView]");


  MODULE_TEST_ALL("module foo {"
                  "  module bar {"
                  "  }"
                  "  module baz {"
                  "  }"
                  "  module qux {"
                  "  }"
                  "}",
                  "[FileScopeView]\n"
                  "  [ModuleDeclView]\n"
                  "    [NameView][foo]\n"
                  "    [BlockView]\n"
                  "      [ModuleDeclView]\n"
                  "        [NameView][bar]\n"
                  "        [BlockView]\n"
                  "      [ModuleDeclView]\n"
                  "        [NameView][baz]\n"
                  "        [BlockView]\n"
                  "      [ModuleDeclView]\n"
                  "        [NameView][qux]\n"
                  "        [BlockView]");
}


TEST(ModuleParser, ParseExport) {
  MODULE_TEST_ALL("export var a = 0;",
                  "[FileScopeView]\n"
                  "  [ExportView]\n"
                  "    [VariableDeclView]\n"
                  "      [VariableView]\n"
                  "        [NameView][a]\n"
                  "        [NumberView][0]\n"
                  "        [Empty]\n"
                  "    [Empty]");


  MODULE_TEST_ALL("export * from 'foo/bar/baz.js'",
                  "[FileScopeView]\n"
                  "  [ExportView]\n"
                  "    [Empty]\n"
                  "    [StringView]['foo/bar/baz.js']");


  MODULE_TEST_ALL("export {x} from 'foo/bar/baz.js'",
                  "[FileScopeView]\n"
                  "  [ExportView]\n"
                  "    [NamedExportListView]\n"
                  "      [NamedExportView]\n"
                  "        [NameView][x]\n"
                  "        [Empty]\n"
                  "    [StringView]['foo/bar/baz.js']");


  MODULE_TEST_ALL("export {Foo as foo} from 'foo/bar/baz.js'",
                  "[FileScopeView]\n"
                  "  [ExportView]\n"
                  "    [NamedExportListView]\n"
                  "      [NamedExportView]\n"
                  "        [NameView][Foo]\n"
                  "        [NameView][foo]\n"
                  "    [StringView]['foo/bar/baz.js']");


  MODULE_TEST(yatsc::LanguageMode::ES6,
              "export let x = 0",
              "[FileScopeView]\n"
              "  [ExportView]\n"
              "    [LexicalDeclView][Let]\n"
              "      [VariableView]\n"
              "        [NameView][x]\n"
              "        [NumberView][0]\n"
              "        [Empty]\n"
              "    [Empty]");


  MODULE_TEST(yatsc::LanguageMode::ES6,
              "export const x = 0",
              "[FileScopeView]\n"
              "  [ExportView]\n"
              "    [LexicalDeclView][Const]\n"
              "      [VariableView]\n"
              "        [NameView][x]\n"
              "        [NumberView][0]\n"
              "        [Empty]\n"
              "    [Empty]");


  MODULE_TEST_ALL("export class Foo {}",
                  "[FileScopeView]\n"
                  "  [ExportView]\n"
                  "    [ClassDeclView]\n"
                  "      [NameView][Foo]\n"
                  "      [Empty]\n"
                  "      [ClassBasesView]\n"
                  "        [Empty]\n"
                  "        [Empty]\n"
                  "      [ClassFieldListView]\n"
                  "    [Empty]");


  MODULE_TEST_ALL("export interface Foo {}",
                  "[FileScopeView]\n"
                  "  [ExportView]\n"
                  "    [InterfaceView]\n"
                  "      [NameView][Foo]\n"
                  "      [Empty]\n"
                  "      [InterfaceExtendsView]\n"
                  "      [ObjectTypeExprView]\n"
                  "    [Empty]");


  MODULE_TEST_ALL("export function foo(){}",
                  "[FileScopeView]\n"
                  "  [ExportView]\n"
                  "    [FunctionView]\n"
                  "      [FunctionOverloadsView]\n"
                  "      [NameView][foo]\n"
                  "      [CallSignatureView]\n"
                  "        [ParamList]\n"
                  "        [Empty]\n"
                  "        [Empty]\n"
                  "      [BlockView]\n"
                  "    [Empty]");

  
  MODULE_TEST_ALL("export function *foo(){}",
                  "[FileScopeView]\n"
                  "  [ExportView]\n"
                  "    [FunctionView]\n"
                  "      [FunctionOverloadsView]\n"
                  "      [NameView][foo]\n"
                  "      [CallSignatureView]\n"
                  "        [ParamList]\n"
                  "        [Empty]\n"
                  "        [Empty]\n"
                  "      [BlockView]\n"
                  "    [Empty]");


  MODULE_TEST_ALL("export default Foo",
                  "[FileScopeView]\n"
                  "  [ExportView]\n"
                  "    [NameView][Foo]\n"
                  "    [Empty]");


  MODULE_TEST_ALL("export = Foo",
                  "[FileScopeView]\n"
                  "  [ExportView]\n"
                  "    [NameView][Foo]\n"
                  "    [Empty]");
}


TEST(ModuleParser, ParseArrayExport) {
  MODULE_TEST_ALL("export var a: A[] = ["
                  "  new Opcode(1, function(x: foo.bar, y: foo.bar2) {return this.value + x + y;}),"
                  "  new Opcode(1, function(x: foo.bar, y: foo.bar2) {return this.value + x + y;})"
                  "];",
                  "[FileScopeView]\n"
                  "  [ExportView]\n"
                  "    [VariableDeclView]\n"
                  "      [VariableView]\n"
                  "        [NameView][a]\n"
                  "        [ArrayLiteralView]\n"
                  "          [NewCallView]\n"
                  "            [NameView][Opcode]\n"
                  "            [ArgumentsView]\n"
                  "              [Empty]\n"
                  "              [CallArgsView]\n" 
                  "                [NumberView][1]\n"
                  "                [FunctionView]\n"
                  "                  [Empty]\n"
                  "                  [Empty]\n"
                  "                  [CallSignatureView]\n"
                  "                    [ParamList]\n"
                  "                      [ParameterView]\n"
                  "                        [NameView][x]\n"
                  "                        [Empty]\n"
                  "                        [SimpleTypeExprView]\n"
                  "                          [GetPropView]\n"
                  "                            [NameView][foo]\n"
                  "                            [NameView][bar]\n"
                  "                        [Empty]\n"
                  "                      [ParameterView]\n"
                  "                        [NameView][y]\n"
                  "                        [Empty]\n"
                  "                        [SimpleTypeExprView]\n"
                  "                          [GetPropView]\n"
                  "                            [NameView][foo]\n"
                  "                            [NameView][bar2]\n"
                  "                        [Empty]\n"
                  "                    [Empty]\n"
                  "                    [Empty]\n"
                  "                  [BlockView]\n"
                  "                    [ReturnStatementView]\n"
                  "                      [BinaryExprView][Plus]\n"
                  "                        [BinaryExprView][Plus]\n"
                  "                          [GetPropView]\n"
                  "                            [ThisView]\n"
                  "                            [NameView][value]\n"
                  "                          [NameView][x]\n"
                  "                        [NameView][y]\n"
                  "          [NewCallView]\n"
                  "            [NameView][Opcode]\n"
                  "            [ArgumentsView]\n"
                  "              [Empty]\n"
                  "              [CallArgsView]\n" 
                  "                [NumberView][1]\n"
                  "                [FunctionView]\n"
                  "                  [Empty]\n"
                  "                  [Empty]\n"
                  "                  [CallSignatureView]\n"
                  "                    [ParamList]\n"
                  "                      [ParameterView]\n"
                  "                        [NameView][x]\n"
                  "                        [Empty]\n"
                  "                        [SimpleTypeExprView]\n"
                  "                          [GetPropView]\n"
                  "                            [NameView][foo]\n"
                  "                            [NameView][bar]\n"
                  "                        [Empty]\n"
                  "                      [ParameterView]\n"
                  "                        [NameView][y]\n"
                  "                        [Empty]\n"
                  "                        [SimpleTypeExprView]\n"
                  "                          [GetPropView]\n"
                  "                            [NameView][foo]\n"
                  "                            [NameView][bar2]\n"
                  "                        [Empty]\n"
                  "                    [Empty]\n"
                  "                    [Empty]\n"
                  "                  [BlockView]\n"
                  "                    [ReturnStatementView]\n"
                  "                      [BinaryExprView][Plus]\n"
                  "                        [BinaryExprView][Plus]\n"
                  "                          [GetPropView]\n"
                  "                            [ThisView]\n"
                  "                            [NameView][value]\n"
                  "                          [NameView][x]\n"
                  "                        [NameView][y]\n"
                  "        [ArrayTypeExprView]\n"
                  "          [SimpleTypeExprView]\n"
                  "            [NameView][A]\n"
                  "    [Empty]");
}

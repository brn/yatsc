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

#include <gtest/gtest.h>
#include "../../src/ir/node.h"


class DummyView: public rasp::ir::Node {
 public:
  DummyView():
      Node(rasp::ir::NodeType::kUndefinedView, 0) {}
};


TEST(NodeTest, FileScopeView_ToFileScopeView_test) {
  rasp::ir::Node* node = new rasp::ir::FileScopeView();

  rasp::ir::FileScopeView* scope = node->ToFileScopeView();
  ASSERT_NE(scope, nullptr);
  delete scope;
}


// StatementView node
TEST(StatemetTest, StatementView_expr_test) {
  rasp::ir::StatementView statement;
  rasp::ir::Node* n = new DummyView();
  statement.set_expr(n);
  ASSERT_EQ(statement.expr(), n);
  delete n;
}


TEST(StatemetTest, StatementView_ToStatementView_test) {
  rasp::ir::Node* statement = new rasp::ir::StatementView();
  ASSERT_NE(statement->ToStatementView(), nullptr);
  delete statement;
}


// VariableDeclView node
TEST(VariableDeclView, VariableDeclView_InsertLast_test) {
  rasp::ir::VariableDeclView var_decl;
  const rasp::ir::Node::List& list = var_decl.node_list();
  rasp::ir::Node* n1 = new DummyView();
  rasp::ir::Node* n2 = new DummyView();
  rasp::ir::Node* n3 = new DummyView();
  
  var_decl.InsertLast(n1);
  var_decl.InsertLast(n2);
  var_decl.InsertLast(n3);

  ASSERT_EQ(list[0], n1);
  ASSERT_EQ(list[1], n2);
  ASSERT_EQ(list[2], n3);

  delete n1;
  delete n2;
  delete n3;
}


TEST(VariableDeclView, VariableDeclView_Remove_test) {
  rasp::ir::VariableDeclView var_decl;
  const rasp::ir::Node::List& list = var_decl.node_list();
  rasp::ir::Node* n1 = new DummyView();
  rasp::ir::Node* n2 = new DummyView();
  rasp::ir::Node* n3 = new DummyView();
  
  var_decl.InsertLast(n1);
  var_decl.InsertLast(n2);
  var_decl.InsertLast(n3);

  ASSERT_EQ(list[0], n1);
  ASSERT_EQ(list[1], n2);
  ASSERT_EQ(list[2], n3);

  var_decl.Remove(n1);
  ASSERT_EQ(list[0], n2);
  ASSERT_EQ(list[1], n3);

  var_decl.Remove(n2);
  ASSERT_EQ(list[0], n3);

  var_decl.Remove(n3);

  ASSERT_EQ(list.size(), 0);
}


TEST(VariableDeclView, VariableDeclView_ToVariableDeclView_test) {
  rasp::ir::Node* var_decl = new rasp::ir::VariableDeclView();
  ASSERT_NE(var_decl->ToVariableDeclView(), nullptr);
  delete var_decl;
}


// TrueView node
TEST(TrueViewTest, TrueView_ToTrueView_test) {
  rasp::ir::Node* t = new rasp::ir::TrueView();
  ASSERT_NE(t->ToTrueView(), nullptr);
  delete t;
}


// FalseView node
TEST(FalseViewTest, FalseView_ToFalseView_test) {
  rasp::ir::Node* f = new rasp::ir::FalseView();
  ASSERT_NE(f->ToFalseView(), nullptr);
  delete f;
}


TEST(BlockViewTest, BlockView_ToBlockView_test) {
  rasp::ir::Node* node = new rasp::ir::BlockView();

  rasp::ir::BlockView* scope = node->ToBlockView();
  ASSERT_NE(scope, nullptr);
  delete scope;
}


// ModuleDeclView node.
TEST(ModuleDeclViewTest, ModuleDeclView_set_name_test) {
  rasp::ir::ModuleDeclView md;
  rasp::ir::Node* n = new DummyNode();
  md.set_name(n);
  ASSERT_EQ(md.name(), n);
  delete n;
}


TEST(ModuleDeclViewTest, ModuleDeclView_ToModuleDeclView_test) {
  rasp::ir::Node* md = new rasp::ir::ModuleDeclView();
  ASSERT_NE(md->ToModuleDeclView(), nullptr);
  delete md;
}


TEST(ExportViewTest, ExportView_target_test) {
  rasp::ir::Node* n = new DummyView();
  rasp::ir::ExportView ex(n, true);
  ASSERT_EQ(ex.target(), n);
  delete n;
}


// ImportView node.
TEST(ImportViewTest, ImportView_alias_test) {
  rasp::ir::Node* n1 = new DummyView();
  rasp::ir::Node* n2 = new DummyView();
  rasp::ir::ImportView i(n1, n2);
  ASSERT_EQ(i.alias(), n1);
  ASSERT_EQ(i.from_expr(), n2);
  delete n1;
  delete n2;
}


TEST(ImportViewTest, ImportView_toImportView_test) {
  rasp::ir::Node* n1 = new DummyView();
  rasp::ir::Node* n2 = new DummyView();
  rasp::ir::Node* i = new rasp::ir::ImportView(n1, n2);

  ASSERT_NE(i->ToImportView(), nullptr);
  delete n1;
  delete n2;
  delete i;
}

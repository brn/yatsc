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
#include "../../src/utils/regions.h"
#include "../../src/ir/node.h"
#include "../../src/ir/irfactory.h"


class DummyView: public rasp::ir::Node {
 public:
  DummyView():
      Node(rasp::ir::NodeType::kUndefinedView, 0) {}
};


class NodeTest: public ::testing::Test {
 protected:
  static rasp::ir::IRFactory irfactory;
};

rasp::ir::IRFactory NodeTest::irfactory;

TEST_F(NodeTest, FileScopeView_ToFileScopeView_test) {
  rasp::ir::Node* node = irfactory.New<rasp::ir::FileScopeView>();

  rasp::ir::FileScopeView* scope = node->ToFileScopeView();
  ASSERT_NE(scope, nullptr);
}


// StatementView node
TEST_F(NodeTest, StatementView_expr_test) {
  rasp::ir::StatementView* statement = irfactory.New<rasp::ir::StatementView>();
  rasp::ir::Node* n = irfactory.New<DummyView>();
  statement->set_expr(n);
  ASSERT_EQ(statement->expr(), n);
}


TEST_F(NodeTest, StatementView_ToStatementView_test) {
  rasp::ir::Node* statement = irfactory.New<rasp::ir::StatementView>();
  ASSERT_NE(statement->ToStatementView(), nullptr);
}


// VariableDeclView node
TEST_F(NodeTest, VariableDeclView_InsertLast_test) {
  rasp::ir::VariableDeclView* var_decl = irfactory.New<rasp::ir::VariableDeclView>();
  const rasp::ir::Node::List& list = var_decl->node_list();
  rasp::ir::Node* n1 = irfactory.New<DummyView>();
  rasp::ir::Node* n2 = irfactory.New<DummyView>();
  rasp::ir::Node* n3 = irfactory.New<DummyView>();
  
  var_decl->InsertLast(n1);
  var_decl->InsertLast(n2);
  var_decl->InsertLast(n3);

  ASSERT_EQ(list[0], n1);
  ASSERT_EQ(list[1], n2);
  ASSERT_EQ(list[2], n3);
}


TEST_F(NodeTest, VariableDeclView_Remove_test) {
  rasp::ir::VariableDeclView* var_decl = irfactory.New<rasp::ir::VariableDeclView>();
  const rasp::ir::Node::List& list = var_decl->node_list();
  rasp::ir::Node* n1 = irfactory.New<DummyView>();
  rasp::ir::Node* n2 = irfactory.New<DummyView>();
  rasp::ir::Node* n3 = irfactory.New<DummyView>();
  
  var_decl->InsertLast(n1);
  var_decl->InsertLast(n2);
  var_decl->InsertLast(n3);

  ASSERT_EQ(list[0], n1);
  ASSERT_EQ(list[1], n2);
  ASSERT_EQ(list[2], n3);

  var_decl->Remove(n1);
  ASSERT_EQ(list[0], n2);
  ASSERT_EQ(list[1], n3);

  var_decl->Remove(n2);
  ASSERT_EQ(list[0], n3);

  var_decl->Remove(n3);

  ASSERT_EQ(list.size(), 0);
}


TEST_F(NodeTest, VariableDeclView_ToVariableDeclView_test) {
  rasp::ir::Node* var_decl = irfactory.New<rasp::ir::VariableDeclView>();
  ASSERT_NE(var_decl->ToVariableDeclView(), nullptr);
}


// TrueView node
TEST_F(NodeTest, TrueView_ToTrueView_test) {
  rasp::ir::Node* t = irfactory.New<rasp::ir::TrueView>();
  ASSERT_NE(t->ToTrueView(), nullptr);
}


// FalseView node
TEST_F(NodeTest, FalseView_ToFalseView_test) {
  rasp::ir::Node* f = irfactory.New<rasp::ir::FalseView>();
  ASSERT_NE(f->ToFalseView(), nullptr);
}


TEST_F(NodeTest, BlockView_ToBlockView_test) {
  rasp::ir::Node* node = irfactory.New<rasp::ir::BlockView>();
  rasp::ir::BlockView* scope = node->ToBlockView();
  ASSERT_NE(scope, nullptr);
}


// ModuleDeclView node.
TEST_F(NodeTest, ModuleDeclView_set_name_test) {
  rasp::ir::ModuleDeclView* md = irfactory.New<rasp::ir::ModuleDeclView>();
  rasp::ir::Node* n = irfactory.New<DummyView>();
  md->set_name(n);
  ASSERT_EQ(md->name(), n);
}


TEST_F(NodeTest, ModuleDeclView_ToModuleDeclView_test) {
  rasp::ir::Node* md = irfactory.New<rasp::ir::ModuleDeclView>();
  ASSERT_NE(md->ToModuleDeclView(), nullptr);
}


TEST_F(NodeTest, ExportView_target_test) {
  rasp::ir::Node* n = irfactory.New<DummyView>();
  rasp::ir::ExportView* ex = irfactory.New<rasp::ir::ExportView>(n);
  ASSERT_EQ(ex->target(), n);
}


// ImportView node.
TEST_F(NodeTest, ImportView_alias_test) {
  rasp::ir::Node* n1 = irfactory.New<DummyView>();
  rasp::ir::Node* n2 = irfactory.New<DummyView>();
  rasp::ir::ImportView* i = irfactory.New<rasp::ir::ImportView>(n1, n2);
  ASSERT_EQ(i->alias(), n1);
  ASSERT_EQ(i->from_expr(), n2);
}


TEST_F(NodeTest, ImportView_toImportView_test) {
  rasp::ir::Node* n1 = irfactory.New<DummyView>();
  rasp::ir::Node* n2 = irfactory.New<DummyView>();
  rasp::ir::Node* i = irfactory.New<rasp::ir::ImportView>(n1, n2);

  ASSERT_NE(i->ToImportView(), nullptr);
}

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


class DummyView: public yatsc::ir::Node {
 public:
  DummyView():
      Node(yatsc::ir::NodeType::kUndefinedView, 0) {}
};


class NodeTest: public ::testing::Test {
 protected:
  static yatsc::ir::IRFactory irfactory;
};

yatsc::ir::IRFactory NodeTest::irfactory;

TEST_F(NodeTest, FileScopeView_ToFileScopeView_test) {
  yatsc::ir::Node* node = irfactory.New<yatsc::ir::FileScopeView>();

  yatsc::ir::FileScopeView* scope = node->ToFileScopeView();
  ASSERT_NE(scope, nullptr);
}


// StatementView node
TEST_F(NodeTest, StatementView_expr_test) {
  yatsc::ir::StatementView* statement = irfactory.New<yatsc::ir::StatementView>();
  yatsc::ir::Node* n = irfactory.New<DummyView>();
  statement->set_expr(n);
  ASSERT_EQ(statement->expr(), n);

  yatsc::ir::Node* n2 = irfactory.New<DummyView>();
  yatsc::ir::StatementView* statement2 = irfactory.New<yatsc::ir::StatementView>(n2);
  ASSERT_EQ(statement2->expr(), n2);
}


TEST_F(NodeTest, StatementView_ToStatementView_test) {
  yatsc::ir::Node* statement = irfactory.New<yatsc::ir::StatementView>();
  ASSERT_NE(statement->ToStatementView(), nullptr);
}


// VariableDeclView node
TEST_F(NodeTest, VariableDeclView_InsertLast_test) {
  yatsc::ir::VariableDeclView* var_decl = irfactory.New<yatsc::ir::VariableDeclView>();
  const yatsc::ir::Node::List& list = var_decl->node_list();
  yatsc::ir::Node* n1 = irfactory.New<DummyView>();
  yatsc::ir::Node* n2 = irfactory.New<DummyView>();
  yatsc::ir::Node* n3 = irfactory.New<DummyView>();
  
  var_decl->InsertLast(n1);
  var_decl->InsertLast(n2);
  var_decl->InsertLast(n3);

  ASSERT_EQ(list[0], n1);
  ASSERT_EQ(list[1], n2);
  ASSERT_EQ(list[2], n3);
}


TEST_F(NodeTest, VariableDeclView_Remove_test) {
  yatsc::ir::VariableDeclView* var_decl = irfactory.New<yatsc::ir::VariableDeclView>();
  const yatsc::ir::Node::List& list = var_decl->node_list();
  yatsc::ir::Node* n1 = irfactory.New<DummyView>();
  yatsc::ir::Node* n2 = irfactory.New<DummyView>();
  yatsc::ir::Node* n3 = irfactory.New<DummyView>();
  
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
  yatsc::ir::Node* var_decl = irfactory.New<yatsc::ir::VariableDeclView>();
  ASSERT_NE(var_decl->ToVariableDeclView(), nullptr);
}


// TrueView node
TEST_F(NodeTest, TrueView_ToTrueView_test) {
  yatsc::ir::Node* t = irfactory.New<yatsc::ir::TrueView>();
  ASSERT_NE(t->ToTrueView(), nullptr);
}


// FalseView node
TEST_F(NodeTest, FalseView_ToFalseView_test) {
  yatsc::ir::Node* f = irfactory.New<yatsc::ir::FalseView>();
  ASSERT_NE(f->ToFalseView(), nullptr);
}


TEST_F(NodeTest, BlockView_ToBlockView_test) {
  yatsc::ir::Node* node = irfactory.New<yatsc::ir::BlockView>();
  yatsc::ir::BlockView* scope = node->ToBlockView();
  ASSERT_NE(scope, nullptr);
}


// ModuleDeclView node.
TEST_F(NodeTest, ModuleDeclView_set_name_test) {
  yatsc::ir::ModuleDeclView* md = irfactory.New<yatsc::ir::ModuleDeclView>();
  yatsc::ir::Node* n = irfactory.New<DummyView>();
  md->set_name(n);
  ASSERT_EQ(md->name(), n);
}


TEST_F(NodeTest, ModuleDeclView_set_body_test) {
  yatsc::ir::ModuleDeclView* md = irfactory.New<yatsc::ir::ModuleDeclView>();
  yatsc::ir::Node* n = irfactory.New<DummyView>();
  md->set_body(n);
  ASSERT_EQ(md->body(), n);

  md->set_body(nullptr);
  ASSERT_EQ(md->body(), nullptr);
}


TEST_F(NodeTest, ModuleDeclView_ToModuleDeclView_test) {
  yatsc::ir::Node* md = irfactory.New<yatsc::ir::ModuleDeclView>();
  ASSERT_NE(md->ToModuleDeclView(), nullptr);
}


TEST_F(NodeTest, ExportView_target_test) {
  yatsc::ir::Node* n = irfactory.New<DummyView>();
  yatsc::ir::ExportView* ex = irfactory.New<yatsc::ir::ExportView>(n);
  ASSERT_EQ(ex->target(), n);
}


// ImportView node.
TEST_F(NodeTest, ImportView_alias_test) {
  yatsc::ir::Node* n1 = irfactory.New<DummyView>();
  yatsc::ir::Node* n2 = irfactory.New<DummyView>();
  yatsc::ir::ImportView* i = irfactory.New<yatsc::ir::ImportView>(n1, n2);
  ASSERT_EQ(i->alias(), n1);
  ASSERT_EQ(i->from_expr(), n2);
}


TEST_F(NodeTest, ImportView_toImportView_test) {
  yatsc::ir::Node* n1 = irfactory.New<DummyView>();
  yatsc::ir::Node* n2 = irfactory.New<DummyView>();
  yatsc::ir::Node* i = irfactory.New<yatsc::ir::ImportView>(n1, n2);

  ASSERT_NE(i->ToImportView(), nullptr);
}


TEST_F(NodeTest, VariableView_test) {
  yatsc::ir::Node* name = irfactory.New<DummyView>();
  yatsc::ir::Node* value = irfactory.New<DummyView>();
  yatsc::ir::Node* type = irfactory.New<DummyView>();
  yatsc::ir::VariableView* vv = irfactory.New<yatsc::ir::VariableView>(name, value, type);
  ASSERT_EQ(vv->binding_identifier(), name);
  ASSERT_EQ(vv->value(), value);
  ASSERT_EQ(vv->type(), type);

  vv->set_binding_identifier(value);
  vv->set_value(type);
  vv->set_type(name);

  ASSERT_EQ(vv->binding_identifier(), value);
  ASSERT_EQ(vv->value(), type);
  ASSERT_EQ(vv->type(), name);
}


TEST_F(NodeTest, IfStatementView_test) {
  yatsc::ir::Node* then_block = irfactory.New<DummyView>();
  yatsc::ir::Node* else_block = irfactory.New<DummyView>();
  yatsc::ir::Node* expr = irfactory.New<DummyView>();
  yatsc::ir::IfStatementView* ifv = irfactory.New<yatsc::ir::IfStatementView>(expr, then_block, else_block);
  ASSERT_EQ(ifv->then_block(), then_block);
  ASSERT_EQ(ifv->else_block(), else_block);

  ifv->set_then_block(else_block);
  ifv->set_else_block(then_block);

  ASSERT_EQ(ifv->then_block(), else_block);
  ASSERT_EQ(ifv->else_block(), then_block);
}


TEST_F(NodeTest, ReturnStatementView_test) {
  yatsc::ir::Node* n = irfactory.New<DummyView>();
  yatsc::ir::ReturnStatementView* rn = irfactory.New<yatsc::ir::ReturnStatementView>(n);
  ASSERT_EQ(rn->expr(), n);
  rn->set_expr(nullptr);
  ASSERT_EQ(rn->expr() == nullptr, true);
}

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
#include "../../src/syntax/node.h"


class DummyNode: public rasp::syntax::Node {
 public:
  DummyNode():
      Node(rasp::syntax::NodeType::kUndefined) {}
};


// FileScope node.
TEST(NodeTest, FileScope_AddNode_test) {
  rasp::syntax::FileScope scope;
  rasp::syntax::Node* n1 = new DummyNode();
  rasp::syntax::Node* n2 = new DummyNode();
  rasp::syntax::Node* n3 = new DummyNode();
  scope.AddNode(n1);
  scope.AddNode(n2);
  scope.AddNode(n3);
  const rasp::syntax::Node::List& list = scope.children();
  ASSERT_EQ(list[0], n1);
  ASSERT_EQ(list[1], n2);
  ASSERT_EQ(list[2], n3);
  delete n1;
  delete n2;
  delete n3;
}


TEST(NodeTest, FileScope_RemoveNode_test) {
  rasp::syntax::FileScope scope;
  rasp::syntax::Node* n1 = new DummyNode();
  rasp::syntax::Node* n2 = new DummyNode();
  rasp::syntax::Node* n3 = new DummyNode();
  scope.AddNode(n1);
  scope.AddNode(n2);
  scope.AddNode(n3);
  const rasp::syntax::Node::List& list = scope.children();

  scope.RemoveNode(n1);
  ASSERT_EQ(list[0], n2);
  ASSERT_EQ(list[1], n3);

  scope.RemoveNode(n2);
  ASSERT_EQ(list[0], n3);

  scope.RemoveNode(n3);
  ASSERT_EQ(list.size(), 0);
  
  delete n1;
  delete n2;
  delete n3;
}


TEST(NodeTest, FileScope_ToFileScope_test) {
  rasp::syntax::Node* node = new rasp::syntax::FileScope();

  rasp::syntax::FileScope* scope = node->ToFileScope();
  ASSERT_NE(scope, nullptr);
  delete scope;
}


// Statement node
TEST(StatemetTest, Statement_exp_test) {
  rasp::syntax::Statement statement;
  rasp::syntax::Node* n = new DummyNode();
  statement.set_exp(n);
  ASSERT_EQ(statement.exp(), n);
  delete n;
}


TEST(StatemetTest, Statement_ToStatement_test) {
  rasp::syntax::Node* statement = new rasp::syntax::Statement();
  ASSERT_NE(statement->ToStatement(), nullptr);
  delete statement;
}


// VariableDecl node
TEST(VariableDecl, VariableDecl_AddDeclaration_test) {
  rasp::syntax::VariableDecl var_decl;
  const rasp::syntax::Node::List& list = var_decl.declarations();
  rasp::syntax::Node* n1 = new DummyNode();
  rasp::syntax::Node* n2 = new DummyNode();
  rasp::syntax::Node* n3 = new DummyNode();
  
  var_decl.AddDeclaration(n1);
  var_decl.AddDeclaration(n2);
  var_decl.AddDeclaration(n3);

  ASSERT_EQ(list[0], n1);
  ASSERT_EQ(list[1], n2);
  ASSERT_EQ(list[2], n3);

  delete n1;
  delete n2;
  delete n3;
}


TEST(VariableDecl, VariableDecl_RemoveDeclaration_test) {
  rasp::syntax::VariableDecl var_decl;
  const rasp::syntax::Node::List& list = var_decl.declarations();
  rasp::syntax::Node* n1 = new DummyNode();
  rasp::syntax::Node* n2 = new DummyNode();
  rasp::syntax::Node* n3 = new DummyNode();
  
  var_decl.AddDeclaration(n1);
  var_decl.AddDeclaration(n2);
  var_decl.AddDeclaration(n3);

  ASSERT_EQ(list[0], n1);
  ASSERT_EQ(list[1], n2);
  ASSERT_EQ(list[2], n3);

  var_decl.RemoveDeclaration(n1);
  ASSERT_EQ(list[0], n2);
  ASSERT_EQ(list[1], n3);

  var_decl.RemoveDeclaration(n2);
  ASSERT_EQ(list[0], n3);

  var_decl.RemoveDeclaration(n3);

  ASSERT_EQ(list.size(), 0);
}


TEST(VariableDecl, VariableDecl_ToVariableDecl_test) {
  rasp::syntax::Node* var_decl = new rasp::syntax::VariableDecl();
  ASSERT_NE(var_decl->ToVariableDecl(), nullptr);
  delete var_decl;
}


// True node
TEST(TrueTest, True_ToTrue_test) {
  rasp::syntax::Node* t = new rasp::syntax::True();
  ASSERT_NE(t->ToTrue(), nullptr);
  delete t;
}


// False node
TEST(FalseTest, False_ToFalse_test) {
  rasp::syntax::Node* f = new rasp::syntax::False();
  ASSERT_NE(f->ToFalse(), nullptr);
  delete f;
}


// Block node.
TEST(BlockTest, Block_AddNode_test) {
  rasp::syntax::Block scope;
  rasp::syntax::Node* n1 = new DummyNode();
  rasp::syntax::Node* n2 = new DummyNode();
  rasp::syntax::Node* n3 = new DummyNode();
  scope.AddNode(n1);
  scope.AddNode(n2);
  scope.AddNode(n3);
  const rasp::syntax::Node::List& list = scope.children();
  ASSERT_EQ(list[0], n1);
  ASSERT_EQ(list[1], n2);
  ASSERT_EQ(list[2], n3);
  delete n1;
  delete n2;
  delete n3;
}


TEST(BlockTest, Block_RemoveNode_test) {
  rasp::syntax::Block scope;
  rasp::syntax::Node* n1 = new DummyNode();
  rasp::syntax::Node* n2 = new DummyNode();
  rasp::syntax::Node* n3 = new DummyNode();
  scope.AddNode(n1);
  scope.AddNode(n2);
  scope.AddNode(n3);
  const rasp::syntax::Node::List& list = scope.children();

  scope.RemoveNode(n1);
  ASSERT_EQ(list[0], n2);
  ASSERT_EQ(list[1], n3);

  scope.RemoveNode(n2);
  ASSERT_EQ(list[0], n3);

  scope.RemoveNode(n3);
  ASSERT_EQ(list.size(), 0);
  
  delete n1;
  delete n2;
  delete n3;
}


TEST(BlockTest, Block_ToBlock_test) {
  rasp::syntax::Node* node = new rasp::syntax::Block();

  rasp::syntax::Block* scope = node->ToBlock();
  ASSERT_NE(scope, nullptr);
  delete scope;
}


// ModuleDecl node.
TEST(ModuleDeclTest, ModuleDecl_set_name_test) {
  rasp::syntax::ModuleDecl md;
  md.set_name("test");
  ASSERT_STREQ(md.name(), "test");
  md.set_name(std::string("test2"));
  ASSERT_STREQ(md.name(), "test2");
}


TEST(ModuleDeclTest, ModuleDecl_ToModuleDecl_test) {
  rasp::syntax::Node* md = new rasp::syntax::ModuleDecl();
  ASSERT_NE(md->ToModuleDecl(), nullptr);
  delete md;
}


// Export node.
TEST(ExportTest, Export_IsDirectExport_test) {
  rasp::syntax::Node* n = new DummyNode();
  rasp::syntax::Export ex(n, true);
  rasp::syntax::Export ex2(n, false);
  ASSERT_TRUE(ex.IsDirectExport());
  ASSERT_FALSE(ex2.IsDirectExport());
  delete n;
}


TEST(ExportTest, Export_target_test) {
  rasp::syntax::Node* n = new DummyNode();
  rasp::syntax::Export ex(n, true);
  ASSERT_EQ(ex.target(), n);
  delete n;
}


// Import node.
TEST(ImportTest, Import_alias_test) {
  rasp::syntax::Node* n1 = new DummyNode();
  rasp::syntax::Node* n2 = new DummyNode();
  rasp::syntax::Import i(n1, n2);
  ASSERT_EQ(i.alias(), n1);
  ASSERT_EQ(i.from_exp(), n2);
  delete n1;
  delete n2;
}


TEST(ImportTest, Import_toImport_test) {
  rasp::syntax::Node* n1 = new DummyNode();
  rasp::syntax::Node* n2 = new DummyNode();
  rasp::syntax::Node* i = new rasp::syntax::Import(n1, n2);

  ASSERT_NE(i->ToImport(), nullptr);
  delete n1;
  delete n2;
  delete i;
}

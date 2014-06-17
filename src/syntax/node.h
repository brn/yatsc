/*
 * The MIT License (MIT)
 * 
 * Copyright (c) Taketoshi Aono(brn)
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

#ifndef SYNTAX_NODE_H
#define SYNTAX_NODE_H


#include <algorithm>
#include <string>
#include <vector>
#include <stdint.h>
#include "../utils/utils.h"

namespace rasp {namespace syntax {
// Node types.
enum class NodeType: uint8_t {
  kFileScope = 0,
  kStatement,
  kVariableDecl,
  kTrue,
  kFalse,
  kBlock,
  kModuleDecl,
  kExport,
  kImport,
  kVariable,
  kIfStatement,
  kContinueStatement,
  kReturnStatement,
  kBreakStatement,
  kWithStatement,
  kLabelledStatement,
  kSwitchStatement,
  kCase,
  kTryStmt,
  kCatchStmt,
  kFinallyStmt,
  kThrowStmt,
  kFor,
  kForIn,
  kWhile,
  kDoWhile,
  kClass,
  kClassField,
  kFunction,
  kCallExp,
  kProperty,
  kNewExp,
  kPostfixExp,
  kUnaryExp,
  kBinaryExp,
  kCompareExp,
  kConditionalExp,
  kAssignmentExp,
  kUAObjectLiteral, //Destructuring Assignment Object
  kUAArrayLiteal, //Destructuring Assignment Array
  kArrayLiteral,
  kObjectLiteral,
  kGeneratorExpression,
  kDstaTree,
  kDstaExtractedExpressions,
  kUndefined
};


class FileScope;
class Statement;
class VariableDecl;
class True;
class False;
class Block;
class ModuleDecl;
class Export;
class Import;
class Variable;


// The AbstractSyntax Tree representation class.
// This class role as generic node for all ast.
// Asts are identified by NodeType and childrens.
class Node {
 public:

  typedef std::vector<Node*> List;
  typedef List::iterator Iterator;
  typedef std::string String;

  
  RASP_INLINE Node(NodeType type)
      : type_(type) {}

  
  RASP_INLINE virtual ~Node() {}

  
#define DEF_CAST(type)                                                  \
  RASP_INLINE type* To##type() RASP_NOEXCEPT {                          \
    return type_ == NodeType::k##type? reinterpret_cast<type*>(this): nullptr; \
  }
  
  // Define cast methods.
  DEF_CAST(FileScope);
  DEF_CAST(Statement);
  DEF_CAST(VariableDecl);
  DEF_CAST(True);
  DEF_CAST(False);
  DEF_CAST(Block);
  DEF_CAST(ModuleDecl);
  DEF_CAST(Export);
  DEF_CAST(Import);
  DEF_CAST(Variable);

#undef DEF_CAST

  
  /**
   * Return formated string expression of this node.
   * @returns The string expression of this node.
   */
  std::string ToString();

  /**
   * Return formated string expression of this node and children.
   * @returns The string expression of this node and children.
   */
  std::string ToStringTree();
  
 private:  
  NodeType type_;
};


// Represent file root of script.
class FileScope: public Node {
 public:
  FileScope(): Node(NodeType::kFileScope){};

  /**
   * Return children.
   * @return Children nodes.
   */
  RASP_INLINE const Node::List& children() RASP_NO_SE {
    return children_;
  }


  /**
   * Set child node.
   * @param block A node that will insert.
   */
  RASP_INLINE void AddNode(Node* block) {
    children_.push_back(block);
  }


  /**
   * Remove specified node from children.
   * @param block A node that want to erase from children.
   */
  RASP_INLINE void RemoveNode(Node* block) {
    children_.erase(std::remove(children_.begin(), children_.end(), block), children_.end());
  }

 private:
  Node::List children_;
};


// Represent statement.
class Statement : public Node {
 public:
  Statement(): Node(NodeType::kStatement){}


  /**
   * Return expression.
   * @returns An expression node.
   */
  RASP_INLINE Node* exp() RASP_NO_SE {
    return exp_;
  }


  /**
   * Set expression.
   * @param node An expression.
   */
  RASP_INLINE void set_exp(Node* node) {
    exp_ = node;
  }

 private:
  Node* exp_;
};


// Represent variable declarations.
class VariableDecl: public Node {
 public:
  VariableDecl(): Node(NodeType::kVariableDecl) {}


  /**
   * Return the variable declaration list.
   * @returns Teh variable declaration list.
   */
  RASP_INLINE const Node::List& declarations() RASP_NO_SE {
    return declarations_;
  }
  

  /**
   * Add a variable declaration to this node.
   * @param decl A variable declaration.
   */
  RASP_INLINE void AddDeclaration(Node* decl) {
    declarations_.push_back(decl);
  }


  /**
   * Remove a variable declaration from children.
   * @param node A node that will removed from children.
   */
  RASP_INLINE void RemoveDeclaration(Node* node) {
    declarations_.erase(std::remove(declarations_.begin(), declarations_.end(), node), declarations_.end());
  }
  

 private:
  Node::List declarations_;
};


class True: public Node {
 public:
  True(): Node(NodeType::kTrue) {}
};


class False: public Node {
 public:
  False(): Node(NodeType::kFalse) {}
};


// Represent block.
class Block: public Node {
 public:
  Block(): Node(NodeType::kBlock){};

  /**
   * Return children.
   * @return Children nodes.
   */
  RASP_INLINE const Node::List& children() RASP_NO_SE {
    return children_;
  }


  /**
   * Set child node.
   * @param block A node that will insert.
   */
  RASP_INLINE void AddNode(Node* block) {
    children_.push_back(block);
  }


  /**
   * Remove specified node from children.
   * @param block A node that want to erase from children.
   */
  RASP_INLINE void RemoveNode(Node* block) {
    children_.erase(std::remove(children_.begin(), children_.end(), block), children_.end());
  }

 protected:
  Block(NodeType type): Node(type) {}

 private:
  Node::List children_;
};


// Represent module.
class ModuleDecl: public Block {
 public:
  ModuleDecl(): Block(NodeType::kModuleDecl) {}

  /**
   * Return the name of this module.
   * @returns The name of this module.
   */
  RASP_INLINE const char* name() RASP_NOEXCEPT {
    return name_.c_str();
  }


  /**
   * Set the name of this module.
   * @param name The name of this module.
   */
  RASP_INLINE void set_name(const Node::String& name) {
    name_ = name;
  }


  /**
   * const char* version of the ModuleDecl::set_name
   * @param name The name of this module.
   */
  RASP_INLINE void set_name(const char* name) {
    name_ = name;
  }

 private:
  Node::String name_;
};


class Export: public Node {
 public:
  Export(Node* target, bool direct_export)
      : Node(NodeType::kExport),
        direct_export_(direct_export),
        target_(target) {}


  /**
   * Return true if export statement writed as direct export statemtn.
   * @return This export statement is direct export or not.
   */
  RASP_INLINE bool IsDirectExport() RASP_NO_SE {return direct_export_;}


  /**
   * Return rhs expression.
   * @returns A rhs node.
   */
  RASP_INLINE Node* target() RASP_NOEXCEPT {
    return target_;
  }

 private:
  bool direct_export_;
  Node* target_;
};


// Represent import.
class Import: public Node {
 public:
  Import(Node* alias, Node* from_exp)
      : Node(NodeType::kImport),
        alias_(alias),
        from_exp_(from_exp) {}


  /**
   * Return alias node.
   * @param An alias node.
   */
  RASP_INLINE Node* alias() RASP_NOEXCEPT {
    return alias_;
  }


  /**
   * Return expression node.
   * @returns expressions.
   */
  RASP_INLINE Node* from_exp() RASP_NOEXCEPT {
    return from_exp_;
  }
  
 private:
  Node* alias_;
  Node* from_exp_;
};


// Represent variable.
class Variable : public Node {
 public:
  Variable(Node* name, Node* value, Node* type):
      Node(NodeType::kVariable),
      name_(name),
      value_(value) {}


  /**
   * Return a value of the variable.
   * @returns A value node.
   */
  RASP_INLINE Node* value() RASP_NOEXCEPT {
    return value_;
  }


  /**
   * Return a name of the variable.
   * @returns A name of the variable.
   */
  RASP_INLINE Node* name() RASP_NOEXCEPT {
    return name_;
  }


  /**
   * Return type expression.
   * @returns A type expression.
   */
  RASP_INLINE Node* type() RASP_NOEXCEPT {
    return type_;
  }


 private:
  Node* name_;
  Node* value_;
};


// Represent if statement.
class IfStatement : public Node {
 public:
  IfStatement(Node* ifBlock, Node* elseBlock)
      : Node(NodeType::kIfStatement) {}

  
  /**
   * Return A then block.
   * @returns A then block node.
   */
  RASP_INLINE Node* then_block() RASP_NOEXCEPT {
    return then_block_;
  }


  /**
   * Set a then block.
   * @param then_block A then block node.
   */
  RASP_INLINE void set_then_block(Node* then_block) RASP_NOEXCEPT {
    then_block_ = then_block;
  }


  /**
   * Return else block.
   * @returns A else block node.
   */
  RASP_INLINE Node* else_block() RASP_NOEXCEPT {
    return else_block_;
  }


  /**
   * Set a else block.
   * @param else_block A else block node.
   */
  RASP_INLINE void set_else_block(Node* else_block) RASP_NOEXCEPT {
    else_block_ = else_block;
  }

 private:
  Node* then_block_;
  Node* else_block_;
};


// Represent continue statement.
class ContinueStatement: public Node {
 public:
  ContinueStatement()
      : Node(NodeType::kContinueStatement){}
};


// Represent return statement.
class ReturnStatement: public Node {
 public:
  ReturnStatement(Node* exp)
      : Node(NodeType::kReturnStatement),
        exp_(exp) {}


  /**
   * Return an expression node.
   * @returns An expression node.
   */
  RASP_INLINE Node* exp() RASP_NOEXCEPT {
    return exp_;
  }

 private:
  Node* exp_;
};


// Represent break statement.
class BreakStatement: public Node {
 public:
  BreakStatement(Node* label)
      : Node(NodeType::kBreakStatement),
        label_(label) {}

  BreakStatement()
      : Node(NodeType::kBreakStatement)
        label_(nullptr) {}


  /**
   * Return a label node.
   * @returns A label node.
   */
  RASP_INLINE Node* label() RASP_NOEXCEPT {
    return label_;
  }


  /**
   * Set a label node.
   * @param label A label node.
   */
  RASP_INLINE void set_label(Node* label) RASP_NOEXCEPT {
    label_ = label;
  }
  
 private:
  Node* label_;
};


// Represent with statement.
class WithStatement: public Block {
 public:
  WithStatement(Node* exp, Node* block)
      : Node(NodeType::kWithStatement),
        exp_(exp),
        block_(block) {}

  /**
   * Return an expression node.
   * @returns An expression node.
   */
  RASP_INLINE Node* exp() RASP_NOEXCEPT {
    return exp_;
  }


  /**
   * Set an expression node.
   * @param exp An expression node.
   */
  RASP_INLINE void set_exp(Node* exp) RASP_NOEXCEPT {
    exp_ = exp;
  }


 private:
  Node* exp_;
  Node* block_;
};

}} //rasp::syntax

#include "./node-inl.h"

#endif

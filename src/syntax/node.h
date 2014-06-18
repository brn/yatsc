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
  kTryStatement,
  kCatchStatement,
  kFinallyStatement,
  kThrowStatement,
  kForStatement,
  kForInStatement,
  kWhileStatement,
  kDoWhileStatement,
  kClassDecl,
  kInstanceProperty,
  kClassProperty,
  kInterface,
  kInterfaceField,
  kSimpleTypeExpression,
  kFunctionTypeExpression,
  kAccessorTypeExpression,
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
  kUndefined,
  kDebugger
};


enum class AccessLevel: uint8_t {
  kPublic = 0,
  kPrivate
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

#define NODE_GETTER(name)                                   \
  RASP_INLINE Node* name() RASP_NOEXCEPT {return name##_;}


#define NODE_SETTER(name)                       \
  RASP_INLINE void set_##name(Node* name) {name##_ = name;}


#define NODE_PROPERTY(name)                         \
  NODE_GETTER(name)                                 \
  NODE_SETTER(name)


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

  // Getter and Setter for exp.
  NODE_PROPERTY(exp);
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


// Statement holder base class.
// e.g ForStatement, WhileStatement...
class StatementHolder: public Node {
 public:
  
  // Getter and Setter for statement.
  NODE_PROPERTY(statement);

  
 protected:
  StatementHolder(NodeType type, Node* statement):
      Node(type),
      statement_(statement) {}

  StatementHolder(NodeType type):
      Node(type),
      statement_(nullptr) {}
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

  
  // Getter and Setter for then_block_.
  NODE_PROPERTY(then_block);
  
  // Getter and Setter for else_block_.
  NODE_PROPERTY(else_block);

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

  // Getter and Setter for exp_.
  NODE_PROPERTY(exp);

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


  // Getter and Setter for label_.
  NODE_PROPERTY(label);
  
 private:
  Node* label_;
};


// Represent with statement.
class WithStatement: public StatementHolder {
 public:
  WithStatement(Node* exp, Node* statement)
      : StatementHolder(NodeType::kWithStatement, statement),
        exp_(exp) {}


  WithStatement()
      : StatementHolder(NodeType::kWithStatement),
        exp_(nullptr) {}

  // Getter and Setter for exp_.
  NODE_PROPERTY(exp);


 private:
  Node* exp_;
};


// Represent labell.
class LabelledStatement: public StatementHolder {
 public:
  LabelledStatement(Node* name, Node* statement)
      : StatementHolder(NodeType::kLabelledStatement, statement),
        name_(name) {}


  LabelledStatement()
      : StatementHolder(NodeType::kLabelledStatement),
        name_(nullptr) {}


  // Getter and Setter for name_.
  NODE_PROPERTY(name);

 private:
  Node* name_;
};


// Represent switch statement.
class SwitchStatement: public Node {
 public:
  SwitchStatement()
      : Node(NodeType::kSwitchStatement) {}


  /**
   * Return the case list.
   * @returns The case list.
   */
  RASP_INLINE const Node::List& cases() RASP_NOEXCEPT {
    return cases_;
  }


  // Getter and Setter for default_case_.
  NODE_PROPERTY(default_case);


  /**
   * Add a case node.
   * @param case_node A case node.
   */
  RASP_INLINE void AddCase(Node* case_node) RASP_NOEXCEPT {
    cases_.push_back(case_node);
  }


  /**
   * Remove a case node.
   * @param case_node A case node.
   */
  RASP_INLINE void RemoveCase(Node* case_node) RASP_NOEXCEPT {
    cases_.erase(std::remove(cases_.begin(), cases_.end(), case_node), cases_.end());
  }

 private:
  Node::List cases_;
  Node* default_case_;
};


// Represent case.
class Case: public StatementHolder {
 public:
  Case(Node* condition, Node* statement)
      : StatementHolder(NodeType::kCase, statement),
        condition_(condition) {}


  Case()
      : StatementHolder(NodeType::kCase),
        condition_(nullptr) {}


  // Getter and Setter for condition_.
  NODE_PROPERTY(condition);


 private:
  Node* condition_;
};


// Represent try catch finally statement.
class TryStatement: public Block {
 public:
  TryStatement(Node* catch_statement, Node* finally_statement)
      : Block(NodeType::kTryStatement),
        catch_statement_(catch_statement),
        finally_statement_(finally_statement) {}

  
  TryStatement(Node* catch_statement)
      : Block(NodeType::kTryStatement),
        catch_statement_(catch_statement),
        finally_statement_(nullptr) {}


  TryStatement(Node* finally_statement)
      : Block(NodeType::kTryStatement),
        catch_statement_(nullptr),
        finally_statement_(finally_statement) {}


  // Getter and Setter for catch_statement_.
  NODE_PROPERTY(catch_statement);


  // Getter and Setter for finally_statement_.
  NODE_PROPERTY(finally_statement);


 private:
  Node* catch_statement_;
  Node* finally_statement_:
};


class CatchStatement: public Block {
 public:
  CatchStatement(Node* error_name)
      : Block(NodeType::kCatchStatement),
        error_name_(error_name){}


  // Getter and Setter for error_name.
  NODE_PROPERTY(error_name);

 private:
  Node* error_name_;
};


// Represent finally statement.
class FinallyStatement: public Block {
 public:
  FinallyStatement()
      : Block(NodeType::kFinallyStatement) {}
};


// Represent throw statement.
class ThrowStatement: public Node {
 public:
  ThrowStatement(Node* exp)
      : Node(NodeType::kThrowStatement),
        exp_(exp) {}

  
  // Getter and Setter for exp.
  NODE_PROPERTY(exp);
  

 private:
  Node* exp_;
};


class ForStatement: public StatementHolder {
 public:
  ForStatement(Node* cond_init, Node* cond_cmp, Node* cond_upd, Node* statement)
      : StatementHolder(NodeType::kForStatement, statement),
        cond_init_(cond_init),
        cond_cmp_(cond_cmp),
        cond_upd_(cond_upd),
        statement_(statement){}

  ForStatement()
      : StatementHolder(NodeType::kForStatement),
        cond_init_(nullptr),
        cond_cmp_(nullptr),
        cond_upd_(nullptr){}


  // Getter and Setter for cond_init.
  NODE_PROPERTY(cond_init);


  // Getter and Setter for cond_upd.
  NODE_PROPERTY(cond_upd);


  // Getter and Setter for cond_cmp.
  NODE_PROPERTY(cond_cmp);

 private:
  Node* cond_init_;
  Node* cond_cmp_;
  Node* cond_upd_;
};


// Represent for in statement.
class ForInStatement: public StatementHolder {
 public:
  ForInStatement(Node* property_name, Node* exp, Node* statement)
      : StatementHolder(NodeType::kForInStatement, statement),
        property_name_(property_name),
        exp_(exp) {}

  ForInStatement()
      : Node(NodeType::kForInStatement),
        property_name_(nullptr),
        exp_(nullptr) {}


  // Getter and Setter for cond_init.
  NODE_PROPERTY(property_name);


  // Getter and Setter for exp.
  NODE_PROPERTY(exp);


 private:
  Node* property_name_;
  Node* exp_;
  Node* statement_;
};


// Represent while statement
class WhileStatement: public StatementHolder {
 public:
  WhileStatement(Node* exp, Node* statement)
      : StatementHolder(NodeType::kWhileStatement, statement)
        exp_(exp) {}

  WhileStatement()
      : Node(NodeType::kWhileStatement),
        exp_(nullptr) {}


  // Getter and Setter for cond_init.
  NODE_PROPERTY(exp);
};


// Represent do while statement.
class DoWhileStatement: public StatementHolder {
 public:
  DoWhileStatement(Node* exp, Node* statement)
      : StatementHolder(NodeType::kDoWhileStatement, statement),
        exp_(exp) {}


  DoWhileStatement()
      : StatementHolder(NodeType::kDoWhileStatement),
        exp_(nullptr) {}


  // Getter and Setter for exp.
  NODE_PROPERTY(exp);

 private:
  Node* exp_;
};


class ClassDeclaration: public Node {
 public:
  ClassDeclaration(Node* name, Node* constructor, Node* inheritance)
      : Node(Node::kClassDecl),
        name_(name),
        inheritance_(inheritance),
        constructor_(constructor){}

  
  ClassDeclaration()
      : Node(Node::kClassDecl),
        name_(nullptr),
        inheritance_(nullptr),
        constructor(nullptr){}

  /**
   * Add a field node.
   * @param filed A filed node.
   */
  RASP_INLINE AddField(Node* field) RASP_NOEXCEPT {
    fields_.push_back(filed);
  }


  /**
   * Remove a filed node.
   * @param filed A field node.
   */
  RASP_INLINE RemoveField(Node* filed) RASP_NOEXCEPT {
    filelds_.erase(std::remove(fields_.begin(), fields_.end(), field), fields_.end());
  }

  
  /**
   * Return the filed list.
   * @returns Fileld list.
   */
  RASP_INLINE const Node::List& fields() RASP_NO_SE {
    return fields_;
  }


  /**
   * Add an implementation node.
   * @param impl An implementation node.
   */
  RASP_INLINE AddImplementation(Node* impl) RASP_NOEXCEPT {
    implementations_.push_back(impl);
  }


  /**
   * Remove an implementation node.
   * @param impl An implementation node.
   */
  RASP_INLINE RemoveImplementation(Node* impl) RASP_NOEXCEPT {
    implementations_.erase(std::remove(implementations_.begin(), implementations_.end(), impl), implementations_.end());
  }


  /**
   * Return the implementation list.
   * @returns The implementation list.
   */
  RASP_INLINE const Node::List& implementations() RASP_NO_SE {
    return implementations_;
  }
  

  // Getter and Setter for name.
  NODE_PROPERTY(name);

  
  // Getter and Setter for inheritance.
  NODE_PROPERTY(inheritance);


  // Getter and Setter for constructor.
  NODE_PROPERTY(constructor);

 private:
  Node* name_;
  Node* inheritance_;
  Node* constructor_;
  Node::List fields_;
  Node::List implementations_;
};


// Represent instance property.
class InstanceProperty: public Node {
 public:
  InstanceProperty(bool method, AccessLevel level, Node* name, Node* value)
      : Node(NodeType::kInstanceProperty),
        method_(method),
        access_level_(level),
        name_(name),
        value_(value) {}

  InstanceProperty(bool method)
      : Node(NodeType::kInstanceProperty),
        method_(method),
        access_level_(AccessLevel::kPublic),
        name_(nullptr),
        value_(nullptr) {}


  // Getter and Setter for access_level.
  NODE_PROPERTY(access_level);
  

  // Getter and Setter for name.
  NODE_PROPERTY(name);

  
  // Getter and Setter for value.
  NODE_PROPERTY(value);


  RASP_INLINE bool IsMethod() RASP_NO_SE {
    return method_;
  }


 private:
  bool method_;
  AccessLevel access_level_;
  Node* name_;
  Node* value_;
};


// Represent class property.
class ClassProperty: public Node {
  ClassProperty(bool method, Node* name, Node* value)
      : Node(NodeType::kClassProperty),
        method_(method),
        name_(name),
        value_(value) {}

  ClassProperty(bool method)
      : Node(NodeType::kClassProperty),
        method_(method),
        name_(nullptr),
        value_(nullptr) {}


  // Getter and Setter for name.
  NODE_PROPERTY(name);


  // Getter and Setter for value.
  NODE_PROPERTY(value);

  
  /**
   * Return whether this property is method or not.
   * @returns Method or not.
   */
  bool IsMethod() RASP_NO_SE {
    return method_;
  }

 private:
  bool method_;
  Node* name_;
  Node* value_;
};


// Represent interface.
class Interface: public Node {
 public:
  Interface(Node* name)
      : Node(NodeType::kInterface),
        name_(name) {}

  /**
   * Add a field node.
   * @param filed A filed node.
   */
  RASP_INLINE AddField(Node* field) RASP_NOEXCEPT {
    fields_.push_back(filed);
  }


  /**
   * Remove a filed node.
   * @param filed A field node.
   */
  RASP_INLINE RemoveField(Node* filed) RASP_NOEXCEPT {
    filelds_.erase(std::remove(fields_.begin(), fields_.end(), field), fields_.end());
  }

  
  /**
   * Return the filed list.
   * @returns Fileld list.
   */
  RASP_INLINE const Node::List& fields() RASP_NO_SE {
    return fields_;
  }


  /**
   * Add an extends node.
   * @param impl An extends node.
   */
  RASP_INLINE AddExtends(Node* extends) RASP_NOEXCEPT {
    extends_.push_back(extends);
  }


  /**
   * Remove an extends node.
   * @param impl An extends node.
   */
  RASP_INLINE RemoveExtends(Node* extends) RASP_NOEXCEPT {
    extends_.erase(std::remove(extends_.begin(), extends_.end(), extends), extends_.end());
  }


  /**
   * Return the extends list.
   * @returns The extends list.
   */
  RASP_INLINE const Node::List& extends() RASP_NO_SE {
    return extends_;
  }
  

  // Getter and Setter for name.
  NODE_PROPERTY(name);

 private:
  Node* name_;
  Node::List fields_;
  Node::List extends_;  
};


// Represent interface field.
class InterfaceField: public Node {
 public:
  InterfaceField(Node* name, Node* value):
      Node(NodeType::kInterfaceField),
      name_(name),
      value_(value){}


  InterfaceField():
      Node(NodeType::kInterfaceField),
      name_(nullptr),
      value_(nullptr){}

  
  // Getter and Setter for name.
  NODE_PROPERTY(name);
  

  // Getter and Setter for value.
  NODE_PROPERTY(value);

 private:
  Node* name_;
  Node* value_;
};


// Represent type expression like `var x: string`
class SimpleTypeExpression: public Node {
 public:
  
  SimpleTypeExpression(Node* type_name, boolean array = false)
      : Node(NodeType::kSimpleTypeExpression),
        array_(array),
        type_name_(type_name) {}


  /**
   * Return whether type expression represent array.
   * @returns Array or not.
   */
  RASP_INLINE bool IsArray() RASP_NO_SE {
    return array_;
  }
  

  // Getter and Setter for name.
  NODE_PROPERTY(name);

 private:
  bool array_;
  Node* name_;
};


// Represent function type expression like, `var x:(a:string, b:string) => string;`
class FunctionTypeExpression: public Node {
 public:
  FunctionTypeExpression(Node* param_list, Node* return_type)
      : Node(NodeType::kFunctionTypeExpression),
        param_list_(param_list),
        return_type_(return_type) {}


  FunctionTypeExpression()
      : Node(NodeType::kFunctionTypeExpression),
        param_list_(nullptr),
        return_type_(nullptr) {}


  // Getter and setter for param_list_.
  NODE_PROPERTY(param_list);
  

  // Getter and setter for return_type_.
  NODE_PROPERTY(return_type);
  

 private:
  Node* param_list_;
  Node* return_type_;
};


// Represent accessor type expression like, `interface x {[index:int]}`
class AccessorTypeExpression: public Node {
 public:
  AccessorTypeExpression(Node* name, Node* type_expression):
      Node(NodeType::kAccessorTypeExpression),
      name_(name) {}

  
  // Getter and setter for name_.
  NODE_PROPERTY(name);

 private:
  Node* name_;
};


// Represent function.
class Function: public Node {
 public:
  Function(bool declaration, Node* name, Node* param_list, Node* body)
      : Node(NodeType::kFunction),
        declaration_(declaration),
        name_(name),
        param_list_(param_list),
        body_(body) {}

  // Getter for declaration_.
  NODE_GETTER(declaration);

  // Getter for name_.
  NODE_GETTER(name);

  // Getter for param_list_.
  NODE_GETTER(param_list);

  // Getter for body_.
  NODE_GETTER(body);

 private:
  bool declaration_;
  Node* name_;
  Node* param_list_;
  Node* body_;
};


class Call: public Node {
 public:
  Call(Node* target, std::initializer_list<Node*> args)
      : Node(NodeType::kCall),
        target_(target),
        args_(args) {}


  /**
   * Return the arguments list.
   * @param The arguments list.
   */
  RASP_INLINE const Node::List& args() RASP_NO_SE {
    return args_;
  }


  /**
   * Add an argument.
   * @param arg An argument.
   */
  RASP_INLINE void AddArg(Node* arg) RASP_NOEXCEPT {
    arg_.push_back(arg);
  }


  /**
   * Remove an argument.
   * @param arg An argument.
   */
  RASP_INLINE void RemoveArg(Node* arg) RASP_NOEXCEPT {
    args_.erase(std::remove(args_.begin(), args_.end(), arg), args_.end());
  }
  
  // Getter for target_.
  NODE_GETTER(target);

 protected:
  Call(NodeType type, Node* target, std::initializer_list<Node*> args)
      : Node(type),
        target_(target),
        args_(args) {}

 private:
  Node* target_;
  Node::List args_;
};


class NewCall: public Call {
 public:
  NewCall(Node* target, std::initializer_list<Node*> args)
      : Call(NodeType::kNewCall, target, args) {}
};


class Name: public Node {
 public:
  Name(const TokeInfo& tokenInfo)
      : Node(NodeType::kName),
        token_info_(token_info) {}


  RASP_INLINE Utf8Value Utf8Value() RASP_NO_SE {
    return token_info_.value().ToUtf8Value();
  }


  RASP_INLINE Utf16Value Utf16Value() RASP_NO_SE {
    return token_info_.value().ToUtf16Value();
  }

 private:
  TokenInfo token_info_;
};


class GetProp: public Node {
 public:
  GetProp(Node* target, Node* prop)
      : Node(NodeType::kGetProp)
        target_(target),
        prop_(prop) {}

  // Getter and Setter for target_.
  NODE_PROPERTY(target);

  // Getter and Setter for prop_.
  NODE_PROPERTY(prop);

 protected:
  GetProp(NodeType type, Node* target, Node* prop)
      : Node(type),
        target_(target),
        prop_(prop) {}

 private:
  Node* target_;
  Node* prop_;
};


class GetElem: public GetProp {
 public:
  GetElem(Node* target, Node* prop)
      : GetProp(target, prop) {}
};


class Assignment: public Node {
 public:
  Assignment(Node* target, Node* exp)
      : Node(NodeType::kAssignment),
        target_(target),
        exp_(exp){}


  // Getter and Setter for target_.
  NODE_PROPERTY(target);


  // Getter and Setter for prop_.
  NODE_PROPERTY(exp);

 private:
  Node* target_;
  Node* prop_;
};


class TemaryExpression: public Node {
 public :
  TemaryExpression(Node* cond, Node* then_exp, Node* else_exp)
      : Node(NodeType::kTemaryExpression),
        cond_(cond),
        then_exp_(then_exp),
        else_exp_(else_exp) {}

  
  // Getter and Setter for then_exp_.
  NODE_PROPERTY(then_exp);

  
  // Getter and Setter for else_exp_.
  NODE_PROPERTY(else_exp);
  
 private:
  Node* then_exp_;
  Node* else_exp_;
};


class BinaryOpBase: public Node {
 public:
  BinaryOpBase(NodeType type, Node* first, Node* second)
      : Node(type),
        first_(first),
        second_(second) {}

  // Getter and Setter for first_.
  NODE_PROPERTY(frist);

  // Getter and Setter for second_.
  NODE_PROPERTY(second);
  
 private:
  Node* first_;
  Node* second_;
};


class UnaryOpBase: public Node {
 public:
  UnaryOpBase(NodeType type, Node* exp)
      : Node(type),
        exp_(exp) {}

  // Getter and Setter for exp_.
  NODE_PROPERTY(exp);
  
 private:
  Node* exp_;
};


class BinaryExpression: public BinaryOpBase {
 public:
  BinaryExpression(Token token, Node* first, Node* second)
      : BinaryOpBase(NodeType::kBinaryExpression, first, second),
        type_(type) {}


  // Getter and Setter for type_.
  NODE_PROPERTY(type);

 private:
  Token type_;
};


class UnaryExpression: public UnaryOpBase {
 public:
  BinaryExpression(Token token, Node* exp)
      : BinaryOpBase(NodeType::kUnaryExpression, exp),
        type_(type) {}


  // Getter and Setter for type_.
  NODE_PROPERTY(type);

 private:
  Token type_;
};


class CommaExpression: public BinaryOpBase {
 public:
  CommaExpression(Node* first, Node* second)
      : BinaryOpBase(NodeType::kCommaExpression, first, second) {}
};


class AndExpression: public BinaryOpBase {
 public:
  AndExpression(Node* first, Node* second)
      : BinaryOpBase(NodeType::kCommaExpression, first, second) {}
};


class OrExpression: public BinaryOpBase {
 public:
  OrExpression(Node* first, Node* second)
      : BinaryOpBase(NodeType::kOrExpression, first, second) {}
};


class NotExpression: public UnaryOpBase {
 public:
  NotExpression(Node* exp)
      : UnaryOpBase(NodeType::kNotExpression, exp) {}
};


class StrictEqExpression: public BinaryOpBase {
 public:
  BinaryOpBase(Node* first, Node* second)
      : BinaryOpBase(NodeType::kStrictEqExpression, first, second) {}
};


class ShallowEqExpression: public BinaryOpBase {
  public:
  BinaryOpBase(Node* first, Node* second)
      : BinaryOpBase(NodeType::kShallowEqExpression, first, second) {}
};


class Void: public UnaryOpBase {
 public:
  Void(Node* exp)
      : UnaryOpBase(NodeType::kVoid),
        exp_(exp) {}
};


class TypeCast: public UnaryOpBase {
 public:
  TypeCast(Node* exp)
      : UnaryOpBase(NodeType::kTypeCast),
        exp_(exp) {}
};


class AddExpression: public BinaryOpBase {
 public:
  AddExpression(Node* first, Node* second)
      : BinaryOpBase(NodeType::kAddExpression, first, second) {}
};


class SubExpression: public BinaryOpBase {
 public:
  SubExpression(Node* first, Node* second)
      : BinaryOpBase(NodeType::kSubExpression, first, second) {}
};


class This: public Node {
 public:
  This()
      : Node(NodeType::kThis) {}
};


class Number: public Node {
 public:
  Number(double value)
      : Node(NodeType::kNumber),
        value_(value_) {}

 private:
  double value_;
};


class Null: public Node {
 public:
  Null()
      : Node(NodeType::kNull) {}
};


class String: public Node {
 public:
  String(TokenInfo token_info)
      : token_info_(token_info) {}

  
  RASP_INLINE Utf8Value Utf8Value() RASP_NO_SE {
    return token_info_.value().ToUtf8Value();
  }


  RASP_INLINE Utf16Value Utf16Value() RASP_NO_SE {
    return token_info_.value().ToUtf16Value();
  }
  
  
 private:
  TokenInfo token_info_;
};


}} //rasp::syntax

#include "./node-inl.h"

#endif

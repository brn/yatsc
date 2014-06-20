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

#ifndef IR_NODE_H
#define IR_NODE_H


#include <algorithm>
#include <string>
#include <vector>
#include <stdint.h>
#include "../utils/utils.h"
#include "../utils/regions.h"
#include "../utils/environment.h"
#include "../parser/token.h"

namespace rasp {namespace ir {
// Node types.
enum class NodeType: uint8_t {
  kFileScopeView = 0,
  kStatementView,
  kVariableDeclView,
  kTrueView,
  kFalseView,
  kBlockView,
  kModuleDeclView,
  kExportView,
  kImportView,
  kVariableView,
  kIfStatementView,
  kContinueStatementView,
  kReturnStatementView,
  kBreakStatementView,
  kWithStatementView,
  kLabelledStatementView,
  kSwitchStatementView,
  kCaseListView,
  kCaseView,
  kTryStatementView,
  kCatchStatementView,
  kFinallyStatementView,
  kThrowStatementView,
  kForStatementView,
  kForInStatementView,
  kWhileStatementView,
  kDoWhileStatementView,
  kClassDeclView,
  kInstancePropertyView,
  kInstanceMethodView,
  kClassPropertyView,
  kClassMethodView,
  kClassFieldListView,
  kClassFieldAccessLevelView,
  kInterfaceView,
  kInterfaceFieldListView,
  kInterfaceFieldView,
  kSimpleTypeExprView,
  kArrayTypeExprView,
  kFunctionNodeTypeExprView,
  kAccessorTypeExprView,
  kFunctionView,
  kCallView,
  kCallArgsView,
  kNewCallView,
  kNameView,
  kGetPropView,
  kGetElemView,
  kAssignmentView,
  kTemaryExprView,
  kCastView,
  kBinaryExprView,
  kUnaryExprView,
  kThisView,
  kNumberView,
  kNullView,
  kStringView,
  kObjectElementView,
  kObjectLiteralView,
  kArrayLiteralView,
  kUAObjectLiteralView, //Destructuring AssignmentView Object
  kUAArrayLiteal, //Destructuring AssignmentView Array
  kUndefinedView,
  kDebuggerView
};


// Forward declarations.
class Node;
class FileScopeView;
class StatementView;
class VariableDeclView;
class TrueView;
class FalseView;
class BlockView;
class ModuleDeclView;
class ExportView;
class ImportView;
class VariableView;
class IfStatementView;
class ContinueStatementView;
class ReturnStatementView;
class BreakStatementView;
class WithStatementView;
class LabelledStatementView;
class SwitchStatementView;
class CaseView;
class TryStatementView;
class CatchStatementView;
class FinallyStatementView;
class ThrowStatementView;
class ForStatementView;
class ForInStatementView;
class WhileStatementView;
class DoWhileStatementView;
class ClassDeclView;
class ClassFieldListView;
class ClassFieldAccessLevelView;
class InstancePropertyView;
class InstanceMethodView;
class ClassPropertyView;
class ClassMethodView;
class InterfaceView;
class InterfaceFieldListView;
class InterfaceFieldView;
class SimpleTypeExprView;
class ArrayTypeExprView;
class FunctionNodeTypeExprView;
class AccessorTypeExprView;
class FunctionView;
class CallView;
class CallArgsView;
class NewCallView;
class NameView;
class GetPropView;
class GetElemView;
class AssignmentView;
class TemaryExprView;
class CastView;
class BinaryExprView;
class UnaryExprView;
class ThisView;
class NumberView;
class NullView;
class StringView;
class ObjectElementView;
class ObjectLiteralView;
class ArrayLiteralView;
class UndefinedView;
class DebuggerView;
// End forward declarations.


// Define getter accessor.
#define NODE_GETTER(name, pos)                                      \
  RASP_INLINE Node* name() RASP_NOEXCEPT {return node_list_[pos];}


// Define setter accessor.
#define NODE_SETTER(name, pos)                                      \
  RASP_INLINE void set_##name(Node* name) {                         \
    node_list_[pos] = name;                                         \
    if (name != nullptr) name->set_parent_node(this);               \
  }


// Define both getter and setter accessors.
#define NODE_PROPERTY(name, pos)                \
  NODE_GETTER(name, pos)                        \
  NODE_SETTER(name, pos)


class SourceInformation {
  friend class Node;
 public:
  SourceInformation(size_t line_number, size_t start_col)
      : line_number_(line_number),
        start_col_(start_col) {}


  SourceInformation(const SourceInformation& source_information)
      : line_number_(source_information.line_number_),
        start_col_(source_information.start_col_) {}


  SourceInformation(SourceInformation&& source_information) = delete;


  SourceInformation() = default;

  
  RASP_PROPERTY(size_t, line_number, line_number_);


  RASP_PROPERTY(size_t, start_col, start_col_);
  
 private:
  size_t line_number_;
  size_t start_col_;
};


// The IR Tree representation class.
// This class has all tree properties and accessible from Node type.
// All other **View classes are only view of this Node class.
class Node : public RegionalObject, private Uncopyable {
 public:

  typedef std::string String;
  typedef std::vector<Node*> List;
  typedef List::iterator ListIterator;


  /**
   * Create Node.
   * @param node_type The node type.
   * @param capacity The size of children tree.
   */
  RASP_INLINE Node(NodeType node_type, size_t capacity = 0)
      : node_type_(node_type),
        capacity_(capacity),
        parent_node_(nullptr),
        operand_(Token::ILLEGAL),
        double_value_(0l),
        environment_(nullptr){
    if (capacity != 0) {
      node_list_.reserve(capacity);
    }
    std::fill(node_list_.begin(), node_list_.end(), nullptr);
  }

  
  RASP_INLINE virtual ~Node() {}


  // Getter for node_type.
  RASP_GETTER(NodeType, node_type, node_type_);


  // Getter and setter for parent_node_.
  RASP_PROPERTY(Node*, parent_node, parent_node_);


  // Getter for children list.
  RASP_CONST_GETTER(const List&, node_list, node_list_);

  // Getter for environment.
  RASP_PROPERTY(Environment*, environment, environment_);


  /**
   * Insert a node at the end of the children.
   * @param node A node that want to insert.
   */
  RASP_INLINE void InsertLast(Node* node) {
    node_list_.push_back(node);
    node->set_parent_node(this);
  }


  /**
   * Insert a node to the front of the children.
   * @param node A node that want to insert.
   */
  RASP_INLINE void InsertFront(Node* node) {
    if (node_list_.size() > 0 ) {
      InsertBefore(node, node_list_[0]);
    } else {
      node_list_.push_back(node);
    }
    node->set_parent_node(this);
  }


  /**
   * Insert a new node to before specified node.
   * @param newNode A node that want to isnert.
   * @param oldNode A node taht has inserted.
   */
  RASP_INLINE void InsertBefore(Node* newNode, Node* oldNode) {
    ListIterator found = std::find(node_list_.begin(), node_list_.end(), oldNode);
    if (found != node_list_.end()) {
      node_list_.insert(found, newNode);
    }
    newNode->set_parent_node(this);
  }


  /**
   * Insert a new node after the specified node.
   * @param newNode A node that want to isnert.
   * @param oldNode A node taht has inserted.
   */
  RASP_INLINE void InsertAfter(Node* newNode, Node* oldNode) {
    ListIterator end = node_list_.end();
    ListIterator found = std::find(node_list_.begin(), end, oldNode);
    if (found != end && found + 1 != end) {
      node_list_.insert(found + 1, newNode);
    } else if (found != end) {
      node_list_.push_back(newNode);
    }
    newNode->set_parent_node(this);
  }


  /**
   * Set string value.
   * @param str String value.
   */
  RASP_INLINE void set_string_value(UtfString str) RASP_NOEXCEPT {
    string_value_ = std::move(str);
  }


  /**
   * Return string value.
   * @returns String value.
   */
  RASP_INLINE const UtfString& string_value() RASP_NO_SE {
    return string_value_;
  }


  /**
   * Set double value.
   * @param d Double value.
   */
  RASP_INLINE void set_double_value(double d) RASP_NOEXCEPT {
    double_value_ = d;
  }


  /**
   * Return double value.
   * @return Double value.
   */
  RASP_INLINE double double_value() RASP_NO_SE {
    return double_value_;
  }


  /**
   * Set an operand.
   * @param op An operand.
   */
  RASP_INLINE void set_operand(Token op) RASP_NOEXCEPT {
    operand_ = op;
  }


  /**
   * Return an operand.
   * @returns An operand.
   */
  RASP_INLINE Token operand() RASP_NO_SE {
    return operand_;
  }


  /**
   * Remove specified node from children.
   * @param block A node that want to erase from children.
   */
  RASP_INLINE void Remove(Node* block) {
    node_list_.erase(std::remove(node_list_.begin(), node_list_.end(), block), node_list_.end());
    block->set_parent_node(nullptr);
  }


  /**
   * Remove a specified iterator from children.
   * @param block An iterator that want to erase from children.
   */
  RASP_INLINE void Remove(Node::ListIterator iterator) {
    node_list_.erase(iterator);
    (*iterator)->set_parent_node(nullptr);
  }


  RASP_INLINE void SetInformationForNode(const TokenInfo& token_info) RASP_NOEXCEPT {
    source_information_.line_number_ = token_info.line_number();
    source_information_.start_col_ = token_info.start_col();
  }


  RASP_INLINE void SetInformationForTree(const TokenInfo& token_info) RASP_NOEXCEPT  {
    for (size_t i = 0u; i < node_list_.size(); i++) {
      Node* node = node_list_[i];
      if (node != nullptr) {
        node->SetInformationForNode(token_info);
        node->SetInformationForTree(token_info);
      }
    }
  }


  RASP_INLINE Node* Clone() RASP_NOEXCEPT {
    RASP_CHECK(true, environment_ != nullptr);
    Node* cloned = environment_->New<Node>(node_type_, capacity_);
    cloned->double_value_ = double_value_;
    cloned->string_value_ = string_value_;
    cloned->operand_ = operand_;
    cloned->environment_ = environment_;
    for (size_t i = 0u; i < node_list_.size(); i++) {
      Node* node = node_list_[i];
      if (node != nullptr) {
        Node* ret = node->Clone();
        cloned->node_list_.push_back(ret);
      } else {
        cloned->node_list_.push_back(nullptr);
      }
    }
    return cloned;
  }

  
#define DEF_CAST(type)                                                  \
  RASP_INLINE type* To##type() RASP_NOEXCEPT {                          \
    return node_type_ == NodeType::k##type? reinterpret_cast<type*>(this): nullptr; \
  }                                                                     \
  RASP_INLINE bool Has##type() RASP_NO_SE {                             \
    return node_type_ == NodeType::k##type;                             \
  }
  
  // Define cast methods.
  DEF_CAST(FileScopeView);
  DEF_CAST(StatementView);
  DEF_CAST(VariableDeclView);
  DEF_CAST(TrueView);
  DEF_CAST(FalseView);
  DEF_CAST(BlockView);
  DEF_CAST(ModuleDeclView);
  DEF_CAST(ExportView);
  DEF_CAST(ImportView);
  DEF_CAST(VariableView);
  DEF_CAST(IfStatementView);
  DEF_CAST(ContinueStatementView);
  DEF_CAST(ReturnStatementView);
  DEF_CAST(BreakStatementView);
  DEF_CAST(WithStatementView);
  DEF_CAST(LabelledStatementView);
  DEF_CAST(SwitchStatementView);
  DEF_CAST(CaseView);
  DEF_CAST(TryStatementView);
  DEF_CAST(CatchStatementView);
  DEF_CAST(FinallyStatementView);
  DEF_CAST(ThrowStatementView);
  DEF_CAST(ForStatementView);
  DEF_CAST(ForInStatementView);
  DEF_CAST(WhileStatementView);
  DEF_CAST(DoWhileStatementView);
  DEF_CAST(ClassDeclView);
  DEF_CAST(InstancePropertyView);
  DEF_CAST(InstanceMethodView);
  DEF_CAST(ClassFieldListView);
  DEF_CAST(ClassFieldAccessLevelView);
  DEF_CAST(ClassPropertyView);
  DEF_CAST(ClassMethodView);
  DEF_CAST(InterfaceView);
  DEF_CAST(InterfaceFieldListView);
  DEF_CAST(InterfaceFieldView);
  DEF_CAST(SimpleTypeExprView);
  DEF_CAST(ArrayTypeExprView);
  DEF_CAST(FunctionNodeTypeExprView);
  DEF_CAST(AccessorTypeExprView);
  DEF_CAST(FunctionView);
  DEF_CAST(CallView);
  DEF_CAST(CallArgsView);
  DEF_CAST(NewCallView);
  DEF_CAST(NameView);
  DEF_CAST(GetPropView);
  DEF_CAST(GetElemView);
  DEF_CAST(AssignmentView);
  DEF_CAST(TemaryExprView);
  DEF_CAST(CastView);
  DEF_CAST(BinaryExprView);
  DEF_CAST(UnaryExprView);
  DEF_CAST(ThisView);
  DEF_CAST(NumberView);
  DEF_CAST(NullView);
  DEF_CAST(StringView);
  DEF_CAST(ObjectElementView);
  DEF_CAST(ObjectLiteralView);
  DEF_CAST(ArrayLiteralView);
  DEF_CAST(UndefinedView);
  DEF_CAST(DebuggerView);

#undef DEF_CAST

  
  /**
   * Return formated string expression of this node.
   * @returns The string expression of this node.
   */
  Node::String ToString();

  /**
   * Return formated string expression of this node and children.
   * @returns The string expression of this node and children.
   */
  Node::String ToStringTree();

 protected:

  void InitNodeList(std::initializer_list<Node*> node_list) {
    node_list_.insert(node_list_.end(), node_list.begin(), node_list.end());
  }
  
  List node_list_;
  
 private:
  SourceInformation source_information_;
  NodeType node_type_;
  size_t capacity_;
  Node* parent_node_;
  Token operand_;
  double double_value_;
  UtfString string_value_;
  Environment* environment_;
};


// Represent block.
class BlockView: public Node {
 public:
  BlockView(): Node(NodeType::kBlockView){};
};


// Represent file root of script.
class FileScopeView: public Node {
 public:
  FileScopeView(): Node(NodeType::kFileScopeView, 1u){};

  
  FileScopeView(Node* statement): Node(NodeType::kFileScopeView, 1u){
    InsertLast(statement);
  };
};


// Represent statement.
class StatementView : public Node {
 public:
  StatementView(): Node(NodeType::kStatementView, 1u){}

  StatementView(Node* expr):
      Node(NodeType::kStatementView, 1u) {
    InsertLast(expr);
  }

  // Getter and Setter for exp.
  NODE_PROPERTY(expr, 0);
};


// Represent variable declarations.
class VariableDeclView: public Node {
 public:
  VariableDeclView():
      Node(NodeType::kVariableDeclView, 0) {}

  
  VariableDeclView(std::initializer_list<Node*> vars):
      Node(NodeType::kVariableDeclView, 0) {
    node_list_.insert(node_list_.end(), vars.begin(), vars.end());
  }
};


// Represent true.
class TrueView: public Node {
 public:
  TrueView(): Node(NodeType::kTrueView, 0) {}
};


// Represent false.
class FalseView: public Node {
 public:
  FalseView(): Node(NodeType::kFalseView, 0) {}
};


// Represent module.
class ModuleDeclView: public Node {
 public:
  ModuleDeclView():
      Node(NodeType::kModuleDeclView, 2u) {}


  ModuleDeclView(Node* name, Node* statement):
      Node(NodeType::kModuleDeclView, 2u) {
    InitNodeList({name, statement});
  }

  
  NODE_PROPERTY(name, 0);

  NODE_PROPERTY(statement, 1);
};


// Represent export.
class ExportView: public Node {
 public:
  ExportView(Node* target)
      : Node(NodeType::kExportView, 1u) {
    InsertLast(target);
  }


  ExportView()
      : Node(NodeType::kExportView, 1u) {}

  
  NODE_PROPERTY(target, 0);
};


// Represent import.
class ImportView: public Node {
 public:
  ImportView(Node* alias, Node* from_expr)
      : Node(NodeType::kImportView, 2u) {
    InitNodeList({alias, from_expr});
  }


  ImportView()
      : Node(NodeType::kImportView, 2u) {}

  
  NODE_PROPERTY(alias, 0);

  
  NODE_PROPERTY(from_expr, 1);
};


// Represent variable.
class VariableView : public Node {
 public:
  VariableView(Node* name, Node* value, Node* type):
      Node(NodeType::kVariableView, 3u) {
    InitNodeList({name, value, type});
  }


  VariableView():
      Node(NodeType::kVariableView, 3u) {}


  NODE_PROPERTY(name, 0);
  NODE_PROPERTY(value, 1);
  NODE_PROPERTY(type, 2);
};


// Represent if statement.
class IfStatementView : public Node {
 public:
  IfStatementView(Node* if_block_Node, Node* else_block_node)
      : Node(NodeType::kIfStatementView, 2u) {
    InitNodeList({if_block_Node, else_block_node});
  }


  IfStatementView()
      : Node(NodeType::kIfStatementView, 2u) {}

  
  // Getter and Setter for then_block_.
  NODE_PROPERTY(then_block, 0);
  
  // Getter and Setter for else_block_.
  NODE_PROPERTY(else_block, 1);
};


// Represent continue statement.
class ContinueStatementView: public Node {
 public:
  ContinueStatementView()
      : Node(NodeType::kContinueStatementView, 0u){}
};


// Represent return statement.
class ReturnStatementView: public Node {
 public:
  ReturnStatementView(Node* expr)
      : Node(NodeType::kReturnStatementView, 1u) {
    InitNodeList({expr});
  }


  ReturnStatementView()
      : Node(NodeType::kReturnStatementView, 1u) {}
  

  // Getter and Setter for expr_.
  NODE_PROPERTY(expr, 0);
};


// Represent break statement.
class BreakStatementView: public Node {
 public:
  BreakStatementView(Node* label)
      : Node(NodeType::kBreakStatementView, 1u) {
    InitNodeList({label});
  }

  BreakStatementView()
      : Node(NodeType::kBreakStatementView, 1u) {}


  // Getter and Setter for label.
  NODE_PROPERTY(label, 0);
};


// Represent with statement.
class WithStatementView: public Node {
 public:
  WithStatementView(Node* expr, Node* statement)
      : Node(NodeType::kWithStatementView, 2u) {
    InitNodeList({expr, statement});
  }


  WithStatementView()
      : Node(NodeType::kWithStatementView, 2u) {}

  // Getter and Setter for expr.
  NODE_PROPERTY(expr, 0);

  
  // Getter and Setter for statement.
  NODE_PROPERTY(statement, 1);
};


// Represent labell.
class LabelledStatementView: public Node {
 public:
  LabelledStatementView(Node* name, Node* statement)
      : Node(NodeType::kLabelledStatementView, 2u) {
    InitNodeList({name, statement});
  }


  LabelledStatementView()
      : Node(NodeType::kLabelledStatementView, 2u) {}


  // Getter and Setter for name_.
  NODE_PROPERTY(name, 0);


  NODE_PROPERTY(statement, 1);
};


// Represent switch statement.
class SwitchStatementView: public Node {
 public:
  SwitchStatementView(Node* case_list, Node* default_case)
      : Node(NodeType::kSwitchStatementView, 2u) {
    InitNodeList({case_list, default_case});
  }


  SwitchStatementView()
      : Node(NodeType::kSwitchStatementView, 2u) {}


  // Getter and Setter for case_list.
  NODE_PROPERTY(case_list, 0);


  // Getter and Setter for default_case.
  NODE_PROPERTY(default_case, 1);
};


class CaseListView: public Node {
 public:
  CaseListView(std::initializer_list<Node*> case_list)
      : Node(NodeType::kCaseListView, 0) {
    InitNodeList(case_list);
  }


  CaseListView()
      : Node(NodeType::kCaseListView, 0) {}
};


// Represent case.
class CaseView: public Node {
 public:
  CaseView(Node* condition, Node* body)
      : Node(NodeType::kCaseView, 2u) {
    InitNodeList({condition, body});
  }


  CaseView()
      : Node(NodeType::kCaseView, 2u) {}


  // Getter and Setter for condition.
  NODE_PROPERTY(condition, 0);


  // Getter and Setter for body.
  NODE_PROPERTY(body, 1);
};


// Represent try catch finally statement.
class TryStatementView: public Node {
 public:
  TryStatementView(Node* statement, Node* catch_statement, Node* finally_statement)
      : Node(NodeType::kTryStatementView, 3u) {
    InitNodeList({statement, catch_statement, finally_statement});
  }

  
  TryStatementView()
      : Node(NodeType::kTryStatementView, 3u) {}


  // Getter and Setter for statement.
  NODE_PROPERTY(statement, 0);

  
  // Getter and Setter for catch_statement.
  NODE_PROPERTY(catch_statement, 1);


  // Getter and Setter for finally_statement.
  NODE_PROPERTY(finally_statement, 2);
};


class CatchStatementView: public Node {
 public:
  CatchStatementView(Node* error_name, Node* body)
      : Node(NodeType::kCatchStatementView, 2u) {
    InitNodeList({error_name, body});
  }


  CatchStatementView()
      : Node(NodeType::kCatchStatementView, 2u) {}


  // Getter and Setter for error_name.
  NODE_PROPERTY(error_name, 0);


  // Getter and Setter for body.
  NODE_PROPERTY(body, 1);
};


// Represent finally statement.
class FinallyStatementView: public Node {
 public:
  FinallyStatementView(Node* body)
      : Node(NodeType::kFinallyStatementView, 1u) {
    InitNodeList({body});
  }


  FinallyStatementView()
      : Node(NodeType::kFinallyStatementView, 1u) {}


  NODE_PROPERTY(body, 0);
};


// Represent throw statement.
class ThrowStatementView: public Node {
 public:
  ThrowStatementView(Node* expr)
      : Node(NodeType::kThrowStatementView, 1u) {
    InitNodeList({expr});
  }

  
  // Getter and Setter for expr.
  NODE_PROPERTY(expr, 0);
};


class ForStatementView: public Node {
 public:
  ForStatementView(Node* cond_init, Node* cond_cmp, Node* cond_upd, Node* body)
      : Node(NodeType::kForStatementView, 4u) {
    InitNodeList({cond_init, cond_cmp, cond_upd, body});
  }

  ForStatementView()
      : Node(NodeType::kForStatementView, 4u) {}


  // Getter and Setter for cond_init.
  NODE_PROPERTY(cond_init, 0);


  // Getter and Setter for cond_upd.
  NODE_PROPERTY(cond_upd, 1);


  // Getter and Setter for cond_cmp.
  NODE_PROPERTY(cond_cmp, 2);


  // Getter and Setter for body .
  NODE_PROPERTY(body, 3);
};


// Represent for in statement.
class ForInStatementView: public Node {
 public:
  ForInStatementView(Node* property_name, Node* expr, Node* body)
      : Node(NodeType::kForInStatementView, 3u) {
    InitNodeList({property_name, expr, body});
  }

  ForInStatementView()
      : Node(NodeType::kForInStatementView, 3u) {}


  // Getter and Setter for cond_init.
  NODE_PROPERTY(property_name, 0);


  // Getter and Setter for expr.
  NODE_PROPERTY(expr, 1);

  
  // Getter and Setter for body.
  NODE_PROPERTY(body, 2);
};


// Represent while statement
class WhileStatementView: public Node {
 public:
  WhileStatementView(Node* expr, Node* body)
      : Node(NodeType::kWhileStatementView, 2u) {
    InitNodeList({expr, body});
  }

  WhileStatementView()
      : Node(NodeType::kWhileStatementView, 2u) {}


  // Getter and Setter for expr.
  NODE_PROPERTY(expr, 0);


  // Getter and Setter for body.
  NODE_PROPERTY(body, 1);
};


// Represent do while statement.
class DoWhileStatementView: public Node {
 public:
  DoWhileStatementView(Node* expr, Node* body)
      : Node(NodeType::kDoWhileStatementView, 2u) {
    InitNodeList({expr, body});
  }


  DoWhileStatementView()
      : Node(NodeType::kDoWhileStatementView, 2u) {}


  // Getter and Setter for expr.
  NODE_PROPERTY(expr, 0);


  // Getter and Setter for body .
  NODE_PROPERTY(body, 1);
};


class ClassDeclViewaration: public Node {
 public:
  ClassDeclViewaration(Node* name, Node* constructor, Node* field_list, Node* inheritance, Node* impl_list)
      : Node(NodeType::kClassDeclView, 5u) {
    InitNodeList({name, constructor, field_list, inheritance, impl_list});
  }

  
  ClassDeclViewaration()
      : Node(NodeType::kClassDeclView, 5u) {}
  

  // Getter and Setter for name.
  NODE_PROPERTY(name, 0);


  // Getter and Setter for constructor.
  NODE_PROPERTY(constructor, 1);


  // Getter and Setter for field_list.
  NODE_PROPERTY(field_list, 2);


  // Getter and Setter for inheritance.
  NODE_PROPERTY(inheritance, 3);


  // Getter and Setter for impl_list.
  NODE_PROPERTY(impl_list, 4);
};


class ClassFieldListView: public Node {
 public:
  ClassFieldListView(std::initializer_list<Node*> fields)
      : Node(NodeType::kClassFieldListView, 0) {
    InitNodeList(fields);
  }


  ClassFieldListView()
      : Node(NodeType::kClassFieldListView, 0) {}
};


class ClassFieldAccessLevelView: public Node {
 public:
  ClassFieldAccessLevelView(Node* value)
      : Node(NodeType::kClassFieldAccessLevelView, 1u) {
    InitNodeList({value});
  }


  // Getter and Setter for value.
  NODE_PROPERTY(value, 0);
};


class FieldBase: public Node {
 public:
  FieldBase() = delete;

  // Getter and Setter for access_level.
  NODE_PROPERTY(access_level, 0);
  

  // Getter and Setter for name.
  NODE_PROPERTY(name, 1);

  
  // Getter and Setter for value.
  NODE_PROPERTY(value, 2);

 protected:
  FieldBase(NodeType type, Node* access_level, Node* name, Node* value)
      : Node(type, 3u) {
    InitNodeList({access_level, name, value});
  }

  FieldBase(NodeType type)
      : Node(type, 3u) {}
};


// Represent instance property.
class InstancePropertyView: public FieldBase {
 public:
  InstancePropertyView(Node* access_level, Node* name, Node* value)
      : FieldBase(NodeType::kInstancePropertyView, access_level, name, value) {}

  InstancePropertyView()
      : FieldBase(NodeType::kInstancePropertyView) {}
};


// Represent instance property.
class InstanceMethodView: public FieldBase {
 public:
  InstanceMethodView(Node* access_level, Node* name, Node* value)
      : FieldBase(NodeType::kInstanceMethodView, access_level, name, value) {}

  InstanceMethodView()
      : FieldBase(NodeType::kInstanceMethodView) {}
};


// Represent instance property.
class ClassPropertyView: public FieldBase {
 public:
  ClassPropertyView(Node* access_level, Node* name, Node* value)
      : FieldBase(NodeType::kClassPropertyView, access_level, name, value) {}

  ClassPropertyView()
      : FieldBase(NodeType::kClassPropertyView) {}
};


// Represent instance property.
class ClassMethodView: public FieldBase {
 public:
  ClassMethodView(Node* access_level, Node* name, Node* value)
      : FieldBase(NodeType::kClassMethodView, access_level, name, value) {}

  ClassMethodView()
      : FieldBase(NodeType::kClassMethodView) {}
};


// Represent interface.
class InterfaceView: public Node {
 public:
  InterfaceView(Node* name, Node* interface_field_list)
      : Node(NodeType::kInterfaceView, 2u) {
    InitNodeList({name, interface_field_list});
  }

  InterfaceView()
      : Node(NodeType::kInterfaceView, 2u) {}

  
  // Getter and Setter for name.
  NODE_PROPERTY(name, 0);


  // Getter and Setter for name.
  NODE_PROPERTY(field_list, 1);
};


class InterfaceFieldListView: public Node {
  InterfaceFieldListView(std::initializer_list<Node*> fields)
      : Node(NodeType::kInterfaceFieldListView, 0) {
    InitNodeList(fields);
  }


  InterfaceFieldListView()
      : Node(NodeType::kInterfaceFieldListView, 0) {}
};


// Represent interface field.
class InterfaceFieldView: public Node {
 public:
  InterfaceFieldView(Node* name, Node* value):
      Node(NodeType::kInterfaceFieldView, 2u) {
    InitNodeList({name, value});
  }


  InterfaceFieldView():
      Node(NodeType::kInterfaceFieldView, 2u) {}


  // Getter and Setter for name.
  NODE_PROPERTY(name, 0);


  // Getter and Setter for value.
  NODE_PROPERTY(value, 1);
};


// Represent type expression like `var x: string`
class SimpleTypeExprView: public Node {
 public:
  
  SimpleTypeExprView(Node* type_name)
      : Node(NodeType::kSimpleTypeExprView, 1u) {
    InitNodeList({type_name});
  }
  

  // Getter and Setter for type_name_.
  NODE_PROPERTY(type_name, 0);
};


// Represent type expression like `var x: string`
class ArrayTypeExprView: public Node {
 public:
  
  ArrayTypeExprView(Node* type_name)
      : Node(NodeType::kArrayTypeExprView, 1u) {
    InitNodeList({type_name});
  }
  

  // Getter and Setter for type_name_.
  NODE_PROPERTY(type_name, 0);
};


// Represent function type expression like, `var x:(a:string, b:string) => string;`
class FunctionNodeTypeExprView: public Node {
 public:
  FunctionNodeTypeExprView(Node* param_list, Node* return_type)
      : Node(NodeType::kFunctionNodeTypeExprView, 2u) {
    InitNodeList({param_list, return_type});
  }


  FunctionNodeTypeExprView()
      : Node(NodeType::kFunctionNodeTypeExprView, 2u) {}


  // Getter and setter for param_list_.
  NODE_PROPERTY(param_list, 0);
  

  // Getter and setter for return_type_.
  NODE_PROPERTY(return_type, 1);
};


// Represent accessor type expression like, `interface x {[index:int]}`
class AccessorTypeExprView: public Node {
 public:
  AccessorTypeExprView(Node* name, Node* type_expression):
      Node(NodeType::kAccessorTypeExprView, 2u) {
    InitNodeList({name, type_expression});
  }

  
  // Getter and setter for name_.
  NODE_PROPERTY(name, 0);


  // Getter and setter for type_expr.
  NODE_PROPERTY(type_expr, 1);
};


// Represent function.
class FunctionView: public Node {
 public:
  FunctionView(Node* name, Node* param_list, Node* body)
      : Node(NodeType::kFunctionView, 3u) {}

  // Getter for name_.
  NODE_GETTER(name, 0);

  // Getter for param_list_.
  NODE_GETTER(param_list, 1);

  // Getter for body_.
  NODE_GETTER(body, 2);
};


class CallView: public Node {
 public:
  CallView(Node* target, Node* args)
      : Node(NodeType::kCallView, 2u) {
    InitNodeList({target, args});
  }

  
  NODE_PROPERTY(target, 0);


  NODE_PROPERTY(args, 1);
};


class CallArgsView: public Node {
 public:
  CallArgsView(std::initializer_list<Node*> args)
      : Node(NodeType::kCallArgsView, 0) {
    InitNodeList(args);
  }


  CallArgsView()
      : Node(NodeType::kCallArgsView, 0) {}
};


class NewCallView: public Node {
 public:
  NewCallView(Node* target, Node* args)
      : Node(NodeType::kNewCallView, 2u) {
    InitNodeList({target, args});
  }


  NewCallView()
      : Node(NodeType::kNewCallView, 2u) {}
};


class NameView: public Node {
 public:
  NameView(UtfString name)
      : Node(NodeType::kNameView, 0) {
    set_string_value(name);
  }
};


class GetPropView: public Node {
 public:
  GetPropView(Node* target, Node* prop)
      : Node(NodeType::kGetPropView, 2u) {
    InitNodeList({target, prop});
  }

  // Getter and Setter for target.
  NODE_PROPERTY(target, 0);

  // Getter and Setter for prop.
  NODE_PROPERTY(prop, 1);
};


class GetElemView: public Node {
 public:
  GetElemView(Node* target, Node* prop)
      : Node(NodeType::kGetElemView, 2u) {
    InitNodeList({target, prop});
  }

  
  // Getter and Setter for target.
  NODE_PROPERTY(target, 0);

  
  // Getter and Setter for prop.
  NODE_PROPERTY(prop, 1);
};


class AssignmentView: public Node {
 public:
  AssignmentView(Node* target, Node* expr)
      : Node(NodeType::kAssignmentView, 2u) {
    InitNodeList({target, expr});
  }


  // Getter and Setter for target.
  NODE_PROPERTY(target, 0);


  // Getter and Setter for expr.
  NODE_PROPERTY(expr, 1);
};


class TemaryExprView: public Node {
 public :
  TemaryExprView(Node* cond, Node* then_expr, Node* else_expr)
      : Node(NodeType::kTemaryExprView, 3u) {
    InitNodeList({cond, then_expr, else_expr});
  }


  // Getter and Setter for cond.
  NODE_PROPERTY(cond, 0);
  
  
  // Getter and Setter for then_exp.
  NODE_PROPERTY(then_expr, 1);

  
  // Getter and Setter for else_exp.
  NODE_PROPERTY(else_expr, 2);
};


class BinaryExprView: public Node {
 public:
  BinaryExprView(Token op, Node* first, Node* second)
      : Node(NodeType::kBinaryExprView, 2u) {
    InitNodeList({first, second});
    set_operand(op);
  }

  // Getter and Setter for first.
  NODE_PROPERTY(first, 0);

  // Getter and Setter for second.
  NODE_PROPERTY(second, 1);
};


// Represent cast.
class CastView: public Node {
 public:
  CastView(Node* type_expr)
      : Node(NodeType::kCastView, 1u) {
    InitNodeList({type_expr});
  }


  // Getter for type_expr.
  NODE_GETTER(type_expr, 0);
};


class UnaryExprView: public Node {
 public:
  UnaryExprView(Token op, Node* expr)
      : Node(NodeType::kUnaryExprView, 1u) {
    InitNodeList({expr});
    set_operand(op);
  }

  // Getter and Setter for exp_.
  NODE_PROPERTY(expr, 0);
};


class ThisView: public Node {
 public:
  ThisView()
      : Node(NodeType::kThisView, 0) {}
};


class NumberView: public Node {
 public:
  NumberView(UtfString value)
      : Node(NodeType::kNumberView, 0) {
    const char* val = value.ToUtf8Value().value();
    set_string_value(value);
    double d = 0l;
    sscanf(val, "%lf", &d);
    set_double_value(d);
  }
};


class NullView: public Node {
 public:
  NullView()
      : Node(NodeType::kNullView, 0) {}
};


class StringView: public Node {
 public:
  StringView(UtfString str)
      : Node(NodeType::kStringView, 0) {
    set_string_value(str);
  }
};


class ObjectElementView: public Node {
 public:
  ObjectElementView(Node* key, Node* value)
      : Node(NodeType::kObjectElementView, 2u) {
    InitNodeList({key, value});
  }

  // Getter and setter for key_.
  NODE_PROPERTY(key, 0);


  // Getter and setter for value_.
  NODE_PROPERTY(value, 1);
};


class ObjectLiteralView: public Node {
 public:
  ObjectLiteralView(std::initializer_list<Node*> properties)
      : Node(NodeType::kObjectLiteralView, 0) {
    InitNodeList(properties);
  }


  ObjectLiteralView()
      : Node(NodeType::kObjectLiteralView, 0) {}
};


class ArrayLiteralView: public Node {
 public:
  ArrayLiteralView(std::initializer_list<Node*> elements)
      : Node(NodeType::kArrayLiteralView, 0) {
    InitNodeList(elements);
  }


  ArrayLiteralView()
      : Node(NodeType::kArrayLiteralView, 0) {}
};


class UndefinedView: public Node {
 public:
  UndefinedView():
      Node(NodeType::kUndefinedView, 0){}
};


class DebuggerView: public Node {
 public:
  DebuggerView():
      Node(NodeType::kDebuggerView, 0){}
};


}} //rasp::ir

#undef NODE_PROPERTY
#undef NODE_GETTER
#undef NODE_SETTER
#undef DEF_CAST

#include "./node-inl.h"

#endif

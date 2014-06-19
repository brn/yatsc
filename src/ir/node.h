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
  RASP_INLINE void set_##name(Node* name) {node_list_[pos] = name;}


// Define both getter and setter accessors.
#define NODE_PROPERTY(name, pos)                \
  NODE_GETTER(name, pos)                        \
  NODE_SETTER(name, pos)


#define OPTIONAL_PARENT_PARAMETER               \
  Node* parent_node = nullptr


class SourceInformation {
  friend class Node;
 public:
  SourceInformation(size_t line_number, size_t start_col)
      : line_number_(line_number),
        start_col_(start_col) {}


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
class Node : public RegionalObject {
 public:

  typedef std::string String;
  typedef std::vector<Node*> List;
  typedef List::iterator ListIterator;


  /**
   * Create Node.
   * @param node_type The node type.
   * @param capacity The size of children tree.
   * @param parent_node A parent node of this node.
   */
  RASP_INLINE Node(NodeType node_type, size_t capacity = 0, Node* parent_node = nullptr)
      : node_type_(node_type),
        parent_node_(parent_node),
        operand_(Token::ILLEGAL),
        double_value_(0l) {
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


  /**
   * Insert a node at the end of the children.
   * @param node A node that want to insert.
   */
  RASP_INLINE void InsertLast(Node* node) {
    node_list_.push_back(node);
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
  }


  /**
   * Remove a specified iterator from children.
   * @param block An iterator that want to erase from children.
   */
  RASP_INLINE void Remove(Node::ListIterator iterator) {
    node_list_.erase(iterator);
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
  Node* parent_node_;
  Token operand_;
  double double_value_;
  UtfString string_value_;
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
  StatementView(OPTIONAL_PARENT_PARAMETER): Node(NodeType::kStatementView, 1u, parent_node){}

  StatementView(Node* expr, OPTIONAL_PARENT_PARAMETER):
      Node(NodeType::kStatementView, 1u, parent_node) {
    InsertLast(expr);
  }

  // Getter and Setter for exp.
  NODE_PROPERTY(expr, 0);
};


// Represent variable declarations.
class VariableDeclView: public Node {
 public:
  VariableDeclView(OPTIONAL_PARENT_PARAMETER):
      Node(NodeType::kVariableDeclView, 0, parent_node) {}

  
  VariableDeclView(std::initializer_list<Node*> vars, OPTIONAL_PARENT_PARAMETER):
      Node(NodeType::kVariableDeclView, 0, parent_node) {
    node_list_.insert(node_list_.end(), vars.begin(), vars.end());
  }
};


// Represent true.
class TrueView: public Node {
 public:
  TrueView(OPTIONAL_PARENT_PARAMETER): Node(NodeType::kTrueView, 0, parent_node) {}
};


// Represent false.
class FalseView: public Node {
 public:
  FalseView(OPTIONAL_PARENT_PARAMETER): Node(NodeType::kFalseView, 0, parent_node) {}
};


// Represent module.
class ModuleDeclView: public Node {
 public:
  ModuleDeclView(OPTIONAL_PARENT_PARAMETER):
      Node(NodeType::kModuleDeclView, 2u, parent_node) {}


  ModuleDeclView(Node* name, Node* statement, OPTIONAL_PARENT_PARAMETER):
      Node(NodeType::kModuleDeclView, 2u, parent_node) {
    InitNodeList({name, statement});
  }

  
  NODE_PROPERTY(name, 0);

  NODE_PROPERTY(statement, 1);
};


// Represent export.
class ExportView: public Node {
 public:
  ExportView(Node* target, OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kExportView, 1u, parent_node) {
    InsertLast(target);
  }


  ExportView(OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kExportView, 1u, parent_node) {}

  
  NODE_PROPERTY(target, 0);
};


// Represent import.
class ImportView: public Node {
 public:
  ImportView(Node* alias, Node* from_expr, OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kImportView, 2u, parent_node) {
    InitNodeList({alias, from_expr});
  }


  ImportView(OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kImportView, 2u, parent_node) {}

  
  NODE_PROPERTY(alias, 0);

  
  NODE_PROPERTY(from_expr, 1);
};


// Represent variable.
class VariableView : public Node {
 public:
  VariableView(Node* name, Node* value, Node* type, OPTIONAL_PARENT_PARAMETER):
      Node(NodeType::kVariableView, 3u, parent_node) {
    InitNodeList({name, value, type});
  }


  VariableView(OPTIONAL_PARENT_PARAMETER):
      Node(NodeType::kVariableView, 3u, parent_node) {}


  NODE_PROPERTY(name, 0);
  NODE_PROPERTY(value, 1);
  NODE_PROPERTY(type, 2);
};


// Represent if statement.
class IfStatementView : public Node {
 public:
  IfStatementView(Node* if_block_Node, Node* else_block_node, OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kIfStatementView, 2u, parent_node) {
    InitNodeList({if_block_Node, else_block_node});
  }


  IfStatementView(OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kIfStatementView, 2u, parent_node) {}

  
  // Getter and Setter for then_block_.
  NODE_PROPERTY(then_block, 0);
  
  // Getter and Setter for else_block_.
  NODE_PROPERTY(else_block, 1);
};


// Represent continue statement.
class ContinueStatementView: public Node {
 public:
  ContinueStatementView(OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kContinueStatementView, 0u, parent_node){}
};


// Represent return statement.
class ReturnStatementView: public Node {
 public:
  ReturnStatementView(Node* expr, OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kReturnStatementView, 1u, parent_node) {
    InitNodeList({expr});
  }


  ReturnStatementView(OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kReturnStatementView, 1u, parent_node) {}
  

  // Getter and Setter for expr_.
  NODE_PROPERTY(expr, 0);
};


// Represent break statement.
class BreakStatementView: public Node {
 public:
  BreakStatementView(Node* label, OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kBreakStatementView, 1u, parent_node) {
    InitNodeList({label});
  }

  BreakStatementView(OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kBreakStatementView, 1u, parent_node) {}


  // Getter and Setter for label.
  NODE_PROPERTY(label, 0);
};


// Represent with statement.
class WithStatementView: public Node {
 public:
  WithStatementView(Node* expr, Node* statement, OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kWithStatementView, 2u, parent_node) {
    InitNodeList({expr, statement});
  }


  WithStatementView(OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kWithStatementView, 2u, parent_node) {}

  // Getter and Setter for expr.
  NODE_PROPERTY(expr, 0);

  
  // Getter and Setter for statement.
  NODE_PROPERTY(statement, 1);
};


// Represent labell.
class LabelledStatementView: public Node {
 public:
  LabelledStatementView(Node* name, Node* statement, OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kLabelledStatementView, 2u, parent_node) {
    InitNodeList({name, statement});
  }


  LabelledStatementView(OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kLabelledStatementView, 2u, parent_node) {}


  // Getter and Setter for name_.
  NODE_PROPERTY(name, 0);


  NODE_PROPERTY(statement, 1);
};


// Represent switch statement.
class SwitchStatementView: public Node {
 public:
  SwitchStatementView(Node* case_list, Node* default_case, OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kSwitchStatementView, 2u, parent_node) {
    InitNodeList({case_list, default_case});
  }


  SwitchStatementView(OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kSwitchStatementView, 2u, parent_node) {}


  // Getter and Setter for case_list.
  NODE_PROPERTY(case_list, 0);


  // Getter and Setter for default_case.
  NODE_PROPERTY(default_case, 1);
};


class CaseListView: public Node {
 public:
  CaseListView(std::initializer_list<Node*> case_list, OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kCaseListView, 0, parent_node) {
    InitNodeList(case_list);
  }


  CaseListView(OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kCaseListView, 0, parent_node) {}
};


// Represent case.
class CaseView: public Node {
 public:
  CaseView(Node* condition, Node* body, OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kCaseView, 2u, parent_node) {
    InitNodeList({condition, body});
  }


  CaseView(OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kCaseView, 2u, parent_node) {}


  // Getter and Setter for condition.
  NODE_PROPERTY(condition, 0);


  // Getter and Setter for body.
  NODE_PROPERTY(body, 1);
};


// Represent try catch finally statement.
class TryStatementView: public Node {
 public:
  TryStatementView(Node* statement, Node* catch_statement, Node* finally_statement, OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kTryStatementView, 3u, parent_node) {
    InitNodeList({statement, catch_statement, finally_statement});
  }

  
  TryStatementView(OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kTryStatementView, 3u, parent_node) {}


  // Getter and Setter for statement.
  NODE_PROPERTY(statement, 0);

  
  // Getter and Setter for catch_statement.
  NODE_PROPERTY(catch_statement, 1);


  // Getter and Setter for finally_statement.
  NODE_PROPERTY(finally_statement, 2);
};


class CatchStatementView: public Node {
 public:
  CatchStatementView(Node* error_name, Node* body, OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kCatchStatementView, 2u, parent_node) {
    InitNodeList({error_name, body});
  }


  CatchStatementView(OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kCatchStatementView, 2u, parent_node) {}


  // Getter and Setter for error_name.
  NODE_PROPERTY(error_name, 0);


  // Getter and Setter for body.
  NODE_PROPERTY(body, 1);
};


// Represent finally statement.
class FinallyStatementView: public Node {
 public:
  FinallyStatementView(Node* body, OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kFinallyStatementView, 1u, parent_node) {
    InitNodeList({body});
  }


  FinallyStatementView(OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kFinallyStatementView, 1u, parent_node) {}


  NODE_PROPERTY(body, 0);
};


// Represent throw statement.
class ThrowStatementView: public Node {
 public:
  ThrowStatementView(Node* expr, OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kThrowStatementView, 1u, parent_node) {
    InitNodeList({expr});
  }

  
  // Getter and Setter for expr.
  NODE_PROPERTY(expr, 0);
};


class ForStatementView: public Node {
 public:
  ForStatementView(Node* cond_init, Node* cond_cmp, Node* cond_upd, Node* body, OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kForStatementView, 4u, parent_node) {
    InitNodeList({cond_init, cond_cmp, cond_upd, body});
  }

  ForStatementView(OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kForStatementView, 4u, parent_node) {}


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
  ForInStatementView(Node* property_name, Node* expr, Node* body, OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kForInStatementView, 3u, parent_node) {
    InitNodeList({property_name, expr, body});
  }

  ForInStatementView(OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kForInStatementView, 3u, parent_node) {}


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
  WhileStatementView(Node* expr, Node* body, OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kWhileStatementView, 2u, parent_node) {
    InitNodeList({expr, body});
  }

  WhileStatementView(OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kWhileStatementView, 2u, parent_node) {}


  // Getter and Setter for expr.
  NODE_PROPERTY(expr, 0);


  // Getter and Setter for body.
  NODE_PROPERTY(body, 1);
};


// Represent do while statement.
class DoWhileStatementView: public Node {
 public:
  DoWhileStatementView(Node* expr, Node* body, OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kDoWhileStatementView, 2u, parent_node) {
    InitNodeList({expr, body});
  }


  DoWhileStatementView(OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kDoWhileStatementView, 2u, parent_node) {}


  // Getter and Setter for expr.
  NODE_PROPERTY(expr, 0);


  // Getter and Setter for body .
  NODE_PROPERTY(body, 1);
};


class ClassDeclViewaration: public Node {
 public:
  ClassDeclViewaration(Node* name, Node* constructor, Node* field_list, Node* inheritance, Node* impl_list, OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kClassDeclView, 5u, parent_node) {
    InitNodeList({name, constructor, field_list, inheritance, impl_list});
  }

  
  ClassDeclViewaration(OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kClassDeclView, 5u, parent_node) {}
  

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
  ClassFieldListView(std::initializer_list<Node*> fields, OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kClassFieldListView, 0, parent_node) {
    InitNodeList(fields);
  }


  ClassFieldListView(OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kClassFieldListView, 0, parent_node) {}
};


class ClassFieldAccessLevelView: public Node {
 public:
  ClassFieldAccessLevelView(Node* value, OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kClassFieldAccessLevelView, 1u, parent_node) {
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
  FieldBase(NodeType type, Node* access_level, Node* name, Node* value, Node* parent_node)
      : Node(type, 3u, parent_node) {
    InitNodeList({access_level, name, value});
  }

  FieldBase(NodeType type, Node* parent_node)
      : Node(type, 3u, parent_node) {}
};


// Represent instance property.
class InstancePropertyView: public FieldBase {
 public:
  InstancePropertyView(Node* access_level, Node* name, Node* value, OPTIONAL_PARENT_PARAMETER)
      : FieldBase(NodeType::kInstancePropertyView, access_level, name, value, parent_node) {}

  InstancePropertyView(OPTIONAL_PARENT_PARAMETER)
      : FieldBase(NodeType::kInstancePropertyView, parent_node) {}
};


// Represent instance property.
class InstanceMethodView: public FieldBase {
 public:
  InstanceMethodView(Node* access_level, Node* name, Node* value, OPTIONAL_PARENT_PARAMETER)
      : FieldBase(NodeType::kInstanceMethodView, access_level, name, value, parent_node) {}

  InstanceMethodView(OPTIONAL_PARENT_PARAMETER)
      : FieldBase(NodeType::kInstanceMethodView, parent_node) {}
};


// Represent instance property.
class ClassPropertyView: public FieldBase {
 public:
  ClassPropertyView(Node* access_level, Node* name, Node* value, OPTIONAL_PARENT_PARAMETER)
      : FieldBase(NodeType::kClassPropertyView, access_level, name, value, parent_node) {}

  ClassPropertyView(OPTIONAL_PARENT_PARAMETER)
      : FieldBase(NodeType::kClassPropertyView, parent_node) {}
};


// Represent instance property.
class ClassMethodView: public FieldBase {
 public:
  ClassMethodView(Node* access_level, Node* name, Node* value, OPTIONAL_PARENT_PARAMETER)
      : FieldBase(NodeType::kClassMethodView, access_level, name, value, parent_node) {}

  ClassMethodView(OPTIONAL_PARENT_PARAMETER)
      : FieldBase(NodeType::kClassMethodView, parent_node) {}
};


// Represent interface.
class InterfaceView: public Node {
 public:
  InterfaceView(Node* name, Node* interface_field_list, OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kInterfaceView, 2u, parent_node) {
    InitNodeList({name, interface_field_list});
  }

  InterfaceView(OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kInterfaceView, 2u, parent_node) {}

  
  // Getter and Setter for name.
  NODE_PROPERTY(name, 0);


  // Getter and Setter for name.
  NODE_PROPERTY(field_list, 1);
};


class InterfaceFieldListView: public Node {
  InterfaceFieldListView(std::initializer_list<Node*> fields, OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kInterfaceFieldListView, 0, parent_node) {
    InitNodeList(fields);
  }


  InterfaceFieldListView(OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kInterfaceFieldListView, 0, parent_node) {}
};


// Represent interface field.
class InterfaceFieldView: public Node {
 public:
  InterfaceFieldView(Node* name, Node* value, OPTIONAL_PARENT_PARAMETER):
      Node(NodeType::kInterfaceFieldView, 2u, parent_node) {
    InitNodeList({name, value});
  }


  InterfaceFieldView(OPTIONAL_PARENT_PARAMETER):
      Node(NodeType::kInterfaceFieldView, 2u, parent_node) {}


  // Getter and Setter for name.
  NODE_PROPERTY(name, 0);


  // Getter and Setter for value.
  NODE_PROPERTY(value, 1);
};


// Represent type expression like `var x: string`
class SimpleTypeExprView: public Node {
 public:
  
  SimpleTypeExprView(Node* type_name, OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kSimpleTypeExprView, 1u, parent_node) {
    InitNodeList({type_name});
  }
  

  // Getter and Setter for type_name_.
  NODE_PROPERTY(type_name, 0);
};


// Represent type expression like `var x: string`
class ArrayTypeExprView: public Node {
 public:
  
  ArrayTypeExprView(Node* type_name, OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kArrayTypeExprView, 1u, parent_node) {
    InitNodeList({type_name});
  }
  

  // Getter and Setter for type_name_.
  NODE_PROPERTY(type_name, 0);
};


// Represent function type expression like, `var x:(a:string, b:string) => string;`
class FunctionNodeTypeExprView: public Node {
 public:
  FunctionNodeTypeExprView(Node* param_list, Node* return_type, OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kFunctionNodeTypeExprView, 2u, parent_node) {
    InitNodeList({param_list, return_type});
  }


  FunctionNodeTypeExprView(OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kFunctionNodeTypeExprView, 2u, parent_node) {}


  // Getter and setter for param_list_.
  NODE_PROPERTY(param_list, 0);
  

  // Getter and setter for return_type_.
  NODE_PROPERTY(return_type, 1);
};


// Represent accessor type expression like, `interface x {[index:int]}`
class AccessorTypeExprView: public Node {
 public:
  AccessorTypeExprView(Node* name, Node* type_expression, OPTIONAL_PARENT_PARAMETER):
      Node(NodeType::kAccessorTypeExprView, 2u, parent_node) {
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
  FunctionView(Node* name, Node* param_list, Node* body, OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kFunctionView, 3u, parent_node) {}

  // Getter for name_.
  NODE_GETTER(name, 0);

  // Getter for param_list_.
  NODE_GETTER(param_list, 1);

  // Getter for body_.
  NODE_GETTER(body, 2);
};


class CallView: public Node {
 public:
  CallView(Node* target, Node* args, OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kCallView, 2u, parent_node) {
    InitNodeList({target, args});
  }

  
  NODE_PROPERTY(target, 0);


  NODE_PROPERTY(args, 1);
};


class CallArgsView: public Node {
 public:
  CallArgsView(std::initializer_list<Node*> args, OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kCallArgsView, 0, parent_node) {
    InitNodeList(args);
  }


  CallArgsView(OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kCallArgsView, 0, parent_node) {}
};


class NewCallView: public Node {
 public:
  NewCallView(Node* target, Node* args, OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kNewCallView, 2u, parent_node) {
    InitNodeList(target, args);
  }


  NewCallView(OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kNewCallView, 2u, parent_node) {}
};


class NameView: public Node {
 public:
  NameView(UtfString name, OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kNameView, 0, parent_node) {
    set_string_value(name);
  }
};


class GetPropView: public Node {
 public:
  GetPropView(Node* target, Node* prop, OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kGetPropView, 2u, parent_node) {
    InitNodeList({target, prop});
  }

  // Getter and Setter for target.
  NODE_PROPERTY(target, 0);

  // Getter and Setter for prop.
  NODE_PROPERTY(prop, 1);
};


class GetElemView: public GetElemView {
 public:
  GetElemView(Node* target, Node* prop, OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kGetElemView, 2u, parent_node) {
    InitNodeList({target, prop});
  }

  
  // Getter and Setter for target.
  NODE_PROPERTY(target, 0);

  
  // Getter and Setter for prop.
  NODE_PROPERTY(prop, 1);
};


class AssignmentView: public Node {
 public:
  AssignmentView(Node* target, Node* expr, OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kAssignmentView, 2u, parent_node) {
    InitNodeList({target, expr});
  }


  // Getter and Setter for target.
  NODE_PROPERTY(target, 0);


  // Getter and Setter for expr.
  NODE_PROPERTY(expr, 1);
};


class TemaryExprView: public Node {
 public :
  TemaryExprView(Node* cond, Node* then_expr, Node* else_expr, OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kTemaryExprView, 3u, parent_node) {
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
  BinaryExprView(Token op, Node* first, Node* second, OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kBinaryExprView, 2u, parent_node) {
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
  CastView(Node* type_expr, OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kCastView, 1u, parent_node) {
    InitNodeList({type_expr});
  }


  // Getter for type_expr.
  NODE_GETTER(type_expr, 0);
};


class UnaryExprView: public Node {
 public:
  UnaryExprView(Token op, Node* expr, OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kUnaryExprView, 1u, parent_node) {
    InitNodeList({expr});
    set_operand(op);
  }

  // Getter and Setter for exp_.
  NODE_PROPERTY(expr, 0);
};


class ThisView: public Node {
 public:
  ThisView(OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kThisView, 0, parent_node) {}
};


class NumberView: public Node {
 public:
  NumberView(UtfString value, OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kNumberView, 0, parent_node) {
    const char* val = value.ToUtf8Value().value();
    set_string_value(value);
    double d = 0l;
    sscanf(val, "%lf", d);
    set_double_value(d);
  }
};


class NullView: public Node {
 public:
  NullView(OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kNullView, 0, parent_node) {}
};


class StringView: public Node {
 public:
  StringView(UtfString str, OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kStringView, 0, parent_node) {
    set_string_value(str);
  }
};


class ObjectElementView: public Node {
 public:
  ObjectElementView(Node* key, Node* value, OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kObjectElementView, 2u, parent_node) {
    InitNodeList({key, value});
  }

  // Getter and setter for key_.
  NODE_PROPERTY(key, 0);


  // Getter and setter for value_.
  NODE_PROPERTY(value, 1);
};


class ObjectLiteralView: public Node {
 public:
  ObjectLiteralView(std::initializer_list<Node*> properties, OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kObjectLiteralView, 0, parent_node) {
    InitNodeList(properties);
  }


  ObjectLiteralView(OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kObjectLiteralView, 0, parent_node) {}
};


class ArrayLiteralView: public Node {
 public:
  ArrayLiteralView(std::initializer_list<Node*> elements, OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kArrayLiteralView, 0, parent_node) {
    InitNodeList(elements);
  }


  ArrayLiteralView(OPTIONAL_PARENT_PARAMETER)
      : Node(NodeType::kArrayLiteralView, 0, parent_node) {}
};


class UndefinedView: public Node {
 public:
  UndefinedView(OPTIONAL_PARENT_PARAMETER):
      Node(NodeType::kUndefinedView, 0, parent_node){}
};


class DebuggerView: public Node {
 public:
  DebuggerView(OPTIONAL_PARENT_PARAMETER):
      Node(NodeType::kDebuggerView, 0, parent_node){}
};


}} //rasp::ir

#undef NODE_PROPERTY
#undef NODE_GETTER
#undef NODE_SETTER
#undef DEF_CAST
#undef OPTIONAL_PARENT_PARAMETER

#include "./node-inl.h"

#endif

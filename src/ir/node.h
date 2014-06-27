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

namespace yatsc {namespace ir {

// The list of the views.
#define VIEW_LIST(DECLARE, DECLARE_FIRST, DECLARE_LAST) \
  DECLARE_FIRST(Node)                                   \
  DECLARE(FileScopeView)                                \
  DECLARE(StatementView)                                \
  DECLARE(VariableDeclView)                             \
  DECLARE(TrueView)                                     \
  DECLARE(FalseView)                                    \
  DECLARE(BlockView)                                    \
  DECLARE(ModuleDeclView)                               \
  DECLARE(ExportView)                                   \
  DECLARE(ImportView)                                   \
  DECLARE(VariableView)                                 \
  DECLARE(IfStatementView)                              \
  DECLARE(ContinueStatementView)                        \
  DECLARE(ReturnStatementView)                          \
  DECLARE(BreakStatementView)                           \
  DECLARE(WithStatementView)                            \
  DECLARE(LabelledStatementView)                        \
  DECLARE(SwitchStatementView)                          \
  DECLARE(CaseView)                                     \
  DECLARE(CaseListView)                                 \
  DECLARE(TryStatementView)                             \
  DECLARE(CatchStatementView)                           \
  DECLARE(FinallyStatementView)                         \
  DECLARE(ThrowStatementView)                           \
  DECLARE(ForStatementView)                             \
  DECLARE(ForInStatementView)                           \
  DECLARE(WhileStatementView)                           \
  DECLARE(DoWhileStatementView)                         \
  DECLARE(ClassDeclView)                                \
  DECLARE(ClassFieldListView)                           \
  DECLARE(ClassFieldAccessLevelView)                    \
  DECLARE(InstancePropertyView)                         \
  DECLARE(InstanceMethodView)                           \
  DECLARE(ClassPropertyView)                            \
  DECLARE(ClassMethodView)                              \
  DECLARE(InterfaceView)                                \
  DECLARE(InterfaceFieldListView)                       \
  DECLARE(InterfaceFieldView)                           \
  DECLARE(SimpleTypeExprView)                           \
  DECLARE(ArrayTypeExprView)                            \
  DECLARE(ObjectTypeExprView)                           \
  DECLARE(PropertySignatureView)                        \
  DECLARE(MethodSignatureView)                          \
  DECLARE(FunctionTypeExprView)                         \
  DECLARE(ConstructSignatureView)                       \
  DECLARE(AccessorTypeExprView)                         \
  DECLARE(TypeParametersView)                           \
  DECLARE(TypeParameterView)                            \
  DECLARE(CommaExprView)                                \
  DECLARE(FunctionView)                                 \
  DECLARE(ArrowFunctionView)                            \
  DECLARE(ParameterView)                                \
  DECLARE(RestParamView)                                \
  DECLARE(ParamList)                                    \
  DECLARE(CallView)                                     \
  DECLARE(CallSinatureView)                             \
  DECLARE(CallArgsView)                                 \
  DECLARE(NewCallView)                                  \
  DECLARE(NameView)                                     \
  DECLARE(SuperView)                                    \
  DECLARE(PostfixView)                                  \
  DECLARE(GetPropView)                                  \
  DECLARE(GetElemView)                                  \
  DECLARE(AssignmentView)                               \
  DECLARE(TemaryExprView)                               \
  DECLARE(CastView)                                     \
  DECLARE(BinaryExprView)                               \
  DECLARE(UnaryExprView)                                \
  DECLARE(ThisView)                                     \
  DECLARE(NumberView)                                   \
  DECLARE(NullView)                                     \
  DECLARE(NaNView)                                      \
  DECLARE(StringView)                                   \
  DECLARE(RegularExprView)                              \
  DECLARE(ObjectElementView)                            \
  DECLARE(ObjectLiteralView)                            \
  DECLARE(ArrayLiteralView)                             \
  DECLARE(UndefinedView)                                \
  DECLARE_LAST(DebuggerView)


// Node types.
enum class NodeType: uint8_t {
#define DECLARE_ENUM(ViewName) k##ViewName,
#define DECLARE_FIRST(ViewName) k##ViewName = 0,
#define DECLARE_LAST(ViewName) k##ViewName
  VIEW_LIST(DECLARE_ENUM, DECLARE_FIRST, DECLARE_LAST)
#undef DECLARE_ENUM
#undef DECLARE_FIRST
#undef DECLARE_LAST
};


// Forward declarations.
#define FORWARD_DECL(ViewName) class ViewName;
VIEW_LIST(FORWARD_DECL, FORWARD_DECL, FORWARD_DECL)
#undef FORWARD_DECL
// End forward declarations.


// Define getter accessor.
#define NODE_GETTER(name, pos)                                      \
  YATSC_INLINE Node* name() YATSC_NOEXCEPT {return node_list_[pos];}


// Define setter accessor.
#define NODE_SETTER(name, pos)                                      \
  YATSC_INLINE void set_##name(Node* name) {                         \
    node_list_[pos] = name;                                         \
    if (name != nullptr) name->set_parent_node(this);               \
  }


// Define both getter and setter accessors.
#define NODE_PROPERTY(name, pos)                \
  NODE_GETTER(name, pos)                        \
  NODE_SETTER(name, pos)


#define NODE_FLAG_GETTER(name, pos)             \
  YATSC_INLINE bool name() {                    \
    return TestFlag(pos);                       \
  }


#define NODE_FLAG_SETTER(name, pos)               \
  YATSC_INLINE void set_##name(bool val) {        \
    if (val) {                                    \
      set_flag(pos);                              \
    } else {                                      \
      ClearFlag(pos);}                            \
  }


#define NODE_FLAG_PROPERTY(name, pos)           \
  NODE_FLAG_GETTER(name, pos)                   \
  NODE_FLAG_SETTER(name, pos)
  


// Source information holder.
class SourceInformation: private Unmovable{
  friend class Node;
 public:
  // Use default constructor.
  SourceInformation() = default;
  
  /**
   * @param line_number The line number of the current token.
   * @param start_col The start column of the current token.
   */
  SourceInformation(SourcePosition source_position)
      : source_position_(source_position) {}


  /**
   * Copy constructor
   */
  SourceInformation(const SourceInformation& source_information)
      : source_position_(source_information.source_position_){}


  // Copy assignment operator.
  SourceInformation& operator = (const SourceInformation& source_information) {
    source_position_ = source_information.source_position_;
    return *this;
  }


  // Getter and setter for line_number_
  YATSC_CONST_PROPERTY(const SourcePosition&, source_position, source_position_);
  
 private:
  SourcePosition source_position_;
};


// The IR Tree representation class.
// This class has all tree properties and accessible from Node type.
// All other **View classes are only view of this Node class.
class Node : public RegionalObject, private Uncopyable, private Unmovable {
 public:

  typedef std::string String;
  typedef std::vector<Node*> List;
  typedef List::iterator ListIterator;


  /**
   * Create Node.
   * @param node_type The node type.
   * @param capacity The size of children tree.
   */
  YATSC_INLINE Node(NodeType node_type, size_t capacity = 0)
      : node_type_(node_type),
        capacity_(capacity),
        parent_node_(nullptr),
        operand_(Token::ILLEGAL),
        double_value_(0l),
        invalid_lhs_(false),
        environment_(nullptr){
    if (capacity != 0) {
      node_list_.resize(capacity, nullptr);
    }
  }


  /**
   * Create Node.
   * @param node_type The node type.
   * @param capacity The size of children tree.
   */
  YATSC_INLINE Node(NodeType node_type, size_t capacity, std::initializer_list<Node*> node_list)
      : node_list_(node_list),
        node_type_(node_type),
        capacity_(capacity),
        parent_node_(nullptr),
        operand_(Token::ILLEGAL),
        double_value_(0l),
        invalid_lhs_(false),
        environment_(nullptr){
    if (capacity != 0) {
      node_list_.resize(capacity, nullptr);
    }
  }

  
  virtual ~Node() {}


  // Getter for node_type.
  YATSC_GETTER(NodeType, node_type, node_type_);


  YATSC_CONST_GETTER(const SourcePosition&, source_position, source_information_.source_position());


  // Getter and setter for parent_node_.
  YATSC_PROPERTY(Node*, parent_node, parent_node_);


  // Getter for children list.
  YATSC_CONST_GETTER(const List&, node_list, node_list_);


  // Getter for environment.
  YATSC_PROPERTY(Environment*, environment, environment_);


  YATSC_CONST_GETTER(Node*, first_child, node_list_[0]);


  YATSC_CONST_GETTER(Node*, last_child, node_list_.back());


  /**
   * Mark this node as invalid for the left-hand-side-expression.
   */
  void MarkAsInValidLhs() {invalid_lhs_ = true;}


  /**
   * Return wheter this node is valid lef-hand-side-expression or not.
   */
  bool IsValidLhs() {return !invalid_lhs_;}
  

  /**
   * Insert a node at the end of the children.
   * @param node A node that want to insert.
   */
  void InsertLast(Node* node);


  /**
   * Insert a node to the front of the children.
   * @param node A node that want to insert.
   */
  void InsertFront(Node* node);


  /**
   * Insert a new node to before specified node.
   * @param newNode A node that want to isnert.
   * @param oldNode A node taht has inserted.
   */
  void InsertBefore(Node* newNode, Node* oldNode);


  /**
   * Insert a new node after the specified node.
   * @param newNode A node that want to isnert.
   * @param oldNode A node taht has inserted.
   */
  void InsertAfter(Node* newNode, Node* oldNode);


  /**
   * Set string value.
   * @param str String value.
   */
  YATSC_INLINE void set_string_value(UtfString str) YATSC_NOEXCEPT {
    string_value_ = std::move(str);
  }


  /**
   * Return string value.
   * @returns String value.
   */
  YATSC_INLINE const UtfString& string_value() YATSC_NO_SE {
    return string_value_;
  }


  /**
   * Set double value.
   * @param d Double value.
   */
  YATSC_INLINE void set_double_value(double d) YATSC_NOEXCEPT {
    double_value_ = d;
  }


  /**
   * Return double value.
   * @return Double value.
   */
  YATSC_INLINE double double_value() YATSC_NO_SE {
    return double_value_;
  }


  /**
   * Set an operand.
   * @param op An operand.
   */
  YATSC_INLINE void set_operand(Token op) YATSC_NOEXCEPT {
    operand_ = op;
  }


  YATSC_INLINE void set_flag(int pos) YATSC_NOEXCEPT {
    flags_.set(pos);
  }


  YATSC_INLINE bool TestFlag(int pos) YATSC_NO_SE {
    return 1 == flags_.test(pos);
  }


  YATSC_INLINE void ClearFlag(size_t pos) YATSC_NOEXCEPT {
    flags_.reset(pos);
  }


  /**
   * Return an operand.
   * @returns An operand.
   */
  YATSC_INLINE Token operand() YATSC_NO_SE {
    return operand_;
  }


  /**
   * Remove specified node from children.
   * @param block A node that want to erase from children.
   */
  YATSC_INLINE void Remove(Node* block) {
    node_list_.erase(std::remove(node_list_.begin(), node_list_.end(), block), node_list_.end());
    block->set_parent_node(nullptr);
  }


  /**
   * Remove a specified iterator from children.
   * @param block An iterator that want to erase from children.
   */
  YATSC_INLINE void Remove(Node::ListIterator iterator) {
    node_list_.erase(iterator);
    (*iterator)->set_parent_node(nullptr);
  }


  /**
   * Set source information to this node.
   * @param token_info A token inforamtion class.
   */
  void SetInformationForNode(const TokenInfo& token_info) YATSC_NOEXCEPT;
  

  /**
   * Set source information to this node and children.
   * @param token_info A token inforamtion class.
   */
  void SetInformationForTree(const TokenInfo& token_info) YATSC_NOEXCEPT;

  
  /**
   * Set source information to this node.
   * @param token_info A token inforamtion class.
   */
  void SetInformationForNode(const Node* node) YATSC_NOEXCEPT;


  /**
   * Set source information to this node and children.
   * @param token_info A token inforamtion class.
   */
  void SetInformationForTree(const Node* node) YATSC_NOEXCEPT;


  /**
   * Clone this node and all children.
   * @returns Cloned node tree.
   */
  Node* Clone() YATSC_NOEXCEPT;

  
#define DEF_CAST(type)                                                  \
  YATSC_INLINE type* To##type() YATSC_NOEXCEPT {                          \
    return node_type_ == NodeType::k##type? reinterpret_cast<type*>(this): nullptr; \
  }                                                                     \
  YATSC_INLINE bool Has##type() YATSC_NO_SE {                             \
    return node_type_ == NodeType::k##type;                             \
  }


#define DECLARE_CAST(ViewName) DEF_CAST(ViewName);
  // Define cast methods like To[ViewName].
VIEW_LIST(DECLARE_CAST, DECLARE_CAST, DECLARE_CAST)
#undef DEF_CAST
#undef DECLARE_CAST

  
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
  
  List node_list_;
  
 private:
  std::bitset<8> flags_;
  SourceInformation source_information_;
  NodeType node_type_;
  size_t capacity_;
  Node* parent_node_;
  Token operand_;
  double double_value_;
  bool invalid_lhs_;
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

  
  FileScopeView(Node* body):
      Node(NodeType::kFileScopeView, 1u, {body}){};
};


// Represent statement.
class StatementView : public Node {
 public:
  StatementView():
      Node(NodeType::kStatementView, 1u){}

  StatementView(Node* expr):
      Node(NodeType::kStatementView, 1u, {expr}) {}

  // Getter and Setter for exp.
  NODE_PROPERTY(expr, 0);
};


// Represent variable declarations.
class VariableDeclView: public Node {
 public:
  VariableDeclView():
      Node(NodeType::kVariableDeclView, 0) {}

  
  VariableDeclView(std::initializer_list<Node*> vars):
      Node(NodeType::kVariableDeclView, 0, vars) {}
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


  ModuleDeclView(Node* name, Node* body):
      Node(NodeType::kModuleDeclView, 2u, {name, body}) {}

  
  NODE_PROPERTY(name, 0);

  NODE_PROPERTY(body, 1);
};


// Represent export.
class ExportView: public Node {
 public:
  ExportView(Node* target)
      : Node(NodeType::kExportView, 1u, {target}) {}


  ExportView()
      : Node(NodeType::kExportView, 1u) {}

  
  NODE_PROPERTY(target, 0);
};


// Represent import.
class ImportView: public Node {
 public:
  ImportView(Node* alias, Node* from_expr)
      : Node(NodeType::kImportView, 2u, {alias, from_expr}) {}


  ImportView()
      : Node(NodeType::kImportView, 2u) {}

  
  NODE_PROPERTY(alias, 0);

  
  NODE_PROPERTY(from_expr, 1);
};


// Represent variable.
class VariableView : public Node {
 public:
  VariableView(Node* name, Node* value, Node* type):
      Node(NodeType::kVariableView, 3u, {name, value, type}) {}


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
      : Node(NodeType::kIfStatementView, 2u, {if_block_Node, else_block_node}) {}


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
      : Node(NodeType::kReturnStatementView, 1u, {expr}) {}


  ReturnStatementView()
      : Node(NodeType::kReturnStatementView, 1u) {}
  

  // Getter and Setter for expr_.
  NODE_PROPERTY(expr, 0);
};


// Represent break statement.
class BreakStatementView: public Node {
 public:
  BreakStatementView(Node* label)
      : Node(NodeType::kBreakStatementView, 1u, {label}) {}

  BreakStatementView()
      : Node(NodeType::kBreakStatementView, 1u) {}


  // Getter and Setter for label.
  NODE_PROPERTY(label, 0);
};


// Represent with statement.
class WithStatementView: public Node {
 public:
  WithStatementView(Node* expr, Node* statement)
      : Node(NodeType::kWithStatementView, 2u, {expr, statement}) {}


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
      : Node(NodeType::kLabelledStatementView, 2u, {name, statement}) {}


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
      : Node(NodeType::kSwitchStatementView, 2u, {case_list, default_case}) {}


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
      : Node(NodeType::kCaseListView, 0, case_list) {}


  CaseListView()
      : Node(NodeType::kCaseListView, 0) {}
};


// Represent case.
class CaseView: public Node {
 public:
  CaseView(Node* condition, Node* body)
      : Node(NodeType::kCaseView, 2u, {condition, body}) {}


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
      : Node(NodeType::kTryStatementView, 3u, {statement, catch_statement, finally_statement}) {}

  
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
      : Node(NodeType::kCatchStatementView, 2u, {error_name, body}) {}


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
      : Node(NodeType::kFinallyStatementView, 1u, {body}) {}


  FinallyStatementView()
      : Node(NodeType::kFinallyStatementView, 1u) {}


  NODE_PROPERTY(body, 0);
};


// Represent throw statement.
class ThrowStatementView: public Node {
 public:
  ThrowStatementView(Node* expr)
      : Node(NodeType::kThrowStatementView, 1u, {expr}) {}

  
  // Getter and Setter for expr.
  NODE_PROPERTY(expr, 0);
};


class ForStatementView: public Node {
 public:
  ForStatementView(Node* cond_init, Node* cond_cmp, Node* cond_upd, Node* body)
      : Node(NodeType::kForStatementView, 4u, {cond_init, cond_cmp, cond_upd, body}) {}

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
      : Node(NodeType::kForInStatementView, 3u, {property_name, expr, body}) {}

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
      : Node(NodeType::kWhileStatementView, 2u, {expr, body}) {}

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
      : Node(NodeType::kDoWhileStatementView, 2u, {expr, body}) {}


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
      : Node(NodeType::kClassDeclView, 5u, {name, constructor, field_list, inheritance, impl_list}) {}

  
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
      : Node(NodeType::kClassFieldListView, 0, fields) {}


  ClassFieldListView()
      : Node(NodeType::kClassFieldListView, 0) {}
};


class ClassFieldAccessLevelView: public Node {
 public:
  ClassFieldAccessLevelView(Token op)
      : Node(NodeType::kClassFieldAccessLevelView, 0u) {
    set_operand(op);
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
      : Node(type, 3u, {access_level, name, value}) {}

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
      : Node(NodeType::kInterfaceView, 2u, {name, interface_field_list}) {}

  InterfaceView()
      : Node(NodeType::kInterfaceView, 2u) {}

  
  // Getter and Setter for name.
  NODE_PROPERTY(name, 0);


  // Getter and Setter for name.
  NODE_PROPERTY(field_list, 1);
};


class InterfaceFieldListView: public Node {
  InterfaceFieldListView(std::initializer_list<Node*> fields)
      : Node(NodeType::kInterfaceFieldListView, 0, fields) {}


  InterfaceFieldListView()
      : Node(NodeType::kInterfaceFieldListView, 0) {}
};


// Represent interface field.
class InterfaceFieldView: public Node {
 public:
  InterfaceFieldView(Node* name, Node* value):
      Node(NodeType::kInterfaceFieldView, 2u, {name, value}) {}


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
      : Node(NodeType::kSimpleTypeExprView, 1u, {type_name}) {}
  

  // Getter and Setter for type_name_.
  NODE_PROPERTY(type_name, 0);
};


// Represent type expression like `var x: string`
class ArrayTypeExprView: public Node {
 public:
  
  ArrayTypeExprView(Node* type_name)
      : Node(NodeType::kArrayTypeExprView, 1u, {type_name}) {}
  

  // Getter and Setter for type_name_.
  NODE_PROPERTY(type_name, 0);
};


// Represent type expression like `var x: string`
class ObjectTypeExprView: public Node {
 public:
  
  ObjectTypeExprView(std::initializer_list<Node*> type_member_list)
      : Node(NodeType::kArrayTypeExprView, 0u, type_member_list) {}
  

  ObjectTypeExprView()
      : Node(NodeType::kArrayTypeExprView, 0u) {}
};


class PropertySignatureView: public Node {
 public:
  PropertySignatureView(bool opt, Node* property_name, Node* type_expr)
      : Node(NodeType::kPropertySignatureView, 2u, {property_name, type_expr}) {
    if (opt) {
      set_flag(0);
    }
  }


  PropertySignatureView()
      : Node(NodeType::kPropertySignatureView, 2u) {}
  

  NODE_PROPERTY(property_name, 0);


  NODE_PROPERTY(type_expr, 1);


  NODE_FLAG_PROPERTY(optional, 0);
};


class MethodSignatureView: public Node {
 public:
  MethodSignatureView(bool opt, Node* property_name, Node* type_expr)
      : Node(NodeType::kMethodSignatureView, 2u, {property_name, type_expr}) {
    if (opt) {
      set_flag(0);
    }
  }


  MethodSignatureView()
      : Node(NodeType::kMethodSignatureView, 2u) {}
  

  NODE_PROPERTY(property_name, 0);


  NODE_PROPERTY(type_expr, 1);


  NODE_FLAG_PROPERTY(optional, 0);
};


// Represent function type expression like, `var x:(a:string, b:string) => string;`
class FunctionTypeExprView: public Node {
 public:
  FunctionTypeExprView(Node* param_list, Node* return_type)
      : Node(NodeType::kFunctionTypeExprView, 2u, {param_list, return_type}) {}


  FunctionTypeExprView()
      : Node(NodeType::kFunctionTypeExprView, 2u) {}


  // Getter and setter for param_list_.
  NODE_PROPERTY(param_list, 0);
  

  // Getter and setter for return_type_.
  NODE_PROPERTY(return_type, 1);
};


// Represent function type expression like, `var x:(a:string, b:string) => string;`
class ConstructSignatureView: public Node {
 public:
  ConstructSignatureView(Node* call_signature)
      : Node(NodeType::kConstructSignatureView, 1u, {call_signature}) {}


  ConstructSignatureView()
      : Node(NodeType::kConstructSignatureView, 1u) {}


  // Getter and setter for type_parameters.
  NODE_PROPERTY(call_signature, 0);
};


// Represent accessor type expression like, `interface x {[index:int]}`
class AccessorTypeExprView: public Node {
 public:
  AccessorTypeExprView(Node* name, Node* type_expression):
      Node(NodeType::kAccessorTypeExprView, 2u, {name, type_expression}) {}

  
  // Getter and setter for name_.
  NODE_PROPERTY(name, 0);


  // Getter and setter for type_expr.
  NODE_PROPERTY(type_expr, 1);
};


class TypeParametersView: public Node {
 public:
  TypeParametersView(std::initializer_list<Node*> type_arguments)
      : Node(NodeType::kTypeParametersView, 0u, type_arguments) {}


  TypeParametersView()
      : Node(NodeType::kTypeParametersView, 0u) {}
};


// Represent accessor type expression like, `interface x {[index:int]}`
class TypeParameterView: public Node {
 public:
  TypeParameterView(Node* identifier, Node* constraint):
      Node(NodeType::kTypeParameterView, 2u, {identifier, constraint}) {}


  TypeParameterView():
      Node(NodeType::kTypeParameterView, 2u) {}


  NODE_PROPERTY(identifier, 0);


  NODE_PROPERTY(constraint, 1);
};


class CommaExprView: public Node {
 public:
  CommaExprView(std::initializer_list<Node*> exprs)
      : Node(NodeType::kCommaExprView, 0u, exprs) {}

  CommaExprView()
      : Node(NodeType::kCommaExprView, 0u) {}
};


// Represent function.
class FunctionView: public Node {
 public:
  FunctionView(Node* name, Node* param_list, Node* body)
      : Node(NodeType::kFunctionView, 3u, {name, param_list, body}) {}

  // Getter for name_.
  NODE_GETTER(name, 0);

  // Getter for param_list_.
  NODE_GETTER(param_list, 1);

  // Getter for body_.
  NODE_GETTER(body, 2);
};


// Represent function.
class ArrowFunctionView: public Node {
 public:
  ArrowFunctionView(Node* name, Node* param_list, Node* body)
      : Node(NodeType::kArrowFunctionView, 3u, {name, param_list, body}) {}

  // Getter for name_.
  NODE_GETTER(name, 0);

  // Getter for param_list_.
  NODE_GETTER(param_list, 1);

  // Getter for body_.
  NODE_GETTER(body, 2);
};


class ParameterView: public Node {
 public:
  ParameterView(bool optional, Node* expr, Node* access_level)
      : Node(NodeType::kParameterView, 1u, {expr, access_level}){
    if (optional) {
      set_flag(0);
    }
  }


  ParameterView()
      : Node(NodeType::kParameterView, 1u){}


  NODE_PROPERTY(expr, 0);

  
  NODE_PROPERTY(access_level, 1);
};


class RestParamView: public Node {
 public:
  RestParamView(Node* identifier)
      : Node(NodeType::kRestParamView, 1u, {identifier}){}


  NODE_PROPERTY(identifier, 0);
};


class ParamList: public Node {
 public:
  ParamList(std::initializer_list<Node*> args)
      : Node(NodeType::kParamList, 0u, args) {}


  ParamList()
      : Node(NodeType::kParamList, 0u) {}
};


class CallView: public Node {
 public:
  CallView(Node* target, Node* args)
      : Node(NodeType::kCallView, 2u, {target, args}) {}

  
  NODE_PROPERTY(target, 0);


  NODE_PROPERTY(args, 1);
};


class CallSinatureView: public Node {
 public:
  CallSinatureView(Node* param_list, Node* return_type)
      : Node(NodeType::kCallView, 2u, {param_list, return_type}) {}

  
  NODE_PROPERTY(param_list, 0);


  NODE_PROPERTY(return_type, 1);
};


class CallArgsView: public Node {
 public:
  CallArgsView(std::initializer_list<Node*> args)
      : Node(NodeType::kCallArgsView, 0, args) {}


  CallArgsView()
      : Node(NodeType::kCallArgsView, 0) {}
};


class NewCallView: public Node {
 public:
  NewCallView(Node* target, Node* args)
      : Node(NodeType::kNewCallView, 2u, {target, args}) {}


  NewCallView()
      : Node(NodeType::kNewCallView, 2u) {}
};


class NameView: public Node {
 public:
  NameView(UtfString name, Node* type = nullptr)
      : Node(NodeType::kNameView, 1u, {type}) {
    set_string_value(name);
  }

  NODE_PROPERTY(type_expr, 0);
};


class SuperView: public Node {
 public:
  SuperView()
      : Node(NodeType::kSuperView, 0u) {}
};


class PostfixView: public Node {
 public:
  PostfixView(Node* target, Token op)
      : Node(NodeType::kPostfixView, 1u, {target}) {
    set_operand(op);
  }

  PostfixView()
      : Node(NodeType::kPostfixView, 1u) {}
};


class GetPropView: public Node {
 public:
  GetPropView(Node* target, Node* prop)
      : Node(NodeType::kGetPropView, 2u, {target, prop}) {}

  // Getter and Setter for target.
  NODE_PROPERTY(target, 0);

  // Getter and Setter for prop.
  NODE_PROPERTY(prop, 1);
};


class GetElemView: public Node {
 public:
  GetElemView(Node* target, Node* prop)
      : Node(NodeType::kGetElemView, 2u, {target, prop}) {}

  
  // Getter and Setter for target.
  NODE_PROPERTY(target, 0);

  
  // Getter and Setter for prop.
  NODE_PROPERTY(prop, 1);
};


class AssignmentView: public Node {
 public:
  AssignmentView(Token op, Node* target, Node* expr)
      : Node(NodeType::kAssignmentView, 2u, {target, expr}) {
    set_operand(op);
  }


  // Getter and Setter for target.
  NODE_PROPERTY(target, 0);


  // Getter and Setter for expr.
  NODE_PROPERTY(expr, 1);
};


class TemaryExprView: public Node {
 public :
  TemaryExprView(Node* cond, Node* then_expr, Node* else_expr)
      : Node(NodeType::kTemaryExprView, 3u, {cond, then_expr, else_expr}) {}


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
      : Node(NodeType::kBinaryExprView, 2u, {first, second}) {
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
      : Node(NodeType::kCastView, 1u, {type_expr}) {}


  // Getter for type_expr.
  NODE_GETTER(type_expr, 0);
};


class UnaryExprView: public Node {
 public:
  UnaryExprView(Token op, Node* expr)
      : Node(NodeType::kUnaryExprView, 1u, {expr}) {
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


class NaNView: public Node {
 public:
  NaNView()
      : Node(NodeType::kNaNView, 0) {}
};


class StringView: public Node {
 public:
  StringView(UtfString str)
      : Node(NodeType::kStringView, 0) {
    set_string_value(str);
  }
};


class RegularExprView: public Node {
 public:
  RegularExprView(UtfString str)
      : Node(NodeType::kStringView, 0) {
    set_string_value(str);
  }
};


class ObjectElementView: public Node {
 public:
  ObjectElementView(Node* key, Node* value)
      : Node(NodeType::kObjectElementView, 2u, {key, value}) {}

  // Getter and setter for key_.
  NODE_PROPERTY(key, 0);


  // Getter and setter for value_.
  NODE_PROPERTY(value, 1);
};


class ObjectLiteralView: public Node {
 public:
  ObjectLiteralView(std::initializer_list<Node*> properties)
      : Node(NodeType::kObjectLiteralView, 0, properties) {}


  ObjectLiteralView()
      : Node(NodeType::kObjectLiteralView, 0) {}
};


class ArrayLiteralView: public Node {
 public:
  ArrayLiteralView(std::initializer_list<Node*> elements)
      : Node(NodeType::kArrayLiteralView, 0, elements) {}


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


}} //yatsc::ir

#undef NODE_PROPERTY
#undef NODE_GETTER
#undef NODE_SETTER
#undef DEF_CAST
#undef VIEW_LIST

#endif

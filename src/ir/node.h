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
#include "../utils/stl.h"
#include "../memory/heap.h"
#include "../parser/token.h"

namespace yatsc {namespace ir {

// The list of the views.
#define VIEW_LIST(DECLARE, DECLARE_FIRST, DECLARE_LAST) \
  DECLARE_FIRST(Node)                                   \
  DECLARE(FileScopeView)                                \
  DECLARE(StatementView)                                \
  DECLARE(VariableDeclView)                             \
  DECLARE(LexicalDeclView)                              \
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
  DECLARE(CaseBody)                                     \
  DECLARE(CaseListView)                                 \
  DECLARE(TryStatementView)                             \
  DECLARE(CatchStatementView)                           \
  DECLARE(FinallyStatementView)                         \
  DECLARE(ThrowStatementView)                           \
  DECLARE(ForStatementView)                             \
  DECLARE(ForInStatementView)                           \
  DECLARE(ForOfStatementView)                           \
  DECLARE(WhileStatementView)                           \
  DECLARE(DoWhileStatementView)                         \
  DECLARE(ClassDeclView)                                \
  DECLARE(ClassBasesView)                               \
  DECLARE(ClassImplsView)                               \
  DECLARE(ClassFieldListView)                           \
  DECLARE(ClassFieldModifiersView)                      \
  DECLARE(ClassFieldAccessLevelView)                    \
  DECLARE(ClassHeritageView)                            \
  DECLARE(MemberVariableView)                           \
  DECLARE(MemberFunctionDefinitionView)                 \
  DECLARE(MemberFunctionOverloadsView)                  \
  DECLARE(MemberFunctionOverloadView)                   \
  DECLARE(MemberFunctionView)                           \
  DECLARE(InterfaceView)                                \
  DECLARE(InterfaceExtendsView)                         \
  DECLARE(SimpleTypeExprView)                           \
  DECLARE(IndexSignatureView)                           \
  DECLARE(GenericTypeExprView)                          \
  DECLARE(TypeConstraintsView)                          \
  DECLARE(TypeArgumentsView)                            \
  DECLARE(TypeQueryView)                                \
  DECLARE(ArrayTypeExprView)                            \
  DECLARE(ObjectTypeExprView)                           \
  DECLARE(PropertySignatureView)                        \
  DECLARE(MethodSignatureView)                          \
  DECLARE(FunctionTypeExprView)                         \
  DECLARE(ConstructSignatureView)                       \
  DECLARE(AccessorTypeExprView)                         \
  DECLARE(TypeParametersView)                           \
  DECLARE(CommaExprView)                                \
  DECLARE(FunctionOverloadsView)                        \
  DECLARE(FunctionOverloadView)                         \
  DECLARE(FunctionView)                                 \
  DECLARE(ArrowFunctionView)                            \
  DECLARE(ParameterView)                                \
  DECLARE(RestParamView)                                \
  DECLARE(ParamList)                                    \
  DECLARE(CallView)                                     \
  DECLARE(CallSignatureView)                            \
  DECLARE(CallArgsView)                                 \
  DECLARE(NewCallView)                                  \
  DECLARE(NameView)                                     \
  DECLARE(BindingPropListView)                          \
  DECLARE(BindingArrayView)                             \
  DECLARE(BindingElementView)                           \
  DECLARE(DefaultView)                                  \
  DECLARE(YieldView)                                    \
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
  DECLARE(TemplateLiteralView)                          \
  DECLARE(ComprehensionExprView)                        \
  DECLARE(Empty)                                        \
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


static const char* kNodeTypeStringList[] = {
    // Forward declarations.
#define DECLARE(ViewName) #ViewName,
#define DECLARE_LAST(ViewName) #ViewName
    VIEW_LIST(DECLARE, DECLARE, DECLARE_LAST)
#undef DECLARE
#undef DECLARE_LAST
    // End forward declarations.
  };


// Define getter accessor.
#define NODE_GETTER(name, pos)                                        \
  YATSC_INLINE Handle<Node> name() YATSC_NOEXCEPT {return node_list_[pos];}


// Define setter accessor.
#define NODE_SETTER(name, pos)                            \
  YATSC_INLINE void set_##name(Handle<Node> name) {       \
    node_list_[pos] = name;                               \
    if (name) name->set_parent_node(Handle<Node>(this));  \
  }


// Define both getter and setter accessors.
#define NODE_PROPERTY(name, pos)                \
  NODE_GETTER(name, pos)                        \
  NODE_SETTER(name, pos)


#define NODE_FLAG_GETTER(name, pos)             \
  YATSC_INLINE bool name() {                    \
    return TestFlag(pos);                       \
  }


#define NODE_FLAG_SETTER(name, pos)             \
  YATSC_INLINE void set_##name(bool val) {      \
    if (val) {                                  \
      set_flag(pos);                            \
    } else {                                    \
      ClearFlag(pos);}                          \
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
  YATSC_CONST_GETTER(const SourcePosition&, source_position, source_position_);
  
 private:
  SourcePosition source_position_;
};


// The IR Tree representation class.
// This class has all tree properties and accessible from Node type.
// All other **View classes are only view of this Node class.
class Node : public heap::HeapReference, private Uncopyable, private Unmovable {
 public:

  typedef Vector<Handle<Node>> List;
  typedef List::iterator ListIterator;


  // Create Node.
  YATSC_INLINE Node(NodeType node_type, size_t capacity = 0)
      : node_type_(node_type),
        capacity_(capacity),
        parent_node_(nullptr),
        operand_(Token::ILLEGAL),
        double_value_(0l),
        invalid_lhs_(false) {
    if (capacity != 0) {
      node_list_.resize(capacity, nullptr);
    }
  }


  // Create Node.
  YATSC_INLINE Node(NodeType node_type, size_t capacity, std::initializer_list<Handle<Node>> node_list)
      : node_list_(node_list),
        node_type_(node_type),
        capacity_(capacity),
        parent_node_(nullptr),
        operand_(Token::ILLEGAL),
        double_value_(0l),
        invalid_lhs_(false) {
    if (capacity != 0) {
      node_list_.resize(capacity, nullptr);
    }
  }

  
  virtual ~Node() {}


  static Handle<Node> Null() {return Handle<Node>();}


  // Getter for node_type.
  YATSC_CONST_GETTER(NodeType, node_type, node_type_);


  YATSC_CONST_GETTER(const SourcePosition&, source_position, source_information_.source_position());


  // Getter and setter for parent_node_.
  YATSC_PROPERTY(Handle<Node>, parent_node, parent_node_);


  // Getter for children list.
  YATSC_CONST_GETTER(const List&, node_list, node_list_);

  
  // Getter for size.
  YATSC_CONST_GETTER(size_t, size, node_list_.size());
  

  YATSC_CONST_GETTER(Handle<Node>, first_child, node_list_.front());


  YATSC_CONST_GETTER(Handle<Node>, last_child, node_list_.back());


  YATSC_INLINE bool HasLiteralView() YATSC_NO_SE {
    return node_type_ == NodeType::kNullView
      || node_type_ == NodeType::kTrueView
      || node_type_ == NodeType::kFalseView
      || node_type_ == NodeType::kStringView
      || node_type_ == NodeType::kNumberView
      || node_type_ == NodeType::kRegularExprView
      || node_type_ == NodeType::kUndefinedView
      || node_type_ == NodeType::kNaNView;
  }


  YATSC_INLINE bool HasKeywordLiteralView() YATSC_NO_SE {
    return node_type_ == NodeType::kNullView
      || node_type_ == NodeType::kTrueView
      || node_type_ == NodeType::kFalseView
      || node_type_ == NodeType::kUndefinedView
      || node_type_ == NodeType::kNaNView;
  }
  

  // Mark this node as invalid for the left-hand-side-expression.
  void MarkAsInValidLhs() {invalid_lhs_ = true;}


  // Return wheter this node is valid lef-hand-side-expression or not.
  bool IsValidLhs() {return !invalid_lhs_;}
  

  // Insert a node at the end of the children.
  void InsertLast(Handle<Node> node);


  // Insert a node to the front of the children.
  void InsertFront(Handle<Node> node);


  // Insert a new node to before specified node.
  void InsertBefore(Handle<Node> newNode, Handle<Node> oldNode);


  // Insert a new node after the specified node.
  void InsertAfter(Handle<Node> newNode, Handle<Node> oldNode);


  void InsertAt(size_t index, Handle<Node> node);


  Handle<Node> at(size_t index) YATSC_NOEXCEPT {
    return node_list_.size() > index? node_list_[index]: nullptr;
  }


  // Set string value.
  YATSC_INLINE void set_string_value(UtfString str) YATSC_NOEXCEPT {
    string_value_ = std::move(str);
  }


  // Return string value.
  YATSC_INLINE const UtfString& string_value() YATSC_NO_SE {
    return string_value_;
  }


  YATSC_INLINE bool string_equals(const Handle<Node>& node) YATSC_NO_SE {
    if (node == nullptr) {
      return false;
    }
    const UtfString& utf_string = node->string_value();
    return utf_string == string_value_;
  }


  // Return string value.
  YATSC_INLINE const char* utf8_value() YATSC_NO_SE {
    return string_value_.utf8_value();
  }


  // Set double value.
  YATSC_INLINE void set_double_value(double d) YATSC_NOEXCEPT {
    double_value_ = d;
  }


  // Return double value.
  YATSC_INLINE double double_value() YATSC_NO_SE {
    return double_value_;
  }


  YATSC_INLINE bool double_equals(Handle<Node> node) YATSC_NO_SE {
    return double_value_ == node->double_value_;
  }


  // Set an operand.
  YATSC_INLINE void set_operand(Token op) YATSC_NOEXCEPT {
    operand_ = op;
  }


  YATSC_INLINE bool operand_equals(const Handle<Node>& node) YATSC_NO_SE {
    return operand_ == node->operand_;
  }


  YATSC_INLINE void set_flag(int pos) YATSC_NOEXCEPT {
    flags_.set(pos);
  }


  YATSC_INLINE void set_flag(int pos, bool value) YATSC_NOEXCEPT {
    if (value) {
      flags_.set(pos);
    }
  }


  YATSC_INLINE bool TestFlag(int pos) YATSC_NO_SE {
    return 1 == flags_.test(pos);
  }


  YATSC_INLINE void ClearFlag(size_t pos) YATSC_NOEXCEPT {
    flags_.reset(pos);
  }


  // Return an operand.
  YATSC_INLINE Token operand() YATSC_NO_SE {
    return operand_;
  }


  // Remove specified node from children.
  YATSC_INLINE void Remove(Handle<Node> block) {
    node_list_.erase(std::remove(node_list_.begin(), node_list_.end(), block), node_list_.end());
    block->set_parent_node(nullptr);
  }


  // Remove a specified iterator from children.
  YATSC_INLINE void Remove(Node::ListIterator iterator) {
    node_list_.erase(iterator);
    (*iterator)->set_parent_node(nullptr);
  }


  // Set source information to this node.
  void SetInformationForNode(const SourcePosition& source_position) YATSC_NOEXCEPT;
  

  // Set source information to this node.
  void SetInformationForNode(const TokenInfo& token_info) YATSC_NOEXCEPT;
  

  // Set source information to this node and children.
  void SetInformationForTree(const TokenInfo& token_info) YATSC_NOEXCEPT;


  // Set source information to this node.
  void SetInformationForNode(const TokenInfo* token_info) YATSC_NOEXCEPT {SetInformationForNode(*token_info);}
  

  // Set source information to this node and children.
  void SetInformationForTree(const TokenInfo* token_info) YATSC_NOEXCEPT {SetInformationForTree(*token_info);}

  
  // Set source information to this node.
  void SetInformationForNode(const Handle<Node>& node) YATSC_NOEXCEPT;


  // Set source information to this node and children.
  void SetInformationForTree(const Handle<Node>& node) YATSC_NOEXCEPT;

  bool Equals(const Handle<Node>& node) YATSC_NO_SE;

  // Clone this node and all children.
  Handle<Node> Clone() YATSC_NOEXCEPT;

  
#define DEF_CAST(type)                                                  \
  YATSC_INLINE type* To##type() YATSC_NOEXCEPT {                        \
    return node_type_ == NodeType::k##type? reinterpret_cast<type*>(this): nullptr; \
  }                                                                     \
  YATSC_INLINE bool Has##type() YATSC_NO_SE {                           \
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
  String ToString() const;

  /**
   * Return formated string expression of this node and children.
   * @returns The string expression of this node and children.
   */
  String ToStringTree() const;

 protected:
  
  List node_list_;
  
 private:

  void DoToStringTree(String& indent, StringStream& ss) const;


  void ToStringSelf(const Node* target, String& indent, StringStream& ss) const;
  
  std::bitset<8> flags_;
  SourceInformation source_information_;
  NodeType node_type_;
  size_t capacity_;
  Handle<Node> parent_node_;
  Token operand_;
  double double_value_;
  bool invalid_lhs_;
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

  
  FileScopeView(Handle<Node> body):
      Node(NodeType::kFileScopeView, 1u, {body}){};
};


// Represent statement.
class StatementView : public Node {
 public:
  StatementView():
      Node(NodeType::kStatementView, 1u){}

  StatementView(Handle<Node> expr):
      Node(NodeType::kStatementView, 1u, {expr}) {}

  // Getter and Setter for exp.
  NODE_PROPERTY(expr, 0);
};


// Represent variable declarations.
class VariableDeclView: public Node {
 public:
  VariableDeclView():
      Node(NodeType::kVariableDeclView, 0) {}

  
  VariableDeclView(std::initializer_list<Handle<Node>> vars):
      Node(NodeType::kVariableDeclView, 0, vars) {}
};


// Represent variable declarations.
class LexicalDeclView: public Node {
 public:
  LexicalDeclView(Token op):
      Node(NodeType::kLexicalDeclView, 0) {
    set_operand(op);
  }

  
  LexicalDeclView(Token op, std::initializer_list<Handle<Node>> vars):
      Node(NodeType::kLexicalDeclView, 0, vars) {
    set_operand(op);
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


  ModuleDeclView(Handle<Node> name, Handle<Node> body):
      Node(NodeType::kModuleDeclView, 2u, {name, body}) {}

  
  NODE_PROPERTY(name, 0);

  NODE_PROPERTY(body, 1);
};


// Represent export.
class ExportView: public Node {
 public:
  ExportView(Handle<Node> target)
      : Node(NodeType::kExportView, 1u, {target}) {}


  ExportView()
      : Node(NodeType::kExportView, 1u) {}

  
  NODE_PROPERTY(target, 0);
};


// Represent import.
class ImportView: public Node {
 public:
  ImportView(Handle<Node> alias, Handle<Node> from_expr)
      : Node(NodeType::kImportView, 2u, {alias, from_expr}) {}


  ImportView()
      : Node(NodeType::kImportView, 2u) {}

  
  NODE_PROPERTY(alias, 0);

  
  NODE_PROPERTY(from_expr, 1);
};


// Represent variable.
class VariableView : public Node {
 public:
  VariableView(Handle<Node> binding_identifier, Handle<Node> value, Handle<Node> type):
      Node(NodeType::kVariableView, 3u, {binding_identifier, value, type}) {}


  VariableView():
      Node(NodeType::kVariableView, 3u) {}


  NODE_PROPERTY(binding_identifier, 0);
  NODE_PROPERTY(value, 1);
  NODE_PROPERTY(type, 2);
};


// Represent if statement.
class IfStatementView : public Node {
 public:
  IfStatementView(Handle<Node> expr, Handle<Node> if_block_Node, Handle<Node> else_block_node)
      : Node(NodeType::kIfStatementView, 3u, {expr, if_block_Node, else_block_node}) {}


  IfStatementView()
      : Node(NodeType::kIfStatementView, 3u) {}

  // Getter and setter for expr.
  NODE_PROPERTY(expr, 0);
  
  // Getter and Setter for then_block.
  NODE_PROPERTY(then_block, 1);
  
  // Getter and Setter for else_block.
  NODE_PROPERTY(else_block, 2);
};


// Represent continue statement.
class ContinueStatementView: public Node {
 public:
  ContinueStatementView()
      : Node(NodeType::kContinueStatementView, 1u){}


  ContinueStatementView(Handle<Node> labelled_identifier)
      : Node(NodeType::kContinueStatementView, 1u, {labelled_identifier}) {}


  NODE_PROPERTY(label, 0);
};


// Represent return statement.
class ReturnStatementView: public Node {
 public:
  ReturnStatementView(Handle<Node> expr)
      : Node(NodeType::kReturnStatementView, 1u, {expr}) {}


  ReturnStatementView()
      : Node(NodeType::kReturnStatementView, 1u) {}
  

  // Getter and Setter for expr_.
  NODE_PROPERTY(expr, 0);
};


// Represent break statement.
class BreakStatementView: public Node {
 public:
  BreakStatementView(Handle<Node> label)
      : Node(NodeType::kBreakStatementView, 1u, {label}) {}

  BreakStatementView()
      : Node(NodeType::kBreakStatementView, 1u) {}


  // Getter and Setter for label.
  NODE_PROPERTY(label, 0);
};


// Represent with statement.
class WithStatementView: public Node {
 public:
  WithStatementView(Handle<Node> expr, Handle<Node> statement)
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
  LabelledStatementView(Handle<Node> name, Handle<Node> statement)
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
  SwitchStatementView(Handle<Node> expr, Handle<Node> case_list)
      : Node(NodeType::kSwitchStatementView, 2u, {expr, case_list}) {}


  SwitchStatementView()
      : Node(NodeType::kSwitchStatementView, 2u) {}


  // Getter and Setter for expr.
  NODE_PROPERTY(expr, 0);
  

  // Getter and Setter for case_list.
  NODE_PROPERTY(case_list, 1);
};


class CaseListView: public Node {
 public:
  CaseListView(std::initializer_list<Handle<Node>> case_list)
      : Node(NodeType::kCaseListView, 0, case_list) {}


  CaseListView()
      : Node(NodeType::kCaseListView, 0) {}
};


// Represent case.
class CaseView: public Node {
 public:
  CaseView(Handle<Node> condition, Handle<Node> body)
      : Node(NodeType::kCaseView, 2u, {condition, body}) {}


  CaseView()
      : Node(NodeType::kCaseView, 2u) {}


  // Getter and Setter for condition.
  NODE_PROPERTY(condition, 0);


  // Getter and Setter for body.
  NODE_PROPERTY(body, 1);
};


// Represent case.
class CaseBody: public Node {
 public:
  CaseBody(std::initializer_list<Handle<Node>> case_body)
      : Node(NodeType::kCaseBody, 0u, case_body) {}


  CaseBody()
      : Node(NodeType::kCaseBody, 0u) {}
};


// Represent try catch finally statement.
class TryStatementView: public Node {
 public:
  TryStatementView(Handle<Node> statement, Handle<Node> catch_statement, Handle<Node> finally_statement)
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
  CatchStatementView(Handle<Node> error_name, Handle<Node> body)
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
  FinallyStatementView(Handle<Node> body)
      : Node(NodeType::kFinallyStatementView, 1u, {body}) {}


  FinallyStatementView()
      : Node(NodeType::kFinallyStatementView, 1u) {}


  NODE_PROPERTY(body, 0);
};


// Represent throw statement.
class ThrowStatementView: public Node {
 public:
  ThrowStatementView(Handle<Node> expr)
      : Node(NodeType::kThrowStatementView, 1u, {expr}) {}

  
  // Getter and Setter for expr.
  NODE_PROPERTY(expr, 0);
};


class ForStatementView: public Node {
 public:
  ForStatementView(Handle<Node> cond_init, Handle<Node> cond_cmp, Handle<Node> cond_upd, Handle<Node> body)
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
  ForInStatementView(Handle<Node> property_name, Handle<Node> expr, Handle<Node> body)
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


// Represent for in statement.
class ForOfStatementView: public Node {
 public:
  ForOfStatementView(Handle<Node> property_name, Handle<Node> expr, Handle<Node> body)
      : Node(NodeType::kForOfStatementView, 3u, {property_name, expr, body}) {}

  ForOfStatementView()
      : Node(NodeType::kForOfStatementView, 3u) {}


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
  WhileStatementView(Handle<Node> expr, Handle<Node> body)
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
  DoWhileStatementView(Handle<Node> expr, Handle<Node> body)
      : Node(NodeType::kDoWhileStatementView, 2u, {expr, body}) {}


  DoWhileStatementView()
      : Node(NodeType::kDoWhileStatementView, 2u) {}


  // Getter and Setter for expr.
  NODE_PROPERTY(expr, 0);


  // Getter and Setter for body .
  NODE_PROPERTY(body, 1);
};


class ClassBasesView: public Node {
 public:
  ClassBasesView(Handle<Node> base, Handle<Node> impls)
      : Node(NodeType::kClassBasesView, 2u, {base, impls}) {}

  ClassBasesView()
      : Node(NodeType::kClassBasesView, 2u) {}

  
  NODE_PROPERTY(base, 0);

  NODE_PROPERTY(impls, 1);
};


class ClassImplsView: public Node {
 public:
  ClassImplsView()
      : Node(NodeType::kClassImplsView, 0u) {}
};


class ClassDeclView: public Node {
 public:
  ClassDeclView(Handle<Node> name, Handle<Node> type_parameters, Handle<Node> bases, Handle<Node> field_list)
      : Node(NodeType::kClassDeclView, 4u, {name, type_parameters, bases, field_list}) {}
  
  ClassDeclView()
      : Node(NodeType::kClassDeclView, 4u) {}
  

  // Getter and Setter for name.
  NODE_PROPERTY(name, 0);


  // Getter and Setter for type_parameters.
  NODE_PROPERTY(type_parameters, 1);

  
    // Getter and Setter for inheritance.
  NODE_PROPERTY(bases, 2);

  
  // Getter and Setter for field_list.
  NODE_PROPERTY(field_list, 3);
};


class ClassHeritageView: public Node {
 public:
  ClassHeritageView(Handle<Node> ref)
      : Node(NodeType::kClassHeritageView, 1u, {ref}) {}


  ClassHeritageView()
      : Node(NodeType::kClassHeritageView, 1u) {}


  NODE_PROPERTY(type_reference, 0);
};


class ClassFieldListView: public Node {
 public:
  ClassFieldListView(std::initializer_list<Handle<Node>> fields)
      : Node(NodeType::kClassFieldListView, 0, fields) {}


  ClassFieldListView()
      : Node(NodeType::kClassFieldListView, 0) {}
};


class ClassFieldModifiersView: public Node {
 public:
  ClassFieldModifiersView(std::initializer_list<Handle<Node>> modifiers)
      : Node(NodeType::kClassFieldModifiersView, 0, modifiers) {}

  ClassFieldModifiersView()
      : Node(NodeType::kClassFieldModifiersView, 0) {}
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


class MemberVariableView: public Node {
 public:
  MemberVariableView(Handle<Node> accessor, Handle<Node> name, Handle<Node> type, Handle<Node> value)
      : Node(NodeType::kMemberVariableView, 4u, {accessor, name, type, value}) {}

  MemberVariableView()
      : Node(NodeType::kMemberVariableView, 4u) {}

  NODE_PROPERTY(accessor, 0);

  NODE_PROPERTY(name, 1);

  NODE_PROPERTY(type, 2)

  NODE_PROPERTY(value, 3);
};


class MemberFunctionDefinitionView: public Node {
 protected:
  MemberFunctionDefinitionView(NodeType node_type, bool getter, bool setter, bool generator, size_t size, std::initializer_list<Handle<Node>> list)
      : Node(node_type, size, list) {
    set_flag(0, getter);
    set_flag(1, setter);
    set_flag(2, generator);
  }

  MemberFunctionDefinitionView(NodeType node_type, size_t size, std::initializer_list<Handle<Node>> list)
      : Node(node_type, size, list) {}

  
  MemberFunctionDefinitionView(NodeType node_type, size_t size)
      : Node(node_type, size) {}

 public:


  NODE_PROPERTY(modifiers, 0)
  
  NODE_PROPERTY(name, 1);

  NODE_PROPERTY(call_signature, 2);

  NODE_FLAG_PROPERTY(getter, 0);

  NODE_FLAG_PROPERTY(setter, 1);

  NODE_FLAG_PROPERTY(generator, 2);
};


class MemberFunctionOverloadsView: public Node {
 public:
  MemberFunctionOverloadsView(std::initializer_list<Handle<Node>> overloads)
      : Node(NodeType::kMemberFunctionOverloadsView, 0u, overloads) {}

  MemberFunctionOverloadsView()
      : Node(NodeType::kMemberFunctionOverloadsView, 0u) {}
};


class MemberFunctionOverloadView: public MemberFunctionDefinitionView {
 public:
  MemberFunctionOverloadView(bool getter, bool setter, bool generator, Handle<Node> accessor, Handle<Node> name, Handle<Node> call_signature)
      : MemberFunctionDefinitionView(NodeType::kMemberFunctionOverloadView, getter, setter, generator, 3u, {accessor, name, call_signature}) {}

  MemberFunctionOverloadView(Handle<Node> accessor, Handle<Node> name, Handle<Node> call_signature)
      : MemberFunctionDefinitionView(NodeType::kMemberFunctionOverloadView, 3u, {accessor, name, call_signature}) {}

  MemberFunctionOverloadView()
      : MemberFunctionDefinitionView(NodeType::kMemberFunctionOverloadView, 3u) {}
};


class MemberFunctionView: public MemberFunctionDefinitionView {
 public:
  MemberFunctionView(bool getter, bool setter, bool generator, Handle<Node> accessor, Handle<Node> name, Handle<Node> call_signature, Handle<Node> overloads, Handle<Node> body)
      : MemberFunctionDefinitionView(NodeType::kMemberFunctionView, getter, setter, generator, 5u, {accessor, name, call_signature, overloads, body}) {}

  MemberFunctionView(Handle<Node> accessor, Handle<Node> name, Handle<Node> call_signature, Handle<Node> overloads, Handle<Node> body)
      : MemberFunctionDefinitionView(NodeType::kMemberFunctionView, 5u, {accessor, name, call_signature, overloads, body}) {}

  
  MemberFunctionView()
      : MemberFunctionDefinitionView(NodeType::kMemberFunctionView, 5u) {}

  
  NODE_PROPERTY(overloads, 3);
  
  // Getter for body_.
  NODE_PROPERTY(body, 4);
};


// Represent interface.
class InterfaceView: public Node {
 public:
  InterfaceView(Handle<Node> name, Handle<Node> type_parameters, Handle<Node> extends, Handle<Node> interface_field_list)
      : Node(NodeType::kInterfaceView, 4u, {name, type_parameters, extends, interface_field_list}) {}

  InterfaceView()
      : Node(NodeType::kInterfaceView, 4u) {}

  
  // Getter and Setter for name.
  NODE_PROPERTY(name, 0);


  // Getter and Setter for type_parameters.
  NODE_PROPERTY(type_parameters, 1);


  // Getter and Setter for impls.
  NODE_PROPERTY(extends, 2);


  // Getter and Setter for name.
  NODE_PROPERTY(field_list, 3);
};


class InterfaceExtendsView: public Node {
 public:
  InterfaceExtendsView(std::initializer_list<Handle<Node>> extends)
      : Node(NodeType::kInterfaceExtendsView, 0u, extends) {}


  InterfaceExtendsView()
      : Node(NodeType::kInterfaceExtendsView, 0u) {}
};


// Represent type expression like `var x: string`
class SimpleTypeExprView: public Node {
 public:
  
  SimpleTypeExprView(Handle<Node> type_name)
      : Node(NodeType::kSimpleTypeExprView, 1u, {type_name}) {}
  

  // Getter and Setter for type_name_.
  NODE_PROPERTY(type_name, 0);
};


class IndexSignatureView: public Node {
 public:
  IndexSignatureView(Handle<Node> identifier, Handle<Node> type, bool string_type)
      : Node(NodeType::kIndexSignatureView, 2u, {identifier, type}) {
    set_flag(0, string_type);
  }


  IndexSignatureView()
      : Node(NodeType::kIndexSignatureView, 2u) {}


  NODE_PROPERTY(identifier, 0);
  

  NODE_FLAG_PROPERTY(string_type, 0);
};


// Represent type expression like `var x: string`
class GenericTypeExprView: public Node {
 public:
  
  GenericTypeExprView(Handle<Node> type_name, Handle<Node> type_arguments)
      : Node(NodeType::kGenericTypeExprView, 2u, {type_name, type_arguments}) {}
  

  // Getter and Setter for type_name_.
  NODE_PROPERTY(type_name, 0);


  // Getter and Setter for type_name_.
  NODE_PROPERTY(type_arguments, 1);
};


// Represent type expression like `var x: string`
class TypeConstraintsView: public Node {
 public:
  
  TypeConstraintsView(Handle<Node> derived, Handle<Node> base)
      : Node(NodeType::kTypeConstraintsView, 2u, {derived, base}) {}
  

  // Getter and Setter for type_name_.
  NODE_PROPERTY(derived, 0);

  // Getter and Setter for type_name_.
  NODE_PROPERTY(base, 0);
};


class TypeArgumentsView: public Node {
 public:
  
  TypeArgumentsView()
      : Node(NodeType::kTypeArgumentsView, 0u) {}
};


class TypeQueryView: public Node {
 public:
  
  TypeQueryView(Handle<Node> var_name)
      : Node(NodeType::kTypeArgumentsView, 1u, {var_name}) {}


  TypeQueryView()
      : Node(NodeType::kTypeArgumentsView, 1u) {}
};


// Represent type expression like `var x: string`
class ArrayTypeExprView: public Node {
 public:
  
  ArrayTypeExprView(Handle<Node> type_name)
      : Node(NodeType::kArrayTypeExprView, 1u, {type_name}) {}
  

  // Getter and Setter for type_name_.
  NODE_PROPERTY(type_name, 0);
};


// Represent type expression like `var x: string`
class ObjectTypeExprView: public Node {
 public:
  
  ObjectTypeExprView(std::initializer_list<Handle<Node>> type_member_list)
      : Node(NodeType::kObjectTypeExprView, 0u, type_member_list) {}
  

  ObjectTypeExprView()
      : Node(NodeType::kObjectTypeExprView, 0u) {}
};


class PropertySignatureView: public Node {
 public:
  PropertySignatureView(bool opt, Handle<Node> property_name, Handle<Node> type_expr)
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
  MethodSignatureView(bool opt, bool getter, bool setter, bool generator, Handle<Node> property_name, Handle<Node> type_expr)
      : Node(NodeType::kMethodSignatureView, 2u, {property_name, type_expr}) {
    set_flag(0, opt);
    set_flag(1, getter);
    set_flag(2, setter);
    set_flag(3, generator);
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
  FunctionTypeExprView(Handle<Node> param_list, Handle<Node> return_type)
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
  ConstructSignatureView(Handle<Node> call_signature)
      : Node(NodeType::kConstructSignatureView, 1u, {call_signature}) {}


  ConstructSignatureView()
      : Node(NodeType::kConstructSignatureView, 1u) {}


  // Getter and setter for type_parameters.
  NODE_PROPERTY(call_signature, 0);
};


// Represent accessor type expression like, `interface x {[index:int]}`
class AccessorTypeExprView: public Node {
 public:
  AccessorTypeExprView(Handle<Node> name, Handle<Node> type_expression):
      Node(NodeType::kAccessorTypeExprView, 2u, {name, type_expression}) {}


  AccessorTypeExprView():
      Node(NodeType::kAccessorTypeExprView, 2u) {}

  
  // Getter and setter for name_.
  NODE_PROPERTY(name, 0);


  // Getter and setter for type_expr.
  NODE_PROPERTY(type_expr, 1);
};


class TypeParametersView: public Node {
 public:
  TypeParametersView(std::initializer_list<Handle<Node>> type_arguments)
      : Node(NodeType::kTypeParametersView, 0u, type_arguments) {}


  TypeParametersView()
      : Node(NodeType::kTypeParametersView, 0u) {}
};


class CommaExprView: public Node {
 public:
  CommaExprView(std::initializer_list<Handle<Node>> exprs)
      : Node(NodeType::kCommaExprView, 0u, exprs) {}

  CommaExprView()
      : Node(NodeType::kCommaExprView, 0u) {}
};


class FunctionOverloadView: public Node {
 public:
  FunctionOverloadView(bool generator, Handle<Node> name, Handle<Node> call_signature)
      : Node(NodeType::kFunctionOverloadView, 2u, {name, call_signature}) {
    if (generator) {
      set_flag(0);
    }
  }

  FunctionOverloadView()
      : Node(NodeType::kFunctionOverloadView, 2u) {}

  NODE_PROPERTY(name, 0);

  NODE_PROPERTY(call_signature, 1);

  NODE_FLAG_PROPERTY(generator, 0);
};


class FunctionOverloadsView: public Node {
 public:
  FunctionOverloadsView(std::initializer_list<Handle<Node>> overloads)
      : Node(NodeType::kFunctionOverloadsView, 0u, overloads) {}

  FunctionOverloadsView()
      : Node(NodeType::kFunctionOverloadsView, 0u) {}
};


// Represent function.
class FunctionView: public Node {
 public:
  FunctionView(bool getter, bool setter, bool generator, Handle<Node> overloads, Handle<Node> name, Handle<Node> call_signature, Handle<Node> body)
      : Node(NodeType::kFunctionView, 4u, {overloads, name, call_signature, body}) {
    if (getter) {
      set_flag(0);
    } else if (setter) {
      set_flag(1);
    } else if (generator) {
      set_flag(2);
    }
  }

  FunctionView(Handle<Node> overloads, Handle<Node> name, Handle<Node> call_signature, Handle<Node> body)
      : Node(NodeType::kFunctionView, 4u, {overloads, name, call_signature, body}) {}

  
  FunctionView()
      : Node(NodeType::kFunctionView, 4u) {}

  NODE_PROPERTY(overloads, 0);
  
  // Getter for name_.
  NODE_GETTER(name, 1);

  // Getter for param_list_.
  NODE_GETTER(call_signature, 2);

  // Getter for body_.
  NODE_GETTER(body, 3);

  NODE_FLAG_PROPERTY(getter, 0);

  NODE_FLAG_PROPERTY(setter, 1);

  NODE_FLAG_PROPERTY(generator, 2);
};


// Represent function.
class ArrowFunctionView: public Node {
 public:
  ArrowFunctionView(Handle<Node> call_signature, Handle<Node> body)
      : Node(NodeType::kArrowFunctionView, 2u, {call_signature, body}) {}

  // Getter for param_list_.
  NODE_GETTER(call_signature, 0);

  // Getter for body_.
  NODE_GETTER(body, 1);
};


class ParameterView: public Node {
 public:
  ParameterView(bool optional, Handle<Node> name, Handle<Node> value, Handle<Node> type_expr, Handle<Node> access_level)
      : Node(NodeType::kParameterView, 4u, {name, value, type_expr, access_level}){
    if (optional) {
      set_flag(0);
    }
  }


  ParameterView()
      : Node(NodeType::kParameterView, 4u){}


  NODE_PROPERTY(name, 0);

  NODE_PROPERTY(value, 1);

  NODE_PROPERTY(type_expr, 2);
  
  NODE_PROPERTY(access_level, 3);

  NODE_FLAG_PROPERTY(optional, 0);
};


class RestParamView: public Node {
 public:
  RestParamView(Handle<Node> parameter)
      : Node(NodeType::kRestParamView, 1u, {parameter}){}

  
  RestParamView()
      : Node(NodeType::kRestParamView, 1u){}


  NODE_PROPERTY(parameter, 0);
};


class ParamList: public Node {
 public:
  ParamList(std::initializer_list<Handle<Node>> args)
      : Node(NodeType::kParamList, 0u, args) {}


  ParamList()
      : Node(NodeType::kParamList, 0u) {}
};


class CallView: public Node {
 public:
  CallView(Handle<Node> target, Handle<Node> args, Handle<Node> type_expr)
      : Node(NodeType::kCallView, 3u, {target, args, type_expr}) {}

  
  NODE_PROPERTY(target, 0);


  NODE_PROPERTY(args, 1);

  
  NODE_PROPERTY(type_expr, 2);
};


class CallSignatureView: public Node {
 public:
  CallSignatureView(Handle<Node> param_list, Handle<Node> return_type, Handle<Node> type_parameters)
      : Node(NodeType::kCallSignatureView, 3u, {param_list, return_type, type_parameters}) {}

  
  NODE_PROPERTY(param_list, 0);


  NODE_PROPERTY(return_type, 1);

  
  NODE_PROPERTY(type_parameters, 2);
};


class CallArgsView: public Node {
 public:
  CallArgsView(std::initializer_list<Handle<Node>> args)
      : Node(NodeType::kCallArgsView, 0, args) {}


  CallArgsView()
      : Node(NodeType::kCallArgsView, 0) {}
};


class NewCallView: public Node {
 public:
  NewCallView(Handle<Node> target, Handle<Node> args, Handle<Node> type_expr)
      : Node(NodeType::kNewCallView, 3u, {target, args, type_expr}) {}


  NewCallView()
      : Node(NodeType::kNewCallView, 3u) {}


  NODE_PROPERTY(target, 0);


  NODE_PROPERTY(args, 1);

  
  NODE_PROPERTY(type_expr, 2);
};


class NameView: public Node {
 public:
  NameView(UtfString name)
      : Node(NodeType::kNameView, 0u) {
    set_string_value(std::move(name));
  }
};


class BindingPropListView: public Node {
 public:
  BindingPropListView()
      : Node(NodeType::kBindingPropListView, 0u) {}
};


class BindingArrayView: public Node {
 public:
  BindingArrayView()
      : Node(NodeType::kBindingArrayView, 0u) {}
};


class BindingElementView: public Node {
 public:
  BindingElementView(Handle<Node> prop, Handle<Node> elem, Handle<Node> init)
      : Node(NodeType::kBindingElementView, 3u, {prop, elem, init}) {}

  BindingElementView()
      : Node(NodeType::kBindingElementView, 3u) {}

  
  NODE_PROPERTY(prop, 0);
  NODE_PROPERTY(elem, 1);
  NODE_PROPERTY(init, 2);
};


class DefaultView: public Node {
 public:
  DefaultView()
      : Node(NodeType::kDefaultView, 0u) {}
};


class YieldView: public Node {
 public:
  YieldView(bool continuation, Handle<Node> expr)
      : Node(NodeType::kYieldView, 1u, {expr}) {
    if (continuation) {
      set_flag(0);
    }
  }

  YieldView()
      : Node(NodeType::kYieldView, 1u) {}

  NODE_PROPERTY(expr, 0);

  NODE_FLAG_PROPERTY(continuation, 0);
};


class SuperView: public Node {
 public:
  SuperView()
      : Node(NodeType::kSuperView, 0u) {}
};


class PostfixView: public Node {
 public:
  PostfixView(Handle<Node> target, Token op)
      : Node(NodeType::kPostfixView, 1u, {target}) {
    set_operand(op);
  }

  PostfixView()
      : Node(NodeType::kPostfixView, 1u) {}
};


class GetPropView: public Node {
 public:
  GetPropView(Handle<Node> target, Handle<Node> prop)
      : Node(NodeType::kGetPropView, 2u, {target, prop}) {}


  GetPropView()
      : Node(NodeType::kGetPropView, 2u) {}

  // Getter and Setter for target.
  NODE_PROPERTY(target, 0);

  // Getter and Setter for prop.
  NODE_PROPERTY(prop, 1);
};


class GetElemView: public Node {
 public:
  GetElemView(Handle<Node> target, Handle<Node> prop)
      : Node(NodeType::kGetElemView, 2u, {target, prop}) {}

  
  // Getter and Setter for target.
  NODE_PROPERTY(target, 0);

  
  // Getter and Setter for prop.
  NODE_PROPERTY(prop, 1);
};


class AssignmentView: public Node {
 public:
  AssignmentView(Token op, Handle<Node> target, Handle<Node> expr)
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
  TemaryExprView(Handle<Node> cond, Handle<Node> then_expr, Handle<Node> else_expr)
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
  BinaryExprView(Token op, Handle<Node> first, Handle<Node> second)
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
  CastView(Handle<Node> type_expr, Handle<Node> expr)
      : Node(NodeType::kCastView, 2u, {type_expr, expr}) {}


  // Getter and setter for type_expr.
  NODE_PROPERTY(type_expr, 0);

  // Getter and setter for expr.
  NODE_PROPERTY(expr, 1);
};


class UnaryExprView: public Node {
 public:
  UnaryExprView(Token op, Handle<Node> expr)
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
    const char* val = value.utf8_value();
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
      : Node(NodeType::kRegularExprView, 0) {
    set_string_value(str);
  }
};


class ObjectElementView: public Node {
 public:
  ObjectElementView(Handle<Node> key, Handle<Node> value)
      : Node(NodeType::kObjectElementView, 2u, {key, value}) {}

  // Getter and setter for key_.
  NODE_PROPERTY(key, 0);


  // Getter and setter for value_.
  NODE_PROPERTY(value, 1);
};


class ObjectLiteralView: public Node {
 public:
  ObjectLiteralView(std::initializer_list<Handle<Node>> properties)
      : Node(NodeType::kObjectLiteralView, 0, properties) {}


  ObjectLiteralView()
      : Node(NodeType::kObjectLiteralView, 0) {}
};


class ArrayLiteralView: public Node {
 public:
  ArrayLiteralView(std::initializer_list<Handle<Node>> elements)
      : Node(NodeType::kArrayLiteralView, 0, elements) {}


  ArrayLiteralView()
      : Node(NodeType::kArrayLiteralView, 0) {}
};


class UndefinedView: public Node {
 public:
  UndefinedView():
      Node(NodeType::kUndefinedView, 0){}
};


class TemplateLiteralView: public Node {
 public:
  TemplateLiteralView(UtfString expr):
      Node(NodeType::kUndefinedView, 0) {
    set_string_value(expr);
  }
};


class DebuggerView: public Node {
 public:
  DebuggerView():
      Node(NodeType::kDebuggerView, 0){}
};


class ComprehensionExprView: public Node {
 public:
  ComprehensionExprView(bool generator, Handle<Node> for_expr, Handle<Node> tail)
      : Node(NodeType::kComprehensionExprView, 2, {for_expr, tail}) {
    if (generator) {
      set_flag(0);
    }
  }

  ComprehensionExprView()
      : Node(NodeType::kComprehensionExprView, 2) {}

  
  NODE_PROPERTY(for_expr, 0);

  NODE_PROPERTY(tail, 1);

  NODE_FLAG_PROPERTY(generator, 0);
};


class Empty: public Node {
 public:
  Empty()
      : Node(NodeType::kEmpty, 0){}
};

}} //yatsc::ir

#undef NODE_PROPERTY
#undef NODE_GETTER
#undef NODE_SETTER
#undef DEF_CAST
#undef VIEW_LIST

#endif

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
#include <math.h>
#include <float.h>
#include "../utils/utils.h"
#include "../utils/stl.h"
#include "../memory/heap.h"
#include "../parser/token.h"
#include "./scope.h"
#include "./properties.h"
#include "./symbol.h"

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
  DECLARE(NamedExportListView)                          \
  DECLARE(NamedExportView)                              \
  DECLARE(ImportView)                                   \
  DECLARE(ImportClauseView)                             \
  DECLARE(NamedImportListView)                          \
  DECLARE(NamedImportView)                              \
  DECLARE(ExternalModuleReference)                      \
  DECLARE(ModuleImportView)                             \
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
  DECLARE(EnumDeclView)                                 \
  DECLARE(EnumBodyView)                                 \
  DECLARE(EnumFieldView)                                \
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
  DECLARE(UnionTypeExprView)                            \
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
  DECLARE(ArgumentsView)                                \
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
  DECLARE(AmbientVariableView)                          \
  DECLARE(AmbientFunctionDeclarationView)               \
  DECLARE(AmbientClassDeclarationView)                  \
  DECLARE(AmbientClassFieldsView)                       \
  DECLARE(AmbientConstructorView)                       \
  DECLARE(AmbientMemberFunctionView)                    \
  DECLARE(AmbientMemberVariableView)                    \
  DECLARE(AmbientEnumDeclarationView)                   \
  DECLARE(AmbientEnumBodyView)                          \
  DECLARE(AmbientEnumFieldView)                         \
  DECLARE(AmbientModuleView)                            \
  DECLARE(AmbientModuleBody)                            \
  DECLARE(EmptyStatement)                               \
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
  YATSC_INLINE Node* name() YATSC_NOEXCEPT {return node_list_[pos];}


// Define setter accessor.
#define NODE_SETTER(name, pos)                            \
  YATSC_INLINE void set_##name(Node* name) {       \
    node_list_[pos] = name;                               \
    if (name) name->set_parent_node(this);  \
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
  
  SourceInformation(SourcePosition source_position)
      : source_position_(source_position) {}



  // Copy constructor
  SourceInformation(const SourceInformation& source_information)
      : source_position_(source_information.source_position_){}


  // Copy assignment operator.
  SourceInformation& operator = (const SourceInformation& source_information) {
    source_position_ = source_information.source_position_;
    return *this;
  }


  // Getter and setter for line_number_
  YATSC_CONST_GETTER(const SourcePosition&, source_position, source_position_)
  
 private:
  SourcePosition source_position_;
};


// The IR Tree representation class.
// This class has all tree properties and accessible from Node type.
// All other **View classes are only view of this Node class.
class Node : private Uncopyable, private Unmovable {
 public:

  typedef Vector<Node*> List;
  typedef List::iterator ListIterator;


  // Create Node.
  YATSC_INLINE Node(NodeType node_type, size_t capacity = 0)
      : node_type_(node_type),
        capacity_(capacity),
        operand_(TokenKind::kIllegal),
        double_value_(0l),
        invalid_lhs_(false),
        string_value_(nullptr) {
    if (capacity != 0) {
      node_list_.resize(capacity);
    }
  }


  // Create Node.
  YATSC_INLINE Node(NodeType node_type, size_t capacity, std::initializer_list<Node*> node_list)
      : node_list_(node_list),
        node_type_(node_type),
        capacity_(capacity),
        operand_(TokenKind::kIllegal),
        double_value_(0l),
        invalid_lhs_(false),
        string_value_(nullptr) {
    if (capacity != 0) {
      node_list_.resize(capacity);
    }
  }

  
  virtual ~Node() {}


  static Node* Null() {return nullptr;}


  // Getter for node_type.
  YATSC_CONST_GETTER(NodeType, node_type, node_type_)


  YATSC_CONST_GETTER(const SourcePosition&, source_position, source_information_.source_position())


  // Getter and setter for parent_node_.
  YATSC_SETTER(Node*, parent_node, parent_node_)


  YATSC_GETTER(Node*, parent_node, parent_node_)


  // Getter for children list.
  YATSC_GETTER(List&, node_list, node_list_)

  
  // Getter for size.
  YATSC_CONST_GETTER(size_t, size, node_list_.size())
  

  YATSC_CONST_GETTER(Node*, first_child, node_list_.front())
  

  YATSC_CONST_GETTER(Node*, last_child, node_list_.back())


  YATSC_INLINE List::iterator begin() {return node_list_.begin();}


  YATSC_INLINE List::iterator end() {return node_list_.end();}


  YATSC_INLINE List::const_iterator cbegin() const {return node_list_.cbegin();}


  YATSC_INLINE const List::const_iterator cend() const {return node_list_.cend();}


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
  void InsertLast(Node* node);


  // Insert a node to the front of the children.
  void InsertFront(Node* node);


  // Insert a new node to before specified node.
  void InsertBefore(Node* newNode, Node* oldNode);


  // Insert a new node after the specified node.
  void InsertAfter(Node* newNode, Node* oldNode);


  void InsertAt(size_t index, Node* node);


  Node* at(size_t index) YATSC_NOEXCEPT {
    return node_list_.size() > index? node_list_[index]: nullptr;
  }


  // Set string value.
  YATSC_INLINE void set_string_value(const Literal* value) YATSC_NOEXCEPT {
    string_value_ = value;
  }


  // Return string value.
  YATSC_INLINE bool has_string_value() YATSC_NO_SE {
    return string_value_ != nullptr;
  }


  // Return string value.
  YATSC_INLINE const Literal* string_value() YATSC_NO_SE {
    return string_value_;
  }


  YATSC_INLINE void set_symbol(Handle<Symbol> symbol) {
    symbol_ = symbol;
  }
  

  YATSC_INLINE Handle<Symbol> symbol() YATSC_NO_SE {
    return symbol_;
  }


  YATSC_INLINE const Utf16String& utf16_symbol_value() YATSC_NO_SE {
    return symbol_->utf16_string();
  }


  YATSC_INLINE const Utf8String& utf8_symbol_value() YATSC_NO_SE {
    return symbol_->utf8_string();
  }


  YATSC_INLINE bool HasSymbol() YATSC_NO_SE {
    return static_cast<bool>(symbol_);
  }


  YATSC_INLINE bool StringEquals(const Node* node) YATSC_NO_SE {
    if (!node) {
      return false;
    }
    return node->string_value()->Equals(string_value_);
  }


  YATSC_INLINE bool SymbolEquals(const Node* node) YATSC_NO_SE {
    if (!node) {
      return false;
    }
    return symbol_->Equals(node->symbol());
  }


  // Return string value.
  YATSC_INLINE const char* utf8_string_value() YATSC_NO_SE {
    return string_value_->utf8_value();
  }


  // Return string value.
  YATSC_INLINE const UC16* utf16_string_value() YATSC_NO_SE {
    return string_value_->utf16_value();
  }


  // Set double value.
  YATSC_INLINE void set_double_value(double d) YATSC_NOEXCEPT {
    double_value_ = d;
  }


  // Return double value.
  YATSC_INLINE double double_value() YATSC_NO_SE {
    return double_value_;
  }


  YATSC_INLINE bool double_equals(Node* node) YATSC_NO_SE {
    return double_equals(node->double_value());
  }


  YATSC_INLINE bool double_equals(double double_value) YATSC_NO_SE {
    return fabs(double_value_ - double_value) >= DBL_EPSILON;
  }


  // Set an operand.
  YATSC_INLINE void set_operand(TokenKind op) YATSC_NOEXCEPT {
    operand_ = op;
  }


  YATSC_INLINE bool operand_equals(const Node* node) YATSC_NO_SE {
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
  YATSC_INLINE TokenKind operand() YATSC_NO_SE {
    return operand_;
  }


  // Remove specified node from children.
  YATSC_INLINE void Remove(Node* block) {
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
  void SetInformationForNode(const Token& token) YATSC_NOEXCEPT;
  

  // Set source information to this node and children.
  void SetInformationForTree(const Token& token) YATSC_NOEXCEPT;


  // Set source information to this node.
  void SetInformationForNode(const Token* token) YATSC_NOEXCEPT {SetInformationForNode(*token);}
  

  // Set source information to this node and children.
  void SetInformationForTree(const Token* token) YATSC_NOEXCEPT {SetInformationForTree(*token);}

  
  // Set source information to this node.
  void SetInformationForNode(const Node* node) YATSC_NOEXCEPT;


  // Set source information to this node and children.
  void SetInformationForTree(const Node* node) YATSC_NOEXCEPT;

  bool Equals(const Node* node) YATSC_NO_SE;

  // Clone this node and all children.
  Node* Clone() YATSC_NOEXCEPT;

  void set_unsafe_zone_allocator(UnsafeZoneAllocator* unsafe_zone_allocator) {
    unsafe_zone_allocator_ = unsafe_zone_allocator;
  }
  
#define DEF_CAST(type)                                                  \
  YATSC_INLINE type* To##type() YATSC_NOEXCEPT {                        \
    return node_type_ == NodeType::k##type? reinterpret_cast<type*>(this): nullptr; \
  }                                                                     \
  YATSC_INLINE bool Has##type() YATSC_NO_SE {                           \
    return node_type_ == NodeType::k##type;                             \
  }


#define DECLARE_CAST(ViewName) DEF_CAST(ViewName)
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

  String DoToString() const;
  
  
  void DoToStringTree(String& indent, StringStream& ss) const;


  void ToStringSelf(const Node* target, String& indent, StringStream& ss) const;
  
  
  std::bitset<8> flags_;
  SourceInformation source_information_;
  NodeType node_type_;
  size_t capacity_;
  Node* parent_node_;
  TokenKind operand_;
  double double_value_;
  bool invalid_lhs_;
  const Literal* string_value_;
  Handle<Symbol> symbol_;
  UnsafeZoneAllocator* unsafe_zone_allocator_;
};



class ScopedNode: public Node, public Scope {
 protected:
  ScopedNode(NodeType type, size_t count, Handle<Scope> scope, std::initializer_list<Node*> nodes)
      : Node(type, count, nodes),
        scope_(scope) {}


  ScopedNode(NodeType type, size_t count, Handle<Scope> scope)
      : Node(type, count),
        scope_(scope) {}


  ScopedNode(NodeType type, Handle<Scope> scope)
      : Node(type, 0u),
        scope_(scope) {}


 public:
  Handle<Scope> scope() {return scope_;}
  

 private:
  Handle<Scope> scope_;
};


class PropertyNode: public Node, public Properties {
 protected:
  PropertyNode(NodeType type, size_t count, std::initializer_list<Node*> nodes)
      : Node(type, count, nodes),
        properties_(Heap::NewHandle<Properties>()) {}


  PropertyNode(NodeType type, size_t count)
      : Node(type, count),
        properties_(Heap::NewHandle<Properties>()) {}


  PropertyNode(NodeType type)
      : Node(type, 0u),
        properties_(Heap::NewHandle<Properties>()) {}


 public:
  Handle<Properties> properties() {return properties_;}
  

 private:
  Handle<Properties> properties_;
};


// Represent block.
class BlockView: public ScopedNode {
 public:
  BlockView(Handle<Scope> scope): ScopedNode(NodeType::kBlockView, scope){}
};


// Represent file root of script.
class FileScopeView: public ScopedNode {
 public:
  FileScopeView(Handle<Scope> scope, std::initializer_list<Node*> body): ScopedNode(NodeType::kFileScopeView, 0u, scope, body){}

  
  FileScopeView(Handle<Scope> scope): ScopedNode(NodeType::kFileScopeView, 0u, scope){}
};


// Represent statement.
class StatementView : public Node {
 public:
  StatementView():
      Node(NodeType::kStatementView, 1u){}

  StatementView(Node* expr):
      Node(NodeType::kStatementView, 1u, {expr}) {}

  // Getter and Setter for exp.
  NODE_PROPERTY(expr, 0)
};


// Represent variable declarations.
class VariableDeclView: public Node {
 public:
  VariableDeclView():
      Node(NodeType::kVariableDeclView, 0) {}

  
  VariableDeclView(std::initializer_list<Node*> vars):
      Node(NodeType::kVariableDeclView, 0, vars) {}
};


// Represent variable declarations.
class LexicalDeclView: public Node {
 public:
  LexicalDeclView(TokenKind op):
      Node(NodeType::kLexicalDeclView, 0) {
    set_operand(op);
  }

  
  LexicalDeclView(TokenKind op, std::initializer_list<Node*> vars):
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
class ModuleDeclView: public PropertyNode {
 public:
  ModuleDeclView():
      PropertyNode(NodeType::kModuleDeclView, 2u) {}


  ModuleDeclView(Node* name, Node* body):
      PropertyNode(NodeType::kModuleDeclView, 2u, {name, body}) {}

  
  NODE_PROPERTY(name, 0)

  NODE_PROPERTY(body, 1)
};


// Represent export.
class ExportView: public Node {
 public:
  ExportView(bool default_export, Node* export_clause, Node* from_clause)
      : Node(NodeType::kExportView, 2u, {export_clause, from_clause}) {
    set_flag(0, default_export);
  }

  
  ExportView(Node* export_clause, Node* from_clause)
      : Node(NodeType::kExportView, 2u, {export_clause, from_clause}) {}


  ExportView()
      : Node(NodeType::kExportView, 2u) {}

  
  NODE_PROPERTY(export_clause, 0)

  NODE_PROPERTY(from_clause, 1)

  NODE_FLAG_PROPERTY(default_export, 0)
};


class NamedExportListView: public Node {
 public:
  NamedExportListView(std::initializer_list<Node*> binding)
      : Node(NodeType::kNamedExportListView, 0u, binding) {}


  NamedExportListView()
      : Node(NodeType::kNamedExportListView, 0u) {}
};


class NamedExportView: public Node {
 public:
  NamedExportView(Node* name, Node* binding)
      : Node(NodeType::kNamedExportView, 2u, {name, binding}) {}


  NamedExportView()
      : Node(NodeType::kNamedExportView, 2u) {}


  NODE_PROPERTY(name, 0)


  NODE_PROPERTY(binding, 1)
};


// Represent import.
class ImportView: public Node {
 public:
  ImportView(Node* import_clause, Node* from_expr)
      : Node(NodeType::kImportView, 2u, {import_clause, from_expr}) {}


  ImportView()
      : Node(NodeType::kImportView, 2u) {}

  
  NODE_PROPERTY(import_clause, 0)

  
  NODE_PROPERTY(from_expr, 1)
};


class ModuleImportView: public Node {
 public:
  ModuleImportView(Node* binding, Node* module_specifier)
      : Node(NodeType::kModuleImportView, 2u, {binding, module_specifier}) {}


  ModuleImportView()
      : Node(NodeType::kModuleImportView, 2u) {}


  NODE_PROPERTY(binding, 0)


  NODE_PROPERTY(module_specifier, 1)
};


class ImportClauseView: public Node {
 public:
  ImportClauseView(Node* first, Node* second)
      : Node(NodeType::kImportClauseView, 2u, {first, second}) {}


  ImportClauseView()
      : Node(NodeType::kImportClauseView, 2u) {}


  NODE_PROPERTY(first, 0)


  NODE_PROPERTY(second, 1)
};


class NamedImportListView: public Node {
 public:
  NamedImportListView(Node* binding)
      : Node(NodeType::kNamedImportListView, 0u, {binding}) {}


  NamedImportListView()
      : Node(NodeType::kNamedImportListView, 0u) {}
};


class NamedImportView: public Node {
 public:
  NamedImportView(Node* name, Node* binding)
      : Node(NodeType::kNamedImportView, 2u, {name, binding}) {}


  NamedImportView()
      : Node(NodeType::kNamedImportView, 2u) {}


  NODE_PROPERTY(name, 0)


  NODE_PROPERTY(binding, 1)
};


class ExternalModuleReference: public Node {
 public:
  ExternalModuleReference(Handle<Symbol> symbol)
      : Node(NodeType::kExternalModuleReference, 0u) {
    set_symbol(symbol);
  }


  ExternalModuleReference()
      : Node(NodeType::kExternalModuleReference, 0u) {}
};


// Represent variable.
class VariableView : public Node {
 public:
  VariableView(Node* binding_identifier, Node* value, Node* type):
      Node(NodeType::kVariableView, 3u, {binding_identifier, value, type}) {}


  VariableView():
      Node(NodeType::kVariableView, 3u) {}


  NODE_PROPERTY(binding_identifier, 0)
  NODE_PROPERTY(value, 1)
  NODE_PROPERTY(type, 2)
};


// Represent if statement.
class IfStatementView : public Node {
 public:
  IfStatementView(Node* expr, Node* if_block_Node, Node* else_block_node)
      : Node(NodeType::kIfStatementView, 3u, {expr, if_block_Node, else_block_node}) {}


  IfStatementView()
      : Node(NodeType::kIfStatementView, 3u) {}

  // Getter and setter for expr.
  NODE_PROPERTY(expr, 0)
  
  // Getter and Setter for then_block.
  NODE_PROPERTY(then_block, 1)
  
  // Getter and Setter for else_block.
  NODE_PROPERTY(else_block, 2)
};


// Represent continue statement.
class ContinueStatementView: public Node {
 public:
  ContinueStatementView()
      : Node(NodeType::kContinueStatementView, 1u){}


  ContinueStatementView(Node* labelled_identifier)
      : Node(NodeType::kContinueStatementView, 1u, {labelled_identifier}) {}


  NODE_PROPERTY(label, 0)
};


// Represent return statement.
class ReturnStatementView: public Node {
 public:
  ReturnStatementView(Node* expr)
      : Node(NodeType::kReturnStatementView, 1u, {expr}) {}


  ReturnStatementView()
      : Node(NodeType::kReturnStatementView, 1u) {}
  

  // Getter and Setter for expr_.
  NODE_PROPERTY(expr, 0)
};


// Represent break statement.
class BreakStatementView: public Node {
 public:
  BreakStatementView(Node* label)
      : Node(NodeType::kBreakStatementView, 1u, {label}) {}

  BreakStatementView()
      : Node(NodeType::kBreakStatementView, 1u) {}


  // Getter and Setter for label.
  NODE_PROPERTY(label, 0)
};


// Represent with statement.
class WithStatementView: public Node {
 public:
  WithStatementView(Node* expr, Node* statement)
      : Node(NodeType::kWithStatementView, 2u, {expr, statement}) {}


  WithStatementView()
      : Node(NodeType::kWithStatementView, 2u) {}

  // Getter and Setter for expr.
  NODE_PROPERTY(expr, 0)

  
  // Getter and Setter for statement.
  NODE_PROPERTY(statement, 1)
};


// Represent labell.
class LabelledStatementView: public Node {
 public:
  LabelledStatementView(Node* name, Node* statement)
      : Node(NodeType::kLabelledStatementView, 2u, {name, statement}) {}


  LabelledStatementView()
      : Node(NodeType::kLabelledStatementView, 2u) {}


  // Getter and Setter for name_.
  NODE_PROPERTY(name, 0)


  NODE_PROPERTY(statement, 1)
};


// Represent switch statement.
class SwitchStatementView: public Node {
 public:
  SwitchStatementView(Node* expr, Node* case_list)
      : Node(NodeType::kSwitchStatementView, 2u, {expr, case_list}) {}


  SwitchStatementView()
      : Node(NodeType::kSwitchStatementView, 2u) {}


  // Getter and Setter for expr.
  NODE_PROPERTY(expr, 0)
  

  // Getter and Setter for case_list.
  NODE_PROPERTY(case_list, 1)
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
  NODE_PROPERTY(condition, 0)


  // Getter and Setter for body.
  NODE_PROPERTY(body, 1)
};


// Represent case.
class CaseBody: public Node {
 public:
  CaseBody(std::initializer_list<Node*> case_body)
      : Node(NodeType::kCaseBody, 0u, case_body) {}


  CaseBody()
      : Node(NodeType::kCaseBody, 0u) {}
};


// Represent try catch finally statement.
class TryStatementView: public Node {
 public:
  TryStatementView(Node* statement, Node* catch_statement, Node* finally_statement)
      : Node(NodeType::kTryStatementView, 3u, {statement, catch_statement, finally_statement}) {}

  
  TryStatementView()
      : Node(NodeType::kTryStatementView, 3u) {}


  // Getter and Setter for statement.
  NODE_PROPERTY(statement, 0)

  
  // Getter and Setter for catch_statement.
  NODE_PROPERTY(catch_statement, 1)


  // Getter and Setter for finally_statement.
  NODE_PROPERTY(finally_statement, 2)
};


class CatchStatementView: public Node {
 public:
  CatchStatementView(Node* error_name, Node* body)
      : Node(NodeType::kCatchStatementView, 2u, {error_name, body}) {}


  CatchStatementView()
      : Node(NodeType::kCatchStatementView, 2u) {}


  // Getter and Setter for error_name.
  NODE_PROPERTY(error_name, 0)


  // Getter and Setter for body.
  NODE_PROPERTY(body, 1)
};


// Represent finally statement.
class FinallyStatementView: public Node {
 public:
  FinallyStatementView(Node* body)
      : Node(NodeType::kFinallyStatementView, 1u, {body}) {}


  FinallyStatementView()
      : Node(NodeType::kFinallyStatementView, 1u) {}


  NODE_PROPERTY(body, 0)
};


// Represent throw statement.
class ThrowStatementView: public Node {
 public:
  ThrowStatementView(Node* expr)
      : Node(NodeType::kThrowStatementView, 1u, {expr}) {}

  
  // Getter and Setter for expr.
  NODE_PROPERTY(expr, 0)
};


class ForStatementView: public Node {
 public:
  ForStatementView(Node* cond_init, Node* cond_cmp, Node* cond_upd, Node* body)
      : Node(NodeType::kForStatementView, 4u, {cond_init, cond_cmp, cond_upd, body}) {}

  ForStatementView()
      : Node(NodeType::kForStatementView, 4u) {}


  // Getter and Setter for cond_init.
  NODE_PROPERTY(cond_init, 0)


  // Getter and Setter for cond_upd.
  NODE_PROPERTY(cond_upd, 1)


  // Getter and Setter for cond_cmp.
  NODE_PROPERTY(cond_cmp, 2)


  // Getter and Setter for body .
  NODE_PROPERTY(body, 3)
};


// Represent for in statement.
class ForInStatementView: public Node {
 public:
  ForInStatementView(Node* property_name, Node* expr, Node* body)
      : Node(NodeType::kForInStatementView, 3u, {property_name, expr, body}) {}

  ForInStatementView()
      : Node(NodeType::kForInStatementView, 3u) {}


  // Getter and Setter for cond_init.
  NODE_PROPERTY(property_name, 0)


  // Getter and Setter for expr.
  NODE_PROPERTY(expr, 1)

  
  // Getter and Setter for body.
  NODE_PROPERTY(body, 2)
};


// Represent for in statement.
class ForOfStatementView: public Node {
 public:
  ForOfStatementView(Node* property_name, Node* expr, Node* body)
      : Node(NodeType::kForOfStatementView, 3u, {property_name, expr, body}) {}

  ForOfStatementView()
      : Node(NodeType::kForOfStatementView, 3u) {}


  // Getter and Setter for cond_init.
  NODE_PROPERTY(property_name, 0)


  // Getter and Setter for expr.
  NODE_PROPERTY(expr, 1)

  
  // Getter and Setter for body.
  NODE_PROPERTY(body, 2)
};


// Represent while statement
class WhileStatementView: public Node {
 public:
  WhileStatementView(Node* expr, Node* body)
      : Node(NodeType::kWhileStatementView, 2u, {expr, body}) {}

  WhileStatementView()
      : Node(NodeType::kWhileStatementView, 2u) {}


  // Getter and Setter for expr.
  NODE_PROPERTY(expr, 0)


  // Getter and Setter for body.
  NODE_PROPERTY(body, 1)
};


// Represent do while statement.
class DoWhileStatementView: public Node {
 public:
  DoWhileStatementView(Node* expr, Node* body)
      : Node(NodeType::kDoWhileStatementView, 2u, {expr, body}) {}


  DoWhileStatementView()
      : Node(NodeType::kDoWhileStatementView, 2u) {}


  // Getter and Setter for expr.
  NODE_PROPERTY(expr, 0)


  // Getter and Setter for body .
  NODE_PROPERTY(body, 1)
};


class EnumDeclView: public Node {
 public:
  EnumDeclView(Node* name, Node* body)
      : Node(NodeType::kEnumDeclView, 2u, {name, body}) {}


  EnumDeclView()
      : Node(NodeType::kEnumDeclView, 2u) {}


  NODE_PROPERTY(name, 0)


  NODE_PROPERTY(body, 1)
};


class EnumBodyView: public Node {
 public:
  EnumBodyView(std::initializer_list<Node*> fields)
      : Node(NodeType::kEnumBodyView, 0u, fields) {}

  EnumBodyView()
      : Node(NodeType::kEnumBodyView, 0u) {}
};


class EnumFieldView: public Node {
 public:
  EnumFieldView(Node* name, Node* value)
      : Node(NodeType::kEnumFieldView, 2u, {name, value}) {}


  EnumFieldView()
      : Node(NodeType::kEnumFieldView, 2u) {}


  NODE_PROPERTY(name, 0)


  NODE_PROPERTY(value, 1)
};


class ClassBasesView: public Node {
 public:
  ClassBasesView(Node* base, Node* impls)
      : Node(NodeType::kClassBasesView, 2u, {base, impls}) {}

  ClassBasesView()
      : Node(NodeType::kClassBasesView, 2u) {}

  
  NODE_PROPERTY(base, 0)

  NODE_PROPERTY(impls, 1)
};


class ClassImplsView: public Node {
 public:
  ClassImplsView()
      : Node(NodeType::kClassImplsView, 0u) {}
};


class ClassDeclView: public PropertyNode {
 public:
  ClassDeclView(Node* name, Node* type_parameters, Node* bases, Node* field_list)
      : PropertyNode(NodeType::kClassDeclView, 4u, {name, type_parameters, bases, field_list}) {}
  
  ClassDeclView()
      : PropertyNode(NodeType::kClassDeclView, 4u) {}
  

  // Getter and Setter for name.
  NODE_PROPERTY(name, 0)


  // Getter and Setter for type_parameters.
  NODE_PROPERTY(type_parameters, 1)

  
    // Getter and Setter for inheritance.
  NODE_PROPERTY(bases, 2)

  
  // Getter and Setter for field_list.
  NODE_PROPERTY(field_list, 3)
};


class ClassHeritageView: public Node {
 public:
  ClassHeritageView(Node* ref)
      : Node(NodeType::kClassHeritageView, 1u, {ref}) {}


  ClassHeritageView()
      : Node(NodeType::kClassHeritageView, 1u) {}


  NODE_PROPERTY(type_reference, 0)
};


class ClassFieldListView: public Node {
 public:
  ClassFieldListView(std::initializer_list<Node*> fields)
      : Node(NodeType::kClassFieldListView, 0, fields) {}


  ClassFieldListView()
      : Node(NodeType::kClassFieldListView, 0) {}
};


class ClassFieldModifiersView: public Node {
 public:
  ClassFieldModifiersView(std::initializer_list<Node*> modifiers)
      : Node(NodeType::kClassFieldModifiersView, 0, modifiers) {}

  ClassFieldModifiersView()
      : Node(NodeType::kClassFieldModifiersView, 0) {}
};


class ClassFieldAccessLevelView: public Node {
 public:
  ClassFieldAccessLevelView(TokenKind op)
      : Node(NodeType::kClassFieldAccessLevelView, 0u) {
    set_operand(op);
  }


  // Getter and Setter for value.
  NODE_PROPERTY(value, 0)
};


class MemberVariableView: public Node {
 public:
  MemberVariableView(Node* accessor, Node* name, Node* type, Node* value)
      : Node(NodeType::kMemberVariableView, 4u, {accessor, name, type, value}) {}

  MemberVariableView()
      : Node(NodeType::kMemberVariableView, 4u) {}

  NODE_PROPERTY(accessor, 0)

  NODE_PROPERTY(name, 1)

  NODE_PROPERTY(type, 2)

  NODE_PROPERTY(value, 3)
};


class MemberFunctionDefinitionView: public Node {
 protected:
  MemberFunctionDefinitionView(NodeType node_type, bool getter, bool setter, bool generator, size_t size, std::initializer_list<Node*> list)
      : Node(node_type, size, list) {
    set_flag(0, getter);
    set_flag(1, setter);
    set_flag(2, generator);
  }

  MemberFunctionDefinitionView(NodeType node_type, size_t size, std::initializer_list<Node*> list)
      : Node(node_type, size, list) {}

  
  MemberFunctionDefinitionView(NodeType node_type, size_t size)
      : Node(node_type, size) {}

 public:


  NODE_PROPERTY(modifiers, 0)
  
  NODE_PROPERTY(name, 1)

  NODE_PROPERTY(call_signature, 2)

  NODE_FLAG_PROPERTY(getter, 0)

  NODE_FLAG_PROPERTY(setter, 1)

  NODE_FLAG_PROPERTY(generator, 2)
};


class MemberFunctionOverloadsView: public Node {
 public:
  MemberFunctionOverloadsView(std::initializer_list<Node*> overloads)
      : Node(NodeType::kMemberFunctionOverloadsView, 0u, overloads) {}

  MemberFunctionOverloadsView()
      : Node(NodeType::kMemberFunctionOverloadsView, 0u) {}
};


class MemberFunctionOverloadView: public MemberFunctionDefinitionView {
 public:
  MemberFunctionOverloadView(bool getter, bool setter, bool generator, Node* accessor, Node* name, Node* call_signature)
      : MemberFunctionDefinitionView(NodeType::kMemberFunctionOverloadView, getter, setter, generator, 3u, {accessor, name, call_signature}) {}

  MemberFunctionOverloadView(Node* accessor, Node* name, Node* call_signature)
      : MemberFunctionDefinitionView(NodeType::kMemberFunctionOverloadView, 3u, {accessor, name, call_signature}) {}

  MemberFunctionOverloadView()
      : MemberFunctionDefinitionView(NodeType::kMemberFunctionOverloadView, 3u) {}
};


class MemberFunctionView: public MemberFunctionDefinitionView {
 public:
  MemberFunctionView(bool getter, bool setter, bool generator, Node* accessor, Node* name, Node* call_signature, Node* overloads, Node* body)
      : MemberFunctionDefinitionView(NodeType::kMemberFunctionView, getter, setter, generator, 5u, {accessor, name, call_signature, overloads, body}) {}

  MemberFunctionView(Node* accessor, Node* name, Node* call_signature, Node* overloads, Node* body)
      : MemberFunctionDefinitionView(NodeType::kMemberFunctionView, 5u, {accessor, name, call_signature, overloads, body}) {}

  
  MemberFunctionView()
      : MemberFunctionDefinitionView(NodeType::kMemberFunctionView, 5u) {}

  
  NODE_PROPERTY(overloads, 3)
  
  // Getter for body_.
  NODE_PROPERTY(body, 4)
};


// Represent interface.
class InterfaceView: public Node {
 public:
  InterfaceView(Node* name, Node* type_parameters, Node* extends, Node* interface_field_list)
      : Node(NodeType::kInterfaceView, 4u, {name, type_parameters, extends, interface_field_list}) {}

  InterfaceView()
      : Node(NodeType::kInterfaceView, 4u) {}

  
  // Getter and Setter for name.
  NODE_PROPERTY(name, 0)


  // Getter and Setter for type_parameters.
  NODE_PROPERTY(type_parameters, 1)


  // Getter and Setter for impls.
  NODE_PROPERTY(extends, 2)


  // Getter and Setter for name.
  NODE_PROPERTY(field_list, 3)
};


class InterfaceExtendsView: public Node {
 public:
  InterfaceExtendsView(std::initializer_list<Node*> extends)
      : Node(NodeType::kInterfaceExtendsView, 0u, extends) {}


  InterfaceExtendsView()
      : Node(NodeType::kInterfaceExtendsView, 0u) {}
};


// Represent type expression like `var x: string`
class SimpleTypeExprView: public Node {
 public:
  
  SimpleTypeExprView(Node* type_name)
      : Node(NodeType::kSimpleTypeExprView, 1u, {type_name}) {}
  

  // Getter and Setter for type_name_.
  NODE_PROPERTY(type_name, 0)
};


class IndexSignatureView: public Node {
 public:
  IndexSignatureView(Node* identifier, Node* type, bool string_type)
      : Node(NodeType::kIndexSignatureView, 2u, {identifier, type}) {
    set_flag(0, string_type);
  }


  IndexSignatureView()
      : Node(NodeType::kIndexSignatureView, 2u) {}


  NODE_PROPERTY(identifier, 0)
  

  NODE_FLAG_PROPERTY(string_type, 0)
};


// Represent type expression like `var x: string`
class GenericTypeExprView: public Node {
 public:
  
  GenericTypeExprView(Node* type_name, Node* type_arguments)
      : Node(NodeType::kGenericTypeExprView, 2u, {type_name, type_arguments}) {}
  

  // Getter and Setter for type_name_.
  NODE_PROPERTY(type_name, 0)


  // Getter and Setter for type_name_.
  NODE_PROPERTY(type_arguments, 1)
};


// Represent type expression like `var x: string`
class TypeConstraintsView: public Node {
 public:
  
  TypeConstraintsView(Node* derived, Node* base)
      : Node(NodeType::kTypeConstraintsView, 2u, {derived, base}) {}
  

  // Getter and Setter for type_name_.
  NODE_PROPERTY(derived, 0)

  // Getter and Setter for type_name_.
  NODE_PROPERTY(base, 1)
};


class TypeArgumentsView: public Node {
 public:
  
  TypeArgumentsView()
      : Node(NodeType::kTypeArgumentsView, 0u) {}
};


class TypeQueryView: public Node {
 public:
  
  TypeQueryView(Node* var_name)
      : Node(NodeType::kTypeQueryView, 1u, {var_name}) {}


  TypeQueryView()
      : Node(NodeType::kTypeQueryView, 1u) {}
};


// Represent type expression like `var x: string`
class ArrayTypeExprView: public Node {
 public:
  
  ArrayTypeExprView(Node* type_name)
      : Node(NodeType::kArrayTypeExprView, 1u, {type_name}) {}
  

  // Getter and Setter for type_name_.
  NODE_PROPERTY(type_name, 0)
};


// Represent type expression like `var x: string`
class ObjectTypeExprView: public Node {
 public:
  
  ObjectTypeExprView(std::initializer_list<Node*> type_member_list)
      : Node(NodeType::kObjectTypeExprView, 0u, type_member_list) {}
  

  ObjectTypeExprView()
      : Node(NodeType::kObjectTypeExprView, 0u) {}
};


class UnionTypeExprView: public Node {
 public:
  UnionTypeExprView(std::initializer_list<Node*> union_member_list)
      : Node(NodeType::kUnionTypeExprView, 0u, union_member_list) {}

  UnionTypeExprView()
      : Node(NodeType::kUnionTypeExprView, 0u) {}
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
  

  NODE_PROPERTY(property_name, 0)


  NODE_PROPERTY(type_expr, 1)


  NODE_FLAG_PROPERTY(optional, 0)
};


class MethodSignatureView: public Node {
 public:
  MethodSignatureView(bool opt, bool getter, bool setter, bool generator, Node* property_name, Node* type_expr)
      : Node(NodeType::kMethodSignatureView, 2u, {property_name, type_expr}) {
    set_flag(0, opt);
    set_flag(1, getter);
    set_flag(2, setter);
    set_flag(3, generator);
  }


  MethodSignatureView()
      : Node(NodeType::kMethodSignatureView, 2u) {}
  

  NODE_PROPERTY(property_name, 0)


  NODE_PROPERTY(type_expr, 1)


  NODE_FLAG_PROPERTY(optional, 0)
};


// Represent function type expression like, `var x:(a:string, b:string) => string;`
class FunctionTypeExprView: public Node {
 public:
  FunctionTypeExprView(Node* param_list, Node* return_type, Node* type_parameters)
      : Node(NodeType::kFunctionTypeExprView, 2u, {param_list, return_type}) {}


  FunctionTypeExprView()
      : Node(NodeType::kFunctionTypeExprView, 2u) {}


  // Getter and setter for param_list_.
  NODE_PROPERTY(param_list, 0)
  

  // Getter and setter for return_type_.
  NODE_PROPERTY(return_type, 1)


  NODE_PROPERTY(type_parameters, 2)
};


// Represent function type expression like, `var x:(a:string, b:string) => string;`
class ConstructSignatureView: public Node {
 public:
  ConstructSignatureView(Node* call_signature)
      : Node(NodeType::kConstructSignatureView, 1u, {call_signature}) {}


  ConstructSignatureView()
      : Node(NodeType::kConstructSignatureView, 1u) {}


  // Getter and setter for type_parameters.
  NODE_PROPERTY(call_signature, 0)
};


// Represent accessor type expression like, `interface x {[index:int]}`
class AccessorTypeExprView: public Node {
 public:
  AccessorTypeExprView(Node* name, Node* type_expression):
      Node(NodeType::kAccessorTypeExprView, 2u, {name, type_expression}) {}


  AccessorTypeExprView():
      Node(NodeType::kAccessorTypeExprView, 2u) {}

  
  // Getter and setter for name_.
  NODE_PROPERTY(name, 0)


  // Getter and setter for type_expr.
  NODE_PROPERTY(type_expr, 1)
};


class TypeParametersView: public Node {
 public:
  TypeParametersView(std::initializer_list<Node*> type_arguments)
      : Node(NodeType::kTypeParametersView, 0u, type_arguments) {}


  TypeParametersView()
      : Node(NodeType::kTypeParametersView, 0u) {}
};


class CommaExprView: public Node {
 public:
  CommaExprView(std::initializer_list<Node*> exprs)
      : Node(NodeType::kCommaExprView, 0u, exprs) {}

  CommaExprView()
      : Node(NodeType::kCommaExprView, 0u) {}
};


class FunctionOverloadView: public Node {
 public:
  FunctionOverloadView(bool generator, Node* name, Node* call_signature)
      : Node(NodeType::kFunctionOverloadView, 2u, {name, call_signature}) {
    if (generator) {
      set_flag(0);
    }
  }

  FunctionOverloadView()
      : Node(NodeType::kFunctionOverloadView, 2u) {}

  NODE_PROPERTY(name, 0)

  NODE_PROPERTY(call_signature, 1)

  NODE_FLAG_PROPERTY(generator, 0)
};


class FunctionOverloadsView: public Node {
 public:
  FunctionOverloadsView(std::initializer_list<Node*> overloads)
      : Node(NodeType::kFunctionOverloadsView, 0u, overloads) {}

  FunctionOverloadsView()
      : Node(NodeType::kFunctionOverloadsView, 0u) {}
};


// Represent function.
class FunctionView: public Node {
 public:
  FunctionView(bool getter, bool setter, bool generator, Node* overloads, Node* name, Node* call_signature, Node* body)
      : Node(NodeType::kFunctionView, 4u, {overloads, name, call_signature, body}) {
    if (getter) {
      set_flag(0);
    } else if (setter) {
      set_flag(1);
    } else if (generator) {
      set_flag(2);
    }
  }

  FunctionView(Node* overloads, Node* name, Node* call_signature, Node* body)
      : Node(NodeType::kFunctionView, 4u, {overloads, name, call_signature, body}) {}

  
  FunctionView()
      : Node(NodeType::kFunctionView, 4u) {}

  NODE_PROPERTY(overloads, 0)
  
  // Getter for name_.
  NODE_GETTER(name, 1)

  // Getter for param_list_.
  NODE_GETTER(call_signature, 2)

  // Getter for body_.
  NODE_GETTER(body, 3)

  NODE_FLAG_PROPERTY(getter, 0)

  NODE_FLAG_PROPERTY(setter, 1)
  
  NODE_FLAG_PROPERTY(generator, 2)
};


// Represent function.
class ArrowFunctionView: public Node {
 public:
  ArrowFunctionView(Node* call_signature, Node* body)
      : Node(NodeType::kArrowFunctionView, 2u, {call_signature, body}) {}

  // Getter for param_list_.
  NODE_GETTER(call_signature, 0)

  // Getter for body_.
  NODE_GETTER(body, 1)
};


class ParameterView: public Node {
 public:
  ParameterView(bool optional, Node* name, Node* value, Node* type_expr, Node* access_level)
      : Node(NodeType::kParameterView, 4u, {name, value, type_expr, access_level}){
    if (optional) {
      set_flag(0);
    }
  }


  ParameterView()
      : Node(NodeType::kParameterView, 4u){}


  NODE_PROPERTY(name, 0)

  NODE_PROPERTY(value, 1)

  NODE_PROPERTY(type_expr, 2)
  
  NODE_PROPERTY(access_level, 3)

  NODE_FLAG_PROPERTY(optional, 0)
};


class RestParamView: public Node {
 public:
  RestParamView(Node* parameter)
      : Node(NodeType::kRestParamView, 1u, {parameter}){}

  
  RestParamView()
      : Node(NodeType::kRestParamView, 1u){}


  NODE_PROPERTY(parameter, 0)
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
  CallView(Node* target, Node* arguments)
      : Node(NodeType::kCallView, 2u, {target, arguments}) {}

  
  NODE_PROPERTY(target, 0)


  NODE_PROPERTY(args, 1)
};


class CallSignatureView: public Node {
 public:
  CallSignatureView(Node* param_list, Node* return_type, Node* type_parameters)
      : Node(NodeType::kCallSignatureView, 3u, {param_list, return_type, type_parameters}) {}

  
  NODE_PROPERTY(param_list, 0)


  NODE_PROPERTY(return_type, 1)

  
  NODE_PROPERTY(type_parameters, 2)
};


class ArgumentsView: public Node {
 public:
  ArgumentsView(Node* type_parameters, Node* call_args)
      : Node(NodeType::kArgumentsView, 2u, {type_parameters, call_args}) {}


  ArgumentsView(Node* call_args)
      : Node(NodeType::kArgumentsView, 2u, {Node::Null(), call_args}) {}


  NODE_PROPERTY(type_parameters, 0)


  NODE_PROPERTY(arguments, 1)
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


  NODE_PROPERTY(target, 0)


  NODE_PROPERTY(args, 1)
};


class NameView: public Node {
 public:
  NameView(Handle<Symbol> name)
      : Node(NodeType::kNameView, 0u) {
    set_symbol(name);
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
  BindingElementView(Node* prop, Node* value)
      : Node(NodeType::kBindingElementView, 2u, {prop, value}) {}

  BindingElementView()
      : Node(NodeType::kBindingElementView, 2u) {}

  
  NODE_PROPERTY(prop, 0)
  NODE_PROPERTY(value, 1)
};


class DefaultView: public Node {
 public:
  DefaultView()
      : Node(NodeType::kDefaultView, 0u) {}
};


class YieldView: public Node {
 public:
  YieldView(bool continuation, Node* expr)
      : Node(NodeType::kYieldView, 1u, {expr}) {
    if (continuation) {
      set_flag(0);
    }
  }

  YieldView()
      : Node(NodeType::kYieldView, 1u) {}

  NODE_PROPERTY(expr, 0)

  NODE_FLAG_PROPERTY(continuation, 0)
};


class SuperView: public Node {
 public:
  SuperView()
      : Node(NodeType::kSuperView, 0u) {}
};


class PostfixView: public Node {
 public:
  PostfixView(Node* target, TokenKind op)
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


  GetPropView()
      : Node(NodeType::kGetPropView, 2u) {}

  // Getter and Setter for target.
  NODE_PROPERTY(target, 0)

  // Getter and Setter for prop.
  NODE_PROPERTY(prop, 1)
};


class GetElemView: public Node {
 public:
  GetElemView(Node* target, Node* prop)
      : Node(NodeType::kGetElemView, 2u, {target, prop}) {}

  
  // Getter and Setter for target.
  NODE_PROPERTY(target, 0)

  
  // Getter and Setter for prop.
  NODE_PROPERTY(prop, 1)
};


class AssignmentView: public Node {
 public:
  AssignmentView(TokenKind op, Node* target, Node* expr)
      : Node(NodeType::kAssignmentView, 2u, {target, expr}) {
    set_operand(op);
  }


  // Getter and Setter for target.
  NODE_PROPERTY(target, 0)


  // Getter and Setter for expr.
  NODE_PROPERTY(expr, 1)
};


class TemaryExprView: public Node {
 public :
  TemaryExprView(Node* cond, Node* then_expr, Node* else_expr)
      : Node(NodeType::kTemaryExprView, 3u, {cond, then_expr, else_expr}) {}


  // Getter and Setter for cond.
  NODE_PROPERTY(cond, 0)
  
  
  // Getter and Setter for then_exp.
  NODE_PROPERTY(then_expr, 1)

  
  // Getter and Setter for else_exp.
  NODE_PROPERTY(else_expr, 2)
};


class BinaryExprView: public Node {
 public:
  BinaryExprView(TokenKind op, Node* first, Node* second)
      : Node(NodeType::kBinaryExprView, 2u, {first, second}) {
    set_operand(op);
  }

  // Getter and Setter for first.
  NODE_PROPERTY(first, 0)

  // Getter and Setter for second.
  NODE_PROPERTY(second, 1)
};


// Represent cast.
class CastView: public Node {
 public:
  CastView(Node* type_expr, Node* expr)
      : Node(NodeType::kCastView, 2u, {type_expr, expr}) {}


  // Getter and setter for type_expr.
  NODE_PROPERTY(type_expr, 0)

  // Getter and setter for expr.
  NODE_PROPERTY(expr, 1)
};


class UnaryExprView: public Node {
 public:
  UnaryExprView(TokenKind op, Node* expr)
      : Node(NodeType::kUnaryExprView, 1u, {expr}) {
    set_operand(op);
  }

  // Getter and Setter for exp_.
  NODE_PROPERTY(expr, 0)
};


class ThisView: public Node {
 public:
  ThisView()
      : Node(NodeType::kThisView, 0) {}
};


class NumberView: public Node {
 public:
  NumberView(const Literal* value)
      : Node(NodeType::kNumberView, 0) {
    const char* val = value->utf8_value();
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
  StringView(const Literal* str)
      : Node(NodeType::kStringView, 0) {
    set_string_value(str);
  }
};


class RegularExprView: public Node {
 public:
  RegularExprView(const Literal* str)
      : Node(NodeType::kRegularExprView, 0) {
    set_string_value(str);
  }
};


class ObjectElementView: public Node {
 public:
  ObjectElementView(Node* key, Node* value)
      : Node(NodeType::kObjectElementView, 2u, {key, value}) {}

  // Getter and setter for key_.
  NODE_PROPERTY(key, 0)


  // Getter and setter for value_.
  NODE_PROPERTY(value, 1)
};


class ObjectLiteralView: public PropertyNode {
 public:
  ObjectLiteralView(std::initializer_list<Node*> properties)
      : PropertyNode(NodeType::kObjectLiteralView, 0, properties) {}


  ObjectLiteralView()
      : PropertyNode(NodeType::kObjectLiteralView, 0) {}
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


class TemplateLiteralView: public Node {
 public:
  TemplateLiteralView(const Literal* expr):
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
  ComprehensionExprView(bool generator, Node* for_expr, Node* tail)
      : Node(NodeType::kComprehensionExprView, 2, {for_expr, tail}) {
    if (generator) {
      set_flag(0);
    }
  }

  ComprehensionExprView()
      : Node(NodeType::kComprehensionExprView, 2) {}

  
  NODE_PROPERTY(for_expr, 0)

  NODE_PROPERTY(tail, 1)

  NODE_FLAG_PROPERTY(generator, 0)
};


class AmbientFunctionDeclarationView: public Node {
 public:
  AmbientFunctionDeclarationView(bool generator, Node* identifier, Node* call_signature)
      : Node(NodeType::kAmbientFunctionDeclarationView, 2u, {identifier, call_signature}) {
    set_flag(0, generator);
  }


  AmbientFunctionDeclarationView()
      : Node(NodeType::kAmbientFunctionDeclarationView, 2u) {}


  NODE_PROPERTY(identifier, 0)


  NODE_PROPERTY(call_signature, 1)


  NODE_FLAG_PROPERTY(generator, 0)
};


class AmbientClassDeclarationView: public Node {
 public:
  AmbientClassDeclarationView(Node* identifier, Node* type_parameters, Node* bases, Node* body)
      : Node(NodeType::kAmbientClassDeclarationView, 4u, {identifier, type_parameters, bases, body}) {}

  AmbientClassDeclarationView()
      : Node(NodeType::kAmbientClassDeclarationView, 4u) {}


  NODE_PROPERTY(identifier, 0)

  
  NODE_PROPERTY(type_parameters, 1)


  NODE_PROPERTY(bases, 2)


  NODE_PROPERTY(body, 3)
};


class AmbientClassFieldsView: public Node {
 public:
  AmbientClassFieldsView()
      : Node(NodeType::kAmbientClassFieldsView, 0u) {}
};


class AmbientVariableView: public Node {
 public:
  AmbientVariableView(Node* identifier, Node* type)
      : Node(NodeType::kAmbientVariableView, 2u, {identifier, type}) {}

  
  AmbientVariableView()
      : Node(NodeType::kAmbientVariableView, 2u) {}


  NODE_PROPERTY(identifier, 0)

  
  NODE_PROPERTY(type, 1)
};


class AmbientConstructorView: public Node {
 public:
  AmbientConstructorView(Node* modifiers, Node* call_signature)
      : Node(NodeType::kAmbientConstructorView, 2u, {modifiers, call_signature}) {}


  AmbientConstructorView()
      : Node(NodeType::kAmbientConstructorView, 2u) {}


  NODE_PROPERTY(modifiers, 0)


  NODE_PROPERTY(call_signature, 1)
};


class AmbientMemberFunctionView: public Node {
 public:
  AmbientMemberFunctionView(bool getter, bool setter, bool generator, Node* modifiers, Node* identifier, Node* call_signature)
      : Node(NodeType::kAmbientMemberFunctionView, 3u, {modifiers, identifier, call_signature}) {
    set_flag(0, getter);
    set_flag(1, setter);
    set_flag(2, generator);
  }


  AmbientMemberFunctionView()
      : Node(NodeType::kAmbientMemberFunctionView, 3u) {}


  NODE_PROPERTY(modifiers, 0)


  NODE_PROPERTY(identifier, 1)

  
  NODE_PROPERTY(call_signature, 2)
};


class AmbientMemberVariableView: public Node {
 public:
  AmbientMemberVariableView(Node* modifiers, Node* identifier, Node* type)
      : Node(NodeType::kAmbientMemberVariableView, 3u, {modifiers, identifier, type}) {}


  AmbientMemberVariableView()
      : Node(NodeType::kAmbientMemberVariableView, 3u) {}


  NODE_PROPERTY(modifiers, 0)


  NODE_PROPERTY(identifier, 1)


  NODE_PROPERTY(type, 2)
};


class AmbientEnumDeclarationView: public Node {
 public:
  AmbientEnumDeclarationView(Node* identifier, Node* body)
      : Node(NodeType::kAmbientEnumDeclarationView, 2u, {identifier, body}) {}


  AmbientEnumDeclarationView()
      : Node(NodeType::kAmbientEnumDeclarationView, 2u) {}


  NODE_PROPERTY(identifier, 0)


  NODE_PROPERTY(body, 1)
};


class AmbientEnumBodyView: public Node {
 public:
  AmbientEnumBodyView()
      : Node(NodeType::kAmbientEnumBodyView, 0u) {}
};


class AmbientEnumFieldView: public Node {
 public:
  AmbientEnumFieldView(Node* property_name, Node* value)
      : Node(NodeType::kAmbientEnumFieldView, 2u, {property_name, value}) {}


  AmbientEnumFieldView()
      : Node(NodeType::kAmbientEnumFieldView, 2u) {}


  NODE_PROPERTY(property_name, 0)


  NODE_PROPERTY(value, 1)
};


class AmbientModuleView: public Node {
 public:
  AmbientModuleView(bool external, Node* identifier, Node* body)
      : Node(NodeType::kAmbientModuleView, 2u, {identifier, body}) {
    set_flag(0, external);
  }


  AmbientModuleView()
      : Node(NodeType::kAmbientModuleView, 2u) {}


  NODE_PROPERTY(identifier, 0)


  NODE_PROPERTY(body, 1)


  NODE_FLAG_PROPERTY(external, 0)
};


class AmbientModuleBody: public Node {
 public:
  AmbientModuleBody()
      : Node(NodeType::kAmbientModuleBody, 0u) {}
};


class Empty: public Node {
 public:
  Empty()
      : Node(NodeType::kEmptyStatement, 0){}
};

}} //yatsc::ir

#undef NODE_PROPERTY
#undef NODE_GETTER
#undef NODE_SETTER
#undef DEF_CAST
#undef VIEW_LIST

#endif

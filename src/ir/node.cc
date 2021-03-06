//
// The MIT License (MIT)
//
// Copyright (c) Taketoshi Aono(brn)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.


#include <float.h>
#include "./node.h"
#include "../memory/heap.h"

namespace yatsc { namespace ir {

// Insert new node to the end of children.
void Node::InsertLast(Node* node)  {
  node_list_.push_back(node);
  node->set_parent_node(this);
}


// Insert new node to the front of children.
void Node::InsertFront(Node* node) {
  if (node_list_.size() > 0 ) {
    InsertBefore(node, node_list_[0]);
  } else {
    node_list_.push_back(node);
  }
  node->set_parent_node(this);
}


// Insert new node after old node.
void Node::InsertAfter(Node* newNode, Node* oldNode) {
  ListIterator end = node_list_.end();
  ListIterator found = std::find(node_list_.begin(), end, oldNode);
  if (found != end && found + 1 != end) {
    node_list_.insert(found + 1, newNode);
  } else if (found != end) {
    node_list_.push_back(newNode);
  }
  newNode->set_parent_node(this);
}


// Insert new node before old node.
void Node::InsertBefore(Node* newNode, Node* oldNode) {
  ListIterator found = std::find(node_list_.begin(), node_list_.end(), oldNode);
  if (found != node_list_.end()) {
    node_list_.insert(found, newNode);
  }
  newNode->set_parent_node(this);
}


void Node::InsertAt(size_t index, Node* node) {
  node_list_[index] = node;
}


bool Node::Equals(const Node* node) YATSC_NO_SE {
  if (!node) {
    return false;
  }

  if (node == this) {
    return true;
  }
  
  if (node_type_ != node->node_type()) {
    return false;
  }
  
  switch (node->node_type()) {
    case NodeType::kNameView: {
      if (((symbol_->utf16_length() > 0 && node->symbol()->utf16_length() > 0))
          && !SymbolEquals(node)) {
        return false;
      }
      return true;
    }
    case NodeType::kStringView: {
      if (((string_value_->utf8_length() > 0 && node->string_value_->utf8_length() > 0))
          && !StringEquals(node)) {
        return false;
      }
      return true;
    }
    case NodeType::kNumberView: {
      if (node->double_equals(double_value_)) {
        return true;
      }
      return false;
    }
    default: {
      if (node->operand() != TokenKind::kIllegal &&
          operand_ != TokenKind::kIllegal &&
          operand_ != node->operand_) {
        return false;
      }

      if (node->size() != size()) {
        return false;
      }

      for (size_t i = 0; i < node_list_.size(); i++) {
        if (!node->node_list_[i] && node_list_[i]) {
          return false;
        }
        if (!node_list_[i]->Equals(node->node_list_[i])) {
          return false;
        }
      }
      return true;
    }
  }
}


// Clone node and node's children.
Node* Node::Clone() YATSC_NOEXCEPT {
  Node* cloned = this->unsafe_zone_allocator_->template New<Node>(node_type_, capacity_);
  cloned->double_value_ = double_value_;
  cloned->string_value_ = string_value_;
  cloned->operand_ = operand_;
  cloned->source_information_ = source_information_;
  for (size_t i = 0u; i < node_list_.size(); i++) {
    Node* node = node_list_[i];
    if (node) {
      Node* ret = node->Clone();
      cloned->node_list_.push_back(ret);
    } else {
      cloned->node_list_.push_back(Node::Null());
    }
  }
  return cloned;
}


// Attach source information to this node.
void Node::SetInformationForNode(const Token& token_info) YATSC_NOEXCEPT {
  source_information_.source_position_ = token_info.source_position();
}


// Attach source information to this node and children.
void Node::SetInformationForTree(const Token& token_info) YATSC_NOEXCEPT  {
  for (size_t i = 0u; i < node_list_.size(); i++) {
    Node* node = node_list_[i];
    if (node) {
      node->SetInformationForNode(token_info);
      node->SetInformationForTree(token_info);
    }
  }
}


// Attach source information to this node.
void Node::SetInformationForNode(const Node* node) YATSC_NOEXCEPT {
  source_information_.source_position_ = node->source_position();
}


// Attach source information to this node and children.
void Node::SetInformationForTree(const Node* node) YATSC_NOEXCEPT  {
  for (size_t i = 0u; i < node_list_.size(); i++) {
    Node* target = node_list_[i];
    if (target) {
      target->SetInformationForNode(node);
      target->SetInformationForTree(node);
    }
  }
}


String Node::ToString() const {
  StringStream ss;
  String i("");
  ToStringSelf(this, i, ss);
  return std::move(ss.str());
}


String Node::DoToString() const {
  return std::move(String(kNodeTypeStringList[static_cast<int>(node_type_)]));
}


String Node::ToStringTree() const {
  StringStream ss;
  String indent = "";
  ToStringSelf(this, indent, ss);
  indent += "  ";
  DoToStringTree(indent, ss);
  String ret = std::move(ss.str());
  return std::move(ret.substr(0, ret.size() - 1));
}


void Node::DoToStringTree(String& indent, StringStream& ss) const {
  for (size_t i = 0u; i < node_list_.size(); i++) {
    Node* target = node_list_[i];
    if (target) {
      ToStringSelf(target, indent, ss);
      String nindent = indent + "  ";
      target->DoToStringTree(nindent, ss);
    } else {
      ss << indent << "[Empty]" << "\n";
    }
  }
}


void Node::ToStringSelf(const Node* target, String& indent, StringStream& ss) const {
  ss << indent << '[' << target->DoToString() << ']';
  if (target->has_string_value() && !target->HasNumberView()) {
    if (target->HasStringView()) {
      ss << "[\'" << target->utf8_string_value() << "\']";
    } else {
      ss << '[' << target->utf8_string_value() << ']';
    }
  }
  if (target->HasSymbol()) {
    ss << '[' << target->utf8_symbol_value() << ']';
  }
  if (target->HasNumberView()) {
    ss << '[' << target->double_value() << ']';
  }
  if (target->operand() != TokenKind::kIllegal) {
    ss << '[' << Token::ToString(target->operand()) << ']';
  }
  ss << '\n';
}

}}

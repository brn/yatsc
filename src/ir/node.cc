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

#include "./node.h"

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


// Clone node and node's children.
Node* Node::Clone() YATSC_NOEXCEPT {
  YATSC_CHECK(true, environment_ != nullptr);
  Node* cloned = environment_->New<Node>(node_type_, capacity_);
  cloned->double_value_ = double_value_;
  cloned->string_value_ = string_value_;
  cloned->operand_ = operand_;
  cloned->environment_ = environment_;
  cloned->source_information_ = source_information_;
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


// Attach source information to this node.
void Node::SetInformationForNode(const TokenInfo& token_info) YATSC_NOEXCEPT {
  source_information_.line_number_ = token_info.line_number();
  source_information_.start_col_ = token_info.start_col();
}


// Attach source information to this node and children.
void Node::SetInformationForTree(const TokenInfo& token_info) YATSC_NOEXCEPT  {
  for (size_t i = 0u; i < node_list_.size(); i++) {
    Node* node = node_list_[i];
    if (node != nullptr) {
      node->SetInformationForNode(token_info);
      node->SetInformationForTree(token_info);
    }
  }
}

Node::String Node::ToString() {
  return Node::String("");
}


Node::String Node::ToStringTree() {
  return Node::String("");
}

}}

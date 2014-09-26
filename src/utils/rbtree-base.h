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

#ifndef UTILS_RBTREE_BASE_H
#define UTILS_RBTREE_BASE_H

#include <string>
#include <vector>
#include <type_traits>
#include "./utils.h"

namespace yatsc {

// The color of Red-Black-Tree.
enum class RbTreeNodeColor: uint8_t {
  kBlack = 0,
  kRed
};


template <typename Key, typename T>
class RbTreeNode;


// The base class of Red-Black-Tree.
// Red-Black-Tree guranteed follows
// worst-case -> insert, delete and find is O(log n)
template <typename Key, typename T>
class RbTreeBase {  
 public:

  // Check concept of Key
  static_assert(std::is_copy_assignable<Key>::value == true,
                "The second type argument of RbTreeBase must be copy assignable.");
  
  typedef RbTreeNode<Key, T>* NodeType;
  
#if defined(DEBUG) || defined(UNIT_TEST)
  // Types for number of black node counting result.
  typedef std::pair<int, T> CountResult;

  
  // Return string expression of the tree.
  inline std::string ToString() YATSC_NO_SE;


  // Return the vector that contains all leaf nodes and black nodes count.
  inline std::vector<CountResult> GetBlackNodeCountOfLeafs() YATSC_NO_SE;
#endif

  // Getter for node count.
  YATSC_CONST_GETTER(size_t, size, size_);
  
 protected:
  
  RbTreeBase()
      : root_(nullptr),
        size_(0) {}


  // Insert value to tree and assosiated by key.
  inline NodeType InsertInternal(Key key, NodeType value) YATSC_NOEXCEPT;

  
  // Insert value that assosiated with key to tree.
  YATSC_INLINE NodeType InsertInternal(NodeType value) YATSC_NOEXCEPT;


  // Delete node that assosiated with key from tree.
  YATSC_INLINE NodeType DeleteInternal(Key key) YATSC_NOEXCEPT;

  
  // Delete node from tree.
  YATSC_INLINE NodeType DeleteInternal(NodeType node) YATSC_NOEXCEPT;

  
  // Find node that assosicated with key.
  YATSC_INLINE NodeType FindInternal(Key key) YATSC_NO_SE;
  
 private:
  
  // Rebalancing tree when node is inserted.
  YATSC_INLINE void RebalanceWhenInsert(NodeType node) YATSC_NOEXCEPT;

  
  // Rebalancing tree when node is deleted.
  inline void RebalanceWhenDelete(NodeType node) YATSC_NOEXCEPT;

  
  // Replace node by target and rebalance if neccesary.
  inline void ElevateNode(NodeType node, NodeType target) YATSC_NOEXCEPT;


  // Rotate right.
  inline NodeType RotateR(NodeType value, NodeType parent, NodeType grand_parent, bool when_delete = false) YATSC_NOEXCEPT;


  // Double rotate, rotate right and rotate left.
  inline NodeType RotateRL(NodeType value, NodeType parent, NodeType grand_parent) YATSC_NOEXCEPT;

  
  // Double rotate, rotate left and rotate right.
  inline NodeType RotateLR(NodeType value, NodeType parent, NodeType grand_parent) YATSC_NOEXCEPT;

  
  // rotate left.
  inline NodeType RotateL(NodeType value, NodeType parent, NodeType grand_parent, bool when_delete = false) YATSC_NOEXCEPT;

  
  // Detach node from it's parent node.
  inline void DetachFromParent(NodeType value) YATSC_NOEXCEPT;

  
  // Find a node that has max key from the left partial tree.
  inline NodeType FindLeftMax(NodeType node) YATSC_NOEXCEPT;

  
#if defined(DEBUG) || defined(UNIT_TEST)
  // Helper function for converting tree to string(DEBUG ONLY).
  inline std::string ToStringHelper(std::string& head, const char* bar, NodeType node) YATSC_NO_SE;


  // Helper function for get all leaf nodes from tree(DEBUG ONLY).
  inline void DoGetBlackNodeCountOfLeafs(std::vector<CountResult>& v, NodeType node) YATSC_NO_SE;
#endif

  
  NodeType root_;
  size_t size_;
};


// Node class.
// This class used as node of each item.
// Key must be comparable and copieable.
template <typename Key, typename T>
class RbTreeNode {
  template <typename KeyType, typename Type>
  friend class RbTreeBase;
  typedef RbTreeNode<Key,T>* Pointer;
 public:
  RbTreeNode()
      : color_(RbTreeNodeColor::kRed),
        left_(nullptr),
        right_(nullptr),
        parent_(nullptr),
        exists_(false) {}

  
  RbTreeNode(Key key)
      : color_(RbTreeNodeColor::kRed),
        key_(key),
        left_(nullptr),
        right_(nullptr),
        parent_(nullptr),
        exists_(false) {}

#if defined(DEBUG) || defined(UNIT_TEST)
  // Return node color.
  static inline RbTreeNodeColor GetColor(RbTreeNode<Key, T>* n) {return n->color();};


  // Return node key.
  static inline Key GetKey(RbTreeNode<Key, T>* n) {return n->key();};
#endif


  // Return value of node.
  YATSC_PROPERTY(T, node_value, value_);
    
 private:
  // Compare equality of key that contained in node.
  YATSC_INLINE bool operator == (const RbTreeNode<Key, T>& node) YATSC_NO_SE {
    return node.key_ == key_;
  }


  // Compare key that contained in node by greater than.
  YATSC_INLINE bool operator > (const RbTreeNode<Key, T>& node) YATSC_NO_SE {
    return key_ > node.key_;
  }


  // Compare node that contained in node by less than.
  YATSC_INLINE bool operator < (const RbTreeNode<Key, T>& node) YATSC_NO_SE {
    return key_ < node.key_;
  }


  // Compare equality of key.
  YATSC_INLINE bool operator == (const Key& key) YATSC_NO_SE {
    return key == key_;
  }


  // Compare key by greater than.
  YATSC_INLINE bool operator > (const Key& key) YATSC_NO_SE {
    return key_ > key;
  }


  // Compare key by less than.
  YATSC_INLINE bool operator < (const Key& key) YATSC_NO_SE {
    return key_ < key;
  }


  // Return whether this node has left tree or not.
  YATSC_INLINE bool HasLeft() YATSC_NO_SE {
    return left_ != nullptr;
  }


  // Return whether this node has right tree or not.
  YATSC_INLINE bool HasRight() YATSC_NO_SE {
    return right_ != nullptr;
  }


  // Return wheter this node is black or not.
  YATSC_INLINE bool IsBlack() YATSC_NO_SE {
    return color_ == RbTreeNodeColor::kBlack;
  }


  // Return wheter this node is red or not.
  YATSC_INLINE bool IsRed() YATSC_NO_SE {
    return color_ == RbTreeNodeColor::kRed;
  }


  // Return wheter this node is left child or not.
  YATSC_INLINE bool IsLeftChild() YATSC_NO_SE {
    return parent_->left() == this;
  }


  // Return wheter this node is right child or not.
  YATSC_INLINE bool IsRightChild() YATSC_NO_SE {
    return !IsLeftChild();
  }


  // Return wheter this node has parent node or not.
  YATSC_INLINE bool HasParent() YATSC_NO_SE {
    return parent_ != nullptr;
  }


  // Return sibling of this node, if sibling not exists, return nullptr.
  YATSC_INLINE Pointer sibling() YATSC_NO_SE {
    if (parent_ == nullptr) {
      return nullptr;
    }
    if (IsLeftChild()) {
      return parent_->right();
    }
    return parent_->left();
  }


  // Change this node color to red.
  YATSC_INLINE void ToRed() YATSC_NOEXCEPT {
    color_ = RbTreeNodeColor::kRed;
  }


  // Change this node color to black.
  YATSC_INLINE void ToBlack() YATSC_NOEXCEPT {
    color_ = RbTreeNodeColor::kBlack;
  }


  // Getter and setter for the color.
  YATSC_CONST_PROPERTY(RbTreeNodeColor, color, color_);


  // Getter and setter for the key.
  YATSC_CONST_PROPERTY(Key, key, key_);


  // Getter and setter for the left tree.
  YATSC_PROPERTY(Pointer, left, left_);


  // Getter and setter for the right tree.
  YATSC_PROPERTY(Pointer, right, right_);


  // Getter and setter for the parent node.
  YATSC_PROPERTY(Pointer, parent, parent_);


  // Get or set whether this node is exists in tree or not.
  YATSC_CONST_PROPERTY(bool, exists, exists_);


  // Swap node.
  // This method change each node's parent, left, right, color properties.
  void Swap(Pointer new_value) YATSC_NOEXCEPT {
    Pointer left = left_;
    Pointer right = right_;

    // Change left tree.
    if (left != nullptr && left != new_value) {
      new_value->set_left(left);
      // Change left tree parent.
      left->set_parent(new_value);
    }

    // Change right tree.
    if (right != nullptr && right != new_value) {
      new_value->set_right(right);
      // Change right tree parent.
      right->set_parent(new_value);
    }
  
    Pointer parent = parent_;
    // Change parent.
    if (parent != nullptr) {
      new_value->set_parent(parent);
      if (parent->left() == this) {
        // Change parent left tree.
        parent->set_left(new_value);
      } else if (parent->right() == this) {
        // Change parent right tree.
        parent->set_right(new_value);
      } else {
        FATAL("Invalid parent.");
      }
    }
    set_left(nullptr);
    set_right(nullptr);
    set_parent(nullptr);

    // Change color.
    new_value->set_color(color_);
  }

  
  RbTreeNodeColor color_;
  Key key_;
  T value_;
  Pointer left_;
  Pointer right_;
  Pointer parent_;
  bool exists_;
};
}

#include "./rbtree-base-inl.h"

#endif

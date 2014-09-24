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


#ifndef UTILS_INTRUSIVE_RB_TREE_H
#define UTILS_INTRUSIVE_RB_TREE_H

#include <string>
#include <sstream>
#include <type_traits>
#include "./utils.h"

namespace yatsc {

enum class RBTreeColor: uint8_t {
  kBlack = 0,
  kRed
};

template <typename T, typename Key>
class IntrusiveRBTreeValueBase;


// T must be implemented methods follows.
// void set_color(IntrusiveRBTree::Color)
// void set_key(Key key)
// void set_parent(T)
// IntrusiveRBTree::Color color()
// Key key()
// T left()
// T right()
// T parent()
// int compare(Key value)
//
template <typename T, typename Key>
class IntrusiveRBTree {
 private:
 public:

  static_assert(std::is_base_of<IntrusiveRBTreeValueBase<T, Key>, typename std::remove_pointer<T>::type>::value == true,
                "The first type arguemnt of IntrusiveRBTree must be derived class of IntrusiveRBTreeValueBase<T, Key>.");

  static_assert(std::is_copy_assignable<Key>::value == true,
                "The second type argument of IntrusiveRBTree must be copy assignable.");
  
  IntrusiveRBTree()
      : root_(nullptr),
        size_(0) {}
  
  inline T Insert(Key key, T value);

  inline T Insert(T value);

  inline T Delete(Key key);

  inline T Delete(T node);

  inline T Find(Key key);
  
  inline T Find(T node);

  YATSC_CONST_GETTER(size_t, size, size_);

#if defined(DEBUG) || defined(UNIT_TEST)
  typedef std::pair<int, T> CountResult;
  inline std::string ToString() YATSC_NO_SE;
  inline std::vector<CountResult> GetBlackNodeCountOfLeafs() YATSC_NO_SE;
#endif
  
 private:

  inline void Rebalance(T node);

  inline void RebalanceWhenDelete(T node);

  inline void DoDelete(T node);

  inline void ElevateLeftMax(T node);
  
  inline void ElevateLeftTree(T node);

  inline void ElevateRightTree(T node);

  inline void DoElevateNode(T node, T target);

  inline T BalanceR(T value);

  inline T BalanceL(T value);

  inline T RotateR(T value, T parent, T grand_parent, bool when_delete = false);

  inline T RotateRL(T value, T parent, T grand_parent);

  inline T RotateLR(T value, T parent, T grand_parent);

  inline T RotateL(T value, T parent, T grand_parent, bool when_delete = false);

  inline void DetachFromParent(T value);

  inline T FindLeftMax(T node);

#if defined(DEBUG) || defined(UNIT_TEST)
  inline std::string ToStringHelper(std::string& head, const char* bar, T node) YATSC_NO_SE;
  inline void DoGetBlackNodeCountOfLeafs(std::vector<CountResult>& v, T node) YATSC_NO_SE;
#endif

  T root_;
  size_t size_;
};


template <typename T, typename Key>
class IntrusiveRBTreeValueBase {
  template <typename Type, typename KeyType>
  friend class IntrusiveRBTree;
 public:
  IntrusiveRBTreeValueBase()
      : color_(RBTreeColor::kRed),
        left_(nullptr),
        right_(nullptr),
        parent_(nullptr),
        exists_(false) {}

  
  IntrusiveRBTreeValueBase(Key key)
      : color_(RBTreeColor::kRed),
        key_(key),
        left_(nullptr),
        right_(nullptr),
        parent_(nullptr),
        exists_(false) {}

#if defined(DEBUG) || defined(UNIT_TEST)
  static inline RBTreeColor GetColor(IntrusiveRBTreeValueBase<T, Key>* n) {return n->color();};
  static inline Key GetKey(IntrusiveRBTreeValueBase<T, Key>* n) {return n->key();};
#endif
    
 private:
  YATSC_INLINE bool operator == (const IntrusiveRBTreeValueBase<T,Key>& node) {
    return node.key_ == key_;
  }
  YATSC_INLINE bool operator > (const IntrusiveRBTreeValueBase<T,Key>& node) {
    return key_ > node.key_;
  }
  YATSC_INLINE bool operator < (const IntrusiveRBTreeValueBase<T,Key>& node) {
    return key_ < node.key_;
  }

  YATSC_INLINE bool operator == (const Key& key) {
    return key == key_;
  }
  YATSC_INLINE bool operator > (const Key& key) {
    return key_ > key;
  }
  YATSC_INLINE bool operator < (const Key& key) {
    return key_ < key;
  }

  YATSC_INLINE bool HasLeft() YATSC_NO_SE {
    return left_ != nullptr;
  }

  YATSC_INLINE bool HasRight() YATSC_NO_SE {
    return right_ != nullptr;
  }

  YATSC_INLINE bool IsBlack() YATSC_NO_SE {
    return color_ == RBTreeColor::kBlack;
  }

  YATSC_INLINE bool IsRed() YATSC_NO_SE {
    return color_ == RBTreeColor::kRed;
  }

  YATSC_INLINE bool IsLeftChild() YATSC_NO_SE {
    return parent_->left() == this;
  }

  YATSC_INLINE bool IsRightChild() YATSC_NO_SE {
    return !IsLeftChild();
  }

  YATSC_INLINE bool HasParent() YATSC_NO_SE {
    return parent_ != nullptr;
  }

  YATSC_INLINE T Cast() {return reinterpret_cast<T>(this);}
  
  YATSC_CONST_PROPERTY(RBTreeColor, color, color_);
  YATSC_CONST_PROPERTY(Key, key, key_);
  YATSC_PROPERTY(T, left, left_);
  YATSC_PROPERTY(T, right, right_);
  YATSC_PROPERTY(T, parent, parent_);
  YATSC_CONST_PROPERTY(bool, exists, exists_);

  void Swap(T new_value) {
    T left = left_;
    T right = right_;
    if (left != nullptr && left != new_value) {
      new_value->set_left(left);
      left->set_parent(new_value);
    }
    if (right != nullptr && right != new_value) {
      new_value->set_right(right);
      right->set_parent(new_value);
    }
  
    T parent = parent_;
    if (parent != nullptr) {
      new_value->set_parent(parent);
      if (parent->left() == this) {
        parent->set_left(new_value);
      } else if (parent->right() == this) {
        parent->set_right(new_value);
      } else {
        FATAL("Invalid parent.");
      }
    }
    set_left(nullptr);
    set_right(nullptr);
    set_parent(nullptr);
    new_value->set_color(color_);
  }
  
  RBTreeColor color_;
  Key key_;
  T left_;
  T right_;
  T parent_;
  bool exists_;
};
}

#include "./intrusive-rbtree-inl.h"

#endif

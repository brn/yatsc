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

#include <type_traits>
#include "./rbtree-base.h"
#include "./utils.h"

namespace yatsc {

enum class RBTreeColor: uint8_t {
  kBlack = 0,
  kRed
};


// Embeded Red-Black-Tree implementation.
// This class accept type T that is derived class of RbTreeNode.
template <typename Key, typename T>
class IntrusiveRbTree : public RbTreeBase<Key, T> {
 private:
 public:

  typedef typename RbTreeBase<Key, T>::NodeType NodeType;

  // Check minimum concept of the T.
  static_assert(std::is_base_of<RbTreeNode<Key, T>, typename std::remove_pointer<T>::type>::value == true,
                "The second type arguemnt of IntrusiveRbTree must be derived class of RbTreeNode<Key, T>.");
  
  IntrusiveRbTree()
      : RbTreeBase<Key, T>() {}


  // Insert key and value to the tree.
  YATSC_INLINE T Insert(Key key, NodeType value) {
    return reinterpret_cast<T>(this->InsertInternal(key, value));
  }


  // Insert value that has key to the tree.
  YATSC_INLINE T Insert(NodeType value) {
    return reinterpret_cast<T>(this->InsertInternal(value));
  }


  // Delete value that associate with key from tree.
  YATSC_INLINE T Delete(Key key) {
    NodeType ret = this->DeleteInternal(key);
    if (nullptr != ret) {
      return reinterpret_cast<T>(ret);
    }
    return nullptr;
  }


  // Delete value from tree.
  YATSC_INLINE T Delete(NodeType node) {
    NodeType ret = this->DeleteInternal(node);
    if (nullptr != ret) {
      return reinterpret_cast<T>(ret);
    }
    return nullptr;
  }


  // Find value from key.
  YATSC_INLINE T Find(Key key) YATSC_NO_SE {
    NodeType ret = this->FindInternal(key);
    if (nullptr != ret) {
      return reinterpret_cast<T>(ret);
    }
    return nullptr;
  }
};
}

#endif

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

#define NODE typename RbTreeBase<Key, T>::NodeType

namespace yatsc {

// Insert a node and key to the tree.
// This method modifiy the tree.
template <typename Key, typename T>
inline NODE RbTreeBase<Key, T>::InsertInternal(Key key, NODE value) YATSC_NOEXCEPT {
  // If node is already exists in the tree,
  // skip inserting process.
  if (value == root_ ||
      value->parent() != nullptr) {
    return value;
  }

  // Store key to node.
  value->set_key(key);
  
  size_++;


  // Case1: If root is not exists, simply,
  // assign a node to the root.
  if (root_ == nullptr) {
    root_ = value;
    value->ToBlack();
    return value;
  }


  // Case2: If a node has same key of the root,
  // simply swap each other.
  if (*root_ == *value) {
    root_->Swap(value);
    root_ = value;
    return value;
  }

  NODE node = root_;
  NODE last = nullptr;

  // Case3: Find node from the tree,
  // if node that has same value of new node,
  // simply swap each other.
  while (node != nullptr) {
    last = node;
    if (*node > *value) {
      node = node->left();
    } else if (*node < *value) {
      node = node->right();
    } else if (*node == *value) {
      node->Swap(value);
      return value;
    }
  }


  // Case4: If same key not exists in tree,
  // append a new node as leaf of the tree.
  value->set_parent(last);
  if (*last > *value) {
    last->set_left(value);
  } else {
    last->set_right(value);
  }


  // Case5: If the parent node of a new node has red color,
  // this is the sign that is Red-Black-Tree conditions is broken, so rebalancing.
  if (last->IsRed()) {
    RebalanceWhenInsert(value);
  }
  
  return value;
}


// Insert a node that has key.
template <typename Key, typename T>
YATSC_INLINE NODE RbTreeBase<Key, T>::InsertInternal(NODE value) YATSC_NOEXCEPT {
  return InsertInternal(value->key(), value);
}


// Delete a node from the tree by key.
// If target node that has same key is found, return this one,
// but not, return nullptr.
template <typename Key, typename T>
YATSC_INLINE NODE RbTreeBase<Key, T>::DeleteInternal(Key key) YATSC_NOEXCEPT {
  auto node = root_;
  while (node != nullptr) {
    if (node->key() > key) {
      node = node->left();
    } else if (node->key() < key) {
      node = node->right();
    } else if (node->key() == key) {
      return DeleteInternal(node);
    }
  }
  return nullptr;
}


// Delete a node from the tree and rebalance tree,
// to keep the Red-Black-Tree conditions.
template <typename Key, typename T>
YATSC_INLINE NODE RbTreeBase<Key, T>::DeleteInternal(NODE node) YATSC_NOEXCEPT {

  size_--;
  
  bool has_left = node->HasLeft();
  bool has_right = node->HasRight();

  // All cases rebalance tree structures,
  // if structure is broken.
  if (has_left && has_right) {
    // Case1: If node has left and right tree,
    // seach maximum value of left tree and swap target node and it.
    ElevateNode(node, FindLeftMax(node));
  } else if (has_left) {
    // Case2: If node has left tree only,
    // elevate left tree and swap target node and it.
    ElevateNode(node, node->left());
  } else if (has_right) {
    // Case3: If node not has tree,
    // Remove node from tree.
    ElevateNode(node, node->right()); 
  } else {
    // If target node is the root node,
    // simply remove root node from tree,
    // because if node that not has tree is root,
    // this mean, this tree has only root node.
    if (node == root_) {
      root_ = nullptr;
      node->set_parent(nullptr);
      node->set_left(nullptr);
      node->set_right(nullptr);
      return node;
    } else {
      // If node has black color,
      // black node count is decreased,
      // so rebalance this tree before swap.
      if (node->IsBlack()) {
        RebalanceWhenDelete(node);
      }
      // Delete node.
      DetachFromParent(node);
    }
  }

  // Change root node color to black.
  if (root_ != nullptr) {
    root_->ToBlack();
  }
  return node;
}


// Elevate left or right or left maximum node when an old node is deleted from tree.
template <typename Key, typename T>
inline void RbTreeBase<Key, T>::ElevateNode(NODE node, NODE target) YATSC_NOEXCEPT {
  if (node->IsRed()) {
    // If old node color is red.
    if (target->IsBlack()) {
      // If new node color is black,
      // rebalance new node parent.
      RebalanceWhenDelete(target);
    }
    DetachFromParent(target);
    node->Swap(target);
    if (node == root_) {
      root_ = target;
    }
    target->set_color(node->color());
    return;
  } else if (target->IsRed()) {
    // If old node is red and new node is red.
    DetachFromParent(target);
    node->Swap(target);
    target->ToBlack();
    if (node == root_) {
      root_ = target;
    }
    return;
  }

  // If old node and new node is black.
  RebalanceWhenDelete(target);
  DetachFromParent(target);
  node->Swap(target);
  if (node == root_) {
    root_ = target;
  }
}


// Rebalance tree if a new red node inserted as the red node child.
template <typename Key, typename T>
YATSC_INLINE void RbTreeBase<Key, T>::RebalanceWhenInsert(NODE node) YATSC_NOEXCEPT {
  NODE current = node;
  while (1) {
    NODE parent = current->parent();
    NODE grand_parent = parent->parent();
    
    if (*parent > *current) {
      // If node is left tree.
      // check node parent is left or right.
      // if node parent is left tree,
      // rotate right, if not, rotate right and rotate left.
      if (*grand_parent > *parent) {
        current = RotateR(current, parent, grand_parent);
      } else {
        current = RotateRL(current, parent, grand_parent);
      }
    } else {
      // If node is right tree.
      // check node parent is left or right.
      // if node parent is left tree,
      // rotate left and rotate right, if not, rotate left.
      if (*grand_parent > *parent) {
        current = RotateLR(current, parent, grand_parent);
      } else {
        current = RotateL(current, parent, grand_parent);
      }
    }

    // If node parent is null,
    // this node is root node, so replace the root_ property with a current node.
    if (!current->HasParent()) {
      root_ = current;
      root_->ToBlack();
      return;
    }


    // If parent node is black, black node has any child, so end rebalance,
    // if current node color is black, black node has any parent, so end rebalance.
    if (current->parent()->color() == RbTreeNodeColor::kBlack ||
        current->color() == RbTreeNodeColor::kBlack) {
      return;
    }
  }
}


template <typename Key, typename T>
inline void RbTreeBase<Key, T>::RebalanceWhenDelete(NODE node) YATSC_NOEXCEPT {
  while (1) {
    if (!node->HasParent()) {
      node->ToBlack();
      root_ = node;
      break;
    }
    
    NODE parent = node->parent();
    NODE sibling = node->sibling();
    
    if (sibling != nullptr && sibling->IsRed()) {
      if (node->IsLeftChild()) {
        RotateL(sibling->right(), sibling, parent, true);
        parent->ToRed();
        sibling->ToBlack();
        sibling = parent->right();
      } else {
        RotateR(sibling->left(), sibling, parent, true);
        parent->ToRed();
        sibling->ToBlack();
        sibling = parent->left();
      }
    }

    NODE sibling_left = sibling != nullptr? sibling->left(): nullptr;
    NODE sibling_right = sibling != nullptr? sibling->right(): nullptr;
    
    if (parent->IsBlack() &&
        (sibling_left == nullptr || sibling_left->IsBlack()) &&
        (sibling_right == nullptr || sibling_right->IsBlack())) {
      if (sibling != nullptr) {
        sibling->ToRed();
      }
      node = parent;
      continue;
    }
    
    if ((sibling_left == nullptr || sibling_left->IsBlack()) &&
        (sibling_right == nullptr || sibling_right->IsBlack())) {
      parent->ToBlack();
      if (sibling != nullptr) {
        sibling->ToRed();
      }
      break;
    }
    
    if (node->IsLeftChild() &&
        (sibling_left != nullptr && sibling_left->IsRed()) &&
        (sibling_right == nullptr || sibling_right->IsBlack())) {
      sibling_left->ToBlack();
      sibling->ToRed();
      RotateR(sibling_left->left(), sibling_left, sibling, true);
      sibling_right = sibling;
      sibling = sibling_left;
      sibling_left = nullptr;
    } else if (node->IsRightChild() &&
               (sibling_left == nullptr || sibling_left->IsBlack()) &&
               (sibling_right != nullptr && sibling_right->IsRed())) {
      sibling_right->ToBlack();
      sibling->ToRed();
      RotateL(sibling_right->right(), sibling_right, sibling, true);
      sibling_left = sibling;
      sibling = sibling_right;
      sibling_right = nullptr;
    }

    if (node->IsLeftChild() && sibling_right != nullptr && sibling_right->IsRed()) {
      RotateL(sibling_right, sibling, parent, true);
      RbTreeNodeColor c = parent->color();
      parent->set_color(sibling->color());
      sibling->set_color(c);
      sibling_right->ToBlack();
      break;
    } else if (node->IsRightChild() && sibling_left != nullptr && sibling_left->IsRed()) {
      RotateR(sibling_left, sibling, parent, true);
      RbTreeNodeColor c = parent->color();
      parent->set_color(sibling->color());
      sibling->set_color(c);
      sibling_left->ToBlack();
      break;
    }
    break;
  }
}


template <typename Key, typename T>
YATSC_INLINE NODE RbTreeBase<Key, T>::FindInternal(Key key) YATSC_NO_SE {
  if (root_ == nullptr) {return nullptr;}

  if (*root_ == key) {return root_;}

  auto node = root_;
  
  while (node != nullptr) {
    if (*node > key) {
      node = node->left();
    } else if (*node < key) {
      node = node->right();
    } else if (*node == key) {
      return node;
    }
  }

  return nullptr;
}



template <typename Key, typename T>
inline NODE RbTreeBase<Key, T>::RotateR(NODE value, NODE parent, NODE grand_parent, bool when_delete) YATSC_NOEXCEPT {
  NODE right = parent->right();
  NODE grand_grand_parent = grand_parent->parent();

  parent->set_parent(grand_grand_parent);
  parent->set_right(grand_parent);
  
  grand_parent->set_parent(parent);
  grand_parent->set_left(right);

  if (right != nullptr) {
    right->set_parent(grand_parent);
  }

  if (!when_delete) {
    parent->set_color(RbTreeNodeColor::kRed);
    grand_parent->set_color(RbTreeNodeColor::kBlack);
    if (value != nullptr) {
      value->set_color(RbTreeNodeColor::kBlack);
    }
  }

  if (grand_grand_parent != nullptr) {
    if (*grand_grand_parent > *grand_parent) {
      grand_grand_parent->set_left(parent);
    } else {
      grand_grand_parent->set_right(parent);
    }
  }

  if (!parent->HasParent()) {
    root_ = parent;
  }
  
  return parent;
}


template <typename Key, typename T>
inline NODE RbTreeBase<Key, T>::RotateRL(NODE value, NODE parent, NODE grand_parent) YATSC_NOEXCEPT {
  NODE right = value->right();
  NODE left = value->left();
  NODE grand_grand_parent = grand_parent->parent();

  value->set_parent(grand_grand_parent);
  
  //  [B]
  //    [R]
  //      [R]

  value->set_right(parent);
  parent->set_parent(value);
  parent->set_left(right);

  if (right != nullptr) {
    right->set_parent(parent);
  }

  value->set_left(grand_parent);
  grand_parent->set_parent(value);
  grand_parent->set_right(left);

  if (left != nullptr) {
    left->set_parent(grand_parent);
  }
  
  value->set_color(RbTreeNodeColor::kRed);
  parent->set_color(RbTreeNodeColor::kBlack);
  grand_parent->set_color(RbTreeNodeColor::kBlack);


  if (grand_grand_parent != nullptr) {
    if (*grand_grand_parent > *grand_parent) {
      grand_grand_parent->set_left(value);
    } else {
      grand_grand_parent->set_right(value);
    }
  }
  return value;
}


template <typename Key, typename T>
inline NODE RbTreeBase<Key, T>::RotateLR(NODE value, NODE parent, NODE grand_parent) YATSC_NOEXCEPT {
  NODE right = value->right();
  NODE left = value->left();
  NODE grand_grand_parent = grand_parent->parent();

  value->set_parent(grand_grand_parent);
  
  //    [B]
  //  [R]
  //[R]
  value->set_left(parent);
  parent->set_parent(value);
  parent->set_right(left);

  if (left != nullptr) {
    left->set_parent(parent);
  }

  value->set_right(grand_parent);
  grand_parent->set_parent(value);
  grand_parent->set_left(right);

  if (right != nullptr) {
    right->set_parent(grand_parent);
  }

  value->set_color(RbTreeNodeColor::kRed);
  parent->set_color(RbTreeNodeColor::kBlack);
  grand_parent->set_color(RbTreeNodeColor::kBlack);

  if (grand_grand_parent != nullptr) {
    if (*grand_grand_parent > *grand_parent) {
      grand_grand_parent->set_left(value);
    } else {
      grand_grand_parent->set_right(value);
    }    
  }
  return value;
}


template <typename Key, typename T>
inline NODE RbTreeBase<Key, T>::RotateL(NODE value, NODE parent, NODE grand_parent, bool when_delete) YATSC_NOEXCEPT {
  NODE left = parent->left();
  NODE grand_grand_parent = grand_parent->parent();

  parent->set_parent(grand_grand_parent);
  parent->set_left(grand_parent);
  
  grand_parent->set_parent(parent);
  grand_parent->set_right(left);

  if (left != nullptr) {
    left->set_parent(grand_parent);
  }

  if (!when_delete) {
    parent->set_color(RbTreeNodeColor::kRed);
    if (value != nullptr) {
      value->set_color(RbTreeNodeColor::kBlack);
    }
    grand_parent->set_color(RbTreeNodeColor::kBlack);
  }
  
  if (grand_grand_parent != nullptr) {
    if (*grand_grand_parent > *grand_parent) {
      grand_grand_parent->set_left(parent);
    } else {
      grand_grand_parent->set_right(parent);
    }
  }

  if (!parent->HasParent()) {
    root_ = parent;
  }
  return parent;
}


template <typename Key, typename T>
inline void RbTreeBase<Key, T>::DetachFromParent(NODE value) YATSC_NOEXCEPT {
  NODE parent = value->parent();
  if (parent != nullptr) {
    if (parent->left() == value) {
      parent->set_left(nullptr);
    } else if (parent->right() == value) {
      parent->set_right(nullptr);
    } else {
      FATAL("Invalid parent pointer.");
    }
    value->set_parent(nullptr);
    value->set_left(nullptr);
    value->set_right(nullptr);
  }
}


template <typename Key, typename T>
inline NODE RbTreeBase<Key, T>::FindLeftMax(NODE node) YATSC_NOEXCEPT {
  auto current = node->left();
  if (current == nullptr) {
    return nullptr;
  }
  while (1) {
    auto tmp = current->right();
    if (tmp == nullptr) {
      return current;
    }
    current = tmp;
  }
  return nullptr;
}

#if defined(DEBUG) || defined(UNIT_TEST)
template <typename Key, typename T>
inline std::string RbTreeBase<Key, T>::ToString() YATSC_NO_SE {
  std::string head;
  return ToStringHelper(head, "", root_);
}


template <typename Key, typename T>
inline std::string RbTreeBase<Key, T>::ToStringHelper(std::string& head, const char* bar, NODE node) YATSC_NO_SE {
  std::string str = "";
  if (node != nullptr) {
    auto tmp = std::move(std::string(head + "    "));
    str += ToStringHelper(tmp, "/", node->right());
    std::string node_str = node->color() == RbTreeNodeColor::kRed ? "R" : "B";
    std::stringstream ss;
    ss << ":" << node->key() << '[' << reinterpret_cast<void*>(node) << ']';
    node_str += ss.str();
    str += head + bar + node_str + "\n";
    tmp = std::move(std::string(head + "    "));
    str += ToStringHelper(tmp, "\\", node->left());
  }
  return str;
}


template <typename Key, typename T>
inline std::vector<typename RbTreeBase<Key, T>::CountResult> RbTreeBase<Key, T>::GetBlackNodeCountOfLeafs() YATSC_NO_SE {
  if (root_ != nullptr) {
    std::vector<CountResult> ret;
    DoGetBlackNodeCountOfLeafs(ret, root_);
    return std::move(ret);
  }
  return std::move(std::vector<CountResult>());
}


template <typename Key, typename T>
inline void RbTreeBase<Key, T>::DoGetBlackNodeCountOfLeafs(std::vector<CountResult>& v, NODE node) YATSC_NO_SE {
  if (node == nullptr) {
    return;
  }

  DoGetBlackNodeCountOfLeafs(v, node->left());
  if (node->left() == nullptr &&
      node->right() == nullptr) {
    NODE tmp = node;
    int count = 0;
    while (tmp != nullptr) {
      if (tmp->IsBlack()) {
        count++;
      }
      tmp = tmp->parent();
    }
    v.push_back(std::pair<int, T>(count, reinterpret_cast<T>(node)));
  }
  DoGetBlackNodeCountOfLeafs(v, node->right());
}
#endif
}

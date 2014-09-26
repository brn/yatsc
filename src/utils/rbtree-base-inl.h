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
template <typename Key, typename T>
YATSC_INLINE NODE RbTreeBase<Key, T>::InsertInternal(Key key, NODE value) {
  return DoInsert(key, value);
}


template <typename Key, typename T>
YATSC_INLINE NODE RbTreeBase<Key, T>::InsertInternal(NODE value) {
  return DoInsert(value->key(), value);
}


template <typename Key, typename T>
YATSC_INLINE NODE RbTreeBase<Key, T>::DeleteInternal(Key key) {
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


template <typename Key, typename T>
YATSC_INLINE NODE RbTreeBase<Key, T>::DeleteInternal(NODE node) {
  if (!node->exists()) {return nullptr;}
  node->set_exists(false);
  DoDelete(node);
  if (root_ != nullptr) {
    root_->ToBlack();
  }
  return node;
}


template <typename Key, typename T>
inline NODE RbTreeBase<Key, T>::DoInsert(Key key, NODE value) {
  if (value->exists()) {
    return value;
  }

  value->set_key(key);
  value->set_exists(true);
  
  size_++;
  
  if (root_ == nullptr) {
    root_ = value;
    value->ToBlack();
    return value;
  }

  if (*root_ == *value) {
    root_->Swap(value);
    root_ = value;
    return value;
  }

  NODE node = root_;
  NODE last = nullptr;
  
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

  value->set_parent(last);
  if (*last > *value) {
    last->set_left(value);
  } else {
    last->set_right(value);
  }
  
  if (last->IsRed()) {
    Rebalance(value);
  }
  
  return value;
}


template <typename Key, typename T>
inline void RbTreeBase<Key, T>::DoDelete(NODE node) {
  size_--;
  bool has_left = node->HasLeft();
  bool has_right = node->HasRight();
  if (has_left && has_right) {
    ElevateLeftMax(node);
  } else if (has_left) {
    ElevateLeftTree(node);
  } else if (has_right) {
    ElevateRightTree(node);
  } else {
    if (node == root_) {
      root_ = nullptr;
    } else {
      if (node->IsBlack()) {
        RebalanceWhenDelete(node);
      }
      DetachFromParent(node);
    }
  }
}


template <typename Key, typename T>
YATSC_INLINE void RbTreeBase<Key, T>::ElevateLeftMax(NODE node) {
  DoElevateNode(node, FindLeftMax(node));
}


template <typename Key, typename T>
YATSC_INLINE void RbTreeBase<Key, T>::ElevateLeftTree(NODE node) {
  DoElevateNode(node, node->left());
}


template <typename Key, typename T>
YATSC_INLINE void RbTreeBase<Key, T>::ElevateRightTree(NODE node) {
  DoElevateNode(node, node->right()); 
}


template <typename Key, typename T>
inline void RbTreeBase<Key, T>::DoElevateNode(NODE node, NODE target) {    
  if (node->IsRed()) {
    if (target->IsBlack()) {
      RebalanceWhenDelete(target);
    }
    DetachFromParent(target);
    node->Swap(target);
    target->set_color(node->color());
    return;
  } else if (target->IsRed()) {
    DetachFromParent(target);
    node->Swap(target);
    target->ToBlack();
    if (!target->HasParent()) {
      root_ = target;
    }
    return;
  }
  
  RebalanceWhenDelete(target);
  DetachFromParent(target);
  node->Swap(target);
}


template <typename Key, typename T>
inline void RbTreeBase<Key, T>::Rebalance(NODE node) {
  NODE current = node;
  while (1) {
    NODE parent = current->parent();
    NODE grand_parent = parent->parent();
    
    if (*parent > *node) {
      if (*grand_parent > *parent) {
        current = RotateR(current, parent, grand_parent);
      } else {
        current = RotateRL(current, parent, grand_parent);
      }
    } else {
      if (*grand_parent > *parent) {
        current = RotateLR(current, parent, grand_parent);
      } else {
        current = RotateL(current, parent, grand_parent);
      }
    }

    if (!current->HasParent()) {
      root_ = current;
      root_->ToBlack();
      return;
    }
    
    if (current->parent()->color() == RbTreeNodeColor::kBlack ||
        current->color() == RbTreeNodeColor::kBlack) {
      return;
    }
  }
}


template <typename Key, typename T>
inline void RbTreeBase<Key, T>::RebalanceWhenDelete(NODE node) {
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
inline NODE RbTreeBase<Key, T>::FindInternal(Key key) YATSC_NO_SE {
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
inline NODE RbTreeBase<Key, T>::RotateR(NODE value, NODE parent, NODE grand_parent, bool when_delete) {
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
inline NODE RbTreeBase<Key, T>::RotateRL(NODE value, NODE parent, NODE grand_parent) {
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
inline NODE RbTreeBase<Key, T>::RotateLR(NODE value, NODE parent, NODE grand_parent) {
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
inline NODE RbTreeBase<Key, T>::RotateL(NODE value, NODE parent, NODE grand_parent, bool when_delete) {
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
inline void RbTreeBase<Key, T>::DetachFromParent(NODE value) {
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
  }
}


template <typename Key, typename T>
inline NODE RbTreeBase<Key, T>::FindLeftMax(NODE node) {
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
    ss << ":" << node->key();
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
    v.push_back(std::pair<int, T>(count, node->node_value()));
  }
  DoGetBlackNodeCountOfLeafs(v, node->right());
}
}

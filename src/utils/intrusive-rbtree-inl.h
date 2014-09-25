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


namespace yatsc {

template <typename Key, typename T>
inline T IntrusiveRBTree<Key, T>::Insert(Key key, T value) {
  value->set_key(key);
  return Insert(value);
}

template <typename Key, typename T>
inline T IntrusiveRBTree<Key, T>::Insert(T value) {
  value->set_exists(true);
  
  size_++;
  
  if (root_ == nullptr) {
    root_ = value;
    value->set_color(RBTreeColor::kBlack);
    return value->Cast();
  }

  if (*root_ == *value) {
    root_->Swap(value);
    root_ = value;
    return value->Cast();
  }

  T node = root_;
  T last = nullptr;
  
  while (node != nullptr) {
    last = node;
    if (*node == *value) {
      node->Swap(value);
      return value->Cast();
    } else if (*node > *value) {
      node = node->left();
    } else if (*node < *value) {
      node = node->right();
    }
  }

  value->set_parent(last);
  if (*last > *value) {
    last->set_left(value);
  } else if (*last < *value) {
    last->set_right(value);
  }
  
  if (last->color() == RBTreeColor::kRed) {
    Rebalance(value);
  }
  
  return value->Cast();
}


template <typename Key, typename T>
inline T IntrusiveRBTree<Key, T>::Delete(Key key) {
  auto node = root_;
  while (node != nullptr) {
    if (node->key() == key) {
      node->set_exists(false);
      DoDelete(node);
      if (root_ != nullptr) {
        root_->ToBlack();
      }
      return node;
    } else if (node->key() > key) {
      node = node->left();
    } else if (node->key() < key) {
      node = node->right();
    }
  }
  return nullptr;
}


template <typename Key, typename T>
inline T IntrusiveRBTree<Key, T>::Delete(T node) {
  if (!node->exists()) {return nullptr;}
  return Delete(node->key());
}


template <typename Key, typename T>
inline void IntrusiveRBTree<Key, T>::DoDelete(T node) {
  size_--;
  if (node->HasLeft() && node->HasRight()) {
    ElevateLeftMax(node);
  } else if (node->HasLeft()) {
    ElevateLeftTree(node);
  } else if (node->HasRight()) {
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
inline void IntrusiveRBTree<Key, T>::ElevateLeftMax(T node) {
  DoElevateNode(node, FindLeftMax(node));
}


template <typename Key, typename T>
inline void IntrusiveRBTree<Key, T>::ElevateLeftTree(T node) {
  DoElevateNode(node, node->left());
}


template <typename Key, typename T>
inline void IntrusiveRBTree<Key, T>::ElevateRightTree(T node) {
  DoElevateNode(node, node->right()); 
}


template <typename Key, typename T>
inline void IntrusiveRBTree<Key, T>::DoElevateNode(T node, T target) {    
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
    return;
  }
  
  RebalanceWhenDelete(target);
  DetachFromParent(target);
  node->Swap(target);
}


template <typename Key, typename T>
inline void IntrusiveRBTree<Key, T>::Rebalance(T node) {
  T current = node;
  while (1) {
    T parent = current->parent();
    if (*parent > *node) {
      current = BalanceR(current);
    } else {
      current = BalanceL(current);
    }
    if (current->parent() == nullptr) {
      root_ = current;
      root_->set_color(RBTreeColor::kBlack);
      break;
    } else if (current->parent()->color() == RBTreeColor::kBlack ||
               current->color() == RBTreeColor::kBlack) {
      break;
    }
  }
}


template <typename Key, typename T>
inline void IntrusiveRBTree<Key, T>::RebalanceWhenDelete(T node) {
  while (1) {
    if (!node->HasParent()) {
      node->ToBlack();
      root_ = node;
      break;
    }
    
    T parent = node->parent();
    T sibling = node->sibling();
    
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

    T sibling_left = sibling != nullptr? sibling->left(): nullptr;
    T sibling_right = sibling != nullptr? sibling->right(): nullptr;
    
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
      RBTreeColor c = parent->color();
      parent->set_color(sibling->color());
      sibling->set_color(c);
      sibling_right->ToBlack();
      break;
    } else if (node->IsRightChild() && sibling_left != nullptr && sibling_left->IsRed()) {
      RotateR(sibling_left, sibling, parent, true);
      RBTreeColor c = parent->color();
      parent->set_color(sibling->color());
      sibling->set_color(c);
      sibling_left->ToBlack();
      break;
    }
    break;
  }
}


template <typename Key, typename T>
inline T IntrusiveRBTree<Key, T>::Find(Key key) {
  if (root_ == nullptr) {return nullptr;}

  if (*root_ == key) {return root_->Cast();}

  auto node = root_;
  
  while (node != nullptr) {
    if (*node == key) {
      return node->Cast();
    } else if (*node > key) {
      node = node->left();
    } else if (*node < key) {
      node = node->right();
    }
  }

  return nullptr;
}

template <typename Key, typename T>
inline T IntrusiveRBTree<Key, T>::Find(T target) {
  if (!target->exists()) {return nullptr;}
  return Find(target->key());
}


template <typename Key, typename T>
inline T IntrusiveRBTree<T, Key>::BalanceR(T value) {
  T parent = value->parent();
  ASSERT(true, parent != nullptr);
  T grand_parent = parent->parent();
  if (grand_parent == nullptr) {
    return parent;
  }
  
  if (*grand_parent > *parent) {
    return RotateR(value, parent, grand_parent);
  } else {
    return RotateRL(value, parent, grand_parent);
  }
}


template <typename Key, typename T>
inline T IntrusiveRBTree<Key, T>::BalanceL(T value) {
  T parent = value->parent();
  ASSERT(true, parent != nullptr);
  T grand_parent = parent->parent();
  if (grand_parent == nullptr) {
    return parent;
  }

  if (*grand_parent > *parent) {
    return RotateLR(value, parent, grand_parent);
  } else {
    return RotateL(value, parent, grand_parent);
  }
}



template <typename Key, typename T>
inline T IntrusiveRBTree<Key, T::RotateR(T value, T parent, T grand_parent, bool when_delete) {
  T right = parent->right();
  T grand_grand_parent = grand_parent->parent();

  parent->set_parent(grand_grand_parent);
  parent->set_right(grand_parent);
  
  grand_parent->set_parent(parent);
  grand_parent->set_left(right);

  if (right != nullptr) {
    right->set_parent(grand_parent);
  }

  if (!when_delete) {
    parent->set_color(RBTreeColor::kRed);
    grand_parent->set_color(RBTreeColor::kBlack);
    if (value != nullptr) {
      value->set_color(RBTreeColor::kBlack);
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
inline T IntrusiveRBTree<Key, T>::RotateRL(T value, T parent, T grand_parent) {
  T right = value->right();
  T left = value->left();
  T grand_grand_parent = grand_parent->parent();

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
  
  value->set_color(RBTreeColor::kRed);
  parent->set_color(RBTreeColor::kBlack);
  grand_parent->set_color(RBTreeColor::kBlack);


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
inline T IntrusiveRBTree<Key, T>::RotateLR(T value, T parent, T grand_parent) {
  T right = value->right();
  T left = value->left();
  T grand_grand_parent = grand_parent->parent();

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

  value->set_color(RBTreeColor::kRed);
  parent->set_color(RBTreeColor::kBlack);
  grand_parent->set_color(RBTreeColor::kBlack);

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
inline T IntrusiveRBTree<Key, T>::RotateL(T value, T parent, T grand_parent, bool when_delete) {
  T left = parent->left();
  T grand_grand_parent = grand_parent->parent();

  parent->set_parent(grand_grand_parent);
  parent->set_left(grand_parent);
  
  grand_parent->set_parent(parent);
  grand_parent->set_right(left);

  if (left != nullptr) {
    left->set_parent(grand_parent);
  }

  if (!when_delete) {
    parent->set_color(RBTreeColor::kRed);
    if (value != nullptr) {
      value->set_color(RBTreeColor::kBlack);
    }
    grand_parent->set_color(RBTreeColor::kBlack);
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
inline void IntrusiveRBTree<Key, T>::DetachFromParent(T value) {
  T parent = value->parent();
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
inline T IntrusiveRBTree<Key, T>::FindLeftMax(T node) {
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


template <typename T, typename Key>
inline std::string IntrusiveRBTree<Key, T>::ToString() YATSC_NO_SE {
  std::string head;
  return ToStringHelper(head, "", root_);
}


template <typename T, typename Key>
inline std::string IntrusiveRBTree<Key, T>::ToStringHelper(std::string& head, const char* bar, T node) YATSC_NO_SE {
  std::string str = "";
  if (node != nullptr) {
    auto tmp = std::move(std::string(head + "    "));
    str += ToStringHelper(tmp, "/", node->right());
    std::string node_str = node->color() == RBTreeColor::kRed ? "R" : "B";
    std::stringstream ss;
    ss << ":" << node->key();
    node_str += ss.str();
    str += head + bar + node_str + "\n";
    tmp = std::move(std::string(head + "    "));
    str += ToStringHelper(tmp, "\\", node->left());
  }
  return str;
}

template <typename T, typename Key>
inline std::vector<typename IntrusiveRBTree<Key, T>::CountResult> IntrusiveRBTree<T, Key>::GetBlackNodeCountOfLeafs() YATSC_NO_SE {
  if (root_ != nullptr) {
    std::vector<CountResult> ret;
    DoGetBlackNodeCountOfLeafs(ret, root_);
    return std::move(ret);
  }
  return std::move(std::vector<CountResult>());
}


template <typename T, typename Key>
inline void IntrusiveRBTree<Key, T>::DoGetBlackNodeCountOfLeafs(std::vector<CountResult>& v, T node) YATSC_NO_SE {
  if (node == nullptr) {
    return;
  }

  DoGetBlackNodeCountOfLeafs(v, node->left());
  if (node->left() == nullptr &&
      node->right() == nullptr) {
    T tmp = node;
    int count = 0;
    while (tmp != nullptr) {
      if (tmp->IsBlack()) {
        count++;
      }
      tmp = tmp->parent();
    }
    v.push_back(std::pair<int, T>(count, node));
  }
  DoGetBlackNodeCountOfLeafs(v, node->right());
}
}

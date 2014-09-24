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

template <typename T, typename Key>
inline T IntrusiveRBTree<T, Key>::Insert(Key key, T value) {
  value->set_key(key);
  return Insert(value);
}

template <typename T, typename Key>
inline T IntrusiveRBTree<T, Key>::Insert(T value) {
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


template <typename T, typename Key>
inline T IntrusiveRBTree<T, Key>::Delete(Key key) {
  auto node = root_;
  while (node != nullptr) {
    if (node->key() == key) {
      node->set_exists(false);
      DoDelete(node);
      return node;
    } else if (node->key() > key) {
      node = node->left();
    } else if (node->key() < key) {
      node = node->right();
    }
  }
  return nullptr;
}


template <typename T, typename Key>
inline T IntrusiveRBTree<T, Key>::Delete(T node) {
  if (!node->exists()) {return nullptr;}
  return Delete(node->key());
}


template <typename T, typename Key>
inline void IntrusiveRBTree<T, Key>::DoDelete(T node) {
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


template <typename T, typename Key>
inline void IntrusiveRBTree<T, Key>::ElevateLeftMax(T node) {
  DoElevateNode(node, FindLeftMax(node));
}


template <typename T, typename Key>
inline void IntrusiveRBTree<T, Key>::ElevateLeftTree(T node) {
  DoElevateNode(node, node->left());
}


template <typename T, typename Key>
inline void IntrusiveRBTree<T, Key>::ElevateRightTree(T node) {
  DoElevateNode(node, node->right()); 
}


template <typename T, typename Key>
inline void IntrusiveRBTree<T, Key>::DoElevateNode(T node, T target) {
  bool black = target->IsBlack();
  if ((!black && node->IsBlack()) ||
      (node->IsRed() && black)) {
    if (target->IsBlack()) {
      RebalanceWhenDelete(target);
    }
    DetachFromParent(target);
    node->Swap(target);
    target->set_color(RBTreeColor::kBlack);
  } else if (node == root_) {
    DetachFromParent(target);
    node->Swap(target);
    root_ = target;
  } else {
    RebalanceWhenDelete(target);
    DetachFromParent(target);
    node->Swap(target);
  }

  if (!target->HasParent()) {
    root_ = target;
  }
}


template <typename T, typename Key>
inline void IntrusiveRBTree<T, Key>::Rebalance(T node) {
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


template <typename T, typename Key>
inline void IntrusiveRBTree<T, Key>::RebalanceWhenDelete(T node) {
  while (node != nullptr) {
    T parent = node->parent();
    printf("%d %hhu\n", parent->key(), parent->color());
    if (node->IsLeftChild() && parent->HasRight() &&
        parent->right()->IsRed()) {
      puts("case 2 L");
      RBTreeColor c = parent->right()->color();
      parent->right()->set_color(parent->color());
      parent->set_color(c);
      RotateL(parent->right()->right(), parent->right(), parent, true);
      Printf("DELETING:%d\n\n%s\n", node->key(), ToString().c_str());
      node = parent;
    } else if (parent->HasLeft() &&
               parent->left()->IsRed()) {
      puts("case 2 R");
      RBTreeColor c = parent->right()->color();
      parent->right()->set_color(parent->color());
      parent->set_color(c);
      RotateR(parent->left()->left(), parent->left(), parent, true);
      Printf("DELETING:%d\n\n%s\n", node->key(), ToString().c_str());
      node = parent;
    } else {
      if (node->IsLeftChild() && parent->IsBlack()) {
        T child = parent->right();
        if (child != nullptr && child->IsBlack()) {
          T left_grandson = child->left();
          T right_grandson = child->right();
          if ((left_grandson == nullptr || left_grandson->IsBlack()) && (right_grandson == nullptr || right_grandson->IsBlack())) {
            puts("case 3 L");
            child->set_color(RBTreeColor::kRed);
            Printf("DELETING:%d\n\n%s\n", node->key(), ToString().c_str());
            node = parent;
            if (node->parent() != nullptr) {
              continue;
            } else {
              break;
            }
          }
        }
      } else if (node->IsRightChild() && parent->IsBlack()) {
        T child = parent->left();
        if (child != nullptr && child->IsBlack()) {
          T left_grandson = child->left();
          T right_grandson = child->right();
          if ((left_grandson == nullptr || right_grandson == nullptr) || (left_grandson->IsBlack() && right_grandson->IsBlack())) {
            puts("case 3 R");
            child->set_color(RBTreeColor::kRed);
            Printf("DELETING:%d\n\n%s\n", node->key(), ToString().c_str());
            node = parent;
            if (node->parent() != nullptr) {
              continue;
            } else {
              break;
            }
          }
        }
      }
    }

    if (node->IsLeftChild() && parent->IsRed()) {
      T child = parent->right();
      if (child != nullptr && child->IsBlack()) {
        T left_grandson = child->left();
        T right_grandson = child->right();
        if ((left_grandson == nullptr || left_grandson->IsBlack()) &&
            (right_grandson == nullptr || right_grandson->IsBlack())) {
          puts("case 4 L");
          child->set_color(RBTreeColor::kRed);
          parent->set_color(RBTreeColor::kBlack);
        }
      }
    } else if (node->IsRightChild() && parent->IsRed()) {
      T child = parent->left();
      if (child != nullptr && child->IsBlack()) {
        T left_grandson = child->left();
        T right_grandson = child->right();
        if ((left_grandson == nullptr || left_grandson->IsBlack()) &&
            (right_grandson == nullptr || right_grandson->IsBlack())) {
          puts("case 4 R");
          child->set_color(RBTreeColor::kRed);
          parent->set_color(RBTreeColor::kBlack);
          Printf("DELETING:%d\n\n%s\n", node->key(), ToString().c_str());
        }
      }
    }

    if (node->IsLeftChild()) {
      T child = parent->right();
      if (child != nullptr) {
        T left_grandson = child->left();
        T right_grandson = child->right();
        if (left_grandson != nullptr &&
            right_grandson != nullptr) {
          if (left_grandson->IsRed() && right_grandson->IsBlack()) {
            puts("case 5 L");
            RotateR(left_grandson->left(), left_grandson, child, true);
            parent->set_color(RBTreeColor::kRed);
            left_grandson->set_color(RBTreeColor::kBlack);
          }
        }
      }
    } else if (node->IsRightChild()) {
      T child = parent->left();
      if (child != nullptr) {
        T left_grandson = child->left();
        T right_grandson = child->right();
        if (left_grandson != nullptr &&
            right_grandson != nullptr) {
          if (left_grandson->IsBlack() && right_grandson->IsRed()) {
            puts("case 5 R");
            RotateL(right_grandson->right(), right_grandson, child, true);
            parent->set_color(RBTreeColor::kRed);
            right_grandson->set_color(RBTreeColor::kBlack);
          }
        }
      }
    }

    if (node->IsLeftChild()) {
      T child = parent->right();
      if (child != nullptr) {
        T right_grandson = child->right();
        if (right_grandson != nullptr &&
            right_grandson->IsRed()) {
          puts("case 6 L");
          RotateL(right_grandson, child, parent, true);
          RBTreeColor c = child->color();
          child->set_color(parent->color());
          parent->set_color(c);
          right_grandson->set_color(RBTreeColor::kBlack);
        }
      }
    } else if (node->IsRightChild()) {
      T child = parent->left();
      if (child != nullptr) {
        T left_grandson = child->left();
        if (left_grandson != nullptr &&
            left_grandson->IsRed()) {
          puts("case 6 R");
          RotateR(left_grandson, child, parent, true);
          RBTreeColor c = child->color();
          child->set_color(parent->color());
          parent->set_color(c);
          left_grandson->set_color(RBTreeColor::kBlack);
        }
      }
    }
    break;
  }
}


template <typename T, typename Key>
inline T IntrusiveRBTree<T, Key>::Find(Key key) {
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

template <typename T, typename Key>
inline T IntrusiveRBTree<T, Key>::Find(T target) {
  if (!target->exists()) {return nullptr;}
  return Find(target->key());
}


template <typename T, typename Key>
inline T IntrusiveRBTree<T, Key>::BalanceR(T value) {
  T parent = value->parent();
  ASSERT(true, parent != nullptr);
  T grand_parent = parent->parent();
  if (grand_parent == nullptr) {
    return parent;
  }
  
  if (*grand_parent > *parent) {
    //puts("RotateR");
    return RotateR(value, parent, grand_parent);
  } else {
    //puts("RotateRL");
    return RotateRL(value, parent, grand_parent);
  }
}


template <typename T, typename Key>
inline T IntrusiveRBTree<T, Key>::BalanceL(T value) {
  T parent = value->parent();
  ASSERT(true, parent != nullptr);
  T grand_parent = parent->parent();
  if (grand_parent == nullptr) {
    return parent;
  }

  if (*grand_parent > *parent) {
    //puts("RotateLR");
    return RotateLR(value, parent, grand_parent);
  } else {
    //puts("RotateL");
    return RotateL(value, parent, grand_parent);
  }
}



template <typename T, typename Key>
inline T IntrusiveRBTree<T, Key>::RotateR(T value, T parent, T grand_parent, bool when_delete) {
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


template <typename T, typename Key>
inline T IntrusiveRBTree<T, Key>::RotateRL(T value, T parent, T grand_parent) {
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


template <typename T, typename Key>
inline T IntrusiveRBTree<T, Key>::RotateLR(T value, T parent, T grand_parent) {
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


template <typename T, typename Key>
inline T IntrusiveRBTree<T, Key>::RotateL(T value, T parent, T grand_parent, bool when_delete) {
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


template <typename T, typename Key>
inline void IntrusiveRBTree<T, Key>::DetachFromParent(T value) {
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


template <typename T, typename Key>
inline T IntrusiveRBTree<T, Key>::FindLeftMax(T node) {
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
inline std::string IntrusiveRBTree<T, Key>::ToString() YATSC_NO_SE {
  std::string head;
  return ToStringHelper(head, "", root_);
}


template <typename T, typename Key>
inline std::string IntrusiveRBTree<T, Key>::ToStringHelper(std::string& head, const char* bar, T node) YATSC_NO_SE {
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
inline std::vector<typename IntrusiveRBTree<T, Key>::CountResult> IntrusiveRBTree<T, Key>::GetBlackNodeCountOfLeafs() YATSC_NO_SE {
  if (root_ != nullptr) {
    std::vector<CountResult> ret;
    DoGetBlackNodeCountOfLeafs(ret, root_);
    return std::move(ret);
  }
  return std::move(std::vector<CountResult>());
}


template <typename T, typename Key>
inline void IntrusiveRBTree<T, Key>::DoGetBlackNodeCountOfLeafs(std::vector<CountResult>& v, T node) YATSC_NO_SE {
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

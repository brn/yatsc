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
    Swap(root_, value);
    root_ = value;
    return value->Cast();
  }

  T node = root_;
  T last = nullptr;
  
  while (node != nullptr) {
    last = node;
    if (*node == *value) {
      Swap(node, value);
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
      Remove(node);
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
inline void IntrusiveRBTree<T, Key>::Remove(T node) {
  if (node->HasLeft()) {
    DeleteLeftMax(node);
  } else if (node->HasRight()) {
    T right = node->right();
    RBTreeColor color = right->color();
    DetachFromParent(right);
    Swap(node, right);
    if (color == RBTreeColor::kBlack) {
      RebalanceLeftWhenDelete(right->parent());
    }
  } else {
    RBTreeColor color = node->color();
    T parent = node->parent();
    bool left = false;
    if (parent->left() == node) {
      left = true;
    }
    DetachFromParent(node);
    if (color == RBTreeColor::kBlack) {
      T tmp;
      if (left) {
        tmp = RebalanceRightWhenDelete(parent);
      } else {
        tmp = RebalanceLeftWhenDelete(parent);
      }
      if (tmp != nullptr) {
        RebalanceWhenDelete(tmp->parent(), tmp);
      }
    }
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
inline void IntrusiveRBTree<T, Key>::RebalanceWhenDelete(T parent, T node) {
  while (1) {
    if (parent->left() == node) {
      node = RebalanceRightWhenDelete(parent);
    } else {
      node = RebalanceLeftWhenDelete(parent);
    }
    if (node != nullptr) {
      parent = node->parent();
      if (parent == nullptr) {
        node->set_color(RBTreeColor::kBlack);
        break;
      }
    } else {
      break;
    }
  }
}


template <typename T, typename Key>
inline T IntrusiveRBTree<T, Key>::RebalanceLeftWhenDelete(T grand_parent) {
  T parent = grand_parent->left();
  if (parent != nullptr) {
    RBTreeColor color = grand_parent->color();
    T left = parent->left();
    T right = parent->right();
    if (right != nullptr && right->IsRed()) {
      puts("1");
      RotateLR(right, parent, grand_parent);
      right->set_color(color);
    } else if (left != nullptr && left->IsRed()) {
            puts("2");
      RotateR(left, parent, grand_parent);
      parent->set_color(color);
    } else {
      if (parent->IsRed()) {
              puts("3");
        T right = parent->right();
        T grand_grand_parent = grand_parent->parent();

        parent->set_right(grand_parent);
        grand_parent->set_left(right);

        if (right != nullptr) {
          right->set_parent(grand_parent);
        }

        grand_parent->set_parent(parent);
        parent->set_parent(grand_grand_parent);
        
        if (grand_grand_parent != nullptr) {
          if (grand_grand_parent->left() == grand_parent) {
            grand_grand_parent->set_left(parent);
          } else {
            grand_grand_parent->set_right(parent);
          }
        } else if (grand_grand_parent == root_) {
          root_ = parent;
          root_->set_color(RBTreeColor::kBlack);
        }

        return grand_parent;
      } else {
        puts("4");
        bool more = grand_parent->IsBlack();
        grand_parent->set_color(RBTreeColor::kBlack);
        parent->set_color(RBTreeColor::kRed);
        printf("%s %d\n", ToString().c_str(), grand_parent->key());
        if (more) {
          return grand_parent;
        }
      }
    }
  }
  return nullptr;
}


template <typename T, typename Key>
inline T IntrusiveRBTree<T, Key>::RebalanceRightWhenDelete(T grand_parent) {
  T parent = grand_parent->right();
  if (parent != nullptr) {
    RBTreeColor color = grand_parent->color();
    T left = parent->left();
    T right = parent->right();
    if (left != nullptr && left->IsRed()) {
      puts("11");
      RotateRL(left, parent, grand_parent);
      left->set_color(color);
    } else if (right != nullptr && right->IsRed()) {
      puts("12");
      RotateL(right, parent, grand_parent);
      parent->set_color(color);
    } else {
      if (parent->IsRed()) {
        puts("13");
        T left = parent->left();
        T grand_grand_parent = grand_parent->parent();

        parent->set_left(grand_parent);
        grand_parent->set_right(left);

        if (left != nullptr) {
          left->set_parent(grand_parent);
        }

        grand_parent->set_parent(parent);
        parent->set_parent(grand_grand_parent);

        if (grand_grand_parent != nullptr) {
          if (grand_grand_parent->left() == grand_parent) {
            grand_grand_parent->set_left(parent);
          } else {
            grand_grand_parent->set_right(parent);
          }
        } else {
          root_ = parent;
          root_->set_color(RBTreeColor::kBlack);
        }

        printf("%s %d\n", ToString().c_str(), grand_parent->key());
        return grand_parent;
      } else {
        puts("14");
        bool more = grand_parent->IsBlack();
        grand_parent->set_color(RBTreeColor::kBlack);
        parent->set_color(RBTreeColor::kRed);
        printf("%s %d\n", ToString().c_str(), grand_parent->key());
        if (more) {
          return grand_parent;
        }
      }
    }
  }
  return nullptr;
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
inline void IntrusiveRBTree<T, Key>::Swap(T old_value, T new_value) {
  T left = old_value->left();
  T right = old_value->right();
  if (left != nullptr && left != new_value) {
    new_value->set_left(left);
    left->set_parent(new_value);
  }
  if (right != nullptr && right != new_value) {
    new_value->set_right(right);
    right->set_parent(new_value);
  }
  
  T parent = old_value->parent();
  if (parent != nullptr) {
    new_value->set_parent(parent);
    if (parent->left() == old_value) {
      parent->set_left(new_value);
    } else if (parent->right() == old_value) {
      parent->set_right(new_value);
    } else {
      FATAL("Invalid parent.");
    }
  }
  old_value->set_left(nullptr);
  old_value->set_right(nullptr);
  old_value->set_parent(nullptr);
  new_value->set_color(old_value->color());
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
inline T IntrusiveRBTree<T, Key>::RotateR(T value, T parent, T grand_parent) {
  T right = parent->right();
  T grand_grand_parent = grand_parent->parent();

  parent->set_parent(grand_grand_parent);
  parent->set_right(grand_parent);
  
  grand_parent->set_parent(parent);
  grand_parent->set_left(right);

  if (right != nullptr) {
    right->set_parent(grand_parent);
  }
  
  parent->set_color(RBTreeColor::kRed);
  grand_parent->set_color(RBTreeColor::kBlack);
  value->set_color(RBTreeColor::kBlack);

  if (grand_grand_parent != nullptr) {
    if (*grand_grand_parent > *grand_parent) {
      grand_grand_parent->set_left(parent);
    } else {
      grand_grand_parent->set_right(parent);
    }
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
inline T IntrusiveRBTree<T, Key>::RotateL(T value, T parent, T grand_parent) {
  T left = parent->left();
  T grand_grand_parent = grand_parent->parent();

  parent->set_parent(grand_grand_parent);
  parent->set_left(grand_parent);
  
  grand_parent->set_parent(parent);
  grand_parent->set_right(left);

  if (left != nullptr) {
    left->set_parent(grand_parent);
  }

  parent->set_color(RBTreeColor::kRed);
  value->set_color(RBTreeColor::kBlack);
  grand_parent->set_color(RBTreeColor::kBlack);
  
  if (grand_grand_parent != nullptr) {
    if (*grand_grand_parent > *grand_parent) {
      grand_grand_parent->set_left(parent);
    } else {
      grand_grand_parent->set_right(parent);
    }
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
inline void IntrusiveRBTree<T, Key>::DeleteLeftMax(T node) {
  auto current = node->left();
  while (1) {
    auto tmp = current->right();
    if (tmp == nullptr) {
      T parent = current->parent();
      bool left = parent->left() == current;
      RBTreeColor color = current->color();
      RBTreeColor delete_color = node->color();
      DetachFromParent(current);
      Swap(node, current);
      if (parent == node) {
        parent = current->parent();
      }
      
      if (color == RBTreeColor::kRed) {
        return;
      }

      current->set_color(color);
      
      if (current->IsBlack() || delete_color == RBTreeColor::kBlack) {
        T ret = left? RebalanceRightWhenDelete(parent): RebalanceLeftWhenDelete(parent);
        if (ret != nullptr) {
          RebalanceWhenDelete(ret->parent(), ret);
        }
      }
      break;
    }
    current = tmp;
  }
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
    str += ToStringHelper(std::string(head + "    "), "/", node->right());
    std::string node_str = node->color() == RBTreeColor::kRed ? "R" : "B";
    std::stringstream ss;
    ss << ":" << node->key();
    node_str += ss.str();
    str += head + bar + node_str + "\n";
    str += ToStringHelper(std::string(head + "    "), "\\", node->left());
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
        printf("%d\n", tmp->key());
        count++;
      }
      tmp = tmp->parent();
    }
    v.push_back(std::pair<int, T>(count, node));
  }
  DoGetBlackNodeCountOfLeafs(v, node->right());
}
}

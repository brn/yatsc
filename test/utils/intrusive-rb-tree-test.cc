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

#include <gtest/gtest.h>
#include <random>
#include <array>
#include "../../src/utils/intrusive-rbtree.h"


class RBValueObject: public yatsc::IntrusiveRBTreeValueBase<RBValueObject*, int> {
 public:
  RBValueObject()
      : IntrusiveRBTreeValueBase() {}

  RBValueObject(int i)
      : IntrusiveRBTreeValueBase(i),
        s(i){}

  size_t s;
};


template <typename T, typename Key>
inline ::testing::AssertionResult CompareDistance(const yatsc::IntrusiveRBTree<T, Key>& tree, const char* before = nullptr) {
  auto leaf_nodes = tree.GetBlackNodeCountOfLeafs();
  if (tree.size() == 0) {
    if (leaf_nodes.size() == 0u) {
      return ::testing::AssertionSuccess();
    }
    return ::testing::AssertionFailure()
      << "\nLeaf nodes size is not valid.\n"
      << "leaf size: " << leaf_nodes.size() << "\n"
      << "node size: " << tree.size() << "\n";
  }
  if (leaf_nodes.size() == 0u) {
    return ::testing::AssertionFailure()
      << "Leaf nodes is not exists.";
  }
  int count = leaf_nodes[0].first;
  for (size_t i = 1; i < leaf_nodes.size(); i++) {
    if (count != leaf_nodes[i].first) {
      auto ret = ::testing::AssertionFailure()
        << "\nBad structure detected." << "\n"
        << "expected count: " << count << " key: " << leaf_nodes[0].second->s << "\n"
        << "actual count  : " << leaf_nodes[i].first << " key " << leaf_nodes[i].second->s << "\n";
      if (before != nullptr) {
        ret << "BEFORE TREE:\n" << before << "\n";
      }
      ret << "TREE:\n" << tree.ToString();
      return ret;
    }
  }
  return ::testing::AssertionSuccess();
}


TEST(IntrusiveRBTree, Insert) {
  yatsc::IntrusiveRBTree<RBValueObject*, int> tree;
  uint64_t ok = 0u;
  
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<size_t> size(1, 1000);
  std::vector<int> v;
  for (int i = 0; i < 1000; i++) {
    int s = size(mt);
    auto value = new RBValueObject();
    value->s = s;
    tree.Insert(s, value);
    v.push_back(s);
    if (s % 2 == 0) {
      value = new RBValueObject(s);
      value->s = s;
      tree.Insert(s, value);
      v.push_back(s);
    }
  }

  ASSERT_TRUE(CompareDistance(tree));
  
  for (auto s: v) {
    ASSERT_TRUE(tree.Find(s) != nullptr);
  }
}


TEST(IntrusiveRBTree, Delete) {
  yatsc::IntrusiveRBTree<RBValueObject*, int> tree;
  std::vector<RBValueObject*> v;
  std::array<RBValueObject*, 33> x = {{
      new RBValueObject(3),
      new RBValueObject(14),
      new RBValueObject(56),
      new RBValueObject(33),
      new RBValueObject(48),
      new RBValueObject(92),
      new RBValueObject(62),
      new RBValueObject(22),
      new RBValueObject(9),
      new RBValueObject(1),
      new RBValueObject(10),
      new RBValueObject(31),
      new RBValueObject(141),
      new RBValueObject(561),
      new RBValueObject(331),
      new RBValueObject(481),
      new RBValueObject(921),
      new RBValueObject(621),
      new RBValueObject(221),
      new RBValueObject(91),
      new RBValueObject(41),
      new RBValueObject(101),
      new RBValueObject(1000),
      new RBValueObject(1001),
      new RBValueObject(1002),
      new RBValueObject(1003),
      new RBValueObject(1004),
      new RBValueObject(1005),
      new RBValueObject(1006),
      new RBValueObject(1007),
      new RBValueObject(1008),
      new RBValueObject(1009),
      new RBValueObject(1010)}};
  
  for (auto i: x) {
    tree.Insert(i);
  }

  for (size_t i = 0; i < x.size(); i++) {
    tree.Delete(x[i]);
    ASSERT_TRUE(CompareDistance(tree));
  }
}


TEST(IntrusiveRBTree, InsertAndDelete) {
  yatsc::IntrusiveRBTree<RBValueObject*, int> tree;
  uint64_t ok = 0u;
  
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<size_t> size(1, 100000);
  std::vector<int> v;
  for (int i = 0; i < 100; i++) {
    int s = size(mt);
    auto value = new RBValueObject(s);
    tree.Insert(value);
    ASSERT_TRUE(CompareDistance(tree));
    v.push_back(s);
    
    if (i % 50 == 0) {
      for (size_t i = 0; i < v.size(); i++) {
        std::string&& before = std::move(tree.ToString());
        printf("DELETE: %d===================\n%s\n", v[i], before.c_str());
        tree.Delete(v[i]);
        ASSERT_TRUE(CompareDistance(tree, before.c_str()));
      }
      v.clear();
    }
  }
  
  for (auto s: v) {
    ASSERT_TRUE(tree.Find(s) != nullptr);
  }
}

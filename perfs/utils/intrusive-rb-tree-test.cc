// The MIT License (MIT)
// 
// Copyright (c) 2013 Taketoshi Aono(brn)
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.



#include <celero/Celero.h>


#include <random>
#include <map>
#include <unordered_map>
#include <array>
#include "../../src/utils/rbtree-base.h"
#include "../../src/utils/intrusive-rbtree.h"


class RBValueObject: public yatsc::RbTreeNode<size_t, RBValueObject*> {
 public:
  RBValueObject()
      : RbTreeNode() {}

  RBValueObject(size_t i)
      : RbTreeNode(i),
        s(i){}

  size_t s;
};



class IntrusiveRbTreeFixture: public celero::TestFixture {
 public:
  IntrusiveRbTreeFixture() {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<size_t> size(1, 100000);
    int is = 10000;
    values.reserve(is);
    for (int i = 0; i < is; i++) {
      size_t s = size(mt);
      values.push_back(s);
    }
  }


  virtual std::vector<int64_t> getExperimentValues() const {
    std::vector<int64_t> problemSpace;
    const int totalNumberOfTests = 6;

    for(int i = 0; i < totalNumberOfTests; i++) {
      problemSpace.push_back(static_cast<int64_t>(pow(2, i+1)));
    }

    return problemSpace;
  }

  virtual void setUp() {}

  virtual void tearDown() {}

 protected:
  std::vector<size_t> values;
};


CELERO_MAIN;


BASELINE_F(IntrusiveRbTreeVSMap, Baseline, IntrusiveRbTreeFixture, 10, 100) {
  std::map<size_t, RBValueObject*> tree;
  int j = 0;
  for (auto i: this->values) {
    auto value = new RBValueObject(i);
    tree.insert(std::pair<size_t, RBValueObject*>(i, value));
    j++;
  }

  for (auto i: this->values) {
    tree.find(i);
  }
}


BASELINE_F(IntrusiveRbTreeVSUMap, Baseline, IntrusiveRbTreeFixture, 10, 100) {
  std::unordered_map<size_t, RBValueObject*> tree;
  int j = 0;
  
  for (auto i: values) {
    auto value = new RBValueObject(i);
    tree.insert(std::pair<size_t, RBValueObject*>(i, value));
    j++;
  }

  for (auto i: values) {
    tree.find(i);
  }
}


BENCHMARK_F(IntrusiveRbTreeVSMap, InsertAndFind, IntrusiveRbTreeFixture, 10, 100) {
  yatsc::IntrusiveRbTree<size_t, RBValueObject*> tree;
  int j = 0;
  for (auto i: values) {
    auto value = new RBValueObject(i);
    tree.Insert(i, value);
    j++;
  }

  for (auto i: this->values) {
    tree.Find(i);
  }
}


BENCHMARK_F(IntrusiveRbTreeVSUMap, InsertAndFind, IntrusiveRbTreeFixture, 10, 100) {
  yatsc::IntrusiveRbTree<size_t, RBValueObject*> tree;
  int j = 0;
  for (auto i: values) {
    auto value = new RBValueObject(i);
    tree.Insert(i, value);
    j++;
  }

  for (auto i: values) {
    tree.Find(i);
  }
}

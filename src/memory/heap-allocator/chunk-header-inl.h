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


namespace yatsc { namespace heap {

void ChunkHeader::BinaryHeap::Insert(ChunkHeader::HeapHeader* header) {
  if (tree_.size() == 0) {
    tree_.push_back(header);
  } else {
    tree_.push_back(header);
    SiftUp();
  }
}


void ChunkHeader::BinaryHeap::Update() {
  if (tree_.size() > 1) {
    SiftDown(0);
  }
}


void ChunkHeader::BinaryHeap::SiftUp(ChunkHeader::HeapHeader* header) {
  size_t position = tree_.size() - 1;
  HeapHeader* header_tmp = nullptr;
  for (size_t i = 0; i < position; i++) {
    size_t parent_position = (position - 1) >> 1;
    header_tmp = tree_[parent_position];
    if (header_tmp != nullptr && header_tmp->used() > header->used()) {
      tree_[parent_position] = header;
      tree_[position] = header_tmp;
    } else {
      break;
    }
  }
}


void ChunkHeader::BinaryHeap::SiftDown(size_t position) {
  auto len = tree_.size();
  ChunkHeader::HeapHeader* value = nullptr;
  size_t left = nullptr;
  size_t right = nullptr;
  size_t pos_tmp;
  size_t left_value;
  size_t right_value;
  
  while (position < len) {
    value = tree_[position];
    pos_tmp = position << 1;
    left = pos_tmp + 1;
    right = pos_tmp + 2;
    left_value = tree_[left];
    right_value = tree_[right];

    bool cmp = left_value != nullptr && right_value != nullptr? left_value->used() < right_value->used(): left_value? true: false;
    if (cmp && left_value != nullptr && left_value->used() < value->used()) {
      tree_[position] = left_value;
      tree_[left] = value;
      position = left;
    } else if (!cmp && right_value != nullptr && right_value->used() < value->used()) {
      tree_[position] = right_value;
      tree_[right] = value;
      position = right;
    } else {
      break;
    }
  }
}

}}

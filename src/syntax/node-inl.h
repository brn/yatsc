/*
 * The MIT License (MIT)
 * 
 * Copyright (c) Taketoshi Aono(brn)
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


namespace rasp {

Node::Node(NodeType type):
    type_(type) {}


Node* Node::first_child() RASP_NO_SE {
  return children_.front();
}


Node* Node::last_child() RASP_NO_SE {
  return children_.back();
}


Node* Node::nth_child(int n) RASP_NO_SE {
  return children_.at(n);
}


void Node::AppendChild(Node* n) {
  children_.push_back(n):
}


void Node::InsertBefore(Node* newNode, Node* oldNode) {
  NodeIterator it = children_.begin();
  NodeIterator end = children_.end();
  for (;it != end; ++it) {
    if ((*it) == oldNode) {
      break;
    }
  }

  if (it != end) {
    children_.insert(it, newNode);
  }
}


void Node::InsertAfter(Node* newNode, Node* oldNode) {
  NodeIterator it = children_.begin();
  NodeIterator end = children_.end();
  for (;it != end; ++it) {
    if ((*it) == oldNode) {
      break;
    }
  }

  if ((it + 1) != end) {
    children_.insert(it + 1, newNode);
  } else {
    children_.push_back(newNode);
  }
}


NodeList&& Node::children() RASP_NO_SE {
  NodeList list(children_.begin(), children_.end());
  return std::move(list);
}


std::string Node::ToString() {
  return std::string("");
}


std::string Node::ToStringTree() {
  return std::string("");
}
}

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

#include <string>
#include <vector>
#include "../utils/utils.h"

namespace rasp {

// Node types.
enum class NodeType: unit8_t {};

// #Forward declaration.
class Node;
// #End


// The AbstractSyntax Tree representation class.
// This class role as generic node for all ast.
// Asts are identified by NodeType and childrens.
class Node {
 public:
  Node() = default;

  /**
   * Return first child of the children.
   * @returns The first child of the children or nullptr.
   */
  RASP_INLINE Node* first_child() RASP_NO_SE;

  /**
   * Return last child of the children.
   * @returns The last child of the children or nullptr.
   */
  RASP_INLINE Node* last_child() RASP_NO_SE;

  /**
   * Insert new Node.
   * @param n New node.
   */
  RASP_INLINE void AppendChild(Node* n);

  /**
   * Insert a new Node in front of a specified existing node of the children.
   * @param newNode The node that will newly insert.
   * @param oldNode The node that has inserted.
   */
  RASP_INLINE void InsertBefore(Node* newNode, Node* oldNode);

  /**
   * Insert a new Node behind a specified old Node.
   * @param newNode The node that will newly insert.
   * @param oldNode The node that has inserted.
   */
  RASP_INLINE void InsertAfter(Node* newNode, Node* oldNode);

  /**
   * Return NodeType of the node.
   * @returns The NodeType of this node.
   */
  RASP_INLINE NodeType type() RASP_NO_SE;

  /**
   * Return children.
   * @returns The node list of the children.
   */
  RASP_INLINE std::vecotr<Node*>&& children() RASP_NO_SE;

  /**
   * Return formated string expression of this node.
   * @returns The string expression of this node.
   */
  std::string ToString();

  /**
   * Return formated string expression of this node and children.
   * @returns The string expression of this node and children.
   */
  std::string ToStringTree();
};
}

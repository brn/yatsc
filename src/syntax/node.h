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
enum class NodeType: unit8_t {
  ERROR = 0,
  RETURN,
  BITOR,
  BITXOR,
  BITAND,
  EQ,
  NE,
  LT,
  LE,
  GT,
  GE,
  LSH,
  RSH,
  URSH,
  ADD,
  SUB,
  MUL,
  DIV,
  MOD,
  NOT,
  BITNOT,
  POS,
  NEG,
  NEW,
  DELPROP,
  TYPEOF,
  GETPROP,
  GETELEM,
  CALL,
  NAME,
  NUMBER,
  STRING,
  NULL,
  THIS,
  FALSE,
  TRUE,
  SHEQ,
  SHNE,
  REGEXP,
  THROW,
  IN,
  INSTANCEOF,
  ARRAYLIT,
  OBJECTLIT,
  TRY,
  PARAM_LIST,
  COMMA,
  ASSIGN,
  ASSIGN_BITOR,
  ASSIGN_BITXOR,
  ASSIGN_BITAND,
  ASSIGN_LSH,
  ASSIGN_RSH,
  ASSIGN_URSH,
  ASSIGN_ADD,
  ASSIGN_SUB,
  ASSIGN_MUL,
  ASSIGN_DIV,
  ASSIGN_MOD,
  HOOK,
  OR,
  AND,
  INC,
  DEC,
  FUNCTION,
  IF,
  SWITCH,
  CASE,
  DEFAULT_CASE,
  WHILE,
  DO,
  FOR,
  BREAK,
  CONTINUE,
  VAR,
  WITH,
  CATCH,
  CLASS,
  FIELD,
  STRING_KEY,
  IMPORT,
  VOID,
  EMPTY,
  BLOCK,
  LABEL,
  EXPR_RESULT,
  SCRIPT,
  GETTER_DEF,
  SETTER_DEF,
  CONST,
  DEBUGGER
};


// The AbstractSyntax Tree representation class.
// This class role as generic node for all ast.
// Asts are identified by NodeType and childrens.
class Node {
  typedef NodeList Vector<Node*>;
  typedef NodeIterator NodeList::iterator;
 public:
  RASP_INLINE Node(NodeType type);

  /**
   * Return first child.
   * @returns The child Node pointer or nullptr.
   */
  RASP_INLINE Node* first_child() RASP_NO_SE;

  /**
   * Return last child.
   * @returns The child Node pointer or nullptr.
   */
  RASP_INLINE Node* last_child() RASP_NO_SE;

  /**
   * Return a specified index child Node.
   * @returns The child Node pointer or nullptr.
   */
  RASP_INLINE Node* nth_child(int n) RASP_NO_SE;
  
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
  RASP_INLINE NodeType type() RASP_NO_SE {return type_;};
  
  /**
   * Return children.
   * @returns The node list of the children.
   */
  RASP_INLINE NodeList&& children() RASP_NO_SE;

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

 private:
  NodeType type_;
  NodeList children_;
};
}

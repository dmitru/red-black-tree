
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>

#include "btree.h"

#ifdef DEBUG
#define DUMP(format, ...) (fprintf(stderr, format, ##__VA_ARGS__))
#else
#define DUMP(format, ...) 
#endif

#define TOINT(x) ((x == NULL)? -1 : *((int*)(x)->data))
#define COLOR(node) (((node) == NULL)? BTREE_BLACK : (node)->color)

BTree* btree_create(int (*cmp) (void *, void *))
{
  BTree *t = (BTree*)malloc(sizeof(BTree));
  t->cmp = cmp;
  t->root = NULL;
  t->begin = NULL;
  t->end = NULL;
  return t;
}

bool btree_isempty(BTree *t)
{
  return t->root == NULL;
}

/*static char* node_dump(Node *n)
{
  const size_t buf_sz = 1024;
  static char str[buf_sz];
  if (n != NULL)
    snprintf(str, buf_sz, "%3d%c", *(int*)(n->data), (n->color == BTREE_RED)? 'r' : 'b');
  else
    snprintf(str, buf_sz, "NILb"  );
  return str;
}*/


static Node* insert_helper(BTree *t, Node **node, Node *parent, void *data)
{
  if ((*node) == NULL) {
    Node *new_node = (Node*)malloc(sizeof(Node)); 
    if (new_node == NULL) 
      return NULL;
    if (t->root == NULL) {
      t->begin = new_node;
      t->end = new_node;
    }
    *node = new_node;
    (*node)->data = data; 
    (*node)->parent = parent;
    (*node)->left = NULL;
    (*node)->right = NULL;
    if (parent != NULL) {
      if (parent == t->begin && parent->left == *node) {
        t->begin = *node;
      }
      else if (parent == t->end && parent->right == *node) {
        t->end = *node;
      }
    }
    return *node;
  } 
  int cmp_result = (*(t->cmp))(data, (*node)->data);
  if (cmp_result == 0) {
    return NULL;
  } else if (cmp_result > 0) {
    return insert_helper(t, &((*node)->right), *node, data);
  } else {
    return insert_helper(t, &((*node)->left), *node, data); 
  }
}

static void left_rotation(BTree *tree, Node *x)
{
  Node *y = x->right;
  x->right = y->left;
  if (y->left != NULL) 
    y->left->parent = x;
  y->parent = x->parent;
  if (x->parent == NULL) 
    tree->root = y;
  else {
    if (x == x->parent->left)
      x->parent->left = y;
    else
      x->parent->right = y;
  }
  y->left = x;
  x->parent = y;
}

static void right_rotation(BTree *tree, Node *y)
{
  Node *x = y->left;
  y->left = x->right;
  if (x->right != NULL) 
    x->right->parent = y;
  x->parent = y->parent;
  if (y->parent == NULL) 
    tree->root = x;
  else {
    if (y == y->parent->left)
      y->parent->left = x;
    else
      y->parent->right = x;
  }
  x->right = y;
  y->parent = x;
}

bool btree_insert(BTree *tree, void *data)
{ 
  Node *x = NULL;
  if ((x = insert_helper(tree, &tree->root, NULL, data)) == NULL)
    return false;
  x->color = BTREE_RED;
  while (COLOR(x->parent) == BTREE_RED) {
    Node *p = x->parent;
    Node *pp = p->parent;
    if (p == pp->left) {
      Node *y = pp->right;
      if (COLOR(y) == BTREE_RED) {
        p->color = BTREE_BLACK;
        y->color = BTREE_BLACK;
        pp->color = BTREE_RED;
        x = pp;
      } else {
        if (x == p->right) {
          x = x->parent;
          left_rotation(tree, x);
          p = x->parent;
          pp = p->parent;
        }
        p->color = BTREE_BLACK;
        pp->color = BTREE_RED;
        right_rotation(tree, pp);
      } 
    } else {
      Node *y = pp->left;
      if (COLOR(y) == BTREE_RED) {
        p->color = BTREE_BLACK;
        y->color = BTREE_BLACK;
        pp->color = BTREE_RED;
        x = pp;
      } else {
        if (x == p->left) {
          x = p;
          right_rotation(tree, x);
          p = x->parent;
          pp = p->parent;
        }
        p->color = BTREE_BLACK;
        pp->color = BTREE_RED;
        left_rotation(tree, pp);
      } 
    }
  }
  tree->root->color = BTREE_BLACK;
  return true;
}

static BTreeIterator find_helper(BTree *tree, Node *node, void *data)
{
  if (node == NULL) {
    ////DUMP("find_helper(%p, NULL, %d)\n", tree, *(int*)data);
    BTreeIterator res = {tree, NULL};
    return res;
  }
  ////DUMP("find_helper(%p, %d, %d)\n", tree, *(int*)node->data, *(int*)data);
  int cmp_result = (*(tree->cmp))(data, node->data);
  if (cmp_result == 0) {
    BTreeIterator res = {tree, node};
    return res;
  }
  else if (cmp_result < 0)
    return find_helper(tree, node->left, data);
  else
    return find_helper(tree, node->right, data);
}

BTreeIterator btree_find(BTree *tree, void *data)
{
  ////DUMP("btree_find(%d)\n", *(int*)data);
  return find_helper(tree, tree->root, data);
}

bool btree_member(BTree *tree, void *data)
{
  return btree_find(tree, data).node != NULL;
}

static Node* down_to_leftmost_child(Node *t)
{
  if (t == NULL)
    return NULL;
  if (t->left)
    return down_to_leftmost_child(t->left);
  else
    return t;
}

static Node* down_to_rightmost_child(Node *t)
{
  if (t == NULL)
    return NULL;
  if (t->right)
    return down_to_rightmost_child(t->right);
  else
    return t;
}

void btree_remove(BTreeIterator it)
{
  ////DUMP("btree_remove(%d)\n", *(int*)it->data);
  BTree *tree = it.tree;
  Node *node = it.node;
  if (node == NULL)
    return;
  if (node->left == NULL && node->right == NULL) { // Node has no children
    if (node->parent == NULL) { // We're removing the root
      tree->root = NULL;
      tree->begin = NULL;
      tree->end = NULL;
    } else { 
      if (node->parent->left == node)
        node->parent->left = NULL;
      else
        node->parent->right = NULL;
    }
  } else if (node->left != NULL && node->right != NULL) { // Node has both children
    BTreeIterator next = btree_next(it);
    node->data = next.node->data;
    return btree_remove(next);
  } else if (node->left != NULL) {
    node->left->parent = node->parent;
    if (node->parent != NULL) {
      if (node->parent->left == node) 
        node->parent->left = node->left;
     else
        node->parent->right = node->left;
    } else {
      tree->root = node->left;
    }
  } else {
    node->right->parent = node->parent;
    if (node->parent != NULL) {
      if (node->parent->left == node) 
        node->parent->left = node->right;
     else
        node->parent->right = node->right;
    } else {
      tree->root = node->right;
    }
  }
  free(node);
  tree->begin = down_to_leftmost_child(tree->root);
  tree->end = down_to_rightmost_child(tree->root);
}

BTreeIterator btree_begin(BTree *tree)
{
  BTreeIterator res = {tree, down_to_leftmost_child(tree->root)};
  return res;
}

static Node* up_to_first_right(Node *t)
{
  if (t == NULL || t->parent == NULL)
    return NULL;
  if (t->parent->right == t)
    return up_to_first_right(t->parent);
  return t->parent;
}

BTreeIterator btree_next(BTreeIterator it)
{
  ////DUMP("btree_next()\n");
  Node *node = it.node;
  if (node->right != NULL) {
    BTreeIterator res = {it.tree, down_to_leftmost_child(node->right)};
    return res;
  }
  if (node->parent == NULL) {
    BTreeIterator res = {it.tree, NULL};
    return res;
  }
  if (node->parent->left == node) {
    BTreeIterator res = {it.tree, node->parent};
    return res;
  }
  BTreeIterator res = {it.tree, up_to_first_right(node->parent)};
  return res;
}

bool btree_has_more(BTreeIterator it)
{
  return btree_next(it).node != NULL;
}

static void destroy_helper(Node *node)
{
  if (node != NULL) {
    destroy_helper(node->left);
    destroy_helper(node->right);
    free(node);
  }
}

void btree_destroy(BTree *tree)
{
  ////DUMP("btree_destroy()\n");
  destroy_helper(tree->root);
  free(tree);
}

static int max(int a, int b)
{
  if (a > b)
    return a;
  return b;
}

static int btree_height_helper(Node *n)
{
  if (n == NULL)
    return 0;
  return 1 + max(btree_height_helper(n->left), btree_height_helper(n->right));
}

int btree_height(BTree *tree)
{
  return btree_height_helper(tree->root);
}

static void dump_dot_helper(Node *n, char* (*dot_node_attributes)(Node *n))
{
  if (n == NULL)
    return;
  if (n->left != NULL) {
    printf("\"%p\" -> \"%p\"\n", n, n->left);
  }
  if (n->right != NULL) {
    printf("\"%p\" -> \"%p\"\n", n, n->right);
  }
  if (n->color == BTREE_BLACK) {
    printf("\"%p\" %s\n", n, dot_node_attributes(n));
  } else {
    printf("\"%p\" %s\n", n, dot_node_attributes(n));
  }
  dump_dot_helper(n->left, dot_node_attributes);
  dump_dot_helper(n->right, dot_node_attributes);
}

void btree_dump_dot(BTree *tree, char* (*dot_node_attributes)(Node *n))
{
  printf("digraph {\n");
  dump_dot_helper(tree->root, dot_node_attributes);
  printf("}");
}

void dump_helper(Node *n, int indent, char* (*dump_node)(Node *n))
{
  if (n != NULL)
    dump_helper(n->right, indent + 4, dump_node);
  for (int i = 0; i < indent; ++i)
    putchar(' ');
  printf("%s\n", dump_node(n));
  if (n != NULL)
    dump_helper(n->left, indent + 4, dump_node);
}

void btree_dump(BTree *tree, char* (*dump_node)(Node *n))
{
  dump_helper(tree->root, 0, dump_node);
}
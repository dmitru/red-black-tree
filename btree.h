
#ifndef BTREE
#define BTREE

#include <stdbool.h>

#include "stdlib.h"

enum NodeColor {BTREE_RED, BTREE_BLACK};

struct Node {
  struct Node *left;
  struct Node *right;
  struct Node *parent;
  void *data;
  NodeColor color;
};

struct BTree {
  struct Node *root;
  int (*cmp)(void *, void *);
};

struct BTreeIterator {
  struct BTree *tree;  
  struct Node *node;
};

typedef struct BTree BTree;
typedef struct Node Node;

/** 
  * Creates a new tree, using 'cmp' as a compare function.
  * Compare function must return a negative int if first argument is the lesser,
  * zero if the two arguments are equal and a positive int otherwise.
  **/
BTree* btree_create(int (*cmp) (void *, void *));

bool btree_isempty(BTree *tree);

/**
  * Inserts a pointer 'data' into the tree. The client is responsible
  * not to modify inserted objects so that tree's structure will be preserved
  **/
bool btree_insert(BTree *tree, void *data);

BTreeIterator btree_find(BTree *tree, void *data);

bool btree_member(BTree *tree, void *data);

void btree_remove(BTreeIterator it);

BTreeIterator btree_begin(BTree *tree);

BTreeIterator btree_next(BTreeIterator it);

bool btree_has_more(BTreeIterator it);

void btree_destroy(BTree *tree);

int btree_height(BTree *tree);

void btree_dump(BTree *tree, char* (*dump_node)(Node *n));

void btree_dump_dot(BTree *tree, char* (*dot_node_attributes)(Node *));

#endif  // BTREE


#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "btree.h"

#define DUMP(format, ...) (fprintf(stdout, format, ##__VA_ARGS__))

int int_compare (void *va, void *vb)
{
  int *a = (int*)va;
  int *b = (int*)vb;
  return *a - *b;
}

char* node_attrs(Node *n)
{
  const size_t buf_sz = 1024;
  static char attr_str[buf_sz];
  snprintf(attr_str, buf_sz, "[label=\"%d\", style=filled, color=%s]",
    *(int*)(n->data), (n->color == BTREE_RED)? "red" : "gray");
  return attr_str;
}

static char* node_dump(Node *n)
{
  const size_t buf_sz = 1024;
  static char str[buf_sz];
  if (n != NULL)
    snprintf(str, buf_sz, "%3d%c", *(int*)(n->data), (n->color == BTREE_RED)? 'r' : 'b');
  else
    snprintf(str, buf_sz, "NILb");
  return str;
}

int main(int argc, char **argv)
{
  BTree *tree = btree_create(int_compare);
  int n = atoi(argv[1]);
  int a[n];
  //int a[] = {1, 3, 2, 5, -1, 19, 15, 45, 9, 6, -4};
  n = sizeof(a) / sizeof(a[0]);
  //n = 3;
  for (int i = 0; i < n; ++i) {
    //a[i] = i + 1;
    a[i] = rand() % 1000;
    btree_insert(tree, (void*)&a[i]);
  }
  for (int i = 0; i < n / 2; ++i) {
    btree_remove(btree_find(tree, &a[i]));
  }
  int expected_height = 2 * ceil(log(n + 1) / log(2)) + 1;
  btree_dump_dot(tree, node_attrs);
  btree_destroy(tree);
}
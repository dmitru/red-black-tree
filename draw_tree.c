

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

int main(int argc, char **argv)
{
  BTree *tree = btree_create(int_compare);
  int n = atoi(argv[1]);
  int a[n];
  //int a[] = {3, 1, 2, 5, 3, 19, 15, 45, 9, 6, 9};
  //int n = sizeof(a) / sizeof(a[0]);
  //n = 3;
  for (int i = 0; i < n; ++i) {
    a[i] = i;
    btree_insert(tree, (void*)&a[i]);
  }
  int expected_height = 2 * ceil(log(n + 1) / log(2)) + 1;
  btree_dump(tree);
  btree_destroy(tree);
}
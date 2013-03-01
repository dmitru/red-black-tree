
/**
  * This file shows how to use BTree implementation of
  * red-black trees.
  *
  * Author: dmitriy.borodiy@gmail.com
**/

#include <stdlib.h>

#include "btree.h"
#include "stdlib.h"

int cmp (void *va, void *vb)
{
  int *a = (int*)va;
  int *b = (int*)vb;
  return *a - *b;
}

int main (int argc, char **argv)
{
  int n = 10;
  if (argc >= 2) {
    n = atoi(argv[1]);
  }

  BTree t = btree_create(cmp);
  if (btree_isempty(t)) 
    puts("A brand-new tree is empty!");
  else
    puts("Something went wrong :(");

  // Populate the tree with ints 
  for (int i = 0; i < n; ++i) {
    int *p = malloc(sizeof(int));
    *p = i;
    btree_insert(t, p);
  }

  BTree::Iterator it = btree_begin(t);
  while (it) {
    printf("%d ", *(it.data));
    it = btree_next(it);
  }

  // Look for members
  for (int i = 0; i < 2 * n; ++i) {
    if (btree_member(t, i))
      printf("%d is in the tree.\n", i);
    else
      printf("%d is in NOT the tree.\n", i);
  }

  // Remove all even elements
  for (int i = 0; i < n; i += 2) {
    BTree::Iterator it = btree_find(t, i);
    btree_remove(t, it);
  }

  // Free ints memory 
  it = btree_begin(t);
  while (btree_has_more(it)) {
    free(it.data);
    it = btree_next(it);
  }

  btree_destroy(t);
}
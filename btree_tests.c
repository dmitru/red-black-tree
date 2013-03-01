
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <algorithm>

#include "btree.h"

#include "gtest/gtest.h"

#define DUMP(format, ...) (fprintf(stderr, format, ##__VA_ARGS__))

int int_compare (void *va, void *vb)
{
  int *a = (int*)va;
  int *b = (int*)vb;
  return *a - *b;
}

TEST(BalancedTreeTests, CreateDestroyTest) {
  BTree *tree = btree_create(int_compare);
  EXPECT_TRUE(btree_isempty(tree));
  btree_destroy(tree);
}

TEST(BalancedTreeTests, InsertRemoveMemberTest1) {
  BTree *tree = btree_create(int_compare);
  const int n = 100;
  int a[n];
  for (int i = 0; i < n; i += 2)  {
    a[i] = i;
    btree_insert(tree, (void*)&a[i]);
  }
  for (int i = n - 1; i > 0; i -= 2)  {
    a[i] = i;
    btree_insert(tree, (void*)&a[i]);
  }
  int x = 42;
  EXPECT_TRUE(btree_member(tree, (void*)&x));
  x = -1;
  EXPECT_FALSE(btree_member(tree, (void*)&x));
  x = 50;
  btree_remove(btree_find(tree, (void*)&x));
  for (int i = 0; i < n; i += 2) {
    BTreeIterator it = btree_find(tree, (void*)&i);
    if (it.node != NULL) {
      btree_remove(it);
    }
  }
  for (int i = n - 1; i >= 0; --i) {
    BTreeIterator it = btree_find(tree, (void*)&i);
    if (it.node != NULL)
      btree_remove(it);
  }
  EXPECT_TRUE(btree_isempty(tree));
  btree_destroy(tree);
}

TEST(BalancedTreeTests, InsertRemoveMemberTest2) {
  BTree *tree = btree_create(int_compare);
  int x = 1;
  btree_insert(tree, (void*)&x);
  EXPECT_TRUE(btree_member(tree, (void*)&x));
  EXPECT_FALSE(btree_isempty(tree));
  btree_remove(btree_find(tree, (void*)&x));
  EXPECT_FALSE(btree_member(tree, (void*)&x));
  EXPECT_TRUE(btree_isempty(tree));
  btree_destroy(tree);
}

TEST(BalancedTreeTests, IteratorTest1) {
  srand(time(NULL));
  BTree *tree = btree_create(int_compare);
  int a[] = {1, 3, 2, 5, -1, 19, 15, 45, 9, 6, -4};
  int a_sorted[] = {-4, -1, 1, 2, 3, 5, 6, 9, 15, 19, 45};
  const int n = sizeof(a) / sizeof(a[0]);
  for (int i = 0; i < n; ++i) {
    btree_insert(tree, (void*)&a[i]);
  }
  BTreeIterator it = btree_begin(tree);
  int i = 0;
  EXPECT_TRUE(it.node != NULL);
  while (btree_has_more(it)) {
    EXPECT_EQ(*(int*)(it.node->data), a_sorted[i]);
    it = btree_next(it);
    i += 1;
  }
  btree_destroy(tree);
}

TEST(BalancedTreeTests, IteratorTest2) {
  BTree *tree = btree_create(int_compare);
  int a = 1;
  btree_insert(tree, (void*)&a);
  int b = 2;
  btree_insert(tree, (void*)&b);
  int c = 3;
  btree_insert(tree, (void*)&c);
  BTreeIterator it = btree_find(tree, (void*)&c);
  EXPECT_EQ(btree_next(it).node, (Node*)NULL);
  btree_remove(it);
  btree_remove(btree_find(tree, &b));
  it = btree_find(tree, (void*)&a);
  EXPECT_EQ(btree_find(tree, (void*)&b).node, btree_next(it).node);
  btree_remove(it);
  btree_destroy(tree);
}

TEST(BalancedTreeTests, TreeHeightTest) {
  srand(time(NULL));
  BTree *tree = btree_create(int_compare);
  const int n = 100;
  int a[n];
  for (int i = 0; i < n; ++i) {
    a[i] = i;
    btree_insert(tree, (void*)&a[i]);
  }
  int expected_height = 2 * ceil(log(n + 1) / log(2)) + 1;
  DUMP("%d, %d\n", btree_height(tree), expected_height);
  EXPECT_TRUE(btree_height(tree) < expected_height);
  btree_destroy(tree);
}


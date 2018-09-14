#include <stdio.h>
#include "centroid_tree.h"

#define mu_assert(message, test) do { if (!(test)) return message; } while (0)
#define mu_run_test(test) do { char *message = test(); tests_run++; \
                                if (message) return message; } while (0)
static int tests_run;

typedef enum {
  END,
  INSERT,
  UPDATE,
  NUM_AFTER,
  NUM_BEFORE,
  FLOOR,
  CEIL,
  SUM_FLOOR,
  SUM_CEIL,
  NEXT,
  MIN,
  MAX,
  LEN,
  DELETE,
} test_op_type_t;

typedef struct {
  test_op_type_t op;
  int n;
  bool check_val;
  ct_centroid_t centroid;
} test_op_t;

static char *assert_eq(ct_centroid_t expected, ct_centroid_t got) {
  printf("\nhi\n\n");
  mu_assert("values match", expected.val == got.val);
  mu_assert("counts match", expected.count == got.count);
  return NULL;
}

static test_op_t insert(ct_centroid_t v) {
  return (test_op_t) { INSERT, 0, false, v };
}

static test_op_t min_is(ct_centroid_t v) {
  return (test_op_t) { MIN, 0, true, v };
}

static test_op_t max_is(ct_centroid_t v) {
  return (test_op_t) { MAX, 0, true, v };
}

static test_op_t num_after(int n) {
  return (test_op_t) { NUM_AFTER, n, false, { 0, 0 } };
}

static test_op_t num_before(int n) {
  return (test_op_t) { NUM_BEFORE, n, false, { 0, 0 } };
}

static test_op_t update(ct_centroid_t new) {
  return (test_op_t) { UPDATE, 0, false, new };
}

static test_op_t len_is(int n) {
  return (test_op_t) { LEN, n, false, {} };
}

static char *run_ops(test_op_t ops[]) {
  ct_centroid_t *cur = NULL;
  ct_tree_t *t = ct_new();
  for (int i = 0; true; i++) {
    test_op_t c = ops[i];
    switch (c.op) {
    case END:
      ct_free(t);
      return NULL;
    case INSERT:
      cur = ct_create(t, c.centroid);
      break;
    case LEN:
      mu_assert("len is", ct_len(t) == c.n);
      break;
    case UPDATE:
      ct_update(cur, c.centroid);
      break;
    case NUM_AFTER:
      for (int j = c.n; j >= 0; j--) {
        cur = ct_next(cur);
        mu_assert("fewer after than expected", cur != NULL || j == 0);
      }
      mu_assert("more after than expected", cur == NULL);
      break;
    case NUM_BEFORE:
      for (int j = c.n; j >= 0; j--) {
        cur = ct_prev(cur);
        mu_assert("fewer before than expected", cur != NULL || j == 0);
      }
      mu_assert("more before than expected", cur == NULL);
      break;
    case NEXT:
      cur = ct_next(cur);
      break;
    case MIN:
      cur = ct_min(t);
      break;
    case MAX:
      cur = ct_max(t);
      break;
    case DELETE:
      ct_delete(cur);
      break;
    case FLOOR:
      break;
    case CEIL:
      break;
    case SUM_FLOOR:
      break;
    case SUM_CEIL:
      break;
    }
    if (c.check_val) {
      assert_eq(c.centroid, *cur);
    }
    mu_assert("is bst", ct_is_bst(t));
    dump_tree(*t);
  }
  return NULL;
}

static char *table_test() {
  ct_centroid_t data[] = {
    { 1,     1   },
    { 1.1,   1.1 },
    { 1.3,   0.9 },
    { 2,   238   },
    { 4,     1   },
    {-1,     1   },
  };
  test_op_t ops[] = {
    insert(data[0]),
    len_is(1),
    insert(data[1]),
    len_is(2),
    insert(data[2]),
    len_is(3),
    min_is(data[0]),
    num_after(2),
    max_is(data[2]),
    num_after(0),
    max_is(data[2]),
    num_before(2),
    min_is(data[0]),
    update(data[3]),
    len_is(3),
    min_is(data[1]),
    num_after(2),
    max_is(data[3]),
    num_before(2),
    insert(data[4]),
    len_is(4),
    max_is(data[4]),
    num_after(0),
    max_is(data[4]),
    num_before(3),
    insert(data[5]),
    len_is(5),
    min_is(data[5]),
    num_before(0),
    min_is(data[5]),
    num_after(4),
    min_is(data[5]),
    update(data[0]),
    min_is(data[0]),    
    {},
  };
  return run_ops(ops);
};


static char *table_test2() {
  ct_centroid_t d[] = {
    { 1, 1 },
    { 2, 1 },
    { 3, 1 },
    { 4, 1 },
    { 5, 1 },
    { 6, 1 },
    { 7, 1 },
    { 8, 1 },
    { 9, 1 },
  };
  test_op_t ops[] = {
    insert(d[0]),
    insert(d[1]),
    insert(d[2]),
    insert(d[3]),
    insert(d[4]),
    insert(d[5]),
    insert(d[6]),
    insert(d[7]),
    insert(d[8]),
    insert(d[9]),
    min_is(d[0]),
    num_after(9),
    max_is(d[9]),
    num_before(9),
    len_is(10),
    {},
  };
  return run_ops(ops);
};


static char *all_tests() {
  // mu_run_test(table_test);
  mu_run_test(table_test2);
  return NULL;
}

int main(void) {
  char *result = all_tests();
  if (result != NULL) {
    printf("%s\n", result);
  }
  else {
    printf("ALL TESTS PASSED\n");
  }
  printf("Tests run: %d\n", tests_run);
  return result != NULL;
}

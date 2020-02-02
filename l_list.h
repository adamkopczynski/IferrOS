#ifndef __LINKED_LIST_H__
#define __LINKED_LIST_H__

typedef struct node_t {
  void *data;
  struct node_t *prev;
  struct node_t *next;
} node_t;

typedef struct {
  int size;
  node_t *head;
  node_t *tail;
} ll_t;

/*  create new list */
ll_t *ll_init();

/*  get/find functions */
node_t *ll_get(ll_t *, int);
node_t *ll_first(ll_t *);
node_t *ll_last(ll_t *);

/*  add functions */
int ll_add(ll_t *, void *, int);
void *ll_set(ll_t *, void *, int);
int ll_push_front(ll_t *, void *);
int ll_push_back(ll_t *, void *);

/*  remove functions */
void *ll_remove(ll_t *, int);
void *ll_pop_front(ll_t *);
void *ll_pop_back(ll_t *);

/*  destructive functions */
void ll_clear(ll_t *);
void ll_destroy(ll_t *);

#endif
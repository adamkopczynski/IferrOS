#include "l_list.h"
#include "heap.h"
#include <stdint.h>

/*  create new list */
ll_t *ll_init(){

    ll_t *list = (ll_t*)kmalloc(sizeof(ll_t));

    list->size = 0;
    list->head = NULL;
    list->tail = NULL;

    return list;
}

/*  get/find functions */
void *ll_get(ll_t *l, int pos){
    
    node_t* current = l->head;

	for(int i=0; i<pos; i++)
		current = current->next;

	return current->data;
}

void *ll_front(ll_t *l){

    if(!l)
        return NULL;

    return l->head->data;
}

void *ll_back(ll_t *l){

     if(!l)
        return NULL;

    return l->tail->data;
}

/*  add functions */
int ll_push_front(ll_t *list, void *data){

    if(!list)
        return 1;

    node_t *node = (struct node_t*)kmalloc(sizeof(node_t));

    if(!node)
        return 2;

    node->data = data;
    node->next = list->head;
    node->prev = NULL;

    if(list->head)
        list->head->prev = node;

    list->head = node;

    if(!list->tail)
        list->tail = node;

    return 0;
}

int ll_push_back(ll_t *list, void *data){

    if(!list)
        return 1;

    node_t *node = (struct node_t*)kmalloc(sizeof(node_t));

    if(!node)
        return 2;

    node->data = data;
    node->prev = list->tail;
    node->next = NULL;

    if(list->tail)
        list->tail->next = node;
    
    list->tail = node;

    if(!list->head)
        list->head = node;

    return 0;
}

/*  remove functions */
void *ll_remove(ll_t * l, int pos){

    if(pos == 0){
        return ll_pop_front(l);
    }

    node_t* current = l->head;

	for(int i=0; i<pos; i++)
		current = current->next;

    if(!current->next){
        return ll_pop_back(l);
    }

    if(current->prev){
        current->prev->next = current->next;
    }
    
    if(current->next){
        current->next->prev = current->prev;
    }

    kfree(current);

	return current->data;
}

void *ll_pop_front(ll_t *l){

    if(!l){
        return NULL;
    }

    node_t *node = l->head;

    l->head = node->next;
    
    if(l->head){
        l->head->prev = NULL;
    }

    //if in array id only one element
    if(!node->next)
        l->tail = NULL;

    void *data = node->data;
    kfree(node);
    
    return data;
}

void *ll_pop_back(ll_t *l){

    if(!l){
        return NULL;
    }

    node_t *node = l->tail;

    l->tail = node->prev;
    
    if(l->tail){
        l->tail->next = NULL;
    }

    //if in array id only one element
    if(!node->prev)
        l->head = NULL;

    void *data = node->data;
    kfree(node);
    
    return data;

}

/*  destructive functions */
void ll_clear(ll_t *l){

    if(!l)
        return;

    while(ll_pop_back(l) != NULL);
}

void ll_destroy(ll_t *l){

    ll_clear(l);
    kfree(l);
}
#ifndef __LIST_H__
#define __LIST_H__

#define UNI_LIST_H(_NAME, _FIELD)\
struct node_##_NAME##_t{\
	_FIELD data;\
	struct node_##_NAME##_t* next;\
	struct node_##_NAME##_t* prev;\
};\
\
struct list_##_NAME##_t{\
	struct node_##_NAME##_t* head;\
	struct node_##_NAME##_t* tail;\
    uint32_t size;\
};\
\
struct list_##_NAME##_t *list_##_NAME##_create(void);\
void list_##_NAME##_push_back(struct list_##_NAME##_t *l, _FIELD data);\
void list_##_NAME##_push_front(struct list_##_NAME##_t *l, _FIELD data);\
_FIELD list_##_NAME##_pop_front(struct list_##_NAME##_t *l);\
_FIELD list_##_NAME##_pop_back(struct list_##_NAME##_t *l);\
_FIELD list_##_NAME##_front(struct list_##_NAME##_t *l);\
_FIELD list_##_NAME##_back(struct list_##_NAME##_t *l);\
_FIELD list_##_NAME##_remove_node(struct list_##_NAME##_t *l, struct node_##_NAME##_t *node);\
struct node_##_NAME##_t* list_##_NAME##_get_node_at(struct list_##_NAME##_t *l, uint32_t pos);

#define UNI_LIST_C(_NAME, _FIELD)\
struct list_##_NAME##_t *list_##_NAME##_create(void)\
{\
    struct list_##_NAME##_t *list = (struct list_##_NAME##_t*)kmalloc(sizeof(struct list_##_NAME##_t));\
    list->size = 0;\
    list->head = NULL;\
    list->tail = NULL;\
    return list;\
}\
\
void list_##_NAME##_destroy(struct list_##_NAME##_t *l)\
{\
    struct node_##_NAME##_t *current = l->head;\
    while(current!=NULL)\
    {\
        struct node_##_NAME##_t *delete = current;\
        current = current->next;\
        kfree(delete);\
    }\
\
    kfree(l);\
}\
\
void list_##_NAME##_push_back(struct list_##_NAME##_t *l, _FIELD data)\
{\
	struct node_##_NAME##_t *node = (struct node_##_NAME##_t*)kmalloc(sizeof(struct node_##_NAME##_t));\
\
	node->data = data;\
	node->next = NULL;\
	node->prev = l->tail;\
\
	if (l->tail == NULL)\
	{\
		l->head = node;\
		l->tail = node;\
	}\
	else\
	{\
		l->tail->next = node;\
		l->tail = node;\
	}\
\
    l->size++;\
}\
\
void list_##_NAME##_push_front(struct list_##_NAME##_t *l, _FIELD data)\
{\
	struct node_##_NAME##_t *node = (struct node_##_NAME##_t*)kmalloc(sizeof(struct node_##_NAME##_t));\
\
	node->data = data;\
	node->next = l->head;\
	node->prev = NULL;\
\
	if (l->head == NULL)\
	{\
		l->head = node;\
		l->tail = node;\
	}\
	else\
	{\
		l->head->prev = node;\
		l->head = node;\
	}\
\
    l->size++;\
}\
\
_FIELD list_##_NAME##_pop_front(struct list_##_NAME##_t *l)\
{\
	struct node_##_NAME##_t* poped = l->head;\
	_FIELD data = poped->data;\
\
	if (l->head->next != NULL)\
	{\
		l->head = l->head->next;\
		l->head->prev = NULL;\
	}\
	else\
	{\
		l->head = NULL;\
		l->tail = NULL;\
	}\
\
	l->size--;\
\
	kfree(poped);\
	return data;\
}\
\
_FIELD list_##_NAME##_pop_back(struct list_##_NAME##_t *l)\
{\
	struct node_##_NAME##_t* poped = l->tail;\
	_FIELD data = poped->data;\
\
	if (l->tail->prev != NULL)\
	{\
		l->tail = l->tail->prev;\
		l->tail->next = NULL;\
	}\
	else\
	{\
		l->head = NULL;\
		l->tail = NULL;\
	}\
\
	l->size--;\
\
	kfree(poped);\
	return data;\
}\
\
_FIELD list_##_NAME##_front(struct list_##_NAME##_t *l)\
{\
	_FIELD data = l->head->data;\
	return data;\
}\
\
_FIELD list_##_NAME##_back(struct list_##_NAME##_t *l)\
{\
	_FIELD data = l->tail->data;\
	return data;\
}\
\
void list_##_NAME##_insert(struct list_##_NAME##_t *l, _FIELD data, uint32_t pos)\
{\
	if(pos<=0) list_##_NAME##_push_front(l, data);\
	else if(pos>=l->size) list_##_NAME##_push_back(l, data);\
	else\
	{\
		struct node_##_NAME##_t *current_node = l->head;\
		for(uint32_t i=0; i<pos; i++)\
			current_node = current_node->next;\
\
		struct node_##_NAME##_t *node = (struct node_##_NAME##_t*)kmalloc(sizeof(struct node_##_NAME##_t));\
		node->data = data;\
		node->next = current_node;\
		node->prev = current_node->prev;\
\
		current_node->prev->next = node;\
		current_node->prev = node;\
\
		l->size++;\
	}\
}\
\
_FIELD list_##_NAME##_remove_node(struct list_##_NAME##_t *l, struct node_##_NAME##_t *node)\
{\
	if(l->head == node)\
		l->head = node->next;\
	else\
		node->prev->next = node->next;\
\
	if(l->tail == node)\
		l->tail = node->prev;\
	else \
		node->next->prev = node->prev;\
\
	l->size--;\
\
	_FIELD thread = node->data;\
	kfree(node);\
\
	return thread;\
}\
\
struct node_##_NAME##_t* list_##_NAME##_get_node_at(struct list_##_NAME##_t *l, uint32_t pos)\
{\
	struct node_##_NAME##_t* current = l->head;\
\
	for(uint32_t i=0; i<pos; i++)\
		current = current->next;\
\
	return current;\
}


#endif
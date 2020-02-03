#include <stddef.h>
#include <stdint.h>
#include "threads.h"
#include "heap.h"
#include "isr.h"
#include "pic.h"
#include "clock.h"
#include "sys.h"
#include "l_list.h"
#include "libc/stdio.h"

//All threads lists
ll_t *waiting_threads;         
ll_t *active_threads;          
ll_t *terminated_threads;

//All threads ids
thread_t *threads_ids[THREAD_MAX_COUNT] = {0};

//Time quants to switch
uint32_t time_quants = 0;

extern void sys_cli();
extern void sys_sti();
extern void pause();

static void scheduler(void);
static void push_thread32(thread_t *t, uint32_t val);
static void push_thread8(thread_t *t, uint8_t val);
static void on_thread_close(void);
static void find_next_thread(thread_state_t state);
static node_t *find_node_with_id(ll_t *l, uint32_t id, uint32_t* list_id);
static uint32_t find_thread_id(void);

void init_threads(void){
    waiting_threads = ll_init();
    active_threads = ll_init();
    terminated_threads = ll_init();


    // Kernel thread
    create_kernel_thread();
    register_interrupt_handler(IRQ0, scheduler);
    
}

uint32_t create_thread(uint32_t task_addr){

    thread_t *new_thread = (thread_t*)kmalloc(sizeof(thread_t));
    new_thread->state = THREAD_WAITING_FOR_START;
    new_thread->stack = (uint32_t)kmalloc(THREAD_STACK_SIZE);
    new_thread->esp = new_thread->stack + THREAD_STACK_SIZE;
    new_thread->task = task_addr;
    new_thread->priority = THREAD_TIME_QUANT;

    new_thread->thread_id = find_thread_id();

    //Add new thread id to all threads list
    threads_ids[new_thread->thread_id] = new_thread;

    // on_thread_close will be execute when new_thread end its running
    push_thread32(new_thread, (uint32_t)on_thread_close);

    // Add thread to waiting list
    ll_push_back(waiting_threads, new_thread);

    return new_thread->thread_id;
}

//Main kernel thread
uint32_t create_kernel_thread(void){

    thread_t *new_thread = (thread_t*)kmalloc(sizeof(thread_t));
    new_thread->state = THREAD_RUNNING;
    new_thread->priority = THREAD_TIME_QUANT;
    new_thread->stack = 0; //kernel has own stack, don't need to allocate new one
    new_thread->esp = 0;
    new_thread->task = 0;

    new_thread->thread_id = find_thread_id();
    threads_ids[new_thread->thread_id] = new_thread;

    ll_push_back(active_threads, new_thread);

    return new_thread->thread_id;
}

void destroy_thread(uint32_t id){

    sys_cli();

    thread_t *thread = threads_ids[id];

    // If thread has waiting status
    if(thread->state == THREAD_WAITING_FOR_START){

        uint32_t id_at_list= 0;
        node_t *node = find_node_with_id(waiting_threads, id, &id_at_list);
        ll_remove(waiting_threads, id_at_list);

    }

    // If thread is running, terminate it before destroying
    else if(thread->state == THREAD_READY || thread->state == THREAD_RUNNING){

        terminate_thread(id);
        uint32_t id_at_list= 0;
        node_t *node = find_node_with_id(terminated_threads, id, &id_at_list);
        ll_remove(terminated_threads, id_at_list);

    }

    // If thread is stopped
    else if(thread->state == THREAD_END){

        uint32_t id_at_list= 0;
        node_t *node = find_node_with_id(terminated_threads, id, &id_at_list);
        ll_remove(terminated_threads, id_at_list);
    }

    // Free memory
    kfree((uint32_t*)thread->stack);
    kfree(thread);

    threads_ids[id] = NULL;

    sys_sti();
}

void start_thread(uint32_t id){

    sys_cli();

    uint32_t id_at_list= 0;
    node_t *node = find_node_with_id(waiting_threads, id, &id_at_list);
    thread_t *activated = ll_remove(waiting_threads, id_at_list);
    ll_push_back(active_threads, activated);

    sys_sti();
}

void terminate_thread(uint32_t id){

    sys_cli();

    thread_t *current = get_current_thread();

    if(current->thread_id == id)
        find_next_thread(THREAD_END);
    else{
        uint32_t id_at_list= 0;
        node_t *node = find_node_with_id(active_threads, id, &id_at_list);
        thread_t *stopped = ll_remove(active_threads, id_at_list);
        ll_push_back(terminated_threads, stopped);
    
    }

    sys_sti();
}

void join_thread(uint32_t id){

    while(threads_ids[id]->state != THREAD_END) continue;
}

//Round Robin threads time scheduler
static void scheduler(void){

    sys_cli();
    PIC_sendEOI(IRQ0);

    if(time_quants != 0)
        time_quants--;

    if(active_threads->size > 1 && time_quants == 0){

        find_next_thread(THREAD_READY);
    }

    sys_sti();
}

//Switch to next thread
static void find_next_thread(thread_state_t state){

    thread_t *current = get_current_thread();
    current->state = state;

    if(state == THREAD_END){
        
        thread_t *terminated = ll_pop_front(active_threads);
        ll_push_back(terminated_threads, terminated);
    }

    thread_t *next = ll_pop_front(active_threads);
    ll_push_front(active_threads, next);

    // Set new time quants for next thread
    time_quants = next->priority;

    if(next->state == THREAD_WAITING_FOR_START){

        next->state = THREAD_RUNNING;
        switch_stacks_and_jump((uint32_t)current, (uint32_t)next);
    }
    else if(next->state == THREAD_READY){

        next->state = THREAD_RUNNING;
        switch_stacks((uint32_t)current, (uint32_t)next);
    }
}

//Run when thread is terminating
static void on_thread_close(void){

    find_next_thread(THREAD_END);
}

static uint32_t find_thread_id(void){

    static uint32_t last_id = 0;

    for(uint32_t i=last_id; i<last_id+THREAD_MAX_COUNT; i++){

        uint32_t index = i % THREAD_MAX_COUNT;

        if(threads_ids[index] == NULL){
            last_id = index;
            return index;
        }
    }

    kernel_panic("No Free Thread ID found");
    return 0;
}

static node_t *find_node_with_id(ll_t *l, uint32_t id, uint32_t *id_at_list){

    node_t *current = l->head;
    *id_at_list = 0;

    while(current != NULL){

        thread_t* current_thread = (thread_t*)(current->data);
        if(current_thread->thread_id == id) return current;
        current = current->next;
        *id_at_list++;
    }

    kernel_panic("Thread with given id didn't found");
    return 0;
}

thread_t *get_current_thread(void){

    return (thread_t*)ll_front(active_threads);
}

//Push 32 bits to thread stack
static void push_thread32(thread_t *t, uint32_t val){

    //Divide into bytes
    uint8_t b1 = (val & 0x000000FF) >> 0;
    uint8_t b2 = (val & 0x0000FF00) >> 8;
    uint8_t b3 = (val & 0x00FF0000) >> 16;
    uint8_t b4 = (val & 0xFF000000) >> 24;

    push_thread8(t, b4);
    push_thread8(t, b3);
    push_thread8(t, b2);
    push_thread8(t, b1);
}

// Push 1 byte (8 bits) to thread stack
static void push_thread8(thread_t *t, uint8_t val){

    t->esp--;
    uint8_t *addr = (uint8_t*)t->esp;
    *addr = val;

}

// Debug display thread stack content
void debug_display_stack(uint32_t id){

    thread_t *t = threads_ids[id];

    printf("\n");
    printf("ADDRESS       VALUE\n");
    printf("----------------------\n");
    for(uint32_t start=t->esp+40; start>=t->esp; start-=4){

        uint32_t *ptr = (uint32_t*)start;
        printf("%x  ->  %x", start, *ptr);
        if(start==t->esp) printf("   <- ESP \n");
        else printf("\n");
    }

    pause();
}
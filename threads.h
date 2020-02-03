#ifndef __THREADS_H__
#define __THREADS_H__

#include <stddef.h>
#include <stdint.h>

//Stack size
#define THREAD_STACK_SIZE 0x4000
#define THREAD_MAX_COUNT 256

//Time quant
#define THREAD_TIME_QUANT 20

extern void switch_stacks_and_jump(uint32_t current_thread, uint32_t next_thread);
extern void switch_stacks(uint32_t current_thread, uint32_t next_thread);

//Threads states
typedef enum thread_state_t {
    THREAD_WAITING_FOR_START,
    THREAD_READY,
    THREAD_RUNNING,
    THREAD_END
} thread_state_t;

typedef struct thread_t {
    uint32_t esp;
    uint32_t stack;
    uint32_t task;
    uint32_t thread_id;
    uint32_t priority;
    uint32_t process_pid;
    enum thread_state_t state;
} thread_t;

void init_threads(void);
uint32_t create_kernel_thread(void);
uint32_t create_thread(uint32_t task_addr);
void destroy_thread(uint32_t id);
void start_thread(uint32_t id);
void terminate_thread(uint32_t id);
void join_thread(uint32_t id);
void debug_display_stack(uint32_t id);
thread_t *get_thread(uint32_t id);
thread_t *get_current_thread(void);
void yield(void);

#endif
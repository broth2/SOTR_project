/*
 * Joao Castanheira, Nuno Fahla, 12/2022
 * Based on teachers examples
 * Zephyr: ?????
 * 
 * x tasks are periodc, the other y are activated via a semaphore. Data communicated via sharem memory 
 *
 * 
 */

#include <zephyr.h>
#include <device.h>
#include "imageProcAlg.c"

/* Size of stack area used by each thread */
#define STACK_SIZE 1024

#define IMGSIDE 128 /* Square image. Side size, in pixels*/

/* Thread scheduling priority */
#define NEAR_DETECT_PRIO 90
#define OBSTACLE_COUNT_PRIO 1
#define OUTPUT_UPDATE_PRIO 90
#define LOCATION_PRIO 40

/* Thread periodicity (in ms)*/
#define SAMP_PERIOD_MS  1000

/* Create thread stack space */
K_THREAD_STACK_DEFINE(near_detect_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(obstacle_count_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(output_update_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(location_stack, STACK_SIZE);
  
/* Create variables for thread data */
struct k_thread near_detect_data;
struct k_thread obstacle_count_data;
struct k_thread output_update_data;
struct k_thread location_data;

uint8_t img[128][128];  /* shared memory */
uint8_t csa_obj=0;  /* object in csa */

/* Create task IDs */
k_tid_t near_detect_tid;
k_tid_t obstacle_count_tid;
k_tid_t output_update_tid;
k_tid_t location_tid;

/* Semaphores for task synch */
struct k_sem near_detect_sem;

/* Thread code prototypes */
void thread_A_code(void *argA, void *argB, void *argC);
void thread_B_code(void *argA, void *argB, void *argC);
void thread_C_code(void *argA, void *argB, void *argC);
void thread_Reset_code(void *argA, void *argB, void *argC);

void main(void) {

    
    /* Welcome message */
    printk("\n\rinit\n\r");
    
    /* Create and init semaphores */
    k_sem_init(&near_detect_sem, 0, 1);
    
    /* Create tasks */
    near_detect_tid = k_thread_create(&near_detect_data, near_detect_stack,
        K_THREAD_STACK_SIZEOF(near_detect_stack), thread_A_code,
        NULL, NULL, NULL, NEAR_DETECT_PRIO, 0, K_NO_WAIT);

    obstacle_count_tid = k_thread_create(&obstacle_count_data, obstacle_count_stack,
        K_THREAD_STACK_SIZEOF(obstacle_count_stack), thread_B_code,
        NULL, NULL, NULL, OBSTACLE_COUNT_PRIO, 0, K_NO_WAIT); 
        

    output_update_tid = k_thread_create(&output_update_data, output_update_stack,
        K_THREAD_STACK_SIZEOF(output_update_stack), thread_C_code,
        NULL, NULL, NULL, OUTPUT_UPDATE_PRIO, 0, K_NO_WAIT); 

    location_tid = k_thread_create(&location_data, location_stack,
        K_THREAD_STACK_SIZEOF(location_stack), thread_Reset_code,
        NULL, NULL, NULL, LOCATION_PRIO, 0, K_NO_WAIT); 

    
    return;

} 

void thread_A_code(void *argA , void *argB, void *argC){
    printk("A) Near Obstacle Detection Thread init\n\r");

    while(1){
        k_sem_take(&near_detect_sem, K_FOREVER);
        printk("thread a instance released at %lld\n\r", k_uptime_get());
        if (csaObjects(img)){
            csa_obj = 1;
        }
    }
    // when new image (sporadic)
    // csaObjects(image)
    // if found : found=1
}

void thread_B_code(void *argA , void *argB, void *argC){
    printk("B) \n\r");
    // when new image or system state changes
    //cada vez que uma tarefa termina

    // apenas imprime as variaveis da memoria partilha
}

void thread_C_code(void *argA , void *argB, void *argC){
    printk("C) \n\r");
    // when new image or system state changes
    //cada vez que uma tarefa termina

    // apenas imprime as variaveis da memoria partilha
}

void thread_Reset_code(void *argA , void *argB, void *argC){
    printk("D) \n\r");
    // when new image or system state changes
    //cada vez que uma tarefa termina

    // apenas imprime as variaveis da memoria partilha
}

// obs count task: run obs_count, lowest possible priority

// orientantion and position task:


//all tasks are sporadic, obs_count task runs only when nothing else needs to be done
// tasks should run and change variables in shared memory(system state's vars)
// output update simulates outputting to another module of the system, only prints instead of sharing to movement hardware for example
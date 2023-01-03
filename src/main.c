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

/* shared mem vars: */

uint8_t img[128][128];  /* received img */
uint8_t csa_obj = 0;    /* object in csa */
int num_objcts = 0;     /* number of objects in image */
float curr_angle;       /* angle at which the robot is pointing */
int16_t curr_pos;       /* current distance from left border in percentage */

/* Create task IDs */
k_tid_t near_detect_tid;
k_tid_t obstacle_count_tid;
k_tid_t output_update_tid;
k_tid_t location_tid;

/* Semaphores for task synch */
struct k_sem near_detect_sem;
struct k_sem obstacle_count_sem;
struct k_sem output_update_sem;
struct k_sem location_sem;

// Thread code prototypes

void near_detect(void *argA, void *argB, void *argC);
void obstacle_count(void *argA, void *argB, void *argC);
void output_update(void *argA, void *argB, void *argC);
void curr_location(void *argA, void *argB, void *argC);

void main(void) {

    
    /* Welcome message */
    printk("\n\rinit\n\r");
    
    /* Create and init semaphores */
    k_sem_init(&near_detect_sem, 0, 1);
    k_sem_init(&obstacle_count_sem, 0, 1);
    k_sem_init(&output_update_sem, 0, 1);
    k_sem_init(&location_sem, 0, 1);
    
    /* Create tasks */
    near_detect_tid = k_thread_create(&near_detect_data, near_detect_stack,
        K_THREAD_STACK_SIZEOF(near_detect_stack), near_detect,
        NULL, NULL, NULL, NEAR_DETECT_PRIO, 0, K_NO_WAIT);

    obstacle_count_tid = k_thread_create(&obstacle_count_data, obstacle_count_stack,
        K_THREAD_STACK_SIZEOF(obstacle_count_stack), obstacle_count,
        NULL, NULL, NULL, OBSTACLE_COUNT_PRIO, 0, K_NO_WAIT); 
        

    output_update_tid = k_thread_create(&output_update_data, output_update_stack,
        K_THREAD_STACK_SIZEOF(output_update_stack), output_update,
        NULL, NULL, NULL, OUTPUT_UPDATE_PRIO, 0, K_NO_WAIT); 

    location_tid = k_thread_create(&location_data, location_stack,
        K_THREAD_STACK_SIZEOF(location_stack), curr_location,
        NULL, NULL, NULL, LOCATION_PRIO, 0, K_NO_WAIT); 

    
    return;

} 

/*  Task that detects if obstacles are in the CSA, when new image is 
    received and updates to shared mem variable csa_obj y/n  */
void near_detect(void *argA , void *argB, void *argC){
    printk("A) Near Obstacle Detection Thread init\n\r");

    while(1){
        k_sem_take(&near_detect_sem, K_FOREVER);
        printk("thread A instance released at %lld\n\r", k_uptime_get());

        if (csaObjects(img)){
            csa_obj = 1;
            // update shared mem through cab
        }
    }
    
    // high PRIO
}

/*  Task that counts the number of objects detected, when new
    image is received and updated shared mem variable  */
void obstacle_count(void *argA , void *argB, void *argC){
    printk("B) Obstacle Count in Image Thread\n\r");

    while(1){
        k_sem_take(&obstacle_count_sem, K_FOREVER);
        printk("thread B instance released at %lld\n\r", k_uptime_get());

        // update shared mem through cab
        num_objcts = obstCount(img);
        printk("%d obstacles detected\n\r", num_objcts);
    }

    // apenas imprime as variaveis da memoria partilhada,
    // deve correr quando as outras tarefas ja correram todas
    // lowest PRIO
}

/*  Task that prints orientation, number of objects in 
    the CSA and total objects detected, everytime a new image
    is received. Gets data from shared mem */
void output_update(void *argA , void *argB, void *argC){
    printk("C) Output Update Thread\n\r");
    char csa_obj_bool;

    while(1){
        k_sem_take(&output_update_sem, K_FOREVER);
        printk("thread C instance released at %lld\n\r", k_uptime_get());

        // get data of shared mem through cab (curr_angle, csa_obj, num_objcts)
        csa_obj_bool = 'N';
        if (csa_obj != 0)   csa_obj_bool = 'Y';

        printk("\n\r--------- SYSTEM STATE ---------\n\r");
        printk("Current Orientation: %f\n\rObsctacles in CSA: %c\n\rTotal Objects Detected: %d\n\r", curr_angle, csa_obj_bool, num_objcts);
        printk("--------------------------------\n\n\r");
    }

    // high PRIO
}

/*  Task that calculates current position and facing angle of the
    robot, everytime there is a new image and updates shared mem */
void curr_location(void *argA , void *argB, void *argC){
    printk("D) Orientation and Position Thread\n\r");
    
    while(1){
        k_sem_take(&location_sem, K_FOREVER);
        printk("thread D instance released at %lld\n\r", k_uptime_get());

        // get address of vars from shared mem through cab(&curr_angle, &curr_pos)
        // this might prove a vulnerability, maybe we should store the values in temp vars(temp_angle/temp_pos) and update shared mem vars' addresses after calculations

        if(guideLineSearch(img, &curr_pos, &curr_angle)) printk("Failed to find guideline...\n\r");
    }

    // low PRIO
}

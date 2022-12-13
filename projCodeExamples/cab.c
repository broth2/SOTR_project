#include "cab.h"
#include <stdlib.h>

/* structure of CAB buffers  */
struct buffer{		
    struct buffer *next;	
    unsigned char link_counter;
};	


/* structure of CAB  */
struct CAB_struct{
    char name[10]; /* CAB name*/
    int num; /*number of buffers*/
    int num_free_buffers; /*number of free buffers*/
    int dim; /*message dimension*/
    struct buffer *free; /*pointer to first free buffer*/
    struct buffer *last_used; /*pointer to most recent data*/
};



static struct   CAB_struct  cabs[1];
/* returns a CAB*/
CAB open_cab(char *name,int num,int dim,char* first){

    CAB ret = 0;
    struct CAB_struct *cab = &cabs[0] ; /* to return the data where the CAB is descripted*/
    /* Verifies if size vars are correct*/
    if(num < 1 || dim < 1){
        fprintf(stderr,"INVALID CAB DIMENSIONS\n");
        return -1;
    }
    /*Allocate memory to the CAB buffers*/

    /*cabmem_init stores the pointer to the first buffer in the CAB */
    char *first_buffer = malloc(num * (sizeof(struct buffer) + dim)); 

    /* Copy args to struct CAB*/
    strcpy(cab->name,name);
    cab->num =num;

    cab->dim = dim;

    /* Handle first message and store in first buffer*/
    cab->last_used = (struct buffer*)first_buffer;
    printf("Last Used: %p\n", cab->last_used);
    /*pointer to the first buffer*/
    
    /* Store first message in first buffer with memcpy*/
    struct buffer* aux = (struct buffer*) first_buffer;
    
    

    /*initialize the buffers*/
    for(int i = 1; i<=num;i++){
        struct buffer* next_buf = (struct buffer*)(first_buffer +(i* (sizeof(struct buffer) + dim)));
        
        aux->next = next_buf;
        aux->link_counter = 0;
        aux = next_buf;
        printf("Buffer %d: %p\n", i,aux);
    }
    
    
    aux->next = NULL;
    /*fill the rest of the CAB struct*/
    cab->free = cab->last_used->next;
    char *ptr = memcpy((struct buffer*)first_buffer + sizeof(struct buffer),first,10);
    printf("message pointer %p\n",ptr);
    /* Update number of free buffers*/
    cab->num_free_buffers = cab->num - 1;
    printf("cab-last-used: %p\n", cab->last_used);
    
    return ret;
}

char *reserve(CAB id){
    /* Get the pointer to the cab*/
    struct CAB_struct *cab = &cabs[id];

    /*Check if there are free buffers (link_counter < 5)*/
    if(cab->num_free_buffers != 0){
        /*Get buffer free list */
        cab->num_free_buffers--;
        /* chosen buffer is char in order to comply with function type
        Other wise it would be struct_buffer*/
        char* chosen_buffer = (char *)cab->free;

        /*Update free buffer to the next*/
        cab->free = cab->free->next;
        return chosen_buffer;
    }else{
        return(NULL);
    }
   
}

void put_mes(char *buf_pointer,CAB id){
    /* Get the pointer to the cab*/
    struct CAB_struct *cab = &cabs[id];
    struct buffer* old_last_used;
    printf("buf_pointer %p\n", buf_pointer);
    
    /*Get the last used to update with new values*/
    /*Due to CAB properties we always want the data in the last_used buffer*/
    old_last_used = cab->last_used;
    /*if no one is writing or reading*/
    if(old_last_used->link_counter == 0){ 
        /*Update old_last used*/
        old_last_used->next = cab->free;
        cab->free = old_last_used;
        /* Add one free buffer*/
        cab->num_free_buffers++;
        printf("FREE BUFFERS: %d\n",cab->num_free_buffers);
    }
    /*Set the last used as the buffer that we reserved*/
    cab->last_used = (struct buffer*) buf_pointer;
    printf("LAST USED: %p\n", cab->last_used);

}

char* cab_getmes(CAB id){

    struct CAB_struct *cab = &cabs[id];
    /*Increase link_counter while that data is being used*/
    cab->last_used->link_counter++;
    printf("last used get mes: %p\n",cab->last_used + sizeof(struct buffer));
    return (char*)(cab->last_used + sizeof(struct buffer));

}


int main() {
	CAB id;
    char *ola = "olaaaaaaa";
    id = open_cab("Juan", 4,10,ola);
    char *msg_pointer_init = cab_getmes(id);
    printf("MESSAGE: %s\n" , msg_pointer_init);
    char *buf_pointer;
    buf_pointer = reserve(id);
    char *mes = "aaaaaaaaa";
    printf("BUFFER POINTER: %p\n", buf_pointer);
    memcpy((struct buffer*) buf_pointer + sizeof(struct buffer),mes,10);/*10 is dim*/
    put_mes(buf_pointer,id);
    char *buf_pointer2;
    buf_pointer2 = reserve(id);
    char *mes2 = "vvvvvv";
    printf("BUFFER POINTER: %p\n", buf_pointer2);
    memcpy((struct buffer*) buf_pointer2 + sizeof(struct buffer),mes2,10);/*10 is dim*/
    put_mes(buf_pointer2,id);


    char *msg_pointer;
    msg_pointer = cab_getmes(id);
    printf("MESSAGE: %s\n" , msg_pointer);


    
	return 1;
}


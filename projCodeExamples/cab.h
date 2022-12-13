#ifndef CAB_H
#define CAB_H
#include <stdio.h>
#include <string.h>

typedef int CAB;

CAB open_cab(char *name,int num,int dim,char* first);
char *reserve(CAB cab_id);
void put_mes(char* buf_pointer,CAB cab_id);
char  *cab_getmes(CAB id);
void cab_unget(char* mes_pointer, CAB cab_id);

#endif
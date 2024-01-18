
/**********************************************************************

   File          : cmpsc473-p1-second.c

   Description   : This is second chance page replacement algorithm

   Last Modified : Jan 11 09:54:33 EST 2023
   By            : Trent Jaeger

***********************************************************************/
/**********************************************************************
Copyright (c) 2023 The Pennsylvania State University
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of The Pennsylvania State University nor the names of its contributors may be used to endorse or promote products derived from this softwiare without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***********************************************************************/

/* Include Files */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <pthread.h>
#include <sched.h>

/* Project Include Files */
#include "cmpsc473-p1.h"

/* Definitions */

/* second chance list */

typedef struct second_entry {  
  unsigned int pid;
  ptentry_t *ptentry;
  struct second_entry *next;
  struct second_entry *prev;
} second_entry_t;

typedef struct second {
  second_entry_t *first;
} second_t;

second_t *page_list;

/**********************************************************************

    Function    : init_second
    Description : initialize second-chance list
    Inputs      : fp - input file of data
    Outputs     : 0 if successful, -1 otherwise

***********************************************************************/

int init_second( FILE *fp )
{
  page_list = (second_t *)malloc(sizeof(second_t));
  page_list->first = NULL;
  return 0;
}


int rep()
{
  second_entry_t *current;
  current=page_list->first;
  int i=0;
  do{
    printf("value %d: %u\n ",i,current->ptentry->page);
    current=current->next;
    i=i+1;

  } while (current->ptentry->page!=page_list->first->ptentry->page);

  return 0;
}


/**********************************************************************

    Function    : replace_second
    Description : choose victim based on second chance algorithm (first with ref == 0)
    Inputs      : victim - process id of victim frame 
                  frame - frame assigned from fifo -- to be replaced
                  ptentry - pt mapping to be invalidated
    Outputs     : 0 if successful, -1 otherwise

***********************************************************************/

int replace_second( unsigned int *victim, frame_t **frame, ptentry_t **ptentry )
{
  /* Task 3(a) */ 

  second_entry_t *current;
  current=page_list->first;
  if (current==NULL){
    return -1;
  }
  
  do{
    
    current->ptentry->ref=0;
    current=current->next;

  } while (current->ptentry->ref==1);



  *ptentry=current->ptentry;
  *victim=current->pid;
  unsigned int fnum = current->ptentry->frame;
  *frame=&physical_mem[fnum];


  current->next->prev=current->prev;
  current->prev->next=current->next;

  page_list->first=current->prev;
  free(current);
  

  
  
  return 0;
}



/**********************************************************************

    Function    : update_second
    Description : update second chance on allocation 
    Inputs      : pid - process id
                  ptentry - mapping 
    Outputs     : 0 if successful, -1 otherwise

***********************************************************************/

int update_second( unsigned int pid, ptentry_t *ptentry )
{
  /* Task #3(a) */
  
  second_entry_t *new = (second_entry_t *)malloc(sizeof(second_entry_t));
  new->pid=pid;
  new->ptentry=ptentry;

  new->ptentry->ref=1;

  /*Empty Linked List, create a new first node and assign its values.*/
  if (page_list->first==NULL){
    page_list->first=new;
    page_list->first->next=new;
    page_list->first->prev=new;
    return 0;
  }
/* Only one element in the linked list*/
  if (page_list->first->next==page_list->first){
    page_list->first->next=new;
    page_list->first->prev=new;
    new->prev=page_list->first;
    new->next=page_list->first;
    
    printf("second node /n");
    return 0;
  }
  
  else {
    new->next=page_list->first;
    new->prev=page_list->first->prev;

    page_list->first->prev->next=new;

    page_list->first->prev=new;




/*
    new->prev=page_list->first->prev;
    new->next=page_list->first;

    if (page_list->first->prev==page_list->first->next){
      page_list->first->next->prev=new;
    }

    page_list->first->prev->next=new;
    page_list->first->prev=new;
     */
  }
  
  return 0;  
}



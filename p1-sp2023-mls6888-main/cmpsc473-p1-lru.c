
/**********************************************************************

   File          : cmpsc473-p1-lru.c

   Description   : This is LRU page replacement algorithm

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

/* lru list */


typedef struct lru_entry {  
  unsigned int pid;
  ptentry_t *ptentry;
  struct lru_entry *next;
  struct lru_entry *prev;
} lru_entry_t;

typedef struct lru {
  lru_entry_t *first;
} lru_t;

lru_t *frame_list;
/*lru_entry_t *global;*/

/**********************************************************************

    Function    : init_lru
    Description : initialize lru list
    Inputs      : fp - input file of data
    Outputs     : 0 if successful, -1 otherwise

***********************************************************************/

int init_lru( FILE *fp )
{
  printf("initiate lru...\n");
  frame_list = (lru_t *)malloc(sizeof(lru_t));
  frame_list->first = NULL;
  return 0;
}



/**********************************************************************

    Function    : replace_lru
    Description : choose victim from lru list (first in list is oldest)
    Inputs      : victim - process id of victim frame 
                  frame - frame assigned from lru replacement
                  ptentry - pt entry mapping frame currently -- to be invalidated
    Outputs     : 0 if successful, -1 otherwise

***********************************************************************/



int replace_lru( unsigned int *victim, frame_t **frame, ptentry_t **ptentry )
{
  /* Task 3(b) */
  lru_entry_t *dropout=NULL;
  unsigned int mlru=7;
  lru_entry_t *current;
  current=frame_list->first;
  if (current==NULL){
    return -1;
  }
  /*aging period*/
  
  do{
    /*shift by one*/
    physical_mem[current->ptentry->frame].lru=physical_mem[current->ptentry->frame].lru>>1;
   
    /*set msb to ref*/
    if (current->ptentry->ref==1){
        physical_mem[current->ptentry->frame].lru=physical_mem[current->ptentry->frame].lru | 4;
        
      }
      
    /*else{
        physical_mem[current->ptentry->frame].lru=physical_mem[current->ptentry->frame].lru & 3; 
      }*/
    
    
    current->ptentry->ref=0;
    current=current->next;
    /*End loop once head is reached again, this could be wrong so make sure to ckeck this*/
  } while (current!=frame_list->first);


  
current=frame_list->first;
do{
  if (physical_mem[current->ptentry->frame].lru==0){
      dropout=current;
      mlru=0;
      break;
    }
  else if (physical_mem[current->ptentry->frame].lru<mlru){
        dropout=current;
        mlru=physical_mem[current->ptentry->frame].lru;  

    }
  
  current=current->next;
} while (current!=frame_list->first);


  unsigned int fnum = dropout->ptentry->frame;
  *ptentry=dropout->ptentry;
  *victim=dropout->pid;
  dropout->ptentry->valid=INVALID;
  *frame=&physical_mem[fnum];

  dropout->next->prev=dropout->prev;
  dropout->prev->next=dropout->next;

  frame_list->first=dropout->next;
  free(dropout);

  return 0;
}


/**********************************************************************

    Function    : update_lru
    Description : update lru list on allocation (add entry to end)
    Inputs      : pid - process id
                  ptentry - mapped to frame
    Outputs     : 0 if successful, -1 otherwise

***********************************************************************/

int update_lru( unsigned int pid, ptentry_t *ptentry)


{
  /* Task 3(b) */
  /*entry to be inserted*/
  lru_entry_t *insert=(lru_entry_t *)malloc(sizeof(lru_entry_t));
  insert->pid=pid;
  insert->ptentry=ptentry;
  physical_mem[ptentry->frame].lru=4;
  
  insert->ptentry->ref=0;
  /*1 entry in the list*/
  if (frame_list->first==NULL){
    frame_list->first=insert;
    frame_list->first->next=insert;
    frame_list->first->prev=insert;
    return 0;
  }
  /*2 entries in the list*/
  if (frame_list->first->prev==frame_list->first){
    frame_list->first->next=insert;
    frame_list->first->prev=insert;
    insert->next=frame_list->first;
    insert->prev=frame_list->first;
    return 0;
  }
  else{
    /*insert behind the first entry*/
    insert->next=frame_list->first;
    insert->prev=frame_list->first->prev;
    frame_list->first->prev->next=insert;
    frame_list->first->prev=insert;
    
    /*place in front of the head*/
    /*insert->prev=frame_list->first;
    insert->next=frame_list->first->next;
    frame_list->first->next->prev=insert;
    frame_list->first->next=insert;*/


  
  }
    /*End loop once head is reached again, this could be wrong so make sure to ckeck this*/
 
  

 

  
  return 0;  
}



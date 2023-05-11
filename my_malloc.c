#include "my_malloc.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//First Fit malloc/free
unsigned long freeSize = 0;
unsigned long usedSize = 0;

void configMem(node_t * curr, size_t size, node_t * nextNode, node_t *head, node_t * tail) {
  //if the there is only 1 block in the list

  if (curr->prev == NULL && curr->next == NULL) {
    head = nextNode;
    tail = nextNode;
  }
  // if the free block is the first block in the list but there are free block after it

  else if (curr->prev == NULL && curr->next != NULL) {
    if (nextNode == NULL) {
      head = curr->next;
      curr->next->prev = NULL;
    }
    else {
      head = nextNode;
      curr->next->prev = nextNode;
    }
  }
  //if the free bock is the last one in the list
  else if (curr->prev != NULL && curr->next == NULL) {
    if (nextNode == NULL) {
      tail = curr->prev;
      curr->prev->next = nextNode;
    }
    else {
      tail = nextNode;
      curr->prev->next = nextNode;
    }
  }
  // in all other cases, empty block is in the midde of list
  else {
    if (nextNode == NULL) {
      curr->prev->next = curr->next;
      curr->next->prev = curr->prev;
    }
    else {
      curr->prev->next = nextNode;
      curr->next->prev = nextNode;
    }
  }
  if (nextNode != NULL) {
    nextNode->next = curr->next;
    nextNode->prev = curr->prev;
    //    nextNode->size = curr->size - fullSize;

  }
  curr->next = NULL;
  curr->prev = NULL;

  if (nextNode != NULL) {
    curr->size = size;
    //curr->fullsz = size + sizeof(node_t);
  }
}
void * exactMem(node_t * curr, size_t size, node_t * head, node_t *  tail) {
  configMem(curr, size, NULL, head, tail);
  curr->size = size;
  //curr->fullsz = size + sizeof(node_t);
  freeSize -= size + sizeof(node_t);
  return (void *)curr + sizeof(node_t);
}
void * splitMem(node_t * curr, size_t size, node_t * head, node_t * tail) {
  node_t * free = (node_t *)((void *)curr + size + sizeof(node_t));
  free->size = curr->size - size - sizeof(node_t);
  //free->fullsz = free->size + sizeof(node_t);
  configMem(curr, size, free, head, tail);
  freeSize -= size + sizeof(node_t);
  return (void *)curr + sizeof(node_t);
}

void * newMem(size_t size, int tsl) {
  size_t fullSize = size + sizeof(node_t);
  node_t *headNode=NULL;

  if (tsl==0){
     headNode = sbrk(fullSize);
  } else{
    pthread_mutex_lock(&lock);
    headNode = sbrk(fullSize);
    pthread_mutex_unlock(&lock);
  }
  headNode->prev = NULL;
  headNode->next = NULL;
 
  headNode->size = size;
  //headNode->fullsz = fullSize;
  void * beginData = (void *)headNode + sizeof(node_t);
  usedSize += fullSize;
  return beginData;
}
/*void * ff_malloc(size_t size) {
  node_t * curr = head;
  size_t fullSize = size + sizeof(node_t);
  while (curr != NULL) {
    if (size == curr->size) {
      return exactMem(curr, size);
    }
    if (fullSize < (curr->size)) {
      return splitMem(curr, size);
    }
    curr = curr->next;
  }

  return newMem(size);
}*/

void * bf_malloc(size_t size, int tsl, node_t * head, node_t * tail) {
  node_t * curr = head;
  size_t fullSize = size + sizeof(node_t);
  node_t * temp = NULL;
  while (curr != NULL) {
    if (curr->size == size) {
      temp = curr;
      return exactMem(curr, size, head, tail);
    }
    else if (curr->size > fullSize) {
      if (temp == NULL || (temp && temp->size > curr->size)) {
        temp = curr;
      }
      curr = curr->next;
    }
    else
      curr = curr->next;
  }
  if (temp) {
    return splitMem(temp, size, head, tail);
  }
  else
    return newMem(size, tsl);
}

void addToList(node_t * currNode, node_t * head, node_t * tail) {
  //if a node is before the head, add the to the beginning of the list
  if (currNode < (head)) {
    currNode->next = head;
    currNode->prev = NULL;
    (head)->prev = currNode;
    head = currNode;
    
    return;
  }

  // case where node is after head
  node_t * nextNode = head;

  while (nextNode != NULL && currNode > nextNode) {
    nextNode = nextNode->next;
  }
  if (nextNode == NULL) {
    (tail)->next = currNode;
    currNode->next = NULL;
    currNode->prev = (tail);
    (tail) = currNode;
   
  }
  else {
    currNode->prev = nextNode->prev;
    currNode->next = nextNode;
    nextNode->prev->next = currNode;
    nextNode->prev = currNode;
  }
}

void merge(node_t * curr, node_t * head, node_t * tail) {
  if (curr->prev == NULL && curr->next == NULL) {
    head = NULL;
    tail = NULL;
  }
  // if the free block is the first block in the list but there are free block after it

  else if (curr->prev == NULL && curr->next != NULL) {
    curr->next->prev = NULL;
    head = curr->next;
  }
  //if the free bock is the last one in the list
  else if (curr->prev != NULL && curr->next == NULL) {
    curr->prev->next = NULL;
    tail = curr->prev;
  }
  // in all other cases, empty block is in the midde of list
  else {
    curr->prev->next = curr->next;
    curr->next->prev = curr->prev;
  }
  curr->next = NULL;
  curr->prev = NULL;
 
}

void ff_free(void * ptr, node_t * head, node_t * tail) {
  node_t * currNode = ptr - sizeof(node_t);
  //start the list
  if (head == NULL) {
   
    currNode->prev = NULL;
    currNode->next = NULL;
    head = currNode;
    tail = currNode;
    freeSize = currNode->size + sizeof(node_t);
    return;
  }
  //one list is created, keep adding free blocks
  freeSize += currNode->size + sizeof(node_t);
  currNode->next = NULL;
  currNode->prev = NULL;
  addToList(currNode, head, tail);

  if ((currNode->next != NULL &&
       (void *)currNode + currNode->size + sizeof(node_t) == (void *)currNode->next)) {
    currNode->size = currNode->size + sizeof(node_t) + currNode->next->size + sizeof(node_t)-sizeof(node_t);
    //currNode->fullsz += currNode->next->fullsz;
    configMem(currNode->next, currNode->size, NULL, head, tail);
  }

  if ((currNode->prev != NULL &&
       (void *)currNode->prev + currNode->prev->size + sizeof(node_t)== (void *)currNode)) {
    currNode->prev->size = currNode->size + currNode->prev->size + sizeof(node_t);
    //currNode->prev->fullsz += currNode->fullsz;
    configMem(currNode, currNode->size, NULL,head, tail);
  }

}

void bf_free(void * ptr, node_t * head, node_t *tail) {
  ff_free(ptr, head, tail );
}

unsigned long get_data_segment_size() {
  return usedSize;
}
unsigned long get_data_segment_free_space_size() {
  return freeSize;
}

void * ts_malloc_lock (size_t size){
  pthread_mutex_lock(&lock);
  void * ans = bf_malloc(size, 0,  lock_head,  lock_tail);
  pthread_mutex_unlock (&lock);
  return ans;
}

void ts_free_lock (void * ptr){
  pthread_mutex_lock(&lock);
  bf_free(ptr, lock_head, lock_tail);
  pthread_mutex_unlock (&lock);
}


void  * ts_malloc_nolock (size_t size){
  void * ans = bf_malloc (size, 1, nolock_head, nolock_tail);
}
void ts_free_nolock ( void * ptr){
  bf_free(ptr, nolock_head, nolock_tail);
}


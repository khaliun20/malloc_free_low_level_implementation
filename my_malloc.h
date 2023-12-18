// memory allocation project
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct node_tag {
  size_t size;
  struct node_tag * next;
  struct node_tag * prev;
};
typedef struct node_tag node_t;



//First Fit malloc/free
/*void * ff_malloc(size_t size);
void ff_free(void * ptr);

// Best Fit malloc/free
void * bf_malloc(size_t sie);
void bf_free(void * ptr);
unsigned long get_data_segment_size();
unsigned long get_data_segment_free_space_size();
void merge(node_t * curr);
void addToList(node_t * currNode);
void * newMem(size_t size);
void * splitMem(node_t * curr, size_t fullSize);
void * exactMem(node_t * curr, size_t fullSize);
void configMem(node_t * curr, size_t fullSize, node_t * nextNode);*/

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
node_t * lock_head =NULL;
node_t * lock_tail=NULL;
__thread node_t * nolock_tail= NULL;
__thread node_t * nolock_head = NULL;
void configMem(node_t * curr, size_t size, node_t * nextNode, node_t * head, node_t * tail);
void * exactMem(node_t * curr, size_t size, node_t * head, node_t *  tail);
void * splitMem(node_t * curr, size_t size, node_t * head, node_t * tail);
void * newMem(size_t size, int tsl);
void * bf_malloc(size_t size, int tsl, node_t * head, node_t * tail);
void addToList(node_t * currNode, node_t * head, node_t * tail);
void merge(node_t * curr, node_t * head, node_t * tail);
void ff_free(void * ptr, node_t * head, node_t * tail);
void bf_free(void * ptr, node_t * head, node_t *tail);
unsigned long get_data_segment_size();
unsigned long get_data_segment_free_space_size();
void * ts_malloc_lock (size_t size);
void ts_free_lock (void * ptr);
void  * ts_malloc_nolock (size_t size);
void ts_free_nolock ( void * ptr);


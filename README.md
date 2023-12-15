# Low Level Implementation of Malloc() and Free() using Linked List

## Project Overview

In this project, I implemented my very own malloc and free functions in C. If you are not familiar with the function, malloc() is used to request memory space in the heap and free() is used to make the space available again. 

I used linked list as my data structure and keeps track of the free space in the memory and allocate appropriate amount of memory from the space whenever new memory space is requested. 

I employed two different strategies to decide which memory space to allocate:

  * Best Fit Memory Allocation - Allocates memory that is just closest to the requested size of memory
  
  * First Fit Memory Allocation - From the free list, allocates the first set of memory space that is larger than the requested size of the memory space

This project was such an excellent way to deepen my understanding of C and how heap space works! 

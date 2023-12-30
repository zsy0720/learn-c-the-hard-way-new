ifndef _STACK_DARRAY_H
  2 #define _STACK_DARRAY_H
  3 #include "darray.h"
  4 
  5 typedef DArray Stack;
  6 
  7 #define Stack_create DArray_create
  8 #define Stack_destroy DArray_destroy
  9 #define Stack_clear DArray_clear
 10 #define Stack_clear_destroy DArray_clear_destroy
 11 
 12 #define Stack_push DArray_push
 13 #define Stack_pop DArray_pop
 14 #define Stack_peek DArray_last
 15 
 16 #define Stack_count DArray_count
 17 
 18 #define STACK_FOREACH(stack)\
     for(int a =0;a<stack->max;a++)//stack的便利如果用darray动态数组做的话反而不用那么麻烦，因为动态数组使用下标表示元素位置的，所以只需要将下标从0->max即可
    #endif

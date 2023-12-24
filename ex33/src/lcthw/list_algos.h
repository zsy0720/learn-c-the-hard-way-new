#ifndef _List_algos_h
#define _List_algos_h

#include "list.h"
#define LIST_FOREACH(L, S, M, V) ListNode *_node = NULL;\
    ListNode *V = NULL;\
    for(V = _node = L->S; _node != NULL; V = _node = _node->M)//不知道为啥必须得把这个宏定义放到这个头文件中，否则调用就会报错

typedef int (*List_compare)(const void *a, const void *b);//typedef表示用*List_compare作为函数指针来指向一个函数

int List_bubble_sort(List *list, List_compare cmp);//声明.c文件中的函数以供调用

List *List_merge_sort(List *list, List_compare cmp);

#endif

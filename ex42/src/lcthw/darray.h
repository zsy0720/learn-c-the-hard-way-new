#ifndef _DARRAY_H
#define _DARRAY_H
#include <stdlib.h>
#include <assert.h>
#include "dbg.h"

//动态数组的结构，element_size是数组数据类型，max为数组最大能到达长度，end为数组已有元素结束的长度，expand_rate为数组扩展一次长度，contents是数组元素
typedef struct DArray {
    int end;
    int max;
    size_t element_size;
    size_t expand_rate;
    void **contents;
} DArray;

DArray *DArray_create(size_t element_size, size_t initial_max);

void DArray_destroy(DArray *array);

void DArray_clear(DArray *array);

int DArray_expand(DArray *array);

int DArray_contract(DArray *array);

int DArray_push(DArray *array, void *el);

void *DArray_pop(DArray *array);

void DArray_clear_destroy(DArray *array);

#define DArray_last(A) ((A)->contents[(A)->end - 1])
#define DArray_first(A) ((A)->contents[0])
#define DArray_end(A) ((A)->end)
#define DArray_count(A) DArray_end(A)
#define DArray_max(A) ((A)->max)

#define DEFAULT_EXPAND_RATE 300

//插入一个元素，这个元素的位置在已有元素所到达的长度和最大长度之间
static inline void DArray_set(DArray *array, int i, void *el)
{
    check(i < array->max, "darray attempt to set past max");//若是超过最大长度，说明这个位置不在动态数组中
    if(i > array->end) array->end = i;//若是位置在数组已有元素中，则无法插入，只能报错
    array->contents[i] = el;
error:
    return;
}

//读取一个元素
static inline void *DArray_get(DArray *array, int i)
{
    check(i < array->max, "darray attempt to get past max");//这个元素必须在最大长度内
    return array->contents[i];
error:
    return NULL;
}

static inline void *DArray_remove(DArray *array, int i)
{
    void *el = array->contents[i];//将位于i的数组元素给除掉  为啥这次不判断是否在数组最大长度里面了？可以加上吗

    array->contents[i] = NULL;

    return el;
}
//重新分配内存
static inline void *DArray_new(DArray *array)
{
    check(array->element_size > 0, "Can't use DArray_new on 0 size darrays.");

    return calloc(1, array->element_size);

error:
    return NULL;
}

#define DArray_free(E) free((E))

#endif

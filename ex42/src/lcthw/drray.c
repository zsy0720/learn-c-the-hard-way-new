#include "darray.h"
#include <assert.h>

//新建一个动态数组，他的数据类型、最大长度由输入参数给出 如果想要更换数据类型可以用sizeof(void*)，应该能行
DArray *DArray_create(size_t element_size, size_t initial_max)
{
    DArray *array = malloc(sizeof(DArray));
    check_mem(array);
    array->max = initial_max;
    check(array->max > 0, "You must set an initial_max > 0.");

    array->contents = calloc(initial_max, sizeof(void *));//预先分配好所需的最大长度，方便添加
    check_mem(array->contents);

    array->end = 0;//未添加数组元素，所以数组有效长度为0
    array->element_size = element_size;
    array->expand_rate = DEFAULT_EXPAND_RATE;

    return array;

error:
    if(array) free(array);
    return NULL;
}
//清空数组元素并释放内存
void DArray_clear(DArray *array)
{
    int i = 0;
    if(array->element_size > 0) {
        for(i = 0; i < array->max; i++) {
            if(array->contents[i] != NULL) {
                free(array->contents[i]);
            }
        }
    }
}

//重新定义数组最大长度
static inline int DArray_resize(DArray *array, size_t newsize)
{
    array->max = newsize;
    check(array->max > 0, "The newsize must be > 0.");

    void *contents = realloc(array->contents, array->max * sizeof(void *));//重新分配内存，指向array->contents，大小为array->max*sizeof(void*)
    // check contents and assume realloc doesn't harm the original on error

    check_mem(contents);

    array->contents = contents;//将分配好的内存指向array->contents

    return 0;
error:
    return -1;
}

//扩展长度
int DArray_expand(DArray *array)
{
    size_t old_max = array->max;
    //检查是否复制成功
    check(DArray_resize(array, array->max + array->expand_rate) == 0,
            "Failed to expand array to new size: %d",
            array->max + (int)array->expand_rate);

    memset(array->contents + old_max, 0, array->expand_rate + 1);// void *memset(void *str, int c, size_t n) 复制字符 c（一个无符号字符）到参数 str 所指向的字符串的前 n 个字符
    return 0;

error:
    return -1;
}

//进行比较，现有长度和扩张长度哪个大，谁大就取谁作为resize的参数
int DArray_contract(DArray *array)
{
    int new_size = array->end < (int)array->expand_rate ? (int)array->expand_rate : array->end;

    return DArray_resize(array, new_size + 1);
}


//释放内存
void DArray_destroy(DArray *array)
{
    if(array) {
        if(array->contents) free(array->contents);
        free(array);
    }
}

void DArray_clear_destroy(DArray *array)
{
    DArray_clear(array);
    DArray_destroy(array);
}

//在动态数组有效元素后加入一个元素
int DArray_push(DArray *array, void *el)
{
    array->contents[array->end] = el;
    array->end++;

    if(DArray_end(array) >= DArray_max(array)) {
        return DArray_expand(array);
    } else {
        return 0;
    }
}
//去除最后一个元素
void *DArray_pop(DArray *array)
{
    check(array->end - 1 >= 0, "Attempt to pop from empty array.");

    void *el = DArray_remove(array, array->end - 1);
    array->end--;

    if(DArray_end(array) > (int)array->expand_rate && DArray_end(array) % array->expand_rate) {
        DArray_contract(array);
    }

    return el;
error:
    return NULL;
}

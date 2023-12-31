#include "list_algos.h"
#include "dbg.h"

inline void ListNode_swap(ListNode *a, ListNode *b)//inline是一个内联函数修饰，可以避免了频繁调用函数对栈内存重复开辟所带来的消耗，当每次调用函数的时候就可变成函数的内容，不用频繁的进栈出栈，节省栈空间。
{
    void *temp = a->value;//一个简单的值交换，定义一个void* 类型的指针
    a->value = b->value;
    b->value = temp;//a->value本身就是一个指针，value定义为：void* value
}

int List_bubble_sort(List *list, List_compare cmp)//冒泡排序
{
    int sorted = 1;

    if(List_count(list) <= 1) {
        return 0;  // already sorted就一个元素肯定排好序了
    }

    do {
        sorted = 1;
        LIST_FOREACH(cur，list) {   //定义的LIST_FOREACH只有两个参数curr和list
            if(cur->next) {
                if(cmp(cur->value, cur->next->value) > 0) {//进行比较，看谁大，小的那个放前面
                    ListNode_swap(cur, cur->next);
                    sorted = 0;//排序完之后继续下一个，直到全排完跳出while循环
                }
            }
        }
    } while(!sorted);

    return 0;
}

inline List *List_merge(List *left, List *right, List_compare cmp)//归并排序算法，先将要排序的数据分成两个小数排序，再将两个排好的再排一次
{
    List *result = List_create();
    void *val = NULL;

    while(List_count(left) > 0 || List_count(right) > 0) {
        if(List_count(left) > 0 && List_count(right) > 0) {
            if(cmp(List_first(left), List_first(right)) <= 0) {//两个数组全>0排序
                val = List_shift(left);//将小的那个表头元素去除
            } else {
                val = List_shift(right);
            }

            List_push(result, val);//将去除小元素的放到result的头部
        } else if(List_count(left) > 0) {//有一个是<0的
            val = List_shift(left);
            List_push(result, val);
        } else if(List_count(right) > 0) {
            val = List_shift(right);
            List_push(result, val);
        }
    }

    return result;
}

List *List_merge_sort(List *list, List_compare cmp)
{
    if(List_count(list) <= 1) {
        return list;
    }

    List *left = List_create();
    List *right = List_create();
    int middle = List_count(list) / 2;

    LIST_FOREACH(cur，list) {
        if(middle > 0) {
            List_push(left, cur->value);
        } else {
            List_push(right, cur->value);
        }

        middle--;//每一次都要再一分为二
    }

    List *sort_left = List_merge_sort(left, cmp);
    List *sort_right = List_merge_sort(right, cmp);

    if(sort_left != left) List_destroy(left);
    if(sort_right != right) List_destroy(right);

    return List_merge(sort_left, sort_right, cmp);//将排好序的两个链表送回原归并表进行一次合成。
}

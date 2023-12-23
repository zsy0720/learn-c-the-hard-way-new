#include "list.h"
#include <stdlib.h>
#include <assert.h>
#include "dbg.h"//牵扯到自己写的头文件要用“”来调用

static ListNode *List_createNode();//静态变量，声明了一个结构体指针

//创建一个空链表，首先为其分配内存，随后定义第一个和最后一个链表地址为NULL，个数为0，最后返回创建好的链表
List *List_create()
{
	List *list = calloc(1, sizeof(List));
	list->first = NULL;
	list->last = NULL;
	list->count = 0;
	
    return list;
}

//首先传入一个list，随后定义一个双向链表，通过for循环将传入链表的首地址赋给定义的链表，传一个next的成员地址就将上个成员的地址释放掉，直到全释放完，再将定义的链表和传入的链表都释放
void List_destroy(List *list)
{
	ListNode *node;
    for (node = list->first; node != NULL; node = node->next)
        free(node->prev);

    free(node);
    free(list);
}

//同上一个函数，传入链表的首地址赋给双向链表，随后for循环遍历整个链表，将value的空间释放
void List_clear(List *list)
{
    for (ListNode *node = list->first; node != NULL; node = node->next)
        free(node->value);
}

//调用上面的两个函数
void List_clear_destroy(List *list)
{
    List_clear(list);
    List_destroy(list);
}

//查看非空链表的数量
int List_count(List *list)
{
    if (!list) return 0;

    return list->count;
}

//检查链表是否为空，不为空则输出第一条链表的值
void *List_first(List *list)
{
    if (!list || !list->first) return NULL;
    assert(list->first->value != NULL);//先计算表达式expression的值是否为0，为0则调用abort终止程序
    
    return list->first->value;
}

//返回最后一个链表的value
void *List_last(List *list)
{
    if (!list || !list->last) return NULL;
    assert(list->last->value != NULL);
    
    return list->last->value;    
}

//检查链表是否为空，创建一个链表，随后检查链表 list_push作用是在链表末尾添加一个元素
void List_push(List *list, void *value)
{
    if (!list || !value) return;

    ListNode *node = List_createNode();
    check_mem(node);//#define check(A, M, ...) if(!(A)) { log_err(M, ##__VA_ARGS__); errno=0; goto error; }

    node->value = value;

    if (list->count == 0)//若链表值为0，从头开始
    {
        assert(list->last == NULL && list->first == NULL);

        list->first = node;//将node链表首地址赋给list
        list->last = list->first;//list—>first给list_last
    }
    else
    {
		node->prev = list->last;//若不是从头开始，将list_last赋给node_prev
		list->last->next = node;node赋给list_last_next作为下一个
		
        list->last = node;
    }
        
    list->count++;

error:
    return;
}

//list_pop与list_push刚好相反，他是去除最后一个元素
void *List_pop(List *list)
{
	if (!list) return NULL;
	
	return List_remove(list, list->last);	
}

//快速向表头添加文件
void List_unshift(List *list, void *value)
{
    if (!list || !value) return;

	ListNode *node = List_createNode();
	check_mem(node);

	node->value = value;
	
	if (list->count == 0)
	{
		list->last = node;	
	}
	else
	{
		node->next = list->first;
		list->first->prev = node;
	}
	
	list->first = node;
	list->count++;
	
error:
	return;
}

//去除表头第一个元素并且返回当前元素
void *List_shift(List *list)
{
    if (!list) return NULL;

    return List_remove(list, list->first);
}


void *List_remove(List *list, ListNode *node)
{
    if (list == NULL || node == NULL || list->count == 0) return NULL;
    
    if (node->prev)//若前一个链表成员存在，则将下个成员变成当前这个
    {
        node->prev->next = node->next;
    }
    else
    {
        assert(node == list->first);//若链表node与list的下一个一样，则将node_next赋给list作为第一个
        list->first = node->next;
    }

    if (node->next)//若后一个链表成员存在，则将前一个成员当成现在的
    {
        node->next->prev = node->prev;
    }
    else
    {
        assert(node == list->last);
        list->last = node->prev;
    }

    list->count--;
    void *data = node->value;
	free(node);    

    return data;
}

//定义一个空的链表
static ListNode *List_createNode()
{
	ListNode *node = malloc(sizeof(ListNode));
	node->prev = NULL;
	node->next = NULL;
	node->value = NULL;
	
	return node;
}

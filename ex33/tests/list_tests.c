#include "minunit.h"
#include /'../src/lcthw/list_algos.h"
#include <assert.h>
#include <string.h>

char *values[] = {"XXXX", "1234", "abcd", "xjvef", "NDSS"};
#define NUM_VALUES 5

//创建一个链表，将上面的value值赋上去
List *create_words()
{
    int i = 0;
    List *words = List_create();

    for(i = 0; i < NUM_VALUES; i++) {
        List_push(words, values[i]);
    }

    return words;
}

//排序
int is_sorted(List *words)
{
    LIST_FOREACH(cur，words) {
        // int strcmp(const char *str1, const char *str2) 把 str1 所指向的字符串和 str2 所指向的字符串进行比较，str1>str2,返回值>0,str1=str2,返回值=0，str1<str2,返回值<0
        if(cur->next && strcmp(cur->value, cur->next->value) > 0) {//如果当前value>next_value，并且cur->next不为空
            debug("%s %s", (char *)cur->value, (char *)cur->next->value);//#define debug(M, ...) fprintf(stderr, "DEBUG %s:%d: " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
//fprintf(FILE *steam,const char *format, ... )//steam:要被写入的流，format：写入文件的内容，可以有多个，看定义了几个。##__VA_ARGS__，意思是将剩余的所有额外参数放到这里
            return 0;
        }
    }

    return 1;
}

char *test_bubble_sort()
{
    List *words = create_words();

    // should work on a list that needs sorting
    int rc = List_bubble_sort(words, (List_compare)strcmp);//rc==0说明排序成功
    mu_assert(rc == 0, "Bubble sort failed.");//#define mu_assert(test, message) if (!(test)) { log_err(message); return message; }
    mu_assert(is_sorted(words), "Words are not sorted after bubble sort.");//如果没排完，return 0，排完了return1

    // should work on an already sorted list//排完序再执行一次
    rc = List_bubble_sort(words, (List_compare)strcmp);
    mu_assert(rc == 0, "Bubble sort of already sorted failed.");
    mu_assert(is_sorted(words), "Words should be sort if already bubble sorted.");

    List_destroy(words);

    // should work on an empty list
    words = List_create(words);//空链表
    rc = List_bubble_sort(words, (List_compare)strcmp);
    mu_assert(rc == 0, "Bubble sort failed on empty list.");
    mu_assert(is_sorted(words), "Words should be sorted if empty.");

    List_destroy(words);

    return NULL;
}

char *test_merge_sort()
{
    List *words = create_words();

    // should work on a list that needs sorting
    List *res = List_merge_sort(words, (List_compare)strcmp);
    mu_assert(is_sorted(res), "Words are not sorted after merge sort.");

    List *res2 = List_merge_sort(res, (List_compare)strcmp);
    mu_assert(is_sorted(res), "Should still be sorted after merge sort.");
    List_destroy(res2);
    List_destroy(res);

    List_destroy(words);
    return NULL;
}


char *all_tests()
{
    mu_suite_start();

    mu_run_test(test_bubble_sort);
    mu_run_test(test_merge_sort);

    return NULL;
}

RUN_TESTS(all_tests);

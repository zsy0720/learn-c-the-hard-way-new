//函数指针与指针函数
//函数指针：指向函数的一个指针
//指针函数：返回值是指针的函数
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

void die(const char *message)
{
    if(errno) {
        perror(message);
    } else {
        printf("ERROR: %s\n", message);
    }

    exit(1);
}
//typedef可以将一个类型的定义简化
//例如下面typedef int*(*sort)(int*,int,compare_cb)定义后可以使用sort定义一个返回值为指针的函数指针。
typedef int*(*sort)(int*,int,compare_cb);//这是一个指针函数，他的返回值是一个指针，他是一个返回值是指针的函数指针。int*表示这是一个返回值为整形指针的函数，（*sort）表示这是一个函数指针，可以指向函数
typedef int (*compare_cb)(int a, int b);//这是一个函数指针，他可以指向任意一个返回值为整形（int），参数为两个int变量的函数。*compare_cb表示一个指针，可以指向函数
//冒泡排序
int *bubble_sort(int *numbers, int count, compare_cb cmp)//这里compare_cb在定义后可以直接使用
{
    int temp = 0;
    int i = 0;
    int j = 0;
    int *target = malloc(count * sizeof(int));

    if(!target) die("Memory error.");

    memcpy(target, numbers, count * sizeof(int));

    for(i = 0; i < count; i++) {
        for(j = 0; j < count - 1; j++) {
            if(cmp(target[j], target[j+1]) > 0)
	   {
                temp = target[j+1];
                target[j+1] = target[j];
                target[j] = temp;
            }
        }
    }

    return target;
}
//快速排序算法
int* quick_sort(int*s, int l, int r,compare_cb cmp)//通过调用compare_cb类型的变量来实现函数调用
{   
	
    if (l < r)
    {
        int i = l, j = r, x = s[l];
        while (i<j)
        {
            while(cmp(x,s[j])<0 && i<j) 
                j--;  
            if(i < j) 
                s[i++] = s[j];
            
            while(i < j && cmp(s[i],x)<0) 
                i++;  
            if(i < j) 
                s[j--] = s[i];
        }
        s[i] = x;
        quick_sort(s, l, i - 1,cmp); 
        quick_sort(s, i + 1, r,cmp);
    }

	return s;
}

int *quicksort(int *s,int count,compare_cb cmp)
{
	int l=0,h=count-1;
	int *target = malloc(count * sizeof(int));
	memcpy(target, s, count * sizeof(int));
	quick_sort(target,l,h,cmp);
	return target;
	
}

//三种顺序：顺序，倒序，随机顺序
int sorted_order(int a, int b)
{
    return a - b;
}

int reverse_order(int a, int b)
{
    return b - a;
}

int strange_order(int a, int b)
{
    if(a == 0 || b == 0) {
        return 0;
    } else {
        return a % b;
    }
}

/**
 * Used to test that we are sorting things correctly
 * by doing the sort and printing it out.
 */
void test_sorting(int *numbers, int count, compare_cb cmp,sort adp)//这里通过调用compare_cb和sort类型的变量，可以直接将函数当成参数进行传递。compare_cb是定义返回值为整形的函数的定义符，sort是定义返回值为指针的函数指针的定义符。
{
    int i = 0;
    int *sorted = adp(numbers, count, cmp);

    if(!sorted) die("Failed to sort as requested.");

    for(i = 0; i < count; i++) {
        printf("%d ", sorted[i]);
    }
    printf("\n");
unsigned char *data = (unsigned char *)cmp;

for(i = 0; i < 25; i++) {
    printf("%02x:", data[i]);
}

printf("\n");
    free(sorted);
}


int main(int argc, char *argv[])
{
    if(argc < 2) die("USAGE: ex18 4 3 1 5 6");

    int count = argc - 1;
    int i = 0;
    char **inputs = argv + 1;

    int *numbers = malloc(count * sizeof(int));
    if(!numbers) die("Memory error.");

    for(i = 0; i < count; i++) {
        numbers[i] = atoi(inputs[i]);
    }

    test_sorting(numbers, count, sorted_order,bubble_sort);//compare_cb指向了sorted_order，sort指向了bubble_sort。  int sorted_order(int a, int b)
    test_sorting(numbers, count, reverse_order,bubble_sort);                                              //        int *bubble_sort(int *numbers, int count, compare_cb cmp)
    test_sorting(numbers, count, strange_order,bubble_sort);
	
	test_sorting(numbers, count, sorted_order,quick_sort);
	test_sorting(numbers, count, reverse_order,quick_sort);

   

    free(numbers);

    return 0;
}


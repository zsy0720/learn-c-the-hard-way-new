#include <stdio.h>
#include "../src/dbg.h"//此时dbg.h与ex29.c不在一个文件夹内
#include <dlfcn.h>

typedef int (*lib_function)(const char *data);//定义一个返回值为int类型，参数为（const char *）的函数


int main(int argc, char *argv[])
{
    int rc = 0;
    check(argc == 4, "USAGE: ex29 libex29.so function data");

    char *lib_file = ../build/libex29.so;//修改动态库路径为现有路径
    char *func_to_run = argv[2];
    char *data = argv[3];

    void *lib = dlopen(lib_file, RTLD_NOW);
    check(lib != NULL, "Failed to open the library %s: %s", lib_file, dlerror());

    lib_function func = dlsym(lib, func_to_run);
    check(func != NULL, "Did not find %s function in the library %s: %s", func_to_run, lib_file, dlerror());

    rc = func(data);
    check(rc == 0, "Function %s return %d for data: %s", func_to_run, rc, data);

    rc = dlclose(lib);
    check(rc == 0, "Failed to close %s", lib_file);

    return 0;

error:
    return 1;
}

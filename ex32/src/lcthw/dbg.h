#ifndef __dbg_h__
#define __dbg_h__

#include <stdio.h>
#include <errno.h>
#include <string.h>

#ifdef NDEBUG
#define debug(M, ...)
#else
//stderr是stdio.h提供的文件指针
#define debug(M, ...) fprintf(stderr, "DEBUG %s:%d: " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
//fprintf(FILE *steam,const char *format, ... )//steam:要被写入的流，format：写入文件的内容，可以有多个，看定义了几个。##__VA_ARGS__，意思是将剩余的所有额外参数放到这里
#endif//使用了__FILE__和__LINE__来获取当前fine:line用于调试信息

#define clean_errno() (errno == 0 ? "None" : strerror(errno))//C 库函数 char *strerror(int errnum) 从内部数组中搜索错误号 errnum，并返回一个指向错误消息字符串的指针。strerror 生成的错误字符串取决于开发平台和编译器
//char *strerror(int errnum)

//显示error/warning
#define log_err(M, ...) fprintf(stderr, "[ERROR] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)

#define log_warn(M, ...) fprintf(stderr, "[WARN] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)

#define log_info(M, ...) fprintf(stderr, "[INFO] (%s:%d) " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define check(A, M, ...) if(!(A)) { log_err(M, ##__VA_ARGS__); errno=0; goto error; }//goto把控制无条件转移到同一函数内的被标记的语句

#define sentinel(M, ...)  { log_err(M, ##__VA_ARGS__); errno=0; goto error; }

#define check_mem(A) check((A), "Out of memory.")

#define check_debug(A, M, ...) if(!(A)) { debug(M, ##__VA_ARGS__); errno=0; goto error; }

#endif

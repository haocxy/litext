#pragma once

// 定义 litext 库的导出函数的函数名,参数列表,返回类型
// 其中:
// 由 LITEXT_API_FN_ 开头的宏用于定义函数名(FN = Function Name);
// 由 LITEXT_API_FP_ 开头的宏用于定义参数列表(FP = Function Param),必须包含参数名以用于定义函数;
// 由 LITEXT_API_FR_ 开头的宏用于定义返回类型(FR = Function Return);
//
// 通过这样的方式, 可以保证函数提供者和使用者的一致性,
// 函数定义时会通过这些宏来组成函数头部, 而函数使用者则可以通过这些宏来查找函数和转换其函数指针为正确类型

#define LITEXT_API_FN_create() create
#define LITEXT_API_FP_create() ()
#define LITEXT_API_FR_create() void *

#define LITEXT_API_FN_destroy() destroy
#define LITEXT_API_FP_destroy() (void *p)
#define LITEXT_API_FR_destroy() void

#define LITEXT_API_FN_initSetShouldStartAsServer() initSetShouldStartAsServer
#define LITEXT_API_FP_initSetShouldStartAsServer() (void *p, int b)
#define LITEXT_API_FR_initSetShouldStartAsServer() void

#define LITEXT_API_FN_initSetLogLevel() initSetLogLevel
#define LITEXT_API_FP_initSetLogLevel() (void *p, const char *level)
#define LITEXT_API_FR_initSetLogLevel() void

#define LITEXT_API_FN_initSetShouldLogToStd() initSetShouldLogToStd
#define LITEXT_API_FP_initSetShouldLogToStd() (void *p, int b)
#define LITEXT_API_FR_initSetShouldLogToStd() void

#define LITEXT_API_FN_initAddOpenFileWithUtf8FilePathAndRowNum() initAddOpenFileWithUtf8FilePathAndRowNum
#define LITEXT_API_FP_initAddOpenFileWithUtf8FilePathAndRowNum() (void *p, const char *file, long long row)
#define LITEXT_API_FR_initAddOpenFileWithUtf8FilePathAndRowNum() void

#define LITEXT_API_FN_exec() exec
#define LITEXT_API_FP_exec() (void *p)
#define LITEXT_API_FR_exec() int

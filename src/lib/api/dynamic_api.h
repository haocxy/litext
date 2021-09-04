#pragma once

// 定义 notesharp 库的导出函数的函数名,参数列表,返回类型
// 其中:
// 由 NOTESHARP_API_FN_ 开头的宏用于定义函数名(FN = Function Name);
// 由 NOTESHARP_API_FP_ 开头的宏用于定义参数列表(FP = Function Param),必须包含参数名以用于定义函数;
// 由 NOTESHARP_API_FR_ 开头的宏用于定义返回类型(FR = Function Return);
//
// 通过这样的方式, 可以保证函数提供者和使用者的一致性,
// 函数定义时会通过这些宏来组成函数头部, 而函数使用者则可以通过这些宏来查找函数和转换其函数指针为正确类型

#define NOTESHARP_API_FN_create() create
#define NOTESHARP_API_FP_create() ()
#define NOTESHARP_API_FR_create() void *

#define NOTESHARP_API_FN_destroy() destroy
#define NOTESHARP_API_FP_destroy() (void *p)
#define NOTESHARP_API_FR_destroy() void

#define NOTESHARP_API_FN_initSetShouldStartAsServer() initSetShouldStartAsServer
#define NOTESHARP_API_FP_initSetShouldStartAsServer() (void *p, int b)
#define NOTESHARP_API_FR_initSetShouldStartAsServer() void

#define NOTESHARP_API_FN_initSetLogLevel() initSetLogLevel
#define NOTESHARP_API_FP_initSetLogLevel() (void *p, const char *level)
#define NOTESHARP_API_FR_initSetLogLevel() void

#define NOTESHARP_API_FN_initSetShouldLogToStd() initSetShouldLogToStd
#define NOTESHARP_API_FP_initSetShouldLogToStd() (void *p, int b)
#define NOTESHARP_API_FR_initSetShouldLogToStd() void

#define NOTESHARP_API_FN_initAddOpenFileWithUtf8FilePathAndRowNum() initAddOpenFileWithUtf8FilePathAndRowNum
#define NOTESHARP_API_FP_initAddOpenFileWithUtf8FilePathAndRowNum() (void *p, const char *file, long long row)
#define NOTESHARP_API_FR_initAddOpenFileWithUtf8FilePathAndRowNum() void

#define NOTESHARP_API_FN_exec() exec
#define NOTESHARP_API_FP_exec() (void *p)
#define NOTESHARP_API_FR_exec() int


// 获得C字符串形式的实际函数名
#define NOTESHARP_API_FN_NAME(f) BOOST_PP_STRINGIZE(BOOST_PP_CAT(NOTESHARP_API_FN_,f)())

// 获得函数类型
#define NOTESHARP_API_FN_TYPE(f) BOOST_PP_CAT(NOTESHARP_API_FR_, f)() BOOST_PP_CAT(NOTESHARP_API_FP_, f)()

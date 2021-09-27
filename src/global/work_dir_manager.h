#pragma once

#include "core/fs.h"



// 管理程序工作用的各种目录
// 例如存放各种锁文件的目录,存放各种数据文件的目录,存放日志文件的目录,等等
// 
// 目录的选择也是比较复杂的逻辑,有必要把这些逻辑放在类中谨慎地管理
// 
// 复杂性1: 不同类型的文件可能倾向于放在不同的目录
// 例如日志文件倾向于放在操作系统统一管理的临时目录中,以便于操作系统对其回收,
// 而配置文件则需要放在某个更为稳定的目录中
// 
// 复杂性2: 不同操作系统的文件可能倾向于放在不同的目录
// 例如同样是配置文件,Windows平台倾向于放在程序安装目录中,
// 而Linux平台则倾向于放在更为统一的目录(例如home目录)
class DirManager {
public:
    DirManager();

    // 用于锁定 loader 逻辑的文件锁
    //
    // 锁定时间: 短, 只锁定loader程序中启动server和通知server的逻辑
    //
    // 确保有多个 loader 同时运行时只有一个 loader 执行逻辑
    // 在通常的手动操作的情况下,一般是不可能同时启动多个 loader 的
    // 而且 loader 的逻辑也足够快速,不会因为加锁影响使用体验
    // loader 的启动逻辑开始时立刻加锁
    //
    // 在以下两种情况下解锁:
    // 1: loader 发现服务已启动并成功地向服务端发送了请求并收到响应,
    //    此时 loader 应该解锁并退出 loader 程序
    // 2: loader 发现服务未启动且成功启动服务,当服务已经正常运行时,
    //    此时 loader 应该解锁
    fs::path loadLogicLockFile() const;

    // 用于确保只有一个服务进程运行
    //
    // 锁定时间: 很长, 在server正常运行的全程加锁
    // 
    // 服务端在正常运行(即可以响应客户端的请求)的全程都持有锁,
    // 通过判断该文件是否加锁来判断服务器是否运行
    fs::path singletonServerRunningLockFile() const;

    // 锁定服务信息文件的访问
    //
    // 锁定时间: 短, 只在对服务信息文件的写入和读取时加锁
    // 
    // 更新或读取服务端信息文件时使用的锁
    // 仅在服务端更新该文件和客户端读取该文件时才获得该锁
    // 通过该文件的内容查找服务器在哪个端口监听,以及一些其它的辅助信息
    fs::path singletonServerInfoFileLock() const;

    // 记录服务端的信息
    // 该文件本身不加锁,仅记录信息
    // 但使用该文件前应该通过singletonServerInfoFileLock()获得锁来锁定对该文件的访问逻辑
    //
    // TODO 受限于目前使用的库,无法实现对文件本身加锁,只能使用另外一个专门用于加锁的文件
    // TODO 未来应该自己通过直接调用操作系统API实现对文件本身加锁
    fs::path singletonServerInfoFile() const;

    // 字体数据库文件的路径
    fs::path fontDbFile() const;

private:
    fs::path workDir_;
};


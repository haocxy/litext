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

    // 不记录任何数据
    // 服务端在程序运行的全程都持有锁,
    // 通过判断该文件是否加锁来判断服务器是否运行
    fs::path singletonLockFile() const;

    // 不记录任何数据
    // 更新或读取服务端信息文件时使用的锁
    // 仅在服务端更新该文件和客户端读取该文件时才获得该锁
    // 通过该文件的内容查找服务器在哪个端口监听,以及一些其它的辅助信息
    fs::path singletonServerInfoFileLock() const;

    // 记录服务端的信息
    // 该文件本身不加锁,仅记录信息,
    // 但使用该文件前应该通过singletonServerInfoFileLock()获得锁来锁定对该文件的访问逻辑
    fs::path singletonServerInfoFile() const;

private:
    fs::path workDir_;
};


#pragma once


namespace constants
{

// 向文件写服务端信息时的重复次数
// 完整的服务端信息表示为一行,写入的次数即行数
// 服务端向文件写信息时重复写若干次
// 客户端读取服务端信息时需要确保行数足够且每行内容一致
// 这样做的目的是确保信息完整
static const int ServerInfoRepeatCount = 3;

}

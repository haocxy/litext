# notesharp

## 项目介绍

专为超大文本文档设计的纯文本编辑器，快速打开GB级别的超大文本文件。

程序轻量，部署容易，所有可执行的逻辑均由 C 或 C++ 开发，只需要一个很小的 zip 文件或安装包。

运行轻量，操作文件时不会把整个文件加载到内存中，单个文件的内存使用是常数级别的，不用担心大文件拖慢计算机。

充分发挥计算机性能，计算机越好，程序的表现就越好。

多平台支持。

## 技术特点

使用现代化的编码方式，C++语言版本使用C++17，辅助库以boost为主，使用新特性、新思想简化开发。

跨平台，开发中全程都同时在 Windows 10 和 Ubuntu 做构建和执行（虽然我很想用 “测试” 这个词，但现在还没有一套自动化的测试方式）。

## 对 Qt 的使用情况

**用了Qt，但随时可以换掉。**

尤其是**没有**使用诸如 QTextEdit 之类的组件，毕竟这些功能正是我要实现的关键。

首先用一句话概括对第三方库各种库的使用思路：**用组件，不用框架。**

的确，现阶段确实使用了很多 Qt 中的功能，但是，这仅仅是因为 Qt 中几乎有 GUI 程序所需的绝大多数组件，它门可以加速开发。

如果我要开发的是一个简单的对性能要求不高的程序，我会毫不犹豫地选择 Qt。但是在一个没有准确的完整预期的长期项目中，我不会选择 Qt 之类的开箱即用的库。这是因为，这些库会对开发者的思路做出限制，例如 Qt 中的信号系统，或者线程间的通信方式，等等。这些东西很诱人，它门足够便利，也能够在大多数时候提供不错的性能。但是如果严重地依赖它门，会在未来的某一天突然发现自己想要实现的某个新功能无法和这套东西兼容，实际上我在开发的过程中已经遇到了这种情况，例如 QFont 及其系列模块在多线程环境中的糟糕表现。

具体来说，目前我**用到**了 Qt 中的这些东西：

* 字符集转换：字符集转换虽然是本程序最关键的逻辑之一，但它足够独立。
  * 未来，直接把字符集的映射关系以源代码的形式直接内置到项目中，并且直接针对本项目设计逻辑和数据结构，以达到更高的性能。
* 边缘 GUI 组件：例如窗口、菜单、工具栏、滚动条等随处可见的基础 GUI 组件，它门可以在任何 GUI 工具包中被找到。
  * 未来，用更加轻量的控件替换 Qt 提供的控件，例如直接用 Windows 提供的原生控件，这能有效地提升启动速度。
* 2D 绘图：只用于绘制简单的图元和位图，而字符的绘制是直接绘制由 FreeType 获得位图（Qt 的字符绘制有各种问题，无法满足我的目标）。
  * 未来，用更适合我的项目的 2D 绘图库替代 Qt 的绘图。这部分我大概不会自己实现，使用成熟的库可以确保绘图在任何平台上都有很好的效率。我调查了现有的主流的 2D 绘图库，要么太重量级，要么不能轻松地支持跨平台开发。
* 其它暂时没想起来的。


## 构建完成的包

TODO

## 开发环境搭建

TODO


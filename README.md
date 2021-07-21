# notesharp

## 项目介绍

专为超大文本文档设计的纯文本编辑器，**瞬间打开**任何尺寸的文本文件。操作文件时不会把整个文件加载到内存中，计算机的内存不会被大文件占用，也不会因为内存被占用过多被拖慢。

对 “瞬间打开” 的说明：

* 瞬间打开**不等于**瞬间整个加载完成整个文件，巨大的IO操作肯定是需要较长时间的。
* 瞬间打开是指：尽可能快地把加载完成的部分展示出来，且不论文档多大都不会使界面发生卡顿。
* 如果是首次打开大文件或修改后再次打开大文件，依据文件大小可能会有较长的处理时间，但处理过程会及时体现在界面上，界面也不会卡顿。
* 如果某个文件被打开后，在没有被修改的情况下再次打开，由于为文件建立了数据库，打开速度会极大地提升。

## 技术特点

全部模块由C++开发，直接运行于操作系统，不需要虚拟机或解释器。

使用现代化的编码方式，C++语言版本使用C++17，辅助库以boost为主，使用新特性、新思想简化开发。

跨平台，开发的每一步都同时在 Windows 10 和 Ubuntu 做构建和执行，构建工具链使用 CMake，标准化 CMake 流程，无需阅读文档即可构建或开发。

和文档操作有关的关键逻辑不依赖特定的GUI库和特定平台的库，尽量使用平台无关的库。

对于特定的GUI库，只做轻量级使用，例如窗口创建、没有复杂逻辑的诸如菜单或按钮等控件。而复杂的逻辑，例如本程序核心的文本操作区域的展示，则不会使用特定GUI库的功能。

为了加快迭代，目前使用了Qt库的一些功能，可以分为几种情况：

* 和GUI有关的独立性较强的逻辑：暂时使用了Qt的功能。例如字型数据的获取，这些功能的实现在将来是很容易被随时替换掉的。
* 和GUI有关的文本展示和操作的关键的逻辑：使用Qt但和Qt严格划分界限。例如文本编辑器最关键的文本展示和操作的区域（不包括周围的例如按钮或滚动条等控件），只用到了Qt的单个文字绘制能力，而其它文本操作逻辑例如视口内各个文字的展示位置以则是由自己实现的。
* 和GUI无关的功能：接口和Qt无关，但内部实现可能少部分使用Qt。提供和Qt无关的功能接口，内部实现目前有很少一部分使用了Qt，例如文本编码转换。


## 构建完成的包

TODO

## 开发环境搭建

TODO


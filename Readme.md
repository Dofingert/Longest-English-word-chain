## 结对编程项目：单词链求解器

Longest-English-word-chain 是一个使用 c++ 编写，CMake 管理的英文单词链的求解器。

程序提供两种使用方式，分别是命令行接口使用和GUI使用。

命令行程序位于仓库 `/bin` 目录，提供动态链接计算核心版本(WordList.exe)与静态链接计算核心版本(WordList_static_link.exe)。

命令行支持如下参数：

- `-n [text file]`：求解单词链总数
- `-w [text file]`：求解最长单词链
- `-c [text file]`：求解总字符数最多的单词链
- `-h [letter]`：指定单词链首字母
- `-t [letter]`：指定单词链尾字母
- `-j [letter]`：指定不允许出现的首字母
- `-r`：允许隐含单词环

GUI 程序位于仓库 `/guibin/gui.exe` ，直接运行即可启动 gui，与计算核心进行交互


## 目录结构

```plaintext
.
├── CMakeLists.txt           // 项目主 Cmake 文件
│
├── src/core                 // 计算核心源码
│   └── ...
│
├── src/cli                  // 命令行交互程序源码
│   └── ...
│
├── gui                      // 图形化交互 Qt 工程源码（不依赖父级目录）
│   └── ...
│
├── include                  // 工程文件头
│   └── *.h
│
├── src/matching_test        // 对拍测试源码
│   └── ...
│
├── src/publib               // 公用库源码
│   └── ...
│
├── bin                      // 命令行二进制发行文件
│   ├── lib*.dll                 // 其他依赖库
│   ├── libcore.dll              // 计算核心库
│   ├── WordList.exe             // 动态链接执行程序（依赖libcore.dll）
│   └── Wordlist_static_link.exe // 静态链接执行程序（不依赖libcore.dll）
│
├── guibin                   // GUI二进制发行文件
│   ├── gui.exe                  // 可执行 GUI 文件
│   ├── lib*.dll                 // Qt 依赖库
│   └── core.dll                 // 计算核心库（同 bin 目录下的）
│
└── test                // 计算核心单元测试模块
    ├── ...                      // 单元测试源码
    └── test_point               // 单元测试 手工构造数据点
  

```
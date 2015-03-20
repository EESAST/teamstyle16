# Deep Blue 选手文档

## VS / VC++ 玩家看这里看这里 ！！！

构建项目 **只需要双击 `vs.bat`** !!!

不用下 CMake ！！！

不用下 Boost ！！！

简单愉快有木有 ！！！

傻瓜式配置有木有 ！！！

简洁易懂有木有有木有 ！！！！！！

## ALL_BUILD 无法运行？

运行 / 调试 AI 时，请从 ALL_BUILD 项目切换到 ai 项目，方法如下：
* VS: 在解决方案资源管理器找到 ai 项目，右键 => 设为启动项目。此时列表中 ai 变为黑体字说明切换成功
* Xcode: 点击左上方 Build/Stop 按钮右方的 ALL_BUILD，从下拉菜单里选择 ai

## 命令行上的 ai_battle/debugger 不会玩？
使用 `easy_ai_battle` / `easy_debugger` 文件夹下的 `.bat` 文件

默认使用 `1.map` 作为地图，`ai0.exe` & `ai1.exe` 作为双方 AI，保存回放文件为 `replay.battle`

例如，要使用 easy_debugger 调试，则要将你想用的地图 & 对手AI复制进 easy_debugger 文件夹，
并将地图改名成 `1.map`，对手AI改名成 `ai1.exe`，然后双击 `easy_debugger.bat`，再从VS里开始调试

带 `_verbose` 后缀的版本会输出更详细的平台调试信息


## API
暂时只提供了 C++ 接口

C/C++ 程序请写在 ai.cpp 内

平台每隔一定时间（默认1s）向AI发送当前状态

AI则随时可以向平台发送指令

指令产生方式为 **函数调用**

详见 `basic.h`

## 构建项目
依赖：
* 标准 C++ 编译器
* Boost库

### Windows
请安装合适版本的 VC++ / VS

支持版本：
* 8.0 (VC++/VS 2005)
* 9.0 (VC++/VS 2008)
* 10.0 (VC++/VS 2010)
* 11.0 (VC++/VS 2012)
* 12.0 (VC++/VS 2013)

其中 12.0 有 32/64 位版本，其他只有 32 位版本

双击 `vs.bat` 生成 VS 项目文件


#### 不行，我就是要用 Cygwin / MinGW / whatever

见 “用 CMake 手动构建项目” 一节

### Mac
1. 打开终端

2. 安装 Command Line Tools

    一些和开发相关的重要工具

    *（如果你安装过 Xcode，则可以跳过此步）*

        xcode-select --install

    若未安装 Command Line Tools，则会弹出对话框

    此时可以选择安装 Xcode，也可以只安装 Command Line Tools

3. 安装 Homebrew

    OS X 的一个包管理工具
    ```bash
    ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
    ```

4. 安装 Boost 库

    ```bash
    brew install boost
    ```

5. 使用 Xcode/Makefile 管理项目

### Linux
1. 用你最喜欢的包管理系统解决依赖
2. 用 CMake 构建项目


### 用 CMake 手动构建项目
1. 安装 Boost 1.55 或更高版本
2. 安装 CMake
3. 在选手文件夹下创建 build 文件夹
4. 在 build 文件夹下 cmake .. （用 -G 指定生成器）

## Debug

1. 运行 debugger

    基本用法：

        debugger [-s <file>] map opponent

    * -s: 保存存档的位置
    * map: 地图（.map 或 .battle 文件）
    * opponent: 敌方 AI（作为 1号AI）

    程序打开后会等待待测 AI 运行

2. 用你喜欢的姿势开始调试

## 其他问题

请联系李思涵

18800183697 / lisihan969@gmail.com

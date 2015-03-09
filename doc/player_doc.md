# Deep Blue 选手文档

## API
平台每隔一定时间（默认1s）向AI发送当前状态

AI则随时可以向平台发送指令

指令发送方式为 **函数调用**

详见 `basic.h`

## 构建项目
### Windows
你面前有两种选择
1. 使用特定的 MSVC 版本

    支持的版本有
    * 8.0
    * 9.0
    * 10.0
    * 11.0
    * 12.0-32位
    * 12.0-64位

2. 不行，我就是要用 Cygwin / MinGW / whatever



### Mac
1. 打开终端

2. 安装 Command Line Tools

    一些和开发相关的重要工具

    *（如果你安装过 Xcode，则可以跳过此步）*

        xcode-select --install

    若未安装 Command Line Tools，则会弹出对话框

    此时可以选择安装 Xcode，也可以只安装 Command Line Tools

3. 安装 Homebrew

    Homebrew 是 OS X 的包管理工具
    ```bash
    ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
    ```

4. 安装 Boost 库

    编译依赖

    ```bash
    brew install boost
    ```

### Linux

### 用 CMake 手动构建项目

## Debug

1. 运行 debugger

        debugger [-h] [-s <file>] [-t TIMEOUT] [-v] map opponent

2. 用你喜欢的姿势调试

# BeforeEngine

一个基于 DirectX 12 的轻量级图形引擎项目，用于学习和理解现代图形渲染管线。

## 施工中
### 学习知乎文章
[《DirectX12学习专栏》](https://www.zhihu.com/column/c_1268850893845594112)
* 进度记录
  + ~~DX12初始化篇：初始化基础模块~~
  + ~~DX12初始化篇：绘制命令~~
  + ~~DX12初始化篇：添加计时器~~
  + ~~DX12初始化篇：重构代码~~
  + (`学习中`)DX12绘制篇：顶点、索引、常量缓冲区 https://zhuanlan.zhihu.com/p/136753567
    - 目前CreateDefaultBuffer函数还有编译问题

## 项目简介

BeforeEngine 是一个学习型的 DirectX 12 图形引擎，旨在展示如何从零开始构建一个基本的图形渲染框架。该项目实现了 DirectX 12 的核心功能，包括设备初始化、命令队列管理、交换链、渲染目标视图、深度模板视图等基础图形组件。

## 主要特性

- **DX12 核心功能封装**
  - 设备和工厂创建
  - 命令队列、命令分配器、命令列表管理
  - 交换链双缓冲渲染
  - 渲染目标视图 (RTV) 和深度模板视图 (DSV)
  - 资源屏障管理
  - GPU-CPU 同步机制 (Fence)

- **渲染管线**
  - 视口和剪裁矩形设置
  - 顶点缓冲和索引缓冲
  - 上传缓冲和默认缓冲管理
  - MSAA (多重采样抗锯齿) 支持

- **工具类**
  - `GameTimer`: 游戏计时器，提供帧时间统计和 FPS 显示
  - `UploadBufferResource`: 模板化的上传缓冲管理类
  - `DxException`: DirectX 异常处理和错误报告

## 项目结构

```
BeforeEngine/
├── Source/
│   ├── BeforeEngine.h         # 核心引擎类和图形层定义
│   ├── BeforeEngine.cpp       # 引擎实现和 DX12 初始化
│   ├── main.cpp               # WinMain 入口和游戏循环
│   ├── DX12Util.h             # DX12 依赖和工具宏定义
│   ├── DxException.h/cpp      # 异常处理
│   ├── GameTimer.h/cpp        # 游戏计时器
│   └── d3dx12_utils/          # D3D12 官方工具库
├── CMakeLists.txt             # CMake 构建配置
└── readme.md                  # 项目说明
```

## 技术栈

- **API**: DirectX 12 (Feature Level 12.0)
- **语言**: C++17
- **构建工具**: CMake 3.26+
- **平台**: Windows (使用 DirectX 12 需要 Windows 10 或更高版本)
- **数学库**: DirectXMath

## 快速开始

### 环境要求

- Windows 10/11
- Visual Studio 2019 或更高版本
- Windows 10 SDK (包含 DirectX 12 头文件)
- CMake 3.26 或更高版本

### 编译步骤

1. 克隆或下载项目到本地

2. 创建构建目录并生成项目：
```bash
mkdir build
cd build
cmake ..
```

3. 使用 CMake 生成 Visual Studio 项目后，打开 `BeforeEngine.sln` 进行编译

4. 运行生成的可执行文件

## 核心类说明

### BeforeEngine
主引擎类，负责初始化图形设备和场景渲染。
- `InitGraphics()`: 初始化 DX12 管线
- `Render()`: 渲染场景
- `SetupModels()`: 设置模型数据

### DX12Graphics
DirectX 12 图形层的封装，管理所有 DX12 组件。
- 设备、命令对象、交换链创建
- 描述符堆管理 (RTV, DSV)
- 资源状态转换和同步
- 默认缓冲创建

### GameTimer
高精度游戏计时器，基于 Windows 性能计数器。
- `tick()`: 每帧更新计时器
- `totalTime()`: 获取总运行时间
- `deltaTime()`: 获取帧间隔时间

## 渲染流程

```
初始化阶段:
  创建设备 → 创建命令对象 → 创建交换链
  → 创建描述符堆 → 创建 RTV/DSV → 设置视口和剪裁

渲染循环:
  重置命令列表 → 转换资源状态 → 清除渲染目标
  → 设置视口 → 渲染 → 转换资源状态 → 提交命令
  → 呈现 → GPU-CPU 同步
```

## 学习目标

这个项目适合以下学习者：
- 想要学习 DirectX 12 基础 API
- 理解现代图形渲染管线的工作原理
- 学习 GPU-CPU 同步和资源管理
- 掌握 DirectX 12 的调试技巧

## 许可证

本项目采用开源许可证，详见 LICENSE 文件。

## 贡献

欢迎提交 Issue 和 Pull Request！

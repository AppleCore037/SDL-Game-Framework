# SDL-Game-Framework
依托SDL3的游戏开发框架 / Game development framework based on SDL3

注意： 需要配置SDL3相关文件、需要C++17及以上版本

使用方法： 将“FlexCore.h”添加到VS项目下

=======================================================================================================================================================

Note: SDL3 related files need to be configured, C++ 17 and above are required, and all features are consolidated into a single header file compared to version 1.0

How to Use: Add "FlexCore.h" under the VS project


=========================================================== 概  述 =============== Overview ===============================================================

基础变量与属性
定义了全局窗口、渲染器和文本引擎指针

常用数学常量(如圆周率PAI)和颜色常量(红、蓝、绿等)

SDL初始化标志组合

基础函数
SDL初始化相关函数(FCE_init)

窗口、渲染器和文本引擎的初始化函数

资源清理函数(FCE_Clean_Up)

基本图形绘制函数(圆形、实心圆等)

基础类型
Size: 表示大小的结构体

Vector2: 2D向量类，包含常用向量运算

RenderLayer: 渲染层级枚举(背景、游戏对象、特效等)

CollisionLayer: 碰撞层级枚举(玩家、敌人、地图等)，支持位运算

基础工具类
custom_runtime_error: 自定义运行时错误处理

Random: 随机数生成器

Clock: 时钟类，管理帧率和时间缩放

Timer: 计时器类，支持回调函数

maths命名空间: 数学工具函数(角度弧度转换、插值等)

基础元素
Camera: 摄像机类，支持平滑跟随、震动、缩放等功能

Animation: 序列帧动画类，支持循环播放和回调

CollisionBox: 碰撞盒类，管理碰撞检测

Sprite: 精灵基类，提供位置、速度等基础属性

Button: 按钮类(继承自Sprite)，支持悬停、点击效果

Scene: 场景基类，管理精灵池

StateNode: 状态节点基类

行为控制器
AnimationPlayer: 动画播放器，管理多个动画

StateMachine: 状态机，管理状态切换

管理器
ResourcesManager: 资源管理器(单例)，加载和管理纹理、音频、字体

SceneManager: 场景管理器(单例)，切换和管理游戏场景

CollisionManager: 碰撞管理器(单例)，处理碰撞检测和调试

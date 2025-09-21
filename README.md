# FlexCore
依托SDL3的游戏开发框架 / Game development framework based on SDL3

注意： 需要配置SDL3相关文件、需要C++17及以上版本

使用方法： 将“FlexCore.h”添加到VS项目下

=======================================================================

Note: SDL3 related files need to be configured, C++ 17 and above are required, and all features are consolidated into a single header file compared to version 1.0

How to Use: Add "FlexCore.h" under the VS project



==================== 概  述 =============== Overview ==================

📦 核心组件

基础类型

Vector2/Point：2D 向量和坐标点操作

Size：尺寸表示

Atlas：图集管理，支持多帧纹理加载

StateNode：状态节点基类

工具类

custom_error：自定义错误处理

Random：随机数生成器

Clock：全局游戏时钟，支持时间缩放和 FPS 控制

Data：数据读写器，支持多种数据类型的持久化存储

maths：数学工具函数，包括几何绘制、插值、摆动等

游戏元素

Timer：计时器，支持单次和循环触发

Camera2D：2D 摄像机，支持平滑跟随、抖动效果和坐标转换

Animation：序列帧动画，支持循环播放和回调

CollisionBox：碰撞箱，支持多层碰撞检测

Sprite：精灵基类，所有游戏对象的基类

Label：文本标签

Button：UI 按钮，支持悬停和点击效果

行为控制器

AnimationPlayer：动画播放器，管理多个动画状态

StateMachine：状态机，管理游戏对象的状态转换

管理器

ResourcesManager：资源管理器，统一加载和管理纹理、音频、字体等资源

SceneManager：场景管理器，管理游戏场景的切换和更新

CollisionManager：碰撞管理器，处理游戏中的碰撞检测

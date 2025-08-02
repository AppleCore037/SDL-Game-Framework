# FlexCore
依托SDL3的游戏开发框架 / Game development framework based on SDL3

注意： 需要配置SDL3相关文件、需要C++17及以上版本

使用方法： 将“FlexCore.h”添加到VS项目下

=======================================================================

Note: SDL3 related files need to be configured, C++ 17 and above are required, and all features are consolidated into a single header file compared to version 1.0

How to Use: Add "FlexCore.h" under the VS project



==================== 概  述 =============== Overview ==================

1. 基础变量属性

全局核心对象：Main_Window：主窗口指针、Main_Renderer：主渲染器指针、Main_TextEngine：文字引擎指针

常用常量：颜色常量（红/蓝/绿等）、圆周率PAI、SDL初始化标志组合

====================================================================

2. 基础函数

图形初始化：

Init_Graphic()：初始化窗口/渲染器/音频/文字引擎

Clean_Up()：释放所有全局资源

事件处理：

Get_EventName()：将事件转换为可读字符串

====================================================================

3. 基础类型

Size：宽高存储

Vector2：2D向量运算

Atlas：图集管理

RenderLayer：渲染层级

CollisionLayer：碰撞层级（支持位运算）

====================================================================

4. 基础工具

错误处理：custom_error：自定义异常类

随机数：Random：随机数生成器

时间管理：Clock：帧率控制、Timer：定时回调

数学工具：角度/弧度转换、插值计算、几何图形绘制

====================================================================

5. 基础元素

摄像机：

跟随模式（静态/平滑）

抖动效果

坐标转换

动画系统：

序列帧管理

播放控制

碰撞系统：

碰撞箱管理

层级检测

====================================================================

6. 行为控制器

动画控制：

AnimationPlayer：多动画管理

状态管理：

StateMachine：状态切换

====================================================================

7. 管理器

资源管理：

ResourcesManager：统一加载/获取资源

场景管理：

SceneManager：场景切换控制

碰撞管理：

CollisionManager：碰撞检测

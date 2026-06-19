# FlexCore 游戏引擎
FlexCore 是一个基于 SDL3 的轻量级 2D 游戏引擎，采用 C++20 模块化设计，提供了完整的游戏开发基础设施。

✨ 特性
模块化设计 - 基于 C++20 Module 构建，依赖清晰，易于扩展

渲染系统 - 支持摄像机、多渲染层、纹理图集、动画系统

物理碰撞 - SAT 分离轴碰撞检测，支持碰撞层掩码

UI 系统 - 内置按钮、标签、滑块等基础 UI 组件

资源管理 - 自动加载纹理、音频、字体资源

场景管理 - 场景切换、精灵管理、注册表查找

工具集 - 数据序列化、随机数生成、时钟控制、数学工具

📦 模块结构
FlexCore
├── FCE_BaseSetup    - 基础配置（窗口、渲染器、颜色、枚举）
├── FCE_BaseType     - 基础类型（Vector2、Rect、Atlas、Text、CollisionInfo）
├── FCE_Utils        - 工具类（Clock、Random、Data、数学函数）
├── FCE_Component    - 组件（Timer、CollisionBox、Animation、StateMachine）
├── FCE_Graphic      - 图形（Camera、Renderer）
├── FCE_UI           - UI 组件（IButton、ILabel、ISlider）
├── FCE_Element      - 游戏元素（Sprite、SpriteGroup、Scene）
├── FCE_Handler      - 管理器（ResourcesManager、CollisionManager、SceneManager）
└── FlexCore         - 引擎入口（Game 类）

📋 前置依赖
C++20 编译器（支持 Module）

SDL3 - 核心库

SDL3_image - 图像加载

SDL3_mixer - 音频播放

SDL3_ttf - 字体渲染

🎮 内置功能
功能	说明

动画系统	支持序列帧动画、循环/单次播放、回调

状态机	状态节点管理、切换、生命周期回调

计时器	单次/循环触发、暂停/继续、回调函数

数据持久化	键值对序列化存储（.kvp 格式）

随机数	整数/浮点数区间随机

时钟控制	FPS 限制、时间缩放、DeltaTime

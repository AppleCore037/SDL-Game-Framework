#pragma once

#include "fce_base.h"
#include "fce_property.h"
#include "fce_utils.h"

#include <functional>

namespace fce
{
	// 计时器
	class Timer
	{
	public:
		Timer() = default;
		~Timer() = default;

		// 重启计时器
		void restart();

		// 设置等待时间
		void set_wait_time(float val);

		// 设置是否单次触发（默认false）
		void set_one_shot(bool flag);

		// 设置回调函数
		void set_on_timeout(std::function<void()> timeout_callback);

		// 暂停
		void pause();

		// 继续
		void resume();

		// 更新计时器
		void on_update(float delta);

	private:
		float pass_time = 0;				// 经过时间
		float wait_time = 0;				// 等待时间
		bool paused = false;				// 是否暂停
		bool shotted = false;				// 是否已经触发
		bool one_shot = false;				// 是否单词触发
		std::function<void()> on_timeout;	// 回调函数
	};

	// 2D摄像机
	class Camera2D
	{
	public:
		// 跟随方式
		enum FollowStyle
		{
			None = 0,					// 无跟随
			Static_Follow = 1 << 0,		// 静态跟随
			Smooth_Follow = 1 << 1,		// 平滑跟随
			Only_X = 1 << 2,			// 仅跟随X轴
			Only_Y = 1 << 3				// 仅跟随Y轴
		};

	public:
		Camera2D();
		Camera2D(const Point& pos);

		~Camera2D() = default;

		// 获取摄像机世界坐标(中心点)
		const Point& get_position() const;

		// 设置摄像机世界坐标(中心点)
		void set_position(const Point& pos);

		// 设置摄像机缩放
		void set_zoom(float scale);

		// 获取摄像机缩放
		float get_zoom() const;

		// 重置摄像机
		void reset();

		// 抖动摄像机
		void shake(float strength, float duration);

		// 渲染纹理
		void render_texture(SDL_Texture* texture, const SDL_FRect* rect_src, const SDL_FRect* rect_dst,
			double angle, const SDL_FPoint center_anchor = { 0.0f, 0.0f }, bool is_flip = false) const;

		// 渲染文字
		void render_text(const Point& pos, TTF_Font* font, SDL_Color color,
			float ptsize, const std::string& info) const;

		// 跟随角色
		void look_at(const Point& target_pos, int style);

		// 窗口坐标转世界坐标
		Point screen_to_world(const Point& screen_pos) const;

		// 世界坐标转窗口坐标
		Point world_to_screen(const Point& world_pos) const;

		// 更新摄像机
		void on_update(float delta);

	private:
		// 获取屏幕中心点
		Point get_screen_center() const;

	private:
		Point position;				// 摄像机最终世界坐标
		Point shake_position;		// 抖动位置
		Point base_position;		// 基础位置

		Timer timer_shake;					// 抖动计时器
		bool is_shaking = false;			// 是否抖动
		float shaking_strength = 0;			// 抖动幅度
		float smooth_strength = 0;			// 平滑幅度
		float zoom = 1.0f;					// 摄像机缩放

		const float SMOOTH_FACTOR = 1.5f;	// 平滑系数常量
	};

	// 图形渲染器
	class ShapeMaker
	{
	public:
		// 用指定颜色填充屏幕
		static void fill(SDL_Color color);

		// 绘制线条
		static void render_line(const Camera2D& camera, const Point& begin, 
			const Point& end, SDL_Color color);

		// 绘制锚点为左上角的矩形
		static void render_TopLeftRect(const Camera2D& camera, const Point& pos, 
			const Size& size, SDL_Color color, bool is_filled = false);

		// 绘制锚点为中心的矩形
		static void render_CenterRect(const Camera2D& camera, const Point& pos, 
			const Size& size, SDL_Color color, bool is_filled = false);

		// 绘制锚点为左上角的圆形
		static void render_TopLeftCircle(const Camera2D& camera, const Point& pos, 
			float radius, SDL_Color color, bool is_filled = false);

		// 绘制锚点为中心的圆形
		static void render_CenterCircle(const Camera2D& camera, const Point& pos,
			float radius, SDL_Color color, bool is_filled = false);

	private:
		ShapeMaker() = default;
		~ShapeMaker() = default;
	};

	// 序列帧动画
	class Animation
	{
	public:
		Animation();
		Animation(SDL_Texture* texture, int num_h);
		Animation(Atlas* atlas);

		~Animation() = default;

		// 重置动画
		void reset();

		// 设置动画世界坐标
		void set_position(const Point& pos);

		// 设置动画方向
		void set_rotation(float angle);

		// 设置动画是否循环（默认true）
		void set_loop(bool is_loop);

		// 设置动画帧间隔（默认0.1）
		void set_interval(float interval);

		// 设置动画是否翻转（默认false）
		void set_flip(bool flag);

		// 设置动画回调函数
		void set_on_finished(std::function<void()> callback);

		// 更新动画
		void on_update(float delta);

		// 设置动画中心点（默认为纹理中心点，参数范围在0.0~1.0）
		void set_center(float anchor_x, float anchor_y);

		// 添加序列帧
		void add_frame(Atlas* atlas);

		// 添加序列帧
		void add_frame(SDL_Texture* texture, int num_h);

		// 渲染动画
		void on_render(const Camera2D& camera) const;

	private:
		// 序列帧结构体
		struct Frame
		{
			SDL_FRect rect_src;
			SDL_Texture* texture = nullptr;

			Frame() = default;
			Frame(SDL_Texture* texture, const SDL_FRect& rect_src)
				: texture(texture), rect_src(rect_src) {}

			~Frame() = default;
		};

	private:
		Point position;						// 世界坐标
		double angle = 0;					// 角度
		SDL_FPoint center = { 0.5f, 0.5f };	// 中心点
		bool is_flip = false;				// 是否反转

		Timer timer;						// 内置计时器
		bool is_loop = true;				// 是否循环
		size_t idx_frame = 0;				// 当前帧索引
		std::vector<Frame> frame_list;		// 序列帧列表
		std::function<void()> on_finished;	// 动画播放完毕回调函数
	};

	// 碰撞箱
	class CollisionManager;
	class CollisionBox
	{
		friend class CollisionManager;
	public:
		// 设置是否启用碰撞（默认true）
		void set_enabled(bool flag) { this->enabled = flag; }

		// 设置自身碰撞层
		void set_layer_src(CollisionLayer layer) { this->layer_src = layer; }

		// 设置目标碰撞层
		void set_layer_dst(CollisionLayer layer) { this->layer_dst = layer; }

		// 设置碰撞回调函数
		void set_on_collide(std::function<void(CollisionLayer)> callback) { this->on_collide = callback; }

		// 设置碰撞箱大小
		void set_size(const Size& size) { this->size = size; }

		// 获取碰撞箱大小
		const Size& get_size() const { return this->size; }

		// 设置碰撞箱世界位置
		void set_position(const Point& pos) { this->position = pos; }

	private:
		Size size = { 0, 0 };								// 碰撞箱大小
		Point position;										// 碰撞箱世界坐标
		bool enabled = true;								// 是否启用碰撞检测
		std::function<void(CollisionLayer)> on_collide;		// 碰撞回调函数
		CollisionLayer layer_src = CollisionLayer::None;	// 自身碰撞层
		CollisionLayer layer_dst = CollisionLayer::None;	// 目标碰撞层

	private:
		CollisionBox() = default;
		~CollisionBox() = default;
	};

	// 动画播放器
	class AnimationPlayer
	{
	public:
		AnimationPlayer() = default;
		~AnimationPlayer();

		// 注册动画
		void register_animation(const std::string& name, Animation* anim);

		// 切换动画
		void switch_to(const std::string& name);

		// 设置当前动画，并从头播放
		void set_animation(const std::string& name);

		// 获取当前动画
		Animation* get_current();

		// 更新动画
		void on_update(float delta);

		// 渲染动画
		void on_render(const Camera2D& camera);

	private:
		std::unordered_map<std::string, Animation*> animation_pool;		// 动画池
		Animation* current_animation = nullptr;							// 当前动画
		std::string error_title = u8"Animation Player Error";			// 错误标题
	};

	// 状态机
	class StateMachine
	{
	public:
		StateMachine() = default;
		~StateMachine();

		// 更新状态机
		void on_update(float delta);

		// 设置初状态节点（用于初始化）
		void set_entry(const std::string& name);

		// 切换状态节点
		void switch_to(const std::string& name);

		// 注册状态节点
		void register_state(const std::string& name, StateNode* state);

	private:
		bool need_init = true;									// 是否需要初始化状态机
		StateNode* current_state = nullptr;						// 当前状态节点
		std::unordered_map<std::string, StateNode*> state_pool;	// 状态池
	};
};
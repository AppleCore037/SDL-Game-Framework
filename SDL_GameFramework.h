#pragma once

// 标准库
#include <chrono>
#include <filesystem>
#include <functional>
#include <memory>
#include <random>
#include <string>
#include <thread>
#include <unordered_map>

// 第三方库（SDL）
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>

// ==============================================================================================
// 				基础变量属性							Base Variables Attributes
// ==============================================================================================

inline SDL_Window* Main_Window = nullptr;				// 主窗口
inline SDL_Renderer* Main_Renderer = nullptr;			// 主渲染器
inline TTF_TextEngine* Main_TextEngine = nullptr;		// 主文字引擎

constexpr float PAI = 3.14159265f;							// 圆周率
constexpr SDL_Color Color_Red = { 255, 0, 0, 255 };			// 红
constexpr SDL_Color Color_Blue = { 0, 0, 255, 255 };		// 蓝
constexpr SDL_Color Color_Green = { 0, 255, 0, 255 };		// 绿
constexpr SDL_Color Color_White = { 255, 255, 255, 255 };	// 白
constexpr SDL_Color Color_Black = { 0, 0, 0, 255 };			// 黑
constexpr SDL_Color Color_Gray = { 128, 128, 128, 255 };	// 灰

constexpr SDL_InitFlags SDL_INIT_EVERYTHING = (SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC | SDL_INIT_GAMEPAD | SDL_INIT_SENSOR);
constexpr MIX_InitFlags MIX_INIT_EVERYTHING = (MIX_INIT_MP3 | MIX_INIT_FLAC | MIX_INIT_MID | MIX_INIT_MOD | MIX_INIT_OGG | MIX_INIT_OPUS | MIX_INIT_WAVPACK);

// ==============================================================================================
//				基 础 函 数						Base	Functions
// ==============================================================================================

// 初始化内置窗口
inline void Framework_Init_Window(const char* title, int width, int height, SDL_WindowFlags flags)
{
	if (Main_Window) return;	// 如果窗口已存在则不再创建
	Main_Window = SDL_CreateWindow(title, width, height, flags);
	if (!Main_Window)
		throw std::runtime_error(u8"Failed to create Main_Window!");
}

// 初始化内置渲染器
inline void Framework_Init_Renderer(const char* name = nullptr)
{
	if (Main_Renderer) return;	// 如果渲染器已存在则不再创建
	Main_Renderer = SDL_CreateRenderer(Main_Window, name);
	if (!Main_Renderer)
		throw std::runtime_error(u8"Failed to create Main_Renderer!");
}

// 初始化内置文字引擎
inline void Framework_Init_TextEngine()
{
	if (Main_TextEngine) return; // 如果文字引擎已存在则不再创建
	Main_TextEngine = TTF_CreateRendererTextEngine(Main_Renderer);
	if (!Main_TextEngine)
		throw std::runtime_error(u8"Failed to create Main_TextEngine!");
}

// 释放框架内置资源 [包含：Main_TextEngine、Main_Renderer、Main_Window]
inline void Framework_Clean_Up()
{
	if (Main_TextEngine)
	{
		TTF_DestroyRendererTextEngine(Main_TextEngine);
		Main_TextEngine = nullptr;
	}
	if (Main_Renderer)
	{
		SDL_DestroyRenderer(Main_Renderer);
		Main_Renderer = nullptr;
	}
	if (Main_Window)
	{
		SDL_DestroyWindow(Main_Window);
		Main_Window = nullptr;
	}
}

// 绘制空心圆
inline void Framework_Draw_Circle(SDL_Renderer* renderer, float centerX, float centerY, float radius)
{
	float x = radius; float y = 0; float err = 0;
	while (x >= y)
	{
		SDL_RenderPoint(renderer, centerX + x, centerY + y);
		SDL_RenderPoint(renderer, centerX + y, centerY + x);
		SDL_RenderPoint(renderer, centerX - y, centerY + x);
		SDL_RenderPoint(renderer, centerX - x, centerY + y);
		SDL_RenderPoint(renderer, centerX - x, centerY - y);
		SDL_RenderPoint(renderer, centerX - y, centerY - x);
		SDL_RenderPoint(renderer, centerX + y, centerY - x);
		SDL_RenderPoint(renderer, centerX + x, centerY - y);

		if (err <= 0) { y += 1; err += 2 * y + 1; }
		if (err > 0) { x -= 1; err -= 2 * x + 1; }
	}
}

// 绘制实心圆
inline void Framework_Draw_FilledCircle(SDL_Renderer* renderer, float centerX, float centerY, float radius)
{
	for (float y = -radius; y <= radius; y += 1.0f)
	{
		float x = sqrtf(radius * radius - y * y);
		SDL_RenderLine(renderer, centerX - x, centerY + y, centerX + x, centerY + y);
	}
}

// ==============================================================================================
//				基 础 工 具						Base	Kits
// ==============================================================================================

// 大小
struct Size
{
	Size() = default;
	~Size() = default;
	Size(float w, float h) :
		width(w), height(h) {}

	float width;
	float height;
};

// 二维向量
class Vector2
{
public:
	float x = 0.0f;
	float y = 0.0f;

public:
	Vector2() = default;

	~Vector2() = default;

	Vector2(float x, float y) : x(x), y(y) {}

	Vector2 operator+(const Vector2& vct2) const { return Vector2(x + vct2.x, y + vct2.y); }

	Vector2 operator-(const Vector2& vct2) { return Vector2(x - vct2.x, y - vct2.y); }

	Vector2 operator*(float val) { return Vector2(x * val, y * val); }

	Vector2 operator/(float val) { return Vector2(x / val, y / val); }

	float operator*(const Vector2& vct2) { return x * vct2.x + y * vct2.y; }

	void operator+=(const Vector2& vct2) { x += vct2.x; y += vct2.y; }

	void operator-=(const Vector2& vct2) { x -= vct2.x; y -= vct2.y; }

	void operator/=(float val) { x /= val; y /= val; }

	void operator*=(float val) { x *= val; y *= val; }

	// 获取长度
	float length() const { return sqrt(x * x + y * y); }

	// 标准化
	Vector2 normalize() const
	{
		float len = this->length();

		if (len == 0)
			return Vector2(0, 0);
		return Vector2(x / len, y / len);
	}
};

// 进程错误处理
class custom_runtime_error
{
public:
	custom_runtime_error(const std::string& title, const std::string& info)
		: error_title(title), error_info(info) {
	}

	custom_runtime_error(const char* title, const char* info)
		: error_title(title), error_info(info) {
	}

	~custom_runtime_error() = default;

	// 获取错误标题
	const char* title() const noexcept { return error_title.c_str(); }

	// 获取错误信息
	const char* what() const noexcept { return error_info.c_str(); }

private:
	std::string error_title;	// 错误标题
	std::string error_info;		// 错误信息
};

// 随机数生成器
class Random
{
public:
	Random()
	{
		std::random_device rd;
		engine.seed(rd());
	}

	// 生成 [min, max] 范围内的随机整数
	static int randint(int min, int max)
	{
		if (min > max) std::swap(min, max);
		std::uniform_int_distribution<int> dist(min, max);
		return dist(get_engine());
	}

	// 生成 [min, max] 范围内的随机浮点数
	static float randfloat(float min, float max)
	{
		if (min > max) std::swap(min, max);
		std::uniform_real_distribution<float> dist(min, max);
		return dist(get_engine());
	}

private:
	static std::mt19937& get_engine()
	{
		static Random _Instance;
		return _Instance.engine;
	}

	std::mt19937 engine;
};

// 时钟
using namespace std::chrono;
class Clock
{
public:
	Clock()
	{
		this->target_fps = 60;			// 默认60帧
		this->target_time = 1000 / target_fps;
		this->last_time = steady_clock::now();
		this->delta_time = 0;
		this->time_scale = 1.0f;
	}

	Clock(int fps_limit)
	{
		this->target_fps = fps_limit;	// 设置目标FPS
		this->target_time = 1000 / target_fps;
		this->last_time = steady_clock::now();
		this->delta_time = 0;
		this->time_scale = 1.0f;
	}

	~Clock() = default;

	// 时钟开始计时
	void start_frame()
	{
		auto currentTime = steady_clock::now();
		delta_time = duration_cast<milliseconds>(currentTime - last_time).count();
		last_time = currentTime;
	}

	// 时钟结束计时
	void end_frame() const
	{
		auto currentTime = steady_clock::now();
		auto elapsedTime = duration_cast<milliseconds>(currentTime - last_time).count();	// 获取经过时间

		// 如果经过时间小于帧间隔就休眠
		if (elapsedTime < target_time)
			std::this_thread::sleep_for(milliseconds(target_time - elapsedTime));
	}

	// 设置是否垂直同步
	void set_VSync(bool is_abled)
	{
		if (is_abled)
		{
			int refresh = this->get_screen_refreshRate();
			// printf("屏幕刷新率: %d\n", refresh);	// 打印屏幕刷新率

			if (refresh)
				this->target_time = 1000 / refresh;
			else
				throw custom_runtime_error(u8"Clock Error", u8"Warning: Cannot get screen refresh rate!");
		}
		else
			this->target_time = 1000 / target_fps;		// 设置为目标FPS
	}

	// 设置FPS
	void setFPS(int fps_limit)
	{
		this->target_fps = fps_limit;		// 设置目标FPS
		this->target_time = 1000 / fps_limit;
	}

	// 获取FPS
	int getFPS() const { return (int)(1000 / delta_time); }

	// 获取帧间隔
	float get_DeltaTime() const { return (delta_time / 1000.0f) * time_scale; }

	// 设置时间缩放
	void set_time_scale(float scale)
	{
		if (scale <= 0.0f) return;
		this->time_scale = scale;
	}

private:
	// 获取屏幕刷新率
	int get_screen_refreshRate()
	{
		// 获取主显示器（默认显示器）
		SDL_DisplayID display = SDL_GetPrimaryDisplay();
		if (!display) return 0;

		// 获取当前显示模式
		const SDL_DisplayMode* mode = SDL_GetCurrentDisplayMode(display);
		if (!mode) return 0;

		// 返回刷新率（如果有效，否则返回默认值）
		return (mode->refresh_rate > 0) ? (int)mode->refresh_rate : 0;
	}

private:
	int target_fps;							// 目标FPS
	int target_time;						// 目标帧间隔
	steady_clock::time_point last_time;		// 经过时间
	float delta_time;						// 单帧间隔
	float time_scale;						// 时间缩放
};

// 计时器
class Timer
{
public:
	Timer() = default;
	~Timer() = default;

	// 重启计时器
	void restart() { pass_time = 0; shotted = false; }

	// 设置等待时间
	void set_wait_time(float val) { this->wait_time = val; }

	// 设置是否单次触发
	void set_one_shot(bool flag) { this->one_shot = flag; }

	// 设置回调函数
	void set_on_timeout(std::function<void()> timeout_callback) { this->on_timeout = timeout_callback; }

	// 暂停
	void pause() { paused = true; }

	// 继续
	void resume() { paused = false; }

	// 更新计时器
	void on_update(float delta)
	{
		if (paused) return;

		pass_time += delta;
		if (pass_time >= wait_time)
		{
			bool can_shot = (!one_shot || (one_shot && !shotted));
			shotted = true;

			if (can_shot && on_timeout)
				on_timeout();
			pass_time -= wait_time;
		}
	}

private:
	float pass_time = 0;				// 经过时间
	float wait_time = 0;				// 等待时间
	bool paused = false;				// 是否暂停
	bool shotted = false;				// 是否已经触发
	bool one_shot = false;				// 是否单词触发
	std::function<void()> on_timeout;	// 回调函数
};

// 数学运算工具
namespace maths
{
	// 角度转弧度
	inline float deg_to_rad(float degree) noexcept { return degree * PAI / 180.0f; }

	// 弧度转角度
	inline float rad_to_deg(float radian) noexcept { return radian * 180.0f / PAI; }

	// 插值函数
	inline float lerp(float current, float target, float t) noexcept { return current + (target - current) * t; }

	// 计算两点距离
	inline float distance_to(const Vector2& pos_1, const Vector2& pos_2)
	{
		float dx = pos_1.x - pos_2.x;
		float dy = pos_1.y - pos_2.y;
		return sqrtf((dx * dx) + (dy * dy));
	}
};

// 碰撞层级
enum class CollisionLayer
{
	None = 0,				// 无碰撞层
	Player = 1 << 0,		// 玩家层
	Enemy = 1 << 1,			// 敌人层
	GameMap = 1 << 2,		// 游戏地图层
	GameObject = 1 << 3,	// 游戏元素层
	Weapon = 1 << 4,		// 武器层
	UI = 1 << 5				// UI层
};

// =========================================================================================
// 				基 础 元 素							 base	Elements
// =========================================================================================

// 摄像机
class Camera
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
	Camera(SDL_Window* window, SDL_Renderer* renderer)
	{
		this->camera_window = window;
		this->camera_renderer = renderer;

		timer_shake.set_one_shot(true);
		timer_shake.set_on_timeout([&]()
			{
				is_shaking = false;
				shake_position = Vector2(0, 0);
			});
	}

	~Camera() = default;

	// 获取摄像机坐标(中心点)
	const Vector2& get_position() const { return position; }

	// 设置摄像机坐标(中心点)
	void set_position(const Vector2& pos) { this->base_position = pos; }

	// 设置摄像机缩放
	void set_zoom(float scale) { this->zoom = scale; }

	// 获取摄像机缩放
	float get_zoom() const { return this->zoom; }

	// 重置摄像机
	void reset()
	{
		position = base_position = shake_position = Vector2(0, 0);
		this->zoom = 1.0f;		// 重置缩放为1.0
	}

	// 抖动摄像机
	void shake(float strength, float duration)
	{
		is_shaking = true;
		shaking_strength = strength;

		timer_shake.set_wait_time(duration);
		timer_shake.restart();
	}

	// 渲染纹理
	void render_texture(SDL_Texture* texture, const SDL_FRect* rect_src, const SDL_FRect* rect_dst,
		double angle, const SDL_FPoint* center, bool is_flip = false) const
	{
		// 渲染坐标 = 屏幕中心点 + (角色世界坐标 - 摄像机坐标) * 缩放因子
		SDL_FRect rect_dst_win = *rect_dst;
		rect_dst_win.x = this->get_screen_center().x + (rect_dst_win.x - position.x) * zoom;
		rect_dst_win.y = this->get_screen_center().y + (rect_dst_win.y - position.y) * zoom;

		// 渲染大小 = 纹理大小 * 缩放因子
		rect_dst_win.w = rect_dst->w * zoom;
		rect_dst_win.h = rect_dst->h * zoom;

		SDL_RenderTextureRotated(camera_renderer, texture, rect_src, &rect_dst_win, angle, center,
			is_flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
	}

	// 绘制线段
	void render_shape(const Vector2& start, const Vector2& end, SDL_Color color) const
	{
		SDL_SetRenderDrawColor(camera_renderer, color.r, color.g, color.b, color.a);

		Vector2 start_win = { get_screen_center().x + (start.x - position.x) * zoom, get_screen_center().y + (start.y - position.y) * zoom };
		Vector2 end_win = { get_screen_center().x + (end.x - position.x) * zoom, get_screen_center().y + (end.y - position.y) * zoom };

		SDL_RenderLine(camera_renderer, start_win.x, start_win.y, end_win.x, end_win.y);
	}

	// 绘制矩形
	void render_shape(const SDL_FRect* rect, SDL_Color color, bool is_filled) const
	{
		SDL_SetRenderDrawColor(camera_renderer, color.r, color.g, color.b, color.a);

		SDL_FRect rect_dst_win = *rect;
		rect_dst_win.w *= zoom; rect_dst_win.h *= zoom;		// 缩放大小
		rect_dst_win.x = this->get_screen_center().x + (rect_dst_win.x - position.x) * zoom;
		rect_dst_win.y = this->get_screen_center().y + (rect_dst_win.y - position.y) * zoom;

		if(is_filled)
			SDL_RenderFillRect(camera_renderer, &rect_dst_win);
		else
			SDL_RenderRect(camera_renderer, &rect_dst_win);
	}

	// 绘制圆形
	void render_shape(const Vector2& pos, float radius, SDL_Color color, bool is_filled) const
	{
		SDL_SetRenderDrawColor(camera_renderer, color.r, color.g, color.b, color.a);

		Vector2 circle_center = { pos.x + radius, pos.y + radius };
		circle_center.x = this->get_screen_center().x + (circle_center.x - position.x) * zoom;
		circle_center.y = this->get_screen_center().y + (circle_center.y - position.y) * zoom;

		if (is_filled)
			Framework_Draw_FilledCircle(camera_renderer, circle_center.x, circle_center.y, radius * zoom);
		else
			Framework_Draw_Circle(camera_renderer, circle_center.x, circle_center.y, radius * zoom);
	}

	// 跟随角色
	void look_at(const Vector2& target_pos, int style)
	{
		if (style & None) return;

		if (style & Camera::Static_Follow)	// 静态跟随
		{
			if (style & Camera::Only_X)
				this->base_position.x = target_pos.x;
			else if (style & Camera::Only_Y)
				this->base_position.y = target_pos.y;
			else
				this->base_position = target_pos;
		}
		else if (style & Camera::Smooth_Follow)		// 平滑跟随
		{
			if (style & Camera::Only_X)
				this->base_position.x = maths::lerp(base_position.x, target_pos.x, smooth_strength);
			else if (style & Camera::Only_Y)
				this->base_position.y = maths::lerp(base_position.y, target_pos.y, smooth_strength);
			else
			{
				this->base_position.x = maths::lerp(base_position.x, target_pos.x, smooth_strength);
				this->base_position.y = maths::lerp(base_position.y, target_pos.y, smooth_strength);
			}
		}
	}

	// 更新摄像机
	void on_update(float delta)
	{
		timer_shake.on_update(delta);
		smooth_strength = SMOOTH_FACTOR * delta;

		if (is_shaking)
		{
			shake_position.x = (-50 + std::rand() % 100) / 50.0f * shaking_strength;
			shake_position.y = (-50 + std::rand() % 100) / 50.0f * shaking_strength;
		}

		// 最终位置 = 基础位置 + 抖动位置
		position = base_position + shake_position;
	}

private:
	// 获取屏幕中心点
	Vector2 get_screen_center() const
	{
		int screen_w, screen_h;
		SDL_GetWindowSize(camera_window, &screen_w, &screen_h);		// 计算屏幕宽高
		Vector2 screen_center = Vector2(screen_w / 2.0f, screen_h / 2.0f);	// 获取屏幕中心点
		return screen_center;
	}

private:
	Vector2 position;					// 摄像机最终位置
	Vector2 shake_position;				// 抖动位置
	Vector2 base_position;				// 基础位置

	Timer timer_shake;					// 抖动计时器
	bool is_shaking = false;			// 是否抖动
	float shaking_strength = 0;			// 抖动幅度
	float smooth_strength = 0;			// 平滑幅度
	float zoom = 1.0f;					// 摄像机缩放

	SDL_Window* camera_window = nullptr;		// 摄像机窗口
	SDL_Renderer* camera_renderer = nullptr;	// 摄像机渲染器
	const float SMOOTH_FACTOR = 1.5f;			// 平滑系数
};

// 序列帧动画
class Animation
{
public:
	Animation()
	{
		timer.set_one_shot(false);
		timer.set_on_timeout([&]()
			{
				idx_frame++;
				if (idx_frame >= frame_list.size())
				{
					idx_frame = is_loop ? 0 : frame_list.size() - 1;
					if (!is_loop && on_finished)
						on_finished();
				}
			});
	}

	~Animation() = default;

	//重置动画
	void reset() { timer.restart(); idx_frame = 0; }

	// 设置动画位置
	void set_position(const Vector2& position) { this->position = position; }

	// 设置动画方向
	void set_rotation(double angle) { this->angle = angle; }

	// 设置动画中心点
	void set_center(const SDL_FPoint& cneter) { this->center = center; }

	// 设置动画是否循环
	void set_loop(bool is_loop) { this->is_loop = is_loop; }

	// 设置动画帧间隔
	void set_interval(float interval) { timer.set_wait_time(interval); }

	// 设置动画是否翻转
	void set_flip(bool flag) { this->is_flip = flag; }

	// 设置动画回调函数
	void set_on_finished(std::function<void()> callback) { on_finished = callback; }

	// 更新动画
	void on_update(float delta) { timer.on_update(delta); }

	// 添加序列帧
	void add_frame(SDL_Texture* texture, int num_h)
	{
		float width, height;
		SDL_GetTextureSize(texture, &width, &height);	// 获取序列帧宽高

		float width_frame = width / num_h;	// 获取单张序列帧宽高
		for (int i = 0; i < num_h; i++)
		{
			SDL_FRect rect_src;
			rect_src.x = i * width_frame, rect_src.y = 0;
			rect_src.w = width_frame, rect_src.h = height;

			frame_list.emplace_back(texture, rect_src);		// 将序列帧加入列表
		}
	}

	// 渲染动画
	void on_render(const Camera& camera) const
	{
		const Frame& frame = frame_list[idx_frame];
		const Vector2& pos_camera = camera.get_position();

		SDL_FRect rect_dst{};
		rect_dst.x = position.x; rect_dst.y = position.y;
		rect_dst.w = frame.rect_src.w; rect_dst.h = frame.rect_src.h;

		camera.render_texture(frame.texture, &frame.rect_src, &rect_dst, angle, &center, is_flip);
	}

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
	Vector2 position;					// 位置
	double angle = 0;					// 角度
	SDL_FPoint center = { 0 };			// 中心点
	bool is_flip = false;				// 是否反转

	Timer timer;						// 内置计时器
	bool is_loop = true;				// 是否循环
	size_t idx_frame = 0;				// 当前帧索引
	std::vector<Frame> frame_list;		// 序列帧列表
	std::function<void()> on_finished;	// 动画播放完毕回调函数
};

class CollisionManager;
// 碰撞箱
class CollisionBox
{
	friend class CollisionManager;
public:
	// 设置是否启用碰撞
	void set_enabled(bool flag) { this->enabled = flag; }

	// 设置自身碰撞层
	void set_layer_src(CollisionLayer layer) { this->layer_src = layer; }

	// 设置目标碰撞层
	void set_layer_dst(CollisionLayer layer) { this->layer_dst = layer; }

	// 设置碰撞回调函数
	void set_on_collide(std::function<void()> callback) { this->on_collide = callback; }

	// 设置碰撞箱大小
	void set_size(const Size& size) { this->size = size; }

	// 获取碰撞箱大小
	const Size& get_size() const { return this->size; }

	// 设置碰撞箱位置
	void set_position(const Vector2& pos) { this->position = pos; }

private:
	Size size = { 0, 0 };								// 碰撞箱大小
	Vector2 position;									// 碰撞箱位置
	bool enabled = true;								// 是否启用碰撞检测
	std::function<void()> on_collide;					// 碰撞回调函数
	CollisionLayer layer_src = CollisionLayer::None;	// 自身碰撞层
	CollisionLayer layer_dst = CollisionLayer::None;	// 目标碰撞层

private:
	CollisionBox() = default;
	~CollisionBox() = default;
};

// 精灵抽象基类
class Sprite
{
public:
	Sprite() = default;
	~Sprite() = default;

	// 获取位置
	const Vector2& get_position() const { return this->position; }

	// 设置位置
	void set_position(const Vector2& pos) { this->position = pos; }

	virtual void on_update(float delta) = 0;
	virtual void on_render(const Camera& camera) = 0;
	virtual void on_input(const SDL_Event& event) = 0;

protected:
	Vector2 position;						// 位置
	Vector2 velocity;						// 速度
	CollisionBox* collision_box = nullptr;	// 碰撞箱
};

// 场景抽象基类
class Scene
{
public:
	Scene() = default;	// 构造函数处初始化实例化对象

	// 析函数出释放实例化对象
	virtual ~Scene()
	{
		for(auto& sprite: sprite_pool)
			delete sprite.second;
		sprite_pool.clear();
	}

	// 获取精灵
	template <typename _CvtTy>
	_CvtTy* find_sprite(const std::string& name)
	{
		if (sprite_pool.find(name) == sprite_pool.end())
			throw custom_runtime_error(u8"SceneManager Error", u8"Sprite “" + name + u8"” is not found!");
		return (_CvtTy*)sprite_pool[name];
	}

	virtual void on_enter() = 0;						// 进入场景（尽量不要创建新对象，而是对角色属性的重置）
	virtual void on_update(float delta) = 0;			// 更新场景
	virtual void on_render(const Camera& camera) = 0;	// 渲染场景
	virtual void on_input(const SDL_Event& event) = 0;	// 输入事件处理
	virtual void on_exit() = 0;							// 退出场景（尽量不要销毁对象）
	
protected:
	std::unordered_map<std::string, Sprite*> sprite_pool;	// 精灵池
};

// 按钮（仅适用与UI层）
class Button
{
public:
	Button() = default;
	Button(const Vector2& pos, const Size& size)
		: position(pos), size(size) {}

	~Button() = default;

	// 设置位置
	void set_position(const Vector2& pos) { this->position = pos; }

	// 获取位置
	const Vector2& get_position() const { return this->position; }

	// 设置大小
	void set_size(const Size& size) { this->size = size; }

	// 获取大小
	const Size& get_size() const { return this->size; }

	// 设置点击回调函数
	void set_on_click(std::function<void()> callback) { this->on_click = callback; }

	// 设置按钮纹理（分别为：正常、悬停、被点击）
	void set_texture(SDL_Texture* normal, SDL_Texture* hover, SDL_Texture* click)
	{
		// 检查纹理是否为nullptr
		if (!normal || !hover || !click)
			throw custom_runtime_error(u8"Button Argument Error", u8"Argument texture cannot be nullptr!");

		this->tex_normal = normal;
		this->tex_hover = hover;
		this->tex_click = click;
		this->current_texture = tex_normal;	// 默认显示正常状态纹理
	}

	// 渲染按钮
	void on_render(SDL_Renderer* renderer, double angle, SDL_FPoint* center)
	{
		if (!current_texture) return;	// 如果没有设置纹理则不渲染
		SDL_FRect rect_dst_win = { position.x, position.y, size.width, size.height };
		SDL_RenderTextureRotated(renderer, current_texture, nullptr, &rect_dst_win, angle, center, SDL_FLIP_NONE);
	}

	// 处理输入事件
	void on_input(const SDL_Event& event)
	{
		bool in_range_x = event.button.x >= position.x && event.button.x <= position.x + size.width;
		bool in_range_y = event.button.y >= position.y && event.button.y <= position.y + size.height;

		if (event.type == SDL_EVENT_MOUSE_MOTION)	// 鼠标悬停在按钮上
		{
			if (in_range_x && in_range_y)
			{
				this->current_texture = tex_hover;
				SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER));
			}
			else
			{
				this->current_texture = tex_normal;
				SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT));
			}
		}
		if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT) // 按钮被点击
		{
			if (in_range_x && in_range_y)
			{
				this->current_texture = tex_click;
				this->on_click();
			}
		}
		if (event.type == SDL_EVENT_MOUSE_BUTTON_UP && event.button.button == SDL_BUTTON_LEFT) // 按钮被释放
		{
			if (in_range_x && in_range_y)
				this->current_texture = tex_hover;
			else
				this->current_texture = tex_normal;
		}
	}

private:
	Vector2 position;			// 按钮位置
	Size size;					// 按钮大小

	SDL_Texture* tex_normal = nullptr;		// 正常状态纹理
	SDL_Texture* tex_hover = nullptr;		// 悬停状态纹理
	SDL_Texture* tex_click = nullptr;		// 按下状态纹理
	SDL_Texture* current_texture = nullptr;	// 当前显示的纹理

	std::function<void()> on_click;		// 点击回调函数
};

// 状态节点
class StateNode
{
public:
	StateNode() = default;
	~StateNode() = default;

	virtual void on_enter() {}					// 进入状态
	virtual void on_update(float delta) {}		// 更新状态
	virtual void on_exit() {}					// 退出状态
};

// ===========================================================================================
// 				行为控制器						Behavior Controllers
// ===========================================================================================

// 动画播放器
class AnimationPlayer
{
public:
	AnimationPlayer() = default;

	~AnimationPlayer()
	{
		for (auto& anim : animation_pool)
			anim.second.reset();		// 释放动画资源

		animation_pool.clear();
		current_animation = nullptr;
	}

	// 添加动画 (需要添加std::unique_ptr类型)
	void add_animation(const std::string& name, std::unique_ptr<Animation> anim)
	{
		if (animation_pool.find(name) != animation_pool.end())
			throw custom_runtime_error(error_title, u8"Animation “" + name + u8"” is already exist!");

		animation_pool[name] = std::move(anim);
	}

	// 切换动画
	void switch_to(const std::string& name)
	{
		if (animation_pool.find(name) == animation_pool.end())
			throw custom_runtime_error(error_title, u8"Animation “" + name + u8"” is not exist!");

		this->current_animation = animation_pool[name].get();
	}

	// 设置当前动画
	void set_animation(const std::string& name)
	{
		if (animation_pool.find(name) == animation_pool.end())
			throw custom_runtime_error(error_title, u8"Animation “" + name + u8"” is not exist!");

		this->current_animation = animation_pool[name].get();
		this->current_animation->reset();
	}

	// 获取当前动画
	Animation* get_current()
	{
		if (current_animation)
			return current_animation;
		else
			throw custom_runtime_error(error_title, u8"“current_animation” is a nullptr!");
	}

	// 更新动画
	void on_update(float delta)
	{
		if (current_animation)
			current_animation->on_update(delta);
		else
			throw custom_runtime_error(error_title, u8"“current_animation” is a nullptr!");
	}

	// 渲染动画
	void on_render(const Camera& camera)
	{
		if (current_animation)
			current_animation->on_render(camera);
		else
			throw custom_runtime_error(error_title, u8"“current_animation” is a nullptr!");
	}

private:
	std::unordered_map<std::string, std::unique_ptr<Animation>> animation_pool;		// 动画池
	Animation* current_animation = nullptr;											// 当前动画

	std::string error_title = u8"Animation Player Error";	// 错误标题
};

// 状态机
class StateMachine
{
public:
	StateMachine() = default;

	~StateMachine()
	{
		for (auto& state : state_pool)
			state.second.reset();

		state_pool.clear();
		current_state = nullptr;
	}

	void on_update(float delta)
	{
		if (!current_state)
		{
			std::string info = u8"Current state is nullptr! Please check if the status node is initialized";
			throw custom_runtime_error(u8"StateMachine Error", info.c_str());
		}

		if (need_init)	// 如果需要初始化状态机
		{
			current_state->on_enter();
			need_init = false;		// 初始化完成
		}

		current_state->on_update(delta);
	}

	void set_entry(const std::string& name)
	{
		// 如果未找到目标状态
		if (state_pool.find(name) == state_pool.end())
		{
			std::string info = u8"State “" + name + u8"” is not found!";
			throw custom_runtime_error(u8"StateMachine Error", info);
		}

		current_state = state_pool[name].get();
	}

	void switch_to(const std::string& name)
	{
		// 如果未找到目标状态
		if (state_pool.find(name) == state_pool.end())
		{
			std::string info = u8"State “" + name + u8"” is not found!";
			throw custom_runtime_error(u8"StateMachine Error", info);
		}

		if (current_state) current_state->on_exit();
		current_state = state_pool[name].get();
		current_state->on_enter();
	}

	void register_state(const std::string& name, std::unique_ptr<StateNode> state)
	{
		// 如果该状态已存在
		if (state_pool.find(name) != state_pool.end())
		{
			std::string info = u8"State “" + name + u8"” is already exist!";
			throw custom_runtime_error(u8"StateMachine Error", info);
		}

		state_pool[name] = std::move(state);		// 注册状态节点
	}

private:
	bool need_init = true;		// 是否需要初始化状态机
	StateNode* current_state = nullptr;		// 当前状态节点
	std::unordered_map<std::string, std::unique_ptr<StateNode>> state_pool;		// 状态池
};

// =========================================================================================
//					管 理 器								Managers
// =========================================================================================

// 资源管理器
class ResourcesManager
{
public:
	// 获取资源管理器单例
	static inline ResourcesManager* instance()
	{
		if (!m_instance)
			m_instance = new ResourcesManager();

		return m_instance;
	}

	// 加载资源
	inline void load_resources(SDL_Renderer* renderer, const char* _PathName)
	{
		using namespace std::filesystem;

		// 判断文件是否存在
		if (!exists(_PathName))
		{
			std::string info = u8"Dictionary “" + std::string(_PathName) + u8"” is an error dictionary!";
			throw custom_runtime_error(u8"ResourcesManager Error", info);
		}

		// 遍历目标文件内部所有文件
		for (const auto& entry : recursive_directory_iterator(_PathName))
		{
			if (entry.is_regular_file())	// 如果是有效文件
			{
				const path& path = entry.path();
				if (path.extension() == ".bmp" || path.extension() == ".png" || path.extension() == ".jpg")
				{
					SDL_Texture* texture = IMG_LoadTexture(renderer, path.u8string().c_str());
					texture_pool[path.stem().u8string()] = texture;
				}
				if (path.extension() == ".wav" || path.extension() == ".mp3" || path.extension() == ".ogg")
				{
					Mix_Chunk* audio = Mix_LoadWAV(path.u8string().c_str());
					audio_pool[path.stem().u8string()] = audio;
				}
				if (path.extension() == ".ttf" || path.extension() == ".TTF")
				{
					TTF_Font* font = TTF_OpenFont(path.u8string().c_str(), 24);
					font_pool[path.stem().u8string()] = font;
				}
			}
		}
	}

	// 查找纹理资源
	inline SDL_Texture* find_texture(const std::string& name)
	{
		// 未找到
		if (texture_pool.find(name) == texture_pool.end())
		{
			std::string info = u8"Texture “" + name + u8"” is not found!";
			throw custom_runtime_error(u8"ResourcesManager Error", info);
		}

		return texture_pool[name];
	}

	// 查找音频资源
	inline Mix_Chunk* find_audio(const std::string& name)
	{
		// 未找到
		if (audio_pool.find(name) == audio_pool.end())
		{
			std::string info = u8"Audio “" + name + u8"” is not found!";
			throw custom_runtime_error(u8"ResourcesManager Error", info);
		}

		return audio_pool[name];
	}

	// 查找字体资源
	inline TTF_Font* find_font(const std::string& name)
	{
		// 未找到
		if (font_pool.find(name) == font_pool.end())
		{
			std::string info = u8"Font “" + name + u8"” is not found!";
			throw custom_runtime_error(u8"ResourcesManager Error", info);
		}

		return font_pool[name];
	}

	// 显示已加载的资源
	inline void debug_resources_list()
	{
		printf("=============================================================\n");
		printf("已加载的纹理资源如下 (共%d个):\n", (int)texture_pool.size());
		for (auto& texture : texture_pool)
			printf("%s\n", texture.first.c_str());

		printf("\n已加载的音频资源如下 (共%d个):\n", (int)audio_pool.size());
		for (auto& audio : audio_pool)
			printf("%s\n", audio.first.c_str());

		printf("\n已加载的字体资源如下 (共%d个):\n", (int)font_pool.size());
		for (auto& font : font_pool)
			printf("%s\n", font.first.c_str());
		printf("=============================================================\n");
	}

private:
	ResourcesManager() = default;
	~ResourcesManager() = default;

private:
	static ResourcesManager* m_instance;								// 资源管理器单例
	std::unordered_map<std::string, SDL_Texture*> texture_pool;			// 纹理资源池
	std::unordered_map<std::string, Mix_Chunk*> audio_pool;				// 音频资源池
	std::unordered_map<std::string, TTF_Font*> font_pool;				// 字体资源池
};
inline ResourcesManager* ResourcesManager::m_instance = nullptr;
typedef ResourcesManager ResLoader;

// 场景管理器
class SceneManager
{
public:
	// 获取场景管理器单例
	static inline SceneManager* instance()
	{
		if (!m_instance)
			m_instance = new SceneManager();

		return m_instance;
	}

	// 设置当前场景
	inline void set_current_scene(const std::string& name)
	{
		// 如果场景池中不存在该场景
		if (scene_pool.find(name) == scene_pool.end())
			throw custom_runtime_error(u8"SceneManager Error", u8"Scene “" + name + u8"” is not exist!");

		current_scene = scene_pool[name];
		current_scene->on_enter();
	}

	// 切换场景
	inline void switch_to(const std::string& name)
	{
		current_scene->on_exit();

		// 如果场景池中不存在该场景
		if (scene_pool.find(name) == scene_pool.end())
			throw custom_runtime_error(u8"SceneManager Error", u8"Scene “" + name + u8"” is not exist!");

		current_scene = scene_pool[name];
		current_scene->on_enter();
	}

	// 添加新场景
	inline void add_scene(const std::string& name, Scene* scene)
	{
		if (scene_pool.find(name) != scene_pool.end())
			throw custom_runtime_error(u8"SceneManager Error", u8"Scene “" + name + u8"” is already exist!");

		scene_pool[name] = scene;
	}

	// 获取场景
	inline Scene* find_scene(const std::string& name)
	{
		// 如果场景池中不存在该场景
		if (scene_pool.find(name) == scene_pool.end())
			throw custom_runtime_error(u8"SceneManager Error", u8"Scene “" + name + u8"” is not exist!");

		return scene_pool[name];
	}

	// 更新当前场景
	void on_update(float delta)
	{
		if (current_scene)
			current_scene->on_update(delta);
		else
			throw custom_runtime_error(u8"SceneManager Error", u8"“current_scene” is a nullptr!");
	}

	// 渲染当前场景
	void on_render(const Camera& camera)
	{
		if (current_scene)
			current_scene->on_render(camera);
		else
			throw custom_runtime_error(u8"SceneManager Error", u8"“current_scene” is a nullptr!");
	}

	// 处理输入事件
	void on_input(const SDL_Event& event)
	{
		if (current_scene)
			current_scene->on_input(event);
		else
			throw custom_runtime_error(u8"SceneManager Error", u8"“current_scene” is a nullptr!");
	}

private:
	SceneManager() = default;
	~SceneManager() = default;

private:
	static SceneManager* m_instance;						// 场景管理器单例
	Scene* current_scene = nullptr;							// 当前场景
	std::unordered_map<std::string, Scene*> scene_pool;		// 场景池
};
inline SceneManager* SceneManager::m_instance = nullptr;

// 碰撞管理器
class CollisionManager
{
public:
	// 获取碰撞管理器单例
	static inline CollisionManager* instance()
	{
		if (!m_instance)
			m_instance = new CollisionManager();

		return m_instance;
	}

	// 添加碰撞箱
	inline CollisionBox* create_collision_box()
	{
		CollisionBox* collision_box = new CollisionBox();
		collision_box_list.push_back(collision_box);
		return collision_box;
	}

	// 销毁碰撞箱
	inline void destroy_collision_box(CollisionBox* collision_box)
	{
		collision_box_list.erase(std::remove(collision_box_list.begin(),
			collision_box_list.end(), collision_box), collision_box_list.end());
		delete collision_box;
	}

	// 处理碰撞检测
	inline void process_collide()
	{
		for (auto collision_box_src : collision_box_list)
		{
			if (!collision_box_src->enabled || collision_box_src->layer_dst == CollisionLayer::None)
				continue;

			for (auto collision_box_dst : collision_box_list)
			{
				if (!collision_box_dst->enabled || collision_box_src == collision_box_dst
					|| collision_box_src->layer_dst != collision_box_dst->layer_src)
					continue;

				// 横向碰撞条件：两碰撞箱的maxX - 两碰撞箱的minX <= 两碰撞箱的宽度之和
				float max_x = std::max(collision_box_src->position.x + collision_box_src->size.width, collision_box_dst->position.x + collision_box_dst->size.width);
				float min_x = std::min(collision_box_src->position.x, collision_box_dst->position.x);
				bool is_collide_x = (max_x - min_x <= collision_box_src->size.width + collision_box_dst->size.width);

				// 纵向碰撞条件：两碰撞箱的maxY - 两碰撞箱的minY <= 两碰撞箱的高度之和
				float max_y = std::max(collision_box_src->position.y + collision_box_src->size.height, collision_box_dst->position.y + collision_box_dst->size.height);
				float min_y = std::min(collision_box_src->position.y, collision_box_dst->position.y);
				bool is_collide_y = (max_y - min_y <= collision_box_src->size.height + collision_box_dst->size.height);
				
				// 如果横向/纵向都成立，且目标碰撞箱存在回调函数，则执行回调函数
				if (is_collide_x && is_collide_y && collision_box_dst->on_collide)
					collision_box_dst->on_collide();
			}
		}
	}
	
	// 调试碰撞箱
	inline void debug_collision_box(const Camera& camera)
	{
		for (auto collision_box : collision_box_list)
		{
			// 定义碰撞箱渲染矩形
			SDL_FRect rect_dst = { collision_box->position.x, collision_box->position.y,
				collision_box->size.width, collision_box->size.height };

			// 定义碰撞箱渲染颜色
			SDL_Color color;
			if (collision_box->enabled)
				color = { 255,195,195,255 };
			else
				color = { 115,155,175,255 };

			camera.render_shape(&rect_dst, color, false);	// 绘制碰撞箱
		}
	}

private:
	CollisionManager() = default;
	~CollisionManager() = default;

private:
	static CollisionManager* m_instance;			// 碰撞管理器单例
	std::vector<CollisionBox*> collision_box_list;	// 碰撞箱列表
};
inline CollisionManager* CollisionManager::m_instance = nullptr;
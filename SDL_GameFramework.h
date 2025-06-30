#pragma once

// 标准库头文件
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
inline void Framework_Init_Renderer(const char* name)
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
	for (int y = -radius; y <= radius; y++)
	{
		int x = sqrtf(radius * radius - y * y);
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
	int getFPS() const { return 1000 / delta_time; }

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
		return (mode->refresh_rate > 0) ? mode->refresh_rate : 0;
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

// 小工具
namespace utils
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
		this->position = Vector2(0, 0);
		this->shake_position = Vector2(0, 0);
		this->base_position = Vector2(0, 0);
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
				this->base_position.x = utils::lerp(base_position.x, target_pos.x, smooth_strength);
			else if (style & Camera::Only_Y)
				this->base_position.y = utils::lerp(base_position.y, target_pos.y, smooth_strength);
			else
			{
				this->base_position.x = utils::lerp(base_position.x, target_pos.x, smooth_strength);
				this->base_position.y = utils::lerp(base_position.y, target_pos.y, smooth_strength);
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

// 动画类
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

		SDL_FRect rect_dst;
		rect_dst.x = position.x - frame.rect_src.w / 2.0f;
		rect_dst.y = position.y - frame.rect_src.h / 2.0f;
		rect_dst.w = frame.rect_src.w;
		rect_dst.h = frame.rect_src.h;

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
	Vector2 position;	// 位置
	Vector2 velocity;	// 速度
};

// 场景抽象基类
class Scene
{
public:
	Scene() = default;				// 构造函数处初始化实例化对象
	virtual ~Scene() = default;		// 析函数出释放实例化对象

	virtual void on_enter() = 0;						// 进入场景（尽量不要创建新对象，而是对角色属性的重置）
	virtual void on_update(float delta) = 0;			// 更新场景
	virtual void on_render(const Camera& camera) = 0;	// 渲染场景
	virtual void on_input(const SDL_Event& event) = 0;	// 输入事件处理
	virtual void on_exit() = 0;							// 退出场景（尽量不要销毁对象）
};

// =========================================================================================
//					管 理 器								Managers
// =========================================================================================

// 资源管理器
class ResourcesManager
{
public:
	// 获取资源管理器单例
	inline static ResourcesManager* instance()
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
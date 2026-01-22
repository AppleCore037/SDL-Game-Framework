#pragma once

// 标准库
#include <chrono>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
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

constexpr float PI = 3.14159265f;	// 圆周率
constexpr SDL_InitFlags SDL_INIT_EVERYTHING = (SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC | SDL_INIT_GAMEPAD | SDL_INIT_SENSOR);
constexpr MIX_InitFlags MIX_INIT_EVERYTHING = (MIX_INIT_MP3 | MIX_INIT_FLAC | MIX_INIT_MID | MIX_INIT_MOD | MIX_INIT_OGG | MIX_INIT_OPUS | MIX_INIT_WAVPACK);

namespace fce
{
// ==============================================================================================
// 				基础变量属性							Base Variables Attributes
// ==============================================================================================

	inline SDL_Window* Main_Window = nullptr;				// 主窗口
	inline SDL_Renderer* Main_Renderer = nullptr;			// 主渲染器
	inline TTF_TextEngine* Main_TextEngine = nullptr;		// 主文字引擎
	inline SDL_Event Main_Event;							// 主循环事件

	// 预定义颜色
	namespace Colors
	{
		SDL_Color Red = { 255, 0, 0, 255 };			  // 正红
		SDL_Color Blue = { 0, 0, 255, 255 };		  // 正蓝
		SDL_Color Green = { 0, 255, 0, 255 };		  // 正绿
		SDL_Color White = { 255, 255, 255, 255 };	  // 正白
		SDL_Color Black = { 0, 0, 0, 255 };			  // 正黑
		SDL_Color Yellow = { 255, 255, 0, 255 };	  // 正黄
		SDL_Color Pink = { 255, 0, 255, 255 };		  // 正粉
		SDL_Color LightBlue = { 0, 255, 255, 255 };	  // 浅蓝
		SDL_Color LightGray = { 128, 128, 128, 255 }; // 浅灰
		SDL_Color DarkGray = { 50, 50, 50, 255 };	  // 暗灰
		SDL_Color Orange = { 255, 165, 0, 255 };	  // 橙色
		SDL_Color Purple = { 128, 0, 128, 255 };	  // 紫色
	}

// ==============================================================================================
//				基 础 枚 举 类 型						Base Enum Types
// ==============================================================================================

	// 窗口标签
	enum class WindowFlags
	{
		Shown = SDL_EVENT_WINDOW_SHOWN,			// 显示
		Hidden = SDL_EVENT_WINDOW_HIDDEN,		// 隐藏
		Resizable = SDL_EVENT_WINDOW_RESIZED,	// 可调整大小
		Maximized = SDL_EVENT_WINDOW_MAXIMIZED,	// 最大化
		Minimized = SDL_EVENT_WINDOW_MINIMIZED,	// 最小化
		Fullscreen = SDL_WINDOW_FULLSCREEN,		// 全屏
	};

	// 消息框类型
	enum class MsgBoxFlags
	{
		Info = SDL_MESSAGEBOX_INFORMATION,	// 信息
		Warning = SDL_MESSAGEBOX_WARNING,	// 警告
		Error = SDL_MESSAGEBOX_ERROR,		// 错误
	};

	// 渲染层级
	enum class RenderLayer
	{
		None,			// 无渲染层
		Background,		// 背景层
		Frontground,	// 前景层
		Label,			// 游戏内部文本层
		GameObject,		// 游戏元素层
		Effect,			// 特效层
		UI				// UI层
	};

	// 碰撞层级
	enum class CollisionLayer
	{
		None = 0,				// 无碰撞层
		Player = 1 << 0,		// 玩家层
		Enemy = 1 << 1,			// 敌人层
		GameMap = 1 << 2,		// 游戏地图层
		GameObject = 1 << 3,	// 游戏元素层
		Attack = 1 << 4,		// 攻击层
	};

// ==============================================================================================
//				基 础 函 数						Base	Functions
// ==============================================================================================

	// 获取键盘/鼠标事件的UTF-8格式字符串
	inline const char* Get_EventName(const SDL_Event& event)
	{
		static std::string result = "None";

		if (event.type == SDL_EVENT_KEY_DOWN)
			result = SDL_GetKeyName(event.key.key);
		if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
		{
			switch (event.button.button)
			{
			case SDL_BUTTON_LEFT:	result = "Left Mouse Button";   break;
			case SDL_BUTTON_MIDDLE: result = "Middle Mouse Button"; break;
			case SDL_BUTTON_RIGHT:	result = "Right Mouse Button";	break;
			case SDL_BUTTON_X1:		result = "Side Button 1";		break;
			case SDL_BUTTON_X2:		result = "Side Button 2";		break;
			}
		}
		if (event.type == SDL_EVENT_MOUSE_WHEEL)
		{
			if (event.wheel.y > 0) result = "Mouse Wheel Up";
			if (event.wheel.y < 0) result = "Mouse Wheel Down";
		}
		if (event.type == SDL_EVENT_MOUSE_MOTION)
			result = "Mouse Motion";

		return result.c_str();
	}

	// 显示消息框
	inline void Show_MessageBox(MsgBoxFlags type, const char* title, const char* message)
	{
		// 检测主窗口是否存在
		if (Main_Window != nullptr)
			SDL_ShowSimpleMessageBox((SDL_MessageBoxFlags)type, title, message, Main_Window);
		else
			SDL_ShowSimpleMessageBox((SDL_MessageBoxFlags)type, title, message, nullptr);
	}

	// 重载按位或运算符 |
	constexpr CollisionLayer operator|(CollisionLayer lhs, CollisionLayer rhs)
	{
		using underlying = std::underlying_type_t<CollisionLayer>;
		return static_cast<CollisionLayer>(static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
	}

	// 重载按位与运算符 &
	constexpr bool operator&(CollisionLayer lhs, CollisionLayer rhs)
	{
		using underlying = std::underlying_type_t<CollisionLayer>;
		return (static_cast<underlying>(lhs) & static_cast<underlying>(rhs)) != 0;
	}

// ==============================================================================================
//				基 础 类 型								Base	Type
// ==============================================================================================

	// 大小
	struct Size
	{
		Size() = default;
		~Size() = default;
		Size(float w, float h) 
			: w(w), h(h) {}

		float w;
		float h;
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

		Vector2 operator-(const Vector2& vct2) const { return Vector2(x - vct2.x, y - vct2.y); }

		Vector2 operator*(float val) { return Vector2(x * val, y * val); }

		Vector2 operator/(float val) { return Vector2(x / val, y / val); }

		float operator*(const Vector2& vct2) { return x * vct2.x + y * vct2.y; }

		void operator+=(const Vector2& vct2) { x += vct2.x; y += vct2.y; }

		void operator-=(const Vector2& vct2) { x -= vct2.x; y -= vct2.y; }

		void operator/=(float val) { x /= val; y /= val; }

		void operator*=(float val) { x *= val; y *= val; }

		// 获取长度
		float length() const { return sqrt(x * x + y * y); }

		// 检测是否趋近与目标向量
		bool approx(const Vector2& target) const { return (*this - target).length() <= 0.0001f; }

		// 标准化
		Vector2 normalize() const
		{
			float len = this->length();

			if (len == 0)
				return Vector2(0, 0);
			return Vector2(x / len, y / len);
		}
	};
	using Point = Vector2;	// 坐标点

	// 图集
	class Atlas
	{
	public:
		Atlas() = default;
		Atlas(const char* path_template, int beg_idx, int end_idx) { this->load_from_file(path_template, beg_idx, end_idx); }

		~Atlas()
		{
			for (SDL_Texture* texture : tex_list)
				SDL_DestroyTexture(texture);
		}

		// 从指定路径模板加载目标数量纹理
		void load_from_file(const char* path_template, int beg_idx, int end_idx)
		{
			for (int i = beg_idx; i <= end_idx; i++)
			{
				char path_file[256];
				sprintf_s(path_file, path_template, i);		// 补全路径

				SDL_Texture* texture = IMG_LoadTexture(Main_Renderer, path_file);	// 加载纹理
				if (texture == nullptr)
				{
					std::string info = u8"Cannot load texture from “" + std::string(path_file) + u8"”\nPlease check path correctness or image's existence!";
					throw std::runtime_error(info.c_str());
				}

				tex_list.push_back(texture);	// 加入纹理列表
			}
		}

		// 清空图集
		void clear() { tex_list.clear(); }

		// 获取图集纹理数量
		int get_size() const { return (int)tex_list.size(); }

		// 添加纹理
		void add_texture(SDL_Texture* texture) { tex_list.push_back(texture); }

		// 获取纹理
		SDL_Texture* get_texture(int idx)
		{
			if (idx < 0 || idx >= tex_list.size())
				return nullptr;
			return tex_list[idx];
		}

	private:
		std::vector<SDL_Texture*> tex_list;		// 纹理集
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

// ==============================================================================================
//				基 础 工 具								Base	Kits
// ==============================================================================================

	// 进程错误处理
	class custom_error
	{
	public:
		custom_error(const std::string& title, const std::string& info)
			: error_title(title), error_info(info) {}

		custom_error(const char* title, const char* info)
			: error_title(title), error_info(info) {}

		~custom_error() = default;

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
		// 生成 [min, max] 范围内的随机整数
		static int randint(int min, int max)
		{
			if (min > max) std::swap(min, max);
			std::uniform_int_distribution<int> dist(min, max);
			return dist(m_instance.engine);
		}

		// 生成 [min, max] 范围内的随机浮点数
		static float randfloat(float min, float max)
		{
			if (min > max) std::swap(min, max);
			std::uniform_real_distribution<float> dist(min, max);
			return dist(m_instance.engine);
		}

	private:
		Random() { engine.seed(std::random_device()()); }
		~Random() = default;

	private:
		static Random m_instance;	// 单例实例
		std::mt19937 engine;		// 随机数引擎
	};
	inline Random Random::m_instance;

	// 全局游戏时钟
	class Clock
	{
		using clock_t = std::chrono::steady_clock;
		using ms_t = std::chrono::milliseconds;

	public:
		// 起始当前帧计时
		static void start_frame()
		{
			clock_t::time_point cur_time = clock_t::now();
			m_instance.delta_time = static_cast<double>(std::chrono::duration_cast<ms_t>(cur_time - m_instance.last_time).count());
			m_instance.last_time = cur_time;
		}

		// 结束当前帧计时
		static void end_frame()
		{
			auto cur_time = clock_t::now();
			auto elp_time = std::chrono::duration_cast<ms_t>(cur_time - m_instance.last_time).count(); // 经过时间

			// 如果"经过时间小于帧间隔" && "启用帧率限制"就休眠
			if (elp_time < m_instance.target_dur && m_instance.is_frame_limited)
				std::this_thread::sleep_for(ms_t(m_instance.target_dur - elp_time));
			else
				std::this_thread::sleep_for(ms_t(m_instance.MIN_DUR - elp_time));
		}

		// 设置是否垂直同步（默认false）
		static void set_VSync(bool is_abled)
		{
			if (is_abled)
				m_instance.target_dur = 1000 / m_instance.get_screen_refreshRate();
			else
				m_instance.target_dur = 1000 / m_instance.target_fps;	// 设置为目标FPS
		}

		// 设置FPS
	 	static void set_fps(int fps_limit)
		{
			m_instance.target_fps = fps_limit;		// 设置目标FPS
			m_instance.target_dur = 1000 / fps_limit;
		}

		// 获取全局经过时间（秒）
		static float get_global_time()
		{
			auto global_current_time = clock_t::now();
			// 转换为秒
			return static_cast<float>(std::chrono::duration_cast<ms_t>(global_current_time - m_instance.global_start_time).count()) * 0.001f;
		}

		// 重置全局经过时间
		static void restart_global_time() { m_instance.global_start_time = clock_t::now(); }

		// 设置是否启用帧率限制（默认是true）
		static void set_frame_limit(bool flag) { m_instance.is_frame_limited = flag; }

		// 获取FPS
		static int get_fps() { return static_cast<int>(1000 / m_instance.delta_time); }

		// 获取帧间隔
		static float get_DeltaTime() { return static_cast<float>((m_instance.delta_time / 1000.0f) * m_instance.time_scale); }

		// 设置时间缩放
		static void set_time_scale(double scale)
		{
			if (scale <= 0.0f) m_instance.time_scale = 1.0f;
			else m_instance.time_scale = scale;
		}

	private:
		Clock()
		{
			this->last_time = clock_t::now();
			this->global_start_time = clock_t::now();
		}

		~Clock() = default;

		// 获取屏幕刷新率（获取失败默认返回60）
		int get_screen_refreshRate() const
		{
			// 获取主显示器（默认显示器）
			SDL_DisplayID display = SDL_GetPrimaryDisplay();
			if (!display) return 0;

			// 获取当前显示模式
			const SDL_DisplayMode* mode = SDL_GetCurrentDisplayMode(display);
			if (!mode) return 0;

			// 返回刷新率（如果有效，否则返回默认值60）
			return (mode->refresh_rate > 0) ? (int)mode->refresh_rate : 60;
		}

	private:
		static Clock m_instance;		// 单例实例
		int target_fps = 60;			// 目标FPS
		int target_dur = 1000 / 60;		// 目标帧间隔
		double delta_time = 0;			// 单帧间隔
		double time_scale = 1.0f;		// 时间缩放
		bool is_frame_limited = true;	// 是否启用帧率限制
		clock_t::time_point last_time;			// 经过时间
		clock_t::time_point global_start_time;	// 全局起始时间

		const int MIN_DUR = 1000 / 800;		// 最小帧间隔
	};
	inline Clock Clock::m_instance;

	// 数据读写器
	class Data
	{
	public:
		// 保存方式
		enum SvMod
		{
			Overwrite = std::ios::out,	// 覆盖
			Append = std::ios::app,		// 追加
			Truncate = std::ios::trunc	// 清空重写
		};

	public:
		// 清空文件
		static void clear(const std::string& file_name)
		{
			std::ofstream writer = std::ofstream(file_name + ".kvp", SvMod::Truncate);
			writer.close();
		}

		// 保存数据
		template <typename _Ty>
		static void save(const std::string& file_name, const std::string& key, _Ty value, SvMod mod = Append)
		{
			std::string _Type = (typeid(_Ty) == typeid(std::string) ? "string" : typeid(_Ty).name());
			std::string info = key + ":" + _Type + "=";

			auto finder = std::find(m_instance.type_list.begin(), m_instance.type_list.end(), _Type);
			if (finder == m_instance.type_list.end())
			{
				std::cerr << "DataSave Error: Unsupported data type!" << std::endl;
				return;
			}

			std::ofstream writer = std::ofstream(file_name + ".kvp", mod);
			writer << info << value << std::endl;
			writer.close();
		}

		// 读取数据
		template <typename _Ty>
		static _Ty read(const std::string& file_name, const std::string& key)
		{
			std::ifstream reader = std::ifstream(file_name + ".kvp", std::ios::in);
			if (!reader.is_open())
			{
				std::cerr << "DataRead Error: Cannot open file “" + file_name + ".kvp”!" << std::endl;
				return _Ty();
			}

			std::string line, tar_type = (typeid(_Ty) == typeid(std::string) ? "string" : typeid(_Ty).name());
			while (reader >> line)
			{
				std::string _Key = line.substr(0, line.find(':'));
				std::string _Type = line.substr(line.find(':') + 1, line.find('=') - line.find(':') - 1);

				if (_Key == key && _Type == tar_type)
				{
					std::string _Val = line.substr(line.find('=') + 1);
					return m_instance.type_conversion<_Ty>(_Val);
				}
			}

			std::cerr << "DataRead Error: Not found “" + key + "” in the file: " + file_name + ".kvp!" << std::endl;
			return _Ty();
		}

	private:
		Data() { m_instance.init_type_list(); }
		~Data() = default;

		// 初始化类型支持表
		void init_type_list()
		{
			m_instance.type_list.push_back(typeid(int).name());
			m_instance.type_list.push_back(typeid(float).name());
			m_instance.type_list.push_back(typeid(double).name());
			m_instance.type_list.push_back(typeid(bool).name());
			m_instance.type_list.push_back(typeid(char).name());
			m_instance.type_list.push_back(typeid(long long).name());
			m_instance.type_list.push_back("string");
			m_instance.type_list.push_back(typeid(unsigned long long).name());
			m_instance.type_list.push_back(typeid(unsigned int).name());
		}

		// 基础类型转换
		template <typename _Ty>
		_Ty type_conversion(const std::string& val)
		{
			if constexpr (std::is_same_v<_Ty, std::string>) return val;
			if constexpr (std::is_same_v<_Ty, bool>) return (val == "1" || val == "true");
			if constexpr (std::is_same_v<_Ty, char>) return val[0];
			if constexpr (std::is_same_v<_Ty, int>) return std::stoi(val);
			if constexpr (std::is_same_v<_Ty, float>) return std::stof(val);
			if constexpr (std::is_same_v<_Ty, double>) return std::stod(val);
			if constexpr (std::is_same_v<_Ty, long long>) return std::stoll(val);
			if constexpr (std::is_same_v<_Ty, unsigned long long>) return std::stoull(val);
			if constexpr (std::is_same_v<_Ty, unsigned int>) return std::stoul(val);
		}

	private:
		static Data m_instance;	// 单例实例
		std::vector<std::string> type_list;	// 类型支持表
	};
	inline Data Data::m_instance;

	// 数学运算工具
	namespace maths
	{
		// 角度转弧度
		inline float deg_to_rad(float degree) noexcept { return degree * PI / 180.0f; }

		// 弧度转角度
		inline float rad_to_deg(float radian) noexcept { return radian * 180.0f / PI; }

		// 插值函数
		inline float lerp(float current, float target, float t) noexcept { return current + (target - current) * t; }

		// 计算两点距离
		inline float distance_to(const Vector2& pos_1, const Vector2& pos_2)
		{
			float dx = pos_1.x - pos_2.x;
			float dy = pos_1.y - pos_2.y;
			return sqrtf((dx * dx) + (dy * dy));
		}

		// 摆动函数
		inline float swing(float min, float max, float strength)
		{
			if (max < min) std::swap(min, max);
			if (strength < 0) strength = -strength;

			float dt = Clock::get_global_time();
			// 摆动区间 = (max-min)÷2 * sin(dt*k) + (max+min)÷2
			return (max - min) / 2.0f * std::sin(dt * strength) + (max + min) / 2.0f;
		}
	};

// =========================================================================================
// 				基 础 元 素							 base	Elements
// =========================================================================================

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

		// 设置是否单次触发（默认false）
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
		Camera2D()
		{
			timer_shake.set_one_shot(true);
			timer_shake.set_on_timeout([&]()
				{
					is_shaking = false;
					shake_position = Vector2(0, 0);
				});
		}
		Camera2D(const Point& pos) : Camera2D() { this->base_position = pos; }

		~Camera2D() = default;

		// 获取摄像机世界坐标(中心点)
		const Point& get_position() const { return position; }

		// 设置摄像机世界坐标(中心点)
		void set_position(const Point& pos) { this->base_position = pos; }

		// 设置摄像机缩放
		void set_zoom(float scale) { this->zoom = scale; }

		// 获取摄像机缩放
		float get_zoom() const { return this->zoom; }

		// 重置摄像机
		void reset()
		{
			position = base_position = shake_position = Point(0, 0);
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
			double angle, const SDL_FPoint center_anchor = { 0.0f, 0.0f }, bool is_flip = false) const
		{
			SDL_FRect rect_dst_win = *rect_dst;

			// 设置渲染与旋转中心点
			SDL_FPoint rotate_center = { rect_dst->w * center_anchor.x * zoom, rect_dst->h * center_anchor.y * zoom };
			rect_dst_win.x = rect_dst_win.x - rect_dst->w * center_anchor.x;
			rect_dst_win.y = rect_dst_win.y - rect_dst->h * center_anchor.y;

			// 渲染坐标 = 屏幕中心点 + (角色世界坐标 - 摄像机坐标) * 缩放因子
			rect_dst_win.x = this->get_screen_center().x + (rect_dst_win.x - position.x) * zoom;
			rect_dst_win.y = this->get_screen_center().y + (rect_dst_win.y - position.y) * zoom;

			// 渲染大小 = 纹理大小 * 缩放因子
			rect_dst_win.w = rect_dst->w * zoom;
			rect_dst_win.h = rect_dst->h * zoom;

			SDL_RenderTextureRotated(Main_Renderer, texture, rect_src, &rect_dst_win, angle, &rotate_center,
				is_flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
		}

		// 渲染文字
		void render_text(const Point& pos, TTF_Font* font, SDL_Color color,
			float ptsize, const std::string& info) const
		{
			TTF_Text* text_win = TTF_CreateText(Main_TextEngine, font, info.c_str(), NULL);
			TTF_SetTextColor(text_win, color.r, color.g, color.b, color.a);

			Point screen_pos = this->world_to_screen(pos);
			TTF_SetFontSize(font, ptsize * zoom);

			TTF_DrawRendererText(text_win, screen_pos.x, screen_pos.y);
		}

		// 跟随角色
		void look_at(const Point& target_pos, int style)
		{
			if (style & None) return;

			if (style & Camera2D::Static_Follow)	// 静态跟随
			{
				if (style & Camera2D::Only_X)
					this->base_position.x = target_pos.x;
				else if (style & Camera2D::Only_Y)
					this->base_position.y = target_pos.y;
				else
					this->base_position = target_pos;
			}
			else if (style & Camera2D::Smooth_Follow)		// 平滑跟随
			{
				if (style & Camera2D::Only_X)
					this->base_position.x = maths::lerp(base_position.x, target_pos.x, smooth_strength);
				else if (style & Camera2D::Only_Y)
					this->base_position.y = maths::lerp(base_position.y, target_pos.y, smooth_strength);
				else
				{
					this->base_position.x = maths::lerp(base_position.x, target_pos.x, smooth_strength);
					this->base_position.y = maths::lerp(base_position.y, target_pos.y, smooth_strength);
				}
			}
		}

		// 窗口坐标转世界坐标
		Point screen_to_world(const Point& screen_pos) const
		{
			// 世界坐标 = (窗口坐标 - 屏幕中心点) / 缩放因子 + 摄像机坐标
			float world_x = (screen_pos.x - this->get_screen_center().x) / zoom + position.x;
			float world_y = (screen_pos.y - this->get_screen_center().y) / zoom + position.y;
			return Point(world_x, world_y);
		}

		// 世界坐标转窗口坐标
		Point world_to_screen(const Point& world_pos) const
		{
			// 渲染坐标 = 屏幕中心点 + (世界坐标 - 摄像机坐标) * 缩放因子
			float screen_x = this->get_screen_center().x + (world_pos.x - position.x) * zoom;
			float screen_y = this->get_screen_center().y + (world_pos.y - position.y) * zoom;
			return Point(screen_x, screen_y);
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
		Point get_screen_center() const
		{
			int screen_w, screen_h;
			SDL_GetWindowSize(Main_Window, &screen_w, &screen_h);		// 计算屏幕宽高
			Point screen_center = Point(screen_w / 2.0f, screen_h / 2.0f);	// 获取屏幕中心点
			return screen_center;
		}

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
		static void fill(SDL_Color color)
		{
			SDL_SetRenderDrawColor(Main_Renderer, color.r, color.g, color.b, color.a);
			SDL_RenderClear(Main_Renderer);
		}

		// 绘制线条
		static void render_line(const Camera2D& camera, const Point& begin, const Point& end, SDL_Color color)
		{
			Point begin_win = camera.world_to_screen(begin);
			Point end_win = camera.world_to_screen(end);

			SDL_SetRenderDrawColor(Main_Renderer, color.r, color.g, color.b, color.a);
			SDL_RenderLine(Main_Renderer, begin_win.x, begin_win.y, end_win.x, end_win.y);
		}

		// 绘制锚点为左上角的矩形
		static void render_TopLeftRect(const Camera2D& camera, const Point& pos, const Size& size, SDL_Color color, bool is_filled = false)
		{
			// 设置属性
			Point pos_win = camera.world_to_screen(pos);
			Size size_win = { size.w * camera.get_zoom(), size.h * camera.get_zoom() };
			SDL_FRect rect = { pos_win.x, pos_win.y, size_win.w, size_win.h };

			// 绘制
			SDL_SetRenderDrawColor(Main_Renderer, color.r, color.g, color.b, color.a);
			if (is_filled)
				SDL_RenderFillRect(Main_Renderer, &rect);
			else
				SDL_RenderRect(Main_Renderer, &rect);
		}

		// 绘制锚点为中心的矩形
		static void render_CenterRect(const Camera2D& camera, const Point& pos, const Size& size, SDL_Color color, bool is_filled = false)
		{
			// 设置属性
			Point pos_win = camera.world_to_screen(pos);
			Size size_win = { size.w * camera.get_zoom(), size.h * camera.get_zoom() };
			SDL_FRect rect = { pos_win.x - (size_win.w / 2.0f), pos_win.y - (size_win.h / 2.0f), size_win.w, size_win.h };

			// 绘制
			SDL_SetRenderDrawColor(Main_Renderer, color.r, color.g, color.b, color.a);
			if (is_filled)
				SDL_RenderFillRect(Main_Renderer, &rect);
			else
				SDL_RenderRect(Main_Renderer, &rect);
		}

		// 绘制锚点为左上角的圆形
		static void render_TopLeftCircle(const Camera2D& camera, const Point& pos, float radius, SDL_Color color, bool is_filled = false)
		{
			Point pos_win = camera.world_to_screen(pos);
			float radius_win = radius * camera.get_zoom();
			pos_win.x = pos_win.x + radius_win;
			pos_win.y = pos_win.y + radius_win;

			SDL_SetRenderDrawColor(Main_Renderer, color.r, color.g, color.b, color.a);

			if (is_filled)
			{
				for (float y = -radius_win; y <= radius_win; y += 1.0f)
				{
					float x = sqrtf(radius_win * radius_win - y * y);
					SDL_RenderLine(Main_Renderer, pos_win.x - x, pos_win.y + y, pos_win.x + x, pos_win.y + y);
				}
			}
			else
			{
				float x = radius_win; float y = 0; float err = 0;
				while (x >= y)
				{
					SDL_RenderPoint(Main_Renderer, pos_win.x + x, pos_win.y + y);
					SDL_RenderPoint(Main_Renderer, pos_win.x + y, pos_win.y + x);
					SDL_RenderPoint(Main_Renderer, pos_win.x - y, pos_win.y + x);
					SDL_RenderPoint(Main_Renderer, pos_win.x - x, pos_win.y + y);
					SDL_RenderPoint(Main_Renderer, pos_win.x - x, pos_win.y - y);
					SDL_RenderPoint(Main_Renderer, pos_win.x - y, pos_win.y - x);
					SDL_RenderPoint(Main_Renderer, pos_win.x + y, pos_win.y - x);
					SDL_RenderPoint(Main_Renderer, pos_win.x + x, pos_win.y - y);

					if (err <= 0) { y += 1; err += 2 * y + 1; }
					if (err > 0) { x -= 1; err -= 2 * x + 1; }
				}
			}
		}

		// 绘制锚点为中心的圆形
		static void render_CenterCircle(const Camera2D& camera, const Point& pos, float radius, SDL_Color color, bool is_filled = false)
		{
			Point pos_win = camera.world_to_screen(pos);
			float radius_win = radius * camera.get_zoom();

			SDL_SetRenderDrawColor(Main_Renderer, color.r, color.g, color.b, color.a);

			if (is_filled)
			{
				for (float y = -radius_win; y <= radius_win; y += 1.0f)
				{
					float x = sqrtf(radius_win * radius_win - y * y);
					SDL_RenderLine(Main_Renderer, pos_win.x - x, pos_win.y + y, pos_win.x + x, pos_win.y + y);
				}
			}
			else
			{
				float x = radius_win; float y = 0; float err = 0;
				while (x >= y)
				{
					SDL_RenderPoint(Main_Renderer, pos_win.x + x, pos_win.y + y);
					SDL_RenderPoint(Main_Renderer, pos_win.x + y, pos_win.y + x);
					SDL_RenderPoint(Main_Renderer, pos_win.x - y, pos_win.y + x);
					SDL_RenderPoint(Main_Renderer, pos_win.x - x, pos_win.y + y);
					SDL_RenderPoint(Main_Renderer, pos_win.x - x, pos_win.y - y);
					SDL_RenderPoint(Main_Renderer, pos_win.x - y, pos_win.y - x);
					SDL_RenderPoint(Main_Renderer, pos_win.x + y, pos_win.y - x);
					SDL_RenderPoint(Main_Renderer, pos_win.x + x, pos_win.y - y);

					if (err <= 0) { y += 1; err += 2 * y + 1; }
					if (err > 0) { x -= 1; err -= 2 * x + 1; }
				}
			}
		}

	private:
		ShapeMaker() = default;
		~ShapeMaker() = default;
	};

	// 序列帧动画
	class Animation
	{
	public:
		Animation()
		{
			timer.set_one_shot(false);
			timer.set_wait_time(0.1f);	// 默认帧间隔0.1f
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
		Animation(SDL_Texture* texture, int num_h) : Animation() { this->add_frame(texture, num_h); }
		Animation(Atlas* atlas) : Animation() { this->add_frame(atlas); }

		~Animation() = default;

		//重置动画
		void reset() { timer.restart(); idx_frame = 0; }

		// 设置动画世界坐标
		void set_position(const Point& pos) { this->position = pos; }

		// 设置动画方向
		void set_rotation(float angle) { this->angle = static_cast<double>(angle); }

		// 设置动画是否循环（默认true）
		void set_loop(bool is_loop) { this->is_loop = is_loop; }

		// 设置动画帧间隔（默认0.1）
		void set_interval(float interval) { timer.set_wait_time(interval); }

		// 设置动画是否翻转（默认false）
		void set_flip(bool flag) { this->is_flip = flag; }

		// 设置动画回调函数
		void set_on_finished(std::function<void()> callback) { on_finished = callback; }

		// 更新动画
		void on_update(float delta) { timer.on_update(delta); }

		// 设置动画中心点（默认为纹理中心点，参数范围在0.0~1.0）
		void set_center(float anchor_x, float anchor_y) { this->center = { anchor_x, anchor_y }; }

		// 添加序列帧
		void add_frame(Atlas* atlas)
		{
			for (int i = 0; i < atlas->get_size(); i++)
			{
				SDL_Texture* texture = atlas->get_texture(i);

				float width, height;
				SDL_GetTextureSize(texture, &width, &height);
				SDL_FRect rect_src = { 0, 0, width, height };

				frame_list.emplace_back(texture, rect_src);
			}
		}

		// 添加序列帧
		void add_frame(SDL_Texture* texture, int num_h)
		{
			float width, height;
			SDL_GetTextureSize(texture, &width, &height);	// 获取序列帧宽高

			float width_frame = width / num_h;	// 获取单张序列帧宽高
			for (int i = 0; i < num_h; i++)
			{
				SDL_FRect rect_src{};
				rect_src.x = i * width_frame, rect_src.y = 0;
				rect_src.w = width_frame, rect_src.h = height;

				frame_list.emplace_back(texture, rect_src);		// 将序列帧加入列表
			}
		}

		// 渲染动画
		void on_render(const Camera2D& camera) const
		{
			const Frame& frame = frame_list[idx_frame];
			const Point& pos_camera = camera.get_position();

			SDL_FRect rect_dst = { position.x, position.y, frame.rect_src.w, frame.rect_src.h };
			camera.render_texture(frame.texture, &frame.rect_src, &rect_dst, angle, center, is_flip);
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

	// 精灵基类
	class Sprite
	{
	public:
		Sprite() = default;
		virtual ~Sprite() = default;

		// 获取速度
		const Vector2& get_velocity() const { return this->velocity; }

		// 设置速度
		void set_velocity(const Vector2& velocity) { this->velocity = velocity; }

		// 获取方向
		const float get_direction() const { return this->direction; }

		// 设置方向
		void set_direction(float dir) { this->direction = dir; }

		// 获取世界坐标
		const Point& get_position() const { return this->position; }

		// 设置世界坐标
		void set_position(const Point& pos) { this->position = pos; }

		// 获取精灵大小
		const Size& get_size() const { return this->size; }

		// 设置精灵大小
		void set_size(const Size& size) { this->size = size; }

		// 设置渲染层
		void set_render_layer(RenderLayer layer) { this->render_layer = layer; }

		// 获取渲染层
		RenderLayer get_render_layer() const { return this->render_layer; }

		// 获取自身碰撞箱
		CollisionBox* get_collision_box() const { return this->hit_box; }

		// 面向指定坐标点
		void point_torwards(const Point& target)
		{
			float dx = target.x - position.x;
			float dy = target.y - position.y;
			this->direction = maths::rad_to_deg(std::atan2f(dy, dx));
		}

		// 面向鼠标指针
		void point_mousePointer(const Camera2D& camera)
		{
			float mouse_x, mouse_y;
			SDL_GetMouseState(&mouse_x, &mouse_y);
			auto mouse_world_pos = camera.screen_to_world(Point(mouse_x, mouse_y));

			float dx = mouse_world_pos.x - position.x;
			float dy = mouse_world_pos.y - position.y;

			// 旋转方向 = 弧度转角度(atan2(目标坐标 - 自身坐标))
			this->direction = maths::rad_to_deg(std::atan2f(dy, dx));
		}

		virtual void on_update(float delta) {};				// 更新逻辑
		virtual void on_render(const Camera2D& camera) {};	// 渲染画面
		virtual void on_input(const SDL_Event& event) {};	// 处理输入
		virtual void reset_property() {};					// 重置角色属性

	protected:
		Point position;								  // 世界坐标
		Size size = { 0, 0 };						  // 精灵大小
		Vector2 velocity;							  // 速度
		float direction = 0.0f;						  // 方向
		CollisionBox* hit_box = nullptr;			  // 自身碰撞箱
		RenderLayer render_layer = RenderLayer::None; // 渲染层
	};

	// 标签
	class Label : public Sprite
	{
	public:
		Label() { this->render_layer = RenderLayer::Label; }
		~Label() = default;

		Label(const Point& position, TTF_Font* font, SDL_Color color, float size, const std::string& info) 
			: label_color(color), label_font(font), ptsize(size), label_info(info) 
		{
			this->position = position;
			this->render_layer = RenderLayer::Label;
		}

		// 设置标签字体
		void set_font(TTF_Font* font) { this->label_font = font; }

		// 设置标签颜色
		void set_color(SDL_Color color) { this->label_color = color; }

		// 设置标签大小
		void set_size(float ptsize) { this->ptsize = ptsize; }

		// 设置标签文本内容
		void set_info(const std::string& info) { this->label_info = info; }

		void on_render(const Camera2D& camera) override
		{ camera.render_text(position, label_font, label_color, ptsize, label_info.c_str()); }

	private:
		TTF_Font* label_font = nullptr;	// 字体
		SDL_Color label_color{};		// 颜色
		float ptsize = 0.0f;			// 大小
		std::string label_info;			// 文本内容
	};

	// 按钮（仅适用于UI层）
	class Button : public Sprite
	{
		// 按钮特效对组
		using effect_pair = std::pair<SDL_Texture*, Mix_Chunk*>;
	public:
		Button() = default;
		~Button() = default;

		Button(const Point& pos, const Size& size)
		{
			this->position = pos;
			this->size = size;
			this->render_layer = RenderLayer::UI;
			this->effects["normal"] = std::make_pair(nullptr, nullptr);
			this->effects["hover"] = std::make_pair(nullptr, nullptr);
			this->effects["click"] = std::make_pair(nullptr, nullptr);
		}

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
				throw custom_error(u8"Button Argument Error", u8"Argument texture cannot be nullptr!");

			effects["normal"].first = normal;
			effects["hover"].first = hover;
			effects["click"].first = click;
			this->current_texture = effects["normal"].first;	// 默认显示正常状态纹理
		}

		// 设置按钮音效（分别为：悬停、被点击）
		void set_audio(Mix_Chunk* hover, Mix_Chunk* click)
		{
			effects["hover"].second = hover;
			effects["click"].second = click;
		}

		// 渲染按钮
		void on_render(const Camera2D& camera) override
		{
			if (!current_texture) return;	// 如果没有设置纹理则不渲染
			SDL_FRect rect_dst_win = { position.x, position.y, size.w, size.h };
			camera.render_texture(current_texture, nullptr, &rect_dst_win, 0);
		}

		// 处理输入事件
		void on_input(const SDL_Event& event) override
		{
			// 判断鼠标是否在按钮上悬停
			bool in_range_x = event.motion.x >= position.x && event.motion.x <= position.x + size.w;
			bool in_range_y = event.motion.y >= position.y && event.motion.y <= position.y + size.h;

			if (event.type == SDL_EVENT_MOUSE_MOTION)	// 鼠标悬停在按钮上
			{
				if (in_range_x && in_range_y)
				{
					if (effects["hover"].second && !is_first_hover)
						Mix_PlayChannel(-1, effects["hover"].second, 0);

					this->is_first_hover = true;
					this->current_texture = effects["hover"].first;
					SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER));
				}
				else
				{
					this->is_first_hover = false;
					this->current_texture = effects["normal"].first;
					SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT));
				}
			}
			if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT) // 按钮被点击
			{
				if (in_range_x && in_range_y)
				{
					if (effects["click"].second)
						Mix_PlayChannel(-1, effects["click"].second, 0);

					this->current_texture = effects["click"].first;
					if (on_click) this->on_click();
				}
			}
			if (event.type == SDL_EVENT_MOUSE_BUTTON_UP && event.button.button == SDL_BUTTON_LEFT) // 按钮被释放
			{
				if (in_range_x && in_range_y)
					this->current_texture = effects["hover"].first;
				else
					this->current_texture = effects["normal"].first;
			}
		}

	private:
		bool is_first_hover = false;							// 是否首次悬停
		SDL_Texture* current_texture = nullptr;					// 当前显示的纹理
		std::unordered_map<std::string, effect_pair> effects;	// 效果列表
		std::function<void()> on_click;							// 点击回调函数
	};

	// 场景基类
	class Scene
	{
	public:
		Scene() = default;									 /* 构造函数处初始化实例化对象 */
		virtual ~Scene() { this->destroy_all_sprites(); }	 /* 析函数出销毁实例化对象 */

		// 注册新精灵
		void register_sprite(const std::string& name, Sprite* new_sprite) 
		{
			sprites_pool.emplace(name, new_sprite);		// 加入精灵池

			RenderLayer layer_target = new_sprite->get_render_layer();
			if (layer_target == RenderLayer::None)
				throw custom_error(u8"SceneManager Error", u8"sprite “" + name + u8"” render layer no setting!");
			this->layered_sprite_pool[layer_target].push_back(new_sprite); // 加入分层渲染池

			needs_sorting = true;	// 标记需要排序
		}

		// 销毁目标精灵
		void destroy_sprite(Sprite* sprite)
		{
			// 依次遍历直到找到目标精灵
			for (auto iterator = sprites_pool.begin(); iterator != sprites_pool.end(); iterator++)
			{
				// 如果找到了并且存在目标精灵
				if (iterator->second == sprite && iterator->second)
				{
					this->sprites_pool.erase(iterator);
					delete sprite; return;
				}
			}
		}

		// 获取精灵（返回指定类型模板的对象指针）
		template <typename _CvtTy = Sprite>
		_CvtTy* find_sprite(const std::string& name)
		{
			auto range = sprites_pool.equal_range(name);
			auto find_it = sprites_pool.find(name);

			if (find_it == sprites_pool.end())	// 检测是否找到
				throw custom_error(u8"SceneManager Error", u8"Sprite “" + name + u8"” is not found!");
			else if (std::distance(range.first, range.second) > 1)	// 检测唯一性
				throw custom_error(u8"SceneManager Error", u8"“" + name + u8"” is unclear, Please check name's singleness");

			return (_CvtTy*)find_it->second;
		}

		// 获取精灵组（返回一个范围迭代器）
		auto find_group(const std::string& name)
		{
			auto range = sprites_pool.equal_range(name);
			if (range.first == range.second)	// 检测是否找到
				throw custom_error(u8"SceneManager Error", u8"Sprites group “" + name + u8"” is not found!");
			return range;
		}

		virtual void on_enter() {};											  // 进入场景（对角色属性的重置）
		virtual void on_update(float delta) {};								  // 更新场景
		virtual void on_render(const Camera2D& game,  const Camera2D& ui) {}; // 渲染场景（一个负责渲染游戏，一个负责渲染UI）
		virtual void on_input(const SDL_Event& event) {};					  // 输入事件处理
		virtual void on_exit() {};											  // 退出场景（不要销毁对象）

	protected:
		// 更新所有精灵
		void sprites_update(float delta)
		{
			for (auto& [name, sprite] : sprites_pool)
				sprite->on_update(delta);
			needs_sorting = true;
		}

		// 渲染所有精灵
		void sprites_render(const Camera2D& game, const Camera2D& ui)
		{
			// 如果需要排序则进行排序
			if (needs_sorting && is_enabled_y_sort)
			{
				for (auto& [layer, sprites] : this->layered_sprite_pool)
				{
					if (this->layer_need_y_sort(layer))
					{
						// 按Y轴从小到大排序
						std::sort(sprites.begin(), sprites.end(),
							[](const Sprite* a, const Sprite* b) { return a->get_position().y < b->get_position().y; });
					}
				}
			}
			needs_sorting = false;

			// 按渲染层顺序渲染
			this->render_layer(RenderLayer::Background, game, ui);
			this->render_layer(RenderLayer::Frontground, game, ui);
			this->render_layer(RenderLayer::Label, game, ui);
			this->render_layer(RenderLayer::GameObject, game, ui);
			this->render_layer(RenderLayer::Effect, game, ui);
			this->render_layer(RenderLayer::UI, game, ui);
		}

		// 处理所有精灵的输入事件
		void sprites_input(const SDL_Event& event)
		{
			for (auto& [name, sprite] : sprites_pool)
				sprite->on_input(event);
		}

		// 销毁所有精灵
		void destroy_all_sprites()
		{
			// 为空则直接返回
			if (sprites_pool.empty()) return;

			// 释放精灵资源
			for (auto& [name, sprite] : sprites_pool)
				delete sprite;

			sprites_pool.clear();
			layered_sprite_pool.clear();
		}

		// 设置是否启用Y轴排序(默认是true)
		void set_enable_y_sort(bool flag) { this->is_enabled_y_sort = flag; }

	private:
		// 渲染指定渲染层的所有精灵
		void render_layer(RenderLayer layer, const Camera2D& game, const Camera2D& ui)
		{
			// 如果该渲染层不存在则直接返回
			if (layered_sprite_pool.find(layer) == layered_sprite_pool.end())
				return;

			// 选择使用的摄像机
			const Camera2D& used_camera = (layer == RenderLayer::UI) ? ui : game;
			for (Sprite* sprite : layered_sprite_pool[layer])
			{
				int screen_w, screen_h;
				SDL_GetWindowSize(Main_Window, &screen_w, &screen_h);	// 获取窗口宽高
				bool is_out_of_view_x = (used_camera.world_to_screen(sprite->get_position()).x < -sprite->get_size().w || used_camera.world_to_screen(sprite->get_position()).x > screen_w + sprite->get_size().w);
				bool is_out_of_view_y = (used_camera.world_to_screen(sprite->get_position()).y < -sprite->get_size().h || used_camera.world_to_screen(sprite->get_position()).y > screen_h + sprite->get_size().h);
				
				// 如果精灵在摄像机外则跳过渲染
				if (is_out_of_view_x || is_out_of_view_y) 
					continue;

				sprite->on_render(used_camera);
			}
		}

		// 判断某个渲染层是否需要按Y轴排序
		bool layer_need_y_sort(RenderLayer layer) const
		{
			return (layer == RenderLayer::GameObject || layer == RenderLayer::Label
				|| layer == RenderLayer::UI);
		}

	private:
		std::unordered_multimap<std::string, Sprite*> sprites_pool;					// 精灵池（支持重名）
		std::unordered_map<RenderLayer, std::vector<Sprite*>> layered_sprite_pool;	// 分层渲染池
		bool needs_sorting = false;													// 是否需要排序
		bool is_enabled_y_sort = true;												// 是否启用Y轴排序
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
				delete anim.second;		// 释放动画资源

			animation_pool.clear();
			current_animation = nullptr;
		}

		// 注册动画
		void register_animation(const std::string& name, Animation* anim)
		{
			if (animation_pool.find(name) != animation_pool.end())
				throw custom_error(error_title, u8"Animation “" + name + u8"” is already exist!");

			animation_pool[name] = anim;
		}

		// 切换动画
		void switch_to(const std::string& name)
		{
			if (animation_pool.find(name) == animation_pool.end())
				throw custom_error(error_title, u8"Animation “" + name + u8"” is not exist!");

			this->current_animation = animation_pool[name];
		}

		// 设置当前动画，并从头播放
		void set_animation(const std::string& name)
		{
			if (animation_pool.find(name) == animation_pool.end())
				throw custom_error(error_title, u8"Animation “" + name + u8"” is not exist!");

			this->current_animation = animation_pool[name];
			this->current_animation->reset();
		}

		// 获取当前动画
		Animation* get_current()
		{
			if (current_animation)
				return current_animation;
			else
				throw custom_error(error_title, u8"“current_animation” is a nullptr!");
		}

		// 更新动画
		void on_update(float delta)
		{
			if (current_animation)
				current_animation->on_update(delta);
			else
				throw custom_error(error_title, u8"“current_animation” is a nullptr!");
		}

		// 渲染动画
		void on_render(const Camera2D& camera)
		{
			if (current_animation)
				current_animation->on_render(camera);
			else
				throw custom_error(error_title, u8"“current_animation” is a nullptr!");
		}

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

		~StateMachine()
		{
			for (auto& state : state_pool)
				delete state.second;	// 释放状态池内所有状态节点

			state_pool.clear();
			current_state = nullptr;
		}

		// 更新状态机
		void on_update(float delta)
		{
			if (!current_state)
			{
				std::string info = u8"Current state is nullptr! Please check if the status node is initialized";
				throw custom_error(u8"StateMachine Error", info.c_str());
			}

			if (need_init)	// 如果需要初始化状态机
			{
				current_state->on_enter();
				need_init = false;		// 初始化完成
			}

			current_state->on_update(delta);
		}

		// 设置初状态节点（用于初始化）
		void set_entry(const std::string& name)
		{
			// 如果未找到目标状态
			if (state_pool.find(name) == state_pool.end())
			{
				std::string info = u8"State “" + name + u8"” is not found!";
				throw custom_error(u8"StateMachine Error", info);
			}

			current_state = state_pool[name];
		}

		// 切换状态节点
		void switch_to(const std::string& name)
		{
			// 如果未找到目标状态
			if (state_pool.find(name) == state_pool.end())
			{
				std::string info = u8"State “" + name + u8"” is not found!";
				throw custom_error(u8"StateMachine Error", info);
			}

			if (current_state) current_state->on_exit();
			current_state = state_pool[name];
			current_state->on_enter();
		}

		// 注册状态节点
		void register_state(const std::string& name, StateNode* state)
		{
			// 如果该状态已存在
			if (state_pool.find(name) != state_pool.end())
			{
				std::string info = u8"State “" + name + u8"” is already exist!";
				throw custom_error(u8"StateMachine Error", info);
			}

			state_pool[name] = state;		// 注册状态节点
		}

	private:
		bool need_init = true;									// 是否需要初始化状态机
		StateNode* current_state = nullptr;						// 当前状态节点
		std::unordered_map<std::string, StateNode*> state_pool;	// 状态池
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
		inline void load_resources(const char* directory)
		{
			// 判断文件是否存在
			if (!std::filesystem::exists(directory))
			{
				std::string info = u8"Dictionary “" + std::string(directory) + u8"” is an error dictionary!";
				throw custom_error(u8"ResourcesManager Error", info);
			}

			// 遍历目标文件内部所有文件
			for (const auto& entry : std::filesystem::recursive_directory_iterator(directory))
			{
				if (entry.is_regular_file())	// 如果是有效文件
				{
					const std::filesystem::path& path = entry.path();
					if (path.extension() == ".bmp" || path.extension() == ".png" || path.extension() == ".jpg")
					{
						SDL_Texture* texture = IMG_LoadTexture(Main_Renderer, path.u8string().c_str());
						texture_pool[path.stem().u8string()] = texture;
					}
					if (path.extension() == ".wav")
					{
						Mix_Chunk* audio = Mix_LoadWAV(path.u8string().c_str());
						audio_pool[path.stem().u8string()] = audio;
					}
					if (path.extension() == ".mp3" || path.extension() == ".ogg")
					{
						Mix_Music* music = Mix_LoadMUS(path.u8string().c_str());
						music_pool[path.stem().u8string()] = music;
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
				throw custom_error(u8"ResourcesManager Error", info);
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
				throw custom_error(u8"ResourcesManager Error", info);
			}

			return audio_pool[name];
		}

		// 查找音乐资源
		inline Mix_Music* find_music(const std::string& name)
		{
			// 未找到
			if (music_pool.find(name) == music_pool.end())
			{
				std::string info = u8"Music “" + name + u8"” is not found!";
				throw custom_error(u8"ResourcesManager Error", info);
			}

			return music_pool[name];
		}

		// 查找字体资源
		inline TTF_Font* find_font(const std::string& name)
		{
			// 未找到
			if (font_pool.find(name) == font_pool.end())
			{
				std::string info = u8"Font “" + name + u8"” is not found!";
				throw custom_error(u8"ResourcesManager Error", info);
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
			for (auto& music : music_pool)
				printf("%s\n", music.first.c_str());

			printf("\n已加载的字体资源如下 (共%d个):\n", (int)font_pool.size());
			for (auto& font : font_pool)
				printf("%s\n", font.first.c_str());
			printf("=============================================================\n");
		}

	private:
		ResourcesManager() = default;
		~ResourcesManager() = default;

	private:
		static ResourcesManager* m_instance;							// 资源管理器单例

		std::unordered_map<std::string, SDL_Texture*> texture_pool;		// 纹理资源池
		std::unordered_map<std::string, Mix_Chunk*> audio_pool;			// 音频资源池
		std::unordered_map<std::string, Mix_Music*> music_pool;			// 音乐资源池
		std::unordered_map<std::string, TTF_Font*> font_pool;			// 字体资源池
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
				throw custom_error(u8"SceneManager Error", u8"Scene “" + name + u8"” is not exist!");

			current_scene = scene_pool[name];
			current_scene->on_enter();
		}

		// 切换场景
		inline void switch_to(const std::string& name)
		{
			current_scene->on_exit();

			// 如果场景池中不存在该场景
			if (scene_pool.find(name) == scene_pool.end())
				throw custom_error(u8"SceneManager Error", u8"Scene “" + name + u8"” is not exist!");

			current_scene = scene_pool[name];
			current_scene->on_enter();
		}

		// 注册新场景
		inline void register_scene(const std::string& name, Scene* scene)
		{
			if (scene_pool.find(name) != scene_pool.end())
				throw custom_error(u8"SceneManager Error", u8"Scene “" + name + u8"” is already exist!");
			scene_pool[name] = scene;
		}

		// 获取场景
		inline Scene* find_scene(const std::string& name)
		{
			// 如果场景池中不存在该场景
			if (scene_pool.find(name) == scene_pool.end())
				throw custom_error(u8"SceneManager Error", u8"Scene “" + name + u8"” is not exist!");

			return scene_pool[name];
		}

		// 更新当前场景
		void on_update(float delta)
		{
			if (current_scene)
				current_scene->on_update(delta);
			else
				throw custom_error(u8"SceneManager Error", u8"“current_scene” is a nullptr!");
		}

		// 渲染当前场景
		void on_render(const Camera2D& camera_game, const Camera2D& camera_ui)
		{
			if (current_scene)
				current_scene->on_render(camera_game, camera_ui);
			else
				throw custom_error(u8"SceneManager Error", u8"“current_scene” is a nullptr!");
		}

		// 处理输入事件
		void on_input(const SDL_Event& event)
		{
			if (current_scene)
				current_scene->on_input(event);
			else
				throw custom_error(u8"SceneManager Error", u8"“current_scene” is a nullptr!");
		}

	private:
		SceneManager() = default;
		~SceneManager() = default;

	private:
		static SceneManager* m_instance;					// 场景管理器单例
		Scene* current_scene = nullptr;						// 当前场景
		std::unordered_map<std::string, Scene*> scene_pool;	// 场景池
	};
	inline SceneManager* SceneManager::m_instance = nullptr;
	typedef SceneManager Director;

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
		inline void process_collision()
		{
			for (auto collision_box_src : collision_box_list)
			{
				if (!collision_box_src->enabled || collision_box_src->layer_dst == CollisionLayer::None)
					continue;

				for (auto collision_box_dst : collision_box_list)
				{
					if (!collision_box_dst->enabled || collision_box_src == collision_box_dst
						|| !(collision_box_src->layer_dst & collision_box_dst->layer_src))
						continue;

					// 横向碰撞条件：两碰撞箱的maxX - 两碰撞箱的minX <= 两碰撞箱的宽度之和
					float max_x = std::max(collision_box_src->position.x + collision_box_src->size.w / 2.0f, collision_box_dst->position.x + collision_box_dst->size.w / 2.0f);
					float min_x = std::min(collision_box_src->position.x - collision_box_src->size.w / 2.0f, collision_box_dst->position.x - collision_box_dst->size.w / 2.0f);
					bool is_collide_x = (max_x - min_x <= collision_box_src->size.w + collision_box_dst->size.w);

					// 纵向碰撞条件：两碰撞箱的maxY - 两碰撞箱的minY <= 两碰撞箱的高度之和
					float max_y = std::max(collision_box_src->position.y + collision_box_src->size.h / 2.0f, collision_box_dst->position.y + collision_box_dst->size.h / 2.0f);
					float min_y = std::min(collision_box_src->position.y - collision_box_src->size.h / 2.0f, collision_box_dst->position.y - collision_box_dst->size.h / 2.0f);
					bool is_collide_y = (max_y - min_y <= collision_box_src->size.h + collision_box_dst->size.h);

					// 如果横向/纵向都成立，且目标碰撞箱存在回调函数，则执行回调函数
					if (is_collide_x && is_collide_y && collision_box_dst->on_collide)
						collision_box_dst->on_collide(collision_box_src->layer_src);
				}
			}
		}

		// 调试碰撞箱
		inline void debug_collision_box(const Camera2D& camera)
		{
			for (auto collision_box : collision_box_list)
			{
				// 定义碰撞箱渲染颜色
				SDL_Color color;
				if (collision_box->enabled)
					color = { 255,195,195,255 };
				else
					color = { 115,155,175,255 };

				ShapeMaker::render_CenterRect(camera, collision_box->position, collision_box->size, color); // 绘制碰撞箱
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

	// 框架类
	class Framework
	{
	public:
		Framework() = default;
		~Framework() = default;

		// 设置回调函数
		void set_callback(std::function<void(float)> update, std::function<void()> render,
			std::function<void(const SDL_Event&)> input)
		{
			this->update_callback = update;
			this->render_callback = render;
			this->input_callback = input;
		}

		// 初始化图形系统
		void Init_Graphic(const std::string& title, int w, int h, WindowFlags flags = WindowFlags::Shown)
		{
			try
			{
				// 初始化SDL相关模块
				SDL_Init(SDL_INIT_EVERYTHING);
				TTF_Init();

				SDL_AudioSpec spec = { SDL_AUDIO_S32, 2, 44100 };
				Mix_Init(MIX_INIT_EVERYTHING);
				Mix_OpenAudio(NULL, &spec);

				// 初始化窗口
				if (Main_Window) return; // 如果窗口已存在则不再创建
				Main_Window = SDL_CreateWindow(title.c_str(), w, h, (SDL_WindowFlags)flags);
				if (!Main_Window) throw std::runtime_error(u8"Failed to create Main_Window!");

				// 初始化渲染器
				if (Main_Renderer) return;	// 如果渲染器已存在则不再创建
				Main_Renderer = SDL_CreateRenderer(Main_Window, nullptr);
				if (!Main_Renderer) throw std::runtime_error(u8"Failed to create Main_Renderer!");

				// 初始化文本引擎
				if (Main_TextEngine) return; // 如果引擎已存在则不再创建
				Main_TextEngine = TTF_CreateRendererTextEngine(Main_Renderer);
				if (!Main_TextEngine) throw std::runtime_error(u8"Failed to create Main_TextEngine!");
			}
			catch (const std::exception& e)
			{
				Show_MessageBox(MsgBoxFlags::Error, "SDL Error", e.what());
				this->Release_Graphic();
				exit(EXIT_FAILURE);
			}
		}

		// 主循环
		void main_loop(int fps)
		{
			try
			{
				bool is_running = true;
				Clock::set_fps(fps);

				// 主循环
				while (is_running)
				{
					// 帧开始
					Clock::start_frame();

					// 处理输入
					while (SDL_PollEvent(&Main_Event))
					{
						if (Main_Event.type == SDL_EVENT_QUIT)
							is_running = false;
						this->input_callback(Main_Event);
					}

					// 处理数据
					CollisionManager::instance()->process_collision();
					this->update_callback(Clock::get_DeltaTime());

					// 更新画面
					SDL_RenderClear(Main_Renderer);
					this->render_callback();
					SDL_RenderPresent(Main_Renderer);

					// 帧结束
					Clock::end_frame();
				}
			}
			catch (const fce::custom_error& e)
			{
				Show_MessageBox(MsgBoxFlags::Error, e.title(), e.what());
				this->Release_Graphic();
				exit(EXIT_FAILURE);
			}
			catch (const std::exception& e)
			{
				Show_MessageBox(MsgBoxFlags::Error, "SDL Error", e.what());
				this->Release_Graphic();
				exit(EXIT_FAILURE);
			}
			catch (...)
			{
				Show_MessageBox(MsgBoxFlags::Error, "Unknown Error", "An unknown error has occurred!");
				this->Release_Graphic();
				exit(EXIT_FAILURE);
			}
		}

		// 释放图形系统
		void Release_Graphic()
		{
			// 释放内置资源
			if (Main_TextEngine) { TTF_DestroyRendererTextEngine(Main_TextEngine); }
			if (Main_Renderer) { SDL_DestroyRenderer(Main_Renderer); }
			if (Main_Window) { SDL_DestroyWindow(Main_Window); }

			// 退出SDL第三方库
			TTF_Quit();
			Mix_Quit();
			SDL_Quit();
		}

	private:
		std::function<void(float)> update_callback;	// 更新回调函数
		std::function<void()> render_callback;		// 渲染回调函数
		std::function<void(const SDL_Event&)> input_callback;	// 事件回调函数
	};
};
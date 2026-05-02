module;

#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <thread>
#include <typeindex>
#include <unordered_map>

#include <SDL3/SDL.h>

export module FlexCore:FCE_Utils;

import :FCE_BaseSetup;
import :FCE_BaseType;

export namespace fce
{
	// 进程错误处理
	class custom_error
	{
	public:
		custom_error(const std::u8string& title, const std::u8string& info)
			: u8_error_title(title), u8_error_info(info) {}

		custom_error(const char8_t* title, const char8_t* info)
			: u8_error_title(title), u8_error_info(info) {}

		custom_error(const std::string& title, const std::string& info)
			: error_title(title), error_info(info) {}
		
		custom_error(const char* title, const char* info)
			: error_title(title), error_info(info) {}

		~custom_error() = default;

		// 获取错误标题
		const char* title() const noexcept 
		{
			if (!u8_error_title.empty())
				return (const char*)u8_error_title.c_str();
			else
				return error_title.c_str();
		}

		// 获取错误信息
		const char* what() const noexcept 
		{ 
			if (!u8_error_info.empty())
				return (const char*)u8_error_info.c_str();
			else
				return error_info.c_str();
		}

	private:
		std::u8string u8_error_title;	// 错误标题u8格式
		std::u8string u8_error_info;	// 错误信息u8格式
		std::string error_title;		// 错误标题
		std::string error_info;			// 错误信息
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
	Random Random::m_instance;

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
		static void clear(const std::string& fname)
		{
			std::ifstream check = std::ifstream(fname + ".kvp", std::ios::in);
			if (!check.is_open())
			{
				std::cerr << "DataClear Error: “" << fname + ".kvp" << "” is not found!" << std::endl;
				return;
			}
			check.close();

			std::ofstream cleaner = std::ofstream(fname + ".kvp", std::ios::trunc);
			cleaner.close();
		}

		// 保存文件（序列化格式）
		template <typename _Ty>
		static void save(const std::string& fname, const std::string& key, _Ty val, SvMod mod = Append)
		{
			bool is_std_type = (type_map.find(std::type_index(typeid(_Ty))) != type_map.end());	// 判断是否为标准类型
			bool is_std_custom = (std::is_trivially_copyable_v<_Ty> && std::is_standard_layout_v<_Ty>);	// 判断是否是合法自定义类型

			// 如果二者都不是，则报错
			if (!is_std_type && !is_std_custom)
			{
				std::string info = std::string("“") + typeid(_Ty).name() + std::string("”");
				std::cerr << "DataSave Error: Unsupported data type: " << info << std::endl;
				return;
			}

			std::ofstream writer = std::ofstream(fname + ".kvp", std::ios::binary | mod);

			size_t key_len = key.size();
			writer.write((const char*)&key_len, sizeof(size_t));	// 写入键的大小
			writer.write(key.c_str(), key_len);	// 写入键的内容
			writer.write((const char*)&type_map[std::type_index(typeid(_Ty))], sizeof(DataType)); // 写入类型
			serialization(writer, key, val);	// 写入值

			writer.close();
		}

		// 读取文件
		template <typename _Ty>
		static _Ty read(const std::string& fname, const std::string& key)
		{
			bool is_std_type = (type_map.find(std::type_index(typeid(_Ty))) != type_map.end()); // 判断是否为标准类型
			bool is_std_custom = (std::is_trivially_copyable_v<_Ty> && std::is_standard_layout_v<_Ty>); // 判断是否是合法自定义类型

			// 如果二者都不是，则报错
			if (!is_std_type && !is_std_custom)
			{
				std::string info = std::string("“") + typeid(_Ty).name() + std::string("”");
				std::cerr << "DataRead Error: Unsupported data type: " << info << std::endl;
				return _Ty();
			}
			if constexpr (std::is_same_v<_Ty, const char*>)	// 禁止const char*传参
				throw custom_error("DataRead Error", "Can't pass <const char*> return variable\nPlease use std::string!");

			std::ifstream reader = std::ifstream(fname + ".kvp", std::ios::in | std::ios::binary);
			if (!reader.is_open())	// 打开失败
			{
				std::cerr << "DataRead Error: Cannot open file: “" << fname << ".kvp”!" << std::endl;
				return _Ty();
			}

			_Ty ret_value = _Ty();	// 返回值
			while (reader.peek() != EOF)	// 遍历文件
			{
				// 读取键
				std::string _Key; size_t key_len;
				reader.read((char*)&key_len, sizeof(key_len));	// 读取键的大小
				_Key.resize(key_len);
				reader.read(_Key.data(), key_len);		// 读取键的内容

				// 读取类型
				DataType cur_type;
				reader.read((char*)&cur_type, sizeof(DataType));

				// 读取类型大小
				size_t type_size;
				reader.read((char*)&type_size, sizeof(size_t));

				if (_Key == key && cur_type == type_map[std::type_index(typeid(_Ty))])
				{
					deserialization(reader, ret_value, cur_type);
					return ret_value;
				}
				else
					reader.ignore(type_size);
			}

			std::cerr << "DataRead Error: Cannot found key of value is “" << key << "” in the “" << fname + ".kvp”" << std::endl;
			return _Ty();
		}

	private:
		enum DataType : uint8_t // 数据类型
		{
			SHORT = 1U,	// short
			USHORT,		// unsigned short
			INT,		// int
			UINT,		// unsigned int
			FLOAT,		// float
			DOUBLE,		// double
			BOOL,		// bool
			CHAR,		// char
			UCHAR,		// unsigned char
			LONG,		// long
			LLONG,		// long long
			ULLONG,		// unsigned long long
			STRING,		// string
			COSTUM		// 自定义
		};
		static std::unordered_map<std::type_index, DataType> type_map;	// 类型映射表

	private:
		Data() = default;
		~Data() = default;

		// 序列化
		template <typename _Ty>
		static void serialization(std::ostream& ofs, const std::string& key, _Ty val)
		{
			// 写入值，是string需要特殊处理
			if constexpr (std::is_same_v<_Ty, std::string> || std::is_same_v<_Ty, const char*>)
			{
				std::string tmp_str;
				if constexpr (std::is_same_v<_Ty, const char*>) tmp_str = std::string(val);
				else tmp_str = val;

				size_t len = tmp_str.size();
				size_t type_size = sizeof(size_t) + len;	// 一共多大

				ofs.write((const char*)&type_size, sizeof(size_t));	// 写入总大小
				ofs.write((const char*)&len, sizeof(size_t));	// 写入字符串长度
				ofs.write(tmp_str.c_str(), len);	// 写入字符串内容
			}
			else	// 不是string
			{
				_Ty tmp = val; size_t type_size = sizeof(_Ty);
				ofs.write((const char*)&type_size, sizeof(size_t));	// 写入类型大小
				ofs.write((const char*)&tmp, type_size);	// 写入值
			}
		}

		// 反序列化
		template <typename _Ty>
		static void deserialization(std::istream& ifs, _Ty& val, DataType type)
		{
			if (type == STRING)	// 是string
			{
				std::string tmp_str; size_t len;
				ifs.read((char*)&len, sizeof(size_t));
				tmp_str.resize(len);
				ifs.read(tmp_str.data(), len);

				if constexpr (std::is_same_v<_Ty, std::string>)
					val = tmp_str;
			}
			else if (type != STRING)		// 不是string
				ifs.read((char*)&val, sizeof(_Ty));
		}
	};
	std::unordered_map<std::type_index, Data::DataType> Data::type_map = {
		{ std::type_index(typeid(short)), SHORT },
		{ std::type_index(typeid(unsigned short)), USHORT },
		{ std::type_index(typeid(int)), INT },
		{ std::type_index(typeid(unsigned int)), UINT },
		{ std::type_index(typeid(long)), LONG },
		{ std::type_index(typeid(long long)), LLONG },
		{ std::type_index(typeid(unsigned long long)), ULLONG },
		{ std::type_index(typeid(float)), FLOAT },
		{ std::type_index(typeid(double)), DOUBLE },
		{ std::type_index(typeid(bool)), BOOL },
		{ std::type_index(typeid(char)), CHAR },
		{ std::type_index(typeid(unsigned char)), UCHAR },
		{ std::type_index(typeid(std::string)), STRING },
		{ std::type_index(typeid(const char*)), STRING }
	};

	// 全局游戏时钟
	class Clock
	{
		using clock_t = std::chrono::steady_clock;
		using ms_t = std::chrono::milliseconds;

	public:
		// 起始当前帧计时
		static void start_frame()
		{
			auto _past_time = clock_t::now() - m_instance.last_time;
			m_instance.delta_time = static_cast<double>(std::chrono::duration_cast<ms_t>(_past_time).count());
			m_instance.last_time = clock_t::now();	// 更新上次刷新时间
		}

		// 结束当前帧计时
		static void end_frame()
		{
			auto _cur_time = clock_t::now();		// 获取当前时间
			auto _elp_time = std::chrono::duration_cast<ms_t>(_cur_time - m_instance.last_time).count(); // 经过时间

			// 休眠指定时间
			if (_elp_time < m_instance.target_dur && m_instance.is_frame_limited)
				std::this_thread::sleep_for(ms_t(m_instance.target_dur - _elp_time));
			else
				std::this_thread::sleep_for(ms_t(m_instance.MIN_DUR));
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
			auto _cur_time = clock_t::now();
			auto _global_time = std::chrono::duration_cast<ms_t>(_cur_time - m_instance.global_start_time).count();
			return static_cast<float>(_global_time) / 1000.0f;	// 转换为秒
		}

		// 重置全局经过时间
		static void restart_global_time() { m_instance.global_start_time = clock_t::now(); }

		// 设置是否启用帧率限制（默认是true）
		static void set_frame_limit(bool flag) { m_instance.is_frame_limited = flag; }

		// 获取FPS
		static size_t get_fps() { return static_cast<size_t>(1000 / m_instance.delta_time); }

		// 获取帧间隔
		static float get_DeltaTime()
		{
			double _normal_delta = m_instance.delta_time / 1000.0;	// 转换为秒
			return static_cast<float>(_normal_delta * m_instance.time_scale);
		}

		// 设置时间缩放
		static void set_time_scale(double scale)
		{
			if (scale <= 0.0f) m_instance.time_scale = 0.1f;
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
			SDL_DisplayID _display = SDL_GetPrimaryDisplay();
			if (!_display) return 0;

			// 获取当前显示模式
			const SDL_DisplayMode* _mode = SDL_GetCurrentDisplayMode(_display);
			if (!_mode) return 0;

			// 返回刷新率（如果有效，否则返回默认值60）
			return (_mode->refresh_rate > 0) ? (int)_mode->refresh_rate : 60;
		}

	private:
		static Clock m_instance;		// 单例实例
		int target_fps = 60;			// 目标FPS
		int target_dur = 1000 / 60;		// 目标帧间隔
		double delta_time = 0;			// 单帧间隔
		double time_scale = 1.0;		// 时间缩放
		bool is_frame_limited = true;	// 是否启用帧率限制

		clock_t::time_point last_time;			// 上次刷新时间
		clock_t::time_point global_start_time;	// 全局起始时间

		const int MIN_DUR = 1000 / 999;	// 最小帧间隔
	};
	Clock Clock::m_instance;

	// 数学运算工具
	namespace maths
	{
		// 角度转弧度
		float deg_to_rad(float degree) noexcept { return degree * PI / 180.0f; }

		// 弧度转角度
		float rad_to_deg(float radian) noexcept { return radian * 180.0f / PI; }

		// 插值函数
		float lerp(float current, float target, float t) noexcept { return current + (target - current) * t; }

		// 将数值钳定在[min, max]范围内
		template <typename Ty>
		Ty clamp(Ty val, Ty max, Ty min) { return std::max(min, std::min(val, max)); }

		// 计算两点距离
		float distance_to(const Vector2& pos_1, const Vector2& pos_2)
		{
			float _dx = pos_1.x - pos_2.x;
			float _dy = pos_1.y - pos_2.y;
			return sqrtf((_dx * _dx) + (_dy * _dy));
		}

		// 摆动函数
		float swing(float min, float max, float strength)
		{
			if (max < min) std::swap(min, max);
			if (strength < 0) strength = -strength;

			float _dt = Clock::get_global_time();
			// 摆动区间 = (max-min)÷2 * sin(dt*k) + (max+min)÷2
			return (max - min) / 2.0f * std::sin(_dt * strength) + (max + min) / 2.0f;
		}

		// 计算屏幕标准缩放比
		float get_std_zoom_ratio()
		{
			int _screen_width, _screen_height;
			SDL_GetWindowSize(Main_Window, &_screen_width, &_screen_height);

			// 缩放比 = 当前屏幕尺寸 ÷ 标准屏幕尺寸(开发时的屏幕尺寸)
			float _ratio_w = static_cast<float>(_screen_width) / normal_screen_width;
			float _ratio_h = static_cast<float>(_screen_height) / normal_screen_height;

			// 计算宽高缩放比，取较小的一个作为标准缩放比，以保持宽高比不变
			return std::min(_ratio_w, _ratio_h);
		}

		// 绘制空心圆形
		void render_circle(SDL_Renderer* renderer, const Vector2& center_pos, float radius, SDL_Color color)
		{
			if (radius <= 0) return;

			float _x = 0, _y = radius, _d = 3 - 2 * radius;
			SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

			while (_x <= _y)
			{
				SDL_RenderPoint(renderer, center_pos.x + _x, center_pos.y + _y);
				SDL_RenderPoint(renderer, center_pos.x - _x, center_pos.y + _y);
				SDL_RenderPoint(renderer, center_pos.x + _x, center_pos.y - _y);
				SDL_RenderPoint(renderer, center_pos.x - _x, center_pos.y - _y);
				SDL_RenderPoint(renderer, center_pos.x + _y, center_pos.y + _x);
				SDL_RenderPoint(renderer, center_pos.x - _y, center_pos.y + _x);
				SDL_RenderPoint(renderer, center_pos.x + _y, center_pos.y - _x);
				SDL_RenderPoint(renderer, center_pos.x - _y, center_pos.y - _x);

				if (_d < 0)
				{
					_d = _d + 4 * _x + 6;
				}
				else
				{
					_d = _d + 4 * (_x - _y) + 10;
					_y -= 1.0f;
				}
				_x += 1.0f;
			}
		}

		// 绘制实心圆形
		void render_filled_circle(SDL_Renderer* renderer, const Vector2& center_pos, float radius, SDL_Color color)
		{
			if (radius <= 0) return;

			SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

			for (float _y = -radius; _y <= radius; _y += 1.0f)
			{
				int _x_length = static_cast<int>(sqrtf(radius * radius - _y * _y));
				SDL_RenderLine(renderer,
					center_pos.x - _x_length, center_pos.y + _y,
					center_pos.x + _x_length, center_pos.y + _y);
			}
		}
	};

	// 小工具
	namespace utils
	{
		// 获取键盘/鼠标事件的UTF-8格式字符串
		const char* Get_EventName(const SDL_Event& event)
		{
			static std::string _result = "None";

			if (event.type == SDL_EVENT_KEY_DOWN)
				_result = SDL_GetKeyName(event.key.key);
			if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
			{
				switch (event.button.button)
				{
				case SDL_BUTTON_LEFT:	_result = "Left Mouse Button";   break;
				case SDL_BUTTON_MIDDLE: _result = "Middle Mouse Button"; break;
				case SDL_BUTTON_RIGHT:	_result = "Right Mouse Button";	break;
				case SDL_BUTTON_X1:		_result = "Side Button 1";		break;
				case SDL_BUTTON_X2:		_result = "Side Button 2";		break;
				}
			}
			if (event.type == SDL_EVENT_MOUSE_WHEEL)
			{
				if (event.wheel.y > 0) _result = "Mouse Wheel Up";
				if (event.wheel.y < 0) _result = "Mouse Wheel Down";
			}
			if (event.type == SDL_EVENT_MOUSE_MOTION)
				_result = "Mouse Motion";

			return _result.c_str();
		}

		// 显示消息框
		void Show_MessageBox(MsgBoxFlags type, const char* title, const char* message)
		{
			// 检测主窗口是否存在
			if (Main_Window != nullptr)
				SDL_ShowSimpleMessageBox((SDL_MessageBoxFlags)type, title, message, Main_Window);
			else
				SDL_ShowSimpleMessageBox((SDL_MessageBoxFlags)type, title, message, nullptr);
		}

		// 设置屏幕标准宽高
		export void Set_NormalWindowSize(int w, int h)
		{
			normal_screen_width = w;
			normal_screen_height = h;
		}

		// 设置缩放模式
		export void Set_ScaleMode(ScaleMode mode)
		{
			switch (mode)
			{
			case ScaleMode::Linear: scale_mode = SDL_SCALEMODE_LINEAR; break;
			case ScaleMode::Nearest: scale_mode = SDL_SCALEMODE_NEAREST; break;
			case ScaleMode::Best: scale_mode = SDL_SCALEMODE_PIXELART; break;
			}
		}
	}
}
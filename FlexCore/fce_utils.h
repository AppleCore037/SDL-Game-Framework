#pragma once

#include "fce_base.h"
#include "fce_property.h"

#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <thread>

namespace fce
{
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
		const char* title() const noexcept;

		// 获取错误信息
		const char* what() const noexcept;

	private:
		std::string error_title;	// 错误标题
		std::string error_info;		// 错误信息
	};

	// 随机数生成器
	class Random
	{
	public:
		// 生成 [min, max] 范围内的随机整数
		static int randint(int min, int max);

		// 生成 [min, max] 范围内的随机浮点数
		static float randfloat(float min, float max);

	private:
		Random();
		~Random() = default;

	private:
		static Random m_instance;	// 单例实例
		std::mt19937 engine;		// 随机数引擎
	};

	// 全局游戏时钟
	class Clock
	{
		using clock_t = std::chrono::steady_clock;
		using ms_t = std::chrono::milliseconds;

	public:
		// 起始当前帧计时
		static void start_frame();

		// 结束当前帧计时
		static void end_frame();

		// 设置是否垂直同步（默认false）
		static void set_VSync(bool is_abled);

		// 设置FPS
		static void set_fps(int fps_limit);

		// 获取全局经过时间（秒）
		static float get_global_time();

		// 重置全局经过时间
		static void restart_global_time();

		// 设置是否启用帧率限制（默认是true）
		static void set_frame_limit(bool flag);

		// 获取FPS
		static int get_fps();

		// 获取帧间隔
		static float get_DeltaTime();

		// 设置时间缩放
		static void set_time_scale(double scale);

	private:
		Clock();
		~Clock() = default;

		// 获取屏幕刷新率（获取失败默认返回60）
		int get_screen_refreshRate() const;

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

		// 数据类型
		enum DataType : uint8_t
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

	public:
		// 清空文件
		static void clear(const std::string& file_name);

		// 保存数据
		template <typename _Ty>
		static void save(const std::string& fname, const std::string& key, _Ty value, SvMod mod = Append);

		// 读取数据
		template <typename _Ty>
		static _Ty read(const std::string& file_name, const std::string& key);

	private:
		Data();
		~Data() = default;

		// 初始化类型支持表
		void init_type_list();

		// 基础类型转换
		template <typename _Ty>
		_Ty type_conversion(const std::string& val);

	private:
		static Data m_instance;	// 单例实例
		std::vector<std::string> type_list;	// 类型支持表
	};

	// 数学运算工具
	namespace maths
	{
		// 角度转弧度
		float deg_to_rad(float degree) noexcept;

		// 弧度转角度
		float rad_to_deg(float radian) noexcept;

		// 插值函数
		float lerp(float current, float target, float t) noexcept;

		// 计算两点距离
		float distance_to(const Vector2& pos_1, const Vector2& pos_2);

		// 摆动函数
		float swing(float min, float max, float strength);
	};
};
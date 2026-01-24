#pragma once

#include "fce_base.h"
#include "fce_property.h"

#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <thread>
#include <unordered_map>
#include <typeindex>

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

	public:
		// 清空文件
		static void clear(const std::string& fname);

		// 保存文件（序列化格式）
		template <typename _Ty>
		static void save(const std::string& fname, const std::string& key, _Ty val, SvMod mod = Append);
		
		// 读取文件
		template <typename _Ty>
		static _Ty read(const std::string& fname, const std::string& key);

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
		static void serialization(std::ostream& ofs, const std::string& key, _Ty val);

		// 反序列化
		template <typename _Ty>
		static void deserialization(std::istream& ifs, _Ty& val, DataType type);
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

// ======================================= Data =======================================

// 保存文件（序列化格式）
template <typename _Ty>
inline void fce::Data::save(const std::string& fname, const std::string& key, _Ty val, SvMod mod)
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
inline _Ty fce::Data::read(const std::string& fname, const std::string& key)
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
		throw fce::custom_error("DataRead Error", "Can't pass <const char*> return variable\nPlease use std::string!");

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

// 序列化
template <typename _Ty>
inline void fce::Data::serialization(std::ostream& ofs, const std::string& key, _Ty val)
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
template<typename _Ty>
inline void fce::Data::deserialization(std::istream& ifs, _Ty& val, DataType type)
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
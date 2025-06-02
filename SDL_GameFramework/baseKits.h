#pragma once
#include <chrono>
#include <functional>
using namespace std::chrono;

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

// 随机数生成器
class Random
{
public:
	// 初始化随机数种子
	static void init_seed();

	// 生成在[min, max]之间的整数
	static int randint(int min, int max);

	// 生成在[min, max]之间的浮点数
	static float randfloat(float min, float max);

private:
	// 生成[0, 1]之间的随机浮点数
	static float nextfloat();
};

// 时钟
class Clock
{
public:
	Clock();

	Clock(int fps_limit);

	~Clock() = default;

	// 时钟开始计时
	void start_frame();

	// 时钟结束计时
	void end_frame() const;

	// 设置是否垂直同步
	void set_VSync(bool is_abled);

	// 设置FPS
	void setFPS(int fps_limit);

	// 获取FPS
	int getFPS() const;

	// 获取帧间隔
	float get_DeltaTime() const;

	// 设置时间缩放
	void set_time_scale(float scale);

private:
	// 获取屏幕刷新率
	int get_screen_refreshRate();

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
	void restart();

	// 设置等待时间
	void set_wait_time(float val);

	// 设置是否单次触发
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
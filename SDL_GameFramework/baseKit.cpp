#include "baseKits.h"

#include <SDL3/SDL.h>
#include <ctime>
#include <chrono>
#include <thread>

// =====================================================================================
//				随 机 数 类					R a n d o m
// =====================================================================================

// 初始化随机数种子
void Random::init_seed()
{
	srand(static_cast<unsigned>(time(nullptr)));
}

// 生成在[min, max]之间的整数
int Random::randint(int min, int max)
{
	if (min > max)
		std::swap(min, max);	// 确保min比max小

	return min + (rand() % (max - min + 1));
}

// 生成在[min, max]之间的浮点数
float Random::randfloat(float min, float max)
{
	if (min > max)
		std::swap(min, max);	// 确保min比max小

	return min + (nextfloat() * (max - min));
}

// 生成[0, 1]之间的随机浮点数
float Random::nextfloat()
{
	return static_cast<float>(rand()) / (static_cast<float>(RAND_MAX) + 1.0f);
}


// =====================================================================================
//				时	钟	类					C l o c k
// =====================================================================================

using namespace std::chrono;

Clock::Clock()
{
	this->target_fps = 60;			// 默认60帧
	this->target_time = 1000 / target_fps;
	this->last_time = steady_clock::now();
	this->delta_time = 0;
	this->time_scale = 1.0f;
}

Clock::Clock(int fps_limit)
{
	this->target_fps = fps_limit;	// 设置目标FPS
	this->target_time = 1000 / target_fps;
	this->last_time = steady_clock::now();
	this->delta_time = 0;
	this->time_scale = 1.0f;
}

// 时钟开始计时
void Clock::start_frame()
{
	auto currentTime = steady_clock::now();
	delta_time = duration_cast<milliseconds>(currentTime - last_time).count();
	last_time = currentTime;
}

// 时钟结束计时
void Clock::end_frame() const
{
	auto currentTime = steady_clock::now();
	auto elapsedTime = duration_cast<milliseconds>(currentTime - last_time).count();	// 获取经过时间

	// 如果经过时间小于帧间隔就休眠
	if (elapsedTime < target_time)
		std::this_thread::sleep_for(milliseconds(target_time - elapsedTime));
}

// 设置是否垂直同步
void Clock::set_VSync(bool is_abled)
{
	if (is_abled)
	{
		int refresh = this->get_screen_refreshRate();
		// printf("屏幕刷新率: %d\n", refresh);	// 打印屏幕刷新率

		if (refresh)
			this->target_time = 1000 / refresh;
		else
		{
			printf("Warning: Cannot get screen refresh rate!\n");
			return;
		}
	}
	else
		this->target_time = 1000 / target_fps;		// 设置为目标FPS
}

int Clock::get_screen_refreshRate()
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

// 设置FPS
void Clock::setFPS(int fps_limit)
{
	this->target_fps = fps_limit;		// 设置目标FPS
	this->target_time = 1000 / fps_limit;
}

// 获取FPS
int Clock::getFPS() const
{
	return 1000 / delta_time;
}

// 获取帧间隔
float Clock::get_DeltaTime() const
{
	return (delta_time / 1000.0f) * time_scale;
}

// 设置时间缩放
void Clock::set_time_scale(float scale)
{
	if (scale <= 0.0f)
	{
		printf("Warning: Time scale must be greater than 0!\n");
		return;
	}

	this->time_scale = scale;
}

// =====================================================================================
//				计 时 器 类							T i m e r
// =====================================================================================

// 重启计时器
void Timer::restart()
{
	pass_time = 0;
	shotted = false;
}

// 设置等待时间
void Timer::set_wait_time(float val)
{
	this->wait_time = val;
}

// 设置是否单次触发
void Timer::set_one_shot(bool flag)
{
	this->one_shot = flag;
}

// 设置回调函数
void Timer::set_on_timeout(std::function<void()> timeout_callback)
{
	this->on_timeout = timeout_callback;
}

// 暂停
void Timer::pause()
{
	paused = true;
}

// 继续
void Timer::resume()
{
	paused = false;
}

// 更新计时器
void Timer::on_update(float delta)
{
	if (paused)
		return;

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
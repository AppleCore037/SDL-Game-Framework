#include "baseKits.h"

#include <SDL3/SDL.h>
#include <ctime>
#include <chrono>
#include <thread>

// =====================================================================================
//				�� �� �� ��					R a n d o m
// =====================================================================================

// ��ʼ�����������
void Random::init_seed()
{
	srand(static_cast<unsigned>(time(nullptr)));
}

// ������[min, max]֮�������
int Random::randint(int min, int max)
{
	if (min > max)
		std::swap(min, max);	// ȷ��min��maxС

	return min + (rand() % (max - min + 1));
}

// ������[min, max]֮��ĸ�����
float Random::randfloat(float min, float max)
{
	if (min > max)
		std::swap(min, max);	// ȷ��min��maxС

	return min + (nextfloat() * (max - min));
}

// ����[0, 1]֮������������
float Random::nextfloat()
{
	return static_cast<float>(rand()) / (static_cast<float>(RAND_MAX) + 1.0f);
}


// =====================================================================================
//				ʱ	��	��					C l o c k
// =====================================================================================

using namespace std::chrono;

Clock::Clock()
{
	this->target_fps = 60;			// Ĭ��60֡
	this->target_time = 1000 / target_fps;
	this->last_time = steady_clock::now();
	this->delta_time = 0;
	this->time_scale = 1.0f;
}

Clock::Clock(int fps_limit)
{
	this->target_fps = fps_limit;	// ����Ŀ��FPS
	this->target_time = 1000 / target_fps;
	this->last_time = steady_clock::now();
	this->delta_time = 0;
	this->time_scale = 1.0f;
}

// ʱ�ӿ�ʼ��ʱ
void Clock::start_frame()
{
	auto currentTime = steady_clock::now();
	delta_time = duration_cast<milliseconds>(currentTime - last_time).count();
	last_time = currentTime;
}

// ʱ�ӽ�����ʱ
void Clock::end_frame() const
{
	auto currentTime = steady_clock::now();
	auto elapsedTime = duration_cast<milliseconds>(currentTime - last_time).count();	// ��ȡ����ʱ��

	// �������ʱ��С��֡���������
	if (elapsedTime < target_time)
		std::this_thread::sleep_for(milliseconds(target_time - elapsedTime));
}

// �����Ƿ�ֱͬ��
void Clock::set_VSync(bool is_abled)
{
	if (is_abled)
	{
		int refresh = this->get_screen_refreshRate();
		// printf("��Ļˢ����: %d\n", refresh);	// ��ӡ��Ļˢ����

		if (refresh)
			this->target_time = 1000 / refresh;
		else
		{
			printf("Warning: Cannot get screen refresh rate!\n");
			return;
		}
	}
	else
		this->target_time = 1000 / target_fps;		// ����ΪĿ��FPS
}

int Clock::get_screen_refreshRate()
{
	// ��ȡ����ʾ����Ĭ����ʾ����
	SDL_DisplayID display = SDL_GetPrimaryDisplay();
	if (!display) return 0;

	// ��ȡ��ǰ��ʾģʽ
	const SDL_DisplayMode* mode = SDL_GetCurrentDisplayMode(display);
	if (!mode) return 0;

	// ����ˢ���ʣ������Ч�����򷵻�Ĭ��ֵ��
	return (mode->refresh_rate > 0) ? mode->refresh_rate : 0;
}

// ����FPS
void Clock::setFPS(int fps_limit)
{
	this->target_fps = fps_limit;		// ����Ŀ��FPS
	this->target_time = 1000 / fps_limit;
}

// ��ȡFPS
int Clock::getFPS() const
{
	return 1000 / delta_time;
}

// ��ȡ֡���
float Clock::get_DeltaTime() const
{
	return (delta_time / 1000.0f) * time_scale;
}

// ����ʱ������
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
//				�� ʱ �� ��							T i m e r
// =====================================================================================

// ������ʱ��
void Timer::restart()
{
	pass_time = 0;
	shotted = false;
}

// ���õȴ�ʱ��
void Timer::set_wait_time(float val)
{
	this->wait_time = val;
}

// �����Ƿ񵥴δ���
void Timer::set_one_shot(bool flag)
{
	this->one_shot = flag;
}

// ���ûص�����
void Timer::set_on_timeout(std::function<void()> timeout_callback)
{
	this->on_timeout = timeout_callback;
}

// ��ͣ
void Timer::pause()
{
	paused = true;
}

// ����
void Timer::resume()
{
	paused = false;
}

// ���¼�ʱ��
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
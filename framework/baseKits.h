#pragma once
#include <chrono>
#include <functional>
using namespace std::chrono;

// ��С
struct Size
{
	Size() = default;
	~Size() = default;
	Size(float w, float h) :
		width(w), height(h) {}

	float width;
	float height;
};

// ��ά����
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

	// ��ȡ����
	float length() const { return sqrt(x * x + y * y); }

	// ��׼��
	Vector2 normalize() const
	{
		float len = this->length();

		if (len == 0)
			return Vector2(0, 0);
		return Vector2(x / len, y / len);
	}
};

// �����������
class Random
{
public:
	// ��ʼ�����������
	static void init_seed();

	// ������[min, max]֮�������
	static int randint(int min, int max);

	// ������[min, max]֮��ĸ�����
	static float randfloat(float min, float max);

private:
	// ����[0, 1]֮������������
	static float nextfloat();
};

// ʱ��
class Clock
{
public:
	Clock();

	Clock(int fps_limit);

	~Clock() = default;

	// ʱ�ӿ�ʼ��ʱ
	void start_frame();

	// ʱ�ӽ�����ʱ
	void end_frame() const;

	// �����Ƿ�ֱͬ��
	void set_VSync(bool is_abled);

	// ����FPS
	void setFPS(int fps_limit);

	// ��ȡFPS
	int getFPS() const;

	// ��ȡ֡���
	float get_DeltaTime() const;

	// ����ʱ������
	void set_time_scale(float scale);

private:
	// ��ȡ��Ļˢ����
	int get_screen_refreshRate();

private:
	int target_fps;							// Ŀ��FPS
	int target_time;						// Ŀ��֡���
	steady_clock::time_point last_time;		// ����ʱ��
	float delta_time;						// ��֡���
	float time_scale;						// ʱ������
};

// ��ʱ��
class Timer
{
public:
	Timer() = default;
	~Timer() = default;

	// ������ʱ��
	void restart();

	// ���õȴ�ʱ��
	void set_wait_time(float val);

	// �����Ƿ񵥴δ���
	void set_one_shot(bool flag);

	// ���ûص�����
	void set_on_timeout(std::function<void()> timeout_callback);

	// ��ͣ
	void pause();

	// ����
	void resume();

	// ���¼�ʱ��
	void on_update(float delta);

private:
	float pass_time = 0;				// ����ʱ��
	float wait_time = 0;				// �ȴ�ʱ��
	bool paused = false;				// �Ƿ���ͣ
	bool shotted = false;				// �Ƿ��Ѿ�����
	bool one_shot = false;				// �Ƿ񵥴ʴ���
	std::function<void()> on_timeout;	// �ص�����
};
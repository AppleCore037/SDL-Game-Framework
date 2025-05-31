#pragma once
#include "baseKits.h"
#include <SDL3/SDL.h>

class Camera;
class Sprite;

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
	Camera(SDL_Renderer* renderer, SDL_Window* window);

	~Camera() = default;

	// 获取摄像机坐标(中心点)
	const Vector2& get_position() const;

	// 设置摄像机坐标(中心点)
	void set_position(const Vector2& pos);

	// 设置摄像机缩放
	void set_zoom(float scale);

	// 重置摄像机
	void reset();

	// 抖动摄像机
	void shake(float strength, float duration);

	// 渲染纹理
	void render_texture(SDL_Texture* texture, const SDL_FRect* rect_src, const SDL_FRect* rect_dst,
		double angle, const SDL_FPoint* center, bool is_flip) const;

	// 渲染图形 -- 线
	void render_shape(const Vector2& begin, const Vector2& end, SDL_Color color) const;

	// 渲染图形 -- 矩形
	void render_shape(const Vector2& pos, Size rectSize, SDL_Color color, bool is_filled) const;

	// 渲染图形 -- 圆形
	void render_shape(const Vector2& pos, float radius, SDL_Color color, bool is_filled) const;

	// 跟随角色
	void look_at(const Sprite* sprite, int style);

	// 更新摄像机
	void on_update(float delta);

private:
	// 获取屏幕中心点
	Vector2 get_screen_center() const;

	// 获取屏幕大小
	Size get_screen_size() const;

private:
	Vector2 position;					// 摄像机最终位置
	Vector2 shake_position;				// 抖动位置
	Vector2 base_position;				// 基础位置

	Timer timer_shake;					// 抖动计时器
	bool is_shaking = false;			// 是否抖动
	float shaking_strength = 0;			// 抖动幅度
	float smooth_strength = 0;			// 平滑幅度
	float zoom = 1.0f;					// 摄像机缩放

	SDL_Renderer* renderer = nullptr;	// 内置渲染器
	SDL_Window* window = nullptr;		// 内置窗口

private:
	const float SMOOTH_FACTOR = 1.5f;	// 平滑系数
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
	Vector2 position;		// 位置
	Vector2 velocity;		// 速度
};
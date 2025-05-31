#pragma once
#include "baseKits.h"
#include <SDL3/SDL.h>

// 小工具
namespace utils
{
	// 角度转弧度
	float deg_to_rad(float degree) noexcept;

	// 弧度转角度
	float rad_to_deg(float radian) noexcept;

	// 插值函数
	float lerp(float current, float target, float t) noexcept;

	// 计算两点距离
	float distance_to(const Vector2& pos_1, const Vector2& pos_2);

	// 绘制空心圆
	void draw_circle(SDL_Renderer* renderer, float centerX, float centerY, float radius);

	// 绘制实心圆
	void draw_FilledCircle(SDL_Renderer* renderer, float centerX, float centerY, float radius);
}
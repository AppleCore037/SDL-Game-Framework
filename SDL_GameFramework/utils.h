#pragma once
#include "baseKits.h"
#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

// 宏定义
#define SDL_INIT_EVERYTHING (SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS | \
				SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC | SDL_INIT_GAMEPAD | SDL_INIT_SENSOR)
#define MIX_INIT_EVERYTHING (MIX_INIT_MP3 | MIX_INIT_FLAC | MIX_INIT_MID | \
				MIX_INIT_MOD | MIX_INIT_OGG | MIX_INIT_OPUS | MIX_INIT_WAVPACK)

// 常量定义
constexpr float PAI = 3.14159265f;							// 圆周率
constexpr SDL_Color Color_Red = { 255, 0, 0, 255 };			// 红
constexpr SDL_Color Color_Blue = { 0, 0, 255, 255 };		// 蓝
constexpr SDL_Color Color_Green = { 0, 255, 0, 255 };		// 绿
constexpr SDL_Color Color_White = { 255, 255, 255, 255 };	// 白
constexpr SDL_Color Color_Black = { 0, 0, 0, 0 };			// 黑
constexpr SDL_Color Color_Gray = { 128, 128, 128, 255 };	// 灰

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
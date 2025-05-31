#pragma once
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
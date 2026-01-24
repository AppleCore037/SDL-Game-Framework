#pragma once

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <string>
#include <type_traits>

constexpr float PI = 3.14159265f;	// 圆周率
constexpr SDL_InitFlags SDL_INIT_EVERYTHING = (SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC | SDL_INIT_GAMEPAD | SDL_INIT_SENSOR);
constexpr MIX_InitFlags MIX_INIT_EVERYTHING = (MIX_INIT_MP3 | MIX_INIT_FLAC | MIX_INIT_MID | MIX_INIT_MOD | MIX_INIT_OGG | MIX_INIT_OPUS | MIX_INIT_WAVPACK);

namespace fce
{
	inline SDL_Window* Main_Window = nullptr;				// 主窗口
	inline SDL_Renderer* Main_Renderer = nullptr;			// 主渲染器
	inline TTF_TextEngine* Main_TextEngine = nullptr;		// 主文字引擎
	inline SDL_Event Main_Event;							// 主循环事件

	// 预定义颜色
	namespace Colors
	{
		inline SDL_Color Red = { 255, 0, 0, 255 };			 // 正红
		inline SDL_Color Blue = { 0, 0, 255, 255 };			 // 正蓝
		inline SDL_Color Green = { 0, 255, 0, 255 };		 // 正绿
		inline SDL_Color White = { 255, 255, 255, 255 };	 // 正白
		inline SDL_Color Black = { 0, 0, 0, 255 };			 // 正黑
		inline SDL_Color Yellow = { 255, 255, 0, 255 };		 // 正黄
		inline SDL_Color Pink = { 255, 0, 255, 255 };		 // 正粉
		inline SDL_Color LightBlue = { 0, 255, 255, 255 };	 // 浅蓝
		inline SDL_Color LightGray = { 128, 128, 128, 255 }; // 浅灰
		inline SDL_Color DarkGray = { 50, 50, 50, 255 };	 // 暗灰
		inline SDL_Color Orange = { 255, 165, 0, 255 };		 // 橙色
		inline SDL_Color Purple = { 128, 0, 128, 255 };		 // 紫色
	}

	// 窗口标签
	enum class WindowFlags
	{
		Default = SDL_EVENT_WINDOW_SHOWN,	// 显示
		Hidden = SDL_WINDOW_HIDDEN,			// 隐藏
		Resizable = SDL_WINDOW_RESIZABLE,	// 可调整大小
		Maximized = SDL_WINDOW_MAXIMIZED,	// 最大化
		Minimized = SDL_WINDOW_MINIMIZED,	// 最小化
		Fullscreen = SDL_WINDOW_FULLSCREEN	// 全屏
	};

	// 消息框类型
	enum class MsgBoxFlags
	{
		Info = SDL_MESSAGEBOX_INFORMATION,	// 信息
		Warning = SDL_MESSAGEBOX_WARNING,	// 警告
		Error = SDL_MESSAGEBOX_ERROR,		// 错误
	};

	// 渲染层级
	enum class RenderLayer
	{
		None,			// 无渲染层
		Background,		// 背景层
		Frontground,	// 前景层
		Label,			// 游戏内部文本层
		GameObject,		// 游戏元素层
		Effect,			// 特效层
		UI				// UI层
	};

	// 碰撞层级
	enum class CollisionLayer
	{
		None = 0,				// 无碰撞层
		Player = 1 << 0,		// 玩家层
		Enemy = 1 << 1,			// 敌人层
		GameMap = 1 << 2,		// 游戏地图层
		GameObject = 1 << 3,	// 游戏元素层
		Attack = 1 << 4,		// 攻击层
	};

	// 获取键盘/鼠标事件的UTF-8格式字符串
	inline const char* Get_EventName(const SDL_Event& event)
	{
		static std::string result = "None";

		if (event.type == SDL_EVENT_KEY_DOWN)
			result = SDL_GetKeyName(event.key.key);
		if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
		{
			switch (event.button.button)
			{
			case SDL_BUTTON_LEFT:	result = "Left Mouse Button";   break;
			case SDL_BUTTON_MIDDLE: result = "Middle Mouse Button"; break;
			case SDL_BUTTON_RIGHT:	result = "Right Mouse Button";	break;
			case SDL_BUTTON_X1:		result = "Side Button 1";		break;
			case SDL_BUTTON_X2:		result = "Side Button 2";		break;
			}
		}
		if (event.type == SDL_EVENT_MOUSE_WHEEL)
		{
			if (event.wheel.y > 0) result = "Mouse Wheel Up";
			if (event.wheel.y < 0) result = "Mouse Wheel Down";
		}
		if (event.type == SDL_EVENT_MOUSE_MOTION)
			result = "Mouse Motion";

		return result.c_str();
	}

	// 显示消息框
	inline void Show_MessageBox(MsgBoxFlags type, const char* title, const char* message)
	{
		// 检测主窗口是否存在
		if (Main_Window != nullptr)
			SDL_ShowSimpleMessageBox((SDL_MessageBoxFlags)type, title, message, Main_Window);
		else
			SDL_ShowSimpleMessageBox((SDL_MessageBoxFlags)type, title, message, nullptr);
	}

	// 重载按位或运算符 |
	constexpr CollisionLayer operator|(CollisionLayer lhs, CollisionLayer rhs)
	{
		using underlying = std::underlying_type_t<CollisionLayer>;
		return static_cast<CollisionLayer>(static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
	}

	// 重载按位与运算符 &
	constexpr bool operator&(CollisionLayer lhs, CollisionLayer rhs)
	{
		using underlying = std::underlying_type_t<CollisionLayer>;
		return (static_cast<underlying>(lhs) & static_cast<underlying>(rhs)) != 0;
	}
};
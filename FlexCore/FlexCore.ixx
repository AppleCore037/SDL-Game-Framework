module;

#include <iostream>
#include <functional>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>

export module FlexCore;

export import <chrono>;
export import <thread>;
export import <filesystem>;

export import :FCE_BaseSetup;
export import :FCE_BaseType;
export import :FCE_Utils;
export import :FCE_Component;
export import :FCE_Handler;
export import :FCE_Graphic;
export import :FCE_UI;

export class Game
{
public:
	static Camera* Game_Camera;
	static Camera* UI_Camera;

public:
	Game() = default;
	~Game() = default;

	// 初始化
	static void Init_Graphic(const char* title, const Size& size, WindowFlags flag = WindowFlags::Default, const Size& normal_size = { 1280, 720 })
	{
		try
		{
			// 初始化SDL相关模块
			SDL_Init(SDL_INIT_EVERYTHING);
			TTF_Init();

			SDL_AudioSpec spec = { SDL_AUDIO_S32, 2, 44100 };
			Mix_Init(MIX_INIT_EVERYTHING);
			Mix_OpenAudio(NULL, &spec);

			// 创建主窗口
			Main_Window = SDL_CreateWindow(title, (int)size.w, (int)size.h, (SDL_WindowFlags)flag);
			if (!Main_Window) throw std::runtime_error("Failed to create Main_Window!");

			// 创建主渲染器
			Main_Renderer = SDL_CreateRenderer(Main_Window, nullptr);
			if (!Main_Renderer) throw std::runtime_error("Failed to create Main_Renderer!");

			// 创建主文字引擎
			Main_TextEngine = TTF_CreateRendererTextEngine(Main_Renderer);
			if (!Main_TextEngine) throw std::runtime_error("Failed to create Main_TextEngine!");

			// 初始化摄像机
			Game_Camera = new Camera({ 0, 0 });
			UI_Camera = new Camera({ size.w / 2.0f, size.h / 2.0f });

			// 设置默认屏幕大小
			Set_NormalWindowSize((int)normal_size.w, (int)normal_size.h);
		}
		catch (const std::exception& e)
		{
			Show_MessageBox(MsgBoxFlags::Error, "SDL Error", e.what());
			Release_Graphic();
			exit(EXIT_FAILURE);
		}
		catch (const custom_error& e)
		{
			Show_MessageBox(MsgBoxFlags::Error, e.title(), e.what());
			Release_Graphic();
			exit(EXIT_FAILURE);
		}
		catch (...)
		{
			Show_MessageBox(MsgBoxFlags::Error, "Unknow Error", "An unknown error has occurred!");
			Release_Graphic();
			exit(EXIT_FAILURE);
		}
	}

	// 初始化自定义设置项
	static void Init_Custom_Setup(std::function<void()> construction)
	{
		try
		{
			construction();
		}
		catch (const std::exception& e)
		{
			Show_MessageBox(MsgBoxFlags::Error, "SDL Error", e.what());
			Release_Graphic();
			exit(EXIT_FAILURE);
		}
		catch (const custom_error& e)
		{
			Show_MessageBox(MsgBoxFlags::Error, e.title(), e.what());
			Release_Graphic();
			exit(EXIT_FAILURE);
		}
		catch (...)
		{
			Show_MessageBox(MsgBoxFlags::Error, "Unknow Error", "An unknown error has occurred!");
			Release_Graphic();
			exit(EXIT_FAILURE);
		}
	}

	// 主循环
	static void Main_Loop(int fps)
	{
		try
		{
			Clock::set_fps(fps);

			bool is_running = true;
			while (is_running)
			{
				// 开始新帧
				Clock::start_frame();

				// 1.处理输入
				while (SDL_PollEvent(&Main_Event))
				{
					if (Main_Event.type == SDL_EVENT_QUIT)
						is_running = false;
					Director::instance()->on_input(Main_Event);
				}

				// 2.处理数据
				CollisionManager::instance()->process_collision();	// 处理碰撞
				Director::instance()->on_update(Clock::get_DeltaTime());	// 场景更新

				Game_Camera->on_update(Clock::get_DeltaTime());	// 更新游戏摄像机
				UI_Camera->on_update(Clock::get_DeltaTime());	// 更新UI摄像机

				// 3.更新画面
				SDL_RenderClear(Main_Renderer);
				Director::instance()->on_render(*Game_Camera, *UI_Camera);
				SDL_RenderPresent(Main_Renderer);

				// 结束该帧
				Clock::end_frame();
			}
		}
		catch (const custom_error& e)
		{
			Show_MessageBox(MsgBoxFlags::Error, e.title(), e.what());
			Release_Graphic();
			exit(EXIT_FAILURE);
		}
		catch (const std::exception& e)
		{
			Show_MessageBox(MsgBoxFlags::Error, "SDL Error", e.what());
			Release_Graphic();
			exit(EXIT_FAILURE);
		}
		catch (...)
		{
			Show_MessageBox(MsgBoxFlags::Error, "Unknow Error", "An unknown error has occurred!");
			Release_Graphic();
			exit(EXIT_FAILURE);
		}
	}

	// 销毁自定义设置
	static void Release_Custom_Setup(std::function<void()> destruction) 
	{ 
		try
		{
			destruction(); 
		}
		catch (const custom_error& e)
		{
			Show_MessageBox(MsgBoxFlags::Error, e.title(), e.what());
			Release_Graphic();
			exit(EXIT_FAILURE);
		}
		catch (const std::exception& e)
		{
			Show_MessageBox(MsgBoxFlags::Error, "SDL Error", e.what());
			Release_Graphic();
			exit(EXIT_FAILURE);
		}
		catch (...)
		{
			Show_MessageBox(MsgBoxFlags::Error, "Unknow Error", "An unknown error has occurred!");
			Release_Graphic();
			exit(EXIT_FAILURE);
		}
	}

	// 销毁资源
	static void Release_Graphic()
	{
		delete UI_Camera;
		delete Game_Camera;

		TTF_DestroyRendererTextEngine(Main_TextEngine);
		SDL_DestroyRenderer(Main_Renderer);
		SDL_DestroyWindow(Main_Window);

		TTF_Quit();
		Mix_Quit();
		SDL_Quit();
	}
};

Camera* Game::Game_Camera = nullptr;
Camera* Game::UI_Camera = nullptr;
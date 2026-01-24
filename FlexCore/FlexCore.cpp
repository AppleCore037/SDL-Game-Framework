#include "FlexCore.h"

// 初始化图形系统
void fce::Framework::Init_Graphic(const std::string& title, int w, int h, WindowFlags flags)
{
	try
	{
		// 初始化SDL相关模块
		SDL_Init(SDL_INIT_EVERYTHING);
		TTF_Init();

		SDL_AudioSpec spec = { SDL_AUDIO_S32, 2, 44100 };
		Mix_Init(MIX_INIT_EVERYTHING);
		Mix_OpenAudio(NULL, &spec);

		// 初始化窗口
		if (Main_Window) return; // 如果窗口已存在则不再创建
		Main_Window = SDL_CreateWindow(title.c_str(), w, h, (SDL_WindowFlags)flags);
		if (!Main_Window) throw std::runtime_error(u8"Failed to create Main_Window!");

		// 初始化渲染器
		if (Main_Renderer) return;	// 如果渲染器已存在则不再创建
		Main_Renderer = SDL_CreateRenderer(Main_Window, nullptr);
		if (!Main_Renderer) throw std::runtime_error(u8"Failed to create Main_Renderer!");

		// 初始化文本引擎
		if (Main_TextEngine) return; // 如果引擎已存在则不再创建
		Main_TextEngine = TTF_CreateRendererTextEngine(Main_Renderer);
		if (!Main_TextEngine) throw std::runtime_error(u8"Failed to create Main_TextEngine!");

		this->default_camera = new Camera2D();
		SceneManager::instance()->register_scene("default_scene", new DefaultScene());
		SceneManager::instance()->set_current_scene("default_scene");
	}
	catch (const fce::custom_error& e)
	{
		Show_MessageBox(MsgBoxFlags::Error, e.title(), e.what());
		this->Release_Graphic();
		exit(EXIT_FAILURE);
	}
	catch (const std::exception& e)
	{
		Show_MessageBox(MsgBoxFlags::Error, "Game Runtime Error", e.what());
		this->Release_Graphic();
		exit(EXIT_FAILURE);
	}
}

// 主循环
void fce::Framework::main_loop(int fps)
{
	try
	{
		bool is_running = true;
		Clock::set_fps(fps);

		// 主循环
		while (is_running)
		{
			// 帧开始
			Clock::start_frame();

			// 处理输入
			while (SDL_PollEvent(&Main_Event))
			{
				if (Main_Event.type == SDL_EVENT_QUIT)
					is_running = false;
				SceneManager::instance()->on_input(Main_Event);
			}

			// 处理数据
			CollisionManager::instance()->process_collision();
			Director::instance()->on_update(Clock::get_DeltaTime());

			// 更新画面
			SDL_RenderClear(Main_Renderer);
			Director::instance()->on_render(*default_camera, *default_camera);
			SDL_RenderPresent(Main_Renderer);

			// 帧结束
			Clock::end_frame();
		}
	}
	catch (const fce::custom_error& e)
	{
		Show_MessageBox(MsgBoxFlags::Error, e.title(), e.what());
		this->Release_Graphic();
		exit(EXIT_FAILURE);
	}
	catch (const std::exception& e)
	{
		Show_MessageBox(MsgBoxFlags::Error, "Game Runtime Error", e.what());
		this->Release_Graphic();
		exit(EXIT_FAILURE);
	}
	catch (...)
	{
		Show_MessageBox(MsgBoxFlags::Error, "Unknown Error", "An unknown error has occurred!");
		this->Release_Graphic();
		exit(EXIT_FAILURE);
	}
}

// 释放图形系统
void fce::Framework::Release_Graphic()
{
	delete default_camera;

	// 释放内置资源
	if (Main_TextEngine) { TTF_DestroyRendererTextEngine(Main_TextEngine); }
	if (Main_Renderer) { SDL_DestroyRenderer(Main_Renderer); }
	if (Main_Window) { SDL_DestroyWindow(Main_Window); }

	// 退出SDL第三方库
	TTF_Quit();
	Mix_Quit();
	SDL_Quit();
}
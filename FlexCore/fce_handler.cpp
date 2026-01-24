#include "fce_handler.h"

// ======================================= ResourcesManager =======================================

fce::ResourcesManager* fce::ResourcesManager::m_instance = nullptr;

// 获取资源管理器单例
fce::ResourcesManager* fce::ResourcesManager::instance()
{
	if (!m_instance)
		m_instance = new ResourcesManager();

	return m_instance;
}

// 加载资源
void fce::ResourcesManager::load_resources(const char* directory)
{
	// 判断文件是否存在
	if (!std::filesystem::exists(directory))
	{
		std::string info = u8"Dictionary “" + std::string(directory) + u8"” is an error dictionary!";
		throw custom_error(u8"ResourcesManager Error", info);
	}

	// 遍历目标文件内部所有文件
	for (const auto& entry : std::filesystem::recursive_directory_iterator(directory))
	{
		if (entry.is_regular_file())	// 如果是有效文件
		{
			const std::filesystem::path& path = entry.path();
			if (path.extension() == ".bmp" || path.extension() == ".png" || path.extension() == ".jpg")
			{
				SDL_Texture* texture = IMG_LoadTexture(Main_Renderer, path.u8string().c_str());
				texture_pool[path.stem().u8string()] = texture;
			}
			if (path.extension() == ".wav")
			{
				Mix_Chunk* audio = Mix_LoadWAV(path.u8string().c_str());
				audio_pool[path.stem().u8string()] = audio;
			}
			if (path.extension() == ".mp3" || path.extension() == ".ogg")
			{
				Mix_Music* music = Mix_LoadMUS(path.u8string().c_str());
				music_pool[path.stem().u8string()] = music;
			}
			if (path.extension() == ".ttf" || path.extension() == ".TTF")
			{
				TTF_Font* font = TTF_OpenFont(path.u8string().c_str(), 24);
				font_pool[path.stem().u8string()] = font;
			}
		}
	}
}

// 查找纹理资源
SDL_Texture* fce::ResourcesManager::find_texture(const std::string& name)
{
	// 未找到
	if (texture_pool.find(name) == texture_pool.end())
	{
		std::string info = u8"Texture “" + name + u8"” is not found!";
		throw custom_error(u8"ResourcesManager Error", info);
	}
	return texture_pool[name];
}

// 查找音频资源
Mix_Chunk* fce::ResourcesManager::find_audio(const std::string& name)
{
	// 未找到
	if (audio_pool.find(name) == audio_pool.end())
	{
		std::string info = u8"Audio “" + name + u8"” is not found!";
		throw custom_error(u8"ResourcesManager Error", info);
	}
	return audio_pool[name];
}

// 查找音乐资源
Mix_Music* fce::ResourcesManager::find_music(const std::string& name)
{
	// 未找到
	if (music_pool.find(name) == music_pool.end())
	{
		std::string info = u8"Music “" + name + u8"” is not found!";
		throw custom_error(u8"ResourcesManager Error", info);
	}
	return music_pool[name];
}

// 查找字体资源
TTF_Font* fce::ResourcesManager::find_font(const std::string& name)
{
	// 未找到
	if (font_pool.find(name) == font_pool.end())
	{
		std::string info = u8"Font “" + name + u8"” is not found!";
		throw custom_error(u8"ResourcesManager Error", info);
	}
	return font_pool[name];
}

// 显示已加载的资源
void fce::ResourcesManager::debug_resources_list()
{
	printf("=============================================================\n");
	printf("已加载的纹理资源如下 (共%d个):\n", (int)texture_pool.size());
	for (auto& texture : texture_pool)
		printf("%s\n", texture.first.c_str());

	printf("\n已加载的音频资源如下 (共%d个):\n", (int)audio_pool.size());
	for (auto& audio : audio_pool)
		printf("%s\n", audio.first.c_str());
	for (auto& music : music_pool)
		printf("%s\n", music.first.c_str());

	printf("\n已加载的字体资源如下 (共%d个):\n", (int)font_pool.size());
	for (auto& font : font_pool)
		printf("%s\n", font.first.c_str());
	printf("=============================================================\n");
}

// ======================================= SceneManager =======================================

fce::SceneManager* fce::SceneManager::m_instance = nullptr;

fce::SceneManager* fce::SceneManager::instance()
{
	if (!m_instance)
		m_instance = new SceneManager();

	return m_instance;
}

void fce::SceneManager::set_current_scene(const std::string& name)
{
	// 如果场景池中不存在该场景
	if (scene_pool.find(name) == scene_pool.end())
		throw custom_error(u8"SceneManager Error", u8"Scene “" + name + u8"” is not exist!");

	current_scene = scene_pool[name];
	current_scene->on_enter();
}

void fce::SceneManager::switch_to(const std::string& name)
{
	current_scene->on_exit();

	// 如果场景池中不存在该场景
	if (scene_pool.find(name) == scene_pool.end())
		throw custom_error(u8"SceneManager Error", u8"Scene “" + name + u8"” is not exist!");

	current_scene = scene_pool[name];
	current_scene->on_enter();
}

void fce::SceneManager::register_scene(const std::string& name, Scene* scene)
{
	if (scene_pool.find(name) != scene_pool.end())
		throw custom_error(u8"SceneManager Error", u8"Scene “" + name + u8"” is already exist!");
	scene_pool[name] = scene;
}

fce::Scene* fce::SceneManager::find_scene(const std::string& name)
{
	// 如果场景池中不存在该场景
	if (scene_pool.find(name) == scene_pool.end())
		throw custom_error(u8"SceneManager Error", u8"Scene “" + name + u8"” is not exist!");

	return scene_pool[name];
}

void fce::SceneManager::on_update(float delta)
{
	if (current_scene)
		current_scene->on_update(delta);
	else
		throw custom_error(u8"SceneManager Error", u8"“current_scene” is a nullptr!");
}

void fce::SceneManager::on_render(const Camera2D& camera_game, const Camera2D& camera_ui)
{
	if (current_scene)
		current_scene->on_render(camera_game, camera_ui);
	else
		throw custom_error(u8"SceneManager Error", u8"“current_scene” is a nullptr!");
}

void fce::SceneManager::on_input(const SDL_Event& event)
{
	if (current_scene)
		current_scene->on_input(event);
	else
		throw custom_error(u8"SceneManager Error", u8"“current_scene” is a nullptr!");
}

// ======================================= CollisionManager =======================================

fce::CollisionManager* fce::CollisionManager::m_instance = nullptr;

// 获取碰撞管理器单例
fce::CollisionManager* fce::CollisionManager::instance()
{
	if (!m_instance)
		m_instance = new CollisionManager();

	return m_instance;
}

// 添加碰撞箱
fce::CollisionBox* fce::CollisionManager::create_collision_box()
{
	CollisionBox* collision_box = new CollisionBox();
	collision_box_list.push_back(collision_box);
	return collision_box;
}

// 销毁碰撞箱
void fce::CollisionManager::destroy_collision_box(CollisionBox* collision_box)
{
	collision_box_list.erase(std::remove(collision_box_list.begin(),
		collision_box_list.end(), collision_box), collision_box_list.end());
	delete collision_box;
}

// 处理碰撞检测
void fce::CollisionManager::process_collision()
{
	for (auto collision_box_src : collision_box_list)
	{
		if (!collision_box_src->enabled || collision_box_src->layer_dst == CollisionLayer::None)
			continue;

		for (auto collision_box_dst : collision_box_list)
		{
			if (!collision_box_dst->enabled || collision_box_src == collision_box_dst
				|| !(collision_box_src->layer_dst & collision_box_dst->layer_src))
				continue;

			// 横向碰撞条件：两碰撞箱的maxX - 两碰撞箱的minX <= 两碰撞箱的宽度之和
			float max_x = std::max(collision_box_src->position.x + collision_box_src->size.w / 2.0f, collision_box_dst->position.x + collision_box_dst->size.w / 2.0f);
			float min_x = std::min(collision_box_src->position.x - collision_box_src->size.w / 2.0f, collision_box_dst->position.x - collision_box_dst->size.w / 2.0f);
			bool is_collide_x = (max_x - min_x <= collision_box_src->size.w + collision_box_dst->size.w);

			// 纵向碰撞条件：两碰撞箱的maxY - 两碰撞箱的minY <= 两碰撞箱的高度之和
			float max_y = std::max(collision_box_src->position.y + collision_box_src->size.h / 2.0f, collision_box_dst->position.y + collision_box_dst->size.h / 2.0f);
			float min_y = std::min(collision_box_src->position.y - collision_box_src->size.h / 2.0f, collision_box_dst->position.y - collision_box_dst->size.h / 2.0f);
			bool is_collide_y = (max_y - min_y <= collision_box_src->size.h + collision_box_dst->size.h);

			// 如果横向/纵向都成立，且目标碰撞箱存在回调函数，则执行回调函数
			if (is_collide_x && is_collide_y && collision_box_dst->on_collide)
				collision_box_dst->on_collide(collision_box_src->layer_src);
		}
	}
}

// 调试碰撞箱
void fce::CollisionManager::debug_collision_box(const Camera2D& camera)
{
	for (auto collision_box : collision_box_list)
	{
		// 定义碰撞箱渲染颜色
		SDL_Color color;
		if (collision_box->enabled)
			color = { 255,195,195,255 };
		else
			color = { 115,155,175,255 };

		ShapeMaker::render_CenterRect(camera, collision_box->position, collision_box->size, color); // 绘制碰撞箱
	}
}
module;

#include <algorithm>
#include <filesystem>
#include <string>
#include <unordered_map>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>

export module FlexCore:FCE_Handler;

import :FCE_BaseSetup;
import :FCE_Utils;
import :FCE_Component;
import :FCE_Graphic;

// 资源管理器
export class ResourcesManager
{
public:
	// 获取资源管理器单例
	static ResourcesManager* instance()
	{
		if (!m_instance)
			m_instance = new ResourcesManager();
		return m_instance;
	}

	// 加载资源
	void load_resources(const char* directory)
	{
		using namespace std::filesystem;

		if (!exists(directory))	// 判断文件是否存在
		{
			std::u8string _Info = u8"Dictionary “" + std::u8string((const char8_t*)directory) + u8"” is an error dictionary!";
			throw custom_error(u8"ResourcesManager Error", _Info.c_str());
		}

		// 遍历目标文件内部所有文件
		for (const auto& _Entry : recursive_directory_iterator(directory))
		{
			if (_Entry.is_regular_file())	// 如果是有效文件
			{
				const auto& _Path = _Entry.path();
				if (_Path.extension() == ".bmp" || _Path.extension() == ".png" || _Path.extension() == ".jpg")
				{
					SDL_Texture* _Texture = IMG_LoadTexture(Main_Renderer, _Path.string().c_str());
					texture_pool[_Path.stem().string()] = _Texture;
				}
				if (_Path.extension() == ".wav")
				{
					Mix_Chunk* _Audio = Mix_LoadWAV(_Path.string().c_str());
					audio_pool[_Path.stem().string()] = _Audio;
				}
				if (_Path.extension() == ".mp3" || _Path.extension() == ".ogg")
				{
					Mix_Music* _Music = Mix_LoadMUS(_Path.string().c_str());
					music_pool[_Path.stem().string()] = _Music;
				}
				if (_Path.extension() == ".ttf" || _Path.extension() == ".TTF" || _Path.extension() == ".ttc")
				{
					TTF_Font* _Font = TTF_OpenFont(_Path.string().c_str(), 24);	// 默认24号
					font_pool[_Path.stem().string()] = _Font;
				}
			}
		}

	}

	// 查找纹理资源
	SDL_Texture* find_texture(const std::string& name)
	{
		if (texture_pool.find(name) == texture_pool.end())
		{
			std::u8string _Info = u8"Texture “" + std::u8string((const char8_t*)name.c_str()) + u8"” is not found!";
			throw custom_error(u8"ResourcesManager Error", _Info.c_str());
		}
		return texture_pool[name];
	}

	// 查找音频资源
	Mix_Chunk* find_audio(const std::string& name)
	{
		if (audio_pool.find(name) == audio_pool.end())
		{
			std::u8string _Info = u8"Audio “" + std::u8string((const char8_t*)name.c_str()) + u8"” is not found!";
			throw custom_error(u8"ResourcesManager Error", _Info.c_str());
		}
		return audio_pool[name];
	}

	// 查找音乐资源
	Mix_Music* find_music(const std::string& name)
	{
		if (music_pool.find(name) == music_pool.end())
		{
			std::u8string _Info = u8"Music “" + std::u8string((const char8_t*)name.c_str()) + u8"” is not found!";
			throw custom_error(u8"ResourcesManager Error", _Info.c_str());
		}
		return music_pool[name];
	}

	// 查找字体资源
	TTF_Font* find_font(const std::string& name)
	{
		if (font_pool.find(name) == font_pool.end())
		{
			std::u8string _Info = u8"Font “" + std::u8string((const char8_t*)name.c_str()) + u8"” is not found!";
			throw custom_error(u8"ResourcesManager Error", _Info.c_str());
		}
		return font_pool[name];
	}

	// 显示已加载的资源
	void debug_resources_list()
	{
		printf("================================\n");
		printf("已加载的纹理资源如下 (共%d个):\n", (int)texture_pool.size());
		for (auto& texture : texture_pool)
			printf("%s\n", texture.first.c_str());

		printf("\n已加载的音频资源如下 (共%d个):\n", (int)audio_pool.size() + (int)music_pool.size());
		for (auto& audio : audio_pool)
			printf("%s\n", audio.first.c_str());
		for (auto& music : music_pool)
			printf("%s\n", music.first.c_str());

		printf("\n已加载的字体资源如下 (共%d个):\n", (int)font_pool.size());
		for (auto& font : font_pool)
			printf("%s\n", font.first.c_str());
		printf("================================\n");
	}

private:
	ResourcesManager() = default;
	~ResourcesManager() = default;

private:
	static ResourcesManager* m_instance;	// 资源管理器单例
	std::unordered_map<std::string, SDL_Texture*> texture_pool;	// 纹理资源池
	std::unordered_map<std::string, Mix_Chunk*> audio_pool;		// 音频资源池
	std::unordered_map<std::string, Mix_Music*> music_pool;		// 音乐资源池
	std::unordered_map<std::string, TTF_Font*> font_pool;		// 字体资源池
};
ResourcesManager* ResourcesManager::m_instance = nullptr;
export using ResLoader = ResourcesManager;

// 碰撞管理器
export class CollisionManager
{
public:
	// 获取碰撞管理器单例
	static CollisionManager* instance()
	{
		if (!m_instance)
			m_instance = new CollisionManager();
		return m_instance;
	}

	// 创建碰撞箱
	CollisionBox* create_collision_box()
	{
		CollisionBox* _New_box = new CollisionBox();
		collision_box_list.push_back(_New_box);
		return _New_box;
	}

	// 销毁碰撞箱
	void destroy_collision_box(CollisionBox* collision_box)
	{
		collision_box_list.erase(std::remove(collision_box_list.begin(), collision_box_list.end(), collision_box),
			collision_box_list.end());
		delete collision_box;
	}

	// 处理碰撞检测
	void process_collision()
	{
		for (CollisionBox* box_src : collision_box_list)
		{
			// 源碰撞箱未启用 or 没有目标碰撞层
			if (!box_src->enabled || box_src->layer_dst == CollisionLayer::None)
				continue;

			for (CollisionBox* box_dst : collision_box_list)
			{
				// 目标碰撞箱未启用 or 是源碰撞箱 or 该碰撞层不是源碰撞箱要检测的
				if (!box_dst->enabled || box_src == box_dst || !(box_src->layer_dst & box_dst->layer_src))
					continue;

				// 分别计算源碰撞箱和目标碰撞箱的最大最小x坐标 (即两碰撞箱的最左边和最右边)
				float max_x = std::max(box_src->position.x + box_src->size.w / 2.0f, box_dst->position.x + box_dst->size.w / 2.0f);
				float min_x = std::min(box_src->position.x - box_src->size.w / 2.0f, box_dst->position.x - box_dst->size.w / 2.0f);

				// 若最左边与最右边的坐标之差 <= 两碰撞箱宽度之和，那就是在x轴上碰到了
				bool is_collide_x = (max_x - min_x <= box_src->size.w + box_dst->size.w);

				// 分别计算源碰撞箱和目标碰撞箱的最大最小y坐标 (即两碰撞箱的最上边和最下边)
				float max_y = std::max(box_src->position.y + box_src->size.h / 2.0f, box_dst->position.y + box_dst->size.h / 2.0f);
				float min_y = std::min(box_src->position.y - box_src->size.h / 2.0f, box_dst->position.y - box_dst->size.h / 2.0f);

				// 若最上边与最下边的坐标之差 <= 两碰撞箱高度之和，那就是在y轴上碰到了
				bool is_collide_y = (max_y - min_y <= box_src->size.h + box_dst->size.h);

				// 若 x轴y轴都碰上了，且目标碰撞箱有回调函数
				if (is_collide_x && is_collide_y && box_dst->on_collide)
					box_dst->on_collide(box_src->layer_src); // 调用碰撞回调
			}
		}
	}

	// 调试碰撞信息
	void debug_collision_box(const Camera& camera)
	{
		for (CollisionBox* box : collision_box_list)
		{
			SDL_Color color;

			// 设置颜色信息
			if (box->enabled)
				color = { 255,195,195,255 };
			else
				color = { 115,155,175,255 };

			Renderer::render_rect_center(camera, box->position, box->size, color);
		}
	}

private:
	CollisionManager() = default;
	~CollisionManager() = default;

private:
	static CollisionManager* m_instance;	// 碰撞管理器单例
	std::vector<CollisionBox*> collision_box_list;	// 碰撞箱列表（以后可能支持四叉树）
};
CollisionManager* CollisionManager::m_instance = nullptr;

// 场景管理器
export class SceneManager
{
public:
	static SceneManager* instance()
	{
		if (!m_instance)
			m_instance = new SceneManager();
		return m_instance;
	}

	void set_current_scene(const std::string& name)
	{
		if (scene_pool.find(name) == scene_pool.end())
		{
			std::u8string _Info = u8"Scene “" + std::u8string((const char8_t*)name.c_str()) + u8"” is not exist!";
			throw custom_error(u8"Scene Manager Error", _Info.c_str());
		}

		current_scene = scene_pool[name];
		current_scene->on_enter();
	}

	void switch_to(const std::string& name)
	{
		current_scene->on_exit();

		if (scene_pool.find(name) == scene_pool.end())
		{
			std::u8string _Info = u8"Scene “" + std::u8string((const char8_t*)name.c_str()) + u8"” is not exist!";
			throw custom_error(u8"Scene Manager Error", _Info.c_str());
		}

		current_scene = scene_pool[name];
		current_scene->on_enter();
	}

	void register_scene(const std::string& name, Scene* scene)
	{
		if (scene_pool.find(name) != scene_pool.end())
		{
			std::u8string _Info = u8"Scene “" + std::u8string((const char8_t*)name.c_str()) + u8"” is already exist!";
			throw custom_error(u8"SceneManager Error", _Info.c_str());
		}
		scene_pool[name] = scene;
	}

	Scene* find_scene(const std::string& name)
	{
		if (scene_pool.find(name) == scene_pool.end())
		{
			std::u8string _Info = u8"Scene “" + std::u8string((const char8_t*)name.c_str()) + u8"” is not exist!";
			throw custom_error(u8"Scene Manager Error", _Info.c_str());
		}
		return scene_pool[name];
	}

	void on_input(const SDL_Event& event)
	{
		if (current_scene)
			current_scene->on_input(event);
	}

	void on_update(float delta)
	{
		if (current_scene)
			current_scene->on_update(delta);
	}

	void on_render(const Camera& cam_game, const Camera& cam_ui)
	{
		if (current_scene)
			current_scene->on_render(cam_game, cam_ui);
	}

private:
	SceneManager() = default;
	~SceneManager() = default;

private:
	static SceneManager* m_instance;	// 场景管理器单例
	Scene* current_scene = nullptr;		// 当前场景
	std::unordered_map<std::string, Scene*> scene_pool;	// 场景池
};
SceneManager* SceneManager::m_instance = nullptr;
export using Director = SceneManager;
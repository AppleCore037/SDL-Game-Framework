module;

#include <algorithm>
#include <array>
#include <filesystem>
#include <string>
#include <unordered_map>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>

export module FlexCore:FCE_Handler;

import :FCE_BaseSetup;
import :FCE_BaseType;
import :FCE_Utils;
import :FCE_Component;
import :FCE_Graphic;
import :FCE_Element;

export namespace fce
{
	// 资源管理器
	class ResourcesManager
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
				std::string _info = "[load_resources()]: Dictionary \"" + std::string(directory) + "\" is an error dictionary!";
				throw custom_error("ResourcesManager Error", _info.c_str());
			}

			// 遍历目标文件内部所有文件
			for (const auto& _entry : recursive_directory_iterator(directory))
			{
				if (_entry.is_regular_file())	// 如果是有效文件
				{
					const auto& _path = _entry.path();
					if (_path.extension() == ".bmp" || _path.extension() == ".png" || _path.extension() == ".jpg")
					{
						SDL_Texture* _texture = IMG_LoadTexture(Main_Renderer, _path.string().c_str());
						SDL_SetTextureScaleMode(_texture, scale_mode);	// 设置缩放模式
						texture_pool[_path.stem().string()] = _texture;
					}
					if (_path.extension() == ".wav")
					{
						Mix_Chunk* _audio = Mix_LoadWAV(_path.string().c_str());
						audio_pool[_path.stem().string()] = _audio;
					}
					if (_path.extension() == ".mp3" || _path.extension() == ".ogg")
					{
						Mix_Music* _music = Mix_LoadMUS(_path.string().c_str());
						music_pool[_path.stem().string()] = _music;
					}
					if (_path.extension() == ".ttf" || _path.extension() == ".TTF" || _path.extension() == ".ttc")
					{
						TTF_Font* _font = TTF_OpenFont(_path.string().c_str(), 24);	// 默认24号
						font_pool[_path.stem().string()] = _font;
					}
				}
			}
		}

		// 查找纹理资源
		SDL_Texture* find_texture(const std::string& name)
		{
			if (texture_pool.find(name) == texture_pool.end())
			{
				std::string _info = "[find_texture()]: Texture \"" + name + "\" is not found!";
				throw custom_error("ResourcesManager Error", _info.c_str());
			}
			return texture_pool[name];
		}

		// 查找音频资源
		Mix_Chunk* find_audio(const std::string& name)
		{
			if (audio_pool.find(name) == audio_pool.end())
			{
				std::string _info = "[find_audio()]: Audio \"" + name + "\" is not found!";
				throw custom_error("ResourcesManager Error", _info.c_str());
			}
			return audio_pool[name];
		}

		// 查找音乐资源
		Mix_Music* find_music(const std::string& name)
		{
			if (music_pool.find(name) == music_pool.end())
			{
				std::string _info = "[find_music()]: Music \"" + name + "\" is not found!";
				throw custom_error("ResourcesManager Error", _info.c_str());
			}
			return music_pool[name];
		}

		// 查找字体资源
		TTF_Font* find_font(const std::string& name)
		{
			if (font_pool.find(name) == font_pool.end())
			{
				std::string _info = "[find_font()]: Font \"" + name + "\" is not found!";
				throw custom_error("ResourcesManager Error", _info.c_str());
			}
			return font_pool[name];
		}

		// 显示已加载的资源
		void debug_resources_list()
		{
			printf("================================\n");
			printf("已加载的纹理资源如下 (共%d个):\n", (int)texture_pool.size());
			for (auto& _texture : texture_pool)
				printf("%s\n", _texture.first.c_str());

			printf("\n已加载的音频资源如下 (共%d个):\n", (int)audio_pool.size() + (int)music_pool.size());
			for (auto& _audio : audio_pool)
				printf("%s\n", _audio.first.c_str());
			for (auto& _music : music_pool)
				printf("%s\n", _music.first.c_str());

			printf("\n已加载的字体资源如下 (共%d个):\n", (int)font_pool.size());
			for (auto& _font : font_pool)
				printf("%s\n", _font.first.c_str());
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
	class CollisionManager
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
			CollisionBox* _new_box = new CollisionBox();
			collision_box_list.push_back(_new_box);
			return _new_box;
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

					Vector2 normal;	// 分离轴
					float depth;	// 碰撞深度

					box_src->set_corners();	// 设置源碰撞箱的四个角
					box_dst->set_corners(); // 设置目标碰撞箱的四个角
					bool is_collide = SAT_collision(box_src, box_dst, normal, depth);

					CollisionInfo info;
					info.normal = normal, info.depth = depth, info.other_layer = box_src->layer_src;
					
					if (is_collide && box_dst->on_collide)
					{
						if (box_src->send_props) box_src->send_props(info);
						box_dst->on_collide(info);
					}
				}
			}
		}

		// 调试碰撞信息
		void debug_collision_box(const Camera& camera)
		{
			for (CollisionBox* box : collision_box_list)
			{
				SDL_Color _color;

				// 设置颜色信息
				if (box->enabled) _color = { 255,195,195,255 };
				else _color = { 115,155,175,255 };

				box->set_corners();
				Renderer::render_line(camera, box->corners[0], box->corners[1], _color);
				Renderer::render_line(camera, box->corners[1], box->corners[2], _color);
				Renderer::render_line(camera, box->corners[2], box->corners[3], _color);
				Renderer::render_line(camera, box->corners[3], box->corners[0], _color);
			}
		}

	private:
		CollisionManager() = default;
		~CollisionManager() = default;

		// SAT分离轴碰撞检测
		bool SAT_collision(CollisionBox* src, CollisionBox* dst, Vector2& out_normal, float& out_depth)
		{
			/* 以下是2-SAT的碰撞检测算法，原理为如果两个矩形在任意轴上的投影都有重合部分，说明这两个矩形相交了
			   具体逻辑为分别计算两个碰撞箱(矩形)的长和宽，并获取其法向量作为候选分离轴，判断两矩形的投影是否在四条轴上都相交 */

			// 建立四条候选轴
			axes[0] = (src->corners[1] - src->corners[0]).get_normal();
			axes[1] = (src->corners[3] - src->corners[0]).get_normal();
			axes[2] = (dst->corners[1] - dst->corners[0]).get_normal();
			axes[3] = (dst->corners[3] - dst->corners[0]).get_normal();

			float min_overlap = INFINITY;
			Vector2 best_normal;

			// 对于每个轴进行投影计算
			for (Vector2 axis : axes)
			{
				// 计算源碰撞箱的投影
				float min_src = INFINITY, max_src = -INFINITY;
				for (Vector2 corner : src->corners)
				{
					float p = corner * axis; // 矩形的顶点在当前预选轴上的投影长度
					min_src = std::min(min_src, p);
					max_src = std::max(max_src, p);
				}

				// 计算目标碰撞箱的投影
				float min_dst = INFINITY, max_dst = -INFINITY;
				for (Vector2 corner : dst->corners)
				{
					float p = corner * axis; // 矩形的顶点在当前预选轴上的投影长度
					min_dst = std::min(min_dst, p);
					max_dst = std::max(max_dst, p);
				}

				// 在这条轴上没有重叠就是没有发生碰撞
				if (max_src < min_dst || max_dst < min_src) return false;

				// 计算重叠量，并更新最小重叠量与最佳分离轴
				float overlap = std::min(max_src, max_dst) - std::max(min_src, min_dst);
				if (overlap < min_overlap)
				{
					min_overlap = overlap;
					best_normal = axis;
				}
			}

			Vector2 diff = src->position - dst->position; // 获得一个指向源碰撞箱的向量
			if (diff * best_normal < 0) best_normal = -best_normal;	// 翻转，确保碰撞法线指向源碰撞箱

			out_normal = best_normal;
			out_depth = min_overlap;
			return true;	// 每个方向上的轴都有重合，说明碰上了
		}

	private:
		static CollisionManager* m_instance;	// 碰撞管理器单例
		std::vector<CollisionBox*> collision_box_list;	// 碰撞箱列表（以后可能支持四叉树）
		std::array<Vector2, 4> axes;	// 碰撞的四条候选轴
	};
	CollisionManager* CollisionManager::m_instance = nullptr;

	// 场景管理器
	class SceneManager
	{
	public:
		// 获取场景管理器单例
		static SceneManager* instance()
		{
			if (!m_instance)
				m_instance = new SceneManager();
			return m_instance;
		}

		// 设置当前场景
		void set_current_scene(const std::string& name)
		{
			if (scene_pool.find(name) == scene_pool.end())
			{
				std::string _info = "[set_current_scene()]: Scene \"" + name + "\" is not exist!";
				throw custom_error("Scene Manager Error", _info.c_str());
			}

			current_scene = scene_pool[name];
			current_scene->on_enter();
		}

		// 切换场景
		void switch_to(const std::string& name)
		{
			current_scene->on_exit();

			if (scene_pool.find(name) == scene_pool.end())
			{
				std::string _info = "[switch_to()]: Scene \"" + name + "\" is not exist!";
				throw custom_error("Scene Manager Error", _info.c_str());
			}

			current_scene = scene_pool[name];
			current_scene->on_enter();
		}

		// 添加场景
		void add_scene(const std::string& name, Scene* scene)
		{
			if (scene_pool.find(name) != scene_pool.end())
			{
				std::string _info = "[add_scene()]: Scene \"" + name + "\" is already exist!";
				throw custom_error("SceneManager Error", _info.c_str());
			}
			scene_pool[name] = scene;
		}

		// 查找场景
		Scene* find_scene(const std::string& name)
		{
			if (scene_pool.find(name) == scene_pool.end())
			{
				std::string _info = "[find_scene()]: Scene \"" + name + "\" is not exist!";
				throw custom_error("Scene Manager Error", _info.c_str());
			}
			return scene_pool[name];
		}

		// 处理输入
		void on_input(const SDL_Event& event)
		{
			if (current_scene)
				current_scene->on_input(event);
		}

		// 处理数据
		void on_update(float delta)
		{
			if (current_scene)
				current_scene->on_update(delta);
		}

		// 更新画面
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
}
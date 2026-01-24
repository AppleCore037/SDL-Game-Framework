#pragma once

#include "fce_base.h"
#include "fce_elements.h"
#include "fce_kits.h"
#include "fce_utils.h"

#include <filesystem>
#include <unordered_map>

namespace fce
{
	// 资源管理器
	class ResourcesManager
	{
	public:
		// 获取资源管理器单例
		static ResourcesManager* instance();

		// 加载资源
		void load_resources(const char* directory);

		// 查找纹理资源
		SDL_Texture* find_texture(const std::string& name);

		// 查找音频资源
		Mix_Chunk* find_audio(const std::string& name);

		// 查找音乐资源
		Mix_Music* find_music(const std::string& name);

		// 查找字体资源
		TTF_Font* find_font(const std::string& name);

		// 显示已加载的资源
		void debug_resources_list();

	private:
		ResourcesManager() = default;
		~ResourcesManager() = default;

	private:
		static ResourcesManager* m_instance;							// 资源管理器单例

		std::unordered_map<std::string, SDL_Texture*> texture_pool;		// 纹理资源池
		std::unordered_map<std::string, Mix_Chunk*> audio_pool;			// 音频资源池
		std::unordered_map<std::string, Mix_Music*> music_pool;			// 音乐资源池
		std::unordered_map<std::string, TTF_Font*> font_pool;			// 字体资源池
	};
	typedef fce::ResourcesManager ResLoader;

	// 场景管理器
	class SceneManager
	{
	public:
		// 获取场景管理器单例
		static SceneManager* instance();

		// 设置当前场景
		void set_current_scene(const std::string& name);

		// 切换场景
		void switch_to(const std::string& name);

		// 注册新场景
		void register_scene(const std::string& name, Scene* scene);

		// 获取场景
		Scene* find_scene(const std::string& name);

		// 更新当前场景
		void on_update(float delta);

		// 渲染当前场景
		void on_render(const Camera2D& camera_game, const Camera2D& camera_ui);

		// 处理输入事件
		void on_input(const SDL_Event& event);

	private:
		SceneManager() = default;
		~SceneManager() = default;

	private:
		static SceneManager* m_instance;					// 场景管理器单例
		Scene* current_scene = nullptr;						// 当前场景
		std::unordered_map<std::string, Scene*> scene_pool;	// 场景池
	};
	typedef fce::SceneManager Director;

	// 碰撞管理器
	class CollisionManager
	{
	public:
		// 获取碰撞管理器单例
		static CollisionManager* instance();

		// 添加碰撞箱
		CollisionBox* create_collision_box();

		// 销毁碰撞箱
		void destroy_collision_box(CollisionBox* collision_box);

		// 处理碰撞检测
		void process_collision();

		// 调试碰撞箱
		void debug_collision_box(const Camera2D& camera);

	private:
		CollisionManager() = default;
		~CollisionManager() = default;

	private:
		static CollisionManager* m_instance;			// 碰撞管理器单例
		std::vector<CollisionBox*> collision_box_list;	// 碰撞箱列表
	};
};
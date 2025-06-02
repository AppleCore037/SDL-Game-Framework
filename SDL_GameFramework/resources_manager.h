#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include <unordered_map>

// 资源管理器
class ResourcesManager
{
public:
	// 获取资源管理器单例
	static ResourcesManager* instance();

	// 加载资源
	void load_resources(SDL_Renderer* renderer, const char* _PathName);

	// 查找纹理资源
	SDL_Texture* find_texture(const std::string& name);

	// 查找音频资源
	Mix_Chunk* find_audio(const std::string& name);

	// 查找字体资源
	TTF_Font* find_font(const std::string& name);

	// 显示已加载的资源
	void debug_resources_list();

private:
	ResourcesManager() = default;
	~ResourcesManager() = default;

private:
	static ResourcesManager* m_instance;								// 资源管理器单例
	std::unordered_map<std::string, SDL_Texture*> texture_pool;			// 纹理资源池
	std::unordered_map<std::string, Mix_Chunk*> audio_pool;				// 音频资源池
	std::unordered_map<std::string, TTF_Font*> font_pool;				// 字体资源池
};
typedef ResourcesManager ResLoader;
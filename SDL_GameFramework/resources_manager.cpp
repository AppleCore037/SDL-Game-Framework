#include "resources_manager.h"
#include <filesystem>

ResourcesManager* ResourcesManager::m_instance = nullptr;

// 获取资源管理器单例
ResourcesManager* ResourcesManager::instance()
{
	if (!m_instance)
		m_instance = new ResourcesManager();

	return m_instance;
}

// 加载资源
void ResourcesManager::load_resources(SDL_Renderer* renderer, const char* _PathName)
{
	using namespace std::filesystem;

	// 判断文件是否存在
	if (!exists(_PathName))
	{
		std::string info = u8"Dictionary “" + std::string(_PathName) + u8"” is an error dictionary!";
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, u8"Resources Manager Error", info.c_str(), nullptr);
		exit(-1);
	}

	// 遍历目标文件内部所有文件
	for (const auto& entry : recursive_directory_iterator(_PathName))
	{
		if (entry.is_regular_file())	// 如果是有效文件
		{
			const path& path = entry.path();
			if (path.extension() == ".bmp" || path.extension() == ".png" || path.extension() == ".jpg")
			{
				SDL_Texture* texture = IMG_LoadTexture(renderer, path.u8string().c_str());
				texture_pool[path.stem().u8string()] = texture;
			}
			if (path.extension() == ".wav" || path.extension() == ".mp3" || path.extension() == ".ogg")
			{
				Mix_Chunk* audio = Mix_LoadWAV(path.u8string().c_str());
				audio_pool[path.stem().u8string()] = audio;
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
SDL_Texture* ResourcesManager::find_texture(const std::string& name)
{
	// 未找到
	if (texture_pool.find(name) == texture_pool.end())
	{
		std::string info = u8"Texture “" + name + u8"” is not found!";
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, u8"Resources Manager Error", info.c_str(), nullptr);
		exit(-1);
	}

	return texture_pool[name];
}

// 查找音频资源
Mix_Chunk* ResourcesManager::find_audio(const std::string& name)
{
	// 未找到
	if (audio_pool.find(name) == audio_pool.end())
	{
		std::string info = u8"Audio “" + name + u8"” is not found!";
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, u8"Resources Manager Error", info.c_str(), nullptr);
		exit(-1);
	}

	return audio_pool[name];
}

// 查找字体资源
TTF_Font* ResourcesManager::find_font(const std::string& name)
{
	// 未找到
	if (font_pool.find(name) == font_pool.end())
	{
		std::string info = u8"Font “" + name + u8"” is not found!";
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, u8"Resources Manager Error", info.c_str(), nullptr);
		exit(-1);
	}

	return font_pool[name];
}

// 显示已加载的资源
void ResourcesManager::debug_resources_list()
{
	printf("已加载的纹理资源如下 (共%d个):\n", (int)texture_pool.size());
	for (auto& texture : texture_pool)
		printf("%s\n", texture.first.c_str());

	printf("\n已加载的音频资源如下 (共%d个):\n", (int)audio_pool.size());
	for (auto& audio : audio_pool)
		printf("%s\n", audio.first.c_str());

	printf("\n已加载的字体资源如下 (共%d个):\n", (int)font_pool.size());
	for (auto& font : font_pool)
		printf("%s\n", font.first.c_str());
}
#include "resources_manager.h"
#include <filesystem>

ResourcesManager* ResourcesManager::m_instance = nullptr;

// ��ȡ��Դ����������
ResourcesManager* ResourcesManager::instance()
{
	if (!m_instance)
		m_instance = new ResourcesManager();

	return m_instance;
}

// ������Դ
void ResourcesManager::load_resources(SDL_Renderer* renderer, const char* _PathName)
{
	using namespace std::filesystem;

	// �ж��ļ��Ƿ����
	if (!exists(_PathName))
	{
		std::string info = u8"Dictionary ��" + std::string(_PathName) + u8"�� is an error dictionary!";
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, u8"Resources Manager Error", info.c_str(), nullptr);
		exit(-1);
	}

	// ����Ŀ���ļ��ڲ������ļ�
	for (const auto& entry : recursive_directory_iterator(_PathName))
	{
		if (entry.is_regular_file())	// �������Ч�ļ�
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

// ����������Դ
SDL_Texture* ResourcesManager::find_texture(const std::string& name)
{
	// δ�ҵ�
	if (texture_pool.find(name) == texture_pool.end())
	{
		std::string info = u8"Texture ��" + name + u8"�� is not found!";
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, u8"Resources Manager Error", info.c_str(), nullptr);
		exit(-1);
	}

	return texture_pool[name];
}

// ������Ƶ��Դ
Mix_Chunk* ResourcesManager::find_audio(const std::string& name)
{
	// δ�ҵ�
	if (audio_pool.find(name) == audio_pool.end())
	{
		std::string info = u8"Audio ��" + name + u8"�� is not found!";
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, u8"Resources Manager Error", info.c_str(), nullptr);
		exit(-1);
	}

	return audio_pool[name];
}

// ����������Դ
TTF_Font* ResourcesManager::find_font(const std::string& name)
{
	// δ�ҵ�
	if (font_pool.find(name) == font_pool.end())
	{
		std::string info = u8"Font ��" + name + u8"�� is not found!";
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, u8"Resources Manager Error", info.c_str(), nullptr);
		exit(-1);
	}

	return font_pool[name];
}

// ��ʾ�Ѽ��ص���Դ
void ResourcesManager::debug_resources_list()
{
	printf("�Ѽ��ص�������Դ���� (��%d��):\n", (int)texture_pool.size());
	for (auto& texture : texture_pool)
		printf("%s\n", texture.first.c_str());

	printf("\n�Ѽ��ص���Ƶ��Դ���� (��%d��):\n", (int)audio_pool.size());
	for (auto& audio : audio_pool)
		printf("%s\n", audio.first.c_str());

	printf("\n�Ѽ��ص�������Դ���� (��%d��):\n", (int)font_pool.size());
	for (auto& font : font_pool)
		printf("%s\n", font.first.c_str());
}
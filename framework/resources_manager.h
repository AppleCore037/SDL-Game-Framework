#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include <unordered_map>

// ��Դ������
class ResourcesManager
{
public:
	// ��ȡ��Դ����������
	static ResourcesManager* instance();

	// ������Դ
	void load_resources(SDL_Renderer* renderer, const char* _PathName);

	// ����������Դ
	SDL_Texture* find_texture(const std::string& name);

	// ������Ƶ��Դ
	Mix_Chunk* find_audio(const std::string& name);

	// ����������Դ
	TTF_Font* find_font(const std::string& name);

	// ��ʾ�Ѽ��ص���Դ
	void debug_resources_list();

private:
	ResourcesManager() = default;
	~ResourcesManager() = default;

private:
	static ResourcesManager* m_instance;								// ��Դ����������
	std::unordered_map<std::string, SDL_Texture*> texture_pool;			// ������Դ��
	std::unordered_map<std::string, Mix_Chunk*> audio_pool;				// ��Ƶ��Դ��
	std::unordered_map<std::string, TTF_Font*> font_pool;				// ������Դ��
};
typedef ResourcesManager ResLoader;
#include "fce_property.h"

// ======================================= Vector2 =======================================

// 获取长度
float fce::Vector2::length() const
{
	return sqrtf(x * x + y * y);
}

// 检测是否趋近与目标向量
bool fce::Vector2::approx(const Vector2& target) const
{
	return (*this - target).length() <= 0.0001f;
}

// 标准化
fce::Vector2 fce::Vector2::normalize() const
{
	float len = this->length();

	if (len == 0)
		return Vector2(0, 0);
	return Vector2(x / len, y / len);
}

// ======================================= Atlas =======================================

fce::Atlas::Atlas(const char* path_template, int beg_idx, int end_idx)
{
	this->load_from_file(path_template, beg_idx, end_idx);
}

fce::Atlas::~Atlas()
{
	for (SDL_Texture* texture : tex_list)
		SDL_DestroyTexture(texture);
}

// 从指定路径模板加载目标数量纹理
void fce::Atlas::load_from_file(const char* path_template, int beg_idx, int end_idx)
{
	for (int i = beg_idx; i <= end_idx; i++)
	{
		char path_file[256];
		sprintf_s(path_file, path_template, i);		// 补全路径

		SDL_Texture* texture = IMG_LoadTexture(Main_Renderer, path_file);	// 加载纹理
		if (texture == nullptr)
		{
			std::string info = u8"Cannot load texture from “" + std::string(path_file) + u8"”\nPlease check path correctness or image's existence!";
			throw std::runtime_error(info.c_str());
		}

		tex_list.push_back(texture);	// 加入纹理列表
	}
}

// 清空图集
void fce::Atlas::clear()
{
	tex_list.clear();
}

// 获取图集纹理数量
int fce::Atlas::get_size() const
{
	return (int)tex_list.size();
}

// 添加纹理
void fce::Atlas::add_texture(SDL_Texture* texture)
{
	tex_list.push_back(texture);
}

// 获取纹理
SDL_Texture* fce::Atlas::get_texture(int idx)
{
	if (idx < 0 || idx >= tex_list.size())
		return nullptr;
	return tex_list[idx];
}
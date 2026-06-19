module;

#include <any>
#include <cmath>
#include <stdexcept>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <format>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

export module FlexCore:FCE_BaseType;

import :FCE_BaseSetup;

export namespace fce
{
	// 大小
	struct Size
	{
		Size() = default;
		Size(float w, float h)
			: w(w), h(h) {}

		float w = 0.0f;
		float h = 0.0f;
	};

	// 二维向量
	class Vector2
	{
	public:
		float x = 0.0f;
		float y = 0.0f;

	public:
		Vector2() = default;

		~Vector2() = default;

		Vector2(float x, float y) : x(x), y(y) {}

		Vector2 operator+(const Vector2& vct2) const { return Vector2(x + vct2.x, y + vct2.y); }

		Vector2 operator-(const Vector2& vct2) const { return Vector2(x - vct2.x, y - vct2.y); }

		Vector2 operator-() const { return Vector2(-x, -y); }

		Vector2 operator*(float val) { return Vector2(x * val, y * val); }

		Vector2 operator/(float val) { return Vector2(x / val, y / val); }

		float operator*(const Vector2& vct2) { return x * vct2.x + y * vct2.y; }

		void operator+=(const Vector2& vct2) { x += vct2.x; y += vct2.y; }

		void operator-=(const Vector2& vct2) { x -= vct2.x; y -= vct2.y; }

		void operator/=(float val) { x /= val; y /= val; }

		void operator*=(float val) { x *= val; y *= val; }

		// 获取向量模长
		float length() const { return sqrtf(x * x + y * y); }

		// 检测是否趋近与目标向量
		bool approx(const Vector2& target) const { return (*this - target).length() <= 0.001f; }

		// 将向量旋转指定弧度
		Vector2 rotate(float angle)
		{
			float _cos_a = std::cosf(angle), _sin_a = std::sinf(angle);
			return Vector2(x * _cos_a - y * _sin_a, x * _sin_a + y * _cos_a);
		}

		// 获取法向量
		Vector2 get_normal() { return Vector2(-y, x).normalize(); }

		// 将向量标准化为单位向量
		Vector2 normalize() const
		{
			float _len = this->length();
			if (_len == 0) return Vector2(0, 0);
			return Vector2(x / _len, y / _len);
		}
	};

	// 变换矩形
	struct Rect
	{
	public:
		Vector2 position;					// 位置
		Size size;							// 尺寸
		Vector2 anchor = { 0.0f, 0.0f };	// 锚点
		float direction = 0.0f;				// 方向
		bool is_flip = false;				// 是否翻转

		// 获取SDL形式的矩形
		SDL_FRect& get_SDLRect()
		{
			this->rect = { position.x, position.y, size.w, size.h };
			return rect;
		}

	private:
		SDL_FRect rect; // SDL形式矩形
	};

	// 图集
	class Atlas
	{
	public:
		Atlas() = default;
		Atlas(const char* path_template, int beg, int end) { load_from_file(path_template, beg, end); }

		~Atlas()
		{
			for (SDL_Texture* _texture : tex_list)
				SDL_DestroyTexture(_texture);
		}

		// 从指定路径模板加载目标数量纹理
		void load_from_file(const char* path_template, int beg_idx, int end_idx)
		{
			for (int i = beg_idx; i <= end_idx; i++)
			{
				char _path_file[256];
				sprintf_s(_path_file, path_template, i);	// 补全路径

				SDL_Texture* _texture = IMG_LoadTexture(Main_Renderer, _path_file);	// 加载纹理
				SDL_SetTextureScaleMode(_texture, scale_mode);

				if (_texture == nullptr)	// 检查纹理加载状态
				{
					std::string _info = "[load_from_file()]: Cannot load texture from \"" + std::string(_path_file) + "\" !";
					throw std::runtime_error(_info.c_str());
				}

				tex_list.push_back(_texture);	// 加入纹理列表
			}
		}

		// 清空图集
		void clear() { tex_list.clear(); }

		// 获取图集纹理数量
		int get_size() const { return (int)tex_list.size(); }

		// 添加纹理
		void add_texture(SDL_Texture* texture)
		{
			SDL_SetTextureScaleMode(texture, scale_mode);
			tex_list.push_back(texture);
		}

		// 获取纹理
		SDL_Texture* get_texture(int idx)
		{
			if (idx < 0 || idx >= tex_list.size()) return nullptr;
			return tex_list[idx];
		}

	private:
		std::vector<SDL_Texture*> tex_list;		// 纹理集
	};

	// 文本
	class Text
	{
	public:
		Text()
		{
			if (!global_font) throw std::runtime_error("[Text Constructor]: Text default font not set!");
			this->text = TTF_CreateText(Main_TextEngine, global_font, nullptr, NULL);
		}

		Text(const std::string& info)
		{
			if (!global_font) throw std::runtime_error("[Text Constructor]: Text default font not set!");
			this->text = TTF_CreateText(Main_TextEngine, global_font, info.c_str(), NULL);
		}

		~Text() { TTF_DestroyText(text); }

		// 设置全局字体
		static void set_global_font(TTF_Font* font) { global_font = font; }

		// 设置私有字体
		void set_self_font(TTF_Font* font)
		{
			if (!font) TTF_SetTextFont(text, global_font);
			else TTF_SetTextFont(text, font);
		}

		// 设置文本内容
		void set_string(const std::string& info) { TTF_SetTextString(text, info.c_str(), NULL); }

		// 获取SDL格式的文本
		TTF_Text* get_SDLText() { return this->text; }

	private:
		TTF_Text* text = nullptr;
		static TTF_Font* global_font;
	};
	TTF_Font* Text::global_font = nullptr;

	// 碰撞信息上下文
	class CollisionInfo
	{
		using map_t = std::unordered_map<std::string, std::any>;
	public:
		Vector2 normal;				// 碰撞法线(指向自身)
		float depth;				// 碰撞深度
		CollisionLayer other_layer;	// 对方碰撞层

	public:
		CollisionInfo() = default;
		~CollisionInfo() = default;

		template <typename T>
		void pack(const std::string& name, const T& value)
		{
			map_t& map = bags[std::type_index(typeid(T))];
			map[name] = value;
		}

		template <typename T>
		T get(const std::string& name) const
		{
			auto it = bags.find(std::type_index(typeid(T)));
			if (it != bags.end())
			{
				auto val_it = it->second.find(name);
				if (val_it != it->second.end()) return std::any_cast<T>(val_it->second);
			}

			std::string _info = std::format("[CollisionInfo::get()]: Cannot get value!\ntype: {}\nname: {}", typeid(T).name(), name);
			throw std::runtime_error(_info.c_str());
		}

		template <typename T>
		bool has(const std::string& name) const
		{
			auto it = bags.find(std::type_index(typeid(T)));
			if (it != bags.end())
				return (it->second.find(name) != it->second.end());
			else
				return false;
		}

	private:
		std::unordered_map<std::type_index, map_t> bags;
	};
}
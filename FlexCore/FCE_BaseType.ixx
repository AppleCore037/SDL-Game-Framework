module;

#include <stdexcept>
#include <string>
#include <vector>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

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
		bool approx(const Vector2& target) const { return (*this - target).length() <= 0.0001f; }

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
					std::string _info = "Func <load_from_file()>: Cannot load texture from \"" + std::string(_path_file) + "\" !";
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

	// 状态节点
	class StateNode
	{
	public:
		StateNode() = default;
		virtual ~StateNode() = default;

		virtual void on_enter() {}				// 进入状态
		virtual void on_update(float delta) {}	// 更新状态
		virtual void on_exit() {}				// 退出状态
	};
}
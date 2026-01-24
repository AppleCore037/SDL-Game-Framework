#pragma once

#include "fce_base.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <stdexcept>
#include <string>
#include <vector>

namespace fce
{
	// 大小
	struct Size
	{
		Size() = default;
		~Size() = default;
		Size(float w, float h)
			: w(w), h(h) {}

		float w;
		float h;
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

		// 获取长度
		float length() const;

		// 检测是否趋近与目标向量
		bool approx(const Vector2& target) const;

		// 标准化
		Vector2 normalize() const;
	};
	using Point = Vector2;	// 坐标点

	// 图集
	class Atlas
	{
	public:
		Atlas() = default;
		Atlas(const char* path_template, int beg_idx, int end_idx);

		~Atlas();

		// 从指定路径模板加载目标数量纹理
		void load_from_file(const char* path_template, int beg_idx, int end_idx);

		// 清空图集
		void clear();

		// 获取图集纹理数量
		int get_size() const;

		// 添加纹理
		void add_texture(SDL_Texture* texture);

		// 获取纹理
		SDL_Texture* get_texture(int idx);

	private:
		std::vector<SDL_Texture*> tex_list;		// 纹理集
	};

	// 状态节点
	class StateNode
	{
	public:
		StateNode() = default;
		~StateNode() = default;

		virtual void on_enter() {}					// 进入状态
		virtual void on_update(float delta) {}		// 更新状态
		virtual void on_exit() {}					// 退出状态
	};
};
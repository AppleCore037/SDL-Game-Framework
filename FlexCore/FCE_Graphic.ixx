module;

#include <functional>
#include <string>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

export module FlexCore:FCE_Graphic;

import :FCE_BaseSetup;
import :FCE_BaseType;
import :FCE_Utils;

// 摄像机
export class Camera
{
public:
	// 跟随方式
	enum FollowStyle
	{
		None = 0,			// 无跟随
		Only_X = 1 << 0,	// 仅跟随X轴
		Only_Y = 1 << 1,	// 仅跟随Y轴
		Static = 1 << 2,	// 静态跟随
		Smooth = 1 << 3,	// 平滑跟随
	};

public:
	Camera(const Vector2& pos) : base_position(pos) {}
	~Camera() = default;

	// 设置位置
	void set_position(const Vector2& pos) { base_position = pos; }

	// 获取位置
	const Vector2& get_position() const { return position; }

	// 设置缩放
	void set_zoom(float scale)
	{
		if (scale <= 0.0f) zoom = 0.01f; //	防止缩放过小导致的渲染问题
		else zoom = scale;
	}

	// 获取缩放
	float get_zoom() const { return zoom; }

	// 重置摄像机
	void reset()
	{
		position = base_position = shake_position = Vector2(0, 0);
		this->zoom = 1.0f;
	}

	// 抖动摄像机
	void shake(float strength, float decay)
	{
		this->is_shaking = true;
		this->shake_strength = strength;
		this->shake_decay = (decay <= 0.0f ? 0.1f : decay);	// 防止幅度不衰减或负衰减
	}

	// 跟随目标，支持多种跟随方式（静态/平滑，单轴/双轴）
	void look_at(const Vector2& target, int style, float smooth_factor = 0.05f)
	{
		if (style & FollowStyle::None) return;

		if (style & FollowStyle::Static)	// 静态跟随
		{
			if (style & FollowStyle::Only_X)
				base_position.x = target.x;
			else if (style & FollowStyle::Only_Y)
				base_position.y = target.y;
			else
				base_position = target;
		}
		else if (style & FollowStyle::Smooth)	// 平滑跟随
		{
			float _Delta_smooth = smooth_factor * Clock::get_DeltaTime() * 60.0f;	// 保持系数稳定
			if (style & FollowStyle::Only_X)
				base_position.x = maths::lerp(base_position.x, target.x, _Delta_smooth);
			else if (style & FollowStyle::Only_Y)
				base_position.y = maths::lerp(base_position.y, target.y, _Delta_smooth);
			else
			{
				base_position.x = maths::lerp(base_position.x, target.x, _Delta_smooth);
				base_position.y = maths::lerp(base_position.y, target.y, _Delta_smooth);
			}
		}
	}

	// 自定义跟随逻辑，支持回调函数
	void look_at(const Vector2& target, std::function<void(const Vector2&)> follow) { follow(target); }

	// 世界坐标->窗口坐标
	Vector2 world_to_screen(const Vector2& world_pos) const
	{
		int window_w, window_h;
		SDL_GetWindowSize(Main_Window, &window_w, &window_h);

		// 窗口坐标 = 视野中心点 + (世界坐标 - 摄像机坐标) * 缩放因子 * 标准缩放比
		float _Screen_x = (window_w / 2.0f) + (world_pos.x - position.x) * zoom * maths::get_std_zoom_ratio();
		float _Screen_y = (window_h / 2.0f) + (world_pos.y - position.y) * zoom * maths::get_std_zoom_ratio();
		return Vector2(_Screen_x, _Screen_y);

		/* PS：这里如果是直接画在屏幕上的，则视野中心点就是屏幕尺寸÷2，渲染坐标就要适应缩放比，
			   但是如果间接画在画布上，再将画布以缩放比适配的形式铺在屏幕上，渲染坐标就不用适配缩放比 */
	}

	// 窗口坐标->世界坐标
	Vector2 screen_to_world(const Vector2& screen_pos) const
	{
		int window_w, window_h;
		SDL_GetWindowSize(Main_Window, &window_w, &window_h);

		// 世界坐标 = (窗口坐标 - 视野中心点) / (缩放因子 * 标准缩放比) + 摄像机坐标
		float _World_x = (screen_pos.x - window_w / 2.0f) / (zoom * maths::get_std_zoom_ratio()) + position.x;
		float _World_y = (screen_pos.y - window_h / 2.0f) / (zoom * maths::get_std_zoom_ratio()) + position.y;
		return Vector2(_World_x, _World_y);
	}
	
	// 判断对象是否在视野内(传原始是世界属性就行，内部会自动变换)
	bool target_in_view(const Vector2& pos, const Size& size) const
	{
		int window_w, window_h;
		SDL_GetWindowSize(Main_Window, &window_w, &window_h);

		// 世界属性变换
		Vector2 _Screen_pos = this->world_to_screen(pos);
		Size _Screen_size = {
			size.w * zoom * maths::get_std_zoom_ratio(),
			size.h * zoom * maths::get_std_zoom_ratio()
		};

		// 检查是否在视野内
		bool _In_range_x = (_Screen_pos.x >= -_Screen_size.w && _Screen_pos.x <= window_w + _Screen_size.w);
		bool _In_range_y = (_Screen_pos.y >= -_Screen_size.h && _Screen_pos.y <= window_h + _Screen_size.h);
		return (_In_range_x && _In_range_y);
	}

	// 更新摄像机状态
	void on_update(float delta)
	{
		if (is_shaking && shake_strength > 0.1f)	// 如果正在抖动且抖动幅度足够大
		{
			shake_position.x = (-50 + std::rand() % 100) / 50.0f * shake_strength;
			shake_position.y = (-50 + std::rand() % 100) / 50.0f * shake_strength;
			shake_strength *= 1.0f - shake_decay * delta;	// 衰减抖动幅度
		}
		else // 否则就重置抖动偏移
		{
			this->is_shaking = false;
			this->shake_position = Vector2(0, 0);
		}

		// 最终位置 = 基础位置 + 抖动位置
		position = base_position + shake_position;
	}

private:
	Vector2 position;		// 最终位置
	Vector2 base_position;	// 基础位置
	Vector2 shake_position;	// 抖动位置

	bool is_shaking = false;		// 是否正在抖动
	float shake_strength = 0.0f;	// 抖动幅度
	float shake_decay = 0.0f;		// 抖动衰减程度

	float zoom = 1.0f;	// 缩放大小
};

// 渲染器
export class Renderer
{
public:
	// 填充背景色
	static void fill(SDL_Color color)
	{
		SDL_SetRenderDrawColor(Main_Renderer, color.r, color.g, color.b, color.a);
		SDL_RenderClear(Main_Renderer);
	}

	// 渲染纹理
	static void render_texture(const Camera& camera, SDL_Texture* tex, const SDL_FRect* rect_src,
		const SDL_FRect* rect_dst, double angle, const Vector2& anchor = Vector2(0, 0), bool is_flip = false)
	{
		// 视野剔除
		if (!camera.target_in_view({ rect_dst->x, rect_dst->y }, { rect_dst->w, rect_dst->h }))
			return;

		SDL_FRect _Rect_dst_win = *rect_dst;

		// 纹理属性变换
		_Rect_dst_win.x = camera.world_to_screen(Vector2(rect_dst->x, rect_dst->y)).x;
		_Rect_dst_win.y = camera.world_to_screen(Vector2(rect_dst->x, rect_dst->y)).y;
		_Rect_dst_win.w *= camera.get_zoom() * maths::get_std_zoom_ratio();
		_Rect_dst_win.h *= camera.get_zoom() * maths::get_std_zoom_ratio();

		// 设置旋转中心
		SDL_FPoint _Rotate_center = { _Rect_dst_win.w * anchor.x, _Rect_dst_win.h * anchor.y };

		// 设置渲染中心
		_Rect_dst_win.x -= anchor.x * _Rect_dst_win.w;
		_Rect_dst_win.y -= anchor.y * _Rect_dst_win.h;

		SDL_RenderTextureRotated(Main_Renderer, tex, rect_src, &_Rect_dst_win, angle,
			&_Rotate_center, (is_flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE));
	}

	// 渲染线段
	static void render_line(const Camera& camera, const Vector2& begin, const Vector2& end, SDL_Color color)
	{
		// 视野剔除
		if (!camera.target_in_view(begin, { 0, 0 }) && !camera.target_in_view(end, { 0, 0 }))
			return;

		// 线段属性变换
		Vector2 _Screen_begin = camera.world_to_screen(begin);
		Vector2 _Screen_end = camera.world_to_screen(end);

		SDL_SetRenderDrawColor(Main_Renderer, color.r, color.g, color.b, color.a);
		SDL_RenderLine(Main_Renderer, _Screen_begin.x, _Screen_begin.y, _Screen_end.x, _Screen_end.y);
	}

	// 渲染矩形（以中心点为基准）
	static void render_rect_center(const Camera& camera, const Vector2& pos, const Size& size,
		SDL_Color color, bool is_filled = false)
	{
		// 视野剔除
		if (!camera.target_in_view(pos, size))
			return;

		SDL_SetRenderDrawColor(Main_Renderer, color.r, color.g, color.b, color.a);

		// 矩形属性变换
		Vector2 _Corner_pos = { pos.x - size.w / 2.0f, pos.y - size.h / 2.0f };	// 以中心点为基准，计算左上角位置
		SDL_FRect _Rect = {
			camera.world_to_screen(_Corner_pos).x,
			camera.world_to_screen(_Corner_pos).y,
			size.w * camera.get_zoom() * maths::get_std_zoom_ratio(),
			size.h * camera.get_zoom() * maths::get_std_zoom_ratio()
		};

		if (is_filled)
			SDL_RenderFillRect(Main_Renderer, &_Rect);
		else
			SDL_RenderRect(Main_Renderer, &_Rect);
	}

	// 渲染矩形（以左上角为基准）
	static void render_rect_corner(const Camera& camera, const Vector2& pos, const Size& size,
		SDL_Color color, bool is_filled = false)
	{
		// 视野剔除
		if (!camera.target_in_view(pos, size))
			return;

		SDL_SetRenderDrawColor(Main_Renderer, color.r, color.g, color.b, color.a);

		// 矩形属性变换
		SDL_FRect _Rect = {
			camera.world_to_screen(pos).x,
			camera.world_to_screen(pos).y,
			size.w * camera.get_zoom() * maths::get_std_zoom_ratio(),
			size.h * camera.get_zoom() * maths::get_std_zoom_ratio()
		};

		if (is_filled)
			SDL_RenderFillRect(Main_Renderer, &_Rect);
		else
			SDL_RenderRect(Main_Renderer, &_Rect);
	}

	// 渲染圆形
	static void render_circle(const Camera& camera, const Vector2& center_pos, float radius,
		SDL_Color color, bool is_filled = false)
	{
		// 视野剔除
		if (!camera.target_in_view(center_pos, { radius, radius }))
			return;

		// 圆形属性变换
		Vector2 _Screen_center = camera.world_to_screen(center_pos);
		float _Screen_radius = radius * camera.get_zoom() * maths::get_std_zoom_ratio();

		if (is_filled)
			maths::render_filled_circle(Main_Renderer, _Screen_center, _Screen_radius, color);
		else
			maths::render_circle(Main_Renderer, _Screen_center, _Screen_radius, color);
	}

	// 渲染文字
	static void render_text(const Camera& camera, const Vector2& pos, TTF_Text* text, float ptsize, SDL_Color color)
	{
		TTF_SetFontSize(TTF_GetTextFont(text), ptsize * camera.get_zoom() * maths::get_std_zoom_ratio());
		TTF_SetTextColor(text, color.r, color.g, color.b, color.a);
		TTF_DrawRendererText(text, camera.world_to_screen(pos).x, camera.world_to_screen(pos).y);
	}
};
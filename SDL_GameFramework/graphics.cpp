#include "graphics.h"
#include "utils.h"

// =========================================================================================
// 			摄 像 机 类							C a m e r a
// =========================================================================================

Camera::Camera(SDL_Renderer* renderer, SDL_Window* window)
{
	this->renderer = renderer;
	this->window = window;

	timer_shake.set_one_shot(true);
	timer_shake.set_on_timeout([&]()
		{
			is_shaking = false;
			shake_position = Vector2(0, 0);
		});
}

// 获取摄像机坐标(中心点)
const Vector2& Camera::get_position() const
{
	return position;
}

// 设置摄像机坐标(中心点)
void Camera::set_position(const Vector2& pos)
{
	this->base_position = pos;
}

// 设置摄像机缩放
void Camera::set_zoom(float scale)
{
	this->zoom = scale;
}

// 获取摄像机缩放
float Camera::get_zoom() const
{
	return this->zoom;
}

// 重置摄像机
void Camera::reset()
{
	this->position = Vector2(0, 0);
	this->shake_position = Vector2(0, 0);
	this->base_position = Vector2(0, 0);
}

// 抖动摄像机
void Camera::shake(float strength, float duration)
{
	is_shaking = true;
	shaking_strength = strength;

	timer_shake.set_wait_time(duration);
	timer_shake.restart();
}

// 渲染纹理
void Camera::render_texture(SDL_Texture* texture, const SDL_FRect* rect_src, const SDL_FRect* rect_dst,
	double angle, const SDL_FPoint* center, bool is_flip) const
{
	// 渲染坐标 = 屏幕中心点 + (角色世界坐标 - 摄像机坐标) * 标准缩放比例 * 缩放因子
	SDL_FRect rect_dst_win = *rect_dst;
	rect_dst_win.x = get_screen_center().x + (rect_dst_win.x - position.x) * get_uniform_scale() * zoom;
	rect_dst_win.y = get_screen_center().y + (rect_dst_win.y - position.y) * get_uniform_scale() * zoom;

	// 渲染大小 = 纹理大小 * 标准缩放比例 * 缩放因子
	rect_dst_win.w = rect_dst->w * get_uniform_scale() * zoom;
	rect_dst_win.h = rect_dst->h * get_uniform_scale() * zoom;

	SDL_RenderTextureRotated(renderer, texture, rect_src, &rect_dst_win, angle, center,
		(is_flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE));
}

// 跟随角色
void Camera::look_at(const Sprite* sprite, int style)
{
	if (style & None) return;

	Vector2 target_pos;		// 目标位置	
	target_pos.x = sprite->get_position().x;
	target_pos.y = sprite->get_position().y;

	if (style & Camera::Static_Follow)	// 静态跟随
	{
		if (style & Camera::Only_X)
			this->base_position.x = target_pos.x;
		else if (style & Camera::Only_Y)
			this->base_position.y = target_pos.y;
		else
			this->base_position = target_pos;
	}
	else if (style & Camera::Smooth_Follow)		// 平滑跟随
	{
		if (style & Camera::Only_X)
			this->base_position.x = utils::lerp(base_position.x, target_pos.x, smooth_strength);
		else if (style & Camera::Only_Y)
			this->base_position.y = utils::lerp(base_position.y, target_pos.y, smooth_strength);
		else
		{
			this->base_position.x = utils::lerp(base_position.x, target_pos.x, smooth_strength);
			this->base_position.y = utils::lerp(base_position.y, target_pos.y, smooth_strength);
		}
	}
}

// 更新摄像机
void Camera::on_update(float delta)
{
	timer_shake.on_update(delta);
	smooth_strength = SMOOTH_FACTOR * delta;

	if (is_shaking)
	{
		shake_position.x = (-50 + rand() % 100) / 50.0f * shaking_strength;
		shake_position.y = (-50 + rand() % 100) / 50.0f * shaking_strength;
	}

	// 最终位置 = 基础位置 + 抖动位置
	position = base_position + shake_position;
}

// 获取屏幕中心点
Vector2 Camera::get_screen_center() const
{
	int screen_w, screen_h;
	SDL_GetWindowSize(window, &screen_w, &screen_h);		// 计算屏幕宽高
	Vector2 screen_center = Vector2(screen_w / 2.0f, screen_h / 2.0f);	// 获取屏幕中心点
	return screen_center;
}

// 获取屏幕大小
Size Camera::get_screen_size() const
{
	int screen_w, screen_h;
	SDL_GetWindowSize(window, &screen_w, &screen_h);		// 计算屏幕宽高
	return Size(screen_w, screen_h);
}

// 获取屏幕标准缩放比例
float Camera::get_uniform_scale() const
{
	const float BASE_WIDTH = 1280.0f;		// 基础宽度
	const float BASE_HEIGHT = 720.0f;		// 基础高度

	float scaleX = this->get_screen_size().width / BASE_WIDTH;		// 水平缩放比例
	float scaleY = this->get_screen_size().height / BASE_HEIGHT;	// 垂直缩放比例

	return std::min(scaleX, scaleY);		// 返回统一缩放比例
}

// =========================================================================================
//			图 形 渲 染 类					 Shape	Maker
// =========================================================================================

// 渲染直线
void ShapeMaker::render_line(const Camera& camera, const Vector2& begin, 
	const Vector2& end, SDL_Color color)
{
	SDL_SetRenderDrawColor(camera.renderer, color.r, color.g, color.b, color.a);

	// 线端点的初始渲染坐标
	Vector2 begin_dst_win = Vector2(begin.x - camera.get_position().x, begin.y - camera.get_position().y);
	Vector2 end_dst_win = Vector2(end.x - camera.get_position().x, end.y - camera.get_position().y);

	// 处理线条属性
	begin_dst_win.x = camera.get_screen_center().x + begin_dst_win.x * camera.get_zoom() * camera.get_uniform_scale();
	begin_dst_win.y = camera.get_screen_center().y + begin_dst_win.y * camera.get_zoom() * camera.get_uniform_scale();
	end_dst_win.x = camera.get_screen_center().x + end_dst_win.x * camera.get_zoom() * camera.get_uniform_scale();
	end_dst_win.y = camera.get_screen_center().y + end_dst_win.y * camera.get_zoom() * camera.get_uniform_scale();

	SDL_RenderLine(camera.renderer, begin_dst_win.x, begin_dst_win.y, end_dst_win.x, end_dst_win.y);
}

// 渲染矩形
void ShapeMaker::render_rect(const Camera& camera, const Vector2& pos, Size rectSize, 
	SDL_Color color, bool is_filled)
{
	SDL_SetRenderDrawColor(camera.renderer, color.r, color.g, color.b, color.a);

	// 矩形的初始渲染属性
	SDL_FRect rect_dst_win = 
	{ 
		pos.x - camera.get_position().x, pos.y - camera.get_position().y,
		rectSize.width, rectSize.height 
	};

	// 处理矩形属性
	rect_dst_win.x = camera.get_screen_center().x + rect_dst_win.x * camera.get_zoom() * camera.get_uniform_scale();
	rect_dst_win.y = camera.get_screen_center().y + rect_dst_win.y * camera.get_zoom() * camera.get_uniform_scale();
	rect_dst_win.w = rect_dst_win.w * camera.get_zoom() * camera.get_uniform_scale();
	rect_dst_win.h = rect_dst_win.h * camera.get_zoom() * camera.get_uniform_scale();

	if (is_filled)
		SDL_RenderFillRect(camera.renderer, &rect_dst_win);
	else
		SDL_RenderRect(camera.renderer, &rect_dst_win);
}

// 渲染圆形
void ShapeMaker::render_circle(const Camera& camera, const Vector2& pos, float radius, 
	SDL_Color color, bool is_filled)
{
	SDL_SetRenderDrawColor(camera.renderer, color.r, color.g, color.b, color.a);

	// 圆的初始渲染属性
	Vector2 centerPos = Vector2(pos.x + radius, pos.y + radius);		// 计算圆的中心坐标
	centerPos.x = centerPos.x - camera.get_position().x;
	centerPos.y = centerPos.y - camera.get_position().y;

	// 处理圆形属性
	centerPos.x = camera.get_screen_center().x + centerPos.x * camera.get_zoom() * camera.get_uniform_scale();
	centerPos.y = camera.get_screen_center().y + centerPos.y * camera.get_zoom() * camera.get_uniform_scale();
	radius = radius * camera.get_uniform_scale() * camera.get_zoom();	// 计算圆的半径

	if (is_filled)
		utils::draw_FilledCircle(camera.renderer, centerPos.x, centerPos.y, radius);
	else
		utils::draw_circle(camera.renderer, centerPos.x, centerPos.y, radius);
}
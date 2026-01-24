#include "fce_kits.h"

// ======================================= Timer =======================================

// 重启计时器
void fce::Timer::restart()
{
	pass_time = 0; shotted = false;
}

// 设置等待时间
void fce::Timer::set_wait_time(float val)
{
	this->wait_time = val;
}

// 设置是否单次触发（默认false）
void fce::Timer::set_one_shot(bool flag)
{
	this->one_shot = flag;
}

// 设置回调函数
void fce::Timer::set_on_timeout(std::function<void()> timeout_callback)
{
	this->on_timeout = timeout_callback;
}

// 暂停
void fce::Timer::pause()
{
	paused = true;
}

// 继续
void fce::Timer::resume()
{
	paused = false;
}

// 更新计时器
void fce::Timer::on_update(float delta)
{
	if (paused) return;

	pass_time += delta;
	if (pass_time >= wait_time)
	{
		bool can_shot = (!one_shot || (one_shot && !shotted));
		shotted = true;

		if (can_shot && on_timeout)
			on_timeout();
		pass_time -= wait_time;
	}
}

// ======================================= Camera2D =======================================

fce::Camera2D::Camera2D()
{
	timer_shake.set_one_shot(true);
	timer_shake.set_on_timeout([&]()
		{
			is_shaking = false;
			shake_position = Vector2(0, 0);
		});
}

fce::Camera2D::Camera2D(const Point& pos) : Camera2D()
{
	this->base_position = pos;
}

// 获取摄像机世界坐标(中心点)
const fce::Point& fce::Camera2D::get_position() const
{
	return position;
}

// 设置摄像机世界坐标(中心点)
void fce::Camera2D::set_position(const Point& pos)
{
	this->base_position = pos;
}

// 设置摄像机缩放
void fce::Camera2D::set_zoom(float scale)
{
	this->zoom = scale;
}

// 获取摄像机缩放
float fce::Camera2D::get_zoom() const
{
	return this->zoom;
}

// 重置摄像机
void fce::Camera2D::reset()
{
	position = base_position = shake_position = Point(0, 0);
	this->zoom = 1.0f;		// 重置缩放为1.0
}

// 抖动摄像机
void fce::Camera2D::shake(float strength, float duration)
{
	is_shaking = true;
	shaking_strength = strength;

	timer_shake.set_wait_time(duration);
	timer_shake.restart();
}

// 渲染纹理
void fce::Camera2D::render_texture(SDL_Texture* texture, const SDL_FRect* rect_src, 
	const SDL_FRect* rect_dst, double angle, const SDL_FPoint center_anchor, bool is_flip) const
{
	SDL_FRect rect_dst_win = *rect_dst;

	// 设置渲染与旋转中心点
	SDL_FPoint rotate_center = { rect_dst->w * center_anchor.x * zoom, rect_dst->h * center_anchor.y * zoom };
	rect_dst_win.x = rect_dst_win.x - rect_dst->w * center_anchor.x;
	rect_dst_win.y = rect_dst_win.y - rect_dst->h * center_anchor.y;

	// 渲染坐标 = 屏幕中心点 + (角色世界坐标 - 摄像机坐标) * 缩放因子
	rect_dst_win.x = this->get_screen_center().x + (rect_dst_win.x - position.x) * zoom;
	rect_dst_win.y = this->get_screen_center().y + (rect_dst_win.y - position.y) * zoom;

	// 渲染大小 = 纹理大小 * 缩放因子
	rect_dst_win.w = rect_dst->w * zoom;
	rect_dst_win.h = rect_dst->h * zoom;

	SDL_RenderTextureRotated(Main_Renderer, texture, rect_src, &rect_dst_win, angle, &rotate_center,
		is_flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
}

// 渲染文字
void fce::Camera2D::render_text(const Point& pos, TTF_Font* font, 
	SDL_Color color, float ptsize, const std::string& info) const
{
	TTF_Text* text_win = TTF_CreateText(Main_TextEngine, font, info.c_str(), NULL);
	TTF_SetTextColor(text_win, color.r, color.g, color.b, color.a);

	Point screen_pos = this->world_to_screen(pos);
	TTF_SetFontSize(font, ptsize * zoom);

	TTF_DrawRendererText(text_win, screen_pos.x, screen_pos.y);
}

// 跟随角色
void fce::Camera2D::look_at(const Point& target_pos, int style)
{
	if (style & None) return;

	if (style & Camera2D::Static_Follow)	// 静态跟随
	{
		if (style & Camera2D::Only_X)
			this->base_position.x = target_pos.x;
		else if (style & Camera2D::Only_Y)
			this->base_position.y = target_pos.y;
		else
			this->base_position = target_pos;
	}
	else if (style & Camera2D::Smooth_Follow)		// 平滑跟随
	{
		if (style & Camera2D::Only_X)
			this->base_position.x = maths::lerp(base_position.x, target_pos.x, smooth_strength);
		else if (style & Camera2D::Only_Y)
			this->base_position.y = maths::lerp(base_position.y, target_pos.y, smooth_strength);
		else
		{
			this->base_position.x = maths::lerp(base_position.x, target_pos.x, smooth_strength);
			this->base_position.y = maths::lerp(base_position.y, target_pos.y, smooth_strength);
		}
	}
}

// 窗口坐标转世界坐标
fce::Point fce::Camera2D::screen_to_world(const Point& screen_pos) const
{
	// 世界坐标 = (窗口坐标 - 屏幕中心点) / 缩放因子 + 摄像机坐标
	float world_x = (screen_pos.x - this->get_screen_center().x) / zoom + position.x;
	float world_y = (screen_pos.y - this->get_screen_center().y) / zoom + position.y;
	return Point(world_x, world_y);
}

// 世界坐标转窗口坐标
fce::Point fce::Camera2D::world_to_screen(const Point& world_pos) const
{
	// 渲染坐标 = 屏幕中心点 + (世界坐标 - 摄像机坐标) * 缩放因子
	float screen_x = this->get_screen_center().x + (world_pos.x - position.x) * zoom;
	float screen_y = this->get_screen_center().y + (world_pos.y - position.y) * zoom;
	return Point(screen_x, screen_y);
}

// 更新摄像机
void fce::Camera2D::on_update(float delta)
{
	timer_shake.on_update(delta);
	smooth_strength = SMOOTH_FACTOR * delta;

	if (is_shaking)
	{
		shake_position.x = (-50 + std::rand() % 100) / 50.0f * shaking_strength;
		shake_position.y = (-50 + std::rand() % 100) / 50.0f * shaking_strength;
	}

	// 最终位置 = 基础位置 + 抖动位置
	position = base_position + shake_position;
}

// 获取屏幕中心点
fce::Point fce::Camera2D::get_screen_center() const
{
	int screen_w, screen_h;
	SDL_GetWindowSize(Main_Window, &screen_w, &screen_h);		// 计算屏幕宽高
	Point screen_center = Point(screen_w / 2.0f, screen_h / 2.0f);	// 获取屏幕中心点
	return screen_center;
}

// ======================================= ShapeMaker =======================================

// 用指定颜色填充屏幕
void fce::ShapeMaker::fill(SDL_Color color)
{
	SDL_SetRenderDrawColor(Main_Renderer, color.r, color.g, color.b, color.a);
	SDL_RenderClear(Main_Renderer);
}

// 绘制线条
void fce::ShapeMaker::render_line(const Camera2D& camera, const Point& begin, 
	const Point& end, SDL_Color color)
{
	Point begin_win = camera.world_to_screen(begin);
	Point end_win = camera.world_to_screen(end);

	SDL_SetRenderDrawColor(Main_Renderer, color.r, color.g, color.b, color.a);
	SDL_RenderLine(Main_Renderer, begin_win.x, begin_win.y, end_win.x, end_win.y);
}

// 绘制锚点为左上角的矩形
void fce::ShapeMaker::render_TopLeftRect(const Camera2D& camera, const Point& pos, 
	const Size& size, SDL_Color color, bool is_filled)
{
	// 设置属性
	Point pos_win = camera.world_to_screen(pos);
	Size size_win = { size.w * camera.get_zoom(), size.h * camera.get_zoom() };
	SDL_FRect rect = { pos_win.x, pos_win.y, size_win.w, size_win.h };

	// 绘制
	SDL_SetRenderDrawColor(Main_Renderer, color.r, color.g, color.b, color.a);
	if (is_filled)
		SDL_RenderFillRect(Main_Renderer, &rect);
	else
		SDL_RenderRect(Main_Renderer, &rect);
}

// 绘制锚点为中心的矩形
void fce::ShapeMaker::render_CenterRect(const Camera2D& camera, const Point& pos, 
	const Size& size, SDL_Color color, bool is_filled)
{
	// 设置属性
	Point pos_win = camera.world_to_screen(pos);
	Size size_win = { size.w * camera.get_zoom(), size.h * camera.get_zoom() };
	SDL_FRect rect = { pos_win.x - (size_win.w / 2.0f), pos_win.y - (size_win.h / 2.0f), size_win.w, size_win.h };

	// 绘制
	SDL_SetRenderDrawColor(Main_Renderer, color.r, color.g, color.b, color.a);
	if (is_filled)
		SDL_RenderFillRect(Main_Renderer, &rect);
	else
		SDL_RenderRect(Main_Renderer, &rect);
}

// 绘制锚点为左上角的圆形
void fce::ShapeMaker::render_TopLeftCircle(const Camera2D& camera, const Point& pos, 
	float radius, SDL_Color color, bool is_filled)
{
	Point pos_win = camera.world_to_screen(pos);
	float radius_win = radius * camera.get_zoom();
	pos_win.x = pos_win.x + radius_win;
	pos_win.y = pos_win.y + radius_win;

	SDL_SetRenderDrawColor(Main_Renderer, color.r, color.g, color.b, color.a);

	if (is_filled)
	{
		for (float y = -radius_win; y <= radius_win; y += 1.0f)
		{
			float x = sqrtf(radius_win * radius_win - y * y);
			SDL_RenderLine(Main_Renderer, pos_win.x - x, pos_win.y + y, pos_win.x + x, pos_win.y + y);
		}
	}
	else
	{
		float x = radius_win; float y = 0; float err = 0;
		while (x >= y)
		{
			SDL_RenderPoint(Main_Renderer, pos_win.x + x, pos_win.y + y);
			SDL_RenderPoint(Main_Renderer, pos_win.x + y, pos_win.y + x);
			SDL_RenderPoint(Main_Renderer, pos_win.x - y, pos_win.y + x);
			SDL_RenderPoint(Main_Renderer, pos_win.x - x, pos_win.y + y);
			SDL_RenderPoint(Main_Renderer, pos_win.x - x, pos_win.y - y);
			SDL_RenderPoint(Main_Renderer, pos_win.x - y, pos_win.y - x);
			SDL_RenderPoint(Main_Renderer, pos_win.x + y, pos_win.y - x);
			SDL_RenderPoint(Main_Renderer, pos_win.x + x, pos_win.y - y);

			if (err <= 0) { y += 1; err += 2 * y + 1; }
			if (err > 0) { x -= 1; err -= 2 * x + 1; }
		}
	}
}

// 绘制锚点为中心的圆形
void fce::ShapeMaker::render_CenterCircle(const Camera2D& camera, const Point& pos, 
	float radius, SDL_Color color, bool is_filled)
{
	Point pos_win = camera.world_to_screen(pos);
	float radius_win = radius * camera.get_zoom();

	SDL_SetRenderDrawColor(Main_Renderer, color.r, color.g, color.b, color.a);

	if (is_filled)
	{
		for (float y = -radius_win; y <= radius_win; y += 1.0f)
		{
			float x = sqrtf(radius_win * radius_win - y * y);
			SDL_RenderLine(Main_Renderer, pos_win.x - x, pos_win.y + y, pos_win.x + x, pos_win.y + y);
		}
	}
	else
	{
		float x = radius_win; float y = 0; float err = 0;
		while (x >= y)
		{
			SDL_RenderPoint(Main_Renderer, pos_win.x + x, pos_win.y + y);
			SDL_RenderPoint(Main_Renderer, pos_win.x + y, pos_win.y + x);
			SDL_RenderPoint(Main_Renderer, pos_win.x - y, pos_win.y + x);
			SDL_RenderPoint(Main_Renderer, pos_win.x - x, pos_win.y + y);
			SDL_RenderPoint(Main_Renderer, pos_win.x - x, pos_win.y - y);
			SDL_RenderPoint(Main_Renderer, pos_win.x - y, pos_win.y - x);
			SDL_RenderPoint(Main_Renderer, pos_win.x + y, pos_win.y - x);
			SDL_RenderPoint(Main_Renderer, pos_win.x + x, pos_win.y - y);

			if (err <= 0) { y += 1; err += 2 * y + 1; }
			if (err > 0) { x -= 1; err -= 2 * x + 1; }
		}
	}
}

// ======================================= Animation =======================================

fce::Animation::Animation()
{
	timer.set_one_shot(false);
	timer.set_wait_time(0.1f);	// 默认帧间隔0.1f
	timer.set_on_timeout([&]()
		{
			idx_frame++;
			if (idx_frame >= frame_list.size())
			{
				idx_frame = is_loop ? 0 : frame_list.size() - 1;
				if (!is_loop && on_finished)
					on_finished();
			}
		});
}

fce::Animation::Animation(SDL_Texture* texture, int num_h) : Animation()
{
	this->add_frame(texture, num_h);
}

fce::Animation::Animation(Atlas* atlas) : Animation()
{
	this->add_frame(atlas);
}

// 重置动画
void fce::Animation::reset()
{
	timer.restart(); 
	idx_frame = 0;
}

// 设置动画世界坐标
void fce::Animation::set_position(const Point& pos)
{
	this->position = pos;
}

// 设置动画方向
void fce::Animation::set_rotation(float angle)
{
	this->angle = static_cast<double>(angle);
}

// 设置动画是否循环（默认true）
void fce::Animation::set_loop(bool is_loop)
{
	this->is_loop = is_loop;
}

// 设置动画帧间隔（默认0.1）
void fce::Animation::set_interval(float interval)
{
	timer.set_wait_time(interval);
}

// 设置动画是否翻转（默认false）
void fce::Animation::set_flip(bool flag)
{
	this->is_flip = flag;
}

// 设置动画回调函数
void fce::Animation::set_on_finished(std::function<void()> callback)
{
	on_finished = callback;
}

// 更新动画
void fce::Animation::on_update(float delta)
{
	timer.on_update(delta);
}

// 设置动画中心点（默认为纹理中心点，参数范围在0.0~1.0）
void fce::Animation::set_center(float anchor_x, float anchor_y)
{
	this->center = { anchor_x, anchor_y };
}

// 添加序列帧
void fce::Animation::add_frame(Atlas* atlas)
{
	for (int i = 0; i < atlas->get_size(); i++)
	{
		SDL_Texture* texture = atlas->get_texture(i);

		float width, height;
		SDL_GetTextureSize(texture, &width, &height);
		SDL_FRect rect_src = { 0, 0, width, height };

		frame_list.emplace_back(texture, rect_src);
	}
}

// 添加序列帧
void fce::Animation::add_frame(SDL_Texture* texture, int num_h)
{
	float width, height;
	SDL_GetTextureSize(texture, &width, &height);	// 获取序列帧宽高

	float width_frame = width / num_h;	// 获取单张序列帧宽高
	for (int i = 0; i < num_h; i++)
	{
		SDL_FRect rect_src{};
		rect_src.x = i * width_frame, rect_src.y = 0;
		rect_src.w = width_frame, rect_src.h = height;

		frame_list.emplace_back(texture, rect_src);		// 将序列帧加入列表
	}
}


// 渲染动画
void fce::Animation::on_render(const Camera2D& camera) const
{
	const Frame& frame = frame_list[idx_frame];
	const Point& pos_camera = camera.get_position();

	SDL_FRect rect_dst = { position.x, position.y, frame.rect_src.w, frame.rect_src.h };
	camera.render_texture(frame.texture, &frame.rect_src, &rect_dst, angle, center, is_flip);
}

// ======================================= AnimationPlayer =======================================


fce::AnimationPlayer::~AnimationPlayer()
{
	for (auto& anim : animation_pool)
		delete anim.second;		// 释放动画资源

	animation_pool.clear();
	current_animation = nullptr;
}

// 注册动画
void fce::AnimationPlayer::register_animation(const std::string& name, Animation* anim)
{
	if (animation_pool.find(name) != animation_pool.end())
		throw custom_error(error_title, u8"Animation “" + name + u8"” is already exist!");

	animation_pool[name] = anim;
}

// 切换动画
void fce::AnimationPlayer::switch_to(const std::string& name)
{
	if (animation_pool.find(name) == animation_pool.end())
		throw custom_error(error_title, u8"Animation “" + name + u8"” is not exist!");

	this->current_animation = animation_pool[name];
}

// 设置当前动画，并从头播放
void fce::AnimationPlayer::set_animation(const std::string& name)
{
	if (animation_pool.find(name) == animation_pool.end())
		throw custom_error(error_title, u8"Animation “" + name + u8"” is not exist!");

	this->current_animation = animation_pool[name];
	this->current_animation->reset();
}

// 获取当前动画
fce::Animation* fce::AnimationPlayer::get_current()
{
	if (current_animation)
		return current_animation;
	else
		throw custom_error(error_title, u8"“current_animation” is a nullptr!");
}

// 更新动画
void fce::AnimationPlayer::on_update(float delta)
{
	if (current_animation)
		current_animation->on_update(delta);
	else
		throw custom_error(error_title, u8"“current_animation” is a nullptr!");
}

// 渲染动画
void fce::AnimationPlayer::on_render(const Camera2D& camera)
{
	if (current_animation)
		current_animation->on_render(camera);
	else
		throw custom_error(error_title, u8"“current_animation” is a nullptr!");
}

// ======================================= StateMachine =======================================

fce::StateMachine::~StateMachine()
{
	for (auto& state : state_pool)
		delete state.second;	// 释放状态池内所有状态节点

	state_pool.clear();
	current_state = nullptr;
}

// 更新状态机
void fce::StateMachine::on_update(float delta)
{
	if (!current_state)
	{
		std::string info = u8"Current state is nullptr! Please check if the status node is initialized";
		throw custom_error(u8"StateMachine Error", info.c_str());
	}

	if (need_init)	// 如果需要初始化状态机
	{
		current_state->on_enter();
		need_init = false;		// 初始化完成
	}

	current_state->on_update(delta);
}

// 设置初状态节点（用于初始化）
void fce::StateMachine::set_entry(const std::string& name)
{
	// 如果未找到目标状态
	if (state_pool.find(name) == state_pool.end())
	{
		std::string info = u8"State “" + name + u8"” is not found!";
		throw custom_error(u8"StateMachine Error", info);
	}

	current_state = state_pool[name];
}

// 切换状态节点
void fce::StateMachine::switch_to(const std::string& name)
{
	// 如果未找到目标状态
	if (state_pool.find(name) == state_pool.end())
	{
		std::string info = u8"State “" + name + u8"” is not found!";
		throw custom_error(u8"StateMachine Error", info);
	}

	if (current_state) current_state->on_exit();
	current_state = state_pool[name];
	current_state->on_enter();
}

// 注册状态节点
void fce::StateMachine::register_state(const std::string& name, StateNode* state)
{
	// 如果该状态已存在
	if (state_pool.find(name) != state_pool.end())
	{
		std::string info = u8"State “" + name + u8"” is already exist!";
		throw custom_error(u8"StateMachine Error", info);
	}

	state_pool[name] = state;		// 注册状态节点
}
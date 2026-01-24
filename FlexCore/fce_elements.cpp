#include "fce_elements.h"

// ======================================= Sprite =======================================

// 面向指定坐标点
void fce::Sprite::point_torwards(const Point& target)
{
	float dx = target.x - position.x;
	float dy = target.y - position.y;
	this->direction = maths::rad_to_deg(std::atan2f(dy, dx));
}

// 面向鼠标指针
void fce::Sprite::point_mousePointer(const Camera2D& camera)
{
	float mouse_x, mouse_y;
	SDL_GetMouseState(&mouse_x, &mouse_y);
	auto mouse_world_pos = camera.screen_to_world(Point(mouse_x, mouse_y));

	float dx = mouse_world_pos.x - position.x;
	float dy = mouse_world_pos.y - position.y;

	// 旋转方向 = 弧度转角度(atan2(目标坐标 - 自身坐标))
	this->direction = maths::rad_to_deg(std::atan2f(dy, dx));
}

// ======================================= Label =======================================

fce::Label::Label(const Point& position, TTF_Font* font, SDL_Color color, float size, const std::string& info)
	: label_color(color), label_font(font), ptsize(size), label_info(info)
{
	this->position = position;
	this->render_layer = RenderLayer::Label;
}

// 设置标签字体
void fce::Label::on_render(const Camera2D& camera)
{
	camera.render_text(position, label_font, label_color, ptsize, label_info.c_str());
}

// ======================================= Button =======================================

fce::Button::Button(const Point& pos, const Size& size)
{
	this->position = pos;
	this->size = size;
	this->render_layer = RenderLayer::UI;
	this->effects["normal"] = std::make_pair(nullptr, nullptr);
	this->effects["hover"] = std::make_pair(nullptr, nullptr);
	this->effects["click"] = std::make_pair(nullptr, nullptr);
}

// 设置按钮纹理（分别为：正常、悬停、被点击）
void fce::Button::set_texture(SDL_Texture* normal, SDL_Texture* hover, SDL_Texture* click)
{
	// 检查纹理是否为nullptr
	if (!normal || !hover || !click)
		throw custom_error(u8"Button Argument Error", u8"Argument texture cannot be nullptr!");

	effects["normal"].first = normal;
	effects["hover"].first = hover;
	effects["click"].first = click;
	this->current_texture = effects["normal"].first;	// 默认显示正常状态纹理
}

// 设置按钮音效（分别为：悬停、被点击）
void fce::Button::set_audio(Mix_Chunk* hover, Mix_Chunk* click)
{
	effects["hover"].second = hover;
	effects["click"].second = click;
}

// 渲染按钮
void fce::Button::on_render(const Camera2D& camera)
{
	if (!current_texture) return;	// 如果没有设置纹理则不渲染
	SDL_FRect rect_dst_win = { position.x, position.y, size.w, size.h };
	camera.render_texture(current_texture, nullptr, &rect_dst_win, 0);
}

// 处理输入事件
void fce::Button::on_input(const SDL_Event& event)

{
	// 判断鼠标是否在按钮上悬停
	bool in_range_x = event.motion.x >= position.x && event.motion.x <= position.x + size.w;
	bool in_range_y = event.motion.y >= position.y && event.motion.y <= position.y + size.h;

	if (event.type == SDL_EVENT_MOUSE_MOTION)	// 鼠标悬停在按钮上
	{
		if (in_range_x && in_range_y)
		{
			if (effects["hover"].second && !is_first_hover)
				Mix_PlayChannel(-1, effects["hover"].second, 0);

			this->is_first_hover = true;
			this->current_texture = effects["hover"].first;
			SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER));
		}
		else
		{
			this->is_first_hover = false;
			this->current_texture = effects["normal"].first;
			SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT));
		}
	}
	if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT) // 按钮被点击
	{
		if (in_range_x && in_range_y)
		{
			if (effects["click"].second)
				Mix_PlayChannel(-1, effects["click"].second, 0);

			this->current_texture = effects["click"].first;
			if (on_click) this->on_click();
		}
	}
	if (event.type == SDL_EVENT_MOUSE_BUTTON_UP && event.button.button == SDL_BUTTON_LEFT) // 按钮被释放
	{
		if (in_range_x && in_range_y)
			this->current_texture = effects["hover"].first;
		else
			this->current_texture = effects["normal"].first;
	}
}

// ======================================= Scene =======================================

fce::Scene::~Scene()
{
	this->destroy_all_sprites();
}

// 注册新精灵
void fce::Scene::register_sprite(const std::string& name, Sprite* new_sprite)
{
	sprites_pool.emplace(name, new_sprite);		// 加入精灵池

	RenderLayer layer_target = new_sprite->get_render_layer();
	if (layer_target == RenderLayer::None)
		throw custom_error(u8"SceneManager Error", u8"sprite “" + name + u8"” render layer no setting!");
	this->layered_sprite_pool[layer_target].push_back(new_sprite); // 加入分层渲染池

	needs_sorting = true;	// 标记需要排序
}

// 销毁目标精灵
void fce::Scene::destroy_sprite(Sprite* sprite)
{
	// 依次遍历直到找到目标精灵
	for (auto iterator = sprites_pool.begin(); iterator != sprites_pool.end(); iterator++)
	{
		// 如果找到了并且存在目标精灵
		if (iterator->second == sprite && iterator->second)
		{
			this->sprites_pool.erase(iterator);
			delete sprite; return;
		}
	}
}

// 获取精灵组（返回一个范围迭代器）
auto fce::Scene::find_group(const std::string& name)
{
	auto range = sprites_pool.equal_range(name);
	if (range.first == range.second)	// 检测是否找到
		throw custom_error(u8"SceneManager Error", u8"Sprites group “" + name + u8"” is not found!");
	return range;
}

// 更新所有精灵
void fce::Scene::sprites_update(float delta)
{
	for (auto& [name, sprite] : sprites_pool)
		sprite->on_update(delta);
	needs_sorting = true;
}

// 渲染所有精灵
void fce::Scene::sprites_render(const Camera2D& game, const Camera2D& ui)
{
	// 如果需要排序则进行排序
	if (needs_sorting && is_enabled_y_sort)
	{
		for (auto& [layer, sprites] : this->layered_sprite_pool)
		{
			if (this->layer_need_y_sort(layer))
			{
				// 按Y轴从小到大排序
				std::sort(sprites.begin(), sprites.end(),
					[](const Sprite* a, const Sprite* b) { return a->get_position().y < b->get_position().y; });
			}
		}
	}
	needs_sorting = false;

	// 按渲染层顺序渲染
	this->render_layer(RenderLayer::Background, game, ui);
	this->render_layer(RenderLayer::Frontground, game, ui);
	this->render_layer(RenderLayer::Label, game, ui);
	this->render_layer(RenderLayer::GameObject, game, ui);
	this->render_layer(RenderLayer::Effect, game, ui);
	this->render_layer(RenderLayer::UI, game, ui);
}

// 处理所有精灵的输入事件
void fce::Scene::sprites_input(const SDL_Event& event)
{
	for (auto& [name, sprite] : sprites_pool)
		sprite->on_input(event);
}

// 销毁所有精灵
void fce::Scene::destroy_all_sprites()
{
	// 为空则直接返回
	if (sprites_pool.empty()) return;

	// 释放精灵资源
	for (auto& [name, sprite] : sprites_pool)
		delete sprite;

	sprites_pool.clear();
	layered_sprite_pool.clear();
}

// 渲染指定渲染层的所有精灵
void fce::Scene::render_layer(RenderLayer layer, const Camera2D& game, const Camera2D& ui)
{
	// 如果该渲染层不存在则直接返回
	if (layered_sprite_pool.find(layer) == layered_sprite_pool.end())
		return;

	// 选择使用的摄像机
	const Camera2D& used_camera = (layer == RenderLayer::UI) ? ui : game;
	for (Sprite* sprite : layered_sprite_pool[layer])
	{
		int screen_w, screen_h;
		SDL_GetWindowSize(Main_Window, &screen_w, &screen_h);	// 获取窗口宽高
		bool is_out_of_view_x = (used_camera.world_to_screen(sprite->get_position()).x < -sprite->get_size().w || used_camera.world_to_screen(sprite->get_position()).x > screen_w + sprite->get_size().w);
		bool is_out_of_view_y = (used_camera.world_to_screen(sprite->get_position()).y < -sprite->get_size().h || used_camera.world_to_screen(sprite->get_position()).y > screen_h + sprite->get_size().h);

		// 如果精灵在摄像机外则跳过渲染
		if (is_out_of_view_x || is_out_of_view_y)
			continue;

		sprite->on_render(used_camera);
	}
}

// 设置是否启用Y轴排序(默认是true)
bool fce::Scene::layer_need_y_sort(RenderLayer layer) const
{
	return (layer == RenderLayer::GameObject || layer == RenderLayer::Label
		|| layer == RenderLayer::UI);
}

// 获取精灵（返回指定类型模板的对象指针）
template<typename _CvtTy>
_CvtTy* fce::Scene::find_sprite(const std::string& name)
{
	auto range = sprites_pool.equal_range(name);
	auto find_it = sprites_pool.find(name);

	if (find_it == sprites_pool.end())	// 检测是否找到
		throw custom_error(u8"SceneManager Error", u8"Sprite “" + name + u8"” is not found!");
	else if (std::distance(range.first, range.second) > 1)	// 检测唯一性
		throw custom_error(u8"SceneManager Error", u8"“" + name + u8"” is unclear, Please check name's singleness");

	return (_CvtTy*)find_it->second;
}
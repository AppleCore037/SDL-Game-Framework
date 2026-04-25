module;

#include <functional>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <algorithm>

#include <SDL3/SDL.h>

export module FlexCore:FCE_Component;

import :FCE_BaseSetup;
import :FCE_BaseType;
import :FCE_Utils;
import :FCE_Graphic;
import :FCE_UI;

// 计时器
export class Timer
{
public:
	Timer() = default;
	~Timer() = default;

	// 重启计时器
	void restart() { pass_time = 0; shotted = false; }

	// 设置等待时间
	void set_wait_time(float val) { this->wait_time = val; }

	// 设置是否单次触发（默认false）
	void set_one_shot(bool flag) { this->one_shot = flag; }

	// 设置回调函数
	void set_on_timeout(std::function<void()> callback) { on_timeout = callback; }

	// 暂停
	void pause() { paused = true; }

	// 继续
	void resume() { paused = false; }

	// 更新计时器
	void on_update(float delta) 
	{
		if (paused) return;

		pass_time += delta;
		if (pass_time >= wait_time)
		{
			bool _Can_shot = (!one_shot || (one_shot && !shotted));
			shotted = true;

			if (_Can_shot && on_timeout) this->on_timeout();
			pass_time -= wait_time;
		}
	}

private:
	float pass_time = 0;				// 经过时间
	float wait_time = 0;				// 等待时间
	bool paused = false;				// 是否暂停
	bool shotted = false;				// 是否已经触发
	bool one_shot = false;				// 是否单次触发
	std::function<void()> on_timeout;	// 回调函数
};

// 碰撞箱
export class CollisionManager; // 前向声明
export class CollisionBox
{
	friend class CollisionManager;
public:
	// 设置是否启用碰撞（默认true）
	void set_enabled(bool flag) { this->enabled = flag; }

	// 设置自身碰撞层
	void set_layer_src(CollisionLayer layer) { this->layer_src = layer; }

	// 设置目标碰撞层
	void set_layer_dst(CollisionLayer layer) { this->layer_dst = layer; }

	// 设置碰撞回调函数
	void set_on_collide(std::function<void(CollisionLayer)> callback) { this->on_collide = callback; }

	// 设置碰撞箱大小
	void set_size(const Size& size) { this->size = size; }

	// 设置碰撞箱坐标
	void set_position(const Vector2& pos) { this->position = pos; }

	// 获取碰撞箱大小
	const Size& get_size() const { return this->size; }

	// 获取碰撞箱坐标
	const Vector2& get_position() const { return this->position; }

private:
	Size size = { 0, 0 };	// 碰撞箱大小
	Vector2 position;		// 碰撞箱坐标
	bool enabled = true;	// 是否启用碰撞检测
	std::function<void(CollisionLayer)> on_collide;		// 碰撞回调函数
	CollisionLayer layer_src = CollisionLayer::None;	// 自身碰撞层
	CollisionLayer layer_dst = CollisionLayer::None;	// 目标碰撞层

private:
	CollisionBox() = default;
	~CollisionBox() = default;
};

// 动画
export class Animation
{
public:
	Animation()
	{
		timer.set_wait_time(0.1f);
		timer.set_on_timeout([&]() -> void
			{
				idx_frame++;
				if (idx_frame >= frame_list.size())
				{
					idx_frame = (is_loop ? 0 : frame_list.size() - 1);
					if (!is_loop && on_finished)
						on_finished();
				}
			});
	}

	Animation(SDL_Texture* texture, int num_h) : Animation() { this->add_frame(texture, num_h); }

	Animation(Atlas* atlas) : Animation() { this->add_frame(atlas); }

	// 重置动画
	void reset() 
	{ 
		this->timer.restart(); 
		this->idx_frame = 0; 
	}

	// 设置矩形
	void set_rect(const Rect& rect) { this->rect = rect; }

	// 设置循环(默认为false)
	void set_loop(bool flag) { is_loop = flag; }

	// 设置动画间隔(默认0.1f)
	void set_interval(float dur) { timer.set_wait_time(dur); }

	// 设置回调函数
	void set_on_finished(std::function<void()> callback) { on_finished = callback; }

	// 从长图中添加序列帧
	void add_frame(SDL_Texture* texture, int num_h)
	{
		float _width, _height;
		SDL_GetTextureSize(texture, &_width, &_height);	// 获取序列帧宽高

		float _width_frame = _width / num_h;	// 获取单张序列帧宽高
		for (int i = 0; i < num_h; i++)
		{
			SDL_FRect _rect_src{};
			_rect_src.x = i * _width_frame, _rect_src.y = 0;
			_rect_src.w = _width_frame, _rect_src.h = _height;

			frame_list.emplace_back(texture, _rect_src); // 将序列帧加入列表
		}
	}

	// 从图集中添加序列帧
	void add_frame(Atlas* atlas)
	{
		for (int i = 0; i < atlas->get_size(); i++)
		{
			SDL_Texture* _texture = atlas->get_texture(i);

			float _width, _height;
			SDL_GetTextureSize(_texture, &_width, &_height);
			SDL_FRect _rect_src = { 0, 0, _width, _height };

			frame_list.emplace_back(_texture, _rect_src);
		}
	}

	// 更新
	void on_update(float delta) { timer.on_update(delta); }

	// 渲染
	void on_render(const Camera& camera)
	{
		const Frame& _frame = frame_list[idx_frame];
		Renderer::render_texture(camera, _frame.texture, &_frame.rect_src, &rect.get_SDLRect(), 
			rect.direction, rect.anchor, rect.is_flip);
	}

private:
	// 序列帧结构体
	struct Frame
	{
		SDL_FRect rect_src;
		SDL_Texture* texture = nullptr;

		Frame() = default;
		Frame(SDL_Texture* texture, const SDL_FRect& rect_src)
			: texture(texture), rect_src(rect_src) {}

		~Frame() = default;
	};

private:
	Rect rect;							// 变换矩形
	Timer timer;						// 帧计时器
	bool is_loop = true;				// 是否循环
	size_t idx_frame = 0;				// 当前帧索引
	std::vector<Frame> frame_list;		// 帧列表
	std::function<void()> on_finished;	// 播放结束回调
};

// 动画播放器
export class AnimationPlayer
{
public:
	AnimationPlayer() = default;

	~AnimationPlayer()
	{
		if (animation_pool.empty())
			return;

		// 释放动画资源
		for (auto& anim_pair : animation_pool)
			delete anim_pair.second;

		animation_pool.clear();
		current_animation = nullptr;
	}

	// 注册动画
	void register_animation(const std::string& name, Animation* anim)
	{
		if (animation_pool.find(name) != animation_pool.end())	// 检查动画已存在
		{
			std::u8string _Info = u8"Animation “" + std::u8string(name.begin(), name.end()) + u8"” is already exist!";
			throw custom_error(u8"Animation Player Error", _Info.c_str());
		}
		animation_pool[name] = anim;
	}

	// 切换动画
	void switch_to(const std::string& name)
	{
		if (animation_pool.find(name) == animation_pool.end())
		{
			std::u8string _Info = u8"Animation “" + std::u8string(name.begin(), name.end()) + u8"” is not exist!";
			throw custom_error(u8"Animation Player Error", _Info.c_str());
		}
		this->current_animation = animation_pool[name];
	}

	// 设置当前动画，并从头播放
	void set_animation(const std::string& name)
	{
		if (animation_pool.find(name) == animation_pool.end())
		{
			std::u8string _Info = u8"Animation “" + std::u8string(name.begin(), name.end()) + u8"” is not exist!";
			throw custom_error(u8"Animation Player Error", _Info.c_str());
		}

		this->current_animation = animation_pool[name];
		this->current_animation->reset();
	}

	// 获取当前动画
	Animation* get_current()
	{
		if (!current_animation)
			throw custom_error(u8"Animation Player Error", u8"Variable “current_animation” is a nullptr!");
		return current_animation;
	}

	// 更新动画
	void on_update(float delta)
	{
		if (!current_animation)
			throw custom_error(u8"Animation Player Error", u8"Variable “current_animation” is a nullptr!");
		current_animation->on_update(delta);
	}

	// 渲染动画
	void on_render(const Camera& camera)
	{
		if (!current_animation)
			throw custom_error(u8"Animation Player Error", u8"Variable “current_animation” is a nullptr!");
		current_animation->on_render(camera);
	}

	// 清空动画池
	void clear_animation()
	{
		if (animation_pool.empty())
			return;

		// 释放动画资源
		for (auto& anim_pair : animation_pool)
			delete anim_pair.second;

		animation_pool.clear();
		current_animation = nullptr;
	}

private:
	std::unordered_map<std::string, Animation*> animation_pool;		// 动画池
	Animation* current_animation = nullptr;							// 当前动画
};

// 状态机
export class StateMachine
{
public:
	StateMachine() = default;

	~StateMachine()
	{
		if (state_pool.empty())
			return;

		// 释放状态池内所有状态节点
		for (auto& state_pair : state_pool)
			delete state_pair.second;

		state_pool.clear();
		current_state = nullptr;
	}

	// 更新状态机
	void on_update(float delta)
	{
		if (!current_state)
		{
			std::u8string _Info = u8"Variable “current_state” is nullptr! Please check the initialization";
			throw custom_error(u8"State Machine Error", _Info.c_str());
		}

		if (need_init)	// 如果需要初始化状态机
		{
			current_state->on_enter();
			need_init = false;		// 初始化完成
		}

		current_state->on_update(delta);
	}

	// 设置初状态节点（用于初始化）
	void set_entry(const std::string& name)
	{
		// 如果未找到目标状态
		if (state_pool.find(name) == state_pool.end())
		{
			std::u8string _Info = u8"State node “" + std::u8string(name.begin(), name.end()) + u8"” is not found!";
			throw custom_error(u8"State Machine Error", _Info.c_str());
		}
		current_state = state_pool[name];
	}

	// 切换状态节点
	void switch_to(const std::string& name)
	{
		// 如果未找到目标状态
		if (state_pool.find(name) == state_pool.end())
		{
			std::u8string _Info = u8"State node “" + std::u8string(name.begin(), name.end()) + u8"” is not found!";
			throw custom_error(u8"State Machine Error", _Info.c_str());
		}

		if (current_state) current_state->on_exit();	// 退出
		current_state = state_pool[name];	// 切换
		current_state->on_enter();	// 进入
	}

	// 注册状态节点
	void register_state(const std::string& name, StateNode* state)
	{
		if (state_pool.find(name) != state_pool.end())	// 如果该状态已存在
		{
			std::u8string _Info = u8"State node “" + std::u8string(name.begin(), name.end()) + u8"” is already exist!";
			throw custom_error(u8"State Machine Error", _Info.c_str());
		}
		state_pool[name] = state;		// 注册状态节点
	}

	// 清空状态池
	void clear_state_node()
	{
		if (state_pool.empty())
			return;

		// 释放状态池内所有状态节点
		for (auto& state_pair : state_pool)
			delete state_pair.second;

		state_pool.clear();
		current_state = nullptr;
	}

private:
	bool need_init = true;	// 是否需要初始化状态机
	StateNode* current_state = nullptr;		// 当前状态节点
	std::unordered_map<std::string, StateNode*> state_pool;	// 状态池
};

// 精灵
export class Sprite
{
public:
	Sprite() = default;
	virtual ~Sprite() = default;

	virtual void on_update(float delta) = 0;			// 更新
	virtual void on_render(const Camera& camera) = 0;	// 渲染
	virtual void on_input(const SDL_Event& event) = 0;	// 输入
	virtual void reset_props() = 0;						// 重置属性
	
	Rect& get_rect() { return this->rect; }
	RenderLayer get_layer() const { return this->render_layer; }
	const std::string& get_tag() const { return this->tag; }
	const std::string& get_group_tag() const { return group_tag; }

	void set_layer(RenderLayer layer) { this->render_layer = layer; }
	void set_tag(const std::string& name) { this->tag = name; }
	void set_group_tag(const std::string& name) { this->group_tag = name; }

protected:
	Rect rect;	// 变换矩形
	RenderLayer render_layer = RenderLayer::None;	// 渲染层
	std::string tag = "";		 // 标签

private:
	std::string group_tag = "";	 // 所属组标签
};

// 精灵组
class Scene;
export class SpriteGroup
{
	friend class Scene;
public:
	// 返回组
	const std::vector<Sprite*>& range() { return sprite_list; }

private:
	SpriteGroup() = default;
	~SpriteGroup() = default;

	// 添加精灵
	void add_sprite(Sprite* sprite) 
	{
		sprite_list.push_back(sprite);
		pos_index[sprite] = sprite_list.size() - 1;
	}

	// 删除精灵
	void erase_sprite(Sprite* sprite)
	{
		pos_index[sprite_list[sprite_list.size() - 1]] = pos_index[sprite];
		std::swap(sprite_list[pos_index[sprite]], sprite_list[sprite_list.size() - 1]);
		sprite_list.pop_back();
		pos_index.erase(sprite);
	}

private:
	std::vector<Sprite*> sprite_list;	// 精灵列表
	std::unordered_map<Sprite*, size_t> pos_index;	// 位置索引，辅助增删
};

// 场景
export class Scene
{
public:
	Scene() = default;

	virtual ~Scene() 
	{ 
		// 释放精灵
		this->clear_sprite();

		// 释放UI
		for (UI* ui : ui_list) delete ui;
		ui_list.clear();
	}

	// 进入场景回调（用于属性的初始化）
	virtual void on_enter()
	{
		// 每当场景进入的时候调用重置属性
		for (Sprite* sprite : sprite_list)
			sprite->reset_props();
	}

	// 更新场景
	virtual void on_update(float delta)
	{
		// 对精灵列表排序
		std::sort(sprite_list.begin(), sprite_list.end(), [](Sprite* a, Sprite* b)
			{
				// 让精灵先按照渲染层，再按照Y轴排序
				if (a->get_layer() != b->get_layer())
					return a->get_layer() < b->get_layer();
				else
					return a->get_rect().position.y < b->get_rect().position.y;
			});
		this->rebuild_pos_index();

		// 更新精灵
		for (Sprite* sprite : sprite_list)
			sprite->on_update(delta);

		// 更新UI元素
		for (UI* ui_element : ui_list)
			ui_element->on_update(delta);
	}

	// 渲染画面
	virtual void on_render(const Camera& cam_game, const Camera& cam_ui)
	{
		// 渲染精灵
		for (Sprite* sprite : sprite_list)
		{
			const Rect& _rect = sprite->get_rect();
			if (sprite->get_layer() == RenderLayer::UI)
				sprite->on_render(cam_ui);
			else if (cam_game.target_in_view(_rect.position, _rect.size))
				sprite->on_render(cam_game);
		}

		// 渲染UI
		for (UI* ui : ui_list)
			ui->on_render(cam_ui);
	}

	// 处理输入
	virtual void on_input(const SDL_Event& event)
	{
		// 精灵事件响应
		for (Sprite* sprite : sprite_list)
			sprite->on_input(event);

		// UI事件响应
		for (UI* ui_element : ui_list)
			ui_element->on_input(event);
	}

	// 退出场景回调（用于某些属性的释放）
	virtual void on_exit() {}

	// 查找精灵
	Sprite* find_sprite(const std::string& name)
	{
		// 判断是否存在
		if (sprite_registry.find(name) == sprite_registry.end())
		{
			std::u8string _info = u8"Func <find_sprite()>: Sprite: " + std::u8string((const char8_t*)name.c_str()) + u8" is not found!";
			throw custom_error(u8"Scene Tree Error", _info.c_str());
		}
		return sprite_registry[name];
	}

	// 查找精灵组
	SpriteGroup* find_group(const std::string& name)
	{
		if (sprite_group.find(name) == sprite_group.end())
		{
			std::u8string _info = u8"Func <find_group()>: Sprite Group: " + std::u8string((const char8_t*)name.c_str()) + u8" is not found!";
			throw custom_error(u8"Scene Tree Error", _info.c_str());
		}
		return sprite_group[name];
	}

	// 注册精灵
	void register_sprite(Sprite* sprite, const std::string& group_name = "")
	{
		if (sprite->get_tag() != "") // 提供了唯一标签
		{
			// 验证唯一性
			if (sprite_registry.find(sprite->get_tag()) != sprite_registry.end())
			{
				std::u8string _info = u8"Func <register_sprite()>: Sprite: " + std::u8string((const char8_t*)sprite->get_tag().c_str()) + u8"is already exist!";
				throw custom_error(u8"Scene Tree Error", _info.c_str());
			}
			sprite_registry[sprite->get_tag()] = sprite;
		}
		if (group_name != "") // 提供了group_name
		{
			sprite->set_group_tag(group_name);

			// 将精灵添加到组里，并校验组的存在
			auto& _group_ptr = sprite_group[group_name];
			if (!_group_ptr) _group_ptr = new SpriteGroup();
			_group_ptr->add_sprite(sprite);
		}

		sprite_list.push_back(sprite);
		pos_index[sprite] = sprite_list.size() - 1;
	}

	// 移除精灵
	void remove_sprite(Sprite* sprite)
	{
		if (!sprite) return;

		// 未注册，则索引无效
		if (pos_index.find(sprite) == pos_index.end())
			throw custom_error(u8"Scene Tree Error", u8"Func <remove_sprite()>: sprite not registered!");

		// 从sprite_list中删除
		pos_index[sprite_list[sprite_list.size() - 1]] = pos_index[sprite];
		std::swap(sprite_list[pos_index[sprite]], sprite_list[sprite_list.size() - 1]);
		sprite_list.pop_back();
		pos_index.erase(sprite);

		if (sprite->get_tag() != "")		// 从注册表中删除
			sprite_registry.erase(sprite->get_tag());
		if (sprite->get_group_tag() != "")	// 从组中删除
			sprite_group[sprite->get_group_tag()]->erase_sprite(sprite);

		delete sprite;
	}

	// 清空精灵列表
	void clear_sprite()
	{
		for (Sprite* sprite : sprite_list)
			delete sprite;
		sprite_list.clear();

		pos_index.clear();
		sprite_registry.clear();

		// 如果组不为空，删除组
		if (!sprite_group.empty())
		{
			for (auto& [i, group] : sprite_group)
				delete group;
		}
		sprite_group.clear();
	}

protected:
	// 添加UI元素
	void add_ui_element(UI* element) { ui_list.push_back(element); }

	// 移除UI元素
	void remove_ui_element(UI* element)
	{
		ui_list.erase(std::remove(ui_list.begin(), ui_list.end(), element), ui_list.end());
		delete element;
	}

private:
	// 重建位置索引
	void rebuild_pos_index()
	{
		for (int i = 0; i < sprite_list.size(); i++)
			pos_index[sprite_list[i]] = i;
	}

private:
	std::vector<UI*> ui_list;	// UI列表

	std::vector<Sprite*> sprite_list;				// 精灵列表
	std::unordered_map<Sprite*, size_t> pos_index;  // 位置索引 (辅助sprite_list增删)

	std::unordered_map<std::string, Sprite*> sprite_registry;  // 精灵注册表
	std::unordered_map<std::string, SpriteGroup*> sprite_group; // 精灵组
};
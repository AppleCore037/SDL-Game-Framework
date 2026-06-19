module;

#include <array>
#include <functional>
#include <string>
#include <unordered_map>

#include <SDL3/SDL.h>

export module FlexCore:FCE_Component;

import :FCE_BaseSetup;
import :FCE_BaseType;
import :FCE_Utils;
import :FCE_Graphic;

export namespace fce
{
	// 计时器
	class Timer
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
				bool _can_shot = (!one_shot || (one_shot && !shotted));
				shotted = true;

				if (_can_shot && on_timeout) this->on_timeout();
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
	class CollisionManager; // 前向声明
	class CollisionBox
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
		void set_on_collide(std::function<void(const CollisionInfo&)> func) { this->on_collide = func; }

		// 设置发送属性回调函数
		void set_send_props(std::function<void(CollisionInfo&)> func) { this->send_props = func; }

		// 设置碰撞箱坐标
		void set_position(const Vector2& pos) { this->position = pos; }

		// 设置碰撞箱大小
		void set_size(const Size& size) { this->size = size; }

		// 设置碰撞箱方向
		void set_direction(float dir) { this->direction = dir; }

		// 获取碰撞箱大小
		const Size& get_size() const { return this->size; }

		// 获取碰撞箱坐标
		const Vector2& get_position() const { return this->position; }

		// 获取碰撞箱方向
		const float get_direction() const { return this->direction; }

	private:
		Size size = { 0, 0 };	// 碰撞箱大小
		Vector2 position;		// 碰撞箱坐标
		float direction;		// 碰撞箱方向

		bool enabled = true;	// 是否启用碰撞检测
		CollisionLayer layer_src = CollisionLayer::None;	// 自身碰撞层
		CollisionLayer layer_dst = CollisionLayer::None;	// 目标碰撞层

		std::function<void(const CollisionInfo&)> on_collide;	// 碰撞回调函数
		std::function<void(CollisionInfo&)> send_props;			// 发送属性信息
		std::array<Vector2, 4> corners;		// 矩形碰撞箱四个顶点

	private:
		CollisionBox() = default;
		~CollisionBox() = default;

		// 计算碰撞箱的四个顶点坐标
		void set_corners()
		{
			float half_w = size.w / 2.0f, half_h = size.h / 2.0f;
			float radian = maths::deg_to_rad(direction);

			this->corners[0] = position + Vector2(-half_w, -half_h).rotate(radian);
			this->corners[1] = position + Vector2(half_w, -half_h).rotate(radian);
			this->corners[2] = position + Vector2(half_w, half_h).rotate(radian);
			this->corners[3] = position + Vector2(-half_w, half_h).rotate(radian);
		}
	};

	// 动画
	class Animation
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
				: texture(texture), rect_src(rect_src) {
			}

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
	class AnimationPlayer
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

		// 添加动画
		void add_animation(const std::string& name, Animation* anim)
		{
			if (animation_pool.find(name) != animation_pool.end())	// 检查动画已存在
			{
				std::string _info = "[add_animation()]: Animation \"" + name + "\"is already exist!";
				throw custom_error("Animation Player Error", _info.c_str());
			}
			animation_pool[name] = anim;
		}

		// 切换动画
		void switch_to(const std::string& name)
		{
			if (animation_pool.find(name) == animation_pool.end())
			{
				std::string _info = "[switch_to()]: Animation \"" + name + "\" is not exist!";
				throw custom_error("Animation Player Error", _info.c_str());
			}
			this->current_animation = animation_pool[name];
		}

		// 设置当前动画，并从头播放
		void set_animation(const std::string& name)
		{
			if (animation_pool.find(name) == animation_pool.end())
			{
				std::string _info = "[set_animation()]: Animation \"" + name + "\" is not exist!";
				throw custom_error("Animation Player Error", _info.c_str());
			}

			this->current_animation = animation_pool[name];
			this->current_animation->reset();
		}

		// 获取当前动画
		Animation* get_current()
		{
			if (!current_animation)
				throw custom_error("Animation Player Error", "[get_current()]: \"current_animation\" is a nullptr!");
			return current_animation;
		}

		// 更新动画
		void on_update(float delta)
		{
			if (!current_animation)
				throw custom_error("Animation Player Error", "[on_update()]: \"current_animation\" is a nullptr!");
			current_animation->on_update(delta);
		}

		// 渲染动画
		void on_render(const Camera& camera)
		{
			if (!current_animation)
				throw custom_error("Animation Player Error", "[on_render()]: \"current_animation\" is a nullptr!");
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

	// 状态机
	class StateMachine
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
				std::string _info = "[on_update()]: \"current_state\" is nullptr! Please check the initialization";
				throw custom_error("State Machine Error", _info.c_str());
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
				std::string _info = "[set_entry()]: State node \"" + name + "\" is not found!";
				throw custom_error("State Machine Error", _info.c_str());
			}
			current_state = state_pool[name];
		}

		// 切换状态节点
		void switch_to(const std::string& name)
		{
			// 如果未找到目标状态
			if (state_pool.find(name) == state_pool.end())
			{
				std::string _info = "[switch_to()]: State node \"" + name + "\" is not found!";
				throw custom_error("State Machine Error", _info.c_str());
			}

			if (current_state) current_state->on_exit();	// 退出
			current_state = state_pool[name];	// 切换
			current_state->on_enter();	// 进入
		}

		// 添加状态节点
		void add_state_node(const std::string& name, StateNode* state)
		{
			if (state_pool.find(name) != state_pool.end())	// 如果该状态已存在
			{
				std::string _info = "[add_state_node()]: State node \"" + name + "\" is already exist!";
				throw custom_error("State Machine Error", _info.c_str());
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
}
#pragma once

#include "fce_base.h"
#include "fce_utils.h"
#include "fce_kits.h"

#include <functional>

namespace fce
{

	// 精灵基类
	class Sprite
	{
	public:
		Sprite() = default;
		virtual ~Sprite() = default;

		// 获取速度
		const Vector2& get_velocity() const { return this->velocity; }

		// 设置速度
		void set_velocity(const Vector2& velocity) { this->velocity = velocity; }

		// 获取方向
		const float get_direction() const { return this->direction; }

		// 设置方向
		void set_direction(float dir) { this->direction = dir; }

		// 获取世界坐标
		const Point& get_position() const { return this->position; }

		// 设置世界坐标
		void set_position(const Point& pos) { this->position = pos; }

		// 获取精灵大小
		const Size& get_size() const { return this->size; }

		// 设置精灵大小
		void set_size(const Size& size) { this->size = size; }

		// 设置渲染层
		void set_render_layer(RenderLayer layer) { this->render_layer = layer; }

		// 获取渲染层
		RenderLayer get_render_layer() const { return this->render_layer; }

		// 获取自身碰撞箱
		CollisionBox* get_collision_box() const { return this->hit_box; }

		// 面向指定坐标点
		void point_torwards(const Point& target);

		// 面向鼠标指针
		void point_mousePointer(const Camera2D& camera);

		virtual void on_update(float delta) {};				// 更新逻辑
		virtual void on_render(const Camera2D& camera) {};	// 渲染画面
		virtual void on_input(const SDL_Event& event) {};	// 处理输入
		virtual void reset_property() {};					// 重置角色属性

	protected:
		Point position;								  // 世界坐标
		Size size = { 0, 0 };						  // 精灵大小
		Vector2 velocity;							  // 速度
		float direction = 0.0f;						  // 方向
		CollisionBox* hit_box = nullptr;			  // 自身碰撞箱
		RenderLayer render_layer = RenderLayer::None; // 渲染层
	};

	// 标签
	class Label : public Sprite
	{
	public:
		Label() { this->render_layer = RenderLayer::Label; }
		~Label() = default;

		Label(const Point& position, TTF_Font* font, SDL_Color color, float size, const std::string& info);

		// 设置标签字体
		void set_font(TTF_Font* font) { this->label_font = font; }

		// 设置标签颜色
		void set_color(SDL_Color color) { this->label_color = color; }

		// 设置标签大小
		void set_size(float ptsize) { this->ptsize = ptsize; }

		// 设置标签文本内容
		void set_info(const std::string& info) { this->label_info = info; }

		// 渲染文本
		void on_render(const Camera2D& camera) override;

	private:
		TTF_Font* label_font = nullptr;	// 字体
		SDL_Color label_color{};		// 颜色
		float ptsize = 0.0f;			// 大小
		std::string label_info;			// 文本内容
	};

	// 按钮（仅适用于UI层）
	class Button : public Sprite
	{
		// 按钮特效对组
		using effect_pair = std::pair<SDL_Texture*, Mix_Chunk*>;
	public:
		Button() = default;
		~Button() = default;
		Button(const Point& pos, const Size& size);

		// 设置大小
		void set_size(const Size& size) { this->size = size; }

		// 获取大小
		const Size& get_size() const { return this->size; }

		// 设置点击回调函数
		void set_on_click(std::function<void()> callback) { this->on_click = callback; }

		// 设置按钮纹理（分别为：正常、悬停、被点击）
		void set_texture(SDL_Texture* normal, SDL_Texture* hover, SDL_Texture* click);

		// 设置按钮音效（分别为：悬停、被点击）
		void set_audio(Mix_Chunk* hover, Mix_Chunk* click);

		// 渲染按钮
		void on_render(const Camera2D& camera) override;

		// 处理输入事件
		void on_input(const SDL_Event& event) override;

	private:
		bool is_first_hover = false;							// 是否首次悬停
		SDL_Texture* current_texture = nullptr;					// 当前显示的纹理
		std::unordered_map<std::string, effect_pair> effects;	// 效果列表
		std::function<void()> on_click;							// 点击回调函数
	};

	// 场景基类
	class Scene
	{
	public:
		Scene() = default;	 // 构造函数处初始化实例化对象
		virtual ~Scene();	 // 析函数出销毁实例化对象

		// 注册新精灵
		void register_sprite(const std::string& name, Sprite* new_sprite);

		// 销毁目标精灵
		void destroy_sprite(Sprite* sprite);

		// 获取精灵（返回指定类型模板的对象指针）
		template <typename _CvtTy = Sprite>
		_CvtTy* find_sprite(const std::string& name);

		// 获取精灵组（返回一个范围迭代器）
		auto find_group(const std::string& name);

		virtual void on_enter() {};											  // 进入场景（对角色属性的重置）
		virtual void on_update(float delta) {};								  // 更新场景
		virtual void on_render(const Camera2D& game, const Camera2D& ui) {};  // 渲染场景（一个负责渲染游戏，一个负责渲染UI）
		virtual void on_input(const SDL_Event& event) {};					  // 输入事件处理
		virtual void on_exit() {};											  // 退出场景（不要销毁对象）

	protected:
		// 更新所有精灵
		void sprites_update(float delta);

		// 渲染所有精灵
		void sprites_render(const Camera2D& game, const Camera2D& ui);

		// 处理所有精灵的输入事件
		void sprites_input(const SDL_Event& event);

		// 销毁所有精灵
		void destroy_all_sprites();

		// 设置是否启用Y轴排序(默认是true)
		void set_enable_y_sort(bool flag) { this->is_enabled_y_sort = flag; }

	private:
		// 渲染指定渲染层的所有精灵
		void render_layer(RenderLayer layer, const Camera2D& game, const Camera2D& ui);

		// 判断某个渲染层是否需要按Y轴排序
		bool layer_need_y_sort(RenderLayer layer) const;

	private:
		std::unordered_multimap<std::string, Sprite*> sprites_pool;					// 精灵池（支持重名）
		std::unordered_map<RenderLayer, std::vector<Sprite*>> layered_sprite_pool;	// 分层渲染池
		bool needs_sorting = false;													// 是否需要排序
		bool is_enabled_y_sort = true;												// 是否启用Y轴排序
	};
};
module;

#include <algorithm>
#include <string>
#include <unordered_map>
#include <vector>
#include <cmath>
#include <format>

#include <SDL3/SDL.h>

export module FlexCore:FCE_Element;

import :FCE_BaseSetup;
import :FCE_BaseType;
import :FCE_Graphic;
import :FCE_UI;
import :FCE_Utils;
import :FCE_Component;

export namespace fce
{
	// 精灵
	class Sprite
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

		// 面向鼠标指针
		void point_to_mouse(const Camera& camera)
		{
			Vector2 _window_mouse_pos, _logical_mouse_pos;
			SDL_GetMouseState(&_window_mouse_pos.x, &_window_mouse_pos.y);	// 获取鼠标物理窗口坐标
			SDL_RenderCoordinatesFromWindow(Main_Renderer, _window_mouse_pos.x, _window_mouse_pos.y, &_logical_mouse_pos.x, &_logical_mouse_pos.y); // 获取鼠标逻辑窗口坐标

			Vector2 _world_mouse_pos = camera.screen_to_world(_logical_mouse_pos); // 逻辑窗口坐标转世界坐标
			Vector2 dist = _world_mouse_pos - rect.position;

			rect.direction = maths::rad_to_deg(std::atan2f(dist.y, dist.x));
		}

		// 面向目标点
		void point_to(const Vector2& target)
		{
			Vector2 dist = target - rect.position;
			rect.direction = maths::rad_to_deg(std::atan2f(dist.y, dist.x));
		}

	protected:
		Rect rect;				// 变换矩形
		std::string tag = "";	// 标签
		RenderLayer render_layer = RenderLayer::None;	// 渲染层

	private:
		std::string group_tag = "";	 // 所属组标签
	};

	// 精灵组
	class Scene;
	class SpriteGroup
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
		void remove_sprite(Sprite* sprite)
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

	// 平面角色模板
	class FlatBody : public Sprite
	{
	public:
		FlatBody() = default;
		~FlatBody() override = default;

		void reset_props() override
		{
			velocity = Vector2(0.0f, 0.0f);
			up = down = left = right = false;
		}

		void on_update(float delta) override
		{
			this->move_dir = { (right - left) * 1.0f, (down - up) * 1.0f };
			this->velocity = move_dir.normalize() * speed;
			this->rect.position += velocity * delta;
		}

		void on_input(const SDL_Event& event) override
		{
			if (event.type == SDL_EVENT_KEY_DOWN)
			{
				if (enable_arrow_ctrl) // 启用方向键控制
				{
					if (event.key.key == SDLK_UP) up = true;
					if (event.key.key == SDLK_DOWN) down = true;
					if (event.key.key == SDLK_LEFT) left = true;
					if (event.key.key == SDLK_RIGHT) right = true;
				}
				if (enable_wasd_ctrl)	// 启用WASD控制
				{
					if (event.key.key == SDLK_W) up = true;
					if (event.key.key == SDLK_S) down = true;
					if (event.key.key == SDLK_A) left = true;
					if (event.key.key == SDLK_D) right = true;
				}
			}
			if (event.type == SDL_EVENT_KEY_UP)
			{
				if (enable_arrow_ctrl)
				{
					if (event.key.key == SDLK_UP) up = false;
					if (event.key.key == SDLK_DOWN) down = false;
					if (event.key.key == SDLK_LEFT) left = false;
					if (event.key.key == SDLK_RIGHT) right = false;
				}
				if (enable_wasd_ctrl)
				{
					if (event.key.key == SDLK_W) up = false;
					if (event.key.key == SDLK_S) down = false;
					if (event.key.key == SDLK_A) left = false;
					if (event.key.key == SDLK_D) right = false;
				}
			}
		}

	protected:
		Vector2 velocity;				// 速度
		Vector2 move_dir = { 0, 0 };	// 方向向量
		float speed = 200.0f;			// 移动速度大小

		bool enable_arrow_ctrl = true;  // 开启方向键控制移动
		bool enable_wasd_ctrl = true;	// 开启WASD控制移动

	private:
		bool up = false;
		bool down = false;
		bool left = false;
		bool right = false;
	};

	// 场景
	class Scene
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

		// 查找精灵(支持指定类型查找)
		template <typename cvt_ty = Sprite>
		cvt_ty* find_sprite(const std::string& name)
		{
			// 判断是否存在
			if (sprite_registry.find(name) == sprite_registry.end())
			{
				std::string _info = "[find_sprite()]: Sprite: \"" + name + "\" is not found!";
				throw custom_error("Scene Tree Error", _info.c_str());
			}
			
			// 判断转换类型是否合法
			cvt_ty* _ret = dynamic_cast<cvt_ty*>(sprite_registry[name]);
			if (_ret == nullptr)
			{
				std::string _info = std::format("<{}> --> <{}>", typeid(Sprite*).name(), typeid(cvt_ty*).name());
				throw custom_error("Scene Tree Error", "[find_sprite()]: Illegal polymorphic type conversion!\n" + _info);
			}

			return _ret;
		}

		// 查找精灵组
		SpriteGroup* find_group(const std::string& name)
		{
			if (sprite_group.find(name) == sprite_group.end())
			{
				std::string _info = "[find_group()]: Sprite Group: \"" + name + "\" is not found!";
				throw custom_error("Scene Tree Error", _info.c_str());
			}
			return sprite_group[name];
		}

		// 注册精灵
		void add_sprite(Sprite* sprite, const std::string& group_name = "")
		{
			// 既没有组名也没有标签的精灵是非法的
			if (sprite->get_tag() == "" && group_name == "")
			{
				std::string _info = "[add_sprite()]: Sprites that provide neither a unique tag nor a group name are illegal";
				throw custom_error("Scene Tree Error", _info.c_str());
			}

			if (sprite->get_tag() != "") // 提供了唯一标签
			{
				// 验证唯一性
				if (sprite_registry.find(sprite->get_tag()) != sprite_registry.end())
				{
					std::string _info = "[add_sprite()]: Sprite: " + sprite->get_tag() + "is already exist!";
					throw custom_error("Scene Tree Error", _info.c_str());
				}
				sprite_registry[sprite->get_tag()] = sprite;
			}
			if (group_name != "") // 提供了group_name
			{
				sprite->set_group_tag(group_name);

				// 将精灵添加到组里，并校验组的存在
				SpriteGroup* _group_ptr = sprite_group[group_name];
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
				throw custom_error("Scene Tree Error", "[remove_sprite()]: sprite not registered!");

			// 从sprite_list中删除
			pos_index[sprite_list[sprite_list.size() - 1]] = pos_index[sprite];
			std::swap(sprite_list[pos_index[sprite]], sprite_list[sprite_list.size() - 1]);
			sprite_list.pop_back();
			pos_index.erase(sprite);

			if (sprite->get_tag() != "")		// 从注册表中删除
				sprite_registry.erase(sprite->get_tag());
			if (sprite->get_group_tag() != "")	// 从组中删除
				sprite_group[sprite->get_group_tag()]->remove_sprite(sprite);

			delete sprite;
		}

		// 清空精灵列表
		void clear_sprite()
		{
			for (Sprite* sprite : sprite_list) delete sprite;
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

		std::unordered_map<std::string, Sprite*> sprite_registry;   // 精灵注册表
		std::unordered_map<std::string, SpriteGroup*> sprite_group; // 精灵组
	};
}
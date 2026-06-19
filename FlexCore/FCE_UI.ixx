module;

#include <algorithm>
#include <cmath>
#include <SDL3/SDL.h>

export module FlexCore:FCE_UI;

import :FCE_BaseType;
import :FCE_Graphic;
import :FCE_Utils;

export namespace fce
{
	class UI
	{
	public:
		UI() = default;
		virtual ~UI() = default;

		virtual void on_update(float delta) {};
		virtual void on_render(const Camera& camera) {};
		virtual void on_input(const SDL_Event& event) {};

	protected:
		Vector2 position = { 0, 0 };
	};

	// UI按钮接口
	class IButton : public UI
	{
	public:
		IButton() = default;
		~IButton() override = default;

		void on_input(const SDL_Event& event) override final
		{
			// 判断鼠标是否在按钮上悬停
			bool in_range_x = event.motion.x >= position.x - size.w / 2.0f && event.motion.x <= position.x + size.w / 2.0f;
			bool in_range_y = event.motion.y >= position.y - size.h / 2.0f && event.motion.y <= position.y + size.h / 2.0f;

			if (event.type == SDL_EVENT_MOUSE_MOTION)	// 鼠标悬停在按钮上
			{
				if (in_range_x && in_range_y)
				{
					this->on_hover();
					this->is_first_hover = false;
					SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER));
				}
				else
				{
					this->on_normal();
					this->is_first_hover = true;
					SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT));
				}
			}
			if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT) // 按钮被点击
			{
				if (in_range_x && in_range_y)
					this->on_click();
			}
			if (event.type == SDL_EVENT_MOUSE_BUTTON_UP && event.button.button == SDL_BUTTON_LEFT) // 按钮被释放
			{
				if (in_range_x && in_range_y)
				{
					this->on_hover();
					SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER));
				}
				else
				{
					this->on_normal();
					SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT));
				}
			}
		}

	protected:
		virtual void on_normal() = 0;	// 正常
		virtual void on_hover() = 0;	// 鼠标悬空
		virtual void on_click() = 0;	// 被点击

	protected:
		Size size = { 0, 0 };		 // 大小
		bool is_first_hover = true;	 // 是否首次悬停
	};

	// UI标签接口
	class ILabel : public UI
	{
	public:
		ILabel() = default;
		~ILabel() override = default;
	};

	// UI滑块接口
	class ISlider : public UI
	{
	public:
		ISlider() = default;
		ISlider(int* val, int min, int max, int step) 
			: val(val), val_min(min), val_max(max), step(step) {}

		~ISlider() override { this->val = nullptr; }

		void bind_int(int* val, int min, int max, int step = 1)
		{
			this->val = val;
			this->val_min = min, this->val_max = max;
			this->step = step;
		}

		void on_update(float delta) override final
		{
			if (val == nullptr)
				throw custom_error("ISlider Error", "Func <bind_int()>: Value unbound!");

			float drag_begin_x = position.x - drag_range / 2.0f;
			float drag_end_x = position.x + drag_range / 2.0f;

			dragbtn_pos.x = std::clamp(dragbtn_pos.x, drag_begin_x, drag_end_x);
			*val = std::clamp(*val, val_min, val_max);

			if (can_drag)
			{
				float ratio = (mouse_pos.x - drag_begin_x) / drag_range;
				float original_val = val_min + ratio * (val_max - val_min);

				*val = (int)std::round(original_val / step) * step;
				*val = std::clamp(*val, val_min, val_max);
			}

			dragbtn_pos.x = drag_begin_x + (*val - val_min) * 1.0f / (val_max - val_min) * drag_range;
			dragbtn_pos.y = position.y;
		}

		void on_input(const SDL_Event& event) override final
		{
			this->mouse_pos = { event.motion.x, event.motion.y };
			bool in_range_x = (mouse_pos.x >= dragbtn_pos.x - dragbtn_size.w / 2.0f && mouse_pos.x <= dragbtn_pos.x + dragbtn_size.w / 2.0f);
			bool in_range_y = (mouse_pos.y >= dragbtn_pos.y - dragbtn_size.h / 2.0f && mouse_pos.y <= dragbtn_pos.y + dragbtn_size.h / 2.0f);
			if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
			{
				if (in_range_x && in_range_y && event.button.button == SDL_BUTTON_LEFT)
					this->can_drag = true;
			}
			if (event.type == SDL_EVENT_MOUSE_BUTTON_UP)
			{
				if (event.button.button == SDL_BUTTON_LEFT)
					this->can_drag = false;
			}
		}

		void on_render(const Camera& camera) override { mouse_pos = camera.screen_to_world(mouse_pos); }

	protected:
		int* val = nullptr;
		Vector2 dragbtn_pos = { 0.0f, 0.0f };
		Size dragbtn_size = { 0.0f, 0.0f };
		float drag_range = 0.0f;

	private:
		Vector2 mouse_pos = { 0.0f, 0.0f };
		bool can_drag = false;
		int val_min = 0, val_max = 0;
		int step = 0;
	};
}
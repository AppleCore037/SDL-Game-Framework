#pragma once

// ��׼��
#include <chrono>
#include <filesystem>
#include <functional>
#include <memory>
#include <random>
#include <string>
#include <thread>
#include <unordered_map>

// �������⣨SDL��
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>

namespace fce
{
// ==============================================================================================
// 				������������							Base Variables Attributes
// ==============================================================================================

	inline SDL_Window* Main_Window = nullptr;				// ������
	inline SDL_Renderer* Main_Renderer = nullptr;			// ����Ⱦ��
	inline TTF_TextEngine* Main_TextEngine = nullptr;		// ����������

	constexpr float PAI = 3.14159265f;							// Բ����
	constexpr SDL_Color Color_Red = { 255, 0, 0, 255 };			// ��
	constexpr SDL_Color Color_Blue = { 0, 0, 255, 255 };		// ��
	constexpr SDL_Color Color_Green = { 0, 255, 0, 255 };		// ��
	constexpr SDL_Color Color_White = { 255, 255, 255, 255 };	// ��
	constexpr SDL_Color Color_Black = { 0, 0, 0, 255 };			// ��
	constexpr SDL_Color Color_Gray = { 128, 128, 128, 255 };	// ��

	constexpr SDL_InitFlags SDL_INIT_EVERYTHING = (SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC | SDL_INIT_GAMEPAD | SDL_INIT_SENSOR);
	constexpr MIX_InitFlags MIX_INIT_EVERYTHING = (MIX_INIT_MP3 | MIX_INIT_FLAC | MIX_INIT_MID | MIX_INIT_MOD | MIX_INIT_OGG | MIX_INIT_OPUS | MIX_INIT_WAVPACK);

// ==============================================================================================
//				�� �� �� ��						Base	Functions
// ==============================================================================================

	// ��ʼ��������������
	inline void Init_Graphic(const std::string& title, int w, int h,
		SDL_WindowFlags flags = SDL_EVENT_WINDOW_SHOWN)
	{
		// ��ʼ��SDL���ģ��
		SDL_Init(SDL_INIT_EVERYTHING);
		TTF_Init();

		SDL_AudioSpec spec = { SDL_AUDIO_S16, 2, 44100 };
		Mix_Init(MIX_INIT_EVERYTHING);
		Mix_OpenAudio(NULL, &spec);

		// ��ʼ������
		if (Main_Window) return; // ��������Ѵ������ٴ���
		Main_Window = SDL_CreateWindow(title.c_str(), w, h, flags);
		if (!Main_Window)
			throw std::runtime_error(u8"Failed to create Main_Window!");

		// ��ʼ����Ⱦ��
		if (Main_Renderer) return;	// �����Ⱦ���Ѵ������ٴ���
		Main_Renderer = SDL_CreateRenderer(Main_Window, nullptr);
		if (!Main_Renderer)
			throw std::runtime_error(u8"Failed to create Main_Renderer!");

		// ��ʼ���ı�����
		if (Main_TextEngine) return; // ��������Ѵ������ٴ���
		Main_TextEngine = TTF_CreateRendererTextEngine(Main_Renderer);
		if (!Main_TextEngine)
			throw std::runtime_error(u8"Failed to create Main_TextEngine!");
	}

	// �ͷſ��������Դ [������Main_TextEngine��Main_Renderer��Main_Window]
	inline void Clean_Up()
	{
		if (Main_TextEngine)
		{
			TTF_DestroyRendererTextEngine(Main_TextEngine);
			Main_TextEngine = nullptr;
		}
		if (Main_Renderer)
		{
			SDL_DestroyRenderer(Main_Renderer);
			Main_Renderer = nullptr;
		}
		if (Main_Window)
		{
			SDL_DestroyWindow(Main_Window);
			Main_Window = nullptr;
		}
	}

	// ��ȡ����/����¼���UTF-8��ʽ�ַ���
	inline const char* Get_EventName(const SDL_Event& event)
	{
		static std::string result = "None";

		if (event.type == SDL_EVENT_KEY_DOWN)
			result = SDL_GetKeyName(event.key.key);
		if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
		{
			switch (event.button.button)
			{
			case SDL_BUTTON_LEFT:	result = "Left Mouse Button";   break;
			case SDL_BUTTON_MIDDLE: result = "Middle Mouse Button"; break;
			case SDL_BUTTON_RIGHT:	result = "Right Mouse Button";	break;
			case SDL_BUTTON_X1:		result = "Side Button 1";		break;
			case SDL_BUTTON_X2:		result = "Side Button 2";		break;
			}
		}
		if (event.type == SDL_EVENT_MOUSE_WHEEL)
		{
			if (event.wheel.y > 0) result = "Mouse Wheel Up";
			if (event.wheel.y < 0) result = "Mouse Wheel Down";
		}
		if (event.type == SDL_EVENT_MOUSE_MOTION)
			result = "Mouse Motion";

		return result.c_str();
	}

// ==============================================================================================
//				�� �� �� ��								Base	Type
// ==============================================================================================

	// ��С
	struct Size
	{
		Size() = default;
		~Size() = default;
		Size(float w, float h) :
			width(w), height(h) {}

		float width;
		float height;
	};

	// ��ά����
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

		// ��ȡ����
		float length() const { return sqrt(x * x + y * y); }

		// ����Ƿ�������Ŀ������
		bool approx(const Vector2& target) const { return (*this - target).length() <= 0.0001f; }

		// ��׼��
		Vector2 normalize() const
		{
			float len = this->length();

			if (len == 0)
				return Vector2(0, 0);
			return Vector2(x / len, y / len);
		}
	};

	// ͼ��
	class Atlas
	{
	public:
		Atlas() = default;

		~Atlas()
		{
			for (SDL_Texture* texture : tex_list)
				SDL_DestroyTexture(texture);
		}

		// ��ָ��·��ģ�����Ŀ����������
		void load_from_file(SDL_Renderer* renderer, const char* path_template, int num)
		{
			for (int i = 0; i < num; i++)
			{
				char path_file[256];
				sprintf_s(path_file, path_template, i + 1);		// ��ȫ·��

				SDL_Texture* texture = IMG_LoadTexture(renderer, path_file);	// ��������
				tex_list.push_back(texture);	// ���������б�
			}
		}

		// ���ͼ��
		void clear() { tex_list.clear(); }

		// ��ȡͼ����������
		int get_size() const { return (int)tex_list.size(); }

		// �������
		void add_texture(SDL_Texture* texture) { tex_list.push_back(texture); }

		// ��ȡ����
		SDL_Texture* get_texture(int idx)
		{
			if (idx < 0 || idx >= tex_list.size())
				return nullptr;
			return tex_list[idx];
		}

	private:
		std::vector<SDL_Texture*> tex_list;		// ����
	};

	// ��Ⱦ�㼶
	enum class RenderLayer
	{
		None,			// ����Ⱦ��
		Background,		// ������
		Label,			// ��Ϸ�ڲ��ı���
		GameObject,		// ��ϷԪ�ز�
		Effect,			// ��Ч��
		Frontground,	// ǰ����
		UI				// UI��
	};

	// ��ײ�㼶
	enum class CollisionLayer
	{
		None = 0,				// ����ײ��
		Player = 1 << 0,		// ��Ҳ�
		Enemy = 1 << 1,			// ���˲�
		GameMap = 1 << 2,		// ��Ϸ��ͼ��
		GameObject = 1 << 3,	// ��ϷԪ�ز�
		Attack = 1 << 4,		// ������
	};

	// ���ذ�λ������� |
	constexpr CollisionLayer operator|(CollisionLayer lhs, CollisionLayer rhs)
	{
		using underlying = std::underlying_type_t<CollisionLayer>;
		return static_cast<CollisionLayer>(static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
	}

	// ���ذ�λ������� &
	constexpr bool operator&(CollisionLayer lhs, CollisionLayer rhs)
	{
		using underlying = std::underlying_type_t<CollisionLayer>;
		return (static_cast<underlying>(lhs) & static_cast<underlying>(rhs)) != 0;
	}

// ==============================================================================================
//				�� �� �� ��								Base	Kits
// ==============================================================================================

	// ���̴�����
	class custom_error
	{
	public:
		custom_error(const std::string& title, const std::string& info)
			: error_title(title), error_info(info) {}

		custom_error(const char* title, const char* info)
			: error_title(title), error_info(info) {}

		~custom_error() = default;

		// ��ȡ�������
		const char* title() const noexcept { return error_title.c_str(); }

		// ��ȡ������Ϣ
		const char* what() const noexcept { return error_info.c_str(); }

	private:
		std::string error_title;	// �������
		std::string error_info;		// ������Ϣ
	};

	// �����������
	class Random
	{
	public:
		Random()
		{
			std::random_device rd;
			engine.seed(rd());
		}

		// ���� [min, max] ��Χ�ڵ��������
		static int randint(int min, int max)
		{
			if (min > max) std::swap(min, max);
			std::uniform_int_distribution<int> dist(min, max);
			return dist(get_engine());
		}

		// ���� [min, max] ��Χ�ڵ����������
		static float randfloat(float min, float max)
		{
			if (min > max) std::swap(min, max);
			std::uniform_real_distribution<float> dist(min, max);
			return dist(get_engine());
		}

	private:
		static std::mt19937& get_engine()
		{
			static Random _Instance;
			return _Instance.engine;
		}

		std::mt19937 engine;
	};

	// ʱ��
	class Clock
	{
	public:
		Clock()
		{
			this->target_fps = 60;			// Ĭ��60֡
			this->target_time = 1000 / target_fps;
			this->last_time = std::chrono::steady_clock::now();
			this->delta_time = 0;
			this->time_scale = 1.0f;
		}

		Clock(int fps_limit)
		{
			this->target_fps = fps_limit;	// ����Ŀ��FPS
			this->target_time = 1000 / target_fps;
			this->last_time = std::chrono::steady_clock::now();
			this->delta_time = 0;
			this->time_scale = 1.0f;
		}

		~Clock() = default;

		// ʱ�ӿ�ʼ��ʱ
		void start_frame()
		{
			auto currentTime = std::chrono::steady_clock::now();
			delta_time = static_cast<float>(
				std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - last_time).count());
			last_time = currentTime;
		}

		// ʱ�ӽ�����ʱ
		void end_frame() const
		{
			auto currentTime = std::chrono::steady_clock::now();
			// ��ȡ����ʱ��
			auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - last_time).count();

			// �������ʱ��С��֡���������
			if (elapsedTime < target_time)
				std::this_thread::sleep_for(std::chrono::milliseconds(target_time - elapsedTime));
		}

		// �����Ƿ�ֱͬ��
		void set_VSync(bool is_abled)
		{
			if (is_abled)
			{
				int refresh = this->get_screen_refreshRate();
				// printf("��Ļˢ����: %d\n", refresh);	// ��ӡ��Ļˢ����

				if (refresh)
					this->target_time = 1000 / refresh;
				else
					throw custom_error(u8"Clock Error", u8"Warning: Cannot get screen refresh rate!");
			}
			else
				this->target_time = 1000 / target_fps;		// ����ΪĿ��FPS
		}

		// ����FPS
		void setFPS(int fps_limit)
		{
			this->target_fps = fps_limit;		// ����Ŀ��FPS
			this->target_time = 1000 / fps_limit;
		}

		// ��ȡFPS
		int getFPS() const { return (int)(1000 / delta_time); }

		// ��ȡ֡���
		float get_DeltaTime() const { return (delta_time / 1000.0f) * time_scale; }

		// ����ʱ������
		void set_time_scale(float scale)
		{
			if (scale <= 0.0f) return;
			this->time_scale = scale;
		}

	private:
		// ��ȡ��Ļˢ����
		int get_screen_refreshRate()
		{
			// ��ȡ����ʾ����Ĭ����ʾ����
			SDL_DisplayID display = SDL_GetPrimaryDisplay();
			if (!display) return 0;

			// ��ȡ��ǰ��ʾģʽ
			const SDL_DisplayMode* mode = SDL_GetCurrentDisplayMode(display);
			if (!mode) return 0;

			// ����ˢ���ʣ������Ч�����򷵻�Ĭ��ֵ��
			return (mode->refresh_rate > 0) ? (int)mode->refresh_rate : 0;
		}

	private:
		int target_fps;											// Ŀ��FPS
		int target_time;										// Ŀ��֡���
		std::chrono::steady_clock::time_point last_time;		// ����ʱ��
		float delta_time;										// ��֡���
		float time_scale;										// ʱ������
	};

	// ��ʱ��
	class Timer
	{
	public:
		Timer() = default;
		~Timer() = default;

		// ������ʱ��
		void restart() { pass_time = 0; shotted = false; }

		// ���õȴ�ʱ��
		void set_wait_time(float val) { this->wait_time = val; }

		// �����Ƿ񵥴δ���
		void set_one_shot(bool flag) { this->one_shot = flag; }

		// ���ûص�����
		void set_on_timeout(std::function<void()> timeout_callback) { this->on_timeout = timeout_callback; }

		// ��ͣ
		void pause() { paused = true; }

		// ����
		void resume() { paused = false; }

		// ���¼�ʱ��
		void on_update(float delta)
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

	private:
		float pass_time = 0;				// ����ʱ��
		float wait_time = 0;				// �ȴ�ʱ��
		bool paused = false;				// �Ƿ���ͣ
		bool shotted = false;				// �Ƿ��Ѿ�����
		bool one_shot = false;				// �Ƿ񵥴ʴ���
		std::function<void()> on_timeout;	// �ص�����
	};

	// ��ѧ���㹤��
	namespace maths
	{
		// �Ƕ�ת����
		inline float deg_to_rad(float degree) noexcept { return degree * PAI / 180.0f; }

		// ����ת�Ƕ�
		inline float rad_to_deg(float radian) noexcept { return radian * 180.0f / PAI; }

		// ��ֵ����
		inline float lerp(float current, float target, float t) noexcept { return current + (target - current) * t; }

		// �����������
		inline float distance_to(const Vector2& pos_1, const Vector2& pos_2)
		{
			float dx = pos_1.x - pos_2.x;
			float dy = pos_1.y - pos_2.y;
			return sqrtf((dx * dx) + (dy * dy));
		}

		// ���ƿ���Բ
		inline void Draw_Circle(SDL_Renderer* renderer, float centerX, float centerY, float radius)
		{
			float x = radius; float y = 0; float err = 0;
			while (x >= y)
			{
				SDL_RenderPoint(renderer, centerX + x, centerY + y);
				SDL_RenderPoint(renderer, centerX + y, centerY + x);
				SDL_RenderPoint(renderer, centerX - y, centerY + x);
				SDL_RenderPoint(renderer, centerX - x, centerY + y);
				SDL_RenderPoint(renderer, centerX - x, centerY - y);
				SDL_RenderPoint(renderer, centerX - y, centerY - x);
				SDL_RenderPoint(renderer, centerX + y, centerY - x);
				SDL_RenderPoint(renderer, centerX + x, centerY - y);

				if (err <= 0) { y += 1; err += 2 * y + 1; }
				if (err > 0) { x -= 1; err -= 2 * x + 1; }
			}
		}

		// ����ʵ��Բ
		inline void Draw_FilledCircle(SDL_Renderer* renderer, float centerX, float centerY, float radius)
		{
			for (float y = -radius; y <= radius; y += 1.0f)
			{
				float x = sqrtf(radius * radius - y * y);
				SDL_RenderLine(renderer, centerX - x, centerY + y, centerX + x, centerY + y);
			}
		}
	};

// =========================================================================================
// 				�� �� Ԫ ��							 base	Elements
// =========================================================================================

	// �����
	class Camera
	{
	public:
		// ���淽ʽ
		enum FollowStyle
		{
			None = 0,					// �޸���
			Static_Follow = 1 << 0,		// ��̬����
			Smooth_Follow = 1 << 1,		// ƽ������
			Only_X = 1 << 2,			// ������X��
			Only_Y = 1 << 3				// ������Y��
		};

	public:
		Camera()
		{
			timer_shake.set_one_shot(true);
			timer_shake.set_on_timeout([&]()
				{
					is_shaking = false;
					shake_position = Vector2(0, 0);
				});
		}

		~Camera() = default;

		// ��ȡ���������(���ĵ�)
		const Vector2& get_position() const { return position; }

		// �������������(���ĵ�)
		void set_position(const Vector2& pos) { this->base_position = pos; }

		// �������������
		void set_zoom(float scale) { this->zoom = scale; }

		// ��ȡ���������
		float get_zoom() const { return this->zoom; }

		// ���������
		void reset()
		{
			position = base_position = shake_position = Vector2(0, 0);
			this->zoom = 1.0f;		// ��������Ϊ1.0
		}

		// ���������
		void shake(float strength, float duration)
		{
			is_shaking = true;
			shaking_strength = strength;

			timer_shake.set_wait_time(duration);
			timer_shake.restart();
		}

		// ��Ⱦ����
		void render_texture(SDL_Texture* texture, const SDL_FRect* rect_src, const SDL_FRect* rect_dst,
			double angle, const SDL_FPoint* center, bool is_flip = false) const
		{
			// ��Ⱦ���� = ��Ļ���ĵ� + (��ɫ�������� - ���������) * ��������
			SDL_FRect rect_dst_win = *rect_dst;
			rect_dst_win.x = this->get_screen_center().x + (rect_dst_win.x - position.x) * zoom;
			rect_dst_win.y = this->get_screen_center().y + (rect_dst_win.y - position.y) * zoom;

			// ��Ⱦ��С = �����С * ��������
			rect_dst_win.w = rect_dst->w * zoom;
			rect_dst_win.h = rect_dst->h * zoom;

			SDL_RenderTextureRotated(Main_Renderer, texture, rect_src, &rect_dst_win, angle, center,
				is_flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
		}

		// �����߶�
		void render_shape(const Vector2& start, const Vector2& end, SDL_Color color) const
		{
			SDL_SetRenderDrawColor(Main_Renderer, color.r, color.g, color.b, color.a);

			Vector2 start_win = { get_screen_center().x + (start.x - position.x) * zoom, get_screen_center().y + (start.y - position.y) * zoom };
			Vector2 end_win = { get_screen_center().x + (end.x - position.x) * zoom, get_screen_center().y + (end.y - position.y) * zoom };

			SDL_RenderLine(Main_Renderer, start_win.x, start_win.y, end_win.x, end_win.y);
		}

		// ���ƾ���
		void render_shape(const Vector2& pos, const Size& size, SDL_Color color, bool is_filled) const
		{
			SDL_SetRenderDrawColor(Main_Renderer, color.r, color.g, color.b, color.a);

			// ������Ļ�е���Ⱦ����
			SDL_FRect rect_dst_win = { this->get_screen_center().x + (pos.x - position.x) * zoom,
				this->get_screen_center().y + (pos.y - position.y) * zoom,
				size.width * zoom, size.height * zoom };

			if (is_filled)
				SDL_RenderFillRect(Main_Renderer, &rect_dst_win);
			else
				SDL_RenderRect(Main_Renderer, &rect_dst_win);
		}

		// ����Բ��
		void render_shape(const Vector2& pos, float radius, SDL_Color color, bool is_filled) const
		{
			SDL_SetRenderDrawColor(Main_Renderer, color.r, color.g, color.b, color.a);

			// ����Բ�ε����ĵ�����
			Vector2 circle_center = { pos.x + radius, pos.y + radius };
			circle_center.x = this->get_screen_center().x + (circle_center.x - position.x) * zoom;
			circle_center.y = this->get_screen_center().y + (circle_center.y - position.y) * zoom;

			if (is_filled)
				maths::Draw_FilledCircle(Main_Renderer, circle_center.x, circle_center.y, radius * zoom);
			else
				maths::Draw_Circle(Main_Renderer, circle_center.x, circle_center.y, radius * zoom);
		}

		// ��Ⱦ����
		void render_text(const Vector2& position, TTF_Font* font, SDL_Color color, 
			float ptsize, const std::string& info) const
		{
			TTF_Text* text_win = TTF_CreateText(Main_TextEngine, font, info.c_str(), NULL);
			TTF_SetTextColor(text_win, color.r, color.g, color.b, color.a);

			Vector2 position_win;
			position_win.x = this->get_screen_center().x + (position.x - this->position.x) * zoom;
			position_win.y = this->get_screen_center().y + (position.y - this->position.y) * zoom;
			TTF_SetFontSize(font, ptsize * zoom);

			TTF_DrawRendererText(text_win, position_win.x, position_win.y);
		}

		// �����ɫ
		void look_at(const Vector2& target_pos, int style)
		{
			if (style & None) return;

			if (style & Camera::Static_Follow)	// ��̬����
			{
				if (style & Camera::Only_X)
					this->base_position.x = target_pos.x;
				else if (style & Camera::Only_Y)
					this->base_position.y = target_pos.y;
				else
					this->base_position = target_pos;
			}
			else if (style & Camera::Smooth_Follow)		// ƽ������
			{
				if (style & Camera::Only_X)
					this->base_position.x = maths::lerp(base_position.x, target_pos.x, smooth_strength);
				else if (style & Camera::Only_Y)
					this->base_position.y = maths::lerp(base_position.y, target_pos.y, smooth_strength);
				else
				{
					this->base_position.x = maths::lerp(base_position.x, target_pos.x, smooth_strength);
					this->base_position.y = maths::lerp(base_position.y, target_pos.y, smooth_strength);
				}
			}
		}

		// ��������ת��������
		Vector2 screen_to_world(const Vector2& screen_pos) const
		{
			// �������� = (�������� - ��Ļ���ĵ�) / �������� + ���������
			float world_x = (screen_pos.x - this->get_screen_center().x) / zoom + position.x;
			float world_y = (screen_pos.y - this->get_screen_center().y) / zoom + position.y;
			return Vector2(world_x, world_y);
		}

		// ���������
		void on_update(float delta)
		{
			timer_shake.on_update(delta);
			smooth_strength = SMOOTH_FACTOR * delta;

			if (is_shaking)
			{
				shake_position.x = (-50 + std::rand() % 100) / 50.0f * shaking_strength;
				shake_position.y = (-50 + std::rand() % 100) / 50.0f * shaking_strength;
			}

			// ����λ�� = ����λ�� + ����λ��
			position = base_position + shake_position;
		}

	private:
		// ��ȡ��Ļ���ĵ�
		Vector2 get_screen_center() const
		{
			int screen_w, screen_h;
			SDL_GetWindowSize(Main_Window, &screen_w, &screen_h);		// ������Ļ���
			Vector2 screen_center = Vector2(screen_w / 2.0f, screen_h / 2.0f);	// ��ȡ��Ļ���ĵ�
			return screen_center;
		}

	private:
		Vector2 position;					// ���������λ��
		Vector2 shake_position;				// ����λ��
		Vector2 base_position;				// ����λ��

		Timer timer_shake;					// ������ʱ��
		bool is_shaking = false;			// �Ƿ񶶶�
		float shaking_strength = 0;			// ��������
		float smooth_strength = 0;			// ƽ������
		float zoom = 1.0f;					// ���������

		const float SMOOTH_FACTOR = 1.5f;	// ƽ��ϵ������
	};

	// ����֡����
	class Animation
	{
	public:
		Animation()
		{
			timer.set_one_shot(false);
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

		~Animation() = default;

		//���ö���
		void reset() { timer.restart(); idx_frame = 0; }

		// ���ö���λ��
		void set_position(const Vector2& position) { this->position = position; }

		// ���ö�������
		void set_rotation(double angle) { this->angle = angle; }

		// ���ö������ĵ�
		void set_center(const SDL_FPoint& center) { this->center = center; }

		// ���ö����Ƿ�ѭ����Ĭ����true��
		void set_loop(bool is_loop) { this->is_loop = is_loop; }

		// ���ö���֡���
		void set_interval(float interval) { timer.set_wait_time(interval); }

		// ���ö����Ƿ�ת
		void set_flip(bool flag) { this->is_flip = flag; }

		// ���ö����ص�����
		void set_on_finished(std::function<void()> callback) { on_finished = callback; }

		// ���¶���
		void on_update(float delta) { timer.on_update(delta); }

		// �������֡
		void add_frame(Atlas* atlas)
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

		// �������֡
		void add_frame(SDL_Texture* texture, int num_h)
		{
			float width, height;
			SDL_GetTextureSize(texture, &width, &height);	// ��ȡ����֡���

			float width_frame = width / num_h;	// ��ȡ��������֡���
			for (int i = 0; i < num_h; i++)
			{
				SDL_FRect rect_src{};
				rect_src.x = i * width_frame, rect_src.y = 0;
				rect_src.w = width_frame, rect_src.h = height;

				frame_list.emplace_back(texture, rect_src);		// ������֡�����б�
			}
		}

		// ��Ⱦ����
		void on_render(const Camera& camera) const
		{
			const Frame& frame = frame_list[idx_frame];
			const Vector2& pos_camera = camera.get_position();

			SDL_FRect rect_dst{};
			rect_dst.x = position.x - frame.rect_src.w / 2.0f;
			rect_dst.y = position.y - frame.rect_src.h / 2.0f;
			rect_dst.w = frame.rect_src.w;
			rect_dst.h = frame.rect_src.h;

			camera.render_texture(frame.texture, &frame.rect_src, &rect_dst, angle, &center, is_flip);
		}

	private:
		// ����֡�ṹ��
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
		Vector2 position;					// λ��
		double angle = 0;					// �Ƕ�
		SDL_FPoint center = { 0 };			// ���ĵ�
		bool is_flip = false;				// �Ƿ�ת

		Timer timer;						// ���ü�ʱ��
		bool is_loop = true;				// �Ƿ�ѭ��
		size_t idx_frame = 0;				// ��ǰ֡����
		std::vector<Frame> frame_list;		// ����֡�б�
		std::function<void()> on_finished;	// ����������ϻص�����
	};

	// ��ײ��
	class CollisionManager;
	class CollisionBox
	{
		friend class CollisionManager;
	public:
		// �����Ƿ�������ײ
		void set_enabled(bool flag) { this->enabled = flag; }

		// ����������ײ��
		void set_layer_src(CollisionLayer layer) { this->layer_src = layer; }

		// ����Ŀ����ײ��
		void set_layer_dst(CollisionLayer layer) { this->layer_dst = layer; }

		// ������ײ�ص�����
		void set_on_collide(std::function<void(CollisionLayer)> callback) { this->on_collide = callback; }

		// ������ײ���С
		void set_size(const Size& size) { this->size = size; }

		// ��ȡ��ײ���С
		const Size& get_size() const { return this->size; }

		// ������ײ��λ��
		void set_position(const Vector2& pos) { this->position = pos; }

	private:
		Size size = { 0, 0 };								// ��ײ���С
		Vector2 position;									// ��ײ��λ��
		bool enabled = true;								// �Ƿ�������ײ���
		std::function<void(CollisionLayer)> on_collide;		// ��ײ�ص�����
		CollisionLayer layer_src = CollisionLayer::None;	// ������ײ��
		CollisionLayer layer_dst = CollisionLayer::None;	// Ŀ����ײ��

	private:
		CollisionBox() = default;
		~CollisionBox() = default;
	};

	// ����������
	class Sprite
	{
	public:
		Sprite() = default;
		~Sprite() = default;

		// ��ȡ�ٶ�
		const Vector2& get_velocity() const { return this->velocity; }

		// ��ȡ����
		const float get_direction() const { return this->direction; }

		// ���÷���
		void set_direction(float dir) { this->direction = dir; }

		// ��ȡλ��
		const Vector2& get_position() const { return this->position; }

		// ����λ��
		void set_position(const Vector2& pos) { this->position = pos; }

		// ������Ⱦ��
		void set_render_layer(RenderLayer layer) { this->render_layer = layer; }

		// ��ȡ��Ⱦ��
		RenderLayer get_render_layer() const { return this->render_layer; }

		// ����ָ�������
		void point_torwards(const Vector2& target)
		{
			float dx = target.x - position.x;
			float dy = target.y - position.y;
			this->direction = maths::rad_to_deg(std::atan2f(dy, dx));
		}

		// �������ָ��
		void point_mousePointer(const Camera& camera)
		{
			float mouse_x, mouse_y;
			SDL_GetMouseState(&mouse_x, &mouse_y);
			auto mouse_world_pos = camera.screen_to_world(Vector2(mouse_x, mouse_y));

			float dx = mouse_world_pos.x - position.x;
			float dy = mouse_world_pos.y - position.y;

			// ��ת���� = ����ת�Ƕ�(atan2(Ŀ������ - ��������))
			this->direction = maths::rad_to_deg(std::atan2f(dy, dx));
		}

		virtual void on_update(float delta) = 0;				// �����߼�
		virtual void on_render(const Camera& camera) = 0;		// ��Ⱦ����
		virtual void on_input(const SDL_Event& event) = 0;		// ��������
		virtual void reset_property() = 0;						// ���ý�ɫ����

	protected:
		Vector2 position;								// λ��
		Vector2 velocity;								// �ٶ�
		float direction = 0.0f;							// ����
		RenderLayer render_layer = RenderLayer::None;	// ��Ⱦ��
	};

	// ��ǩ
	class Label : public Sprite
	{
	public:
		Label() { this->render_layer = RenderLayer::Label; }
		~Label() = default;

		Label(const Vector2& position, TTF_Font* font, SDL_Color color, float size, const std::string& info) 
			: label_color(color), label_font(font), ptsize(size), label_info(info) 
		{
			this->position = position;
			this->render_layer = RenderLayer::Label;
		}

		// ���ñ�ǩ����
		void set_font(TTF_Font* font) { this->label_font = font; }

		// ���ñ�ǩ��ɫ
		void set_color(SDL_Color color) { this->label_color = color; }

		// ���ñ�ǩ��С
		void set_size(float ptsize) { this->ptsize = ptsize; }

		// ���ñ�ǩ�ı�����
		void set_info(const std::string& info) { this->label_info = info; }

		void on_update(float delta) override {}
		void on_input(const SDL_Event& event) override {}
		void reset_property() override {}

		void on_render(const Camera& camera) override 
		{ camera.render_text(position, label_font, label_color, ptsize, label_info.c_str()); }

	private:
		TTF_Font* label_font = nullptr;	// ����
		SDL_Color label_color{};		// ��ɫ
		float ptsize = 0.0f;			// ��С
		std::string label_info;			// �ı�����
	};

	// ��ť����������UI�㣩
	class Button : public Sprite
	{
		// ��ť��Ч����
		using effect_pair = std::pair<SDL_Texture*, Mix_Chunk*>;
	public:
		Button() = default;
		~Button() = default;

		Button(const Vector2& pos, const Size& size) : position(pos), size(size)
		{
			this->render_layer = RenderLayer::UI;
			this->effects["normal"] = std::make_pair(nullptr, nullptr);
			this->effects["hover"] = std::make_pair(nullptr, nullptr);
			this->effects["click"] = std::make_pair(nullptr, nullptr);
		}

		// ���ô�С
		void set_size(const Size& size) { this->size = size; }

		// ��ȡ��С
		const Size& get_size() const { return this->size; }

		// ���õ���ص�����
		void set_on_click(std::function<void()> callback) { this->on_click = callback; }

		// ���ð�ť�����ֱ�Ϊ����������ͣ���������
		void set_texture(SDL_Texture* normal, SDL_Texture* hover, SDL_Texture* click)
		{
			// ��������Ƿ�Ϊnullptr
			if (!normal || !hover || !click)
				throw custom_error(u8"Button Argument Error", u8"Argument texture cannot be nullptr!");

			effects["normal"].first = normal;
			effects["hover"].first = hover;
			effects["click"].first = click;
			this->current_texture = effects["normal"].first;	// Ĭ����ʾ����״̬����
		}

		// ���ð�ť��Ч���ֱ�Ϊ����ͣ���������
		void set_audio(Mix_Chunk* hover, Mix_Chunk* click)
		{
			effects["hover"].second = hover;
			effects["click"].second = click;
		}

		// ���º�����Ĭ��
		void on_update(float delta) override 
		{
			// ��ȡ������Ļ����
			float mouse_x, mouse_y;
			SDL_GetMouseState(&mouse_x, &mouse_y);

			// �ж�����Ƿ��ڰ�ť����ͣ
			bool in_range_x = mouse_x >= position.x && mouse_x <= position.x + size.width;
			bool in_range_y = mouse_y >= position.y && mouse_y <= position.y + size.height;
			this->is_hovering = (in_range_x && in_range_y);

			// �����ͣ�ڰ�ť��
			if (is_hovering)
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

		// ��Ⱦ��ť
		void on_render(const Camera& camera) override
		{
			if (!current_texture) return;	// ���û��������������Ⱦ
			SDL_FRect rect_dst_win = { position.x, position.y, size.width, size.height };
			camera.render_texture(current_texture, nullptr, &rect_dst_win, 0, nullptr, false);
		}

		// ���������¼�
		void on_input(const SDL_Event& event) override
		{
			// ��ť�����
			if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT)
			{
				if (is_hovering)
				{
					if (effects["click"].second)
						Mix_PlayChannel(-1, effects["click"].second, 0);

					this->current_texture = effects["click"].first;
					this->on_click();
				}
			}
			// ��ť���ͷ�
			if (event.type == SDL_EVENT_MOUSE_BUTTON_UP && event.button.button == SDL_BUTTON_LEFT)
			{
				if (is_hovering)
					this->current_texture = effects["hover"].first;
				else
					this->current_texture = effects["normal"].first;
			}
		}

	private:
		Vector2 position;										// ��ťλ��
		Size size;												// ��ť��С
		bool is_first_hover = false;							// �Ƿ��״���ͣ
		SDL_Texture* current_texture = nullptr;					// ��ǰ��ʾ������
		std::unordered_map<std::string, effect_pair> effects;	// Ч���б�
		std::function<void()> on_click;							// ����ص�����
		bool is_hovering = false;								// �Ƿ���ͣ�ڰ�ť��
	};

	// �����������
	class Scene
	{
	public:
		Scene() = default;			// ���캯������ʼ��ʵ��������
		virtual ~Scene() = default;	// ������������ʵ��������

		// ��ȡ���飨����ָ������ģ��Ķ���ָ�룩
		template <typename _CvtTy = Sprite>
		_CvtTy* find_sprite(const std::string& name)
		{
			if (sprite_pool.find(name) == sprite_pool.end())
				throw custom_error(u8"SceneManager Error", u8"Sprite ��" + name + u8"�� is not found!");
			return (_CvtTy*)sprite_pool[name];
		}

		virtual void on_enter() = 0;											  // ���볡�����Խ�ɫ���Ե����ã�
		virtual void on_update(float delta) = 0;								  // ���³���
		virtual void on_render(const Camera& cam_game, const Camera& cam_ui) = 0; // ��Ⱦ������һ��������Ⱦ��Ϸ��һ��������ȾUI��
		virtual void on_input(const SDL_Event& event) = 0;						  // �����¼�����
		virtual void on_exit() = 0;												  // �˳���������Ҫ���ٶ���

	protected:
		// Ϊ������еĽ�ɫ�������򣬷���std::vector<Sprite*>
		std::vector<Sprite*> sort_sprite_pool()
		{
			std::vector<Sprite*> sorted_list;
			sorted_list.reserve(sprite_pool.size());

			for (auto& sprite : sprite_pool)
				sorted_list.push_back(sprite.second);

			//�Ȱ����꣬�ٰ���Ⱦ�㼶��������
			std::sort(sorted_list.begin(), sorted_list.end(), [](const Sprite* a, const Sprite* b)
				{
					if (a->get_render_layer() == b->get_render_layer())
						return a->get_position().y > b->get_position().y;
					else
						return a->get_render_layer() > b->get_render_layer();
				});

			return sorted_list;
		}

	protected:
		std::unordered_map<std::string, Sprite*> sprite_pool;	// �����
	};

	// ״̬�ڵ�
	class StateNode
	{
	public:
		StateNode() = default;
		~StateNode() = default;

		virtual void on_enter() {}					// ����״̬
		virtual void on_update(float delta) {}		// ����״̬
		virtual void on_exit() {}					// �˳�״̬
	};

// ===========================================================================================
// 				��Ϊ������						Behavior Controllers
// ===========================================================================================

	// ����������
	class AnimationPlayer
	{
	public:
		AnimationPlayer() = default;

		~AnimationPlayer()
		{
			for (auto& anim : animation_pool)
				delete anim.second;		// �ͷŶ�����Դ

			animation_pool.clear();
			current_animation = nullptr;
		}

		// ��Ӷ���
		void add_animation(const std::string& name, Animation* anim)
		{
			if (animation_pool.find(name) != animation_pool.end())
				throw custom_error(error_title, u8"Animation ��" + name + u8"�� is already exist!");

			animation_pool[name] = anim;
		}

		// �л�����
		void switch_to(const std::string& name)
		{
			if (animation_pool.find(name) == animation_pool.end())
				throw custom_error(error_title, u8"Animation ��" + name + u8"�� is not exist!");

			this->current_animation = animation_pool[name];
		}

		// ���õ�ǰ����������ͷ����
		void set_animation(const std::string& name)
		{
			if (animation_pool.find(name) == animation_pool.end())
				throw custom_error(error_title, u8"Animation ��" + name + u8"�� is not exist!");

			this->current_animation = animation_pool[name];
			this->current_animation->reset();
		}

		// ��ȡ��ǰ����
		Animation* get_current()
		{
			if (current_animation)
				return current_animation;
			else
				throw custom_error(error_title, u8"��current_animation�� is a nullptr!");
		}

		// ���¶���
		void on_update(float delta)
		{
			if (current_animation)
				current_animation->on_update(delta);
			else
				throw custom_error(error_title, u8"��current_animation�� is a nullptr!");
		}

		// ��Ⱦ����
		void on_render(const Camera& camera)
		{
			if (current_animation)
				current_animation->on_render(camera);
			else
				throw custom_error(error_title, u8"��current_animation�� is a nullptr!");
		}

	private:
		std::unordered_map<std::string, Animation*> animation_pool;		// ������
		Animation* current_animation = nullptr;							// ��ǰ����
		std::string error_title = u8"Animation Player Error";			// �������
	};

	// ״̬��
	class StateMachine
	{
	public:
		StateMachine() = default;

		~StateMachine()
		{
			for (auto& state : state_pool)
				delete state.second;		// �ͷ�״̬��������״̬�ڵ�

			state_pool.clear();
			current_state = nullptr;
		}

		// ����״̬��
		void on_update(float delta)
		{
			if (!current_state)
			{
				std::string info = u8"Current state is nullptr! Please check if the status node is initialized";
				throw custom_error(u8"StateMachine Error", info.c_str());
			}

			if (need_init)	// �����Ҫ��ʼ��״̬��
			{
				current_state->on_enter();
				need_init = false;		// ��ʼ�����
			}

			current_state->on_update(delta);
		}

		// ���ó�״̬�ڵ㣨���ڳ�ʼ����
		void set_entry(const std::string& name)
		{
			// ���δ�ҵ�Ŀ��״̬
			if (state_pool.find(name) == state_pool.end())
			{
				std::string info = u8"State ��" + name + u8"�� is not found!";
				throw custom_error(u8"StateMachine Error", info);
			}

			current_state = state_pool[name];
		}

		// �л�״̬�ڵ�
		void switch_to(const std::string& name)
		{
			// ���δ�ҵ�Ŀ��״̬
			if (state_pool.find(name) == state_pool.end())
			{
				std::string info = u8"State ��" + name + u8"�� is not found!";
				throw custom_error(u8"StateMachine Error", info);
			}

			if (current_state) current_state->on_exit();
			current_state = state_pool[name];
			current_state->on_enter();
		}

		// ע��״̬�ڵ�
		void register_state(const std::string& name, StateNode* state)
		{
			// �����״̬�Ѵ���
			if (state_pool.find(name) != state_pool.end())
			{
				std::string info = u8"State ��" + name + u8"�� is already exist!";
				throw custom_error(u8"StateMachine Error", info);
			}

			state_pool[name] = state;		// ע��״̬�ڵ�
		}

	private:
		bool need_init = true;									// �Ƿ���Ҫ��ʼ��״̬��
		StateNode* current_state = nullptr;						// ��ǰ״̬�ڵ�
		std::unordered_map<std::string, StateNode*> state_pool;	// ״̬��
	};

// =========================================================================================
//					�� �� ��								Managers
// =========================================================================================

	// ��Դ������
	class ResourcesManager
	{
	public:
		// ��ȡ��Դ����������
		static inline ResourcesManager* instance()
		{
			if (!m_instance)
				m_instance = new ResourcesManager();

			return m_instance;
		}

		// ������Դ
		inline void load_resources(SDL_Renderer* renderer, const char* _PathName)
		{
			// �ж��ļ��Ƿ����
			if (!std::filesystem::exists(_PathName))
			{
				std::string info = u8"Dictionary ��" + std::string(_PathName) + u8"�� is an error dictionary!";
				throw custom_error(u8"ResourcesManager Error", info);
			}

			// ����Ŀ���ļ��ڲ������ļ�
			for (const auto& entry : std::filesystem::recursive_directory_iterator(_PathName))
			{
				if (entry.is_regular_file())	// �������Ч�ļ�
				{
					const std::filesystem::path& path = entry.path();
					if (path.extension() == ".bmp" || path.extension() == ".png" || path.extension() == ".jpg")
					{
						SDL_Texture* texture = IMG_LoadTexture(renderer, path.u8string().c_str());
						texture_pool[path.stem().u8string()] = texture;
					}
					if (path.extension() == ".wav")
					{
						Mix_Chunk* audio = Mix_LoadWAV(path.u8string().c_str());
						audio_pool[path.stem().u8string()] = audio;
					}
					if (path.extension() == ".mp3" || path.extension() == ".ogg")
					{
						Mix_Music* music = Mix_LoadMUS(path.u8string().c_str());
						music_pool[path.stem().u8string()] = music;
					}
					if (path.extension() == ".ttf" || path.extension() == ".TTF")
					{
						TTF_Font* font = TTF_OpenFont(path.u8string().c_str(), 24);
						font_pool[path.stem().u8string()] = font;
					}
				}
			}
		}

		// ����������Դ
		inline SDL_Texture* find_texture(const std::string& name)
		{
			// δ�ҵ�
			if (texture_pool.find(name) == texture_pool.end())
			{
				std::string info = u8"Texture ��" + name + u8"�� is not found!";
				throw custom_error(u8"ResourcesManager Error", info);
			}

			return texture_pool[name];
		}

		// ������Ƶ��Դ
		inline Mix_Chunk* find_audio(const std::string& name)
		{
			// δ�ҵ�
			if (audio_pool.find(name) == audio_pool.end())
			{
				std::string info = u8"Audio ��" + name + u8"�� is not found!";
				throw custom_error(u8"ResourcesManager Error", info);
			}

			return audio_pool[name];
		}

		// ����������Դ
		inline Mix_Music* find_music(const std::string& name)
		{
			// δ�ҵ�
			if (music_pool.find(name) == music_pool.end())
			{
				std::string info = u8"Music ��" + name + u8"�� is not found!";
				throw custom_error(u8"ResourcesManager Error", info);
			}

			return music_pool[name];
		}

		// ����������Դ
		inline TTF_Font* find_font(const std::string& name)
		{
			// δ�ҵ�
			if (font_pool.find(name) == font_pool.end())
			{
				std::string info = u8"Font ��" + name + u8"�� is not found!";
				throw custom_error(u8"ResourcesManager Error", info);
			}

			return font_pool[name];
		}

		// ��ʾ�Ѽ��ص���Դ
		inline void debug_resources_list()
		{
			printf("=============================================================\n");
			printf("�Ѽ��ص�������Դ���� (��%d��):\n", (int)texture_pool.size());
			for (auto& texture : texture_pool)
				printf("%s\n", texture.first.c_str());

			printf("\n�Ѽ��ص���Ƶ��Դ���� (��%d��):\n", (int)audio_pool.size());
			for (auto& audio : audio_pool)
				printf("%s\n", audio.first.c_str());
			for (auto& music : music_pool)
				printf("%s\n", music.first.c_str());

			printf("\n�Ѽ��ص�������Դ���� (��%d��):\n", (int)font_pool.size());
			for (auto& font : font_pool)
				printf("%s\n", font.first.c_str());
			printf("=============================================================\n");
		}

	private:
		ResourcesManager() = default;
		~ResourcesManager() = default;

	private:
		static ResourcesManager* m_instance;							// ��Դ����������

		std::unordered_map<std::string, SDL_Texture*> texture_pool;		// ������Դ��
		std::unordered_map<std::string, Mix_Chunk*> audio_pool;			// ��Ƶ��Դ��
		std::unordered_map<std::string, Mix_Music*> music_pool;			// ������Դ��
		std::unordered_map<std::string, TTF_Font*> font_pool;			// ������Դ��
	};
	inline ResourcesManager* ResourcesManager::m_instance = nullptr;
	typedef ResourcesManager ResLoader;

	// ����������
	class SceneManager
	{
	public:
		// ��ȡ��������������
		static inline SceneManager* instance()
		{
			if (!m_instance)
				m_instance = new SceneManager();

			return m_instance;
		}

		// ���õ�ǰ����
		inline void set_current_scene(const std::string& name)
		{
			// ����������в����ڸó���
			if (scene_pool.find(name) == scene_pool.end())
				throw custom_error(u8"SceneManager Error", u8"Scene ��" + name + u8"�� is not exist!");

			current_scene = scene_pool[name];
			current_scene->on_enter();
		}

		// �л�����
		inline void switch_to(const std::string& name)
		{
			current_scene->on_exit();

			// ����������в����ڸó���
			if (scene_pool.find(name) == scene_pool.end())
				throw custom_error(u8"SceneManager Error", u8"Scene ��" + name + u8"�� is not exist!");

			current_scene = scene_pool[name];
			current_scene->on_enter();
		}

		// ����³���
		inline void add_scene(const std::string& name, Scene* scene)
		{
			if (scene_pool.find(name) != scene_pool.end())
				throw custom_error(u8"SceneManager Error", u8"Scene ��" + name + u8"�� is already exist!");
			scene_pool[name] = scene;
		}

		// ��ȡ����
		inline Scene* find_scene(const std::string& name)
		{
			// ����������в����ڸó���
			if (scene_pool.find(name) == scene_pool.end())
				throw custom_error(u8"SceneManager Error", u8"Scene ��" + name + u8"�� is not exist!");

			return scene_pool[name];
		}

		// ���µ�ǰ����
		void on_update(float delta)
		{
			if (current_scene)
				current_scene->on_update(delta);
			else
				throw custom_error(u8"SceneManager Error", u8"��current_scene�� is a nullptr!");
		}

		// ��Ⱦ��ǰ����
		void on_render(const Camera& camera_game, const Camera& camera_ui)
		{
			if (current_scene)
				current_scene->on_render(camera_game, camera_ui);
			else
				throw custom_error(u8"SceneManager Error", u8"��current_scene�� is a nullptr!");
		}

		// ���������¼�
		void on_input(const SDL_Event& event)
		{
			if (current_scene)
				current_scene->on_input(event);
			else
				throw custom_error(u8"SceneManager Error", u8"��current_scene�� is a nullptr!");
		}

	private:
		SceneManager() = default;
		~SceneManager() = default;

	private:
		static SceneManager* m_instance;					// ��������������
		Scene* current_scene = nullptr;						// ��ǰ����
		std::unordered_map<std::string, Scene*> scene_pool;	// ������
	};
	inline SceneManager* SceneManager::m_instance = nullptr;
	typedef SceneManager Director;

	// ��ײ������
	class CollisionManager
	{
	public:
		// ��ȡ��ײ����������
		static inline CollisionManager* instance()
		{
			if (!m_instance)
				m_instance = new CollisionManager();

			return m_instance;
		}

		// �����ײ��
		inline CollisionBox* create_collision_box()
		{
			CollisionBox* collision_box = new CollisionBox();
			collision_box_list.push_back(collision_box);
			return collision_box;
		}

		// ������ײ��
		inline void destroy_collision_box(CollisionBox* collision_box)
		{
			collision_box_list.erase(std::remove(collision_box_list.begin(),
				collision_box_list.end(), collision_box), collision_box_list.end());
			delete collision_box;
		}

		// ������ײ���
		inline void process_collide()
		{
			for (auto collision_box_src : collision_box_list)
			{
				if (!collision_box_src->enabled || collision_box_src->layer_dst == CollisionLayer::None)
					continue;

				for (auto collision_box_dst : collision_box_list)
				{
					if (!collision_box_dst->enabled || collision_box_src == collision_box_dst
						|| !(collision_box_src->layer_dst & collision_box_dst->layer_src))
						continue;

					// ������ײ����������ײ���maxX - ����ײ���minX <= ����ײ��Ŀ��֮��
					float max_x = std::max(collision_box_src->position.x + collision_box_src->size.width / 2.0f, collision_box_dst->position.x + collision_box_dst->size.width / 2.0f);
					float min_x = std::min(collision_box_src->position.x - collision_box_src->size.width / 2.0f, collision_box_dst->position.x - collision_box_dst->size.width / 2.0f);
					bool is_collide_x = (max_x - min_x <= collision_box_src->size.width + collision_box_dst->size.width);

					// ������ײ����������ײ���maxY - ����ײ���minY <= ����ײ��ĸ߶�֮��
					float max_y = std::max(collision_box_src->position.y + collision_box_src->size.height / 2.0f, collision_box_dst->position.y + collision_box_dst->size.height / 2.0f);
					float min_y = std::min(collision_box_src->position.y - collision_box_src->size.height / 2.0f, collision_box_dst->position.y - collision_box_dst->size.height / 2.0f);
					bool is_collide_y = (max_y - min_y <= collision_box_src->size.height + collision_box_dst->size.height);

					// �������/���򶼳�������Ŀ����ײ����ڻص���������ִ�лص�����
					if (is_collide_x && is_collide_y && collision_box_dst->on_collide)
						collision_box_dst->on_collide(collision_box_src->layer_src);
				}
			}
		}

		// ������ײ��
		inline void debug_collision_box(const Camera& camera)
		{
			for (auto collision_box : collision_box_list)
			{
				// ������ײ����Ⱦ��������
				Vector2 rect_pos = { collision_box->position.x - collision_box->size.width / 2.0f, collision_box->position.y - collision_box->size.height / 2.0f, };
				Size rect_size = { collision_box->size.width, collision_box->size.height };

				// ������ײ����Ⱦ��ɫ
				SDL_Color color;
				if (collision_box->enabled)
					color = { 255,195,195,255 };
				else
					color = { 115,155,175,255 };

				camera.render_shape(rect_pos, rect_size, color, false);	// ������ײ��
			}
		}

	private:
		CollisionManager() = default;
		~CollisionManager() = default;

	private:
		static CollisionManager* m_instance;			// ��ײ����������
		std::vector<CollisionBox*> collision_box_list;	// ��ײ���б�
	};
	inline CollisionManager* CollisionManager::m_instance = nullptr;
};
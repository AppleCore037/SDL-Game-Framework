#pragma once
#include "baseKits.h"
#include <SDL3/SDL.h>

class Camera;
class Sprite;

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
	Camera(SDL_Renderer* renderer, SDL_Window* window);

	~Camera() = default;

	// ��ȡ���������(���ĵ�)
	const Vector2& get_position() const;

	// �������������(���ĵ�)
	void set_position(const Vector2& pos);

	// �������������
	void set_zoom(float scale);

	// ���������
	void reset();

	// ���������
	void shake(float strength, float duration);

	// ��Ⱦ����
	void render_texture(SDL_Texture* texture, const SDL_FRect* rect_src, const SDL_FRect* rect_dst,
		double angle, const SDL_FPoint* center, bool is_flip) const;

	// ��Ⱦͼ�� -- ��
	void render_shape(const Vector2& begin, const Vector2& end, SDL_Color color) const;

	// ��Ⱦͼ�� -- ����
	void render_shape(const Vector2& pos, Size rectSize, SDL_Color color, bool is_filled) const;

	// ��Ⱦͼ�� -- Բ��
	void render_shape(const Vector2& pos, float radius, SDL_Color color, bool is_filled) const;

	// �����ɫ
	void look_at(const Sprite* sprite, int style);

	// ���������
	void on_update(float delta);

private:
	// ��ȡ��Ļ���ĵ�
	Vector2 get_screen_center() const;

	// ��ȡ��Ļ��С
	Size get_screen_size() const;

private:
	Vector2 position;					// ���������λ��
	Vector2 shake_position;				// ����λ��
	Vector2 base_position;				// ����λ��

	Timer timer_shake;					// ������ʱ��
	bool is_shaking = false;			// �Ƿ񶶶�
	float shaking_strength = 0;			// ��������
	float smooth_strength = 0;			// ƽ������
	float zoom = 1.0f;					// ���������

	SDL_Renderer* renderer = nullptr;	// ������Ⱦ��
	SDL_Window* window = nullptr;		// ���ô���

private:
	const float SMOOTH_FACTOR = 1.5f;	// ƽ��ϵ��
};

// ����������
class Sprite
{
public:
	Sprite() = default;
	~Sprite() = default;

	// ��ȡλ��
	const Vector2& get_position() const { return this->position; }

	// ����λ��
	void set_position(const Vector2& pos) { this->position = pos; }

	virtual void on_update(float delta) = 0;
	virtual void on_render(const Camera& camera) = 0;
	virtual void on_input(const SDL_Event& event) = 0;

protected:
	Vector2 position;		// λ��
	Vector2 velocity;		// �ٶ�
};
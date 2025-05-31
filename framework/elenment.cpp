#include "Elenment.h"
#include "utils.h"

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

// ��ȡ���������(���ĵ�)
const Vector2& Camera::get_position() const
{
	return position;
}

// �������������(���ĵ�)
void Camera::set_position(const Vector2& pos)
{
	this->base_position = pos;
}

// �������������
void Camera::set_zoom(float scale)
{
	this->zoom = scale;
}

// ���������
void Camera::reset()
{
	this->position = Vector2(0, 0);
	this->shake_position = Vector2(0, 0);
	this->base_position = Vector2(0, 0);
}

// ���������
void Camera::shake(float strength, float duration)
{
	is_shaking = true;
	shaking_strength = strength;

	timer_shake.set_wait_time(duration);
	timer_shake.restart();
}

// ��Ⱦ����
void Camera::render_texture(SDL_Texture* texture, const SDL_FRect* rect_src, const SDL_FRect* rect_dst,
	double angle, const SDL_FPoint* center, bool is_flip) const
{
	// ��Ⱦ���� = ��Ļ���ĵ� + (��ɫ�������� - ���������) * ����
	SDL_FRect rect_dst_win = *rect_dst;
	rect_dst_win.x = this->get_screen_center().x + (rect_dst_win.x - position.x) * zoom;
	rect_dst_win.y = this->get_screen_center().y + (rect_dst_win.y - position.y) * zoom;
	rect_dst_win.w = rect_dst_win.w * zoom; rect_dst_win.h = rect_dst_win.h * zoom;

	SDL_RenderTextureRotated(renderer, texture, rect_src, &rect_dst_win, angle, center,
		(is_flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE));
}

// ��Ⱦͼ�� -- ��
void Camera::render_shape(const Vector2& begin, const Vector2& end, SDL_Color color) const
{
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	Vector2 begin_dst_win = Vector2(begin.x - position.x, begin.y - position.y);
	Vector2 end_dst_win = Vector2(end.x - position.x, end.y - position.y);

	// ������������
	begin_dst_win.x = this->get_screen_center().x + begin_dst_win.x * zoom;
	begin_dst_win.y = this->get_screen_center().y + begin_dst_win.y * zoom;
	end_dst_win.x = this->get_screen_center().x + end_dst_win.x * zoom;
	end_dst_win.y = this->get_screen_center().y + end_dst_win.y * zoom;

	SDL_RenderLine(renderer, begin_dst_win.x, begin_dst_win.y, end_dst_win.x, end_dst_win.y);
}

// ��Ⱦͼ�� -- ����
void Camera::render_shape(const Vector2& pos, Size rectSize, SDL_Color color, bool is_filled) const
{
	// �����������
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	SDL_FRect rect_dst_win = { 0, 0, rectSize.width * zoom, rectSize.height * zoom };
	rect_dst_win.x = this->get_screen_center().x + (pos.x - position.x) * zoom;
	rect_dst_win.y = this->get_screen_center().y + (pos.y - position.y) * zoom;

	if (is_filled)
		SDL_RenderFillRect(renderer, &rect_dst_win);
	else
		SDL_RenderRect(renderer, &rect_dst_win);
}

// ��Ⱦͼ�� -- Բ��
void Camera::render_shape(const Vector2& pos, float radius, SDL_Color color, bool is_filled) const
{
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	Vector2 centerPos = Vector2(pos.x + radius, pos.y + radius);		// ����Բ����������

	// ����Բ������
	Vector2 pos_dst_win = Vector2(centerPos.x - position.x, centerPos.y - position.y);
	pos_dst_win.x = this->get_screen_center().x + pos_dst_win.x * zoom;
	pos_dst_win.y = this->get_screen_center().y + pos_dst_win.y * zoom;

	if (is_filled)
		utils::draw_FilledCircle(renderer, pos_dst_win.x, pos_dst_win.y, radius * zoom);
	else
		utils::draw_circle(renderer, pos_dst_win.x, pos_dst_win.y, radius * zoom);
}

// �����ɫ
void Camera::look_at(const Sprite* sprite, int style)
{
	if (style & None) return;

	Vector2 target_pos;		// Ŀ��λ��	
	target_pos.x = sprite->get_position().x;
	target_pos.y = sprite->get_position().y;

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

// ���������
void Camera::on_update(float delta)
{
	timer_shake.on_update(delta);
	smooth_strength = SMOOTH_FACTOR * delta;

	if (is_shaking)
	{
		shake_position.x = (-50 + rand() % 100) / 50.0f * shaking_strength;
		shake_position.y = (-50 + rand() % 100) / 50.0f * shaking_strength;
	}

	// ����λ�� = ����λ�� + ����λ��
	position = base_position + shake_position;
}

// ��ȡ��Ļ���ĵ�
Vector2 Camera::get_screen_center() const
{
	int screen_w, screen_h;
	SDL_GetWindowSize(window, &screen_w, &screen_h);		// ������Ļ���
	Vector2 screen_center = Vector2(screen_w / 2.0f, screen_h / 2.0f);	// ��ȡ��Ļ���ĵ�
	return screen_center;
}

// ��ȡ��Ļ��С
Size Camera::get_screen_size() const
{
	int screen_w, screen_h;
	SDL_GetWindowSize(window, &screen_w, &screen_h);		// ������Ļ���
	return Size(screen_w, screen_h);
}
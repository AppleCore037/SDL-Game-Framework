#include "utils.h"

// �Ƕ�ת����
float utils::deg_to_rad(float degree) noexcept
{
	return degree * PAI / 180.0f;
}

// ����ת�Ƕ�
float utils::rad_to_deg(float radian) noexcept
{
	return radian * 180.0f / PAI;
}

// ��ֵ����
float utils::lerp(float current, float target, float t) noexcept
{
	return current + (target - current) * t;
}

// �����������
float utils::distance_to(const Vector2& pos_1, const Vector2& pos_2)
{
	float dx = pos_1.x - pos_2.x;
	float dy = pos_1.y - pos_2.y;
	return sqrtf((dx * dx) + (dy * dy));
}

// ���ƿ���Բ
void utils::draw_circle(SDL_Renderer* renderer, float centerX, float centerY, float radius)
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
void utils::draw_FilledCircle(SDL_Renderer* renderer, float centerX, float centerY, float radius)
{
	for (int y = -radius; y <= radius; y++)
	{
		int x = sqrtf(radius * radius - y * y);
		SDL_RenderLine(renderer, centerX - x, centerY + y, centerX + x, centerY + y);
	}
}
#pragma once
#include "baseKits.h"
#include <SDL3/SDL.h>

// С����
namespace utils
{
	// �Ƕ�ת����
	float deg_to_rad(float degree) noexcept;

	// ����ת�Ƕ�
	float rad_to_deg(float radian) noexcept;

	// ��ֵ����
	float lerp(float current, float target, float t) noexcept;

	// �����������
	float distance_to(const Vector2& pos_1, const Vector2& pos_2);

	// ���ƿ���Բ
	void draw_circle(SDL_Renderer* renderer, float centerX, float centerY, float radius);

	// ����ʵ��Բ
	void draw_FilledCircle(SDL_Renderer* renderer, float centerX, float centerY, float radius);
}
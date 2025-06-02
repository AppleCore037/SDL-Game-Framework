#pragma once
#include "baseKits.h"
#include "graphics.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <string>
#include <unordered_map>
#include <functional>
#include <memory>

// ������
class Animation
{
public:
	Animation();

	~Animation() = default;

	//���ö���
	void reset();

	// ���ö���λ��
	void set_position(const Vector2& position);

	// ���ö�������
	void set_rotation(double angle);

	// ���ö������ĵ�
	void set_center(const SDL_FPoint& cneter);

	// ���ö����Ƿ�ѭ��
	void set_loop(bool is_loop);

	// ���ö���֡���
	void set_interval(float interval);

	// ���ö����Ƿ�ת
	void set_flip(bool flag);

	// ���ö����ص�����
	void set_on_finished(std::function<void()> finish_callback);

	// ���¶���
	void on_update(float delta);

	// �������֡
	void add_frame(SDL_Texture* texture, int num_h);

	// ��Ⱦ����
	void on_render(const Camera& camera) const;

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

// ����������
class AnimationPlayer
{
public:
	AnimationPlayer() = default;

	~AnimationPlayer();

	// ��Ӷ��� (��Ҫ���std::unique_ptr����)
	void add_animation(const std::string& name, std::unique_ptr<Animation> anim);

	// �л�����
	void switch_to(const std::string& name);

	// ���õ�ǰ����
	void set_animation(const std::string& name);

	// ��ȡ��ǰ����
	Animation* get_current();

	// ���¶���
	void on_update(float delta);

	// ��Ⱦ����
	void on_render(const Camera& camera);

private:
	// ��ʾ������Ϣ
	void show_error(const std::string& info);

private:
	std::unordered_map<std::string, std::unique_ptr<Animation>> animation_pool;		// ������
	Animation* current_animation = nullptr;											// ��ǰ����
};
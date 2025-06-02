#include "animation.h"

// =====================================================================
//			�� �� ��						A n i m a t i o n
// =====================================================================

Animation::Animation()
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

//���ö���
void Animation::reset()
{
	timer.restart();
	idx_frame = 0;
}

// ���ö���λ��
void Animation::set_position(const Vector2& pos)
{
	this->position = pos;
}

// ���ö�������
void Animation::set_rotation(double angle)
{
	this->angle = angle;
}

// ���ö������ĵ�
void Animation::set_center(const SDL_FPoint& center)
{
	this->center = center;
}

// ���ö����Ƿ�ѭ��
void Animation::set_loop(bool is_loop)
{
	this->is_loop = is_loop;
}

// ���ö���֡���
void Animation::set_interval(float interval)
{
	timer.set_wait_time(interval);
}

// ���ö����Ƿ�ת
void Animation::set_flip(bool flag)
{
	this->is_flip = flag;
}

// ���ö����ص�����
void Animation::set_on_finished(std::function<void()> finish_callback)
{
	on_finished = finish_callback;
}

// ���¶���
void Animation::on_update(float delta)
{
	timer.on_update(delta);
}

// �������֡
void Animation::add_frame(SDL_Texture* texture, int num_h)
{
	float width, height;
	SDL_GetTextureSize(texture, &width, &height);	// ��ȡ����֡���

	float width_frame = width / num_h;	// ��ȡ��������֡���
	for (int i = 0; i < num_h; i++)
	{
		SDL_FRect rect_src;
		rect_src.x = i * width_frame, rect_src.y = 0;
		rect_src.w = width_frame, rect_src.h = height;

		frame_list.emplace_back(texture, rect_src);		// ������֡�����б�
	}
}

// ��Ⱦ����
void Animation::on_render(const Camera& camera) const
{
	const Frame& frame = frame_list[idx_frame];
	const Vector2& pos_camera = camera.get_position();

	SDL_FRect rect_dst;
	rect_dst.x = position.x - frame.rect_src.w / 2.0f;
	rect_dst.y = position.y - frame.rect_src.h / 2.0f;
	rect_dst.w = frame.rect_src.w;
	rect_dst.h = frame.rect_src.h;

	camera.render_texture(frame.texture, &frame.rect_src, &rect_dst,
		angle, &center, is_flip);
}


// =====================================================================
//		 ������������					Animation  Player
// =====================================================================

AnimationPlayer::~AnimationPlayer()
{
	for (auto& anim : animation_pool)
		anim.second.reset();		// �ͷŶ�����Դ

	animation_pool.clear();
	current_animation = nullptr;
}

// ��Ӷ��� (��Ҫ���std::unique_ptr����)
void AnimationPlayer::add_animation(const std::string& name, std::unique_ptr<Animation> anim)
{
	if (animation_pool.find(name) != animation_pool.end())
		this->show_error(u8"Animation ��" + name + u8"�� is already exist!");

	animation_pool[name] = std::move(anim);
}

// �л�����
void AnimationPlayer::switch_to(const std::string& name)
{
	if (animation_pool.find(name) == animation_pool.end())
		this->show_error(u8"Animation ��" + name + u8"�� is not exist!");

	this->current_animation = animation_pool[name].get();
}

// ���õ�ǰ����
void AnimationPlayer::set_animation(const std::string& name)
{
	if (animation_pool.find(name) == animation_pool.end())
		this->show_error(u8"Animation ��" + name + u8"�� is not exist!");

	this->current_animation = animation_pool[name].get();
	this->current_animation->reset();
}

// ��ȡ��ǰ����
Animation* AnimationPlayer::get_current()
{
	if (current_animation)
		return current_animation;
	else
		this->show_error(u8"��current_animation�� is a nullptr!");
}

// ���¶���
void AnimationPlayer::on_update(float delta)
{
	if (current_animation)
		current_animation->on_update(delta);
	else
		this->show_error(u8"��current_animation�� is a nullptr!");
}

// ��Ⱦ����
void AnimationPlayer::on_render(const Camera& camera)
{
	if (current_animation)
		current_animation->on_render(camera);
	else
		this->show_error(u8"��current_animation�� is a nullptr!");
}

// ��ʾ������Ϣ
void AnimationPlayer::show_error(const std::string& info)
{
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, u8"Animation Player Error", info.c_str(), nullptr);
	exit(-1);
}
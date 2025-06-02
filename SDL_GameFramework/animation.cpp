#include "animation.h"

// =====================================================================
//			动 画 类						A n i m a t i o n
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

//重置动画
void Animation::reset()
{
	timer.restart();
	idx_frame = 0;
}

// 设置动画位置
void Animation::set_position(const Vector2& pos)
{
	this->position = pos;
}

// 设置动画方向
void Animation::set_rotation(double angle)
{
	this->angle = angle;
}

// 设置动画中心点
void Animation::set_center(const SDL_FPoint& center)
{
	this->center = center;
}

// 设置动画是否循环
void Animation::set_loop(bool is_loop)
{
	this->is_loop = is_loop;
}

// 设置动画帧间隔
void Animation::set_interval(float interval)
{
	timer.set_wait_time(interval);
}

// 设置动画是否翻转
void Animation::set_flip(bool flag)
{
	this->is_flip = flag;
}

// 设置动画回调函数
void Animation::set_on_finished(std::function<void()> finish_callback)
{
	on_finished = finish_callback;
}

// 更新动画
void Animation::on_update(float delta)
{
	timer.on_update(delta);
}

// 添加序列帧
void Animation::add_frame(SDL_Texture* texture, int num_h)
{
	float width, height;
	SDL_GetTextureSize(texture, &width, &height);	// 获取序列帧宽高

	float width_frame = width / num_h;	// 获取单张序列帧宽高
	for (int i = 0; i < num_h; i++)
	{
		SDL_FRect rect_src;
		rect_src.x = i * width_frame, rect_src.y = 0;
		rect_src.w = width_frame, rect_src.h = height;

		frame_list.emplace_back(texture, rect_src);		// 将序列帧加入列表
	}
}

// 渲染动画
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
//		 动画播放器类					Animation  Player
// =====================================================================

AnimationPlayer::~AnimationPlayer()
{
	for (auto& anim : animation_pool)
		anim.second.reset();		// 释放动画资源

	animation_pool.clear();
	current_animation = nullptr;
}

// 添加动画 (需要添加std::unique_ptr类型)
void AnimationPlayer::add_animation(const std::string& name, std::unique_ptr<Animation> anim)
{
	if (animation_pool.find(name) != animation_pool.end())
		this->show_error(u8"Animation “" + name + u8"” is already exist!");

	animation_pool[name] = std::move(anim);
}

// 切换动画
void AnimationPlayer::switch_to(const std::string& name)
{
	if (animation_pool.find(name) == animation_pool.end())
		this->show_error(u8"Animation “" + name + u8"” is not exist!");

	this->current_animation = animation_pool[name].get();
}

// 设置当前动画
void AnimationPlayer::set_animation(const std::string& name)
{
	if (animation_pool.find(name) == animation_pool.end())
		this->show_error(u8"Animation “" + name + u8"” is not exist!");

	this->current_animation = animation_pool[name].get();
	this->current_animation->reset();
}

// 获取当前动画
Animation* AnimationPlayer::get_current()
{
	if (current_animation)
		return current_animation;
	else
		this->show_error(u8"“current_animation” is a nullptr!");
}

// 更新动画
void AnimationPlayer::on_update(float delta)
{
	if (current_animation)
		current_animation->on_update(delta);
	else
		this->show_error(u8"“current_animation” is a nullptr!");
}

// 渲染动画
void AnimationPlayer::on_render(const Camera& camera)
{
	if (current_animation)
		current_animation->on_render(camera);
	else
		this->show_error(u8"“current_animation” is a nullptr!");
}

// 显示错误信息
void AnimationPlayer::show_error(const std::string& info)
{
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, u8"Animation Player Error", info.c_str(), nullptr);
	exit(-1);
}
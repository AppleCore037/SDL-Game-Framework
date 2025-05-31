#pragma once
#include "baseKits.h"
#include "elenment.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <string>
#include <unordered_map>
#include <functional>
#include <memory>

// 动画类
class Animation
{
public:
	Animation();

	~Animation() = default;

	//重置动画
	void reset();

	// 设置动画位置
	void set_position(const Vector2& position);

	// 设置动画方向
	void set_rotation(double angle);

	// 设置动画中心点
	void set_center(const SDL_FPoint& cneter);

	// 设置动画是否循环
	void set_loop(bool is_loop);

	// 设置动画帧间隔
	void set_interval(float interval);

	// 设置动画是否翻转
	void set_flip(bool flag);

	// 设置动画回调函数
	void set_on_finished(std::function<void()> finish_callback);

	// 更新动画
	void on_update(float delta);

	// 添加序列帧
	void add_frame(SDL_Texture* texture, int num_h);

	// 渲染动画
	void on_render(const Camera& camera) const;

private:
	// 序列帧结构体
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
	Vector2 position;					// 位置
	double angle = 0;					// 角度
	SDL_FPoint center = { 0 };			// 中心点
	bool is_flip = false;				// 是否反转

	Timer timer;						// 内置计时器
	bool is_loop = true;				// 是否循环
	size_t idx_frame = 0;				// 当前帧索引
	std::vector<Frame> frame_list;		// 序列帧列表
	std::function<void()> on_finished;	// 动画播放完毕回调函数
};

// 动画播放器
class AnimationPlayer
{
public:
	AnimationPlayer() = default;

	~AnimationPlayer();

	// 添加动画 (需要添加std::unique_ptr类型)
	void add_animation(const std::string& name, std::unique_ptr<Animation> anim);

	// 切换动画
	void switch_to(const std::string& name);

	// 设置当前动画
	void set_animation(const std::string& name);

	// 获取当前动画
	Animation* get_current();

	// 更新动画
	void on_update(float delta);

	// 渲染动画
	void on_render(const Camera& camera);

private:
	// 显示错误信息
	void show_error(const std::string& info);

private:
	std::unordered_map<std::string, std::unique_ptr<Animation>> animation_pool;		// 动画池
	Animation* current_animation = nullptr;											// 当前动画
};
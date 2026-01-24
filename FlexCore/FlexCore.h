#pragma once

#include "fce_base.h"
#include "fce_elements.h"
#include "fce_handler.h"
#include "fce_kits.h"
#include "fce_property.h"
#include "fce_utils.h"

class DefaultScene : public fce::Scene
{
public:
	DefaultScene() = default;
	~DefaultScene() = default;
};

namespace fce
{
	// 框架类
	class Framework
	{
	public:
		Framework() = default;
		~Framework() = default;

		// 初始化图形系统
		void Init_Graphic(const std::string& title, int w, int h, WindowFlags flags = WindowFlags::Default);

		// 主循环
		void main_loop(int fps);

		// 释放图形系统
		void Release_Graphic();

	private:
		Camera2D* default_camera = nullptr;
	};
};
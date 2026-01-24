#include "fce_utils.h"

// ======================================= custom_error =======================================

// 获取错误标题
const char* fce::custom_error::title() const noexcept
{
	return error_title.c_str();
}

// 获取错误信息
const char* fce::custom_error::what() const noexcept
{
	return error_info.c_str();
}

// ======================================= Random =======================================

fce::Random fce::Random::m_instance;

fce::Random::Random()
{
	engine.seed(std::random_device()());
}

// 生成 [min, max] 范围内的随机整数
int fce::Random::randint(int min, int max)
{
	if (min > max) std::swap(min, max);
	std::uniform_int_distribution<int> dist(min, max);
	return dist(m_instance.engine);
}

// 生成 [min, max] 范围内的随机浮点数
float fce::Random::randfloat(float min, float max)
{
	if (min > max) std::swap(min, max);
	std::uniform_real_distribution<float> dist(min, max);
	return dist(m_instance.engine);
}

// ======================================= Clock =======================================

fce::Clock fce::Clock::m_instance;

fce::Clock::Clock()
{
	this->last_time = clock_t::now();
	this->global_start_time = clock_t::now();
}

// 获取屏幕刷新率（获取失败默认返回60）
int fce::Clock::get_screen_refreshRate() const
{
	// 获取主显示器（默认显示器）
	SDL_DisplayID display = SDL_GetPrimaryDisplay();
	if (!display) return 0;

	// 获取当前显示模式
	const SDL_DisplayMode* mode = SDL_GetCurrentDisplayMode(display);
	if (!mode) return 0;

	// 返回刷新率（如果有效，否则返回默认值60）
	return (mode->refresh_rate > 0) ? (int)mode->refresh_rate : 60;
}

// 起始当前帧计时
void fce::Clock::start_frame()
{
	clock_t::time_point cur_time = clock_t::now();
	m_instance.delta_time = static_cast<double>(std::chrono::duration_cast<ms_t>(
		cur_time - m_instance.last_time).count());
	m_instance.last_time = cur_time;
}

// 结束当前帧计时
void fce::Clock::end_frame()
{
	auto cur_time = clock_t::now();
	auto elp_time = std::chrono::duration_cast<ms_t>(cur_time - m_instance.last_time).count(); // 经过时间

	// 如果"经过时间小于帧间隔" && "启用帧率限制"就休眠
	if (elp_time < m_instance.target_dur && m_instance.is_frame_limited)
		std::this_thread::sleep_for(ms_t(m_instance.target_dur - elp_time));
	else
		std::this_thread::sleep_for(ms_t(m_instance.MIN_DUR - elp_time));
}

// 设置是否垂直同步（默认false）
void fce::Clock::set_VSync(bool is_abled)
{
	if (is_abled)
		m_instance.target_dur = 1000 / m_instance.get_screen_refreshRate();
	else
		m_instance.target_dur = 1000 / m_instance.target_fps;	// 设置为目标FPS
}

// 设置FPS
void fce::Clock::set_fps(int fps_limit)
{
	m_instance.target_fps = fps_limit;		// 设置目标FPS
	m_instance.target_dur = 1000 / fps_limit;
}

// 获取全局经过时间（秒）
float fce::Clock::get_global_time()
{
	auto global_current_time = clock_t::now();
	// 转换为秒
	return static_cast<float>(std::chrono::duration_cast<ms_t>(
		global_current_time - m_instance.global_start_time).count()) * 0.001f;
}

// 重置全局经过时间
void fce::Clock::restart_global_time()
{
	m_instance.global_start_time = clock_t::now();
}

// 设置是否启用帧率限制（默认是true）
void fce::Clock::set_frame_limit(bool flag)
{
	m_instance.is_frame_limited = flag;
}

// 获取FPS
int fce::Clock::get_fps()
{
	return static_cast<int>(1000 / m_instance.delta_time);
}

// 获取帧间隔
float fce::Clock::get_DeltaTime()
{
	return static_cast<float>((m_instance.delta_time / 1000.0f) * m_instance.time_scale);
}

// 设置时间缩放
void fce::Clock::set_time_scale(double scale)
{
	if (scale <= 0.0f) m_instance.time_scale = 0.1f;
	else m_instance.time_scale = scale;
}

// ======================================= Data =======================================

fce::Data fce::Data::m_instance;

fce::Data::Data()
{
	m_instance.init_type_list();
}

// 清空文件
void fce::Data::clear(const std::string& file_name)
{
	std::ofstream writer = std::ofstream(file_name + ".kvp", SvMod::Truncate);
	writer.close();
}

// 初始化类型支持表
void fce::Data::init_type_list()
{
	m_instance.type_list.push_back(typeid(int).name());
	m_instance.type_list.push_back(typeid(float).name());
	m_instance.type_list.push_back(typeid(double).name());
	m_instance.type_list.push_back(typeid(bool).name());
	m_instance.type_list.push_back(typeid(char).name());
	m_instance.type_list.push_back(typeid(long long).name());
	m_instance.type_list.push_back("string");
	m_instance.type_list.push_back(typeid(unsigned long long).name());
	m_instance.type_list.push_back(typeid(unsigned int).name());
}

// 保存数据
template<typename _Ty>
void fce::Data::save(const std::string& fname, const std::string& key, _Ty value, SvMod mod)
{
	std::string _Type = (typeid(_Ty) == typeid(std::string) ? "string" : typeid(_Ty).name()); // 判断类型
	std::string info = key + ":" + _Type + "=";		// 补全格式

	// 判断类型是否支持
	auto finder = std::find(m_instance.type_list.begin(), m_instance.type_list.end(), _Type);
	if (finder == m_instance.type_list.end())
	{
		std::cerr << "DataSave Error: Unsupported data type!" << std::endl;
		return;
	}

	// 保存写入
	std::ofstream writer = std::ofstream(fname + ".kvp", mod);
	writer << info << value << std::endl;
	writer.close();
}

// 读取数据
template<typename _Ty>
_Ty fce::Data::read(const std::string& file_name, const std::string& key)
{
	// 补全目标类型
	std::string tar_type = (typeid(_Ty) == typeid(std::string) ? "string" : typeid(_Ty).name());
	std::ifstream reader = std::ifstream(file_name + ".kvp", std::ios::in);

	if (!reader.is_open())	// 无法打开
	{
		std::cerr << "DataRead Error: Cannot open file “" + file_name + ".kvp”!" << std::endl;
		return _Ty();
	}

	// 类型不支持
	auto finder = std::find(m_instance.type_list.begin(), m_instance.type_list.end(), tar_type);
	if (finder == m_instance.type_list.end())
	{
		std::cerr << "DataRead Error: Unsupported data type!" << std::endl;
		return _Ty();
	}

	// 开始读取
	std::string line;
	while (reader >> line)
	{
		std::string _Key = line.substr(0, line.find(':'));	// 提取键
		std::string _Type = line.substr(line.find(':') + 1, line.find('=') - line.find(':') - 1); // 提取类型

		if (_Key == key && _Type == tar_type)	// 判断键与类型是否都匹配
		{
			std::string _Val = line.substr(line.find('=') + 1);
			return m_instance.type_conversion<_Ty>(_Val);	// 转换类型
		}
	}

	std::cerr << "DataRead Error: Not found “" + key + "” in the file: " + file_name + ".kvp!" << std::endl;
	return _Ty();
}

// 基础类型转换
template<typename _Ty>
_Ty fce::Data::type_conversion(const std::string& val)
{
	if constexpr (std::is_same_v<_Ty, std::string>) return val;
	if constexpr (std::is_same_v<_Ty, bool>) return (val == "1" || val == "true");
	if constexpr (std::is_same_v<_Ty, char>) return val[0];
	if constexpr (std::is_same_v<_Ty, int>) return std::stoi(val);
	if constexpr (std::is_same_v<_Ty, float>) return std::stof(val);
	if constexpr (std::is_same_v<_Ty, double>) return std::stod(val);
	if constexpr (std::is_same_v<_Ty, long long>) return std::stoll(val);
	if constexpr (std::is_same_v<_Ty, unsigned long long>) return std::stoull(val);
	if constexpr (std::is_same_v<_Ty, unsigned int>) return std::stoul(val);
}

// ======================================= maths =======================================

// 角度转弧度
float fce::maths::deg_to_rad(float degree) noexcept
{
	return degree * PI / 180.0f;
}

// 弧度转角度
float fce::maths::rad_to_deg(float radian) noexcept
{
	return radian * 180.0f / PI;
}

// 插值函数
float fce::maths::lerp(float current, float target, float t) noexcept
{
	return current + (target - current) * t;
}

// 计算两点距离
float fce::maths::distance_to(const Vector2& pos_1, const Vector2& pos_2)
{
	float dx = pos_1.x - pos_2.x;
	float dy = pos_1.y - pos_2.y;
	return sqrtf((dx * dx) + (dy * dy));
}

// 摆动函数
float fce::maths::swing(float min, float max, float strength)
{
	if (max < min) std::swap(min, max);
	if (strength < 0) strength = -strength;

	float dt = Clock::get_global_time();
	// 摆动区间 = (max-min)÷2 * sin(dt*k) + (max+min)÷2
	return (max - min) / 2.0f * std::sin(dt * strength) + (max + min) / 2.0f;
}
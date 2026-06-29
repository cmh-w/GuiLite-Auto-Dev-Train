#pragma once

// 实时任务周期（毫秒）
#define REAL_TIME_TASK_CYCLE_MS		50
// 最大值宏定义
#define MAX(a,b) (((a)>(b))?(a):(b))
// 最小值宏定义
#define MIN(a,b) (((a)<(b))?(a):(b))

// 颜色宏定义：将ARGB分量合并为32位颜色值
#define GL_ARGB(a, r, g, b) ((((unsigned int)(a)) << 24) | (((unsigned int)(r)) << 16) | (((unsigned int)(g)) << 8) | ((unsigned int)(b)))
// 提取ARGB颜色中的Alpha分量
#define GL_ARGB_A(rgb) ((((unsigned int)(rgb)) >> 24) & 0xFF)

// 颜色宏定义：将RGB分量合并为32位颜色值（Alpha固定为0xFF）
#define GL_RGB(r, g, b) ((0xFF << 24) | (((unsigned int)(r)) << 16) | (((unsigned int)(g)) << 8) | ((unsigned int)(b)))
// 提取RGB颜色中的红色分量
#define GL_RGB_R(rgb) ((((unsigned int)(rgb)) >> 16) & 0xFF)
// 提取RGB颜色中的绿色分量
#define GL_RGB_G(rgb) ((((unsigned int)(rgb)) >> 8) & 0xFF)
// 提取RGB颜色中的蓝色分量
#define GL_RGB_B(rgb) (((unsigned int)(rgb)) & 0xFF)
// 32位颜色转换为16位颜色
#define GL_RGB_32_to_16(rgb) (((((unsigned int)(rgb)) & 0xFF) >> 3) | ((((unsigned int)(rgb)) & 0xFC00) >> 5) | ((((unsigned int)(rgb)) & 0xF80000) >> 8))
// 16位颜色转换为32位颜色
#define GL_RGB_16_to_32(rgb) ((0xFF << 24) | ((((unsigned int)(rgb)) & 0x1F) << 3) | ((((unsigned int)(rgb)) & 0x7E0) << 5) | ((((unsigned int)(rgb)) & 0xF800) << 8))

// 水平居中对齐标志
#define ALIGN_HCENTER		0x00000000L
// 左对齐标志
#define ALIGN_LEFT			0x01000000L
// 右对齐标志
#define ALIGN_RIGHT			0x02000000L
// 水平对齐掩码
#define ALIGN_HMASK			0x03000000L

// 垂直居中对齐标志
#define ALIGN_VCENTER		0x00000000L
// 顶部对齐标志
#define ALIGN_TOP			0x00100000L
// 底部对齐标志
#define ALIGN_BOTTOM		0x00200000L
// 垂直对齐掩码
#define ALIGN_VMASK			0x00300000L

// 时间结构体：包含年月日时分秒
typedef struct
{
	unsigned short year;
	unsigned short month;
	unsigned short date;
	unsigned short day;
	unsigned short hour;
	unsigned short minute;
	unsigned short second;
}T_TIME;

// 注册调试函数：设置断言和日志输出回调
void register_debug_function(void(*my_assert)(const char* file, int line), void(*my_log_out)(const char* log));
// 断言函数：触发断言时记录文件和行号
void _assert(const char* file, int line);
#define ASSERT(condition)	\
	do{                     \
	if(!(condition))_assert(__FILE__, __LINE__);\
	}while(0)
// 日志输出函数
void log_out(const char* log);

// 获取当前时间（秒数）
long get_time_in_second();
// 秒数转换为日期时间结构
T_TIME second_to_day(long second);
// 获取当前日期时间
T_TIME get_time();

// 启动实时定时器
void start_real_timer(void (*func)(void* arg));
// 注册定时器：指定周期和回调函数
void register_timer(int milli_second, void func(void* param), void* param);

// 获取当前线程ID
unsigned int get_cur_thread_id();
// 创建新线程
void create_thread(unsigned long* thread_id, void* attr, void *(*start_routine) (void *), void* arg);
// 线程休眠指定毫秒数
void thread_sleep(unsigned int milli_seconds);
// 构建BMP图像文件
int build_bmp(const char *filename, unsigned int width, unsigned int height, unsigned char *data);

#define FIFO_BUFFER_LEN		1024
// FIFO缓冲区类：用于线程间数据传递的循环队列
class c_fifo
{
public:
	// 构造函数：初始化FIFO缓冲区
	c_fifo();
	// 从FIFO读取数据
	int read(void* buf, int len);
	// 向FIFO写入数据
	int write(void* buf, int len);
private:
	unsigned char 	m_buf[FIFO_BUFFER_LEN];
	int		m_head;
	int		m_tail;
	void* m_read_sem;
	void* m_write_mutex;
};

// 矩形类：表示二维矩形区域，提供碰撞检测和尺寸计算
class c_rect
{
public:
	// 构造函数：创建空矩形（所有坐标为-1）
	c_rect(){ m_left = m_top = m_right = m_bottom = -1; }//empty rect
	// 构造函数：创建指定位置和大小的矩形
	c_rect(int left, int top, int width, int height)
	{
		set_rect(left, top, width, height);
	}
	// 设置矩形的位置和尺寸
	void set_rect(int left, int top, int width, int height)
	{
		ASSERT(width > 0 && height > 0);
		m_left = left;
		m_top = top;
		m_right = left + width - 1;
		m_bottom = top + height -1;
	}
	// 判断点是否在矩形区域内
	bool pt_in_rect(int x, int y) const
	{
		return x >= m_left && x <= m_right && y >= m_top && y <= m_bottom;
	}
	// 比较两个矩形是否相等
	int operator==(const c_rect& rect) const
	{
		return (m_left == rect.m_left) && (m_top == rect.m_top) && (m_right == rect.m_right) && (m_bottom == rect.m_bottom);
	}
	int width() const { return m_right - m_left + 1; }
	int height() const { return m_bottom - m_top + 1 ; }

	int	    m_left;
	int     m_top;
	int     m_right;
	int     m_bottom;
};

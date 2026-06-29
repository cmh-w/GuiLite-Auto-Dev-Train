#ifdef GUILITE_ON
#if (defined __none_os__) || ((!defined _WIN32) && (!defined WIN32) && (!defined _WIN64) && (!defined WIN64) && (!defined __linux__) && (!defined __APPLE__))

#include "../../core/api.h"
#include <stdio.h>

static void(*do_assert)(const char* file, int line);
static void(*do_log_out)(const char* log);
// 注册调试函数：设置断言和日志输出的回调
void register_debug_function(void(*my_assert)(const char* file, int line), void(*my_log_out)(const char* log))
{
	do_assert = my_assert;
	do_log_out = my_log_out;
}

// 断言失败处理：调用回调或进入死循环
void _assert(const char* file, int line)
{
	if(do_assert)
	{
		do_assert(file, line);
	}
	while(1);
}

// 日志输出：将日志信息输出到回调
void log_out(const char* log)
{
	if (do_log_out)
	{
		do_log_out(log);
	}
}

// 获取当前时间（秒数）：无操作系统时返回0
long get_time_in_second()
{
	return 0;
}

// 秒数转时间结构：无操作系统时返回空结构
T_TIME second_to_day(long second)
{
	T_TIME ret = {0};
	return ret;
}

// 获取本地时间：无操作系统时返回空结构
T_TIME get_time()
{
	T_TIME ret = {0};
	return ret;
}

// 启动实时定时器：当前平台不支持
void start_real_timer(void (*func)(void* arg))
{
	log_out("Not support now");
}

// 注册定时器：当前平台不支持
void register_timer(int milli_second, void func(void* ptmr, void* parg))
{
	log_out("Not support now");
}

// 获取当前线程ID：当前平台不支持
unsigned int get_cur_thread_id()
{
	log_out("Not support now");
	return 0;
}

// 创建线程：当前平台不支持
void create_thread(unsigned long* thread_id, void* attr, void *(*start_routine) (void *), void* arg)
{
	log_out("Not support now");
}

extern "C" void delay_ms(unsigned short nms);
// 线程睡眠：调用MCU级毫秒延时函数
void thread_sleep(unsigned int milli_seconds)
{//MCU alway implemnet driver code in APP.
	delay_ms(milli_seconds);
}

// 构建BMP图像文件：当前平台不支持
int build_bmp(const char *filename, unsigned int width, unsigned int height, unsigned char *data)
{
	log_out("Not support now");
	return 0;
}

// FIFO构造函数：初始化读写指针，无同步对象
c_fifo::c_fifo()
{
	m_head = m_tail = 0;
	m_read_sem = m_write_mutex = 0;
}

// FIFO读取：从缓冲区读取指定字节数（无锁）
int c_fifo::read(void* buf, int len)
{
	unsigned char* pbuf = (unsigned char*)buf;
	int i = 0;
	while(i < len)
	{
		if (m_tail == m_head)
		{//empty
			continue;
		}
		*pbuf++ = m_buf[m_head];
		m_head = (m_head + 1) % FIFO_BUFFER_LEN;
		i++;
	}
	if(i != len)
	{
		ASSERT(false);
	}
	return i;
}

// FIFO写入：将数据写入缓冲区（满时返回0并告警，无锁）
int c_fifo::write(void* buf, int len)
{
	unsigned char* pbuf = (unsigned char*)buf;
	int i = 0;
	int tail = m_tail;

	while(i < len)
	{
		if ((m_tail + 1) % FIFO_BUFFER_LEN == m_head)
		{//full, clear data has been written;
			m_tail = tail;
			log_out("Warning: fifo full\n");
			return 0;
		}
		m_buf[m_tail] = *pbuf++;
		m_tail = (m_tail + 1) % FIFO_BUFFER_LEN;
		i++;
	}

	if(i != len)
	{
		ASSERT(false);
	}
	return i;
}

#endif
#endif

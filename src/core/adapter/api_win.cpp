#ifdef GUILITE_ON
#if (defined _WIN32) || (defined WIN32) || (defined _WIN64) || (defined WIN64)

#include "../../core/api.h"
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <conio.h>
#include <windows.h>
#include <assert.h>

#define MAX_TIMER_CNT 10
#define TIMER_UNIT 50//ms

static void(*do_assert)(const char* file, int line);
static void(*do_log_out)(const char* log);
// 注册调试函数：设置断言和日志输出的回调
void register_debug_function(void(*my_assert)(const char* file, int line), void(*my_log_out)(const char* log))
{
	do_assert = my_assert;
	do_log_out = my_log_out;
}

// 断言失败处理：输出错误信息并触发断言
void _assert(const char* file, int line)
{
	static char s_buf[192];
	if (do_assert) 
	{
		do_assert(file, line);
	}
	else
	{
		memset(s_buf, 0, sizeof(s_buf));
		sprintf_s(s_buf, sizeof(s_buf), "vvvvvvvvvvvvvvvvvvvvvvvvvvvv\n\nAssert@ file = %s, line = %d\n\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n", file, line);
		OutputDebugStringA(s_buf);
		printf("%s", s_buf);
		fflush(stdout);
		assert(false);
	}
}

// 日志输出：将日志信息输出到回调或标准输出
void log_out(const char* log)
{
	if (do_log_out)
	{
		do_log_out(log);
	}
	else
	{
		printf("%s", log);
		fflush(stdout);
		OutputDebugStringA(log);
	}
}

typedef struct _timer_manage
{
	struct  _timer_info
	{
		int state; /* on or off */
		int interval;
		int elapse; /* 0~interval */
		void (* timer_proc) (void* param);
		void* param;
	}timer_info[MAX_TIMER_CNT];

	void (* old_sigfunc)(int);
	void (* new_sigfunc)(int);
}_timer_manage_t;
static struct _timer_manage timer_manage;

// 定时器线程：循环检查各定时器的状态并触发回调
DWORD WINAPI timer_routine(LPVOID lpParam)
{
	int i;
	while(true)
	{
		for(i = 0; i < MAX_TIMER_CNT; i++)
		{
			if(timer_manage.timer_info[i].state == 0)
			{
				continue;
			}
			timer_manage.timer_info[i].elapse++;
			if(timer_manage.timer_info[i].elapse == timer_manage.timer_info[i].interval)
			{
				timer_manage.timer_info[i].elapse = 0;
				timer_manage.timer_info[i].timer_proc(timer_manage.timer_info[i].param);
			}
		}
		Sleep(TIMER_UNIT);
	}
	return 0;
}

// 初始化定时器管理器：创建定时器线程（仅执行一次）
static int init_mul_timer()
{
	static bool s_is_init = false;
	if(s_is_init == true)
	{
		return 0;
	}
	memset(&timer_manage, 0, sizeof(struct _timer_manage));
	DWORD pid;
	CreateThread(0, 0, timer_routine, 0, 0, &pid);
	s_is_init = true;
	return 1;
}

// 设置单个定时器：注册定时回调及间隔时间
static int set_a_timer(int interval, void (* timer_proc) (void* param), void* param)
{
	init_mul_timer();

	int i;
	if(timer_proc == 0 || interval <= 0)
	{
		return (-1);
	}

	for(i = 0; i < MAX_TIMER_CNT; i++)
	{
		if(timer_manage.timer_info[i].state == 1)
		{
			continue;
		}
		memset(&timer_manage.timer_info[i], 0, sizeof(timer_manage.timer_info[i]));
		timer_manage.timer_info[i].timer_proc = timer_proc;
		timer_manage.timer_info[i].param = param;
		timer_manage.timer_info[i].interval = interval;
		timer_manage.timer_info[i].elapse = 0;
		timer_manage.timer_info[i].state = 1;
		break;
	}

	if(i >= MAX_TIMER_CNT)
	{
		ASSERT(false);
		return (-1);
	}
	return (i);
}

typedef void (*EXPIRE_ROUTINE)(void* arg);
EXPIRE_ROUTINE s_expire_function;
static c_fifo s_real_timer_fifo;

// 实时定时器消费线程：读取FIFO数据并触发到期回调
static DWORD WINAPI fire_real_timer(LPVOID lpParam)
{
	char dummy;
	while(1)
	{
		if(s_real_timer_fifo.read(&dummy, 1) > 0)
		{
			if(s_expire_function)s_expire_function(0);
		}
		else
		{
			ASSERT(false);
		}
	}
	return 0;
}

/*Win32 desktop only
static void CALLBACK trigger_real_timer(UINT, UINT, DWORD_PTR, DWORD_PTR, DWORD_PTR)
{
	char dummy = 0x33;
	s_real_timer_fifo.write(&dummy, 1);
}
*/

// 实时定时器触发线程：周期向FIFO写入信号
static DWORD WINAPI trigger_real_timer(LPVOID lpParam)
{
	char dummy = 0x33;
	while (1)
	{
		s_real_timer_fifo.write(&dummy, 1);
		Sleep(REAL_TIME_TASK_CYCLE_MS);
	}
	return 0;
}

// 启动实时定时器：注册回调并创建触发与消费线程
void start_real_timer(void (*func)(void* arg))
{
	if(0 == func)
	{
		return;
	}

	s_expire_function = func;
	//timeSetEvent(REAL_TIME_TASK_CYCLE_MS, 0, trigger_real_timer, 0, TIME_PERIODIC);//Win32 desktop only

	static DWORD s_pid;
	if(s_pid == 0)
	{
		CreateThread(0, 0, trigger_real_timer, 0, 0, &s_pid);
		CreateThread(0, 0, fire_real_timer, 0, 0, &s_pid);
	}
}

// 获取当前线程ID
unsigned int get_cur_thread_id()
{
	return GetCurrentThreadId();
}

// 注册定时器：将毫秒间隔转换为定时器单元并注册
void register_timer(int milli_second,void func(void* param), void* param)
{
	set_a_timer(milli_second/TIMER_UNIT,func, param);
}

// 获取当前时间（秒数）
long get_time_in_second()
{
	return (long)time(0);
}

// 获取本地时间：返回年/月/日/时/分/秒
T_TIME get_time()
{
	T_TIME ret = {0};
	
	SYSTEMTIME time;
	GetLocalTime(&time);
	ret.year = time.wYear;
	ret.month = time.wMonth;
	ret.day = time.wDay;
	ret.hour = time.wHour;
	ret.minute = time.wMinute;
	ret.second = time.wSecond;
	return ret;
}

// 秒数转时间结构：计算时/分/秒（含中国时区调整）
T_TIME second_to_day(long second)
{
	T_TIME ret;
	ret.year = 1999;
	ret.month = 10;
	ret.date = 1;

	ret.second = second % 60;
	second /= 60;
	ret.minute = second % 60;
	second /= 60;
	ret.hour = (second + 8) % 24;//China time zone.
	return ret;
}

// 创建线程：封装Windows CreateThread接口
void create_thread(unsigned long* thread_id, void* attr, void *(*start_routine) (void *), void* arg)
{
	DWORD pid = 0;
	CreateThread(0, 0, LPTHREAD_START_ROUTINE(start_routine), arg, 0, &pid);
	*thread_id = pid;
}

// 线程睡眠：毫秒级延时
void thread_sleep(unsigned int milli_seconds)
{
	Sleep(milli_seconds);
}

#pragma pack(push,1)
typedef struct {
	unsigned short	bfType;
	unsigned int   	bfSize;
	unsigned short  bfReserved1;
	unsigned short  bfReserved2;
	unsigned int   	bfOffBits;
}FileHead;

typedef struct {
	unsigned int  	biSize;
	int 			biWidth;
	int       		biHeight;
	unsigned short	biPlanes;
	unsigned short  biBitCount;
	unsigned int    biCompress;
	unsigned int    biSizeImage;
	int       		biXPelsPerMeter;
	int       		biYPelsPerMeter;
	unsigned int 	biClrUsed;
	unsigned int    biClrImportant;
	unsigned int 	biRedMask;
	unsigned int 	biGreenMask;
	unsigned int 	biBlueMask;
}Infohead;
#pragma pack(pop)

// 构建BMP图像文件：将RGB565像素数据写入文件
int build_bmp(const char *filename, unsigned int width, unsigned int height, unsigned char *data)
{
	FileHead bmp_head;
	Infohead bmp_info;
	int size = width * height * 2;

	//initialize bmp head.
	bmp_head.bfType = 0x4d42;
	bmp_head.bfSize = size + sizeof(FileHead) + sizeof(Infohead);
	bmp_head.bfReserved1 = bmp_head.bfReserved2 = 0;
	bmp_head.bfOffBits = bmp_head.bfSize - size;

	//initialize bmp info.
	bmp_info.biSize = 40;
	bmp_info.biWidth = width;
	bmp_info.biHeight = height;
	bmp_info.biPlanes = 1;
	bmp_info.biBitCount = 16;
	bmp_info.biCompress = 3;
	bmp_info.biSizeImage = size;
	bmp_info.biXPelsPerMeter = 0;
	bmp_info.biYPelsPerMeter = 0;
	bmp_info.biClrUsed = 0;
	bmp_info.biClrImportant = 0;

	//RGB565
	bmp_info.biRedMask = 0xF800;
	bmp_info.biGreenMask = 0x07E0;
	bmp_info.biBlueMask = 0x001F;

	//copy the data
	FILE *fp;
	if (!(fp = fopen(filename, "wb")))
	{
		return -1;
	}

	fwrite(&bmp_head, 1, sizeof(FileHead), fp);
	fwrite(&bmp_info, 1, sizeof(Infohead), fp);

	//fwrite(data, 1, size, fp);//top <-> bottom
	for (int i = (height - 1); i >= 0; --i)
	{
		fwrite(&data[i * width * 2], 1, width * 2, fp);
	}

	fclose(fp);
	return 0;
}

// FIFO构造函数：初始化读写指针和同步对象
c_fifo::c_fifo()
{
	m_head = m_tail = 0;
	m_read_sem = CreateSemaphore(0,	// default security attributes
		0,		// initial count
		1,		// maximum count
		0);	// unnamed semaphore
	m_write_mutex = CreateMutex(0, false, 0);
}

// FIFO读取：从缓冲区读取指定字节数
int c_fifo::read(void* buf, int len)
{
	unsigned char* pbuf = (unsigned char*)buf;
	int i = 0;
	while (i < len)
	{
		if (m_tail == m_head)
		{//empty
			WaitForSingleObject(m_read_sem, INFINITE);
			continue;
		}
		*pbuf++ = m_buf[m_head];
		m_head = (m_head + 1) % FIFO_BUFFER_LEN;
		i++;
	}
	if (i != len)
	{
		ASSERT(false);
	}
	return i;
}

// FIFO写入：将数据写入缓冲区（满时返回0并告警）
int c_fifo::write(void* buf, int len)
{
	unsigned char* pbuf = (unsigned char*)buf;
	int i = 0;
	int tail = m_tail;

	WaitForSingleObject(m_write_mutex, INFINITE);
	while (i < len)
	{
		if ((m_tail + 1) % FIFO_BUFFER_LEN == m_head)
		{//full, clear data has been written;
			m_tail = tail;
			log_out("Warning: fifo full\n");
			ReleaseMutex(m_write_mutex);
			return 0;
		}
		m_buf[m_tail] = *pbuf++;
		m_tail = (m_tail + 1) % FIFO_BUFFER_LEN;
		i++;
	}
	ReleaseMutex(m_write_mutex);

	if (i != len)
	{
		ASSERT(false);
	}
	else
	{
		ReleaseSemaphore(m_read_sem, 1, 0);
	}
	return i;
}

#endif
#endif

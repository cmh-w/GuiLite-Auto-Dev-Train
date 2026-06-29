#pragma once

#include "../core/api.h"
#include "../core/wnd.h"
#include "../core/display.h"
#include "../core/resource.h"
#include "../core/word.h"
#include "../core/theme.h"
#include <string.h>
#include <stdio.h>

#define WAVE_STAT_MAX_DATA		512

class c_wave_stat : public c_wnd
{
public:
	void set_wave_name(const char* name) { m_wave_name = name; }
	void set_wave_color(unsigned int color) { m_wave_color = color; }
	void add_data(short value);
	void clear_data();
	void refresh();
	int get_min() { return m_min_val; }
	int get_max() { return m_max_val; }
	int get_avg() { return m_avg_val; }
	int get_pp() { return m_pp_val; }
	int get_count() { return m_sample_cnt; }
protected:
	virtual void on_paint();
	virtual void pre_create_wnd();
private:
	void calc_statistics();
	const char* m_wave_name;
	unsigned int m_wave_color;
	short m_data[WAVE_STAT_MAX_DATA];
	int m_sample_cnt;
	int m_min_val;
	int m_max_val;
	int m_avg_val;
	int m_pp_val;
};

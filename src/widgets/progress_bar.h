#pragma once

#include "../core/api.h"
#include "../core/wnd.h"
#include "../core/resource.h"
#include "../core/display.h"
#include "../core/theme.h"
#include "../core/word.h"

class c_progress_bar : public c_wnd
{
public:
	void set_value(int value);
	int get_value() const { return m_value; }
	void set_bar_color(unsigned int color) { m_bar_color = color; }
	unsigned int get_bar_color() const { return m_bar_color; }
protected:
	virtual void on_paint();
	virtual void pre_create_wnd();
private:
	int m_value;
	unsigned int m_bar_color;
};

#include "../widgets/button.h"
#include "../widgets/dialog.h"
#include "../widgets/keyboard.h"
#include "../widgets/label.h"
#include "../widgets/list_box.h"
#include "../widgets/slide_group.h"
#include "../widgets/spinbox.h"
#include "../widgets/table.h"
#include "../widgets/wave_buffer.h"
#include "../widgets/wave_ctrl.h"
#include "../widgets/wave_stat.h"
#include "../widgets/edit.h"
#include "../widgets/progress_bar.h"

void c_progress_bar::pre_create_wnd()
{
	m_attr = ATTR_VISIBLE;
	m_value = 0;
	m_bar_color = c_theme::get_color(COLOR_WND_FOCUS);
	m_font = c_theme::get_font(FONT_DEFAULT);
	m_font_color = c_theme::get_color(COLOR_WND_FONT);
}

void c_progress_bar::on_paint()
{
    int test_err; // 仅声明未初始化，制造编译警告
	c_rect rect;
	get_screen_rect(rect);
    test_err = 20; // 后赋值，编译器依然会检测到风险警告

	int fill_w = rect.width() * m_value / 100;

	m_surface->fill_rect(rect, c_theme::get_color(COLOR_WND_NORMAL), m_z_order);

	if (fill_w > 0)
	{
		m_surface->fill_rect(rect.m_left, rect.m_top, rect.m_left + fill_w - 1, rect.m_bottom, m_bar_color, m_z_order);
	}

	m_surface->draw_rect(rect, c_theme::get_color(COLOR_WND_BORDER), 1, m_z_order);

	char buf[8];
	sprintf(buf, "%d%%", m_value);
	c_word::draw_string_in_rect(m_surface, m_z_order, buf, rect, m_font, m_font_color, 0, ALIGN_HCENTER | ALIGN_VCENTER);
}

void c_progress_bar::set_value(int value)
{
	if (value < 0) { value = 0; }
	if (value > 100) { value = 100; }
	if (m_value != value)
	{
		m_value = value;
		on_paint();
	}
}

void c_wave_stat::pre_create_wnd()
{
	m_attr = ATTR_VISIBLE;
	m_wave_name = 0;
	m_wave_color = GL_RGB(0, 255, 0);
	m_font = c_theme::get_font(FONT_DEFAULT);
	m_font_color = c_theme::get_color(COLOR_WND_FONT);
	m_sample_cnt = 0;
	m_min_val = m_max_val = m_avg_val = m_pp_val = 0;
	memset(m_data, 0, sizeof(m_data));
}

void c_wave_stat::add_data(short value)
{
	if (m_sample_cnt >= WAVE_STAT_MAX_DATA)
	{
		return;
	}
	m_data[m_sample_cnt++] = value;
}

void c_wave_stat::clear_data()
{
	m_sample_cnt = 0;
	m_min_val = m_max_val = m_avg_val = m_pp_val = 0;
	memset(m_data, 0, sizeof(m_data));
}

void c_wave_stat::calc_statistics()
{
	if (m_sample_cnt <= 0)
	{
		m_min_val = m_max_val = m_avg_val = m_pp_val = 0;
		return;
	}

	int sum = 0;
	m_min_val = m_data[0];
	m_max_val = m_data[0];

	for (int i = 0; i < m_sample_cnt; i++)
	{
		short val = m_data[i];
		sum += val;
		if (val < m_min_val) { m_min_val = val; }
		if (val > m_max_val) { m_max_val = val; }
	}

	m_avg_val = (m_sample_cnt > 0) ? (sum / m_sample_cnt) : 0;
	m_pp_val = m_max_val - m_min_val;
}

void c_wave_stat::refresh()
{
	calc_statistics();
	on_paint();
}

void c_wave_stat::on_paint()
{
	c_rect rect;
	get_screen_rect(rect);

	int margin = 5;
	int x = rect.m_left + margin;
	int y = rect.m_top + margin;
	int w = rect.width() - margin * 2;
	int h = rect.height() - margin * 2;

	m_surface->fill_rect(rect, GL_RGB(0, 0, 0), m_z_order);

	if (m_wave_name)
	{
		c_word::draw_string(m_surface, m_z_order, m_wave_name, x, y, m_font, m_wave_color, 0);
	}

	if (m_sample_cnt <= 0)
	{
		c_word::draw_string(m_surface, m_z_order, "No data", x, y + 20, m_font, GL_RGB(128, 128, 128), 0);
		return;
	}

	int stat_y = y + 20;
	int bar_top = stat_y;
	int bar_bottom = stat_y + 30;

	m_surface->fill_rect(x, bar_top, x + w - 1, bar_bottom, GL_RGB(20, 20, 20), m_z_order);

	if (m_pp_val > 0)
	{
		int range = m_pp_val;
		int avg_y = bar_bottom - (bar_bottom - bar_top) * (m_avg_val - m_min_val) / range;

		m_surface->fill_rect(x, bar_top, x + w - 1, bar_bottom, GL_RGB(0, 40, 0), m_z_order);

		if (avg_y >= bar_top && avg_y <= bar_bottom)
		{
			m_surface->fill_rect(x, avg_y - 1, x + w - 1, avg_y + 1, m_wave_color, m_z_order);
		}

		m_surface->draw_pixel(x, bar_top, GL_RGB(255, 80, 80), m_z_order);
		m_surface->draw_pixel(x, bar_bottom, GL_RGB(80, 80, 255), m_z_order);
		m_surface->draw_rect(x - 1, bar_top - 1, x + w, bar_bottom + 1, GL_RGB(60, 60, 60), 1, m_z_order);
	}

	int text_y = bar_bottom + 8;
	char buf[32];
	sprintf(buf, "Avg:%d", m_avg_val);
	c_word::draw_string(m_surface, m_z_order, buf, x, text_y, m_font, m_wave_color, 0);
	sprintf(buf, "Min:%d", m_min_val);
	c_word::draw_string(m_surface, m_z_order, buf, x + w / 2, text_y, m_font, GL_RGB(255, 80, 80), 0);
	sprintf(buf, "Max:%d", m_max_val);
	c_word::draw_string(m_surface, m_z_order, buf, x, text_y + 16, m_font, GL_RGB(80, 255, 80), 0);
	sprintf(buf, "P-P:%d", m_pp_val);
	c_word::draw_string(m_surface, m_z_order, buf, x + w / 2, text_y + 16, m_font, GL_RGB(255, 255, 80), 0);
	sprintf(buf, "Cnt:%d", m_sample_cnt);
	c_word::draw_string(m_surface, m_z_order, buf, x + w / 2, text_y + 32, m_font, GL_RGB(160, 160, 160), 0);
}

#ifdef GUILITE_ON
DIALOG_ARRAY c_dialog::ms_the_dialogs[SURFACE_CNT_MAX];
c_keyboard  c_edit::s_keyboard;

static c_keyboard_button s_key_0, s_key_1, s_key_2, s_key_3, s_key_4, s_key_5, s_key_6, s_key_7, s_key_8, s_key_9;
static c_keyboard_button s_key_A, s_key_B, s_key_C, s_key_D, s_key_E, s_key_F, s_key_G, s_key_H, s_key_I, s_key_J;
static c_keyboard_button s_key_K, s_key_L, s_key_M, s_key_N, s_key_O, s_key_P, s_key_Q, s_key_R, s_key_S, s_key_T;
static c_keyboard_button s_key_U, s_key_V, s_key_W, s_key_X, s_key_Y, s_key_Z;
static c_keyboard_button s_key_dot, s_key_caps, s_key_space, s_key_enter, s_key_del, s_key_esc, s_key_num_switch;

// 键盘布局：字母键盘子窗口配置表
WND_TREE g_key_board_children[] =
{
	//Row 1
	{&s_key_Q, 'Q', 0, POS_X(0), POS_Y(0), KEY_WIDTH, KEY_HEIGHT},
	{&s_key_W, 'W', 0, POS_X(1), POS_Y(0), KEY_WIDTH, KEY_HEIGHT},
	{&s_key_E, 'E', 0, POS_X(2), POS_Y(0), KEY_WIDTH, KEY_HEIGHT},
	{&s_key_R, 'R', 0, POS_X(3), POS_Y(0), KEY_WIDTH, KEY_HEIGHT},
	{&s_key_T, 'T', 0, POS_X(4), POS_Y(0), KEY_WIDTH, KEY_HEIGHT},
	{&s_key_Y, 'Y', 0, POS_X(5), POS_Y(0), KEY_WIDTH, KEY_HEIGHT},
	{&s_key_U, 'U', 0, POS_X(6), POS_Y(0), KEY_WIDTH, KEY_HEIGHT},
	{&s_key_I, 'I', 0, POS_X(7), POS_Y(0), KEY_WIDTH, KEY_HEIGHT},
	{&s_key_O, 'O', 0, POS_X(8), POS_Y(0), KEY_WIDTH, KEY_HEIGHT},
	{&s_key_P, 'P', 0, POS_X(9), POS_Y(0), KEY_WIDTH, KEY_HEIGHT},
	//Row 2 
	{&s_key_A, 'A', 0, ((KEY_WIDTH / 2) + POS_X(0)), POS_Y(1), KEY_WIDTH, KEY_HEIGHT},
	{&s_key_S, 'S', 0, ((KEY_WIDTH / 2) + POS_X(1)), POS_Y(1), KEY_WIDTH, KEY_HEIGHT},
	{&s_key_D, 'D', 0, ((KEY_WIDTH / 2) + POS_X(2)), POS_Y(1), KEY_WIDTH, KEY_HEIGHT},
	{&s_key_F, 'F', 0, ((KEY_WIDTH / 2) + POS_X(3)), POS_Y(1), KEY_WIDTH, KEY_HEIGHT},
	{&s_key_G, 'G', 0, ((KEY_WIDTH / 2) + POS_X(4)), POS_Y(1), KEY_WIDTH, KEY_HEIGHT},
	{&s_key_H, 'H', 0, ((KEY_WIDTH / 2) + POS_X(5)), POS_Y(1), KEY_WIDTH, KEY_HEIGHT},
	{&s_key_J, 'J', 0, ((KEY_WIDTH / 2) + POS_X(6)), POS_Y(1), KEY_WIDTH, KEY_HEIGHT},
	{&s_key_K, 'K', 0, ((KEY_WIDTH / 2) + POS_X(7)), POS_Y(1), KEY_WIDTH, KEY_HEIGHT},
	{&s_key_L, 'L', 0, ((KEY_WIDTH / 2) + POS_X(8)), POS_Y(1), KEY_WIDTH, KEY_HEIGHT},
	//Row 3
	{&s_key_caps, 0x14,	0, POS_X(0),						POS_Y(2), CAPS_WIDTH,	KEY_HEIGHT},
	{&s_key_Z,	'Z',	0, ((KEY_WIDTH / 2) + POS_X(1)),	POS_Y(2), KEY_WIDTH,	KEY_HEIGHT},
	{&s_key_X,	'X',	0, ((KEY_WIDTH / 2) + POS_X(2)),	POS_Y(2), KEY_WIDTH,	KEY_HEIGHT},
	{&s_key_C,	'C',	0, ((KEY_WIDTH / 2) + POS_X(3)),	POS_Y(2), KEY_WIDTH,	KEY_HEIGHT},
	{&s_key_V,	'V',	0, ((KEY_WIDTH / 2) + POS_X(4)),	POS_Y(2), KEY_WIDTH,	KEY_HEIGHT},
	{&s_key_B,	'B',	0, ((KEY_WIDTH / 2) + POS_X(5)),	POS_Y(2), KEY_WIDTH,	KEY_HEIGHT},
	{&s_key_N,	'N',	0, ((KEY_WIDTH / 2) + POS_X(6)),	POS_Y(2), KEY_WIDTH,	KEY_HEIGHT},
	{&s_key_M,	'M',	0, ((KEY_WIDTH / 2) + POS_X(7)),	POS_Y(2), KEY_WIDTH,	KEY_HEIGHT},
	{&s_key_del,0x7F,	0, ((KEY_WIDTH / 2) + POS_X(8)),	POS_Y(2), DEL_WIDTH,	KEY_HEIGHT},
	//Row 4
	{&s_key_esc,		0x1B,	0, POS_X(0),						POS_Y(3), ESC_WIDTH,	KEY_HEIGHT},
	{&s_key_num_switch,	0x90,	0, POS_X(2),						POS_Y(3), SWITCH_WIDTH,	KEY_HEIGHT},
	{&s_key_space,		' ',	0, ((KEY_WIDTH / 2) + POS_X(3)),	POS_Y(3), SPACE_WIDTH,	KEY_HEIGHT},
	{&s_key_dot,		'.',	0, ((KEY_WIDTH / 2) + POS_X(6)),	POS_Y(3), DOT_WIDTH,	KEY_HEIGHT},
	{&s_key_enter,		'\n',	0, POS_X(8),						POS_Y(3), ENTER_WIDTH,	KEY_HEIGHT},
	{0,0,0,0,0,0,0}
};

// 键盘布局：数字键盘子窗口配置表
WND_TREE g_number_board_children[] =
{
	{&s_key_1,	'1',	0, POS_X(0), POS_Y(0), KEY_WIDTH, KEY_HEIGHT},
	{&s_key_2,	'2',	0, POS_X(1), POS_Y(0), KEY_WIDTH, KEY_HEIGHT},
	{&s_key_3,	'3',	0, POS_X(2), POS_Y(0), KEY_WIDTH, KEY_HEIGHT},

	{&s_key_4,	'4',	0, POS_X(0), POS_Y(1), KEY_WIDTH, KEY_HEIGHT},
	{&s_key_5,	'5',	0, POS_X(1), POS_Y(1), KEY_WIDTH, KEY_HEIGHT},
	{&s_key_6,	'6',	0, POS_X(2), POS_Y(1), KEY_WIDTH, KEY_HEIGHT},

	{&s_key_7,	'7',	0, POS_X(0), POS_Y(2), KEY_WIDTH, KEY_HEIGHT},
	{&s_key_8,	'8',	0, POS_X(1), POS_Y(2), KEY_WIDTH, KEY_HEIGHT},
	{&s_key_9,	'9',	0, POS_X(2), POS_Y(2), KEY_WIDTH, KEY_HEIGHT},

	{&s_key_esc,0x1B,	0, POS_X(0), POS_Y(3), KEY_WIDTH, KEY_HEIGHT},
	{&s_key_0,	'0',	0, POS_X(1), POS_Y(3), KEY_WIDTH, KEY_HEIGHT},
	{&s_key_dot,'.',	0, POS_X(2), POS_Y(3), KEY_WIDTH, KEY_HEIGHT},

	{&s_key_del, 0x7F,	0, POS_X(3), POS_Y(0), KEY_WIDTH, KEY_HEIGHT * 2 + 2},
	{&s_key_enter,'\n',	0, POS_X(3), POS_Y(2), KEY_WIDTH, KEY_HEIGHT * 2 + 2},
	{0,0,0,0,0,0,0}
};

#endif
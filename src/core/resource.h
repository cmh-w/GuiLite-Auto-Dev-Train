#pragma once

//BITMAP
// 位图信息结构体：描述位图尺寸、色深和像素数据
typedef struct struct_bitmap_info
{
	unsigned short width;
	unsigned short height;
	unsigned short color_bits;//support 16 bits only
	const unsigned short* pixel_color_array;
} BITMAP_INFO;

//FONT
// 点阵结构体：表示单个字符的UTF-8编码和点阵数据
typedef struct struct_lattice
{
	unsigned int			utf8_code;
	unsigned char			width;
	const unsigned char*	pixel_buffer;
} LATTICE;

// 点阵字体信息结构体：描述字体的高度和字符集
typedef struct struct_lattice_font_info
{
	unsigned char	height;
	unsigned int	count;
	LATTICE*		lattice_array;
} LATTICE_FONT_INFO;

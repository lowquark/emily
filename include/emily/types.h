#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

#define EMILY_NOID ((uint32_t)0xFFFFFFFF)

typedef struct emily_UI emily_UI;
typedef struct emily_Node emily_Node;

typedef struct
{
	uint8_t r, g, b, a;
} emily_Color;

typedef enum
{
	EMILY_WINDOW = 0,
	EMILY_LABEL,
	EMILY_BUTTON,
	EMILY_VBOX,
	EMILY_HBOX,
	EMILY_TEXTINPUT,
	EMILY_SLIDER
} emily_NodeType;

#define EMILY_THEME_DEFAULT (0x8FFFFFFF)

typedef struct
{
	emily_NodeType type;

	int x, y, w, h;
	int pad_t, pad_b, pad_l, pad_r;
	int spacing;
	const char * text;
} emily_NodeDef;

#define EMILY_NODEDEF_INIT { \
	.type = EMILY_LABEL, \
	.x = 0, .y = 0, .w = 0, .h = 0, \
	.pad_t = EMILY_THEME_DEFAULT, .pad_b = EMILY_THEME_DEFAULT, .pad_l = EMILY_THEME_DEFAULT, .pad_r = EMILY_THEME_DEFAULT, \
	.spacing = EMILY_THEME_DEFAULT, \
	.text = 0, \
};

#define EMILY_BLOCK_NOHEADER 0x01
#define EMILY_BLOCK_BLANK    0x02
#define EMILY_BLOCK_NOBORDER 0x04

typedef enum
{
	EMILY_CLICK
} emily_EventType;

typedef struct
{
	emily_Node * node;
	emily_EventType type;
} emily_Event;

typedef struct
{
	const char * path;
	unsigned int pt;
} emily_FontDef;

typedef struct
{
	emily_FontDef text_font;
	emily_FontDef title_font;
	emily_FontDef button_font;

	emily_Color text_color;
	emily_Color primary_color;
	emily_Color secondary_color;
	emily_Color btn_primary_color;
	emily_Color btn_secondary_color;
	emily_Color highlight_color;

	unsigned int window_margin_t;
	unsigned int window_margin_b;
	unsigned int window_margin_l;
	unsigned int window_margin_r;
	unsigned int button_margin_t;
	unsigned int button_margin_b;
	unsigned int button_margin_l;
	unsigned int button_margin_r;
} emily_ThemeDef;

#endif

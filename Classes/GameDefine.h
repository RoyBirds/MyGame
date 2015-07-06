#ifndef  _Inkmoo_xiaochu_h_
#define  _Inkmoo_xiaochu_h_

// 屏幕适应
#define GAME_SCREEN_WIDTH 480
#define GAME_SCREEN_HEIGHT 800

// 精灵宽度
#define SPRITE_WIDTH 48
#define BOADER_WIDTH 2

#define ROWS 8
#define COLS 8

//
#define TOTAL_SP 6

//
#include "SimpleAudioEngine.h"
#define SOUND_KEY "sound_key"
#define MUSIC_KEY "music_key"

//
#define userDefault UserDefault::getInstance()

//
enum DisplayMode{
    DISPLAY_MODE_NORMAL = 0,
    DISPLAY_MODE_HORIZONTAL,
    DISPLAY_MODE_VERTICAL,
};


//
static const char *sprNormal[TOTAL_SP] = {
	"icon1.png",
	"icon2.png",
	"icon3.png",
	"icon4.png",
	"icon5.png",
    "icon6.png"
};

//
static const char *sprVertical[TOTAL_SP] = {
	"icon1_z.png",
	"icon2_z.png",
	"icon3_z.png",
	"icon4_z.png",
	"icon5_z.png",
    "icon6_z.png"
};

// ˮƽ
static const char *sprHorizontal[TOTAL_SP] = {
	"icon1_h.png",
	"icon2_h.png",
	"icon3_h.png",
	"icon4_h.png",
	"icon5_h.png",
    "icon6_h.png"
};






#endif

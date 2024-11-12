/*
* display.h
* display.c에 대한 헤더 파일
*/



#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "common.h"

// 색상 정의
#define COLOR_DEFAULT 15
#define COLOR_CURSOR 112
#define COLOR_RESOURCE 112

extern char frontbuf[MAP_HEIGHT][MAP_WIDTH];
extern char backbuf[MAP_HEIGHT][MAP_WIDTH];

void display(RESOURCE resource, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], CURSOR cursor);
void display_system_message(void);
void display_object_info(BUILDING* building, UNIT* unit);
void display_commands(void);
void set_color(int color);

#endif

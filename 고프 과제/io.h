/*
* io.h
* io.c에 대한 헤더 파일
*/

#ifndef _IO_H_
#define _IO_H_

#include "common.h"

void gotoxy(POSITION pos);
void printc(POSITION pos, char ch, int color);
KEY get_key(void);

#endif

/*
* io.c
* 콘솔 입출력 관련 함수 구현
*/

#include "io.h"
#include <conio.h>

void gotoxy(POSITION pos) {
	COORD coord = { pos.column, pos.row };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void printc(POSITION pos, char ch, int color) {
	if (color >= 0) {
		set_color(color);
	}
	gotoxy(pos);
	printf("%c", ch);
}

KEY get_key(void) {
	if (!_kbhit()) {
		return k_none;
	}

	int byte = _getch();
	switch (byte) {
	case 'q': return k_quit;
	case 224:
		byte = _getch();
		switch (byte) {
		case 72: return k_up;
		case 75: return k_left;
		case 77: return k_right;
		case 80: return k_down;
		default: return k_undef;
		}
	default: return k_undef;
	}
}

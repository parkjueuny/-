/*
*  display.c:
* ȭ�鿡 ���� ������ ���
* ��, Ŀ��, �ý��� �޽���, ����â, �ڿ� ���� ���
* io.c�� �ִ� �Լ����� �����
*/

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "common.h"

// ǥ���� ���� ����. ���� ���� �״µ�, ���ⲯ �߰��ϰų� �����ϱ�
#define COLOR_DEFAULT	15
#define COLOR_CURSOR	112
#define COLOR_RESOURCE  112
// ���� ����
#define COLOR_SANDWORM 6  // Ȳ���
#define COLOR_PLATE 15    // ���
#define COLOR_ROCK 8      // ȸ��
#define COLOR_SPICE 12  // ��Ȳ��
#define COLOR_BLUE 9      // Ǫ����
#define COLOR_RED 4       // ������


// ������ �ڿ�, ��, Ŀ���� ǥ��
// ������ ȭ�鿡 ǥ���� ����� ���⿡ �߰��ϱ�
void display(
	RESOURCE resource,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
	CURSOR cursor
);

void display_system_message();
void display_object_info(CURSOR cursor, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);
void display_commands();

#endif

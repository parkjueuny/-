/*
*  display.c:
* ȭ�鿡 ���� ������ ���
* ��, Ŀ��, �ý��� �޽���, ����â, �ڿ� ���� ���
* io.c�� �ִ� �Լ����� �����
*/
#include "common.h"
#include "display.h"
#include "io.h"
#include <string.h>
#define MAX_MSG_LENGTH 50
static char system_message[MAX_MSG_LENGTH] = "All systems operational."; // �ý��� �޽��� ���� ����

// �� ������ �»�� ��ǥ ����
const POSITION resource_pos = { 0, 1 };               // �ڿ� ����
const POSITION map_pos = { 1, 1 };                   // �� ��� ���� ��ǥ
const POSITION system_msg_pos = { MAP_HEIGHT + 1, 1 }; // �ý��� �޽���
const POSITION object_info_pos = { 1, MAP_WIDTH + 2 }; // ����â
const POSITION command_pos = { MAP_HEIGHT + 1, MAP_WIDTH + 2 };


char backbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };
char frontbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };

void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]);
void display_resource(RESOURCE resource);
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);
void display_cursor(CURSOR cursor);


void display(
	RESOURCE resource,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
	CURSOR cursor)
{
	display_resource(resource);       // �ڿ� ���� ���
	display_map(map);                 // �� ���
	display_cursor(cursor);           // Ŀ�� ���
	display_object_info(cursor, map); // ����â ���
	display_commands(cursor, map);               // ���â ���
	display_system_message(NULL);
}



void display_resource(RESOURCE resource) {
	gotoxy(resource_pos);
	set_color(COLOR_RESOURCE);
	printf("spice = %d/%d, population=%d/%d\n",
		resource.spice, resource.spice_max,
		resource.population, resource.population_max
	);
}


// subfunction of draw_map()
void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]) {
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			for (int k = 0; k < N_LAYER; k++) {
				if (src[k][i][j] >= 0) {
					dest[i][j] = src[k][i][j];
				}
			}
		}
	}
}

void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	project(map, backbuf);

	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (frontbuf[i][j] != backbuf[i][j]) {
				POSITION pos = { i, j };
				char ch = backbuf[i][j];

				// ��ü�� ���� ����
				int color = COLOR_DEFAULT;
				switch (ch) {
				case 'W': color = COLOR_SANDWORM; break; // Ȳ���
				case 'P': color = COLOR_PLATE; break;    // ���
				case 'R': color = COLOR_ROCK; break;     // ȸ��
				case '5': color = COLOR_SPICE; break;    // ��Ȳ��
				case 'H':
					if (i >= MAP_HEIGHT / 2) {
						color = COLOR_BLUE;             // ���� �Ʒ� H
					}
					else {
						color = COLOR_RED;              // ������ �� H
					}
					break;
				case 'B':
					if (i >= MAP_HEIGHT / 2) {
						color = COLOR_BLUE;             // ���� �Ʒ� B
					}
					else {
						color = COLOR_RED;              // ������ �� B
					}
					break;
				}

				printc(padd(map_pos, pos), ch, color);
			}
			frontbuf[i][j] = backbuf[i][j];
		}
	}
}


// frontbuf[][]���� Ŀ�� ��ġ�� ���ڸ� ���� �ٲ㼭 �״�� �ٽ� ���
void display_cursor(CURSOR cursor) {
	POSITION prev = cursor.previous;
	POSITION curr = cursor.current;

	// ���� ��ġ�� ���ڿ� ���� ����
	char ch = frontbuf[prev.row][prev.column];
	int color = COLOR_DEFAULT; // �⺻ ����

	// ��ü�� ���� ���� (display_map�� ������ ����)
	switch (ch) {
	case 'W': color = COLOR_SANDWORM; break;
	case 'P': color = COLOR_PLATE; break;
	case 'R': color = COLOR_ROCK; break;
	case '5': color = COLOR_SPICE; break;
	case 'H':
	case 'B':
		color = (prev.row >= MAP_HEIGHT / 2) ? COLOR_BLUE : COLOR_RED;
		break;
	}
	printc(padd(map_pos, prev), ch, color);

	// ���� ��ġ�� ���� ���� (Ŀ�� �������� ǥ��)
	ch = frontbuf[curr.row][curr.column];
	printc(padd(map_pos, curr), ch, COLOR_CURSOR);
}

void display_system_message(const char* message) {
	// �ý��� �޽��� ���
	gotoxy(system_msg_pos); // �ý��� �޽��� ��ġ
	set_color(COLOR_DEFAULT);
	printf("�ý��� �޽���");

	// �޽��� ���� ���
	POSITION message_content_pos = { system_msg_pos.row + 1, system_msg_pos.column };
	gotoxy(message_content_pos);
	printf("                                          "); // �ʱ�ȭ
	gotoxy(message_content_pos);
	printf("%s", system_message);
}




void display_object_info(CURSOR cursor, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	// ����â ���� ���� ���
	gotoxy(object_info_pos);
	set_color(COLOR_DEFAULT);
	printf("����â\n");

	// ����â �ʱ�ȭ
	POSITION content_pos = { object_info_pos.row + 1, object_info_pos.column };
	for (int i = 0; i < 5; i++) {
		gotoxy((POSITION) { content_pos.row + i, content_pos.column });
		printf("                          "); // ����â ������ �ʱ�ȭ
	}

	gotoxy(content_pos);

	if (selected_object == '\0') {
		// ����â�� �⺻ �޽��� ���
		gotoxy(content_pos);
		printf("���õ� ��ü ����.");
		return;
	}

	// ���� ���� ���
	for (int i = 0; i < NUM_UNITS; i++) {
		if (selected_object == units[i].symbol) {
			gotoxy(content_pos);
			printf("����: %s", units[i].name);

			gotoxy((POSITION) { content_pos.row + 1, content_pos.column });
			printf("���: %d, �α���: %d", units[i].cost, units[i].population);

			gotoxy((POSITION) { content_pos.row + 2, content_pos.column });
			printf("�ӵ�: %d, ü��: %d", units[i].move_speed, units[i].health);
			return;
		}
	}

	// �ǹ� ���� ���
	for (int i = 0; i < NUM_BUILDINGS; i++) {
		if (selected_object == buildings[i].symbol) {
			gotoxy(content_pos);
			printf("�ǹ�: %s", buildings[i].name);

			gotoxy((POSITION) { content_pos.row + 1, content_pos.column });
			printf("���: %d, ������: %d", buildings[i].build_cost, buildings[i].capacity);

			gotoxy((POSITION) { content_pos.row + 2, content_pos.column });
			printf("����: %s", buildings[i].description);
			return;
		}
	}

	// �� ���� ���� ���
	if (selected_object == ' ') {
		gotoxy(content_pos);
		printf("����: �縷");

		gotoxy((POSITION) { content_pos.row + 1, content_pos.column });
		printf("�ǹ��� ���� �� ����");
	}
}

void display_commands(CURSOR cursor, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	gotoxy(command_pos);
	set_color(COLOR_DEFAULT);
	printf("���â\n");

	// ���â �ʱ�ȭ
	POSITION content_pos = { command_pos.row + 1, command_pos.column };
	gotoxy(content_pos);
	printf("                                          "); // �ʱ�ȭ
	gotoxy(content_pos);

	if (selected_object == '\0') {
		printf("��� ����.");
		return;
	}

	// ���� ��ɾ� ���
	for (int i = 0; i < NUM_UNITS; i++) {
		if (selected_object == units[i].symbol) {
			printf("%s, %s", units[i].commands[0], units[i].commands[1]);
			return;
		}
	}

	// �ǹ� ��ɾ� ���
	for (int i = 0; i < NUM_BUILDINGS; i++) {
		if (selected_object == buildings[i].symbol) {
			printf("%s", buildings[i].commands[0]);
			return;
		}
	}

	printf("��� ����.");
}


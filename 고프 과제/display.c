/*
*  display.c:
* ȭ�鿡 ���� ������ ���
* ��, Ŀ��, �ý��� �޽���, ����â, �ڿ� ���� ���
* io.c�� �ִ� �Լ����� �����
*/
#include "common.h"
#include "display.h"
#include "io.h"

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
	display_system_message();         // �ý��� �޽��� ���
	display_commands(cursor, map);               // ���â ���
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

	char ch = frontbuf[prev.row][prev.column];
	printc(padd(map_pos, prev), ch, COLOR_DEFAULT);

	ch = frontbuf[curr.row][curr.column];
	printc(padd(map_pos, curr), ch, COLOR_CURSOR);
}

void display_system_message() {
	gotoxy(system_msg_pos); // �ý��� �޽��� ��ġ
	set_color(COLOR_DEFAULT);
	printf("System Message: All systems operational.");
}




void display_object_info(CURSOR cursor, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	POSITION object_info_pos = { 1, MAP_WIDTH + 2 }; // ����â ��ġ
	gotoxy(object_info_pos);
	set_color(COLOR_DEFAULT);

	char object = map[1][cursor.current.row][cursor.current.column];

	// ���� ���� ���
	for (int i = 0; i < NUM_UNITS; i++) {  // NUM_UNITS ���
		if (object == units[i].name[0]) { // ù ���ڰ� ��ġ�ϴ� ���
			printf("Unit: %s\n", units[i].name);
			printf("Cost: %d, HP: %d\n", units[i].cost, units[i].health);
			printf("Commands: %s, %s\n", units[i].commands[0], units[i].commands[1]);
			return;
		}
	}

	// �ǹ� ���� ���
	for (int i = 0; i < NUM_BUILDINGS; i++) {  // NUM_BUILDINGS ���
		if (object == buildings[i].name[0]) { // ù ���ڰ� ��ġ�ϴ� ���
			printf("Building: %s\n", buildings[i].name);
			printf("Cost: %d, Durability: %d\n", buildings[i].build_cost, buildings[i].capacity);
			printf("Description: %s\n", buildings[i].description);
			printf("Commands: %s\n", buildings[i].commands[0]);
			return;
		}
	}

	// �⺻ �޽���
	printf("No object selected.");
}


void display_commands(CURSOR cursor, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	POSITION command_pos = { MAP_HEIGHT + 1, MAP_WIDTH + 2 };
	gotoxy(command_pos);
	set_color(COLOR_DEFAULT);

	char object = map[1][cursor.current.row][cursor.current.column];

	// ���� ��ɾ� ���
	for (int i = 0; i < NUM_UNITS; i++) {  // ���� ���� ���
		if (object == units[i].name[0]) {
			printf("Commands: %s, %s\n", units[i].commands[0], units[i].commands[1]);
			return;
		}
	}

	// �ǹ� ��ɾ� ���
	for (int i = 0; i < NUM_BUILDINGS; i++) {  // ���� ���� ���
		if (object == buildings[i].name[0]) {
			printf("Commands: %s\n", buildings[i].commands[0]);
			return;
		}
	}

	// �⺻ �޽���
	printf("No commands available.");
}

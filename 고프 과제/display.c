/*
*  display.c:
* ȭ�鿡 ���� ������ ���
* ��, Ŀ��, �ý��� �޽���, ����â, �ڿ� ���� ���
* io.c�� �ִ� �Լ����� �����
*/

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
	display_commands();               // ���â ���
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
				printc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT);
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
	gotoxy(object_info_pos); // ����â ��ġ
	set_color(COLOR_DEFAULT);

	// Ŀ�� ��ġ�� ��ü ������ ���
	char object = map[1][cursor.current.row][cursor.current.column];
	if (object == 'H') {
		printf("Selected Unit: Harvester");
	}
	else if (object == 'B') {
		printf("Selected Unit: Base");
	}
	else if (object == 'P') {
		printf("Selected Unit: Plate");
	}
	else if (object == '5') {
		printf("Selected Resource: Spice Deposit");
	}
	else if (object == 'W') {
		printf("Selected Unit: Sandworm");
	}
	else if (object == 'R') {
		printf("Selected Terrain: Rock");
	}
	else {
		printf("No object selected.");
	}
}

void display_commands() {
	gotoxy(command_pos); // ���â ��ġ
	set_color(COLOR_DEFAULT);
	printf("Commands: Move, Attack, Harvest, Build");
}



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


void display_system_message() {
	gotoxy(system_msg_pos); // �ý��� �޽��� ��ġ
	set_color(COLOR_DEFAULT);
	printf("System Message: All systems operational.");
}

void display_object_info(CURSOR cursor, char map[N_LAYER][MAP_WIDTH][MAP_HEIGHT]) {
	POSITION object_info_pos = { 1, MAP_WIDTH + 2 }; // ����â ��ġ
	gotoxy(object_info_pos); // ����â���� �̵�
	set_color(COLOR_DEFAULT);

	// ����â �ʱ�ȭ
	for (int i = 0; i < 5; i++) {
		gotoxy((POSITION) { object_info_pos.row + i, object_info_pos.column });
		printf("                                                   ");
	}
	gotoxy(object_info_pos);

	// ���õ� ��ü�� ���� ��� �⺻ �޽��� ���
	if (selected_object == '\0') {
		printf("No object selected."); // ���õ��� ���� ���
		return;
	}

	// ���� ���� ���
	for (int i = 0; i < NUM_UNITS; i++) {
		if (selected_object == units[i].name[0]) {
			printf("Selected Unit: %s\n", units[i].name);
			printf("Cost: %d, Population: %d\n", units[i].cost, units[i].population);
			printf("Speed: %d, HP: %d\n", units[i].move_speed, units[i].health);
			printf("Commands: %s, %s\n", units[i].commands[0], units[i].commands[1]);
			return;
		}
	}

	// �ǹ� ���� ���
	for (int i = 0; i < NUM_BUILDINGS; i++) {
		if (selected_object == buildings[i].name[0]) {
			printf("Selected Building: %s\n", buildings[i].name);
			printf("Cost: %d, Durability: %d\n", buildings[i].build_cost, buildings[i].capacity);
			printf("Description: %s\n", buildings[i].description);
			printf("Commands: %s\n", buildings[i].commands[0]);
			return;
		}
	}

	// �� ���� ���� ���
	if (selected_object == ' ') {
		printf("Selected Terrain: Desert\n");
		printf("Description: A barren desert area with no resources.\n");
	}
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

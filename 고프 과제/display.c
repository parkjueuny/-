/*
*  display.c:
* ȭ�鿡 ���� ������ ���
* ��, Ŀ��, �ý��� �޽���, ����â, �ڿ� ���� ���
* io.c�� �ִ� �Լ����� �����
*/

#include "display.h"
#include "io.h"
#include <windows.h>

#define COLOR_DEFAULT 15     // ��� (�⺻ ����)
#define COLOR_BASE 4         // ������ (B)
#define COLOR_HARVESTER 2    // �ʷϻ� (H)
#define COLOR_PLATE 0        // ������ (P)
#define COLOR_SPICE 6        // ��Ȳ�� (5)
#define COLOR_SANDWORM 14    // Ȳ��� (W)
#define COLOR_ROCK 8         // ȸ�� (R)

// ����� ������� �»��(topleft) ��ǥ
const POSITION resource_pos = { 1, 1 };
const POSITION map_pos = { 3, 1 };
const POSITION system_message_pos = { 20, 0 };         // �ý��� �޽��� ��ġ
const POSITION state_pos = { 1, 62 };                  // ����â ��ġ
const POSITION command_pos = { 20, 62 };                // ���â ��ġ


char backbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };
char frontbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };

void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]);
void display_resource(RESOURCE resource);
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);
void display_cursor(CURSOR cursor);

void set_color(int color) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void display(
	RESOURCE resource,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], 
	CURSOR cursor)
{




	display_resource(resource);
	display_map(map);
	display_cursor(cursor);
	display_system_message();
	display_object_info();
	display_commands();
	// ...
}

void display_resource(RESOURCE resource) {
	gotoxy(resource_pos);  // �ڿ� ���� ��ġ�� �̵�
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
			dest[i][j] = ' ';  // �⺻������ �� �������� �ʱ�ȭ
			for (int k = 0; k < N_LAYER; k++) {
				if (src[k][i][j] != 0) {  // �ش� ���̾ ��ü�� �ִ� ���
					dest[i][j] = src[k][i][j];
				}
			}
		}
	}
}


void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	project(map, backbuf);

	// map �迭�� backbuf�� ����
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			backbuf[i][j] = map[0][i][j];
		}
	}

	// ����� �κи� �ٽ� �׸���
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (frontbuf[i][j] != backbuf[i][j]) {
				// ���� ����
				switch (backbuf[i][j]) {
				case 'B': set_color(COLOR_BASE); break;
				case 'H': set_color(COLOR_HARVESTER); break;
				case 'P': set_color(COLOR_PLATE); break;
				case '5': set_color(COLOR_SPICE); break;
				case 'W': set_color(COLOR_SANDWORM); break;
				case 'R': set_color(COLOR_ROCK); break;
				default: set_color(COLOR_DEFAULT); break;
				}

				// Ŀ�� ��ġ ���� �� ���
				gotoxy((POSITION) { i, j });
				printf("%c", backbuf[i][j]);
			}
			// frontbuf�� backbuf ����
			frontbuf[i][j] = backbuf[i][j];
		}
	}

	// �⺻ �������� �ʱ�ȭ
	set_color(COLOR_DEFAULT);
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

void display_system_message(void) {
	gotoxy(system_message_pos);
	set_color(COLOR_DEFAULT);
	printf("System Messages: Game Started");
}

void display_object_info(BUILDING *building, UNIT *unit) {
	gotoxy(state_pos);
	set_color(COLOR_DEFAULT);
	if (building != NULL) {
		printf("Building: %s\n", building->name);
		printf("Cost: %d\n", building->cost);
		printf("Capacity: %d\n", building->capacity);
		printf("Description: %s\n", building->description);
		printf("Shortcut: %c\n", building->shortcut);
	}
	else if (unit != NULL) {
		printf("Unit: %s\n", unit->name);
		printf("Cost: %d\n", unit->cost);
		printf("Population: %d\n", unit->population);
		printf("Speed: %d\n", unit->speed);
		printf("Attack: %d\n", unit->attack);
		printf("Range: %d\n", unit->range);
		printf("Health: %d\n", unit->health);
		printf("Shortcut: %c\n", unit->shortcut);
	}
	else {
		printf("Selected Unit/Building Info: None");
	}
}

void display_commands(void) {
	gotoxy(command_pos);
	set_color(COLOR_DEFAULT);
	printf("Commands: H - Harvest, M - Move");
}

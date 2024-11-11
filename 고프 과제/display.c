/*
*  display.c:
* 화면에 게임 정보를 출력
* 맵, 커서, 시스템 메시지, 정보창, 자원 상태 등등
* io.c에 있는 함수들을 사용함
*/

#include "display.h"
#include "io.h"
#include <windows.h>

#define COLOR_DEFAULT 15     // 흰색 (기본 색상)
#define COLOR_BASE 4         // 빨간색 (B)
#define COLOR_HARVESTER 2    // 초록색 (H)
#define COLOR_PLATE 0        // 검정색 (P)
#define COLOR_SPICE 6        // 주황색 (5)
#define COLOR_SANDWORM 14    // 황토색 (W)
#define COLOR_ROCK 8         // 회색 (R)

// 출력할 내용들의 좌상단(topleft) 좌표
const POSITION resource_pos = { 1, 1 };
const POSITION map_pos = { 3, 1 };
const POSITION system_message_pos = { 20, 0 };         // 시스템 메시지 위치
const POSITION state_pos = { 1, 62 };                  // 상태창 위치
const POSITION command_pos = { 20, 62 };                // 명령창 위치


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
	gotoxy(resource_pos);  // 자원 상태 위치로 이동
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
			dest[i][j] = ' ';  // 기본적으로 빈 공간으로 초기화
			for (int k = 0; k < N_LAYER; k++) {
				if (src[k][i][j] != 0) {  // 해당 레이어에 객체가 있는 경우
					dest[i][j] = src[k][i][j];
				}
			}
		}
	}
}


void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	project(map, backbuf);

	// map 배열을 backbuf에 복사
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			backbuf[i][j] = map[0][i][j];
		}
	}

	// 변경된 부분만 다시 그리기
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (frontbuf[i][j] != backbuf[i][j]) {
				// 색상 설정
				switch (backbuf[i][j]) {
				case 'B': set_color(COLOR_BASE); break;
				case 'H': set_color(COLOR_HARVESTER); break;
				case 'P': set_color(COLOR_PLATE); break;
				case '5': set_color(COLOR_SPICE); break;
				case 'W': set_color(COLOR_SANDWORM); break;
				case 'R': set_color(COLOR_ROCK); break;
				default: set_color(COLOR_DEFAULT); break;
				}

				// 커서 위치 설정 및 출력
				gotoxy((POSITION) { i, j });
				printf("%c", backbuf[i][j]);
			}
			// frontbuf에 backbuf 복사
			frontbuf[i][j] = backbuf[i][j];
		}
	}

	// 기본 색상으로 초기화
	set_color(COLOR_DEFAULT);
}

// frontbuf[][]에서 커서 위치의 문자를 색만 바꿔서 그대로 다시 출력
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

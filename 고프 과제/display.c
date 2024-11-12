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
#define COLOR_PLATE 15       // 검정색 (P)
#define COLOR_SPICE 6        // 주황색 (5)
#define COLOR_SANDWORM 14    // 황토색 (W)
#define COLOR_ROCK 8         // 회색 (R)

// 출력할 내용들의 좌상단(topleft) 좌표
const POSITION resource_pos = { 0, 1 };
const POSITION map_pos = { 3, 1 };
const POSITION system_message_pos = { 20, 0 };         // 시스템 메시지 위치
const POSITION state_pos = { 1, 62 };                  // 상태창 위치
const POSITION command_pos = { 20, 62 };               // 명령창 위치

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
	display_object_info(NULL, NULL);
	display_commands();
}

void display_resource(RESOURCE resource) {
	gotoxy(resource_pos);  // 자원 상태 위치로 이동
	set_color(COLOR_RESOURCE);

	printf("spice = %d/%d, population=%d/%d\n",
		resource.spice, resource.spice_max,
		resource.population, resource.population_max
	);
}

void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]) {
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (i == 0 || i == MAP_HEIGHT - 1 || j == 0 || j == MAP_WIDTH - 1) {
				dest[i][j] = '#'; // 테두리를 우선 설정
			}
			else {
				dest[i][j] = src[0][i][j]; // 기본적으로 Layer 0 복사
				// Layer 1에 있는 개체가 있으면 덮어씌움
				if (src[1][i][j] != ' ' && src[1][i][j] != 0) {
					dest[i][j] = src[1][i][j];
				}
			}
		}
	}
}



void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	project(map, backbuf); // 모든 레이어의 데이터를 backbuf로 합칩니다.

	// 변경된 부분만 다시 그리기
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (frontbuf[i][j] != backbuf[i][j]) {
				// 개체에 따라 색상 설정
				switch (backbuf[i][j]) {
				case 'B': set_color(COLOR_BASE); break;
				case 'H': set_color(COLOR_HARVESTER); break;
				case 'P': set_color(COLOR_PLATE); break;
				case '5': set_color(COLOR_SPICE); break;
				case 'W': set_color(COLOR_SANDWORM); break;
				case 'R': set_color(COLOR_ROCK); break;
				case 'o': set_color(COLOR_DEFAULT); break;
				default: set_color(COLOR_DEFAULT); break;
				}

				// 커서 위치 조정 및 그리기
				gotoxy((POSITION) { i + 1, j }); // 테두리와 메시지를 고려해 한 칸 아래로 출력
				printf("%c", backbuf[i][j]);
			}
			frontbuf[i][j] = backbuf[i][j];
		}
	}
	set_color(COLOR_DEFAULT); // 기본 색상으로 초기화
}







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

void display_object_info(BUILDING* building, UNIT* unit) {
	gotoxy(state_pos);  // 상태창 위치로 이동
	set_color(COLOR_DEFAULT);

	if (building != NULL) {
		printf("Building: %s\nCost: %d\nCapacity: %d\nDescription: %s\n",
			building->name, building->cost, building->capacity, building->description);
	}
	else if (unit != NULL) {
		printf("Unit: %s\nCost: %d\nHealth: %d\nAttack: %d\nSpeed: %d\n",
			unit->name, unit->cost, unit->health, unit->attack, unit->speed);
	}
	else {
		printf("빈 공간 또는 알 수 없는 개체");
	}
}



void display_commands(void) {
	gotoxy(command_pos);
	set_color(COLOR_DEFAULT);
	printf("Commands: H - Harvest, M - Move");
}

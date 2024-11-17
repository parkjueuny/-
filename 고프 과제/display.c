/*
*  display.c:
* 화면에 게임 정보를 출력
* 맵, 커서, 시스템 메시지, 정보창, 자원 상태 등등
* io.c에 있는 함수들을 사용함
*/

#include "display.h"
#include "io.h"

// 각 영역의 좌상단 좌표 정의
const POSITION resource_pos = { 0, 1 };               // 자원 상태
const POSITION map_pos = { 1, 1 };                   // 맵 출력 시작 좌표
const POSITION system_msg_pos = { MAP_HEIGHT + 1, 1 }; // 시스템 메시지
const POSITION object_info_pos = { 1, MAP_WIDTH + 2 }; // 상태창
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
	display_resource(resource);       // 자원 상태 출력
	display_map(map);                 // 맵 출력
	display_cursor(cursor);           // 커서 출력
	display_object_info(cursor, map); // 상태창 출력
	display_system_message();         // 시스템 메시지 출력
	display_commands();               // 명령창 출력
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

// frontbuf[][]에서 커서 위치의 문자를 색만 바꿔서 그대로 다시 출력
void display_cursor(CURSOR cursor) {
	POSITION prev = cursor.previous;
	POSITION curr = cursor.current;

	char ch = frontbuf[prev.row][prev.column];
	printc(padd(map_pos, prev), ch, COLOR_DEFAULT);

	ch = frontbuf[curr.row][curr.column];
	printc(padd(map_pos, curr), ch, COLOR_CURSOR);
}

void display_system_message() {
	gotoxy(system_msg_pos); // 시스템 메시지 위치
	set_color(COLOR_DEFAULT);
	printf("System Message: All systems operational.");
}




void display_object_info(CURSOR cursor, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	gotoxy(object_info_pos); // 상태창 위치
	set_color(COLOR_DEFAULT);

	// 커서 위치의 객체 정보를 출력
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
	gotoxy(command_pos); // 명령창 위치
	set_color(COLOR_DEFAULT);
	printf("Commands: Move, Attack, Harvest, Build");
}



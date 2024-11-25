/*
*  display.c:
* 화면에 게임 정보를 출력
* 맵, 커서, 시스템 메시지, 정보창, 자원 상태 등등
* io.c에 있는 함수들을 사용함
*/
#include "common.h"
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
	display_commands(cursor, map);               // 명령창 출력
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

				// 객체별 색상 적용
				int color = COLOR_DEFAULT;
				switch (ch) {
				case 'W': color = COLOR_SANDWORM; break; // 황토색
				case 'P': color = COLOR_PLATE; break;    // 흰색
				case 'R': color = COLOR_ROCK; break;     // 회색
				case '5': color = COLOR_SPICE; break;    // 주황색
				case 'H':
					if (i >= MAP_HEIGHT / 2) {
						color = COLOR_BLUE;             // 왼쪽 아래 H
					}
					else {
						color = COLOR_RED;              // 오른쪽 위 H
					}
					break;
				case 'B':
					if (i >= MAP_HEIGHT / 2) {
						color = COLOR_BLUE;             // 왼쪽 아래 B
					}
					else {
						color = COLOR_RED;              // 오른쪽 위 B
					}
					break;
				}

				printc(padd(map_pos, pos), ch, color);
			}
			frontbuf[i][j] = backbuf[i][j];
		}
	}
}


// frontbuf[][]에서 커서 위치의 문자를 색만 바꿔서 그대로 다시 출력
void display_cursor(CURSOR cursor) {
	POSITION prev = cursor.previous;
	POSITION curr = cursor.current;

	// 이전 위치의 문자와 색상 복원
	char ch = frontbuf[prev.row][prev.column];
	int color = COLOR_DEFAULT; // 기본 색상

	// 객체별 색상 적용 (display_map과 동일한 로직)
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

	// 현재 위치의 문자 강조 (커서 색상으로 표시)
	ch = frontbuf[curr.row][curr.column];
	printc(padd(map_pos, curr), ch, COLOR_CURSOR);
}


void display_system_message() {
	gotoxy(system_msg_pos); // 시스템 메시지 위치
	set_color(COLOR_DEFAULT);
	printf("System Message: All systems operational.");
}

void display_object_info(CURSOR cursor, char map[N_LAYER][MAP_WIDTH][MAP_HEIGHT]) {
	POSITION object_info_pos = { 1, MAP_WIDTH + 2 }; // 상태창 위치
	gotoxy(object_info_pos); // 상태창으로 이동
	set_color(COLOR_DEFAULT);

	// 상태창 초기화
	for (int i = 0; i < 5; i++) {
		gotoxy((POSITION) { object_info_pos.row + i, object_info_pos.column });
		printf("                                                   ");
	}
	gotoxy(object_info_pos);

	// 선택된 객체가 없을 경우 기본 메시지 출력
	if (selected_object == '\0') {
		printf("No object selected."); // 선택되지 않은 경우
		return;
	}

	// 유닛 정보 출력
	for (int i = 0; i < NUM_UNITS; i++) {
		if (selected_object == units[i].name[0]) {
			printf("Selected Unit: %s\n", units[i].name);
			printf("Cost: %d, Population: %d\n", units[i].cost, units[i].population);
			printf("Speed: %d, HP: %d\n", units[i].move_speed, units[i].health);
			printf("Commands: %s, %s\n", units[i].commands[0], units[i].commands[1]);
			return;
		}
	}

	// 건물 정보 출력
	for (int i = 0; i < NUM_BUILDINGS; i++) {
		if (selected_object == buildings[i].name[0]) {
			printf("Selected Building: %s\n", buildings[i].name);
			printf("Cost: %d, Durability: %d\n", buildings[i].build_cost, buildings[i].capacity);
			printf("Description: %s\n", buildings[i].description);
			printf("Commands: %s\n", buildings[i].commands[0]);
			return;
		}
	}

	// 빈 지형 정보 출력
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

	// 유닛 명령어 출력
	for (int i = 0; i < NUM_UNITS; i++) {  // 전역 변수 사용
		if (object == units[i].name[0]) {
			printf("Commands: %s, %s\n", units[i].commands[0], units[i].commands[1]);
			return;
		}
	}

	// 건물 명령어 출력
	for (int i = 0; i < NUM_BUILDINGS; i++) {  // 전역 변수 사용
		if (object == buildings[i].name[0]) {
			printf("Commands: %s\n", buildings[i].commands[0]);
			return;
		}
	}

	// 기본 메시지
	printf("No commands available.");
}

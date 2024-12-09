/*
*  display.c:
* 화면에 게임 정보를 출력
* 맵, 커서, 시스템 메시지, 정보창, 자원 상태 등등
* io.c에 있는 함수들을 사용함
*/
#include "common.h"
#include "display.h"
#include "io.h"
#include <string.h>
#define MAX_MSG_LENGTH 50
static char system_message[MAX_MSG_LENGTH] = "All systems operational."; // 시스템 메시지 저장 변수

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
	display_commands(cursor, map);               // 명령창 출력
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

void display_system_message(const char* message) {
	// 시스템 메시지 출력
	gotoxy(system_msg_pos); // 시스템 메시지 위치
	set_color(COLOR_DEFAULT);
	printf("시스템 메시지");

	// 메시지 내용 출력
	POSITION message_content_pos = { system_msg_pos.row + 1, system_msg_pos.column };
	gotoxy(message_content_pos);
	printf("                                          "); // 초기화
	gotoxy(message_content_pos);
	printf("%s", system_message);
}




void display_object_info(CURSOR cursor, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	// 상태창 제목 고정 출력
	gotoxy(object_info_pos);
	set_color(COLOR_DEFAULT);
	printf("상태창\n");

	// 상태창 초기화
	POSITION content_pos = { object_info_pos.row + 1, object_info_pos.column };
	for (int i = 0; i < 5; i++) {
		gotoxy((POSITION) { content_pos.row + i, content_pos.column });
		printf("                          "); // 상태창 영역만 초기화
	}

	gotoxy(content_pos);

	if (selected_object == '\0') {
		// 상태창에 기본 메시지 출력
		gotoxy(content_pos);
		printf("선택된 객체 없음.");
		return;
	}

	// 유닛 정보 출력
	for (int i = 0; i < NUM_UNITS; i++) {
		if (selected_object == units[i].symbol) {
			gotoxy(content_pos);
			printf("유닛: %s", units[i].name);

			gotoxy((POSITION) { content_pos.row + 1, content_pos.column });
			printf("비용: %d, 인구수: %d", units[i].cost, units[i].population);

			gotoxy((POSITION) { content_pos.row + 2, content_pos.column });
			printf("속도: %d, 체력: %d", units[i].move_speed, units[i].health);
			return;
		}
	}

	// 건물 정보 출력
	for (int i = 0; i < NUM_BUILDINGS; i++) {
		if (selected_object == buildings[i].symbol) {
			gotoxy(content_pos);
			printf("건물: %s", buildings[i].name);

			gotoxy((POSITION) { content_pos.row + 1, content_pos.column });
			printf("비용: %d, 내구도: %d", buildings[i].build_cost, buildings[i].capacity);

			gotoxy((POSITION) { content_pos.row + 2, content_pos.column });
			printf("설명: %s", buildings[i].description);
			return;
		}
	}

	// 빈 지형 정보 출력
	if (selected_object == ' ') {
		gotoxy(content_pos);
		printf("지형: 사막");

		gotoxy((POSITION) { content_pos.row + 1, content_pos.column });
		printf("건물을 지을 수 없음");
	}
}

void display_commands(CURSOR cursor, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	gotoxy(command_pos);
	set_color(COLOR_DEFAULT);
	printf("명령창\n");

	// 명령창 초기화
	POSITION content_pos = { command_pos.row + 1, command_pos.column };
	gotoxy(content_pos);
	printf("                                          "); // 초기화
	gotoxy(content_pos);

	if (selected_object == '\0') {
		printf("명령 없음.");
		return;
	}

	// 유닛 명령어 출력
	for (int i = 0; i < NUM_UNITS; i++) {
		if (selected_object == units[i].symbol) {
			printf("%s, %s", units[i].commands[0], units[i].commands[1]);
			return;
		}
	}

	// 건물 명령어 출력
	for (int i = 0; i < NUM_BUILDINGS; i++) {
		if (selected_object == buildings[i].symbol) {
			printf("%s", buildings[i].commands[0]);
			return;
		}
	}

	printf("명령 없음.");
}


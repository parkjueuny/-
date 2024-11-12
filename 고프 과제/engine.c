//engine.c

#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <windows.h>
#include <stdio.h>

#define MAP_WIDTH  60
#define MAP_HEIGHT 18

#include "common.h"
#include "io.h"
#include "display.h"

extern char frontbuf[MAP_HEIGHT][MAP_WIDTH];

// 색상 정의 (Windows 콘솔 색상 코드)
#define COLOR_DEFAULT 15     // 흰색 (기본 색상)
#define COLOR_BASE 4         // 빨간색 (B)
#define COLOR_HARVESTER 2    // 초록색 (H)
#define COLOR_PLATE 0        // 검정색 (P)
#define COLOR_SPICE 6        // 주황색 (5)
#define COLOR_SANDWORM 14    // 황토색 (W)
#define COLOR_ROCK 8         // 회색 (R)



void init(void);
void intro(void);
void outro(void);
void cursor_move(DIRECTION dir);
void sample_obj_move(void);
POSITION sample_obj_next_position(void);
void handle_selection(KEY key);

/* ================= control =================== */
int sys_clock = 0;		// system-wide clock(ms)
CURSOR cursor = { { 1, 1 }, {1, 1} };
RESOURCE resource = { 0, 100, 0, 50 };

/* ================= game data =================== */
char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH] = { 0 };


OBJECT_SAMPLE obj = {
	.pos = {1, 1},
	.dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2},
	.repr = ' ',
	.speed = 300,
	.next_move_time = 300
};

// 예제 데이터 초기화
BUILDING buildings[] = {
	{'B', "Base", 0, 50, "기본 건물", ' '},
	{'P', "Plate", 1, 0, "건물 짓기 전 준비", ' '},
	{'D', "Dormitory", 2, 10, "인구 최대치 증가", ' '},
	{'G', "Garage", 4, 10, "스파이스 보관 최대치 증가", ' '},
	{'B', "Barracks", 4, 20, "보병 생산", 'S'},
	{'S', "Shelter", 5, 10, "특수 유닛 생산", 'F'},
	{'A', "Arena", 3, 15, "투사 생산", 'F'},
	{'F', "Factory", 5, 30, "중전차 생산", 'T'}
};

UNIT units[] = {
	{'H', "Harvester", 2000, 5, 2000, 0, 0, 70, 'H'},
	{'S', "Soldier", 400, 1, 15, 8, 20, 10, 'M'},
	{'F', "Fremen", 1000, 1, 20, 15, 10, 30, 'M'},
	{'F', "Fighter", 1200, 1, 25, 20, 10, 40, 'M'},
	{'T', "Heavy Tank", 3000, 4, 10, 60, 4, 80, 'M'},
	{'W', "Sandworm", 0, 0, 2500, 10000, 0, 10000, ' '}
};

/* ================= main() =================== */
// engine.c 파일의 main 함수에서 키 입력 처리 부분을 다음과 같이 수정합니다.
int main(void) {
	srand((unsigned int)time(NULL));

	init();      // 초기 설정 함수 호출
	intro();     // 초기 메시지 출력
	display(resource, map, cursor);  // 첫 화면 표시

	while (1) {
		KEY key = get_key(); // 키 입력 대기

		// 키 입력에 따른 커서 이동 처리
		if (is_arrow_key(key)) {
			cursor_move(ktod(key));
		}
		else if (key == k_quit) {
			outro();  // 종료 키 입력 시 outro() 호출 및 프로그램 종료
		}

		// 선택(스페이스바) 및 취소(ESC 키) 핸들링
		handle_selection(key);

		// 화면 출력 (자원, 맵, 커서, 메시지 등 표시)
		display(resource, map, cursor);

		Sleep(TICK);   // 일정 주기 동안 대기 (10ms)
		sys_clock += 10;  // 시스템 클럭을 10ms 증가
	}
}



/* ================= subfunctions =================== */
void intro(void) {
	printf("DUNE 1.5\n");		
	Sleep(2000);
	system("cls");
}

void outro(void) {
	printf("exiting...\n");
	exit(0);
}






void init(void) {
	// Layer 0: 지형 설정 - 테두리 
	for (int j = 0; j < MAP_WIDTH; j++) {
		map[0][0][j] = '#';
		map[0][MAP_HEIGHT - 1][j] = '#';
		frontbuf[0][j] = '#';
		frontbuf[MAP_HEIGHT - 1][j] = '#';
		backbuf[0][j] = '#';
		backbuf[MAP_HEIGHT - 1][j] = '#';
	}

	for (int i = 1; i < MAP_HEIGHT - 1; i++) {
		map[0][i][0] = '#';
		map[0][i][MAP_WIDTH - 1] = '#';
		frontbuf[i][0] = '#';
		frontbuf[i][MAP_WIDTH - 1] = '#';
		backbuf[i][0] = '#';
		backbuf[i][MAP_WIDTH - 1] = '#';
		for (int j = 1; j < MAP_WIDTH - 1; j++) {
			map[0][i][j] = ' ';
			frontbuf[i][j] = ' ';
			backbuf[i][j] = ' ';
		}
	}

	

	// 지정된 좌표에 객체 배치
	// B (Base) 배치
	map[0][15][1] = 'B';
	map[0][15][2] = 'B';
	map[0][16][1] = 'B';
	map[0][16][2] = 'B';

	map[0][1][57] = 'B';
	map[0][1][58] = 'B';
	map[0][2][57] = 'B';
	map[0][2][58] = 'B';

	// H (Harvester) 배치
	map[1][14][1] = 'H';
	map[1][3][58] = 'H';

	// P (Plate) 배치
	map[0][15][4] = 'P';
	map[0][15][3] = 'P';
	map[0][16][3] = 'P';
	map[0][16][4] = 'P';

	map[0][1][55] = 'P';
	map[0][1][56] = 'P';
	map[0][2][55] = 'P';
	map[0][2][56] = 'P';

	// 5 (Spice 매장량) 배치
	map[1][9][1] = '5';
	map[1][8][58] = '5';

	// W (Sandworm) 배치
	map[0][2][4] = 'W';
	map[0][12][40] = 'W';

	// R (Rock) 배치 - 기존 및 추가된 위치
	map[0][10][30] = 'R';
	map[0][10][31] = 'R';
	map[0][11][30] = 'R';
	map[0][11][31] = 'R';

	map[0][3][28] = 'R';     // 추가된 Rock 위치
	map[0][3][29] = 'R';
	map[0][4][28] = 'R';
	map[0][4][29] = 'R';

	map[0][9][15] = 'R';     // 추가된 Rock 위치
	map[0][4][45] = 'R';     // 추가된 Rock 위치
	map[0][15][55] = 'R';    // 추가된 Rock 위치

	map[1][obj.pos.row][obj.pos.column] = 'o';
	
}



// (가능하다면) 지정한 방향으로 커서 이동
void cursor_move(DIRECTION dir) {
	POSITION curr = cursor.current;
	POSITION new_pos = pmove(curr, dir);

	if (1 <= new_pos.row && new_pos.row <= MAP_HEIGHT - 2 &&
		1 <= new_pos.column && new_pos.column <= MAP_WIDTH - 2) {

		// 이전 위치의 'o'를 초기화 (지우기)
		map[1][cursor.current.row][cursor.current.column] = ' ';
		frontbuf[cursor.current.row][cursor.current.column] = ' '; // frontbuf에서도 초기화

		// 커서 위치 업데이트
		cursor.previous = cursor.current;
		cursor.current = new_pos;

		// 새로운 위치에 'o' 설정
		map[1][cursor.current.row][cursor.current.column] = 'o';
		frontbuf[cursor.current.row][cursor.current.column] = 'o'; // frontbuf에서도 업데이트

		// 상태창 갱신: 커서 위치에 있는 지형/유닛 정보 표시
		char current_obj = map[0][cursor.current.row][cursor.current.column];
		BUILDING* selected_building = NULL;
		UNIT* selected_unit = NULL;

		if (current_obj == 'B') {
			selected_building = &buildings[0];
		}
		else if (current_obj == 'H') {
			selected_unit = &units[0];
		}

		display_object_info(selected_building, selected_unit);
	}
}






/* ================= sample object movement =================== */
POSITION sample_obj_next_position(void) {
	// 현재 위치와 목적지를 비교해서 이동 방향 결정	
	POSITION diff = psub(obj.dest, obj.pos);
	DIRECTION dir;
	// 목적지 도착. 지금은 단순히 원래 자리로 왕복
	if (diff.row == 0 && diff.column == 0) {
		if (obj.dest.row == 1 && obj.dest.column == 1) {
			// topleft --> bottomright로 목적지 설정
			POSITION new_dest = { MAP_HEIGHT - 2, MAP_WIDTH - 2 };
			obj.dest = new_dest;
		}
		else {
			// bottomright --> topleft로 목적지 설정
			POSITION new_dest = { 1, 1 };
			obj.dest = new_dest;
		}
		return obj.pos;
	}

	// 가로축, 세로축 거리를 비교해서 더 먼 쪽 축으로 이동
	if (abs(diff.row) >= abs(diff.column)) {
		dir = (diff.row >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.column >= 0) ? d_right : d_left;
	}

	// validation check
	// next_pos가 맵을 벗어나지 않고, (지금은 없지만)장애물에 부딪히지 않으면 다음 위치로 이동
	// 지금은 충돌 시 아무것도 안 하는데, 나중에는 장애물을 피해가거나 적과 전투를 하거나... 등등
	POSITION next_pos = pmove(obj.pos, dir);
	if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[1][next_pos.row][next_pos.column] < 0) {

		return next_pos;
	}
	else {
		return obj.pos;  // 제자리
	}
}
void sample_obj_move(void) {
	if (sys_clock <= obj.next_move_time) {
		// 아직 시간이 안 됐음
		return;
	}
	// 오브젝트(건물, 유닛 등)은 layer1(map[1])에 저장
	map[1][obj.pos.row][obj.pos.column] = -1;
	obj.pos = sample_obj_next_position();
	map[1][obj.pos.row][obj.pos.column] = obj.repr;
	obj.next_move_time = sys_clock + obj.speed;
}

// engine.c 파일에 새로운 handle_selection 함수 추가
void handle_selection(KEY key) {
	static bool is_selected = false;  // 선택 상태를 유지하는 플래그

	if (key == ' ') {  // 스페이스바 눌렀을 때
		char current_obj = map[0][cursor.current.row][cursor.current.column];

		BUILDING* selected_building = NULL;
		UNIT* selected_unit = NULL;

		// 현재 개체가 BUILDING인지 UNIT인지 확인
		if (current_obj == 'B') {
			selected_building = &buildings[0]; // 예시로 'B'는 Base 건물로 가정
		}
		else if (current_obj == 'H') {
			selected_unit = &units[0]; // 예시로 'H'는 Harvester 유닛으로 가정
		}

		// 선택된 개체에 따라 display_object_info 호출
		display_object_info(selected_building, selected_unit);
		is_selected = true;
	}
	else if (key == k_quit && is_selected) {  // ESC 키로 선택 취소
		display_object_info(NULL, NULL);  // 빈 상태창으로 초기화
		is_selected = false;
	}
}


﻿//engine.c파일

#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "common.h"
#include "io.h"
#include "display.h"


void init(void);
void intro(void);
void outro(void);
void cursor_move(DIRECTION dir);
void sample_obj_move(void);
POSITION sample_obj_next_position(void);


/* ================= control =================== */
int sys_clock = 0;		// system-wide clock(ms)
CURSOR cursor = { { 1, 1 }, {1, 1} };


/* ================= game data =================== */
char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH] = { 0 };

RESOURCE resource = {
	.spice = 0,
	.spice_max = 0,
	.population = 0,
	.population_max = 0
};

OBJECT_SAMPLE obj = {
	.pos = {1, 1},
	.dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2},
	.repr = 'o',
	.speed = 300,
	.next_move_time = 300
};

// 유닛 데이터 초기화
UNIT units[] = {
	{"Harvester", 5, 5, 2000, 0, 0, 70, 0, {"Harvest", "Move"}},
	{"Fremen", 5, 2, 400, 15, 2, 8, 0, {"Move", "Attack"}},
	{"Soldier", 5, 2, 800, 15, 3, 15, 0, {"Move", "Attack"}},
	{"Fighter", 5, 2, 1500, 30, 5, 20, 0, {"Move", "Attack"}},
	{"Heavy Tank", 5, 2, 1000, 50, 7, 50, 5, {"Move", "Attack"}}
};

// 건물 데이터 초기화
BUILDING buildings[] = {
	{"Base", 0, 50, "Main base. Produces Harvesters.", {"H"}},
	{"Plate", 0, 50, "Build before constructing buildings.", {""}},
	{"Dormitory", 2, 10, "Increases max population.", {""}},
	{"Garage", 4, 10, "Increases spice capacity.", {""}},
	{"Barracks", 5, 30, "Produces Soldiers.", {"S"}},
	{"Shelter", 5, 30, "Produces Fremen.", {"F"}},
	{"Arena", 5, 15, "Produces Fighters.", {"A"}},
	{"Factory", 5, 30, "Produces Heavy Tanks.", {"T"}}
};


/* ================= main() =================== */
int main(void) {
	srand((unsigned int)time(NULL));

	init();
	intro();
	display(resource, map, cursor);

	while (1) {
		// loop 돌 때마다(즉, TICK==10ms마다) 키 입력 확인
		KEY key = get_key();

		// 키 입력이 있으면 처리
		if (is_arrow_key(key)) {
			cursor_move(ktod(key));
		}
		else {
			// 방향키 외의 입력
			switch (key) {
			case k_quit: outro();
			case k_none:
			case k_undef:
			default: break;
			}
		}

		// 샘플 오브젝트 동작
		sample_obj_move();

		// 화면 출력
		display(resource, map, cursor);
		Sleep(TICK);
		sys_clock += 10;
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
	// layer 0(map[0])에 지형 생성
	for (int j = 0; j < MAP_WIDTH; j++) {
		map[0][0][j] = '#';
		map[0][MAP_HEIGHT - 1][j] = '#';
	}

	for (int i = 1; i < MAP_HEIGHT - 1; i++) {
		map[0][i][0] = '#';
		map[0][i][MAP_WIDTH - 1] = '#';
		for (int j = 1; j < MAP_WIDTH - 1; j++) {
			map[0][i][j] = ' ';
		}
	}

	// layer 1(map[1])은 비워 두기(-1로 초기화)
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			map[1][i][j] = -1;
		}
	}

	// 테두리 안 객체 배치
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

	// object sample
	map[1][obj.pos.row][obj.pos.column] = 'o';
}


// (가능하다면) 지정한 방향으로 커서 이동
void cursor_move(DIRECTION dir) {
	POSITION curr = cursor.current;
	POSITION new_pos = pmove(curr, dir);

	// validation check
	if (1 <= new_pos.row && new_pos.row <= MAP_HEIGHT - 2 && \
		1 <= new_pos.column && new_pos.column <= MAP_WIDTH - 2) {

		cursor.previous = cursor.current;
		cursor.current = new_pos;
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
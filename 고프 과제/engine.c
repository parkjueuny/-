//engine.c파일

#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "common.h"
#include "io.h"
#include "display.h"

// 선택된 객체 정보 (현재 선택된 객체의 문자를 저장)
char selected_object = '\0';  // '\0'은 선택된 객체가 없음을 나타냄
void init(void);
void intro(void);
void outro(void);
void cursor_move(DIRECTION dir,int double_click);
void sample_obj_move(void);
void handle_spacebar(CURSOR cursor, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);
void update_sandworm(Sandworm* worm, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);
void initialize_sandworm(Sandworm* worm, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);
POSITION sample_obj_next_position(void);
POSITION find_closest_object(Sandworm* worm, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);

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
int last_key_time = 0;  // 마지막 키 입력 시간 기록

int main(void) {
	srand((unsigned int)time(NULL));

	init();                      // 게임 초기화
	intro();                     // 게임 소개 화면
	display(resource, map, cursor);

	while (1) {
		KEY key = get_key();

		if (is_arrow_key(key)) {
			int double_click = (sys_clock - last_key_time <= DOUBLE_CLICK_DELAY);
			cursor_move(ktod(key), double_click);
			last_key_time = sys_clock;
		}
		else if (key == ' ') {
			handle_spacebar(cursor, map);
			display_object_info(cursor, map);
		}
		else if (key == k_escape) {
			selected_object = '\0';
			display_object_info(cursor, map);
		}
		else {
			switch (key) {
			case k_quit:
				outro();
				return 0;
			case k_none:
			case k_undef:
			default:
				break;
			}
		}
		Sandworm worm;
		initialize_sandworm(&worm, map);  // 샌드웜 초기화
		update_sandworm(&worm, map);  // 샌드웜 업데이트
		sample_obj_move();
		display(resource, map, cursor);
		Sleep(TICK);
		sys_clock += TICK;
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
// 테두리 경계 체크 함수
bool is_within_bounds(POSITION pos) {
	return pos.row >= 1 && pos.row < MAP_HEIGHT - 1 &&
		pos.column >= 1 && pos.column < MAP_WIDTH - 1;
}

// 커서 이동 함수
void cursor_move(DIRECTION dir, int double_click) {
	POSITION curr = cursor.current;
	POSITION new_pos = curr;

	// 더블클릭 여부에 따른 이동 거리 설정
	int move_distance = double_click ? MULTI_MOVE_DISTANCE : 1;

	for (int i = 0; i < move_distance; i++) {
		new_pos = pmove(new_pos, dir);

		// 맵 경계 체크: 테두리 안에서만 이동 가능
		if (!is_within_bounds(new_pos)) {
			break; // 경계를 넘으면 이동 중단
		}
	}

	// 커서 위치 업데이트 (경계 안에서만 갱신)
	if (is_within_bounds(new_pos)) {
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

void handle_spacebar(CURSOR cursor, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	char object = map[1][cursor.current.row][cursor.current.column];

	// 유닛 선택
	for (int i = 0; i < NUM_UNITS; i++) {
		if (object == units[i].name[0]) {
			selected_object = object; // 선택된 객체 저장
			return;
		}
	}

	// 건물 선택
	object = map[0][cursor.current.row][cursor.current.column];
	for (int i = 0; i < NUM_BUILDINGS; i++) {
		if (object == buildings[i].name[0]) {
			selected_object = object; // 선택된 객체 저장
			return;
		}
	}

	// 빈 지형 선택
	if (object == ' ') {
		selected_object = ' '; // 빈 지형으로 설정
		return;
	}

	// 아무것도 선택되지 않음
	selected_object = '\0';
}

void eat_unit(Sandworm* worm, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	if (map[1][worm->row][worm->column] == 'U') {
		map[1][worm->row][worm->column] = ' ';  // 유닛 제거
		printf("Sandworm has eaten a unit at [%d, %d]!\n", worm->row, worm->column);
	}
}

void initialize_sandworm(Sandworm* worm, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (map[0][i][j] == 'W') {  // 샌드웜 발견
				worm->row = i;
				worm->column = j;
				worm->active = 1;  // 활성화
				worm->next_move_time = sys_clock + 1000;  // 1초 후 이동 시작
				worm->speed = 1000;  // 1초마다 이동
				return;
			}
		}
	}

	// 샌드웜이 맵에 없으면 비활성 상태로 초기화
	worm->row = -1;
	worm->column = -1;
	worm->active = 0;
	worm->next_move_time = 0;
	worm->speed = 0;
}

void update_sandworm(Sandworm* worm, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	// 샌드웜이 비활성 상태라면 아무 작업도 하지 않음
	if (!worm->active) return;

	// 이동 속도 제한
	static int next_move_time = 0; // 다음 이동 시점
	if (sys_clock < next_move_time) return; // 아직 이동 시간이 아니라면 리턴

	// 가까운 객체 찾기
	POSITION target = find_closest_object(worm, map);
	if (target.row == -1 || target.column == -1) return; // 이동할 대상이 없으면 종료

	// 현재 위치 초기화
	map[0][worm->row][worm->column] = ' ';

	// 한 칸 이동 로직
	if (worm->row < target.row) worm->row++;
	else if (worm->row > target.row) worm->row--;

	if (worm->column < target.column) worm->column++;
	else if (worm->column > target.column) worm->column--;

	// 새 위치에 샌드웜 갱신
	map[0][worm->row][worm->column] = 'W';

	// 다음 이동 시점 설정 (샌드웜 이동 속도: 500ms)
	next_move_time = sys_clock + 500;
}



POSITION find_closest_object(Sandworm* worm, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	int min_distance = INT_MAX;
	POSITION closest_object = { -1, -1 };  // 초기값은 없음을 의미

	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			char obj = map[0][i][j];
			if (obj != ' ' && obj != 'W' && obj != '#') {  // 샌드웜, 공백, 테두리 제외
				int distance = abs(worm->row - i) + abs(worm->column - j);  // 맨해튼 거리 계산
				if (distance < min_distance) {
					min_distance = distance;
					closest_object.row = i;
					closest_object.column = j;
				}
			}
		}
	}
	return closest_object;  // 가장 가까운 객체 반환
}


/*
* common.h
* 공통으로 사용되는 구조체와 상수들
*/

#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdbool.h>
#include <windows.h>
#include <conio.h>
#include <assert.h>

#define TICK 10
#define N_LAYER 2
#define MAP_WIDTH 60
#define MAP_HEIGHT 18
#define COLOR_RESOURCE 112
#define COLOR_DEFAULT 15
#define COLOR_CURSOR 240

typedef struct {
	int row, column;
} POSITION;

typedef struct {
	POSITION previous;
	POSITION current;
} CURSOR;

typedef enum {
	k_none = 0, k_up, k_right, k_left, k_down,
	k_quit,
	k_undef
} KEY;

typedef enum {
	d_stay = 0, d_up, d_right, d_left, d_down
} DIRECTION;

inline POSITION padd(POSITION p1, POSITION p2) {
	POSITION p = { p1.row + p2.row, p1.column + p2.column };
	return p;
}

inline POSITION psub(POSITION p1, POSITION p2) {
	POSITION p = { p1.row - p2.row, p1.column - p2.column };
	return p;
}

#define is_arrow_key(k) (k_up <= (k) && (k) <= k_down)
#define ktod(k) (DIRECTION)(k)

inline POSITION dtop(DIRECTION d) {
	static POSITION direction_vector[] = { {0, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 0} };
	return direction_vector[d];
}

#define pmove(p, d) (padd((p), dtop(d)))

typedef struct {
	int spice;
	int spice_max;
	int population;
	int population_max;
} RESOURCE;

typedef struct {
	POSITION pos;
	POSITION dest;
	char repr;
	int move_period;
	int next_move_time;
	int speed;
} OBJECT_SAMPLE;

typedef struct {
	char type;
	char name[20];
	int cost;
	int capacity;
	char description[50];
	char shortcut;
} BUILDING;

typedef struct {
	char type;
	char name[20];
	int cost;
	int population;
	int speed;
	int attack;
	int range;
	int health;
	char shortcut;
} UNIT;

#endif

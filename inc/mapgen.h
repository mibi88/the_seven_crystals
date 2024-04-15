#ifndef MAPGEN_H
#define MAPGEN_H

#define ROOM_W     16
#define ROOM_H     16

#define LEVEL_W    6
#define LEVEL_H    6

/* The levels for the crystals + the level for the boss fight */
#define DUNGEON_SZ 8

#include <stdlib.h>
#include <tilemap.h>

typedef struct {
    int generated;
    int left, right, up, down; /* If this room can connect to these sides */
    unsigned char data[ROOM_W*ROOM_H];
} Room;

typedef struct {
    Room data[LEVEL_W*LEVEL_H];
    Tilemap tilemap;
    unsigned char tiles[LEVEL_W*ROOM_W*LEVEL_H*ROOM_H];
    unsigned char light[LEVEL_W*ROOM_W*LEVEL_H*ROOM_H];
} Level;

typedef struct {
    Level levels[DUNGEON_SZ];
} Dungeon;

void generate_dungeon(Dungeon *dungeon, int seed);

#endif

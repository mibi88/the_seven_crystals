#ifndef TILEMAP_H
#define TILEMAP_H

#define ATLAS_W      16
#define ATLAS_H      16
#define TILE_W       64
#define TILE_H       64
#define DRAWING_W    (WIDTH/TILE_W+3)
#define DRAWING_H    (HEIGHT/TILE_H+3)

#define LIGHT_LEVELS 10
#define PLAYER_LIGHT 7
#define MIN_LIGHT    26

#define LIGHT_TILES 3

#include <gfx.h>

typedef struct {
    int width, height;
    unsigned char *data;
    unsigned char *light;
} Tilemap;

typedef struct {
    int x, y;
} IntVector;

void tilemap_render(Tilemap *tilemap, GFXSprite *tiles, int x, int y);
unsigned char tilemap_get_tile(Tilemap *tilemap, int x, int y);
void tilemap_replace(Tilemap *tilemap, unsigned char from, unsigned char to);
int tilemap_set_tile(Tilemap *tilemap, int x, int y, unsigned char tile);
void tilemap_replace(Tilemap *tilemap, unsigned char from, unsigned char to);
int tilemap_count_tiles(Tilemap *tilemap, unsigned char tile);

#endif

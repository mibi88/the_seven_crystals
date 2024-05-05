#ifndef DATA_H
#define DATA_H

#define STEP_SOUND "step.wav"
#define PLACE_SOUND "place.wav"
#define GRAB_SOUND "grab.wav"
#define ATTACK_SOUND "attack.wav"
#define HURTS_SOUND "hurts.wav"

#define SPIDER_SPEED    400

#define BULLET_MAX      16
#define MOBS_MAX        16
#define MOB_SPEED       100

#define SHOOT_FREQUENCY 90
#define TILESET_NUM     4

#include <gfx.h>
#include <mapgen.h>
#include <entity.h>
#include <bullet.h>

typedef struct {
    GFXSprite font;
    GFXSprite tileset1, tileset2, tileset3, tileset4, tileset5, tileset6, tileset7,
              tileset8;
    GFXSprite player_img;
    GFXSprite slug;
    GFXSprite zombie;
    GFXSprite cannon;
    GFXSprite bullet;
    GFXSprite boss1_img, boss2_img, boss3_img, boss4_img, boss5_img, boss6_img;
    GFXSprite boss7_img, megaboss_img;
    
    int screen;
    int deaths;
    int mob_limit;

    int map_n;
    Dungeon dungeon;

    int mob_num_limit;
    int min_xp;
    int bossfight;

    int current_set;
    int crystals;

    Direction boss_direction;
    Entity cannons[4];

    Entity mobs[MOBS_MAX];
    Bullet bullets[BULLET_MAX];
} Data;

void load_assets(Data *data);

#endif

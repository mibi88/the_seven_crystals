#ifndef DATA_H
#define DATA_H

#define STEP_SOUND         "step.wav"
#define PLACE_SOUND        "place.wav"
#define GRAB_SOUND         "grab.wav"
#define ATTACK_SOUND       "attack.wav"
#define HURTS_SOUND        "hurts.wav"

#define SPIDER_SPEED       400

#define BULLET_MAX         16
#define MOBS_MAX           16
#define MOB_SPEED          100

#define SHOOT_FREQUENCY    90
#define TILESET_NUM        4

#define MSG_KILL           ("Kill more mobs! Use the space bar to\n" \
                            "attack!")

#define MSG_SUMMON_BOSS    ("Summon the boss!\n" \
                            "Press space while standing on the white\n" \
                            "portal")

#define MSG_PLACE_CRYSTALS ("Put the crystals in their supports\n" \
                            "Press space while standing on the\n" \
                            "support")

#define MSG_SUMMON_BOSS1   ("Go to the room with water to summon\n" \
                            "the boss!\n" \
                            "Press space while standing on the white\n" \
                            "portal")

#define MAX_LEN            16

#include <gfx.h>
#include <mapgen.h>
#include <entity.h>
#include <bullet.h>

typedef struct {
    GFXSprite font;
    GFXSprite title;
    GFXSprite tileset1, tileset2, tileset3, tileset4, tileset5, tileset6,
              tileset7, tileset8;
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

/* TODO: Game struct */
extern int fps, current_frames;
extern int last_time;
extern int last_step;
extern char fps_str[30];
extern char info_str[100];
extern char boss_str[30];

extern const Entity base_player;
extern Entity player;

extern float force;

extern char crystals_str[80];

extern const GFXColor red;
extern const GFXColor white;

extern char seed[16];

extern int cur;

extern SDLKey keys[10];

extern char values[10];

extern int frame;
extern int frame2;

extern Data data;

extern GFXSprite *tilesets[TILESET_NUM*2];;

extern GFXSprite *tileset;

#endif

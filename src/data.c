#include <data.h>

/* GAME DATA */
int fps = 60, current_frames = 0;
int last_time = 0;
int last_step = 0;
char fps_str[30];
char info_str[100];
char boss_str[30];

const Entity base_player = {0, 0, 0, 0, 0, 10, 128.0, 128.0, D_DOWN, 128, 0,
                            D_UP, 0, 0};
Entity player = base_player;

float force = 1.5;

char crystals_str[80];

const GFXColor red = {1.0, 0.5, 0.5};
const GFXColor white = {1.0, 1.0, 1.0};

char seed[16] = "77";

int cur = 2;

SDLKey keys[10] = {
    SDLK_0,
    SDLK_1,
    SDLK_2,
    SDLK_3,
    SDLK_4,
    SDLK_5,
    SDLK_6,
    SDLK_7,
    SDLK_8,
    SDLK_9,
};

char values[10] = "0123456789";

int frame = 0;
int frame2 = 0;

Data data;

GFXSprite *tilesets[TILESET_NUM*2] = {
    &data.tileset1,
    &data.tileset2,
    &data.tileset3,
    &data.tileset4,
    &data.tileset5,
    &data.tileset6,
    &data.tileset7,
    &data.tileset8
};

GFXSprite *tileset;
/*************/

void load_assets(Data *data) {
    data->font = gfx_load_texture("font.png", 128, 128);
    data->title = gfx_load_texture("title.png", 160, 120);
    data->tileset1 = gfx_load_texture("tileset1.png", 256, 256);
    data->tileset2 = gfx_load_texture("tileset2.png", 256, 256);
    data->tileset3 = gfx_load_texture("tileset3.png", 256, 256);
    data->tileset4 = gfx_load_texture("tileset4.png", 256, 256);
    data->tileset5 = gfx_load_texture("tileset5.png", 256, 256);
    data->tileset6 = gfx_load_texture("tileset6.png", 256, 256);
    data->tileset7 = gfx_load_texture("tileset7.png", 256, 256);
    data->tileset8 = gfx_load_texture("tileset8.png", 256, 256);
    data->player_img = gfx_load_texture("player.png", 64, 64);
    data->slug = gfx_load_texture("slug.png", 64, 64);
    data->zombie = gfx_load_texture("zombie.png", 64, 64);
    data->cannon = gfx_load_texture("cannon.png", 64, 64);
    data->bullet = gfx_load_texture("bullet.png", 5, 5);
    data->boss1_img = gfx_load_texture("boss1.png", 256, 256);
    data->boss2_img = gfx_load_texture("boss2.png", 256, 256);
    data->boss3_img = gfx_load_texture("boss3.png", 256, 256);
    data->boss4_img = gfx_load_texture("boss4.png", 256, 256);
    data->boss5_img = gfx_load_texture("boss5.png", 256, 256);
    data->boss6_img = gfx_load_texture("boss6.png", 256, 256);
    data->boss7_img = gfx_load_texture("boss7.png", 256, 256);
    data->megaboss_img = gfx_load_texture("megaboss.png", 256, 256);
}

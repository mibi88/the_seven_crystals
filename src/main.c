#include <gfx.h>
#include <tilemap.h>

#include <mapgen.h>

#include <entity.h>

#include <bullet.h>

#include <SDL/SDL_mixer.h>

#define SPEED 250
#define SLUG_SPEED 100
#define SPIDER_SPEED 400
#define MOBS_MAX 16
#define BULLET_MAX 16
#define SHOOT_FREQUENCY 90
#define TILESET_NUM     4

#define MAX_LEN 16

#define VERSION "v.1.1"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
EM_JS(void, play_audio, (char *file), {
    var audio = new Audio(UTF8ToString(file));
    audio.play();
})
#endif

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

#ifndef __EMSCRIPTEN__
Mix_Chunk *hurts;
Mix_Chunk *attacking;
Mix_Chunk *step;
Mix_Chunk *place;
Mix_Chunk *grab;
#endif

int fps = 60, current_frames = 0;
int last_time = 0;
int last_step = 0;
char fps_str[30];
char info_str[100];
char boss_str[30];

Dungeon dungeon;

const Entity base_player = {0, 0, 0, 0, 0, 10, 128.0, 128.0, D_DOWN, 128, 0,
                            D_UP, 0};
Entity player = base_player;
Entity mobs[MOBS_MAX];
Bullet bullets[BULLET_MAX];
const Entity base_mob = {0, 0, 0, 0, 0, 5, 0.0, 0.0, D_LEFT, 20, 0, D_RIGHT, 0,
                         0};
const Entity boss1 = {0, 0, 0, 0, 0, 20, 0.0, 0.0, D_LEFT, 32, 0, D_RIGHT, 0,
                      3};
const Entity boss2 = {0, 0, 0, 0, 0, 30, 0.0, 0.0, D_LEFT, 32, 0, D_RIGHT, 0,
                      4};
const Entity boss3 = {0, 0, 0, 0, 0, 40, 0.0, 0.0, D_LEFT, 32, 0, D_RIGHT, 0,
                      5};
const Entity boss4 = {0, 0, 0, 0, 0, 50, 0.0, 0.0, D_LEFT, 32, 0, D_RIGHT, 0,
                      6};
const Entity boss5 = {0, 0, 0, 0, 0, 60, 0.0, 0.0, D_LEFT, 32, 0, D_RIGHT, 0,
                      7};
const Entity boss6 = {0, 0, 0, 0, 0, 65, 0.0, 0.0, D_LEFT, 32, 0, D_RIGHT, 0,
                      8};
const Entity boss7 = {0, 0, 0, 0, 0, 70, 0.0, 0.0, D_LEFT, 32, 0, D_RIGHT, 0,
                      9};
const Entity megaboss = {0, 0, 0, 0, 0, 100, 0.0, 0.0, D_LEFT, 32, 0, D_RIGHT,
                         0, 10};

int map_n = 0;

float force = 1.5;

int mob_limit = 1;
int mob_num_limit = 4;

int min_xp = 5;

int bossfight = 0;

int crystals = 0;
char crystals_str[80];

int screen = 0;

int deaths = 0;

const GFXColor red = {1.0, 0.5, 0.5};
const GFXColor white = {1.0, 1.0, 1.0};

Vector spawn_pos[4] = {
    {-128, 0},
    {0, -128},
    {128, 0},
    {0, 128}
};

Direction boss_direction = D_LEFT;

Entity cannons[4];

char seed[16] = "77";

int cur = 2;

int keys[10] = {
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

GFXSprite *tilesets[TILESET_NUM*2] = {
    &tileset1,
    &tileset2,
    &tileset3,
    &tileset4,
    &tileset5,
    &tileset6,
    &tileset7,
    &tileset8
};

GFXSprite *tileset;

int current_set = 0;

void reset(void) {
    int i;
    
    for(i=0;i<MOBS_MAX;i++){
        mobs[i].lives = 0;
    }
    
    player = base_player;
    map_n = 0;
    force = 1.5;
    mob_limit = 1;
    mob_num_limit = 4;
    min_xp = 5;
    bossfight = 0;
    crystals = 0;
    deaths = 0;
    frame = 0;
    frame2 = 0;
    current_set = 0;
}

void spawn_mobs(void) {
    int x, y, i;
    for(y=-256;y<256;y++){
        for(x=-256;x<256;x++){
            for(i=0;i<mob_num_limit;i++){
                if(mobs[i].lives <= 0){
                    if(tilemap_get_tile(&dungeon.levels[map_n].tilemap,
                                        player.x+x, player.x+y) == 2 &&
                                        rand()%100000 == 0){
                        mobs[i] = base_mob;
                        mobs[i].x = player.x+x;
                        mobs[i].y = player.y+y;
                        mobs[i].type = rand()%mob_limit;
                        switch(mobs[i].type){
                            case 1:
                                mobs[i].lives = 10;
                                break;
                            case 2:
                                mobs[i].lives = 15;
                                break;
                        }
                        break;
                    }
                }else if(mobs[i].type <= 2){
                    if(entity_get_distance(&player, &mobs[i]) > 24*64){
                        mobs[i].lives = -1;
                        break;
                    }
                }
            }
        }
    }
}

void attack(void) {
    int min_dist = player.attack_radius+(bossfight ? 128 : 0);
    int n = -1;
    int i;
    float distance;
    for(i=0;i<MOBS_MAX;i++){
        if(mobs[i].lives > 0){
            distance = entity_get_distance(&player, &mobs[i]);
            if(distance < min_dist){
                min_dist = distance;
                n = i;
            }
        }
    }
    if(n >= 0){
#ifndef __EMSCRIPTEN__
        Mix_PlayChannel(-1, attacking, 0);
#else
        play_audio("wav/attack.wav");
#endif
        mobs[n].lives -= force+0.25*((float)player.xp/100)*(rand()%2);
        mobs[n].damaged = 1;
        mobs[n].knockback_force = 1600;
        if(mobs[n].lives <= 0){
            if(rand()%5 == 0){
                player.lives += 0.5+0.5*((float)player.xp/100)*(rand()%2);
            }
            if(rand()%3 == 0){
                player.xp += rand()%12;
            }
        }
    }
}

void game_loop(void) {
    SDL_Event event;
    double delta = 1;
    int i, n, frequency, pos;
    
    tileset = tilesets[frame%2+current_set*2];
    if(fps){
        delta = 1/(double)fps;
    }
    gfx_clear();
    sprintf(crystals_str, "Crystals: %d | Deaths: %d", crystals, deaths);
    sprintf(info_str, "HP: %.02f | XP: %d/%d | LVL: %d",
            player.lives, player.xp, min_xp, map_n+1);
    sprintf(fps_str, "FPS: %d", fps);
    tilemap_render(&dungeon.levels[map_n].tilemap, tileset,
                   (int)player.x-WIDTH/2, (int)player.y-HEIGHT/2);
    
    for(i=0;i<MOBS_MAX;i++){
        if(mobs[i].lives > 0){
            entity_process_knockback(&mobs[i], &dungeon.levels[map_n].tilemap,
                                     delta, 200);
            if(mobs[i].damaged){
                slug.color = red;
                zombie.color = red;
                cannon.color = red;
                boss1_img.color = red;
                boss2_img.color = red;
                boss3_img.color = red;
                boss4_img.color = red;
                boss5_img.color = red;
                boss6_img.color = red;
                boss7_img.color = red;
                megaboss_img.color = red;
            }else{
                slug.color = white;
                zombie.color = white;
                cannon.color = white;
                boss1_img.color = white;
                boss2_img.color = white;
                boss3_img.color = white;
                boss4_img.color = white;
                boss5_img.color = white;
                boss6_img.color = white;
                boss7_img.color = white;
                megaboss_img.color = white;
            }
            pos = mobs[i].facing%D_AMOUNT+4*(frame%2+1);
            switch(mobs[i].type){
                case 0:
                    pos = mobs[i].facing%D_AMOUNT+4*frame2;
                    gfx_draw_sprite_from_atlas(&slug,
                                               mobs[i].x-32-player.x+WIDTH/2,
                                               mobs[i].y-32-player.y+HEIGHT/2,
                                               4, 4, pos, 64, 64, 0);
                    break;
                case 1:
                    gfx_draw_sprite_from_atlas(&zombie,
                                               mobs[i].x-32-player.x+WIDTH/2,
                                               mobs[i].y-32-player.y+HEIGHT/2,
                                               4, 4, pos,
                                               64, 64, 0);
                    break;
                case 2:
                    gfx_draw_sprite_from_atlas(&cannon,
                                               mobs[i].x-32-player.x+WIDTH/2,
                                               mobs[i].y-32-player.y+HEIGHT/2,
                                               4, 4, pos, 64, 64, 0);
                    break;
                case 3:
                    gfx_draw_sprite_from_atlas(&boss1_img,
                                               mobs[i].x-128-player.x+WIDTH/2,
                                               mobs[i].y-128-player.y+HEIGHT/2,
                                               4, 4, mobs[i].facing+frame*4,
                                               256, 256, 0);
                   break;
               case 4:
                    gfx_draw_sprite_from_atlas(&boss2_img,
                                               mobs[i].x-128-player.x+WIDTH/2,
                                               mobs[i].y-128-player.y+HEIGHT/2,
                                               4, 4, pos, 256, 256, 0);
                   break;
               case 5:
                    gfx_draw_sprite_from_atlas(&boss3_img,
                                               mobs[i].x-128-player.x+WIDTH/2,
                                               mobs[i].y-128-player.y+HEIGHT/2,
                                               1, 1, 0, 256, 256, 0);
                   break;
               case 6:
                    gfx_draw_sprite_from_atlas(&boss4_img,
                                               mobs[i].x-128-player.x+WIDTH/2,
                                               mobs[i].y-128-player.y+HEIGHT/2,
                                               1, 1, 0, 256, 256, 0);
                   break;
               case 7:
                    gfx_draw_sprite_from_atlas(&boss5_img,
                                               mobs[i].x-128-player.x+WIDTH/2,
                                               mobs[i].y-128-player.y+HEIGHT/2,
                                               1, 1, 0, 256, 256, 0);
                   break;
               case 8:
                    gfx_draw_sprite_from_atlas(&boss6_img,
                                               mobs[i].x-128-player.x+WIDTH/2,
                                               mobs[i].y-128-player.y+HEIGHT/2,
                                               1, 1, 0, 256, 256, 0);
                   break;
               case 9:
                    gfx_draw_sprite_from_atlas(&boss7_img,
                                               mobs[i].x-128-player.x+WIDTH/2,
                                               mobs[i].y-128-player.y+HEIGHT/2,
                                               1, 1, 0, 256, 256, 0);
                   break;
               case 10:
                    gfx_draw_sprite_from_atlas(&megaboss_img,
                                               mobs[i].x-128-player.x+WIDTH/2,
                                               mobs[i].y-128-player.y+HEIGHT/2,
                                               1, 1, 0, 256, 256, 0); 
                   break;
            }
            
            slug.color = white;
            if(mobs[i].type <= 2 || mobs[i].type == 9){
                if(player.x < mobs[i].x){
                    entity_move(&mobs[i], &dungeon.levels[map_n].tilemap,
                                D_LEFT, delta, SLUG_SPEED);
                }
                if(player.x > mobs[i].x){
                    entity_move(&mobs[i], &dungeon.levels[map_n].tilemap,
                                D_RIGHT, delta, SLUG_SPEED);
                }
                if(player.y < mobs[i].y){
                    entity_move(&mobs[i], &dungeon.levels[map_n].tilemap, D_UP,
                                delta, SLUG_SPEED);
                }
                if(player.y > mobs[i].y){
                    entity_move(&mobs[i], &dungeon.levels[map_n].tilemap,
                                D_DOWN, delta, SLUG_SPEED);
                }
            }
            if(entity_in_attack_radius(&mobs[i], &player)) {
                player.damaged = 1;
                if(player.knockback_force <= 0){
#ifndef __EMSCRIPTEN__
                    Mix_PlayChannel(-1, hurts, 0);
#else
                    play_audio("wav/hurts.wav");
#endif
                    player.knockback_force = 800;
                    switch(mobs[i].type){
                        case 1:
                            player.lives -= 1.5;
                            break;
                        case 2:
                            player.lives -= 0.25;
                            break;
                        default:
                            if(mobs[i].type >= 3){
                                player.lives -= 5;
                            }else{
                                player.lives -= 0.5;
                            }
                    }
                    if(rand()%5 == 0){
                        mobs[i].xp += rand()%5;
                    }
                    player.lives -= 0.25*((float)mobs[i].xp/100)*(rand()%2);
                    if(player.lives < 0){
                        screen = 3;
                        deaths++;
                        player.left = 0;
                        player.right = 0;
                        player.up = 0;
                        player.down = 0;
                    }
                }
            }
        }
    }
    for(n=0;n<BULLET_MAX;n++){
        if(bullets[n].exists){
            gfx_draw_sprite_from_atlas(&bullet,
                                       bullets[n].pos.x-10-player.x+WIDTH/2,
                                       bullets[n].pos.y-10-player.y+HEIGHT/2,
                                       1, 1, 0, 20, 20, 0);
            if(process_bullet(&bullets[n], &mobs[i], &player, delta)){
#ifndef __EMSCRIPTEN__
                Mix_PlayChannel(-1, hurts, 0);
#else
                play_audio("wav/hurts.wav");
#endif
                player.lives -= 2-(0.25*((float)player.xp/100)*(rand()%2));
                player.damaged = 1;
                if(player.lives < 0){
                    screen = 3;
                    deaths++;
                    player.left = 0;
                    player.right = 0;
                    player.up = 0;
                    player.down = 0;
                }
            }
        }
    }
    
    if(player.damaged){
        player_img.color = red;
    }else{
        player_img.color = white;
    }
    if(player.left || player.right || player.up || player.down){
        gfx_draw_sprite_from_atlas(&player_img, WIDTH/2-32, HEIGHT/2-32, 4, 4,
                                   player.facing+4*(frame%2+1), 64, 64, 0);
    }else{
        gfx_draw_sprite_from_atlas(&player_img, WIDTH/2-32, HEIGHT/2-32, 4, 4,
                                   player.facing, 64, 64, 0);
    }
    gfx_draw_string(&font, info_str, 0, 0, 16, 16);
    gfx_draw_string(&font, crystals_str, 0, HEIGHT-16, 16, 16);
    gfx_draw_string(&font, fps_str, WIDTH-8*16, HEIGHT-16, 16, 16);
    if(player.xp < min_xp && !bossfight){
        gfx_draw_string(&font, "Kill more mobs! Use the space bar to\n"
                               "attack!", 0, 16, 16, 16);
    }else if(bossfight == 0){
        if(map_n == 0){
            gfx_draw_string(&font, "Go to the room with water to summon\n"
                                   "the boss!\n"
                                   "Press space while standing on the white\n"
                                   "portal",
                            0, 16, 16, 16);
        }else if(map_n == 7){
            gfx_draw_string(&font, "Put the crystals in their supports\n"
                                   "Press space while standing on the\n"
                                   "support",
                            0, 16, 16, 16);
        }else{
            gfx_draw_string(&font, "Summon the boss!\n"
                                   "Press space while standing on the white\n"
                                   "portal",
                            0, 16, 16, 16);
        }
    }else if(bossfight == 1){
        sprintf(boss_str, "Boss lives: %04f", mobs[MOBS_MAX-1].lives);
        gfx_draw_string(&font, boss_str, 0, 16, 16, 16);
        if(mobs[MOBS_MAX-1].type == 9){
            entity_move(&mobs[MOBS_MAX-1], &dungeon.levels[map_n].tilemap,
                        boss_direction, delta, SPIDER_SPEED);
        }else{
            entity_move(&mobs[MOBS_MAX-1], &dungeon.levels[map_n].tilemap,
                        boss_direction, delta, SLUG_SPEED);
        }
        if(mobs[MOBS_MAX-1].lives <= 0){
            bossfight = 2;
            tilemap_replace(&dungeon.levels[map_n].tilemap, 3, 4);
        }
    }else{
        gfx_draw_string(&font, "Grab the crystal by pressing space",
                        0, 16, 16, 16);
        if(map_n >= 7){
            screen = 4;
        }
    }
    
    entity_process_knockback(&player, &dungeon.levels[map_n].tilemap, delta,
                             100);
    if(player.up){
        entity_move(&player, &dungeon.levels[map_n].tilemap, D_UP, delta,
                    SPEED);
    }
    if(player.down){
        entity_move(&player, &dungeon.levels[map_n].tilemap, D_DOWN, delta,
                    SPEED);
    }
    
    if(player.left){
        entity_move(&player, &dungeon.levels[map_n].tilemap, D_LEFT, delta,
                    SPEED);
    }
    if(player.right){
        entity_move(&player, &dungeon.levels[map_n].tilemap, D_RIGHT, delta,
                    SPEED);
    }
    
    while(SDL_PollEvent(&event)){
        switch(event.type){
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym){
                    case SDLK_UP:
                        player.up = 1;
                        break;
                    case SDLK_DOWN:
                        player.down = 1;
                        break;
                    case SDLK_LEFT:
                        player.left = 1;
                        break;
                    case SDLK_RIGHT:
                        player.right = 1;
                        break;
                }
                break;
            case SDL_KEYUP:
                switch(event.key.keysym.sym){
                    case SDLK_UP:
                        player.up = 0;
                        break;
                    case SDLK_DOWN:
                        player.down = 0;
                        break;
                    case SDLK_LEFT:
                        player.left = 0;
                        break;
                    case SDLK_RIGHT:
                        player.right = 0;
                        break;
                    case SDLK_SPACE:
                        attack();
                        if(tilemap_get_tile(&dungeon.levels[map_n].tilemap,
                                            player.x+64, player.y+64) == 3 &&
                                            player.xp >= min_xp && !bossfight &&
                                            map_n != 7){
                            player.xp -= min_xp;
                            switch(map_n){
                                case 1:
                                    mobs[MOBS_MAX-1] = boss2;
                                    break;
                                case 2:
                                    mobs[MOBS_MAX-1] = boss3;
                                    break;
                                case 3:
                                    mobs[MOBS_MAX-1] = boss4;
                                    break;
                                case 4:
                                    mobs[MOBS_MAX-1] = boss5;
                                    break;
                                case 5:
                                    mobs[MOBS_MAX-1] = boss6;
                                    break;
                                case 6:
                                    mobs[MOBS_MAX-1] = boss7;
                                    break;
                                case 7:
                                    mobs[MOBS_MAX-1] = megaboss;
                                    break;
                                default:
                                    mobs[MOBS_MAX-1] = boss1;
                                    break;
                            }
                            mobs[MOBS_MAX-1].x = player.x;
                            mobs[MOBS_MAX-1].y = player.y;
                            player.x -= 100;
                            bossfight = 1;
                        }else if(bossfight == 2 &&
                                tilemap_get_tile(&dungeon.levels[map_n].tilemap,
                                                 player.x+64,
                                                 player.y+64) == 4 &&
                                                 map_n != 7){
                            bossfight = 0;
#ifndef __EMSCRIPTEN__
                            Mix_PlayChannel(-1, grab, 0);
#else
                            play_audio("wav/grab.wav");
#endif
                            tilemap_replace(&dungeon.levels[map_n].tilemap, 4,
                                            1);
                            crystals++;
                            map_n++;
                            switch(map_n){
                                case 1:
                                    mob_limit = 2;
                                    current_set = 1;
                                    break;
                                case 4:
                                    mob_limit = 3;
                                    current_set = 2;
                                    break;
                                case 7:
                                    current_set = 3;
                            }
                            if(mob_num_limit < MOBS_MAX){
                                mob_num_limit++;
                            }
                            min_xp += 20;
                            player.x = 128;
                            player.y = 128;
                            while(tilemap_get_tile(
                                               &dungeon.levels[map_n].tilemap,
                                               player.x+32,
                                               player.y+32) == 0){
                                player.x += 64;
                                player.y += 64;
                            }
                        }else if(tilemap_get_tile(
                                            &dungeon.levels[map_n].tilemap,
                                            player.x+64, player.y+64) == 8){
                            if(!tilemap_set_tile(&dungeon.levels[map_n].tilemap,
                                                 player.x+64, player.y+64, 9)){
                                crystals--;
#ifndef __EMSCRIPTEN__
                                Mix_PlayChannel(-1, place, 0);
#else
                                play_audio("wav/place.wav");
#endif
                            }
                            if(tilemap_count_tiles(
                                    &dungeon.levels[map_n].tilemap, 9) == 7){
                                mobs[MOBS_MAX-1] = megaboss;
                                mobs[MOBS_MAX-1].x = player.x;
                                mobs[MOBS_MAX-1].y = player.y;
                                bossfight = 1;
                                player.x -= 100;
                            }
                        }
                        break;
                    case SDLK_ESCAPE:
                        screen = 2;
                        player.left = 0;
                        player.right = 0;
                        player.up = 0;
                        player.down = 0;
                        break;
                }
                break;
            case SDL_QUIT:
                gfx_quit();
                break;
        }
    }
    frame = (float)(SDL_GetTicks()-last_time)/1000.0*4.0;
    frame2 = (float)(SDL_GetTicks()-last_time)/1000.0*3.0;
    glFlush();
    SDL_GL_SwapBuffers();
    current_frames++;
    if(SDL_GetTicks()-last_step > 250){
        if(player.left || player.right || player.up || player.down){
#ifndef __EMSCRIPTEN__
            Mix_PlayChannel(-1, step, 0);
#else
            play_audio("wav/step.wav");
#endif
        }
        last_step = SDL_GetTicks();
    }
    if(SDL_GetTicks()-last_time > 1000){
        fps = current_frames;
        current_frames = 0;
        last_time = SDL_GetTicks();
        if(!bossfight){
            spawn_mobs();
        }
        player.damaged = 0;
        for(i=0;i<MOBS_MAX;i++){
            mobs[i].damaged = 0;
            if(mobs[i].type == 2 && rand()%100 > SHOOT_FREQUENCY){
                for(n=0;n<BULLET_MAX;n++){
                    if(!bullets[n].exists){
                        spawn_bullet(&bullets[n], &mobs[i], &player);
                        break;
                    }
                }
            }
        }
        if(bossfight == 1){
            boss_direction = rand()%5; /* So that sometimes he won't move */
            frequency = 8;
            switch(mobs[MOBS_MAX-1].type){
                case 3:
                case 4:
                case 5:
                    frequency = 4;
                case 7:
                case 9:
                    for(i=0;i<4;i++){
                        if(mobs[MOBS_MAX-i-2].lives <= 0 &&
                           rand()%frequency == 0){
                            mobs[MOBS_MAX-i-2] = base_mob;
                            mobs[MOBS_MAX-i-2].x = mobs[MOBS_MAX-1].x-128;
                            mobs[MOBS_MAX-i-2].y = mobs[MOBS_MAX-1].y+i*64-128;
                            switch(mobs[MOBS_MAX-1].type){
                                case 4:
                                    mobs[MOBS_MAX-i-2].type = 1;
                                    break;
                                case 5:
                                    mobs[MOBS_MAX-i-2].type = rand()%2;
                                    break;
                                case 7:
                                    mobs[MOBS_MAX-i-2].type = 2;
                            }
                            break;
                        }
                    }
                    break;
                case 10:
                    if(rand()%15 == 0){
                        for(i=0;i<4;i++){
                            cannons[i].x = mobs[MOBS_MAX-1].x;
                            cannons[i].y = mobs[MOBS_MAX-1].y+i*64-128;
                            spawn_bullet(&bullets[BULLET_MAX-1-i], &cannons[i],
                                         &player);
                        }
                    }
                case 6:
                    frequency = 8;
                    for(i=0;i<4;i++){
                        if(mobs[MOBS_MAX-i-2].lives <= 0 &&
                           rand()%frequency == 0){
                            mobs[MOBS_MAX-i-2] = base_mob;
                            mobs[MOBS_MAX-i-2].x = player.x+spawn_pos[i].x*4;
                            mobs[MOBS_MAX-i-2].y = player.y+spawn_pos[i].y*4;
                            switch(mobs[MOBS_MAX-1].type){
                                case 10:
                                    mobs[MOBS_MAX-i-2].type = rand()%3;
                                    break;
                                default:
                                    mobs[MOBS_MAX-i-2].type = 1;
                            }
                        }
                    }
                    break;
                case 8:
                    if(rand()%3 == 0){
                        spawn_bullet(&bullets[BULLET_MAX-1], &mobs[MOBS_MAX-1],
                                     &player);
                    }
            }
        }
    }
}

void seed_add(char c) {
    if(cur < MAX_LEN){
        seed[cur] = c;
        cur++;
    }
}

void seed_remove(void) {
    if(cur > 0){
        cur--;
        seed[cur] = '\0';
    }
}

void title_loop(void) {
    SDL_Event event;
    int i;
    gfx_clear();
    gfx_draw_string(&font, "THE SEVEN CRYSTALS", (WIDTH-18*16)/2, 16, 16, 16);
    gfx_draw_string(&font, "Press ENTER", (WIDTH-11*16)/2, 128, 16, 16);
    gfx_draw_string(&font, "Seed:", 32, 64, 16, 16);
    gfx_draw_string(&font, seed, 32+6*16, 64, 16, 16);
    gfx_draw_char(&font, '^', 32+6*16+cur*16, 80, 16, 16);
    gfx_draw_string(&font, "2024 Mibi88", 32, HEIGHT-32-16, 16, 16);
    gfx_draw_string(&font, VERSION, WIDTH-16*8, HEIGHT-32-16, 16, 16);
    glFlush();
    SDL_GL_SwapBuffers();
    last_time = SDL_GetTicks();
    while(SDL_PollEvent(&event)){
        switch(event.type){
            case SDL_KEYDOWN:
                for(i=0;i<10;i++){
                    if(keys[i] == event.key.keysym.sym){
                        seed_add(values[i]);
                    }
                }
                switch(event.key.keysym.sym){
                    case SDLK_BACKSPACE:
                        seed_remove();
                }
                break;
            case SDL_KEYUP:
                switch(event.key.keysym.sym){
                    case SDLK_RETURN:
                        reset();
                        generate_dungeon(&dungeon, atoi(seed));
                        while(tilemap_get_tile(&dungeon.levels[map_n].tilemap,
                                               player.x+32, player.y+32) == 0){
                            player.x += 64;
                            player.y += 64;
                        }
                        screen = 1;
                        break;
                    case SDLK_ESCAPE:
#ifndef __EMSCRIPTEN__
                        /*Mix_FreeChunk(hurts);
                        Mix_FreeChunk(attack);
                        Mix_FreeChunk(step);
                        Mix_FreeChunk(place);
                        Mix_FreeChunk(grab);*/
#endif
                        gfx_quit();
                        break;
                }
                break;
            case SDL_QUIT:
                gfx_quit();
                break;
        }
    }
}

void pause_loop(void) {
    SDL_Event event;
    gfx_clear();
    gfx_draw_string(&font, "PAUSE", (WIDTH-5*16)/2, 16, 16, 16);
    gfx_draw_string(&font, "Press ENTER to resume", (WIDTH-21*16)/2, 128, 16,
                    16);
    gfx_draw_string(&font, "Press ESC to go to the title screen",
                    (WIDTH-35*16)/2, 128+16, 16, 16);
    glFlush();
    SDL_GL_SwapBuffers();
    last_time = SDL_GetTicks();
    while(SDL_PollEvent(&event)){
        switch(event.type){
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym){
                    default:
                        break;
                }
                break;
            case SDL_KEYUP:
                switch(event.key.keysym.sym){
                    case SDLK_RETURN:
                        screen = 1;
                        player.left = 0;
                        player.right = 0;
                        player.up = 0;
                        player.down = 0;
                        break;
                    case SDLK_ESCAPE:
                        screen = 0;
                        break;
                }
                break;
            case SDL_QUIT:
                gfx_quit();
                break;
        }
    }
}

void game_over_loop(void) {
    SDL_Event event;
    gfx_clear();
    gfx_draw_string(&font, "GAME OVER", (WIDTH-9*16)/2, 16, 16, 16);
    gfx_draw_string(&font, "Press ENTER to continue", (WIDTH-23*16)/2, 128, 16,
                    16);
    gfx_draw_string(&font, "Press ESC to go to the title screen",
                    (WIDTH-35*16)/2, 128+16, 16, 16);
    glFlush();
    SDL_GL_SwapBuffers();
    last_time = SDL_GetTicks();
    while(SDL_PollEvent(&event)){
        switch(event.type){
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym){
                    default:
                        break;
                }
                break;
            case SDL_KEYUP:
                switch(event.key.keysym.sym){
                    case SDLK_RETURN:
                        player.lives = 10;
                        screen = 1;
                        break;
                    case SDLK_ESCAPE:
                        screen = 0;
                        break;
                }
                break;
            case SDL_QUIT:
                gfx_quit();
                break;
        }
    }
}

void won_loop(void) {
    SDL_Event event;
    gfx_clear();
    gfx_draw_string(&font, "YOU WON!", (WIDTH-8*16)/2, 16, 16, 16);
    gfx_draw_string(&font, "THANKS FOR PLAYING!", (WIDTH-19*16)/2, 128, 16, 16);
    gfx_draw_string(&font, "Press ENTER to go to the title screen",
                    (WIDTH-37*16)/2, 128+48, 16, 16);
    gfx_draw_string(&font, "This game was made by Mibi88 for the",
                    (WIDTH-36*16)/2, 256, 16, 16);
    gfx_draw_string(&font, "Ludum Dare 55", (WIDTH-13*16)/2, 256+16, 16, 16);
    glFlush();
    SDL_GL_SwapBuffers();
    last_time = SDL_GetTicks();
    while(SDL_PollEvent(&event)){
        switch(event.type){
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym){
                    default:
                        break;
                }
                break;
            case SDL_KEYUP:
                switch(event.key.keysym.sym){
                    case SDLK_RETURN:
                        screen = 0;
                        break;
                }
                break;
            case SDL_QUIT:
                gfx_quit();
                break;
        }
    }
}

void loop(void) {
    switch(screen) {
        case 0:
            title_loop();
            break;
        case 1:
            game_loop();
            break;
        case 2:
            pause_loop();
            break;
        case 3:
            game_over_loop();
            break;
        case 4:
            won_loop();
            break;
    }
}

int main(int argc, char **argv) {
    gfx_init(1);
    font = gfx_load_texture("img/font.png");
    tileset1 = gfx_load_texture("img/tileset1.png");
    tileset2 = gfx_load_texture("img/tileset2.png");
    tileset3 = gfx_load_texture("img/tileset3.png");
    tileset4 = gfx_load_texture("img/tileset4.png");
    tileset5 = gfx_load_texture("img/tileset5.png");
    tileset6 = gfx_load_texture("img/tileset6.png");
    tileset7 = gfx_load_texture("img/tileset7.png");
    tileset8 = gfx_load_texture("img/tileset8.png");
    player_img = gfx_load_texture("img/player.png");
    slug = gfx_load_texture("img/slug.png");
    zombie = gfx_load_texture("img/zombie.png");
    cannon = gfx_load_texture("img/cannon.png");
    bullet = gfx_load_texture("img/bullet.png");
    boss1_img = gfx_load_texture("img/boss1.png");
    boss2_img = gfx_load_texture("img/boss2.png");
    boss3_img = gfx_load_texture("img/boss3.png");
    boss4_img = gfx_load_texture("img/boss4.png");
    boss5_img = gfx_load_texture("img/boss5.png");
    boss6_img = gfx_load_texture("img/boss6.png");
    boss7_img = gfx_load_texture("img/boss7.png");
    megaboss_img = gfx_load_texture("img/megaboss.png");
    
#ifndef __EMSCRIPTEN__
    hurts = Mix_LoadWAV("wav/hurts.wav");
    if(!hurts){
        puts("Failed to load sound!");
        return EXIT_FAILURE;
    }
    attacking = Mix_LoadWAV("wav/attack.wav");
    if(!attacking){
        puts("Failed to load sound!");
        return EXIT_FAILURE;
    }
    step = Mix_LoadWAV("wav/step.wav");
    if(!step){
        puts("Failed to load sound!");
        return EXIT_FAILURE;
    }
    place = Mix_LoadWAV("wav/place.wav");
    if(!place){
        puts("Failed to load sound!");
        return EXIT_FAILURE;
    }
    grab = Mix_LoadWAV("wav/grab.wav");
    if(!grab){
        puts("Failed to load sound!");
        return EXIT_FAILURE;
    }
#endif
    
    gfx_mainloop(loop);
    
    return EXIT_SUCCESS;
}

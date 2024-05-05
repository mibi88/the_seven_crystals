#include <gfx.h>
#include <tilemap.h>

#include <mapgen.h>

#include <entity.h>

#include <bullet.h>

#include <mobs.h>

#include <data.h>

#include <player.h>

#include <boss.h>

#include <config.h>

/* TODO: Find a cleaner solution to ignore these warnings. */
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"

#define MSG_KILL           ("Kill more data.mobs! Use the space bar to\n" \
                            "attack!")

#define MSG_SUMMON_BOSS    ("Summon the boss!\n" \
                            "Press space while standing on the white\n" \
                            "portal")

#define MSG_PLACE_CRYSTALS ("Put the data.crystals in their supports\n" \
                            "Press space while standing on the\n" \
                            "support")

#define MSG_SUMMON_BOSS1   ("Go to the room with water to summon\n" \
                            "the boss!\n" \
                            "Press space while standing on the white\n" \
                            "portal")

#define MAX_LEN 16

int fps = 60, current_frames = 0;
int last_time = 0;
int last_step = 0;
char fps_str[30];
char info_str[100];
char boss_str[30];

const Entity base_player = {0, 0, 0, 0, 0, 10, 128.0, 128.0, D_DOWN, 128, 0,
                            D_UP, 0, 0};
Entity player = base_player;
const Entity base_mob = {0, 0, 0, 0, 0, 5, 0.0, 0.0, D_LEFT, 20, 0, D_RIGHT, 0,
                         0};

float force = 1.5;

char crystals_str[80];

const GFXColor red = {1.0, 0.5, 0.5};
const GFXColor white = {1.0, 1.0, 1.0};

Vector spawn_pos[4] = {
    {-128, 0},
    {0, -128},
    {128, 0},
    {0, 128}
};

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

void reset(void) {
    int i;
    
    for(i=0;i<MOBS_MAX;i++){
        data.mobs[i].lives = 0;
    }
    for(i=0;i<BULLET_MAX;i++){
        data.bullets[i].exists = 0;
    }
    
    player = base_player;
    data.map_n = 0;
    force = 1.5;
    data.mob_limit = 1;
    data.mob_num_limit = 4;
    data.min_xp = 5;
    data.bossfight = 0;
    data.crystals = 0;
    data.deaths = 0;
    frame = 0;
    frame2 = 0;
    data.current_set = 0;
    data.boss_direction = D_LEFT;
}

void spawn_mobs(void) {
    int x, y, i;
    for(y=-256;y<256;y++){
        for(x=-256;x<256;x++){
            for(i=0;i<data.mob_num_limit;i++){
                if(mob_update(&data.mobs[i], &player,
                              &data.dungeon.levels[data.map_n].tilemap, &data,
                              x, y)){
                    break;
                }
            }
        }
    }
}

void attack(void) {
    int min_dist = player.attack_radius+(data.bossfight ? 128 : 0);
    int n = -1;
    int i;
    float distance;
    for(i=0;i<MOBS_MAX;i++){
        if(data.mobs[i].lives > 0){
            distance = entity_get_distance(&player, &data.mobs[i]);
            if(distance < min_dist){
                min_dist = distance;
                n = i;
            }
        }
    }
    if(n >= 0){
        gfx_play_audio(ATTACK_SOUND);
        data.mobs[n].lives -= force+0.25*((float)player.xp/100)*(rand()%2);
        data.mobs[n].damaged = 1;
        data.mobs[n].knockback_force = 1600;
        if(data.mobs[n].lives <= 0){
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
    int i, n, frequency;
    
    tileset = tilesets[frame%2+data.current_set*2];
    if(fps){
        delta = 1/(double)fps;
    }
    gfx_clear();
    sprintf(crystals_str, "Crystals: %d | data.deaths: %d", data.crystals,
            data.deaths);
    sprintf(info_str, "HP: %.02f | XP: %d/%d | LVL: %d",
            player.lives, player.xp, data.min_xp, data.map_n+1);
    sprintf(fps_str, "FPS: %d", fps);
    tilemap_render(&data.dungeon.levels[data.map_n].tilemap, tileset,
                   (int)player.x-WIDTH/2, (int)player.y-HEIGHT/2);
    
    for(i=0;i<MOBS_MAX;i++){
        process_mobs(&data.mobs[i], &player,
                     &data.dungeon.levels[data.map_n].tilemap, &data, frame,
                     frame2, delta);
    }
    for(n=0;n<BULLET_MAX;n++){
        if(data.bullets[n].exists){
            gfx_draw_sprite_from_atlas(&data.bullet,
                                       data.bullets[n].pos.x-10-player.x+
                                       WIDTH/2, data.bullets[n].pos.y-10-
                                       player.y+HEIGHT/2, 1, 1, 0, 20, 20, 0);
            if(process_bullet(&data.bullets[n], &data.mobs[i], &player, delta)){
                gfx_play_audio(HURTS_SOUND);
                player.lives -= 2-(0.25*((float)player.xp/100)*(rand()%2));
                player.damaged = 1;
                if(player.lives < 0){
                    data.screen = 3;
                    data.deaths++;
                    player.left = 0;
                    player.right = 0;
                    player.up = 0;
                    player.down = 0;
                }
            }
        }
    }
    
    player_draw(&player, &data, frame);
    
    gfx_draw_string(&data.font, info_str, 0, 0, 16, 16);
    gfx_draw_string(&data.font, crystals_str, 0, HEIGHT-16, 16, 16);
    gfx_draw_string(&data.font, fps_str, WIDTH-8*16, HEIGHT-16, 16, 16);
    if(data.bossfight == 0){
        if(player.xp < data.min_xp){
            gfx_draw_string(&data.font, MSG_KILL, 0, 16, 16, 16);
        }else if(data.map_n == 0 && player.xp >= data.min_xp){
            gfx_draw_string(&data.font, MSG_SUMMON_BOSS1, 0, 16, 16, 16);
        }else if(data.map_n == 7){
            gfx_draw_string(&data.font, MSG_PLACE_CRYSTALS, 0, 16, 16, 16);
        }else if(player.xp >= data.min_xp){
            gfx_draw_string(&data.font, MSG_SUMMON_BOSS, 0, 16, 16, 16);
        }
    }else if(data.bossfight == 1){
        sprintf(boss_str, "Boss lives: %.02f", data.mobs[MOBS_MAX-1].lives);
        gfx_draw_string(&data.font, boss_str, 0, 16, 16, 16);
        if(data.mobs[MOBS_MAX-1].type == 9){
            entity_move(&data.mobs[MOBS_MAX-1],
                        &data.dungeon.levels[data.map_n].tilemap,
                        data.boss_direction, delta, SPIDER_SPEED);
        }else{
            entity_move(&data.mobs[MOBS_MAX-1],
                        &data.dungeon.levels[data.map_n].tilemap,
                        data.boss_direction, delta, MOB_SPEED);
        }
        if(data.mobs[MOBS_MAX-1].lives <= 0){
            data.bossfight = 2;
            tilemap_replace(&data.dungeon.levels[data.map_n].tilemap, 3, 4);
        }
    }else{
        gfx_draw_string(&data.font, "Grab the crystal by pressing space",
                        0, 16, 16, 16);
        if(data.map_n >= 7){
            data.screen = 4;
        }
    }
    
    entity_process_knockback(&player, &data.dungeon.levels[data.map_n].tilemap,
                             delta, 100);
    
    player_move(&player, &data.dungeon.levels[data.map_n].tilemap, delta);
    
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
                    default:
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
                        handle_bossfight(&data, &player);
                        break;
                    case SDLK_ESCAPE:
                        data.screen = 2;
                        player.left = 0;
                        player.right = 0;
                        player.up = 0;
                        player.down = 0;
                        break;
                    default:
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
            gfx_play_audio(STEP_SOUND);
        }
        last_step = SDL_GetTicks();
    }
    if(SDL_GetTicks()-last_time > 1000){
        fps = current_frames;
        current_frames = 0;
        last_time = SDL_GetTicks();
        if(!data.bossfight){
            spawn_mobs();
        }
        player.damaged = 0;
        for(i=0;i<MOBS_MAX;i++){
            data.mobs[i].damaged = 0;
            if(data.mobs[i].type == 2 && rand()%100 > SHOOT_FREQUENCY){
                for(n=0;n<BULLET_MAX;n++){
                    if(!data.bullets[n].exists){
                        spawn_bullet(&data.bullets[n], &data.mobs[i], &player);
                        break;
                    }
                }
            }
        }
        if(data.bossfight == 1){
            data.boss_direction = rand()%5; /* So that sometimes he won't move */
            frequency = 8;
            switch(data.mobs[MOBS_MAX-1].type){
                case 3:
                case 4:
                case 5:
                    frequency = 4;
                case 7:
                case 9:
                    for(i=0;i<4;i++){
                        if(data.mobs[MOBS_MAX-i-2].lives <= 0 &&
                           rand()%frequency == 0){
                            data.mobs[MOBS_MAX-i-2] = base_mob;
                            data.mobs[MOBS_MAX-i-2].x = data.mobs[MOBS_MAX-1].x-
                                                        128;
                            data.mobs[MOBS_MAX-i-2].y = data.mobs[MOBS_MAX-1].y+
                                                        i*64-128;
                            switch(data.mobs[MOBS_MAX-1].type){
                                case 4:
                                    data.mobs[MOBS_MAX-i-2].type = 1;
                                    break;
                                case 5:
                                    data.mobs[MOBS_MAX-i-2].type = rand()%2;
                                    break;
                                case 7:
                                    data.mobs[MOBS_MAX-i-2].type = 2;
                            }
                            break;
                        }
                    }
                    break;
                case 10:
                    if(rand()%15 == 0){
                        for(i=0;i<4;i++){
                            data.cannons[i].x = data.mobs[MOBS_MAX-1].x;
                            data.cannons[i].y = data.mobs[MOBS_MAX-1].y+i*64-
                                                128;
                            spawn_bullet(&data.bullets[BULLET_MAX-1-i],
                                         &data.cannons[i],
                                         &player);
                        }
                    }
                case 6:
                    frequency = 8;
                    for(i=0;i<4;i++){
                        if(data.mobs[MOBS_MAX-i-2].lives <= 0 &&
                           rand()%frequency == 0){
                            data.mobs[MOBS_MAX-i-2] = base_mob;
                            data.mobs[MOBS_MAX-i-2].x = player.x+spawn_pos[i].x*
                                                        4;
                            data.mobs[MOBS_MAX-i-2].y = player.y+spawn_pos[i].y*
                                                        4;
                            switch(data.mobs[MOBS_MAX-1].type){
                                case 10:
                                    data.mobs[MOBS_MAX-i-2].type = rand()%3;
                                    break;
                                default:
                                    data.mobs[MOBS_MAX-i-2].type = 1;
                            }
                        }
                    }
                    break;
                case 8:
                    if(rand()%3 == 0){
                        spawn_bullet(&data.bullets[BULLET_MAX-1],
                                     &data.mobs[MOBS_MAX-1], &player);
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
    gfx_draw_string(&data.font, "THE SEVEN CRYSTALS", (WIDTH-18*16)/2, 16, 16,
                    16);
    gfx_draw_string(&data.font, "Press ENTER", (WIDTH-11*16)/2, 128, 16, 16);
    gfx_draw_string(&data.font, "Seed:", 32, 64, 16, 16);
    gfx_draw_string(&data.font, seed, 32+6*16, 64, 16, 16);
    gfx_draw_char(&data.font, '^', 32+6*16+cur*16, 80, 16, 16);
    gfx_draw_string(&data.font, "2024 Mibi88", 32, HEIGHT-32-16, 16, 16);
    gfx_draw_string(&data.font, VERSION, WIDTH-16*8, HEIGHT-32-16, 16, 16);
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
                    default:
                        break;
                }
                break;
            case SDL_KEYUP:
                switch(event.key.keysym.sym){
                    case SDLK_RETURN:
                        reset();
                        generate_dungeon(&data.dungeon, atoi(seed));
                        while(tilemap_get_tile(&data.dungeon.levels[data.map_n]
                                               .tilemap, player.x+32,
                                               player.y+32) == 0){
                            player.x += 64;
                            player.y += 64;
                        }
                        data.screen = 1;
                        break;
                    case SDLK_ESCAPE:
                        gfx_quit();
                        break;
                    default:
                        break;
                }
                break;
            case SDL_QUIT:
                gfx_quit();
                break;
            default:
                break;
        }
    }
}

void pause_loop(void) {
    SDL_Event event;
    gfx_clear();
    gfx_draw_string(&data.font, "PAUSE", (WIDTH-5*16)/2, 16, 16, 16);
    gfx_draw_string(&data.font, "Press ENTER to resume", (WIDTH-21*16)/2, 128,
                    16, 16);
    gfx_draw_string(&data.font, "Press ESC to go to the title data.screen",
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
                        data.screen = 1;
                        player.left = 0;
                        player.right = 0;
                        player.up = 0;
                        player.down = 0;
                        break;
                    case SDLK_ESCAPE:
                        data.screen = 0;
                        break;
                    default:
                        break;
                }
                break;
            case SDL_QUIT:
                gfx_quit();
                break;
            default:
                break;
        }
    }
}

void game_over_loop(void) {
    SDL_Event event;
    gfx_clear();
    gfx_draw_string(&data.font, "GAME OVER", (WIDTH-9*16)/2, 16, 16, 16);
    gfx_draw_string(&data.font, "Press ENTER to continue", (WIDTH-23*16)/2, 128,
                    16, 16);
    gfx_draw_string(&data.font, "Press ESC to go to the title data.screen",
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
                        data.screen = 1;
                        break;
                    case SDLK_ESCAPE:
                        data.screen = 0;
                        break;
                    default:
                        break;
                }
                break;
            case SDL_QUIT:
                gfx_quit();
                break;
            default:
                break;
        }
    }
}

void won_loop(void) {
    SDL_Event event;
    gfx_clear();
    gfx_draw_string(&data.font, "YOU WON!", (WIDTH-8*16)/2, 16, 16, 16);
    gfx_draw_string(&data.font, "THANKS FOR PLAYING!", (WIDTH-19*16)/2, 128, 16,
                    16);
    gfx_draw_string(&data.font, "Press ENTER to go to the title data.screen",
                    (WIDTH-37*16)/2, 128+48, 16, 16);
    gfx_draw_string(&data.font, "This game was made by Mibi88 for the",
                    (WIDTH-36*16)/2, 256, 16, 16);
    gfx_draw_string(&data.font, "Ludum Dare 55", (WIDTH-13*16)/2, 256+16, 16,
                    16);
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
                        data.screen = 0;
                        break;
                    default:
                        break;
                }
                break;
            case SDL_QUIT:
                gfx_quit();
                break;
            default:
                break;
        }
    }
}

void loop(void) {
    switch(data.screen) {
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
        default:
            break;
    }
}

int main(int argc, char **argv) {
    gfx_init(1);
    
    data.screen = 0;
    
    load_assets(&data);
    
    gfx_mainloop(loop);
    
    return EXIT_SUCCESS;
}

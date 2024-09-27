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
    int i, n;

    tileset = tilesets[frame%2+data.current_set*2];
    if(fps){
        delta = 1/(double)fps;
    }
    gfx_clear();
    sprintf(crystals_str, "Crystals: %d | Deaths: %d", data.crystals,
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
        spawn_enemies(&data, &player);
    }
}

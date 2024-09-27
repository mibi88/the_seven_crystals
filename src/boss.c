#include <boss.h>

void boss_spawn(Data *data, Entity *player) {
    const Entity bosses[] = {
        {0, 0, 0, 0, 0, 20, 0.0, 0.0, D_LEFT, 32, 0, D_RIGHT, 0, 3},
        {0, 0, 0, 0, 0, 30, 0.0, 0.0, D_LEFT, 32, 0, D_RIGHT, 0, 4},
        {0, 0, 0, 0, 0, 40, 0.0, 0.0, D_LEFT, 32, 0, D_RIGHT, 0, 5},
        {0, 0, 0, 0, 0, 50, 0.0, 0.0, D_LEFT, 32, 0, D_RIGHT, 0, 6},
        {0, 0, 0, 0, 0, 60, 0.0, 0.0, D_LEFT, 32, 0, D_RIGHT, 0, 7},
        {0, 0, 0, 0, 0, 65, 0.0, 0.0, D_LEFT, 32, 0, D_RIGHT, 0, 8},
        {0, 0, 0, 0, 0, 70, 0.0, 0.0, D_LEFT, 32, 0, D_RIGHT, 0, 9},
        {0, 0, 0, 0, 0, 100, 0.0, 0.0, D_LEFT, 32, 0, D_RIGHT, 0, 10}
    };

    player->xp -= data->min_xp;
    data->mobs[MOBS_MAX-1] = bosses[data->map_n];
    data->mobs[MOBS_MAX-1].x = player->x;
    data->mobs[MOBS_MAX-1].y = player->y;
    player->x -= 100;
    data->bossfight = 1;
}

void handle_bossfight(Data *data, Entity *player) {
    int i;
    const Entity megaboss = {0, 0, 0, 0, 0, 100, 0.0, 0.0, D_LEFT, 32, 0,
                             D_RIGHT, 0, 10};
    if(tilemap_get_tile(&data->dungeon.levels[data->map_n].tilemap,
                        player->x+64, player->y+64) == 3 &&
                        player->xp >= data->min_xp && !data->bossfight &&
                        data->map_n != 7){
        boss_spawn(data, player);
    }else if(data->bossfight == 2 &&
            tilemap_get_tile(&data->dungeon.levels[data->map_n].tilemap,
                             player->x+64,
                             player->y+64) == 4 &&
                             data->map_n != 7){
        data->bossfight = 0;
        gfx_play_audio(GRAB_SOUND);
        tilemap_replace(&data->dungeon.levels[data->map_n].tilemap, 4,
                        1);
        data->crystals++;
        data->map_n++;
        for(i=0;i<MOBS_MAX;i++){
            data->mobs[i].lives = -1;
        }
        for(i=0;i<BULLET_MAX;i++){
            data->bullets[i].exists = 0;
        }
        switch(data->map_n){
            case 1:
                data->mob_limit = 2;
                data->current_set = 1;
                break;
            case 4:
                data->mob_limit = 3;
                data->current_set = 2;
                break;
            case 7:
                data->current_set = 3;
        }
        if(data->mob_num_limit < MOBS_MAX){
            data->mob_num_limit++;
        }
        data->min_xp += 20;
        player->x = 128;
        player->y = 128;
        while(tilemap_get_tile(
                           &data->dungeon.levels[data->map_n].tilemap,
                           player->x+32,
                           player->y+32) == 0){
            player->x += 64;
            player->y += 64;
        }
    }else if(tilemap_get_tile(
                        &data->dungeon.levels[data->map_n].tilemap,
                        player->x+64, player->y+64) == 8){
        if(!tilemap_set_tile(&data->dungeon.levels[data->map_n].tilemap,
                             player->x+64, player->y+64, 9)){
            data->crystals--;
            gfx_play_audio(PLACE_SOUND);
        }
        if(tilemap_count_tiles(
                &data->dungeon.levels[data->map_n].tilemap, 9) == 7){
            data->mobs[MOBS_MAX-1] = megaboss;
            data->mobs[MOBS_MAX-1].x = player->x;
            data->mobs[MOBS_MAX-1].y = player->y;
            data->bossfight = 1;
            player->x -= 100;
        }
    }
}

void spawn_enemies(Data *data, Entity *player) {
    int i, n, frequency;
    const Entity base_mob = {0, 0, 0, 0, 0, 5, 0.0, 0.0, D_LEFT, 20, 0, D_RIGHT,
                             0, 0};
    Vector spawn_pos[4] = {
        {-128, 0},
        {0, -128},
        {128, 0},
        {0, 128}
    };

    if(!data->bossfight){
        spawn_mobs(data, player);
    }
    player->damaged = 0;
    for(i=0;i<MOBS_MAX;i++){
        data->mobs[i].damaged = 0;
        if(data->mobs[i].type == 2 && rand()%100 > SHOOT_FREQUENCY){
            for(n=0;n<BULLET_MAX;n++){
                if(!data->bullets[n].exists){
                    spawn_bullet(&data->bullets[n], &data->mobs[i], player);
                    break;
                }
            }
        }
    }
    if(data->bossfight == 1){
        data->boss_direction = rand()%5; /* So that sometimes he won't move */
        frequency = 8;
        switch(data->mobs[MOBS_MAX-1].type){
            case 3:
            case 4:
            case 5:
                frequency = 4;
                /* FALLTHRU */
            case 7:
            case 9:
                for(i=0;i<4;i++){
                    if(data->mobs[MOBS_MAX-i-2].lives <= 0 &&
                       rand()%frequency == 0){
                        data->mobs[MOBS_MAX-i-2] = base_mob;
                        data->mobs[MOBS_MAX-i-2].x = data->mobs[MOBS_MAX-1].x-
                                                    128;
                        data->mobs[MOBS_MAX-i-2].y = data->mobs[MOBS_MAX-1].y+
                                                    i*64-128;
                        switch(data->mobs[MOBS_MAX-1].type){
                            case 4:
                                data->mobs[MOBS_MAX-i-2].type = 1;
                                break;
                            case 5:
                                data->mobs[MOBS_MAX-i-2].type = rand()%2;
                                break;
                            case 7:
                                data->mobs[MOBS_MAX-i-2].type = 2;
                        }
                        break;
                    }
                }
                break;
            case 10:
                if(rand()%15 == 0){
                    for(i=0;i<4;i++){
                        data->cannons[i].x = data->mobs[MOBS_MAX-1].x;
                        data->cannons[i].y = data->mobs[MOBS_MAX-1].y+i*64-
                                            128;
                        spawn_bullet(&data->bullets[BULLET_MAX-1-i],
                                     &data->cannons[i],
                                     player);
                    }
                }
                /* FALLTHRU */
            case 6:
                frequency = 8;
                for(i=0;i<4;i++){
                    if(data->mobs[MOBS_MAX-i-2].lives <= 0 &&
                       rand()%frequency == 0){
                        data->mobs[MOBS_MAX-i-2] = base_mob;
                        data->mobs[MOBS_MAX-i-2].x = player->x+spawn_pos[i].x*
                                                    4;
                        data->mobs[MOBS_MAX-i-2].y = player->y+spawn_pos[i].y*
                                                    4;
                        switch(data->mobs[MOBS_MAX-1].type){
                            case 10:
                                data->mobs[MOBS_MAX-i-2].type = rand()%3;
                                break;
                            default:
                                data->mobs[MOBS_MAX-i-2].type = 1;
                        }
                    }
                }
                break;
            case 8:
                if(rand()%3 == 0){
                    spawn_bullet(&data->bullets[BULLET_MAX-1],
                                 &data->mobs[MOBS_MAX-1], player);
                }
        }
    }
}

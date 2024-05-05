#include <boss.h>

void boss_spawn(Data *data, Entity *player) {
    const Entity boss1 = {0, 0, 0, 0, 0, 20, 0.0, 0.0, D_LEFT, 32, 0, D_RIGHT,
                          0, 3};
    const Entity boss2 = {0, 0, 0, 0, 0, 30, 0.0, 0.0, D_LEFT, 32, 0, D_RIGHT,
                          0, 4};
    const Entity boss3 = {0, 0, 0, 0, 0, 40, 0.0, 0.0, D_LEFT, 32, 0, D_RIGHT,
                          0, 5};
    const Entity boss4 = {0, 0, 0, 0, 0, 50, 0.0, 0.0, D_LEFT, 32, 0, D_RIGHT,
                          0, 6};
    const Entity boss5 = {0, 0, 0, 0, 0, 60, 0.0, 0.0, D_LEFT, 32, 0, D_RIGHT,
                          0, 7};
    const Entity boss6 = {0, 0, 0, 0, 0, 65, 0.0, 0.0, D_LEFT, 32, 0, D_RIGHT,
                          0, 8};
    const Entity boss7 = {0, 0, 0, 0, 0, 70, 0.0, 0.0, D_LEFT, 32, 0, D_RIGHT,
                          0, 9};
    const Entity megaboss = {0, 0, 0, 0, 0, 100, 0.0, 0.0, D_LEFT, 32, 0,
                             D_RIGHT, 0, 10};

    player->xp -= data->min_xp;
    switch(data->map_n){
        case 1:
            data->mobs[MOBS_MAX-1] = boss2;
            break;
        case 2:
            data->mobs[MOBS_MAX-1] = boss3;
            break;
        case 3:
            data->mobs[MOBS_MAX-1] = boss4;
            break;
        case 4:
            data->mobs[MOBS_MAX-1] = boss5;
            break;
        case 5:
            data->mobs[MOBS_MAX-1] = boss6;
            break;
        case 6:
            data->mobs[MOBS_MAX-1] = boss7;
            break;
        case 7:
            data->mobs[MOBS_MAX-1] = megaboss;
            break;
        default:
            data->mobs[MOBS_MAX-1] = boss1;
            break;
    }
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

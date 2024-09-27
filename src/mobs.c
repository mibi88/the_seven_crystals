#include <mobs.h>

void spawn_mobs(Data *data, Entity *player) {
    int x, y, i;
    for(y=-256;y<256;y++){
        for(x=-256;x<256;x++){
            for(i=0;i<data->mob_num_limit;i++){
                if(mob_update(&data->mobs[i], player,
                              &data->dungeon.levels[data->map_n].tilemap, data,
                              x, y)){
                    break;
                }
            }
        }
    }
}

void mob_attack(Entity *mob, Entity *player, Tilemap *tilemap, Data *data,
                double delta) {
    player->damaged = 1;
    if(player->knockback_force <= 0){
        gfx_play_audio(HURTS_SOUND);
        player->knockback_force = 800;
        switch(mob->type){
            case 1:
                player->lives -= 1.5;
                break;
            case 2:
                player->lives -= 0.25;
                break;
            default:
                if(mob->type >= 3){
                    player->lives -= 5;
                }else{
                    player->lives -= 0.5;
                }
        }
        if(rand()%5 == 0){
            mob->xp += rand()%5;
        }
        player->lives -= 0.25*((float)mob->xp/100)*(rand()%2);
        if(player->lives < 0){
            data->screen = 3;
            data->deaths++;
            player->left = 0;
            player->right = 0;
            player->up = 0;
            player->down = 0;
        }
    }
}

void mob_move(Entity *mob, Entity *player, Tilemap *tilemap, double delta) {
    if(mob->type <= 2 || mob->type == 9){
        if(player->x < mob->x){
            entity_move(mob, tilemap, D_LEFT, delta, MOB_SPEED);
        }
        if(player->x > mob->x){
            entity_move(mob, tilemap, D_RIGHT, delta, MOB_SPEED);
        }
        if(player->y < mob->y){
            entity_move(mob, tilemap, D_UP, delta, MOB_SPEED);
        }
        if(player->y > mob->y){
            entity_move(mob, tilemap, D_DOWN, delta, MOB_SPEED);
        }
    }
}

void mob_draw(Entity *mob, Entity *player, Data *data, int frame, int frame2) {
    const GFXColor red = {1.0, 0.5, 0.5};
    const GFXColor white = {1.0, 1.0, 1.0};
    GFXSprite *mobs[] = {
        &data->slug,
        &data->zombie,
        &data->cannon,
        &data->boss1_img,
        &data->boss2_img,
        &data->boss3_img,
        &data->boss4_img,
        &data->boss5_img,
        &data->boss6_img,
        &data->boss7_img,
        &data->megaboss_img
    };
    int pos;
    mobs[mob->type]->color = mob->damaged ? red : white;
    pos = mob->facing%D_AMOUNT+4*(frame%2+1);
    gfx_draw_sprite_from_atlas(mobs[mob->type],
                               mob->x-mobs[mob->type]->width/2-player->x+
                                                               WIDTH/2,
                               mob->y-mobs[mob->type]->height/2-player->y+
                                                                HEIGHT/2,
                               4, 4, pos, mobs[mob->type]->width,
                               mobs[mob->type]->height, 0);
}

int mob_update(Entity *mob, Entity *player, Tilemap *tilemap, Data *data, int x,
               int y) {
    const Entity base_mob = {0, 0, 0, 0, 0, 5, 0.0, 0.0, D_LEFT, 20, 0, D_RIGHT,
                             0, 0};
    if(mob->lives <= 0){
        if(tilemap_get_tile(tilemap, player->x+x, player->x+y) == 2 &&
                            rand()%100000 == 0){
            memcpy(mob, &base_mob, sizeof(Entity));
            mob->x = player->x+x;
            mob->y = player->y+y;
            mob->type = rand()%data->mob_limit;
            switch(mob->type){
                case 1:
                    mob->lives = 10;
                    break;
                case 2:
                    mob->lives = 15;
                    break;
            }
            return 1;
        }
    }else if(mob->type <= 2){
        if(entity_get_distance(player, mob) > 24*64){
            mob->lives = -1;
            return 1;
        }
    }
    return 0;
}

void process_mobs(Entity *mob, Entity *player, Tilemap *tilemap, Data *data,
                  int frame, int frame2, double delta) {
    if(mob->lives > 0){
        entity_process_knockback(mob, tilemap, delta, 200);
        mob_draw(mob, player, data, frame, frame2);
        
        mob_move(mob, player, tilemap, delta);
        
        if(entity_in_attack_radius(mob, player)) {
            mob_attack(mob, player, tilemap, data, delta);
        }
    }
}

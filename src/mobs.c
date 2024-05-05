#include <mobs.h>

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
    int pos;
    if(mob->damaged){
        data->slug.color = red;
        data->zombie.color = red;
        data->cannon.color = red;
        data->boss1_img.color = red;
        data->boss2_img.color = red;
        data->boss3_img.color = red;
        data->boss4_img.color = red;
        data->boss5_img.color = red;
        data->boss6_img.color = red;
        data->boss7_img.color = red;
        data->megaboss_img.color = red;
    }else{
       data->slug.color = white;
        data->zombie.color = white;
        data->cannon.color = white;
        data->boss1_img.color = white;
        data->boss2_img.color = white;
        data->boss3_img.color = white;
        data->boss4_img.color = white;
        data->boss5_img.color = white;
        data->boss6_img.color = white;
        data->boss7_img.color = white;
        data->megaboss_img.color = white;
    }
    pos = mob->facing%D_AMOUNT+4*(frame%2+1);
    switch(mob->type){
        case 0:
            pos = mob->facing%D_AMOUNT+4*frame2;
            gfx_draw_sprite_from_atlas(&data->slug,
                                       mob->x-32-player->x+WIDTH/2,
                                       mob->y-32-player->y+HEIGHT/2,
                                       4, 4, pos, 64, 64, 0);
            break;
        case 1:
            gfx_draw_sprite_from_atlas(&data->zombie,
                                       mob->x-32-player->x+WIDTH/2,
                                       mob->y-32-player->y+HEIGHT/2,
                                       4, 4, pos,
                                       64, 64, 0);
            break;
        case 2:
            gfx_draw_sprite_from_atlas(&data->cannon,
                                       mob->x-32-player->x+WIDTH/2,
                                       mob->y-32-player->y+HEIGHT/2,
                                       4, 4, pos, 64, 64, 0);
            break;
        case 3:
            gfx_draw_sprite_from_atlas(&data->boss1_img,
                                       mob->x-128-player->x+WIDTH/2,
                                       mob->y-128-player->y+HEIGHT/2,
                                       4, 4, mob->facing+frame*4,
                                       256, 256, 0);
           break;
       case 4:
            gfx_draw_sprite_from_atlas(&data->boss2_img,
                                       mob->x-128-player->x+WIDTH/2,
                                       mob->y-128-player->y+HEIGHT/2,
                                       4, 4, pos, 256, 256, 0);
           break;
       case 5:
            gfx_draw_sprite_from_atlas(&data->boss3_img,
                                       mob->x-128-player->x+WIDTH/2,
                                       mob->y-128-player->y+HEIGHT/2,
                                       1, 1, 0, 256, 256, 0);
           break;
       case 6:
            gfx_draw_sprite_from_atlas(&data->boss4_img,
                                       mob->x-128-player->x+WIDTH/2,
                                       mob->y-128-player->y+HEIGHT/2,
                                       1, 1, 0, 256, 256, 0);
           break;
       case 7:
            gfx_draw_sprite_from_atlas(&data->boss5_img,
                                       mob->x-128-player->x+WIDTH/2,
                                       mob->y-128-player->y+HEIGHT/2,
                                       1, 1, 0, 256, 256, 0);
           break;
       case 8:
            gfx_draw_sprite_from_atlas(&data->boss6_img,
                                       mob->x-128-player->x+WIDTH/2,
                                       mob->y-128-player->y+HEIGHT/2,
                                       1, 1, 0, 256, 256, 0);
           break;
       case 9:
            gfx_draw_sprite_from_atlas(&data->boss7_img,
                                       mob->x-128-player->x+WIDTH/2,
                                       mob->y-128-player->y+HEIGHT/2,
                                       1, 1, 0, 256, 256, 0);
           break;
       case 10:
            gfx_draw_sprite_from_atlas(&data->megaboss_img,
                                       mob->x-128-player->x+WIDTH/2,
                                       mob->y-128-player->y+HEIGHT/2,
                                       1, 1, 0, 256, 256, 0); 
           break;
    }
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

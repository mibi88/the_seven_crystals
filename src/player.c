#include <player.h>

void player_move(Entity *player, Tilemap *tilemap, double delta) {
    if(player->up){
        entity_move(player, tilemap, D_UP, delta, SPEED);
    }
    if(player->down){
        entity_move(player, tilemap, D_DOWN, delta, SPEED);
    }
    
    if(player->left){
        entity_move(player, tilemap, D_LEFT, delta, SPEED);
    }
    if(player->right){
        entity_move(player, tilemap, D_RIGHT, delta, SPEED);
    }
}

void player_draw(Entity *player, Data *data, int frame) {
    const GFXColor red = {1.0, 0.5, 0.5};
    const GFXColor white = {1.0, 1.0, 1.0};
    
    if(player->damaged){
        data->player_img.color = red;
    }else{
        data->player_img.color = white;
    }
    if(player->left || player->right || player->up || player->down){
        gfx_draw_sprite_from_atlas(&data->player_img, WIDTH/2-32, HEIGHT/2-32,
                                   4, 4, player->facing+4*(frame%2+1), 64, 64,
                                   0);
    }else{
        gfx_draw_sprite_from_atlas(&data->player_img, WIDTH/2-32, HEIGHT/2-32,
                                   4, 4, player->facing, 64, 64, 0);
    }
}

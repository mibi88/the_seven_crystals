#include <data.h>

void load_assets(Data *data) {
    data->font = gfx_load_texture("font.png");
    data->tileset1 = gfx_load_texture("tileset1.png");
    data->tileset2 = gfx_load_texture("tileset2.png");
    data->tileset3 = gfx_load_texture("tileset3.png");
    data->tileset4 = gfx_load_texture("tileset4.png");
    data->tileset5 = gfx_load_texture("tileset5.png");
    data->tileset6 = gfx_load_texture("tileset6.png");
    data->tileset7 = gfx_load_texture("tileset7.png");
    data->tileset8 = gfx_load_texture("tileset8.png");
    data->player_img = gfx_load_texture("player.png");
    data->slug = gfx_load_texture("slug.png");
    data->zombie = gfx_load_texture("zombie.png");
    data->cannon = gfx_load_texture("cannon.png");
    data->bullet = gfx_load_texture("bullet.png");
    data->boss1_img = gfx_load_texture("boss1.png");
    data->boss2_img = gfx_load_texture("boss2.png");
    data->boss3_img = gfx_load_texture("boss3.png");
    data->boss4_img = gfx_load_texture("boss4.png");
    data->boss5_img = gfx_load_texture("boss5.png");
    data->boss6_img = gfx_load_texture("boss6.png");
    data->boss7_img = gfx_load_texture("boss7.png");
    data->megaboss_img = gfx_load_texture("megaboss.png");
}

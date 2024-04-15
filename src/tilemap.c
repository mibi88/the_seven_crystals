#include <tilemap.h>

#include <math.h>

GFXColor light_levels[LIGHT_LEVELS] = {
    {0.1, 0.1, 0.0},
    {0.2, 0.2, 0.1},
    {0.3, 0.3, 0.2},
    {0.4, 0.4, 0.3},
    {0.5, 0.5, 0.4},
    {0.6, 0.6, 0.5},
    {0.7, 0.7, 0.6},
    {0.8, 0.8, 0.7},
    {0.9, 0.9, 0.8},
    {1.0, 1.0, 0.9},
};

void tilemap_render(Tilemap *tilemap, GFXSprite *tiles, int sx, int sy) {
    int stx = (-sx)/TILE_W;
    int sty = -sy/TILE_H;
    int dx = ((-sx)%TILE_W)-TILE_W;
    int dy = (-sy%TILE_H)-TILE_H;
    int x, y, gx, gy;
    unsigned char light;
    float distance;
    
    /* TODO: Use vertex arrays */
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tiles->texture);
    glBegin(GL_QUADS);
    for(y=0;y<DRAWING_H;y++){
        for(x=0;x<DRAWING_W;x++){
            gx = -stx+x;
            gy = -sty+y;
            if(gx >= 0 && gx < tilemap->width && gy >= 0 &&
               gy < tilemap->height){
                light = tilemap->light[gy*tilemap->width+gx];
                distance = sqrt((x-DRAWING_W/2)*(x-DRAWING_W/2)+
                                (y-DRAWING_H/2+1)*(y-DRAWING_H/2+1));
                if(distance > PLAYER_LIGHT){
                    distance = PLAYER_LIGHT;
                }
                distance = PLAYER_LIGHT-distance;
                light += (unsigned char)distance;
                if(light >= LIGHT_LEVELS){
                    light = LIGHT_LEVELS-1;
                }
                tiles->color = light_levels[light];
                gfx_draw_sprite_from_atlas(tiles, dx+x*TILE_W, dy+y*TILE_H,
                                           ATLAS_W, ATLAS_H,
                                           tilemap->data[gy*tilemap->width+gx],
                                           TILE_W, TILE_H, 1);
            }
        }
    }
    glEnd();
}

unsigned char tilemap_get_tile_raw(Tilemap *tilemap, int x, int y) {
    if(x >= 0 && x < tilemap->width && y >= 0 && y < tilemap->height){
        return tilemap->data[y*tilemap->width+x];
    }
    return 0;
}

unsigned char tilemap_get_tile(Tilemap *tilemap, int x, int y) {
    int tx = x/TILE_W;
    int ty = y/TILE_H;
    if(tx >= 0 && tx < tilemap->width && ty >= 0 && ty < tilemap->height){
        return tilemap->data[ty*tilemap->width+tx];
    }
    return 0;
}

int tilemap_set_tile(Tilemap *tilemap, int x, int y, unsigned char tile) {
    int tx = x/TILE_W;
    int ty = y/TILE_H;
    if(tx >= 0 && tx < tilemap->width && ty >= 0 && ty < tilemap->height){
        tilemap->data[ty*tilemap->width+tx] = tile;
        return 0;
    }
    return 1;
}

void tilemap_replace(Tilemap *tilemap, unsigned char from, unsigned char to) {
    int i;
    for(i=0;i<tilemap->width*tilemap->height;i++){
        if(tilemap->data[i] == from){
            tilemap->data[i] = to;
        }
    }
}

int tilemap_count_tiles(Tilemap *tilemap, unsigned char tile) {
    int i;
    int c = 0;
    for(i=0;i<tilemap->width*tilemap->height;i++){
        if(tilemap->data[i] == tile){
            c++;
        }
    }
    return c;
}

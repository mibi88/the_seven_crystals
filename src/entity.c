#include <entity.h>

#include <math.h>

Vector hitbox[4] = {
    {-32, -32},
    {-32, 32},
    {32, -32},
    {32, 32}
};

void entity_move_raw(Entity *entity, Tilemap *tilemap, Direction direction,
                 double delta, float speed) {
    int i, collision = 0;
    entity->facing = direction;
    if(tilemap_get_tile(tilemap, entity->x+64, entity->y+64) > 1) {
        speed /= 2;
    }
    if(tilemap_get_tile(tilemap, entity->x+64, entity->y+64) == 0){
        entity->x += 64;
        entity->y += 64;
    }
    switch(direction){
        case D_RIGHT:
            entity->x += delta*speed*2;
            break;
        case D_LEFT:
            entity->x -= delta*speed;
            break;
        case D_DOWN:
            entity->y += delta*speed;
            break;
        case D_UP:
            entity->y -= delta*speed;
            break;
        default:
            break;
    }
    for(i=0;i<4;i++){
        collision += !tilemap_get_tile(tilemap, entity->x+hitbox[i].x+64,
                                       entity->y+64);
    }
    if(collision){
        entity->x = (int)entity->x/TILE_W*TILE_W+32;
    }
    collision = 0;
    for(i=0;i<4;i++){
        collision += !tilemap_get_tile(tilemap, entity->x+64,
                entity->y+hitbox[i].y+64);
    }
    if(collision){
        entity->y = (int)entity->y/TILE_H*TILE_H+32;
    }
}

void entity_move_safe(Entity *entity, Tilemap *tilemap, Direction direction,
                      double delta, float speed) {
    int i;
    if(delta*speed > 8){
        for(i=0;i<floor(delta*speed/8);i++){
            entity_move_raw(entity, tilemap, direction, delta, 8/delta);
        }
        if(fmod(delta*speed, 8.0) != 0){
            entity_move_raw(entity, tilemap, direction, delta,
                            fmod(delta*speed, 8.0)/delta);
        }
    }else{
        entity_move_raw(entity, tilemap, direction, delta, speed);
    }
}

void entity_move(Entity *entity, Tilemap *tilemap, Direction direction,
                 double delta, float speed) {
    switch(direction){
        case D_RIGHT:
            entity->knockback_direction = D_LEFT;
            break;
        case D_LEFT:
            entity->x -= delta*speed;
            entity->knockback_direction = D_RIGHT;
            break;
        case D_DOWN:
            entity->y += delta*speed;
            entity->knockback_direction = D_UP;
            break;
        case D_UP:
            entity->y -= delta*speed;
            entity->knockback_direction = D_DOWN;
            break;
        default:
            break;
    }
    entity_move_safe(entity, tilemap, direction, delta, speed);
}

void entity_process_knockback(Entity *entity, Tilemap *tilemap, double delta,
                              int decrease) {
    if(entity->knockback_force > 0){
        entity_move_safe(entity, tilemap, entity->knockback_direction, delta,
                         entity->knockback_force);
        entity->knockback_force -= decrease;
    }
}

float entity_get_distance(Entity *entity1, Entity *entity2) {
    return sqrt((entity1->x-entity2->x)*(entity1->x-entity2->x)+
                (entity1->y-entity2->y)*(entity1->y-entity2->y));
}

int entity_in_attack_radius(Entity *entity, Entity *attacked) {
    return entity_get_distance(entity, attacked) < entity->attack_radius;
}

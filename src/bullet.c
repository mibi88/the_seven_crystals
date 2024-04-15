#include <bullet.h>

#include <math.h>

void spawn_bullet(Bullet *bullet, Entity *entity, Entity *player) {
    float distance;
    bullet->exists = 1;
    bullet->pos.x = entity->x;
    bullet->pos.y = entity->y;
    distance = entity_get_distance(entity, player);
    bullet->mov.x = (player->x-entity->x)/distance;
    bullet->mov.y = (player->y-entity->y)/distance;
}

float bullet_distance_from_entity(Bullet *bullet, Entity *entity) {
    return sqrt((entity->x-bullet->pos.x)*(entity->x-bullet->pos.x)+
                (entity->y-bullet->pos.y)*(entity->y-bullet->pos.y));
}

int process_bullet(Bullet *bullet, Entity *entity, Entity *player,
                   double delta) {
    if(!bullet->exists){
        return 0;
    }
    bullet->pos.x += bullet->mov.x*800*delta;
    bullet->pos.y += bullet->mov.y*800*delta;
    
    if(bullet_distance_from_entity(bullet, player) > 16*64){
        bullet->exists = 0;
    }else if(bullet_distance_from_entity(bullet, player) < 20) {
        bullet->exists = 0;
        return 1;
    }
    return 0;
}

#ifndef BULLET_H
#define BULLET_H

#include <entity.h>

typedef struct {
    Vector pos;
    Vector mov;
    int exists;
} Bullet;

void spawn_bullet(Bullet *bullet, Entity *entity, Entity *player);
float bullet_distance_from_entity(Bullet *bullet, Entity *entity);
int process_bullet(Bullet *bullet, Entity *entity, Entity *player,
                   double delta);

#endif

#ifndef MOBS_H
#define MOBS_H

#include <entity.h>

#include <data.h>

#define BOSS_START 3

void spawn_mobs(Data *data, Entity *player);
int mob_update(Entity *mob, Entity *player, Tilemap *tilemap, Data *data, int x,
               int y);
void process_mobs(Entity *mob, Entity *player, Tilemap *tilemap, Data *data,
                  int frame, int frame2, double delta);

#endif

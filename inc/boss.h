#ifndef BOSS_H
#define BOSS_H

#include <entity.h>

#include <mobs.h>

#include <data.h>

void boss_spawn(Data *data, Entity *player);
void handle_bossfight(Data *data, Entity *player);

#endif

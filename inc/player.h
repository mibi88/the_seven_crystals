#ifndef PLAYER_H
#define PLAYER_H

#include <gfx.h>

#include <entity.h>

#include <data.h>

#include <tilemap.h>

#define SPEED 250

void player_move(Entity *player, Tilemap *tilemap, double delta);
void player_draw(Entity *player, Data *data, int frame);

#endif

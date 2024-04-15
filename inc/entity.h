#ifndef ENTITY_H
#define ENTITY_H

#include <tilemap.h>

typedef enum {
    D_LEFT,
    D_DOWN,
    D_RIGHT,
    D_UP,
    D_AMOUNT
} Direction;

typedef struct {
    float x, y;
} Vector;

typedef struct {
    int left;
    int right;
    int up;
    int down;
    
    int xp;
    float lives;
    
    float x, y;
    
    Direction facing;
    
    int attack_radius;
    
    int damaged;
    
    Direction knockback_direction;
    int knockback_force;
    int type;
} Entity;

extern Vector hitbox[4];

void entity_move(Entity *entity, Tilemap *tilemap, Direction direction,
                 double delta, float speed);
float entity_get_distance(Entity *entity1, Entity *entity2);
void entity_process_knockback(Entity *entity, Tilemap *tilemap, double delta,
                              int decrease);
int entity_in_attack_radius(Entity *entity, Entity *attacked);

#endif

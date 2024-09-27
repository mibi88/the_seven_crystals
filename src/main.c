#include <gfx.h>
#include <tilemap.h>

#include <mapgen.h>

#include <entity.h>

#include <bullet.h>

#include <mobs.h>

#include <data.h>

#include <player.h>

#include <boss.h>

#include <config.h>

void loop(void) {
    switch(data.screen) {
        case 0:
            title_loop();
            break;
        case 1:
            game_loop();
            break;
        case 2:
            pause_loop();
            break;
        case 3:
            game_over_loop();
            break;
        case 4:
            win_loop();
            break;
        default:
            break;
    }
}

int main(int argc, char **argv) {
    gfx_init(1);
    
    data.screen = 0;
    
    load_assets(&data);
    
    gfx_mainloop(loop);
    
    return EXIT_SUCCESS;
}

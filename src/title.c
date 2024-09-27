#include <data.h>
#include <config.h>

void seed_add(char c) {
    if(cur < MAX_LEN){
        seed[cur] = c;
        cur++;
    }
}

void seed_remove(void) {
    if(cur > 0){
        cur--;
        seed[cur] = '\0';
    }
}

void title_loop(void) {
    SDL_Event event;
    int i;
    gfx_clear();
    gfx_draw_sprite_from_atlas(&data.title, 0, 0, 1, 1, 0, WIDTH, HEIGHT, 0);
    gfx_draw_string(&data.font, "Press ENTER", (WIDTH-11*16)/2, 256, 16, 16);
    gfx_draw_string(&data.font, "Seed:", 32, 384, 16, 16);
    gfx_draw_string(&data.font, seed, 32+6*16, 384, 16, 16);
    gfx_draw_char(&data.font, '^', 32+6*16+cur*16, 400, 16, 16);
    gfx_draw_string(&data.font, "2024 Mibi88", 32, HEIGHT-32-16, 16, 16);
    gfx_draw_string(&data.font, VERSION, WIDTH-16*8, HEIGHT-32-16, 16, 16);
    glFlush();
    SDL_GL_SwapBuffers();
    last_time = SDL_GetTicks();
    while(SDL_PollEvent(&event)){
        switch(event.type){
            case SDL_KEYDOWN:
                for(i=0;i<10;i++){
                    if(keys[i] == event.key.keysym.sym){
                        seed_add(values[i]);
                    }
                }
                switch(event.key.keysym.sym){
                    case SDLK_BACKSPACE:
                        seed_remove();
                        /* FALLTHRU */
                    default:
                        break;
                }
                break;
            case SDL_KEYUP:
                switch(event.key.keysym.sym){
                    case SDLK_RETURN:
                        reset();
                        generate_dungeon(&data.dungeon, atoi(seed));
                        while(tilemap_get_tile(&data.dungeon.levels[data.map_n]
                                               .tilemap, player.x+32,
                                               player.y+32) == 0){
                            player.x += 64;
                            player.y += 64;
                        }
                        data.screen = 1;
                        break;
                    case SDLK_ESCAPE:
                        gfx_quit();
                        break;
                    default:
                        break;
                }
                break;
            case SDL_QUIT:
                gfx_quit();
                break;
            default:
                break;
        }
    }
}

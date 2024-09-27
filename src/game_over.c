#include <data.h>
#include <config.h>

void game_over_loop(void) {
    SDL_Event event;
    gfx_clear();
    gfx_draw_string(&data.font, "GAME OVER", (WIDTH-9*16)/2, 16, 16, 16);
    gfx_draw_string(&data.font, "Press ENTER to continue", (WIDTH-23*16)/2, 128,
                    16, 16);
    gfx_draw_string(&data.font, "Press ESC to go to the title screen",
                    (WIDTH-35*16)/2, 128+16, 16, 16);
    glFlush();
    SDL_GL_SwapBuffers();
    last_time = SDL_GetTicks();
    while(SDL_PollEvent(&event)){
        switch(event.type){
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym){
                    default:
                        break;
                }
                break;
            case SDL_KEYUP:
                switch(event.key.keysym.sym){
                    case SDLK_RETURN:
                        player.lives = 10;
                        data.screen = 1;
                        break;
                    case SDLK_ESCAPE:
                        data.screen = 0;
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

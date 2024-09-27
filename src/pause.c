#include <data.h>
#include <config.h>

void pause_loop(void) {
    SDL_Event event;
    gfx_clear();
    gfx_draw_string(&data.font, "PAUSE", (WIDTH-5*16)/2, 16, 16, 16);
    gfx_draw_string(&data.font, "Press ENTER to resume", (WIDTH-21*16)/2, 128,
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
                        data.screen = 1;
                        player.left = 0;
                        player.right = 0;
                        player.up = 0;
                        player.down = 0;
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

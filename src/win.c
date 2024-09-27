#include <data.h>
#include <config.h>

void win_loop(void) {
    SDL_Event event;
    gfx_clear();
    gfx_draw_string(&data.font, "YOU WIN!", (WIDTH-8*16)/2, 16, 16, 16);
    gfx_draw_string(&data.font, "THANKS FOR PLAYING!", (WIDTH-19*16)/2, 128, 16,
                    16);
    gfx_draw_string(&data.font, "Press ENTER to go to the title screen",
                    (WIDTH-37*16)/2, 128+48, 16, 16);
    gfx_draw_string(&data.font, "This game was made by Mibi88 for the",
                    (WIDTH-36*16)/2, 256, 16, 16);
    gfx_draw_string(&data.font, "Ludum Dare 55", (WIDTH-13*16)/2, 256+16, 16,
                    16);
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

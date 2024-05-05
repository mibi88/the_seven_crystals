#ifndef GFX_H
#define GFX_H

#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <GL/gl.h>
#include <GL/glu.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#define WIDTH 640
#define HEIGHT 480

typedef struct {
    float r, g, b;
} GFXColor;

typedef struct {
    int width;
    int height;
    unsigned int texture;
    GFXColor color; /* Because I'm using GL_MODULATE */
} GFXSprite;

void gfx_play_audio(char *file);
void gfx_draw_string(GFXSprite *font, char *str, int sx, int sy, int font_w,
                     int font_h);
void gfx_draw_char(GFXSprite *font, char c, int x, int y, int font_w,
                   int font_h);
void gfx_quit(void);
void gfx_draw_sprite_from_atlas(GFXSprite *sprite, int x, int y, int w, int h,
                                int i, int render_w, int render_h, int group);
void gfx_draw_sprite(GFXSprite *sprite, int x, int y);
GFXSprite gfx_load_texture(char *file);
void gfx_init(int vsync);
void gfx_mainloop(void loop(void));
void gfx_clear(void);

#endif

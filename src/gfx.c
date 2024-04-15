#include <gfx.h>

#include <string.h>

void gfx_clear(void) {
    glClear(GL_COLOR_BUFFER_BIT);
}

void gfx_draw_string(GFXSprite *font, char *str, int sx, int sy, int font_w,
                     int font_h) {
    int x = sx, y = sy;
    int i;
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, font->texture);
    glBegin(GL_QUADS);
    for(i=0;i<strlen(str);i++){
        if(str[i] == '\n'){
            x = sx;
            y += font_h;
        }else{
            gfx_draw_sprite_from_atlas(font, x, y, 16, 16, str[i]-0x20, font_w,
                                       font_h, 1);
            x += font_w;
        }
    }
    glEnd();
}

void gfx_draw_char(GFXSprite *font, char c, int x, int y, int font_w,
                   int font_h) {
    gfx_draw_sprite_from_atlas(font, x, y, 16, 16, c-0x20, font_w, font_h, 0);
}

void gfx_draw_sprite_from_atlas(GFXSprite *sprite, int x, int y, int w, int h,
                                int i, int render_w, int render_h, int group) {
    int atlas_x = i%w;
    int atlas_y = i/w;
    float step_x = 1.0/w;
    float step_y = 1.0/h;
    
    float x1 = atlas_x*step_x;
    float y1 = atlas_y*step_y;
    
    float x2 = x1+step_x;
    float y2 = y1+step_y;
    
    if(!group){
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, sprite->texture);
        
        glLoadIdentity();
    
        glBegin(GL_QUADS);
    }
    glColor3f(sprite->color.r, sprite->color.g, sprite->color.b);
    glTexCoord2f(x1, y2);
    glVertex2f(x, render_h+y);
    glColor3f(sprite->color.r, sprite->color.g, sprite->color.b);
    glTexCoord2f(x1, y1);
    glVertex2f(x, y);
    glColor3f(sprite->color.r, sprite->color.g, sprite->color.b);
    glTexCoord2f(x2, y1);
    glVertex2f(render_w+x, y);
    glColor3f(sprite->color.r, sprite->color.g, sprite->color.b);
    glTexCoord2f(x2, y2);
    glVertex2f(render_w+x, render_h+y);
    if(!group){
        glEnd();
    }
    
    /*float vertices[4*3] = {
        0, render_h, 0,
        0, 0, 0,
        render_w, 0, 0,
        render_w, render_h, 0
    };
    float tex_coords[8] = {
        x1, y2,
        x1, y1,
        x2, y1,
        x2, y2
    };
    int indices[4] = {
        0, 1, 2, 3
    };
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glVertexPointer(2, GL_FLOAT, 0, tex_coords);
    glDrawElements(GL_TRIANGLES, 4, GL_INT, indices);*/
    
    glDisable(GL_TEXTURE);
}

void gfx_draw_sprite(GFXSprite *sprite, int x, int y) {
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glColor3f(sprite->color.r, sprite->color.g, sprite->color.b);
    
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, sprite->texture);
    
    glLoadIdentity();
    
    glBegin(GL_QUADS);
    glTexCoord2f(0, 1);
    glVertex2f(x, sprite->height+y);
    glTexCoord2f(0, 0);
    glVertex2f(x, y);
    glTexCoord2f(1, 0);
    glVertex2f(sprite->width+x, y);
    glTexCoord2f(1, 1);
    glVertex2f(sprite->width+x, sprite->height+y);
    glEnd();
    
    glDisable(GL_TEXTURE);
}

GFXSprite gfx_load_texture(char *file) {
    unsigned int id = 0;
    SDL_Surface *image;
    GFXSprite sprite;
    const GFXColor default_color = {1.0, 1.0, 1.0};
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    
    image = IMG_Load(file);
    if(!image){
        puts("Failed to load texture!");
        exit(EXIT_FAILURE);
    }
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->w, image->h, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, image->pixels);
    
    sprite.width = image->w;
    sprite.height = image->h;
    sprite.texture = id;
    sprite.color = default_color;
    
    SDL_FreeSurface(image);
    
    return sprite;
}

void gfx_quit(void) {
    SDL_Quit();
    exit(EXIT_SUCCESS);
}

void gfx_init(int vsync) {
    const SDL_VideoInfo *info;
    int bitsperpixel;
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0){
        puts("SDL initialization failed!");
        exit(EXIT_FAILURE);
    }
    
    info = SDL_GetVideoInfo();
    if(!info){
        puts("Failed to get video info!");
        exit(EXIT_FAILURE);
    }
    
    bitsperpixel = info->vfmt->BitsPerPixel;
    
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    
#ifndef __EMSCRIPTEN__
    SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, vsync);
#endif
    
    if(!SDL_SetVideoMode(640, 480, bitsperpixel, SDL_OPENGL | SDL_FULLSCREEN)){
        puts("Failed to set the SDL video mode!");
        exit(EXIT_FAILURE);
    }
    
#ifndef __EMSCRIPTEN__
    if(Mix_OpenAudio(44100, AUDIO_U8, 2, 2048) < 0){
        puts("Failed to initialize SDL_mixer!");
        exit(EXIT_FAILURE);
    }
#endif
    
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
    glViewport(0, 0, WIDTH, HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WIDTH, HEIGHT, 0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    /* I'm not sure if depth test enabled or disabled by default */
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.5);
}

void gfx_mainloop(void loop(void)) {
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(loop, 0, 1);
#else
    while(1){
        loop();
    }
#endif
}

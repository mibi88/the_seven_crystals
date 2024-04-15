#!/bin/bash

# Yeah I should write a makefile
emcc src/main.c src/gfx.c src/tilemap.c src/mapgen.c src/entity.c src/bullet.c -sUSE_SDL=1 -sUSE_SDL_IMAGE=1 -sLEGACY_GL_EMULATION -sGL_FFP_ONLY -sSTACK_SIZE=524288 -std=c89 -Wall -Wpedantic --use-preload-plugins --preload-file img/font.png --preload-file img/tileset1.png --preload-file img/tileset2.png  --preload-file img/tileset3.png  --preload-file img/tileset4.png --preload-file img/tileset5.png --preload-file img/tileset6.png --preload-file img/tileset7.png --preload-file img/tileset8.png --preload-file img/player.png --preload-file img/slug.png --preload-file img/zombie.png --preload-file img/cannon.png --preload-file img/boss2.png  --preload-file img/boss3.png  --preload-file img/boss4.png  --preload-file img/boss5.png  --preload-file img/boss6.png  --preload-file img/boss7.png  --preload-file img/boss1.png  --preload-file img/megaboss.png --preload-file img/bullet.png -Iinc -o index.html -sUSE_SDL_MIXER=1
gcc src/main.c src/gfx.c src/tilemap.c src/mapgen.c src/entity.c src/bullet.c -o main -lGL -lGLU -lSDL -lSDL_image -lSDL_mixer -Iinc -lm

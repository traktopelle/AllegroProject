
#ifndef effect_H_INCLUDED
#define effect_H_INCLUDED

/* Includes */
#include <stdlib.h>
#include <stdio.h>
#include <allegro.h>

#include "Entity.h"

// La taille de l'écran en longueur et largeur :
// Pour un meilleur affichage, doit être impair
#define TILE_SCREEN_W   16
#define TILE_SCREEN_H   12

#define BOARD_HEIGHT    200
#define BOARD_WIDTH     SCREEN_WIDTH

#define SCREEN_WIDTH  (TILE_SIZE * TILE_SCREEN_W)
#define SCREEN_HEIGHT (TILE_SIZE * TILE_SCREEN_H)

#define RED             1
#define GREEN           2
#define BLUE            3
#define BLACK_AND_WHITE 4

void effect_set_color(BITMAP *img, int col, int intensity);
void draw_laser(BITMAP *double_buffer, Entity *e, int camera_x, int camera_y, int dir);

#endif

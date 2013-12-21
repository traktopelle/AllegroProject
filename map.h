#ifndef Map_H_INCLUDED
#define Map_H_INCLUDED

/* Includes */
#include <stdlib.h>
#include <stdio.h>


/* Defines */
// La largeur (width) et la longueur (height) de notre map
#define MAP_W   50
#define MAP_H   50

// La taille en pixel des tiles
#define TILE_SIZE       32

// Le nombre de tile pour la map
#define TILE_NOMBRE     6

// Définition des ID des tiles
#define HERBE1      4
#define HERBE2      2
#define HERBE3      3
#define MUR         5
#define PIERRE      1
#define TERRE       6

#define NOMBRE_TILE_BLOQUANT 1

extern const int map_tile [MAP_H][MAP_W];
extern const int map_bloquant[NOMBRE_TILE_BLOQUANT];

int is_in_map(int x, int y);
int is_bloquant (int x, int y);

#endif

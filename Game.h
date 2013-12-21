
#ifndef Game_H_INCLUDED
#define Game_H_INCLUDED

/* Includes */
#include <stdlib.h>
#include <stdio.h>
#include <allegro.h>

#define PROBA_CHANGE_FLEUR 10

#include "map.h"
#include "Object.h"

typedef struct Game_s
{
    int point;
    int is_shooting;
    int nb_coffre;

    int arme_selected;

    Object *arme_double;
    Object *lance_flamme;

    int is_pause;
    int is_over;
    int is_credit;

    int vie;

    BITMAP **sprite_img;
    BITMAP **hero_img;
    BITMAP **object_img;

}   Game;

#include "Entity.h"

Game *game_init();
int is_ok (int x, int y);
void get_random_pos (int *x, int *y);
int is_entity_ok (int x, int y);

#endif

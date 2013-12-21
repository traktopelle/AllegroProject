#ifndef Entity_H_INCLUDED
#define Entity_H_INCLUDED

/* Includes */
#include <stdlib.h>
#include <stdio.h>
#include <allegro.h>

#include "map.h"
#include "Game.h"
#include "Object.h"

// Définition du sens de la direction
#define DIRECTION_GAUCHE    0
#define DIRECTION_DROITE    1
#define DIRECTION_HAUT      2
#define DIRECTION_BAS       3
#define AUCUNE_DIRECTION    -1

// Le nombre maximum d'entités
#define ENTITY_MAX      200

// On fait commencer notre héros au début du jeu à une position définie
#define START_HERO_X    10
#define START_HERO_Y    10

// Probabilité que les monstres sortent des arbres
#define SPAWN_PROBABILITY 10000

// Helper pour choisir notre héros
#define hero_entity (entity_array[0])

typedef struct Entity_s
{
    float display_x, display_y;     // Position 2D sur l'écran
    int pos_x, pos_y;               //
    BITMAP **img;                   // Les images de notre entité
    int direction;                  // La direction où regarde l'entité (gauche, droite, haut, bas)
    int animation_index;            // Index de l'animation
    int moving_dir;                 // Direction vers laquelle l'entitée se déplace

    int is_arbre;                   // pour savoir si c'est un arbre ou pas
    int index_animation;            // index de l'animation

}   Entity;


/*
*   Le tableau de nos entités
*/
extern Entity *entity_array [ENTITY_MAX];

Entity *entity_new (int pos_x, int pos_y, BITMAP **img);

int is_occupied (int x, int y);

void entity_array_init ();

int bouger_entity (Entity *moving_entity, int direction);

void random_move_entity (Entity *e);

void add_entity (Entity *e);

void remove_entity (Entity *e);

void try_generate_entity (Entity *e);

Object* entity_is_on_object (Entity *e, int type);

int is_entity_near (Entity *e, Entity *src);

#endif

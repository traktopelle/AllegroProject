
#ifndef Object_H_INCLUDED
#define Object_H_INCLUDED

/* Includes */
#include <stdlib.h>
#include <stdio.h>
#include <allegro.h>

#define OBJET_MAX           50
#define NOMBRE_COFFRE       20
#define NOMBRE_OBJET_COFFRE 3

#define CONTAINER_DOUBLE         0
#define CONTAINER_LANCEFLAMME    1
#define CONTAINER_GOLD           2

#define NOMBRE_OBJECT       2
#define OBJECT_TYPE_COFFRE  0
#define OBJECT_TYPE_FLEUR   1

#define ARME_NORMAL         1
#define ARME_DOUBLE         2
#define ARME_LANCEFLAMME    3

#define ARME_DOUBLE_DUREE       30
#define ARME_LANCEFLAMME_DUREE  15

typedef struct Object_s
{
    BITMAP *img;

    int type;
    int is_bloquant;

    int x, y;
    int duree_taken;

    int container;

}   Object;

extern Object *object_array [OBJET_MAX];

Object *object_new (int x, int y, BITMAP *img, int type, int is_bloquant);

void object_init();

void add_object (Object *o);

void remove_object (Object *o);

int is_object(int x, int y);

Object* is_object_type(int x, int y, int type);

int is_lanceflamme_ig ();

#endif

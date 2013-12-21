#include "Game.h"


Game *game_init()
{
    Game *g = malloc(sizeof(Game));

    g->point = 0;
    g->is_shooting = 0;
    g->nb_coffre = 0;
    g->arme_selected = 1;
    g->arme_double = NULL;
    g->lance_flamme = NULL;
    g->is_pause = 0;
    g->is_over = 0;
    g->is_credit = 0;
    g->vie = 10;

    g->hero_img = malloc(sizeof(BITMAP *) * 4);
    g->hero_img[DIRECTION_GAUCHE] = load_bitmap("img/heros/gauche.bmp", NULL);
    g->hero_img[DIRECTION_DROITE] = load_bitmap("img/heros/droite.bmp", NULL);
    g->hero_img[DIRECTION_HAUT]   = load_bitmap("img/heros/dos.bmp", NULL);
    g->hero_img[DIRECTION_BAS]    = load_bitmap("img/heros/face.bmp", NULL);

    g->sprite_img = malloc(sizeof(BITMAP *) * 4);
    g->sprite_img[DIRECTION_GAUCHE] = load_bitmap("img/ennemi/ennemi_gauche.bmp", NULL);
    g->sprite_img[DIRECTION_DROITE] = load_bitmap("img/ennemi/ennemi_droite.bmp", NULL);
    g->sprite_img[DIRECTION_HAUT]   = load_bitmap("img/ennemi/ennemi_dos.bmp", NULL);
    g->sprite_img[DIRECTION_BAS]    = load_bitmap("img/ennemi/ennemi_face.bmp", NULL);

    g->object_img = malloc(sizeof(BITMAP *) * NOMBRE_OBJECT);
    g->object_img[OBJECT_TYPE_FLEUR] = load_bitmap("img/object/flower.bmp", NULL);
    g->object_img[OBJECT_TYPE_COFFRE] = load_bitmap("img/object/coffre.bmp", NULL);

    font = load_font("data/font.pcx", NULL, NULL);

    return g;
}


int is_ok (int x, int y)
{
    // On vérifie si aucune autre entité n'est sur la case visée
    if (is_occupied(x, y))
        return 0;

    // On vérifie si le personnage n'essaye pas de sortir de la map
    if (!is_in_map(x, y))
        return 0;

    // On vérifie si le personnage peut marcher si le tile visé
    if (is_bloquant(x, y))
        return 0;

    if (is_object(x, y))
        return 2;

    return 1;
}

int is_entity_ok (int x, int y)
{
    // On vérifie si aucune autre entité n'est sur la case visée
    if (is_occupied(x, y))
        return 0;

    // On vérifie si le personnage n'essaye pas de sortir de la map
    if (!is_in_map(x, y))
        return 0;

    // On vérifie si le personnage peut marcher si le tile visé
    if (is_bloquant(x, y))
        return 0;

    if (is_object(x, y))
        return 0;

    return 1;
}

void get_random_pos (int *x, int *y)
{
    int rand_x, rand_y;
    int ok = 0;
    int n = 0;

    do {
        rand_x = rand() % MAP_W;
        rand_y = rand() % MAP_H;

        if (is_ok(rand_x, rand_y))
        {
            *x = rand_x;
            *y = rand_y;
            ok = 1;
        }

        if (n++ > MAP_W * MAP_H)
        {
            *x = -1;
            *y = -1;
            ok = 1;
        }

    } while (!ok);

}

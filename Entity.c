#include "Entity.h"

Entity *entity_array [ENTITY_MAX];

Entity *entity_new (int pos_x, int pos_y, BITMAP **img)
{
    Entity *e = malloc (sizeof(Entity));

    e->pos_x = pos_x;
    e->pos_y = pos_y;
    e->img   = img;

    // Par définition
    e->display_x = (float)(pos_x * TILE_SIZE);
    e->display_y = (float)(pos_y * TILE_SIZE);

    // Par défaut
    e->direction  = DIRECTION_BAS;
    e->moving_dir = AUCUNE_DIRECTION;
    e->is_arbre   = 0;
    e->index_animation = 0;

    return e;
}


void entity_array_init ()
{
    int i;
    // On initialise la totalité des entités à NULL

    for (i = 0; i < ENTITY_MAX; i++)
    {
        if (entity_array[i] != NULL)
        {
            free(entity_array[i]);
        }

        entity_array[i] = NULL;
    }
}

int is_entity_near (Entity *e, Entity *src)
{
    // <-
    if (e->pos_x == src->pos_x - 1
    &&  e->pos_y == src->pos_y)
        return 1;

    // ->
    if (e->pos_x == src->pos_x + 1
    &&  e->pos_y == src->pos_y)
        return 1;

    // V
    if (e->pos_x == src->pos_x
    &&  e->pos_y - 1 == src->pos_y)
        return 1;

    // ^
    if (e->pos_x == src->pos_x
    &&  e->pos_y + 1 == src->pos_y)
        return 1;

    return 0;
}


int is_occupied (int x, int y)
{
    int i;
    Entity *entity = NULL;

    for (i = 0; i < ENTITY_MAX; i++)
    {
        entity = entity_array[i];

        if (entity == NULL)
            continue;

        if (entity->pos_x == x
        &&  entity->pos_y == y)
        {
            // Dans le cas où l'entité mouvante veut se placer sur la position d'une entité déjà existante,
            // On quitte le sous programme en retournant 1
            return 1;
        }
    }

    // Sinon on retourne 0 en cas de succès
    return 0;
}


int bouger_entity (Entity *moving_entity, int direction)
{
    // On définit l'offset de déplacement
    int offset_x = 0, offset_y = 0;
    int x_vise, y_vise;
    int ok = 0;

    // On commence à vérifier si l'entité n'est pas déjà en train de bouger
    if (moving_entity->moving_dir != AUCUNE_DIRECTION)
        return 0;

    switch (direction)
    {
        case DIRECTION_GAUCHE : offset_x = -1; break;
        case DIRECTION_DROITE : offset_x = 1;  break;
        case DIRECTION_HAUT   : offset_y = -1; break;
        case DIRECTION_BAS    : offset_y = 1;  break;
    }

    // On définit les coordonnées du tile visé
    x_vise = moving_entity->pos_x + offset_x;
    y_vise = moving_entity->pos_y + offset_y;

    // On vérifie si la case visée est accessible
    if ((ok = is_ok(x_vise, y_vise)) == 0
    ||   (ok == 2 && moving_entity != hero_entity))
        return 0;

    // On change la direction vers laquelle regarde le perso
    moving_entity->direction = direction;

    // Dans le cas où toutes les collisions sont évitées, on bouge notre entité
    moving_entity->pos_x += offset_x;
    moving_entity->pos_y += offset_y;
    moving_entity->moving_dir = direction;

    // Si tout est OK, on retourne 1
    return 1;
}

void random_move_entity (Entity *e)
{
    // On va essayer de faire bouger
    // l'entité de telle sorte qu'elle sorte de l'impasse,
    // Du moins qu'elle continue à bouger même dans une situation bloquante
    int move_ok = 0;
    int nb_try = 0;

    while (!move_ok)
    {
        move_ok = bouger_entity(e, rand()%4);

        // On évite une boucle infinie en imposant une limite
        if (nb_try++ > 10)
            return;
    }
}

void add_entity (Entity *e)
{
    int index;

    for (index = 0; index < ENTITY_MAX; index++)
    {
        if (entity_array[index] == NULL)
        {
            entity_array[index] = e;
            return;
        }
    }
}

void remove_entity (Entity *e)
{
    int index;

    for (index = 0; index < ENTITY_MAX; index++)
    {
        if (entity_array[index] == e)
        {
            entity_array[index] = NULL;
            free(e);
        }
    }
}

Object* entity_is_on_object (Entity *e, int type)
{
    return is_object_type(e->pos_x, e->pos_y, type);
}

void try_generate_entity (Entity *e)
{
    // Une chance sur x pour que l'arbre génère une entité
    int probability = SPAWN_PROBABILITY;

    if (rand() % probability == 0)
    {
        // On regarde aux alentours de l'arbre pour voir où est-ce qu'on peut générer l'entité
        // On essaye de le générer à gauche
        if (is_entity_ok(e->pos_x - 1, e->pos_y))
            add_entity(entity_new(e->pos_x - 1, e->pos_y, e->img));

        // A droite
        else if (is_entity_ok(e->pos_x + 1, e->pos_y))
            add_entity(entity_new(e->pos_x + 1, e->pos_y, e->img));

        // En haut
        else if (is_entity_ok(e->pos_x, e->pos_y - 1))
            add_entity(entity_new(e->pos_x, e->pos_y - 1, e->img));

        // En bas
        else if (is_entity_ok(e->pos_x, e->pos_y + 1))
            add_entity(entity_new(e->pos_x, e->pos_y + 1, e->img));
    }
}

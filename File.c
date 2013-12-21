#include "File.h"

int sauvegarder(Game *game)
{
    FILE *file = fopen("save.txt", "w+");
    if (file == NULL)
    {
        printf("Sauvegarde impossible\n");
        return 0;
    }

    char buffer[1024];
    int i;

    Object *o = NULL;
    Entity *e = NULL;

    sprintf(buffer, "%d;%d;\n", game->vie, game->point);
    fputs(buffer, file);

    for (i = 0; i < OBJET_MAX; i++)
    {
        o = object_array[i];
        if (o == NULL)
            continue;

        sprintf(buffer, "o;%d;%d;%d;%d;%d;\n", o->x, o->y, o->type, o->container, o->is_bloquant);
        fputs(buffer, file);
    }

    for (i = 0; i < ENTITY_MAX; i++)
    {
        e = entity_array[i];
        if (e == NULL)
            continue;

        if (i == 0)
            sprintf(buffer, "h;%d;%d;\n", e->pos_x, e->pos_y);

        else
            sprintf(buffer, "e;%d;%d;%d;\n", e->pos_x, e->pos_y, e->is_arbre);

        fputs(buffer, file);
    }

    if (game->arme_double != NULL)
    {
        sprintf(buffer, "d;%d;\n", game->arme_double->duree_taken);
        fputs(buffer, file);
    }

    if (game->lance_flamme != NULL)
    {
        sprintf(buffer, "f;%d;\n", game->lance_flamme->duree_taken);
        fputs(buffer, file);
    }

    fclose(file);

    return 1;
}

int load (Game *game)
{
    Entity *e = NULL;
    Object *o = NULL;

    char buffer;
    int data[5];

    FILE *file = fopen("save.txt", "r");
    if (file == NULL)
    {
        printf("Chargement impossible\n");
        return 0;
    }

    // On nettoie tout ce qu'il y avait avant
    entity_array_init();
    object_init();

    if (game->lance_flamme != NULL)
        free(game->lance_flamme);

    if (game->arme_double != NULL)
        free(game->arme_double);

    game->lance_flamme = NULL;
    game->arme_double = NULL;
    game->arme_selected = 1;

    fscanf(file, "%d;%d;\n", &game->vie, &game->point);

    while ((buffer = fgetc(file)) != EOF)
    {
        switch(buffer)
        {
            case 'h':
                fscanf(file, ";%d;%d;\n", &data[0], &data[1]);
                add_entity(entity_new(data[0], data[1], game->hero_img));
            break;

            case 'e':
                fscanf(file, ";%d;%d;%d;\n", &data[0], &data[1], &data[2]);
                add_entity(e = entity_new(data[0], data[1], game->sprite_img));
                e->is_arbre = data[2];
            break;

            case 'o':
                fscanf(file, ";%d;%d;%d;%d;%d;\n", &data[0], &data[1], &data[2], &data[3], &data[4]);
                add_object(o = object_new(data[0], data[1], game->object_img[data[2]], data[2], data[4]));
                o->container = data[3];
            break;

            case 'd':
                fscanf(file, ";%d;\n", &data[0]);
                game->arme_double = object_new(-1, -1, NULL, ARME_DOUBLE, 0);
                game->arme_double->duree_taken = data[0];
            break;

            case 'f':
                fscanf(file, ";%d;\n", &data[0]);
                game->lance_flamme = object_new(-1, -1, NULL, ARME_DOUBLE, 0);
                game->lance_flamme->duree_taken = data[0];
            break;
        }
    }

    fclose(file);

    return 1;
}

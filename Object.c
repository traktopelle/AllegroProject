#include "Object.h"

Object *object_array [OBJET_MAX];

Object *object_new (int x, int y, BITMAP *img, int type, int is_bloquant)
{
    int ok = 0;

    Object *o = malloc(sizeof(Object));

    o->x = x;
    o->y = y;
    o->img = img;

    o->type = type;
    o->is_bloquant = is_bloquant;
    o->duree_taken = 0;

    if (o->type == OBJECT_TYPE_COFFRE)
    {
        while (!ok)
        {
            o->container = rand() % NOMBRE_OBJET_COFFRE;

            if (o->container == CONTAINER_LANCEFLAMME
            && !is_lanceflamme_ig())
                ok = 1;

            else if (o->container != CONTAINER_LANCEFLAMME)
                ok = 1;
        }

    }
    else
        o->container = -1;

    return o;
}

int is_lanceflamme_ig ()
{
    int i;

    for (i = 0; i < OBJET_MAX; i++)
    {
        if (object_array[i] != NULL)
        {
            if (object_array[i]->container == CONTAINER_LANCEFLAMME)
                return 1;
        }
    }

    return 0;
}

void object_init()
{
    int i;

    for (i = 0; i < OBJET_MAX; i++)
    {
        if (object_array[i] != NULL)
        {
            free(object_array[i]);
        }

        object_array[i] = NULL;
    }
}

void add_object (Object *o)
{
    int i;

    for (i = 0; i < OBJET_MAX; i++)
    {
        if (object_array[i] == NULL)
        {
            object_array[i] = o;
            return;
        }
    }
}

void remove_object (Object *o)
{
    int i;

    for (i = 0; i < OBJET_MAX; i++)
    {
        if (object_array[i] == o)
        {
            object_array[i] = NULL;
            free(o);
        }
    }
}

int is_object(int x, int y)
{
    int i;
    Object *o = NULL;

    for (i = 0; i < OBJET_MAX; i++)
    {
        o = object_array[i];

        if (o == NULL)
            continue;

        if (o->x == x
        &&  o->y == y)
        {
            return 1;
        }
    }

    // Sinon on retourne 0 en cas de succès
    return 0;
}

Object* is_object_type(int x, int y, int type)
{
    int i;
    Object *o = NULL;

    for (i = 0; i < OBJET_MAX; i++)
    {
        o = object_array[i];

        if (o == NULL)
            continue;

        if (o->x == x
        &&  o->y == y
        &&  o->type == type)
        {
            return o;
        }
    }

    // Sinon on retourne 0 en cas de succès
    return NULL;
}

void
object_free (Object *p)
{
	if (p != NULL)
	{
		free(p);
	}
}


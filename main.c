#include <allegro.h>
#include <winalleg.h>
#include <windows.h>
#include <time.h>
#include <sys\timeb.h>

#include "Game.h"
#include "File.h"
#include "Effect.h"

int main()
{
    /* Variables d'Allegro */
    BITMAP *double_buffer = NULL; // Notre double buffer
    BITMAP **tile_img = NULL;
    BITMAP *map = NULL;
    BITMAP *bitmap = NULL;
    BITMAP *arbre = NULL;
    BITMAP *board_background = NULL;
    BITMAP *board_case = NULL;
    BITMAP *board_rempli = NULL;
    BITMAP *board_grise = NULL;
    BITMAP *board_touches = NULL;
    BITMAP *board_pause = NULL;
    BITMAP *board_coeur = NULL;
    BITMAP *board_over = NULL;
    BITMAP *board_score = NULL;
    BITMAP *firewp = NULL;
    BITMAP *tmp = NULL;
    BITMAP *hero_feu = NULL;
    BITMAP *dump = NULL;
    FONT   *score_font = NULL;

    /* Variables de jeu */
    int quit = 0; // Variable qui passe à 1 quand on veut quitter la boucle de jeu
    float camera_x = 0, camera_y = 0; // Variable stockant la position de la caméra
    int tile;
    int x, y, i;
    int entity_index;
    int object_index;
    int t_diff;
    int t_diff_arme;
    int deplacement = 1;
    int x_tir[8];
    int y_tir[8];
    int is_f3 = 0, is_f2 = 0, is_f1 = 0, is_f4 = 0;
    int hurt = 0;
    int feu_x = SCREEN_WIDTH;
    int anim = 0;
    int acquired = -1;
    int gold = 0;

    struct timeb t_start, t_current;
    struct timeb t_start_arme, t_current_arme;
    Entity *entity = NULL;
    Object *object = NULL;
    Game *game = NULL;

    /** Initialisation d'Allegro **/
    allegro_init();
    set_color_depth(32);
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, SCREEN_WIDTH, SCREEN_HEIGHT + BOARD_HEIGHT, 0, 0);
    install_keyboard();
    install_mouse();
    show_mouse(screen);
    set_window_title("Allegro Titre");
    set_display_switch_mode(SWITCH_BACKGROUND);

    /** Initialisation variables d'Allegro **/
    double_buffer = create_bitmap(SCREEN_W, SCREEN_H);
    dump          = create_bitmap(SCREEN_W, SCREEN_H);

    /* Initialisation du générateur de nombre aléatoire */
    srand(time(NULL));

    /* Initialisation des images */
    // Chargement des personnages :
    arbre    = load_bitmap("img/entity/arbre.bmp", NULL);
    hero_feu = load_bitmap("img/entity/hero_feu.bmp", NULL);
    firewp   = load_bitmap("img/weapon/firewp.bmp", NULL);

    // Chargement des terrains (tiles)
    tile_img = malloc(sizeof(BITMAP *) * TILE_NOMBRE);
    tile_img[TERRE-1]  = load_bitmap("img/tiles/terre.bmp", NULL);
    tile_img[HERBE1-1]  = load_bitmap("img/tiles/herbe1.bmp", NULL);
    tile_img[HERBE2-1]  = load_bitmap("img/tiles/herbe2.bmp", NULL);
    tile_img[HERBE3-1]  = load_bitmap("img/tiles/herbe3.bmp", NULL);
    tile_img[MUR-1] = load_bitmap("img/tiles/mur.bmp", NULL);
    tile_img[PIERRE-1]    = load_bitmap("img/tiles/pierre.bmp", NULL);

    // Chargement de l'interface (board)
    board_background = load_bitmap("img/board/background.bmp", NULL);
    tmp = create_bitmap(SCREEN_WIDTH, BOARD_HEIGHT);
    for (i = 0; i < SCREEN_WIDTH; i++)
        blit(board_background, tmp, 0, 0, i, 0, board_background->w, board_background->h);
    board_background = tmp;

    board_case    = load_bitmap("img/board/case.bmp", NULL);
    board_rempli  = load_bitmap("img/board/rempli.bmp", NULL);
    board_grise   = load_bitmap("img/board/gris.bmp", NULL);
    board_touches = load_bitmap("img/board/touches.bmp", NULL);
    board_pause   = load_bitmap("img/board/pause.bmp", NULL);
    board_over    = load_bitmap("img/board/gameover.bmp", NULL);
    board_coeur   = load_bitmap("img/board/coeur.bmp", NULL);
    board_score   = load_bitmap("img/board/score.bmp", NULL);

    // Création de la bitmap acceuillant la map
    map = create_bitmap(MAP_W * TILE_SIZE, MAP_H * TILE_SIZE);

    score_font = load_font("data/score.pcx", NULL, NULL);

    /** Initialisation variables de jeu **/
    entity_array_init();
    object_init();
    game = game_init();
    add_entity(entity_new(START_HERO_X, START_HERO_Y, game->hero_img));

    for (i = 0; i < 10; i++)
    {
        get_random_pos(&x, &y);
        add_entity(entity = entity_new(x, y, game->sprite_img));
        entity->is_arbre = 1;
    }

    /* Construction de l'image de la map */
    for (y = 0; y < MAP_H; y++)
    {
        for (x = 0; x < MAP_W; x++)
        {
            // Le tile selectionné est à la position {x, y}
            tile = map_tile[y][x];
            blit (
                  tile_img[tile-1],               // Source : L'image de la tile en {x, y}
                  map,                          // Destination : l'image de la map
                  0, 0,                         //  Source x, y : On prends du haut à gauche du tile
                  x * TILE_SIZE, y * TILE_SIZE, // Destination x, y : On le place à la position {x * 32, y * 32}
                  TILE_SIZE, TILE_SIZE          // On blit une image de la taille TILE_SIZE * TILE_SIZE pixels
            );
        }
    }

    ftime(&t_start);
    ftime(&t_start_arme);

    /**
    *       Boucle de jeu
    */
    while (!quit)
    {
        /**
        *   Gestion de l'affichage
        **/
        ftime(&t_current);
        ftime(&t_current_arme);
        t_diff = (int) (1000.0 * (t_current.time - t_start.time) + (t_current.millitm - t_start.millitm));
        t_diff_arme = (int) (1000.0 * (t_current_arme.time - t_start_arme.time) + (t_current_arme.millitm - t_start_arme.millitm));

        /**
        *   Gestion des évenements clavier / souris
        **/

        if (key[KEY_F1] && !is_f1)
        {
            is_f1 = 1;
            sauvegarder(game);
        }
        else if (!key[KEY_F1])
            is_f1 = 0;

        if (key[KEY_F2] && !is_f2)
        {
            is_f2 = 1;
            load(game);
        }
        else if (!key[KEY_F2])
            is_f2 = 0;

        if (key[KEY_F3] && !is_f3)
        {
            is_f3 = 1;
            game->is_pause = !game->is_pause;
            effect_set_color(double_buffer, BLACK_AND_WHITE, 1);
            masked_blit(board_pause, double_buffer, 0, 0, (SCREEN_W - board_pause->w) / 2, (SCREEN_HEIGHT - board_pause->h) / 2, board_pause->w, board_pause->h);
        }

        else if (!key[KEY_F3])
            is_f3 = 0;

        if (key[KEY_F4] && !is_f4)
        {
            is_f4 = 1;
            game->is_credit = !game->is_credit;
        }
        else if (!key[KEY_F4])
            is_f4 = 0;

        if (key[KEY_ESC])
        {
            // Si échap => On quitte la boucle de jeu
            quit = 1;
        }

        if (key[KEY_LSHIFT])
        {
            // On change juste la direction vers laquelle regarde le perso
            if (key[KEY_LEFT])  hero_entity->direction = DIRECTION_GAUCHE;
            if (key[KEY_RIGHT]) hero_entity->direction = DIRECTION_DROITE;
            if (key[KEY_DOWN])  hero_entity->direction = DIRECTION_BAS;
            if (key[KEY_UP])    hero_entity->direction = DIRECTION_HAUT;
        }

        else
        {
            // Si on appuie sur une touche directionnelle, on bouge le héros
            if (key[KEY_LEFT])  bouger_entity(hero_entity, DIRECTION_GAUCHE);
            if (key[KEY_RIGHT]) bouger_entity(hero_entity, DIRECTION_DROITE);
            if (key[KEY_DOWN])  bouger_entity(hero_entity, DIRECTION_BAS);
            if (key[KEY_UP])    bouger_entity(hero_entity, DIRECTION_HAUT);
        }

        // On gère le tir ici
        if (key[KEY_SPACE])
            game->is_shooting = game->arme_selected;
        else
            game->is_shooting = 0;

        if (key[KEY_Q])
            game->arme_selected = ARME_NORMAL;

        if (key[KEY_W] && game->arme_double != NULL)
            game->arme_selected = ARME_DOUBLE;

        if (key[KEY_E] && game->lance_flamme != NULL)
            game->arme_selected = ARME_LANCEFLAMME;

        if (!game->is_pause && !game->is_over)
        {
            /**
            *   Gestion des évenements du jeu
            */
            // Plus de vie = Game Over !
            if (game->vie < 0)
            {
                game->is_over = 1;
                blit(double_buffer, dump, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
            }

            if (t_diff_arme > 1000)
            {
                ftime(&t_start_arme);
                // Toutes les secondes, on ajoute une seconde au chrono des armes
                if (game->arme_selected == ARME_DOUBLE)
                {
                    game->arme_double->duree_taken++;

                    if (game->arme_double->duree_taken == ARME_DOUBLE_DUREE)
                    {
                        // La durée est terminée
                        free(game->arme_double);
                        game->arme_double = NULL;
                        game->arme_selected = 1;
                    }
                }

                else if (game->arme_selected == ARME_LANCEFLAMME)
                {
                    game->lance_flamme->duree_taken++;

                    if (game->lance_flamme->duree_taken == ARME_LANCEFLAMME_DUREE)
                    {
                        free(game->lance_flamme);
                        game->lance_flamme = NULL;
                        game->arme_selected = 1;
                    }
                }
            }

            // On va faire apparaître les coffres manquants ici
            if (game->nb_coffre < NOMBRE_COFFRE)
            {
                get_random_pos(&x, &y);
                if (x != -1 && y != -1)
                {
                    add_object(object_new(x, y, game->object_img[OBJECT_TYPE_COFFRE], OBJECT_TYPE_COFFRE, 0));
                    game->nb_coffre++;
                }
            }

            // On vérifie si le héros est sur un coffre
            if ((object = entity_is_on_object(hero_entity, OBJECT_TYPE_COFFRE)) != NULL)
            {
                gold = 0;

                switch (object->container)
                {
                    case CONTAINER_LANCEFLAMME:
                        acquired = CONTAINER_LANCEFLAMME;
                        if (game->lance_flamme != NULL)
                            game->lance_flamme->duree_taken = 0;
                        else
                            game->lance_flamme = object_new(-1, -1, NULL, CONTAINER_LANCEFLAMME, 0);
                    break;

                    case CONTAINER_DOUBLE:
                        acquired = CONTAINER_DOUBLE;
                        if (game->arme_double != NULL)
                            game->arme_double->duree_taken = 0;
                         else
                            game->arme_double = object_new(-1, -1, NULL, CONTAINER_DOUBLE, 0);
                    break;

                    case CONTAINER_GOLD:
                        acquired = CONTAINER_GOLD;
                        gold = rand() % 500;
                        game->point += gold;
                    break;
                }

                remove_object(object);
                game->nb_coffre--;
            }

            if ((object = entity_is_on_object(hero_entity, OBJECT_TYPE_FLEUR)) != NULL)
            {
                if (game->vie < 10)
                    game->vie++;

                remove_object(object);
            }

            // On va faire bouger les montres ici : IA des ennemis
            for (entity_index = 1; entity_index < ENTITY_MAX; entity_index++)
            {
                entity = entity_array[entity_index];

                if (entity == NULL)
                    continue;

                if (entity->is_arbre == 1)
                {
                    // Si l'entité est un arbre, pas la peine de le faire bouger.
                    try_generate_entity(entity);
                }

                else if (entity->moving_dir == AUCUNE_DIRECTION)
                {
                    if (abs(entity->pos_x - hero_entity->pos_x) > abs(entity->pos_y - hero_entity->pos_y))
                    {
                        if (hero_entity->pos_x > entity->pos_x)
                        {
                            if (bouger_entity(entity, DIRECTION_DROITE) == 0)
                            {
                                if (hero_entity->pos_y > entity->pos_y)
                                {
                                    if (bouger_entity(entity, DIRECTION_BAS) == 0)
                                    {
                                        random_move_entity(entity);
                                    }
                                }

                                else
                                {
                                    if (bouger_entity(entity, DIRECTION_HAUT) == 0)
                                    {
                                        random_move_entity(entity);
                                    }
                                }
                            }
                        }

                        else
                        {
                            if (bouger_entity(entity, DIRECTION_GAUCHE) == 0)
                            {
                                if (hero_entity->pos_y > entity->pos_y)
                                {
                                    if (bouger_entity(entity, DIRECTION_BAS) == 0)
                                    {
                                        random_move_entity(entity);
                                    }
                                }

                                else
                                {
                                    if (bouger_entity(entity, DIRECTION_HAUT) == 0)
                                    {
                                        random_move_entity(entity);
                                    }
                                }
                            }
                        }
                    }

                    else
                    {
                        if (hero_entity->pos_y > entity->pos_y)
                        {
                            if (bouger_entity(entity, DIRECTION_BAS) == 0)
                            {
                                if (hero_entity->pos_x > entity->pos_x)
                                {
                                    if (bouger_entity(entity, DIRECTION_DROITE) == 0)
                                    {
                                        random_move_entity(entity);
                                    }
                                }

                                else
                                {
                                    if (bouger_entity(entity, DIRECTION_GAUCHE) == 0)
                                    {
                                        random_move_entity(entity);
                                    }
                                }
                            }
                        }

                        else
                        {
                            if (bouger_entity(entity, DIRECTION_HAUT) == 0)
                            {
                                if (hero_entity->pos_x > entity->pos_x)
                                {
                                    if (bouger_entity(entity, DIRECTION_DROITE) == 0)
                                    {
                                        random_move_entity(entity);
                                    }
                                }

                                else
                                {
                                    if (bouger_entity(entity, DIRECTION_GAUCHE) == 0)
                                    {
                                        random_move_entity(entity);
                                    }
                                }
                            }
                        }
                    }
                }
            }

            if (game->is_shooting)
            {
                for (i = 0; i < 8; i++)
                {
                    x_tir[i] = hero_entity->pos_x;
                    y_tir[i] = hero_entity->pos_y;
                }

                if (game->is_shooting == ARME_NORMAL
                ||  game->is_shooting == ARME_LANCEFLAMME)
                {
                    switch (hero_entity->direction)
                    {
                        case DIRECTION_BAS:
                            y_tir[0] = hero_entity->pos_y + 1;
                            y_tir[1] = hero_entity->pos_y + 2;
                            y_tir[2] = hero_entity->pos_y + 3;
                            y_tir[3] = hero_entity->pos_y + 4;
                        break;

                        case DIRECTION_HAUT:
                            y_tir[0] = hero_entity->pos_y - 1;
                            y_tir[1] = hero_entity->pos_y - 2;
                            y_tir[2] = hero_entity->pos_y - 3;
                            y_tir[3] = hero_entity->pos_y - 4;
                        break;

                        case DIRECTION_DROITE:
                            x_tir[0] = hero_entity->pos_x + 1;
                            x_tir[1] = hero_entity->pos_x + 2;
                            x_tir[2] = hero_entity->pos_x + 3;
                            x_tir[3] = hero_entity->pos_x + 4;
                        break;

                        case DIRECTION_GAUCHE:
                            x_tir[0] = hero_entity->pos_x - 1;
                            x_tir[1] = hero_entity->pos_x - 2;
                            x_tir[2] = hero_entity->pos_x - 3;
                            x_tir[3] = hero_entity->pos_x - 4;
                        break;
                    }
                }

                else if (game->is_shooting == ARME_DOUBLE)
                {
                    switch (hero_entity->direction)
                    {
                        case DIRECTION_BAS:
                            y_tir[0] = hero_entity->pos_y + 1;
                            y_tir[1] = hero_entity->pos_y + 2;
                            y_tir[2] = hero_entity->pos_y + 3;
                            y_tir[3] = hero_entity->pos_y + 4;
                            y_tir[4] = hero_entity->pos_y - 1;
                            y_tir[5] = hero_entity->pos_y - 2;
                            y_tir[6] = hero_entity->pos_y - 3;
                            y_tir[7] = hero_entity->pos_y - 4;
                        break;

                        case DIRECTION_HAUT:
                            y_tir[0] = hero_entity->pos_y + 1;
                            y_tir[1] = hero_entity->pos_y + 2;
                            y_tir[2] = hero_entity->pos_y + 3;
                            y_tir[3] = hero_entity->pos_y + 4;
                            y_tir[4] = hero_entity->pos_y - 1;
                            y_tir[5] = hero_entity->pos_y - 2;
                            y_tir[6] = hero_entity->pos_y - 3;
                            y_tir[7] = hero_entity->pos_y - 4;
                        break;

                        case DIRECTION_DROITE:
                            x_tir[0] = hero_entity->pos_x + 1;
                            x_tir[1] = hero_entity->pos_x + 2;
                            x_tir[2] = hero_entity->pos_x + 3;
                            x_tir[3] = hero_entity->pos_x + 4;
                            x_tir[4] = hero_entity->pos_x - 1;
                            x_tir[5] = hero_entity->pos_x - 2;
                            x_tir[6] = hero_entity->pos_x - 3;
                            x_tir[7] = hero_entity->pos_x - 4;
                        break;

                        case DIRECTION_GAUCHE:
                            x_tir[0] = hero_entity->pos_x + 1;
                            x_tir[1] = hero_entity->pos_x + 2;
                            x_tir[2] = hero_entity->pos_x + 3;
                            x_tir[3] = hero_entity->pos_x + 4;
                            x_tir[4] = hero_entity->pos_x - 1;
                            x_tir[5] = hero_entity->pos_x - 2;
                            x_tir[6] = hero_entity->pos_x - 3;
                            x_tir[7] = hero_entity->pos_x - 4;
                        break;
                    }
                }

                for (entity_index = 1; entity_index < ENTITY_MAX; entity_index++)
                {
                    entity = entity_array[entity_index];

                    if (entity == NULL)
                        continue;

                    if (entity->is_arbre && game->is_shooting != ARME_LANCEFLAMME)
                        continue;

                    for (i = 0; i < 8; i++)
                    {
                        if (entity->pos_x == x_tir[i]
                        &&  entity->pos_y == y_tir[i])
                        {
                            int random = rand() % PROBA_CHANGE_FLEUR;
                            // L'entité est dans la zone de tir

                            switch (game->is_shooting)
                            {
                                case ARME_NORMAL:
                                case ARME_DOUBLE:

                                    if (random == 0)
                                    {
                                        add_object(object_new(entity->pos_x, entity->pos_y, game->object_img[OBJECT_TYPE_FLEUR], OBJECT_TYPE_FLEUR, 0));
                                        remove_entity(entity);
                                    }
                                    else
                                    {
                                        entity->is_arbre = 1;
                                        game->point += 10;
                                    }

                                break;

                                case ARME_LANCEFLAMME:
                                    // On remove l'entité puisque le lanceflamme crame tout
                                    remove_entity(entity);
                                break;
                            }

                        }
                    }
                }

                // Si on tire au lanceflamme, on doit aussi virer les entités objets fleurs
                if (game->is_shooting == ARME_LANCEFLAMME)
                {
                    for (object_index = 0; object_index < OBJET_MAX; object_index++)
                    {
                        object = object_array[object_index];

                        if (object == NULL)
                            continue;

                        if (object->type != OBJECT_TYPE_FLEUR)
                            continue;

                        for (i = 0; i < 8; i++)
                        {
                            if (object->x == x_tir[i]
                            &&  object->y == y_tir[i])
                            {
                                remove_object(object);
                            }
                        }
                    }
                }
            }
        }

        if (t_diff > 1000 / 100) // 100 FPS
        {
            ftime(&t_start);

            if (!game->is_over && !game->is_pause)
            {
                // En jeu

                // On actualise la position de la caméra
                camera_x = (SCREEN_WIDTH  / 2) - hero_entity->display_x - (TILE_SIZE / 2);
                camera_y = (SCREEN_HEIGHT / 2) - hero_entity->display_y - (TILE_SIZE / 2);

                // On vérifie que la caméra ne dépasse pas le bord de la carte
                if (camera_x > 0) camera_x = 0;
                if (camera_y > 0) camera_y = 0;
                if (camera_x < MAP_W * TILE_SIZE * -1 + SCREEN_WIDTH)
                    camera_x = MAP_W * TILE_SIZE * -1 + SCREEN_WIDTH;
                if (camera_y < MAP_H * TILE_SIZE * -1 + SCREEN_HEIGHT)
                    camera_y = MAP_H * TILE_SIZE * -1 + SCREEN_HEIGHT;

                // On blit notre map à la bonne position
                blit(map, double_buffer, 0, 0, camera_x, camera_y, map->w, map->h);

                for (i = 0; i < OBJET_MAX; i++)
                {
                    object = object_array[i];
                    if (object == NULL)
                        continue;

                    masked_blit (
                        object->img,
                        double_buffer,
                        0, 0,
                        (object->x * TILE_SIZE) + camera_x,
                        (object->y * TILE_SIZE) + camera_y,
                        object->img->w,
                        object->img->h
                    );
                }

                // On blit nos entités
                for (entity_index = 0; entity_index < ENTITY_MAX; entity_index++)
                {
                    entity = entity_array[entity_index]; // L'entité selectionné dans le tableau

                    // On vérifie si l'entité existe dans le tableau
                    if (entity != NULL)
                    {
                        // Si l'entité est un arbre
                        if (entity->is_arbre == 1
                        &&  entity->moving_dir == AUCUNE_DIRECTION)
                        {
                            // On affiche un arbre
                            bitmap = arbre;
                        }

                        else
                        {
                            bitmap = entity->img[entity->direction]; // Le sprite de l'entité selon sa direction

                            // On vérifie que l'entité est en train de bouger :
                            if (entity->moving_dir != AUCUNE_DIRECTION)
                            {
                                // Dans le cas où elle est train de bouger, on actualise sa position

                                entity->index_animation++;

                                if (entity == hero_entity)
                                    deplacement = 2;

                                else
                                    deplacement = 1;

                                switch (entity->moving_dir)
                                {
                                    case DIRECTION_GAUCHE : entity->display_x += (deplacement) * -1; break;
                                    case DIRECTION_DROITE : entity->display_x += deplacement;  break;
                                    case DIRECTION_HAUT   : entity->display_y += (deplacement) * -1; break;
                                    case DIRECTION_BAS    : entity->display_y += deplacement;  break;
                                }

                                // On vérifie si elle est arrivée à destination
                                if (entity->display_x == (float)(entity->pos_x * TILE_SIZE)
                                &&  entity->display_y == (float)(entity->pos_y * TILE_SIZE))
                                {
                                    entity->index_animation = 0;
                                    // Dans ce cas, on retourne l'entitée à son état immobile
                                    entity->moving_dir = AUCUNE_DIRECTION;

                                    // On vérifie si elle est arrivée à un carré adjacent du héros
                                    if (is_entity_near(entity, hero_entity))
                                    {
                                        game->vie--;
                                        hurt = 155 - (game->vie * 15);
                                    }
                                }
                            }

                            // On vérifie si l'entité est au bord de l'écran
                            if (entity->pos_x == hero_entity->pos_x - TILE_SCREEN_W / 2
                            ||  entity->pos_y == hero_entity->pos_y - TILE_SCREEN_H / 2
                            ||  entity->pos_x == hero_entity->pos_x + TILE_SCREEN_W / 2
                            ||  entity->pos_y == hero_entity->pos_y + TILE_SCREEN_H / 2
                            )
                            {
                                circlefill (
                                    double_buffer,
                                    entity->display_x + TILE_SIZE / 2 + camera_x,
                                    entity->display_y + TILE_SIZE / 2 + camera_y,
                                    TILE_SIZE / 2,
                                    makecol(255, 0, 0)
                                );
                            }
                        }

                        // Et on l'affiche à sa position {display_x, display_y}
                        masked_blit (
                            bitmap, double_buffer,
                            (entity->index_animation / (10 / deplacement) % 3) * 32, 0,
                            entity->display_x + camera_x,
                            entity->display_y + camera_y,
                            TILE_SIZE, TILE_SIZE
                        );
                    }
                }

                // On affiche notre laser
                switch (game->is_shooting)
                {
                    case ARME_NORMAL:
                        draw_laser(double_buffer, hero_entity, camera_x, camera_y, hero_entity->direction);
                    break;

                    case ARME_DOUBLE:
                        if (hero_entity->direction == DIRECTION_GAUCHE || hero_entity->direction == DIRECTION_DROITE)
                        {
                            draw_laser(double_buffer, hero_entity, camera_x, camera_y, DIRECTION_GAUCHE);
                            draw_laser(double_buffer, hero_entity, camera_x, camera_y, DIRECTION_DROITE);
                        }
                        else
                        {
                            draw_laser(double_buffer, hero_entity, camera_x, camera_y, DIRECTION_HAUT);
                            draw_laser(double_buffer, hero_entity, camera_x, camera_y, DIRECTION_BAS);
                        }
                    break;

                    case ARME_LANCEFLAMME:
                        switch (hero_entity->direction)
                        {
                            case DIRECTION_BAS:
                                masked_blit (
                                    firewp, double_buffer,
                                    54, 0,
                                    hero_entity->display_x + camera_x - 4,
                                    hero_entity->display_y + camera_y + 15,
                                    54, firewp->h
                                );
                            break;
                            case DIRECTION_GAUCHE:
                                masked_blit (
                                    firewp, double_buffer,
                                    54*2, 0,
                                    hero_entity->display_x + camera_x - 36,
                                    hero_entity->display_y + camera_y - 11,
                                    54, firewp->h
                                );
                            break;
                            case DIRECTION_DROITE:
                                masked_blit (
                                    firewp, double_buffer,
                                    54*3, 0,
                                    hero_entity->display_x + camera_x + 14,
                                    hero_entity->display_y + camera_y - 9,
                                    54, firewp->h
                                );
                            break;
                            case DIRECTION_HAUT:
                                masked_blit (
                                    firewp, double_buffer,
                                    0, 0,
                                    hero_entity->display_x + camera_x - 14,
                                    hero_entity->display_y + camera_y - 34,
                                    54, firewp->h
                                );
                            break;
                        }
                    break;
                }

                // On affiche la board ici
                blit(board_background, double_buffer, 0, 0, 0, SCREEN_HEIGHT, board_background->w, board_background->h);

                // Les deux barres vides
                blit(board_case, double_buffer, 0, 0, 10, SCREEN_HEIGHT + 10, board_case->w, board_case->h);
                blit(board_case, double_buffer, 0, 0, 10, SCREEN_HEIGHT + 50, board_case->w, board_case->h);

                // Barre de réservoir de l'arme double
                if (game->arme_double != NULL)
                {
                    int duree = 270 - ((game->arme_double->duree_taken) * (270/ARME_DOUBLE_DUREE));

                    if (game->arme_selected == ARME_DOUBLE)
                        tmp = board_rempli;
                    else
                        tmp = board_grise;

                    blit(tmp, double_buffer, 0, 0, 12, SCREEN_HEIGHT + 12, duree, tmp->h);
                }

                // Barre de réservoir du lance flamme
                if (game->lance_flamme != NULL)
                {
                    int duree = 270 - ((game->lance_flamme->duree_taken) * (270/ARME_LANCEFLAMME_DUREE));

                    if (game->arme_selected == ARME_LANCEFLAMME)
                        tmp = board_rempli;
                    else
                        tmp = board_grise;


                    blit(tmp, double_buffer, 0, 0, 12, SCREEN_HEIGHT + 52, duree, board_rempli->h);
                }

                // On affiche les touches
                masked_blit(board_touches, double_buffer, 0, 0, 10, SCREEN_HEIGHT + 90, board_touches->w, board_touches->h);

                // On affiche les coeurs
                for (i = 0; i < game->vie; i++)
                {
                    masked_blit (
                        board_coeur, double_buffer,
                        0, 0,
                        (SCREEN_W - 30 - (i * (board_coeur->w + 5))),
                        SCREEN_HEIGHT - 10,
                        board_coeur->w, board_coeur->h
                    );
                }

                // On affiche la minimap
                stretch_blit(map, double_buffer, 0, 0, map->w, map->h, SCREEN_W - 220, SCREEN_HEIGHT + 10, 210, 180);
                float dx = 210.0 / (float)(MAP_W * TILE_SIZE);
                float dy = 180.0 / (float)(MAP_H * TILE_SIZE);

                rectfill(double_buffer,
                         ((hero_entity->pos_x) * TILE_SIZE) * dx + SCREEN_W - 220,
                         ((hero_entity->pos_y) * TILE_SIZE) * dy + SCREEN_HEIGHT + 10,
                         ((hero_entity->pos_x+1) * TILE_SIZE) * dx + SCREEN_W - 220,
                         ((hero_entity->pos_y+1) * TILE_SIZE) * dy + SCREEN_HEIGHT + 10,
                         makecol(255, 0, 0)
                );

                // Affichage du score
                masked_blit(board_score, double_buffer, 0, 0, 14, SCREEN_HEIGHT + 158, board_score->w, board_score->h);

                textprintf_ex (
                    double_buffer, font,
                    40, SCREEN_HEIGHT + 175,
                    makecol(0,0,0), -1,
                    "%d", game->point
                );

                if (acquired != -1)
                {
                    switch (acquired)
                    {
                        case CONTAINER_GOLD:

                            textprintf_ex (
                                double_buffer, font,
                                150, SCREEN_HEIGHT + 175,
                                makecol(0,0,0), -1,
                                "%d Or", gold
                            );

                        break;

                        case CONTAINER_LANCEFLAMME:

                            textprintf_ex (
                                double_buffer, font,
                                150, SCREEN_HEIGHT + 175,
                                makecol(0,0,0), -1,
                                "Lance-Flamme !"
                            );
                        break;

                        case CONTAINER_DOUBLE:

                            textprintf_ex (
                                double_buffer, font,
                                150, SCREEN_HEIGHT + 175,
                                makecol(0,0,0), -1,
                                "Double Laser !"
                            );
                        break;

                    }
                }

                // Un monstre nous a touché, on affiche une animation rougeatre
                // Pour prévenir le joueur
                if (hurt > 0)
                {
                    effect_set_color(double_buffer, RED, hurt);
                    hurt--;
                }

                // On switch notre double buffer dans notre screen,
                // car nous avons fini de travailler avec tous les éléments graphiques
                blit(double_buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
            }

            else
            {
                if (game->is_pause)
                {
                    // Le jeu est en pause, on affiche la pause
                    blit(double_buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
                }

                else if (game->is_over)
                {
                    // On affiche le gameover
                    blit(dump, double_buffer, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

                    masked_blit(board_over, double_buffer, 0, 0, (SCREEN_W - board_over->w) / 2, (SCREEN_HEIGHT - board_over->h) / 2, board_over->w, board_over->h);
                    masked_blit(hero_feu, double_buffer, (anim / 10) * 28, 0, (feu_x--), 100, 28, hero_feu->h);


                    textprintf_ex (
                        double_buffer, score_font,
                        100, (SCREEN_HEIGHT) / 2 + 30,
                        makecol(255,0,0), -1,
                        "Score = %d", game->point
                    );

                    blit(double_buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

                    anim++;
                    if ((anim / 10) * 28 >= hero_feu->w)
                        anim = 0;

                    if ((feu_x) < 0 - hero_feu->w)
                        feu_x = SCREEN_W;
                }
            }

        }


        // On marque une pause de 1ms
        // pour éviter que la boucle de jeu ne se répète trop vite
        Sleep(1);
    }

    return 0;

}   END_OF_MAIN();

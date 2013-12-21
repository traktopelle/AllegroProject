#ifndef File_H_INCLUDED
#define File_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <allegro.h>
#include <winalleg.h>
#include <windows.h>

#include "Object.h"
#include "Game.h"
#include "Entity.h"

int sauvegarder(Game *game);
int load (Game *game);

#endif

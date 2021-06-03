/*
Copyright (C) 2004 Parallel Realities

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "headers.h"

extern void addObstacle(char *name, int x, int y, char *spriteName);
extern void addItem(int itemType, char *name, int x, int y, char *shapeName, int health, int value, int flags, bool randomMovement);
extern void addMIA(char *name, int x, int y, int type);
extern void addTeleporter(char *name, int x, int y, int destX, int destY, bool active);
extern void addLineDef(char *name, char *linkName, char *message, int x, int y, int width, int height, bool active);
extern void addTrap(char *name, int trapType, int damage, int speed, int startX, int startY, int endX, int endY, int wait1, int wait2, char *sprite, bool active);
extern void loadSprite(char *token);
extern void loadEnemy(char *token);
extern Entity *getDefinedEnemy(char *name);
extern void addEnemy(char *name, int x, int y, int flags);
extern bool perfectlyCompleted();

extern Audio audio;
extern Engine engine;
extern Game game;
extern GameData gameData;
extern Graphics graphics;
extern Map map;

extern Entity player;

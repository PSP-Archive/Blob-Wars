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

extern void addItem(int itemType, char *name, int x, int y, char *spriteName, int health, int value, int flags, bool randomMovement);
extern void addTeleportParticles(float x, float y, int amount, int soundToPlay);
extern void addColorParticles(float x, float y, int amount, int color);
extern bool okayToSpawnEnemy(char *name, int x, int y);
extern void addEnemy(char *name, int x, int y, int flags);
extern void moveEntity(Entity *entity);
extern void addFireTrailParticle(float x, float y);

extern Entity *getEnemy(char *name);
extern Weapon *getRandomAimedWeapon();
extern Weapon *getRandomStraightWeapon();
extern Weapon *getRandomGaldovWeapon();

extern void tankBossMGInit();
extern void tankBossGLInit();
extern void aquaBossInit();
extern void droidBossInit();
extern void galdovInit();
extern void galdovFinalInit();

extern Audio audio;
extern Engine engine;
extern Game game;
extern Graphics graphics;
extern Map map;

extern Entity player;
extern Entity defItem[MAX_ITEMS];

// this is used exclusively by the bosses
Boss *self;

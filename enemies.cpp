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

#include "enemies.h"

Entity *getDefinedEnemy(char *name)
{
	for (int i = 0 ; i < MAX_ENEMIES ; i++)
	{
		if (strcmp(name, defEnemy[i].name) == 0)
		{
			return &defEnemy[i];
		}
	}

	DebuG(("No Such Enemy '%s'\n", name));

	return NULL;
}

Entity *getEnemy(char *name)
{
	Entity *enemy = (Entity*)map.enemyList.getHead();

	while (enemy->next != NULL)
	{
		enemy = (Entity*)enemy->next;

		if (strcmp(name, enemy->name) == 0)
		{
			return enemy;
		}
	}
	
	DebuG(("No Such Enemy '%s'\n", name));

	return NULL;
}

void addEnemy(char *name, int x, int y, int flags)
{
	Entity *enemy = new Entity();
	Entity *defEnemy = getDefinedEnemy(name);

	if (defEnemy == NULL)
	{
		DebuG(("ERROR : COULDN'T FIND ENEMY '%s'!\n", name));
		return;
	}

	enemy->setName(defEnemy->name);
	enemy->setSprites(defEnemy->sprite[0], defEnemy->sprite[1], defEnemy->sprite[2]);
	enemy->currentWeapon = defEnemy->currentWeapon;
	enemy->value = defEnemy->value;
	enemy->health = defEnemy->health;
	enemy->flags = defEnemy->flags;

	enemy->place(x, y);
	enemy->setVelocity(0, 0);
	enemy->baseThink = 60;

	enemy->flags += flags;
	
	enemy->reload = 120; // Wait about seconds seconds before attacking

	if (map.data[(int)(enemy->x) >> BRICKSHIFT][(int)(enemy->y) >> BRICKSHIFT] == MAP_WATER)
	{
		enemy->environment = ENV_WATER;
	}

	map.addEnemy(enemy);
}

bool hasClearShot(Entity *enemy)
{
	int mx, my;
	float x = enemy->x;
	float y = enemy->y;
	float dx, dy;

	Math::calculateSlope(player.x, player.y, enemy->x, enemy->y, &dx, &dy);

	if ((dx == 0) && (dy == 0))
		return true;

	while (true)
	{
		x += dx;
		y += dy;

		//graphics.blit(graphics.getSprite("AimedShot", true)->getCurrentFrame(), (int)(x - engine.playerPosX), (int)(y - engine.playerPosY), graphics.screen, true);

		mx = (int)(x) >> BRICKSHIFT;
		my = (int)(y) >> BRICKSHIFT;

		if ((mx < 0) || (my < 0))
			return false;

		if (map.isSolid(mx, my))
			return false;

		if (Collision::collision(x, y, 3, 3, (int)player.x, (int)player.y, player.height, player.width))
			break;
	}

	return true;
}

void lookForPlayer(Entity *enemy)
{
	// player is dead
	if (player.health <= -60)
		return;
	
	if (game.missionOverReason == MIS_COMPLETE)
		return;

	// can't fire anyway!
	if (enemy->reload > 0)
		return;

	int x = (int)fabs(enemy->x - player.x);
	int y = (int)fabs(enemy->y - player.y);

	// out of range
	if (x > 480)
		return;

	// can't even jump that high!
	if (y > 100)
		return;

	// Player is in range... go for them!
	if (enemy->flags & ENT_ALWAYSCHASE)
	{
		enemy->owner->tx = (int)(player.x);
		enemy->owner->ty = (int)(player.y);
	}
	else if ((rand() % (35 - game.skill)) == 0)
	{
		enemy->owner->tx = (int)(player.x);
		enemy->owner->ty = (int)(player.y);
	}

	// facing the wrong way
	if ((enemy->face == 0) && (player.x < enemy->x))
	{
		return;
	}

	// still facing the wrong way
	if ((enemy->face == 1) && (player.x > enemy->x))
	{
		return;
	}

	if (hasClearShot(enemy))
	{
		if (enemy->flags & ENT_ALWAYSFIRES)
		{
			addBullet(enemy, enemy->currentWeapon->getSpeed(enemy->face), 0);
			if (enemy->currentWeapon == &weapon[WP_ALIENSPREAD])
			{
				addBullet(enemy, enemy->currentWeapon->getSpeed(enemy->face), 2);
				addBullet(enemy, enemy->currentWeapon->getSpeed(enemy->face), -2);
			}
		}
		else if ((rand() % 850) <= (game.skill * 5))
		{
			addBullet(enemy, enemy->currentWeapon->getSpeed(enemy->face), 0);
			if (enemy->currentWeapon == &weapon[WP_ALIENSPREAD])
			{
				addBullet(enemy, enemy->currentWeapon->getSpeed(enemy->face), 2);
				addBullet(enemy, enemy->currentWeapon->getSpeed(enemy->face), -2);
			}
		}

		if (enemy->flags & ENT_RAPIDFIRE)
		{
			if (enemy->flags & ENT_ALWAYSFIRES)
			{
				if ((rand() % 25) > game.skill * 3)
					Math::removeBit(&enemy->flags, ENT_ALWAYSFIRES);
			}
			else
			{
				if ((rand() % 50) < game.skill * 2)
					Math::addBit(&enemy->flags, ENT_ALWAYSFIRES);
			}
		}
	}
	else
	{
		if (enemy->flags & ENT_RAPIDFIRE)
			Math::removeBit(&enemy->flags, ENT_ALWAYSFIRES);
	}

	if ((enemy->flags & ENT_FLIES) || (enemy->flags & ENT_SWIMS) || (enemy->flags & ENT_NOJUMP))
		return;
		
	if (enemy->flags & ENT_JUMPS)
	{
		if (!enemy->falling)
		{
			if ((rand() % 25) == 0)
			{
				int distance = Math::rrand(1, 4);
				enemy->setVelocity(distance - ((distance * 2) * enemy->face), Math::rrand(-12, -10));
			}
		}
		
		return;
	}

	// Jump to try and reach player (even if they are approximately level with you!)
	if (player.y - 5 < enemy->y)
	{
		if (!enemy->falling)
		{
			if ((rand() % 100) == 0)
			{
				enemy->dy = -12;
			}
		}
	}
}

void doAI(Entity *enemy)
{
	if (enemy->flags & ENT_GALDOV)
	{
		doGaldovAI(enemy);
	}
	else if (enemy->flags & ENT_BOSS)
	{
		return;
	}

	int x = (int)enemy->x;
	int y = (int)enemy->y + enemy->height;
	
	if (enemy->dx > 0)
		x += enemy->width;

	x = x >> BRICKSHIFT;
	y = y >> BRICKSHIFT;

	if (enemy->dx == 0)
		enemy->tx = (int)enemy->x;

	// Don't enter areas you're not supposed to
	if (enemy->tx != (int)enemy->x)
	{
		if (!(enemy->flags & ENT_FLIES))
		{
			//if ((map.data[x][y] == MAP_AIR) || (map.data[x][y] >= MAP_DECORATION))
			if (!map.isSolid(x, y))
			{
				enemy->tx = (int)enemy->x;
			}
		}
	}

	if ((int)enemy->x == enemy->tx)
	{
		if ((rand() % 100) == 0)
		{
			enemy->tx = (int)(enemy->x + Math::rrand(-640, 640));
			enemy->ty = (int)(enemy->y);
			if ((enemy->flags & ENT_FLIES) || (enemy->flags & ENT_SWIMS))
			{
				enemy->ty = (int)(enemy->y + Math::rrand(-320, 320));
			}
		}

		Math::limitInt(&enemy->tx, 15, (MAPWIDTH * BRICKSIZE)- 20);
		Math::limitInt(&enemy->ty, 15, (MAPHEIGHT * BRICKSIZE)- 20);

		if (map.isSolid((enemy->tx >> BRICKSHIFT), (enemy->ty >> BRICKSHIFT)))
		{
			enemy->tx = (int)enemy->x;
			enemy->ty = (int)enemy->y;
		}
	}

	// Don't enter areas you're not supposed to
	if (enemy->ty != (int)enemy->y)
	{
		if (enemy->flags & ENT_FLIES)
		{
			if (map.isLiquid(x, y + 1))
			{
				enemy->ty = (int)enemy->y;
			}
		}
	}

	if ((int)enemy->y == enemy->ty)
	{
		enemy->y = enemy->ty;
		enemy->dx = 0;
	}

	if (!enemy->falling)
		enemy->dx = 0;

	if ((enemy->flags & ENT_FLIES) || (enemy->flags & ENT_SWIMS))
	{
		enemy->dx = enemy->dy = 0;

		if ((int)enemy->y < enemy->ty) enemy->dy = 1;
		if ((int)enemy->y > enemy->ty) enemy->dy = -1;
	}

	if (enemy->x < enemy->tx) {enemy->dx = 1; enemy->face = 0;}
	if (enemy->x > enemy->tx) {enemy->dx = -1; enemy->face = 1;}

	if ((enemy->flags & ENT_SWIMS) && (enemy->environment == ENV_WATER))
	{
		enemy->dy = 0;

		if ((int)enemy->y < enemy->ty) enemy->dy = 1;
		if ((int)enemy->y > enemy->ty) enemy->dy = -1;
	}

	lookForPlayer(enemy);
}

void enemyBulletCollisions(Entity *bullet)
{
	if (bullet->health < 1)
	{
		return;
	}

	Entity *enemy = (Entity*)map.enemyList.getHead();

	while (enemy->next != NULL)
	{
		enemy = (Entity*)enemy->next;

		if ((enemy->flags & ENT_TELEPORTING) || (enemy->dead == DEAD_DYING))
		{
			continue;
		}

		char comboString[100];

		if ((bullet->owner == &player) || (bullet->owner == &engine.world) || (bullet->flags & ENT_BOSS))
		{
			sprintf(comboString, "Combo-%s", bullet->name);

			if (Collision::collision(enemy, bullet))
			{
				if (bullet->id != WP_LASER)
				{
					bullet->health = 0;
				}
				
				Math::removeBit(&bullet->flags, ENT_SPARKS);
				Math::removeBit(&bullet->flags, ENT_PUFFS);

				if ((enemy->flags & ENT_IMMUNE) && (!(enemy->flags & ENT_STATIC)))
				{
					bullet->health = 0; // include the Laser for this one!
					enemy->owner->tx = (int)bullet->owner->x;
					enemy->owner->ty = (int)bullet->owner->y;
					if (enemy->x < enemy->tx) {enemy->owner->dx = 1; enemy->owner->face = 0;}
					if (enemy->x > enemy->tx) {enemy->owner->dx = -1; enemy->owner->face = 1;}
					return;
				}

				/*
					Increment the bullet hits counter. The laser can only do this
				 	if the target has more than 0 health. Overwise the stats screen
				 	can show an accurracy of 800%. Which is just plain silly.
				*/
				if (bullet->owner == &player)
				{
					enemy->tx = (int)player.x;
					enemy->ty = (int)player.y;
					enemy->face = 0;
					
					if (player.x < enemy->x)
					{
						enemy->face = 1;
					}

					if ((bullet->id != WP_LASER) || (enemy->health > 0))
					{
						game.incBulletsHit();
					}
				}

				if (!(enemy->flags & ENT_EXPLODES))
				{
					audio.playSound(SND_HIT, CH_ANY);
					if (game.gore)
					{
						addBlood(enemy, bullet->dx / 4, Math::rrand(-6, -3), 1);
					}
					else
					{
						addColorParticles(bullet->x, bullet->y, Math::rrand(25, 75), -1);
					}
				}
				else
				{
					audio.playSound(SND_CLANG, CH_ANY);
					addColorParticles(bullet->x, bullet->y, Math::rrand(25, 75), -1);
				}

				if (enemy->health > 0)
				{
					if (!(enemy->flags & ENT_IMMUNE))
					{
						enemy->health -= bullet->damage;
					}
					
					if (enemy->health <= 0)
					{	
						if (bullet->owner == &player)
						{
							game.score += enemy->value;
							game.currentMissionEnemiesDefeated++;
	
							if (player.currentWeapon != &weapon[WP_LASER])
							{
								game.doCombo();
							}
							
							checkObjectives(comboString, false);
							checkObjectives("Enemy", false);
							checkObjectives(enemy->name, false);
						}
	
						if (!(enemy->flags & ENT_STATIC))
						{
							enemy->dx = (bullet->dx / 4);
							enemy->dy = -10;
							
							if (enemy->flags & ENT_EXPLODES)
							{
								audio.playSound(SND_ELECDEATH1 + rand() % 3, CH_DEATH);
							}
							else if (game.gore)
							{
								audio.playSound(SND_DEATH1 + rand() % 3, CH_DEATH);
							}
						}
					}
					
				}
				
				if (enemy->flags & ENT_STATIC)
				{
					return;
				}

				if (enemy->health < 0)
				{
					enemy->dx = Math::rrand(-3, 3);
					enemy->dy = 5 - rand() % 15;
					enemy->health = -1;
					
					if (enemy->flags & ENT_EXPLODES)
					{
						audio.playSound(SND_ELECDEATH1 + rand() % 3, CH_DEATH);
					}
					else if (game.gore)
					{
						audio.playSound(SND_DEATH1 + rand() % 3, CH_DEATH);
					}

					if (bullet->owner == &player)
					{
						if (player.currentWeapon != &weapon[WP_LASER])
						{
							game.doCombo();
							checkObjectives(comboString, false);
						}
					}
				}

				if (game.currentComboHits >= 3)
				{
					char message[50];
					sprintf(message, "%d Hit Combo!", game.currentComboHits);
					engine.setInfoMessage(message, 0, INFO_NORMAL);
				}

				return;
			}
		}
	}
}

int getNonGoreParticleColor(char *name)
{
	int rtn = graphics.yellow;
	
	if (strcmp(name, "Pistol Blob") == 0)
	{
		rtn = graphics.green;
	}
	else if (strcmp(name, "Grenade Blob") == 0)
	{
		rtn = graphics.skyBlue;
	}
	else if (strcmp(name, "Aqua Blob") == 0)
	{
		rtn = graphics.cyan;
	}
	else if (strcmp(name, "Laser Blob") == 0)
	{
		rtn = SDL_MapRGB(graphics.screen->format, 255, 0, 255);
	}
	else if (strcmp(name, "Machine Gun Blob") == 0)
	{
		rtn = SDL_MapRGB(graphics.screen->format, 200, 64, 24);
	}
	
	return rtn;
}

void gibEnemy(Entity *enemy)
{
	if (enemy->flags & ENT_GALDOV)
	{
		addTeleportParticles(enemy->x, enemy->y, 75, SND_TELEPORT3);
		checkObjectives("Galdov", true);
	}

	if (enemy->flags & ENT_EXPLODES)
	{
		addExplosion(enemy->x + (enemy->width / 2), enemy->y + (enemy->height / 2), 10 + (20 * game.skill), enemy);
		addSmokeAndFire(enemy, Math::rrand(-5, 5), Math::rrand(-5, 5), 2);
		return;
	}

	float x, y, dx, dy;
	int amount = (game.gore) ? 25 : 150;
	int color = getNonGoreParticleColor(enemy->name);

	for (int i = 0 ; i < amount ; i++)
	{
		x = enemy->x + Math::rrand(-3, 3);
		y = enemy->y + Math::rrand(-3, 3);
		
		if (game.gore)
		{
			dx = Math::rrand(-5, 5);
			dy = Math::rrand(-15, -5);
			addEffect(x, y, dx, dy, EFF_BLEEDS);
		}
		else
		{
			dx = Math::rrand(-5, 5);
			dy = Math::rrand(-5, 5);
			addColoredEffect(x, y, dx, dy, color, EFF_COLORED + EFF_WEIGHTLESS);
		}
	}
	
	(game.gore) ? audio.playSound(SND_SPLAT, CH_ANY) : audio.playSound(SND_POP, CH_ANY);
}

void doEnemies()
{
	Entity *enemy = (Entity*)map.enemyList.getHead();
	Entity *previous = enemy;
	
	map.fightingGaldov = false;

	int x, y, absX, absY;

	while (enemy->next != NULL)
	{
		enemy = (Entity*)enemy->next;
		
		if (!engine.cheatBlood)
		{
			if (enemy->dead == DEAD_DYING)
			{
				if (!enemy->referenced)
				{
					DebuG(("Removing unreferenced enemy '%s'\n", enemy->name));
					map.enemyList.remove(previous, enemy);
					enemy = previous;
				}
				else
				{
					previous = enemy;
				}
				
				enemy->referenced = false;
				continue;
			}
		}

		x = (int)(enemy->x - engine.playerPosX);
		y = (int)(enemy->y - engine.playerPosY);

		absX = abs(x);
		absY = abs(y);

		if ((absX < 800) && (absY < 600))
		{
			// Fly forever
			if (enemy->flags & ENT_FLIES)
			{
				enemy->fuel = 7;
			}

			if (enemy->owner->flags & ENT_TELEPORTING)
			{
				moveEntity(enemy);
			}
			else
			{
				if ((enemy->health > 0) && (!(enemy->flags & ENT_STATIC)))
				{
					enemy->think();

					if (enemy->owner == enemy)
					{
						doAI(enemy);
					}
					else
					{
						lookForPlayer(enemy);
					}
				}
				
				if (map.isBlizzardLevel)
				{
					enemy->dx += map.windPower * 0.1;
				}

				if (enemy->flags & ENT_NOMOVE)
				{
					enemy->dx = 0;
				}

				moveEntity(enemy);

				if ((absX < 700) && (absY < 500))
				{
					if (enemy->flags & ENT_FIRETRAIL)
					{
						addFireTrailParticle(enemy->x + (enemy->face * 16) + Math::rrand(-1, 1), enemy->y + Math::rrand(-1, 1));
					}
					
					graphics.blit(enemy->getFaceImage(), x, y, graphics.screen, false);
					
					if ((enemy->dx != 0) || (enemy->flags & ENT_FLIES) || (enemy->flags & ENT_STATIC))
					{
						enemy->animate();
					}
				}
			}
		}
		else
		{
			if (enemy->flags & ENT_SPAWNED)
			{
				if ((absX > 1920) || (absY > 1440))
				{
					enemy->health = -100;
				}
			}
		}

		if (enemy->health > 0)
		{
			previous = enemy;
			
			if ((enemy->environment == ENV_SLIME) || (enemy->environment == ENV_LAVA))
			{
				checkObjectives(enemy->name, false);
				enemy->health = -1;
			}
		}
		else
		{
			if (enemy->flags & ENT_GALDOV)
			{
				enemy->health = -99;
			}

			if (enemy->health == 0)
			{
				Math::removeBit(&enemy->flags, ENT_WEIGHTLESS);
				Math::removeBit(&enemy->flags, ENT_SWIMS);
				Math::removeBit(&enemy->flags, ENT_FLIES);
				Math::addBit(&enemy->flags, ENT_INANIMATE);
				Math::addBit(&enemy->flags, ENT_BOUNCES);
				enemy->health = -1 - rand() % 25;
			}
			
			if (engine.cheatBlood)
			{
				if (!(enemy->flags & ENT_EXPLODES))
				{
					if ((enemy->health % 4) == 0)
					{
						addBlood(enemy, Math::rrand(-2, 2), Math::rrand(-6, -3), 1);
					}
					else if ((enemy->health % 10) == 0)
					{
						if (game.gore)
						{
							audio.playSound(SND_DEATH1 + rand() % 3, CH_DEATH);
						}
					}
				}
			}

			enemy->health--;

			if (enemy->flags & ENT_MULTIEXPLODE)
			{
				if (enemy->health < -30)
				{
					if ((enemy->health % 3) == 0)
					{
						addExplosion(enemy->x + rand() % 25, enemy->y + rand() % 25,  10 + (20 * game.skill), enemy);
						addSmokeAndFire(enemy, Math::rrand(-5, 5), Math::rrand(-5, 5), 2);
					}
				}
			}

			if (enemy->health > -50)
			{
				previous = enemy;
			}
			else
			{
				if (enemy->flags & ENT_GALDOVFINAL)
				{
					enemy->health = -30;
					enemy->dx = Math::rrand(-10, 10);
					enemy->dy = Math::rrand(-10, 10);
				}
				else
				{
					if (enemy->dead == DEAD_ALIVE)
					{
						if ((absX < 800) && (absY < 600))
						{
							gibEnemy(enemy);
							
							if (enemy->value)
							{
								dropRandomItems((int)enemy->x, (int)enemy->y);
							}
						}
						
						enemy->dead = DEAD_DYING;
					}
					
					if (enemy->dead == DEAD_DYING)
					{
						if (!enemy->referenced)
						{
							if ((absX < 800) && (absY < 600))
							{
								gibEnemy(enemy);
								
								if (enemy->value)
								{
									dropRandomItems((int)enemy->x, (int)enemy->y);
								}
							}
							
							DebuG(("Removing unreferenced enemy '%s'\n", enemy->name));
							map.enemyList.remove(previous, enemy);
							enemy = previous;
						}
					}
				}
			}
		}
		
		// default the enemy to not referenced.
		// doBullets() will change this if required.
		enemy->referenced = false;
	}
}

void loadEnemy(char *token)
{
	int enemy = -1;

	for (int i = MAX_ENEMIES ; i > -1 ; i--)
		if (strcmp(defEnemy[i].name, "") == 0)
			enemy = i;

	if (enemy == -1)
	{
		printf("Out of enemy define space!\n");
		exit(1);
	}

	char name[50], sprite[3][100], weapon[100], flags[1024];
	int health, value;

	sscanf(token, "%*c %[^\"] %*c %s %s %s %*c %[^\"] %*c %d %d %s", name, sprite[0], sprite[1], sprite[2], weapon, &health, &value, flags);

	defEnemy[enemy].setName(name);
	defEnemy[enemy].setSprites(graphics.getSprite(sprite[0], true), graphics.getSprite(sprite[1], true), graphics.getSprite(sprite[2], true));
	defEnemy[enemy].currentWeapon = getWeaponByName(weapon);
	defEnemy[enemy].health = health;
	defEnemy[enemy].value = value;

	defEnemy[enemy].flags = engine.getValueOfFlagTokens(flags);
}

void loadDefEnemies()
{
	for (int i = 0 ; i < MAX_ENEMIES ; i++)
	{
		strcpy(defEnemy[i].name, "");
	}

	int enemy = 0;

	if (!engine.loadData("data/defEnemies"))
	{
		graphics.showErrorAndExit("Couldn't load enemy definitions file (%s)", "data/defEnemies");
	}

	char *token = strtok((char*)engine.dataBuffer, "\n");

	char name[50], sprite[3][100], weapon[100], flags[1024];
	int health, value;

	while (true)
	{
		if (strcmp(token, "@EOF@") == 0)
		{
			break;
		}

		sscanf(token, "%*c %[^\"] %*c %s %s %s %*c %[^\"] %*c %d %d %s", name, sprite[0], sprite[1], sprite[2], weapon, &health, &value, flags);

		defEnemy[enemy].setName(name);
		defEnemy[enemy].setSprites(graphics.getSprite(sprite[0], true), graphics.getSprite(sprite[1], true), graphics.getSprite(sprite[2], true));
		defEnemy[enemy].currentWeapon = getWeaponByName(weapon);
		defEnemy[enemy].health = health;
		defEnemy[enemy].value = value;
		defEnemy[enemy].flags = engine.getValueOfFlagTokens(flags);

		enemy++;

		token = strtok(NULL, "\n");
	}
}

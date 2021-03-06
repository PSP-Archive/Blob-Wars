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

#include "player.h"

void resetPlayer()
{
	game.getCheckPoint(&player.x, &player.y);
	
	player.dx = 0;
	player.dy = 0;
	player.immune = 120;
	player.environment = ENV_AIR;
	player.oxygen = 7;
	addTeleportParticles(player.x + 10, player.y + 10, 50, SND_TELEPORT2);
	
	Math::removeBit(&player.flags, ENT_FLIES);
	Math::removeBit(&player.flags, ENT_TELEPORTING);
	player.setSprites(graphics.getSprite("BobRight", true), graphics.getSprite("BobLeft", true), graphics.getSprite("BobSpin", true));
	
	map.windPower = 0;
	map.windChangeTime = 90;
}

void gibPlayer()
{
	float x, y, dx, dy;
	int amount = (game.gore) ? 25 : 150;

	for (int i = 0 ; i < amount ; i++)
	{
		x = player.x + Math::rrand(-3, 3);
		y = player.y + Math::rrand(-3, 3);
		
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
			addColoredEffect(x, y, dx, dy, graphics.yellow, EFF_COLORED + EFF_WEIGHTLESS);
		}
	}

	(game.gore) ? audio.playSound(SND_SPLAT, CH_ANY) : audio.playSound(SND_POP, CH_ANY);
}

void checkPlayerBulletCollisions(Entity *bullet)
{
	if ((bullet->health < 1) || (player.health <= -60))
	{
		return;
	}

	if ((player.flags & ENT_TELEPORTING) || (game.missionOver > 0))
	{
		return;
	}

	if (bullet->owner != &player)
	{
		if (Collision::collision(&player, bullet))
		{
			if ((!player.immune) && (!(bullet->flags & ENT_EXPLODES)))
			{
				addBlood(&player, bullet->dx / 4, Math::rrand(-6, -3), 1);
				audio.playSound(SND_HIT, CH_ANY);
				if (game.missionOverReason == MIS_INPROGRESS)
				{
					player.health -= bullet->damage;
					player.immune = 120;
				}
			}

			Math::removeBit(&bullet->flags, ENT_SPARKS);
			Math::removeBit(&bullet->flags, ENT_PUFFS);

			if (player.health <= 0)
			{
				player.dx = bullet->dx;
				player.dy = -5;
				audio.playSound(SND_DEATH1 + rand() % 3, CH_DEATH);
				player.health = 0;
			}

			if (bullet->id == WP_LASER)
			{
				DebuG(("Laser Hit Player"));
				throwAndDamageEntity(&player, 0, -3, 3, -8);
			}
			else
			{
				bullet->health = 0;
			}
		}
	}
}

void doPlayer()
{
	if (engine.cheatHealth)
	{
		player.health = MAX_HEALTH;
	}

	if (game.missionOverReason == MIS_PLAYERESCAPE)
	{
		return;
	}

	if (player.y > (map.limitDown + 500))
	{
		if (game.missionOver == 0)
		{
			player.health--;
		}

		if (player.health > 0)
		{
			game.setMissionOver(MIS_PLAYEROUT);
		}
		else
		{
			game.setMissionOver(MIS_PLAYERDEAD);
		}
	}
	
	if (player.flags & ENT_TELEPORTING)
	{
		moveEntity(&player);
		return;
	}

	if (game.missionOverReason == MIS_PLAYEROUT)
	{
		graphics.blit(player.getFaceImage(), (int)(player.x - engine.playerPosX), (int)(player.y - engine.playerPosY), graphics.screen, false);
		moveEntity(&player);
		return;
	}

	if ((player.health < 1) || (player.immune > 120))
	{
		if (player.health <= -60)
		{
			return;
		}

		player.think();
		
		if (game.hasAquaLung)
		{
			player.oxygen = 7;
		}

		moveEntity(&player);

		graphics.blit(player.getFaceImage(), (int)(player.x - engine.playerPosX), (int)(player.y - engine.playerPosY), graphics.screen, false);

		if (player.health < 1)
		{
			player.health--;
		}

		if (player.health <= -60)
		{
			gibPlayer();
		}

		return;
	}

	if ((!(player.flags & ENT_FLIES)) && (!map.isIceLevel))
	{
		player.dx = 0;
	}
	
	// toggles the Jetpack if available
	if (engine.isKey(KEY_JETPACK))
	{
		if ((game.hasJetPack) || (engine.cheatExtras))
		{
			if (player.flags & ENT_FLIES)
			{
				Math::removeBit(&player.flags, ENT_FLIES);
				player.setSprites(graphics.getSprite("BobRight", true), graphics.getSprite("BobLeft", true), graphics.getSprite("BobSpin", true));
			}
			else
			{
				if ((player.fuel > 3) && (player.environment == ENV_AIR))
				{
					Math::addBit(&player.flags, ENT_FLIES);
					player.setSprites(graphics.getSprite("JPBobRight", true), graphics.getSprite("JPBobLeft", true), graphics.getSprite("BobSpin", true));
				}
				else if ((player.environment == ENV_WATER))
				{
					engine.setInfoMessage("Jetpack cannot be used underwater", 0, INFO_NORMAL);
				}
				else
				{
					engine.setInfoMessage("Jetpack is recharging...", 0, INFO_NORMAL);
				}
			}
		}
		else
		{
			engine.setInfoMessage("Don't have jetpack!", 0, INFO_NORMAL);
		}
		
		engine.clearKey(KEY_JETPACK);
	}
	
	if (map.isBlizzardLevel)
	{
		if ((!engine.isKey(KEY_LEFT)) && (!engine.isKey(KEY_RIGHT)))
		{
			if ((player.dx > -2) && (player.dx < 2))
			{
				player.dx += map.windPower * 0.1;
			}
		}
	}

	if (engine.isKey(KEY_LEFT) || engine.getJoyLeft())
	{
		player.face = 1;
		
		if (player.flags & ENT_FLIES)
		{
			player.dx -= 0.1;
			Math::limitFloat(&player.dx, -PLAYER_FLY_SPEED, PLAYER_FLY_SPEED);
		}
		else if (map.isIceLevel)
		{
			player.dx -= 0.1;
			Math::limitFloat(&player.dx, -PLAYER_WALK_SPEED * 1.25, PLAYER_WALK_SPEED * 1.25);
			player.animate();
		}
		else
		{
			player.dx = -PLAYER_WALK_SPEED;
			player.animate();
		}

		if (engine.cheatSpeed)
		{
			player.dx *= 3;
		}
	}
	else if (engine.isKey(KEY_RIGHT) || engine.getJoyRight())
	{
		player.face = 0;

		if (player.flags & ENT_FLIES)
		{
			player.dx += 0.1;
			Math::limitFloat(&player.dx, -PLAYER_FLY_SPEED, PLAYER_FLY_SPEED);
		}
		else if (map.isIceLevel)
		{
			player.dx += 0.1;
			player.animate();
			Math::limitFloat(&player.dx, -PLAYER_WALK_SPEED * 1.25, PLAYER_WALK_SPEED * 1.25);
		}
		else
		{
			player.dx = PLAYER_WALK_SPEED;
			player.animate();
		}
		
		if (engine.cheatSpeed)
		{
			player.dx *= 3;
		}
	}
	else if (!(player.flags & ENT_FLIES))
	{
		// usual idle frame
		player.currentFrame = 0;
	}

	Math::limitFloat(&player.x, 10, map.limitRight + 608);
	Math::limitFloat(&player.y, map.limitUp + 5, (MAPHEIGHT * BRICKSIZE) + 64);

	if ((engine.isKey(KEY_UP)) || engine.getJoyUp())
	{
		if (player.flags & ENT_FLIES)
		{
			player.dy -= 0.1;
			Math::limitFloat(&player.dy, -PLAYER_FLY_SPEED, PLAYER_FLY_SPEED);
			if (engine.cheatSpeed)
			{
				player.dy *= 2;
			}
		}
		else if ((player.environment == ENV_AIR) && (player.falling == false))
		{
			player.falling = true;
			player.dy = PLAYER_JUMP_SPEED;
			engine.clearKey(KEY_UP);
		}
		else if (player.environment != ENV_AIR)
		{
			player.dy = -PLAYER_WALK_SPEED;
			player.animate();
			if (engine.cheatSpeed)
			{
				player.dy *= 3;
			}
		}
	}

	if (engine.isKey(KEY_DOWN) || engine.getJoyDown())
	{
		if (player.flags & ENT_FLIES)
		{
			player.dy += 0.1;
			Math::limitFloat(&player.dy, -PLAYER_FLY_SPEED, PLAYER_FLY_SPEED);
		}
		else if (player.environment != ENV_AIR)
		{
			player.dy = 2;
			player.animate();
		}

		if (engine.cheatSpeed)
		{
			player.dy *= 3;
		}
	}
	
	#if !USEPAK
	if (engine.keyState[SDLK_1])
		player.currentWeapon = &weapon[WP_PISTOL];
	else if (engine.keyState[SDLK_2])
		player.currentWeapon = &weapon[WP_MACHINEGUN];
	else if (engine.keyState[SDLK_3])
		player.currentWeapon = &weapon[WP_GRENADES];
	else if (engine.keyState[SDLK_4])
		player.currentWeapon = &weapon[WP_LASER];
	else if (engine.keyState[SDLK_5])
		player.currentWeapon = &weapon[WP_SPREAD];
	#endif

	moveEntity(&player);

	if (engine.isKey(KEY_FIRE))
	{
		if (player.reload == 0)
		{
			addBullet(&player, player.currentWeapon->getSpeed(player.face), 0);
			if (player.currentWeapon == &weapon[WP_SPREAD])
			{
				addBullet(&player, player.currentWeapon->getSpeed(player.face), 2);
				addBullet(&player, player.currentWeapon->getSpeed(player.face), -2);
			}
		}
	}
	
	if (player.environment == ENV_WATER)
	{
		Math::removeBit(&player.flags, ENT_FLIES);

		addBubble(player.x, player.y);
		
		if ((player.thinktime == 30) && (player.oxygen == 0))
		{
			audio.playSound(SND_DROWNING, CH_ANY);
		}
	}

	player.think();
	
	if (player.fuel == 0)
	{
		player.setSprites(graphics.getSprite("BobRight", true), graphics.getSprite("BobLeft", true), graphics.getSprite("BobSpin", true));
	}
	
	if ((game.hasAquaLung) || (engine.cheatExtras))
	{
		player.oxygen = 7;
	}
		
	if (engine.cheatFuel)
	{
		player.fuel = 7;
	}

	if (((player.immune % 5) == 0) && (!(player.flags & ENT_TELEPORTING)))
	{
		if ((game.missionOverReason == MIS_COMPLETE) || (game.missionOverReason == MIS_INPROGRESS) || (game.missionOverReason == MIS_GAMECOMPLETE))
		{
			if (player.flags & ENT_FLIES)
			{
				player.animate();
				addFireTrailParticle(player.x + (player.face * 16) + Math::rrand(-1, 1), player.y + 10 + Math::rrand(-1, 1));
			}
			graphics.blit(player.getFaceImage(), (int)(player.x - engine.playerPosX), (int)(player.y - engine.playerPosY), graphics.screen, false);
		}
	}
}

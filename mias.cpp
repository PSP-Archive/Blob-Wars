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

#include "mias.h"

void initMIAPhrases()
{
	strcpy(mia_scared[0], "help me...");
	strcpy(mia_scared[1], "i don't wanna die...");
	strcpy(mia_scared[2], "please... someone help...");
	strcpy(mia_scared[3], "i... i'm scared...");
	strcpy(mia_scared[4], "i wanna go home...");
	strcpy(mia_scared[5], "what was that?!");
	strcpy(mia_scared[6], "i don't like it here...");
}

void addMIA(char *name, int x, int y, int type)
{
	Entity *mia = new Entity();

	strcpy(mia->name, name);
	mia->id = type;
	mia->baseThink = 60;
	mia->health = 180;
	mia->place(x, y);
	mia->value = Math::rrand(0, 5);
	mia->flags = ENT_INANIMATE; // MIAs don't drown

	switch (mia->id)
	{
		case MIA_NORMAL:
			mia->setSprites(graphics.getSprite("ScaredMIA", true), graphics.getSprite("ScaredMIA", true), graphics.getSprite("ScaredMIA", true));
			break;
		case MIA_AQUA:
			mia->setSprites(graphics.getSprite("AquaMIA", true), graphics.getSprite("AquaMIA", true), graphics.getSprite("AquaMIA", true));
			break;
	}

	map.addMIA(mia);
}

void doMIAs()
{
	Entity *mia = (Entity*)map.miaList.getHead();

	int x, y;

	char message[100];

	while (mia->next != NULL)
	{
		mia = (Entity*)mia->next;

		if (mia->health <= 0)
			continue;

		mia->think();

		if (mia->flags & ENT_TELEPORTING)
		{
			moveEntity(mia);
		}
		else
		{
			x = (int)(mia->x - engine.playerPosX);
			y = (int)(mia->y - engine.playerPosY);

			if ((abs(x) <= 1024) && (abs(y) <= 768))
			{
				moveEntity(mia);

				if (mia->value < 100)
				{
					if ((rand() % 250) == 0)
						mia->value = 100;
				}
				else
				{
					if ((rand() % 250) == 0)
						mia->value = Math::rrand(0, 6);
				}

				if ((mia->value != 100) && (!(mia->flags & ENT_DYING)))
				{
					graphics.setFontColor(0xff, 0xff, 0xff, 0x00, 0x00, 0x00);
					graphics.drawString((char*)mia_scared[mia->value], x + 10, y - 10, true, graphics.screen);
				}

				graphics.blit(mia->getFaceImage(), x, y, graphics.screen, false);
				mia->animate();

			}

			if ((Collision::collision(&player, mia)) && (player.health > 0) && (!(player.flags & ENT_TELEPORTING)))
			{
				if (!(mia->flags & ENT_DYING))
				{
					Math::addBit(&mia->flags, ENT_WEIGHTLESS);
					Math::addBit(&mia->flags, ENT_DYING);
					audio.playSound(SND_TELEPORT1, CH_ANY);
				}
			}
			
			if ((mia->id == MIA_NORMAL) && (mia->environment == ENV_WATER))
			{
				mia->id = MIA_AQUA;
				mia->setSprites(graphics.getSprite("AquaMIA", true), graphics.getSprite("AquaMIA", true), graphics.getSprite("AquaMIA", true));
				DebuG(("MIA '%s' fell into water. Became Aqua Mia\n", mia->name));
			}

			if (mia->flags & ENT_DYING)
			{
				for (int i = 0 ; i < 2 ; i++)
					map.addParticle(mia->x + Math::rrand(-2, 15), mia->y + rand() % mia->height, 0, Math::rrand(-5, -1), Math::rrand(30, 60), graphics.red, graphics.getSprite("TeleportStar", true), PAR_WEIGHTLESS);

				if (mia->health <= 100)
					mia->y -= 5;

				if (mia->health <= 0)
				{
					map.foundMIAs++;
					game.totalMIAsRescued++;

					if ((map.foundMIAs == (map.requiredMIAs / 2)) || (game.skill == 0))
					{
						sprintf(message, "Rescued %s - Checkpoint Reached!", mia->name);
						game.setObjectiveCheckPoint();
					}
					else
					{
						sprintf(message, "Rescued %s!", mia->name);
					}

					if (map.foundMIAs == map.requiredMIAs)
					{
						sprintf(message, "Rescue %d MIAs - Objective Complete - Checkpoint Reached!", map.requiredMIAs);
						game.setObjectiveCheckPoint();
					}

					engine.setInfoMessage(message, 1, INFO_OBJECTIVE);
				}
			}
		}
	}
}

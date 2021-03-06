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

#include "graphics.h"

void showAllSprites()
{
	loadResources();

	Sprite *sprite = graphics.getSpriteHead();

	int x, y, h;

	unsigned int frameLimit = SDL_GetTicks() + 16;

	while (true)
	{
		x = y = h = 0;

		graphics.updateScreen();
		graphics.animateSprites();
		engine.getInput();

		SDL_FillRect(graphics.screen, NULL, graphics.black);

		sprite = graphics.getSpriteHead();

		while (sprite->next != NULL)
		{
			sprite = (Sprite*)sprite->next;

			if (strcmp(sprite->name, "optionsBackground"))
			{
				graphics.blit(sprite->getCurrentFrame(), x, y, graphics.screen, false);

				x += sprite->image[0]->w + 5;
				h = max(h, sprite->image[0]->h + 5);

				if (x >= 600)
				{
     				x = 0;
					y += h;
					h = 0;
				}
			}
		}
		
		while (SDL_GetTicks() < frameLimit){}
		frameLimit = SDL_GetTicks() + 16;

	}
}

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

#include "main.h"
#include <psppower.h>
#include <pspkernel.h>


PSP_HEAP_SIZE_KB(19000);

/* Exit callback */
int exit_callback(int arg1, int arg2, void *common)
{
        sceKernelExitGame();
        return 0;
}

/* Callback thread */
int CallbackThread(SceSize args, void *argp)
{
        int cbid;

        cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
        sceKernelRegisterExitCallback(cbid);

        sceKernelSleepThreadCB();

        return 0;
}


/* Sets up the callback thread and returns its thread id */
int SetupCallbacks(void)
{
        int thid = 0;

        thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
        if(thid >= 0)
        {
                sceKernelStartThread(thid, 0, 0);
        }

        return thid;
}


#ifndef PSP
int main( int argc, char *argv[] )
#else
extern "C" int SDL_main( int argc, char *argv[] )
#endif
{
	scePowerSetClockFrequency(333,333,166);
	 SetupCallbacks();
	#if !USEPAK
	DebuG(("Not Using PAK...\n"));
	#endif

	atexit(cleanup);
	engine.startup();

	int requiredSection = SECTION_INTRO;
	bool showSprites = false;
	bool hub = false;
	for (int i = 1 ; i < argc ; i++)
	{
		if (strcmp(argv[i], "-fullscreen") == 0) engine.fullScreen = true;
		else if (strcmp(argv[i], "-noaudio") == 0) engine.useAudio = 0;
		else if (strcmp(argv[i], "-mono") == 0) engine.useAudio = 1;
		
		#if !USEPAK
		else if (strcmp(argv[i], "-map") == 0) {game.setMapName(argv[++i]); requiredSection = SECTION_GAME;}
		else if (strcmp(argv[i], "-skill") == 0) game.skill = atoi(argv[++i]);
		else if (strcmp(argv[i], "-showsprites") == 0) showSprites = true;
		else if (strcmp(argv[i], "-hub") == 0) hub = true;
		else if (strcmp(argv[i], "-randomscreens") == 0) graphics.takeRandomScreenShots = true;
		else if (strcmp(argv[i], "-nomonsters") == 0) engine.devNoMonsters = true;
		else if (strcmp(argv[i], "-credits") == 0) requiredSection = SECTION_CREDITS;
		#endif
	}


	initSystem();

	player.setName("Player");

	engine.flushInput();
	
	engine.allowQuit = true;

	if (hub)
	{
		requiredSection = SECTION_HUB;
		loadGame(0);
		loadResources();
	}
	
	if (game.skill == 3)
	{
		game.hasAquaLung = game.hasJetPack = true;
	}

	if (showSprites)
	{
		showAllSprites();
	}

	while (true)
	{
		switch (requiredSection)
		{
			case SECTION_INTRO:
				requiredSection = doIntro();
				break;

			case SECTION_TITLE:
				requiredSection = title();
				break;

			case SECTION_HUB:
				requiredSection = doHub();
				break;

			case SECTION_GAME:
				if (!game.continueFromCheckPoint)
				{
					checkStartCutscene();
					loadResources();
				}
				requiredSection = doGame();
				break;

			case SECTION_GAMEOVER:
				requiredSection = gameover();
				break;
				
			case SECTION_EASYOVER:
				map.clear();
				game.clear();
				easyGameFinished();
				requiredSection = SECTION_TITLE;
				break;
				
			case SECTION_CREDITS:
				doCredits();
				requiredSection = SECTION_TITLE;
				break;
		}
	}
	return 0;
}

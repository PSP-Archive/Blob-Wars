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

#include "init.h"

void checkForLicense()
{
	if (!engine.loadData("data/LICENSE"))
	{
		graphics.showLicenseErrorAndExit();
	}
}

/*
Show the GNU Public License the first time the game is played. Waits 4 seconds
and then proceeds. THIS MUST NOT BE REMOVED!!!!!
*/
void showLicense()
{
	SDL_FillRect(graphics.screen, NULL, graphics.black);
	graphics.delay(1000);

	SDL_FillRect(graphics.screen, NULL, graphics.black);
	SDL_Surface *pic = graphics.loadImage("gfx/main/licensePic.png");
	graphics.blit(pic, 0, 0, graphics.screen, false);
	SDL_FreeSurface(pic);

	checkForLicense();

	char line[255];
	int y = 0;

	char *token = strtok((char*)engine.dataBuffer, "\n");

	while (true)
	{
		sscanf(token, "%d %[^\n\r]", &y, line);
		
		if (y == -1)
		{
			break;
		}

		graphics.drawString(line, 320, y, true, graphics.screen);

		token = strtok(NULL, "\n");

		if (token == NULL)
		{
			break;
		}
	}

	graphics.delay(4000);

	graphics.drawString("Press Space to Continue...", 320, 440, true, graphics.screen);

	engine.flushInput();
	engine.clearInput();

	while (true)
	{
		graphics.updateScreen();
		engine.getInput();
		if (engine.keyState[SDLK_SPACE])
			break;
	}
	
	SDL_FillRect(graphics.screen, NULL, graphics.black);
	graphics.delay(1000);
}

/*
This bit is just for Linux and Unix users. It attempts to get the user's
home directory, then creates the .parallelrealities and .parallelrealities/blobwars
directories so that saves and temporary data files can be written there. Good, eh? :)
*/
#if UNIX
void setupUserHomeDirectory()
{
	char *userHome;

	char *name = getlogin();

	passwd *pass;

	if (name != NULL)
	{
		pass = getpwnam(name);
	}
	else
	{
		pass = getpwuid(geteuid());
	}

	if (pass == NULL)
	{
		printf("Couldn't determine the user home directory. Exitting.\n");
		exit(1);
	}

	userHome = pass->pw_dir;
	
	DebuG(("User Home = %s\n", userHome));
	
	char dir[PATH_MAX];
	strcpy(dir, "");

	sprintf(dir, "%s/.parallelrealities", userHome);
	if ((mkdir(dir, S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH) != 0) && (errno != EEXIST))
	{
		printf("Couldn't create required directory '%s'", dir);
		exit(1);
	}

	sprintf(dir, "%s/.parallelrealities/blobwars", userHome);
	if ((mkdir(dir, S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH) != 0) && (errno != EEXIST))
	{
		printf("Couldn't create required directory '%s'", dir);
		exit(1);
	}

	char gameSavePath[PATH_MAX];
	sprintf(gameSavePath, "%s/.parallelrealities/blobwars/", userHome);
	engine.setUserHome(gameSavePath);
}

#endif

bool loadConfig()
{
	float version = 0;
	int release = 0;
	bool rtn = false;

	char configPath[PATH_MAX];

	sprintf(configPath, "%sconfig", engine.userHomeDirectory);

	DebuG(("Loading Config from %s\n", configPath));

	FILE *fp = fopen(configPath, "rb");

	if (!fp)
	{
		return true;
	}

	fscanf(fp, "%f %d", &version, &release);

	DebuG(("Version = %.2f - Expected %.2f\n", version, VER));
	DebuG(("Release = %d - Expected %d\n", release, RELEASE));

	if ((version != VER) && (release != RELEASE))
	{
		rtn = true;
	}

	fscanf(fp, "%d %d %d %d %d %d %d", &engine.fullScreen, &game.musicVol, &game.soundVol, &game.output, &game.brightness, &engine.extremeAvailable, &game.gore);

	fclose(fp);

	DebuG(("Extreme Mode = %d\n", engine.extremeAvailable));
	DebuG(("Output Type = %d\n", game.output));

	// Override audio if there is no sound available
	if ((engine.useAudio) && (game.output))
	{
		engine.useAudio = game.output;
	}
		
	engine.loadKeyConfig();
	engine.loadJoystickConfig();

	return rtn;
}

void saveConfig()
{
	char configPath[PATH_MAX];

	sprintf(configPath, "%sconfig", engine.userHomeDirectory);

	FILE *fp = fopen(configPath, "wb");

	if (!fp)
	{
		printf("Error Saving Config to %s\n", configPath);
		return;
	}

	fprintf(fp, "%f %d\n", VER, RELEASE);
	fprintf(fp, "%d %d %d %d %d %d %d\n", engine.fullScreen, game.musicVol, game.soundVol, game.output, game.brightness, engine.extremeAvailable, game.gore);

	fclose(fp);
	
	DebuG(("Output Type = %d\n", game.output));
}

/*
Chugg chugg chugg.... brrr... chugg chugg chugg...brrrrrr... chugg ch..
BRRRRRRRRRRRRRRRRRMMMMMMMMMMMMMMMMMMM!! Well, hopefully anyway! ;)
*/
void initSystem()
{
	#if UNIX
		setupUserHomeDirectory();
	#endif

	bool displayLicense = loadConfig();
	
	long flags = SDL_INIT_VIDEO|SDL_INIT_JOYSTICK;
			
	if (engine.useAudio)
	{
		flags |= SDL_INIT_AUDIO;
	}

	/* Initialize the SDL library */
	if (SDL_Init(flags) < 0)
	{
		printf("Couldn't initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}

	if (!engine.fullScreen)
	{
		graphics.screen = SDL_SetVideoMode(640, 480, 16,  SDL_FULLSCREEN);
	}
	else
	{
		graphics.screen = SDL_SetVideoMode(640, 480, 16,  SDL_FULLSCREEN);
	}

	if (graphics.screen == NULL)
	{
		printf("Couldn't set 640x480 video mode: %s\n", SDL_GetError());
		exit(1);
	}

	// This (attempts to) set the gamma correction. We attempt to catch an error here
	// in case someone has done something really stupid in the config file(!!)
    if (game.brightness != -1) {
        Math::limitInt(&game.brightness, 1, 20);
        float brightness = game.brightness;
        brightness /= 10;
        SDL_SetGamma(brightness, brightness, brightness);
    }

	if (TTF_Init() < 0)
	{
		printf("Couldn't initialize SDL TTF: %s\n", SDL_GetError());
		exit(1);
	}

	if (engine.useAudio)
	{
		if (Mix_OpenAudio(44100, AUDIO_S16, engine.useAudio, 1024) < 0)
		{
			printf("Warning: Couldn't set 22050 Hz 16-bit audio - Reason: %s\n", Mix_GetError());
			printf("Sound and Music will be disabled\n");
			engine.useAudio = 0;
		}
	}

	DebuG(("Found %d Joysticks...\n", SDL_NumJoysticks()));

	if (SDL_NumJoysticks() > 0)
	{
		SDL_JoystickEventState(SDL_ENABLE);

		for (int i = 0 ; i < SDL_NumJoysticks() ; i++)
		{
			DebuG(("Opening Joystick #%d - %s...\n", i, SDL_JoystickName(i)));
			engine.joystick[i] = SDL_JoystickOpen(i);
			
			if (i == 1)
			{
				break;
			}
		}
	}

	SDL_ShowCursor(SDL_DISABLE);
	SDL_EventState(SDL_MOUSEMOTION, SDL_DISABLE);

	graphics.registerEngine(&engine);
	graphics.mapColors();

 	audio.registerEngine(&engine);
	audio.setSoundVolume(game.soundVol);
	audio.setMusicVolume(game.musicVol);
	audio.output = game.output;

	DebuG(("Sound Volume = %d\n", game.soundVol));
	DebuG(("Music Volume = %d\n", game.musicVol));
	DebuG(("Output Type = %d\n", game.output));
	DebuG(("Brightness = %d\n", game.brightness));
	DebuG(("tmp dir = %s\n", engine.userHomeDirectory));
	DebuG(("Pack Dir = %s\n", PAKLOCATION));
	DebuG(("Loading Fonts...\n"));

	#if USEPAK
			
		char tempPath[PATH_MAX];
		sprintf(tempPath, "%sfont.ttf", engine.userHomeDirectory);	
		remove(tempPath);
		
		SDL_Delay(1000); // wait one second, just to be sure!
		
		if (!engine.unpack("data/vera.ttf", PAK_FONT))
		{
			engine.reportFontFailure();
		}
	#endif

	DebuG(("Trying to load correct font pixel sizes using a really half arsed routine!\n"));
	DebuG(("If it crashes then you'll know why!\n"));
			
	graphics.loadFont(0, "data/vera.ttf", 7);
	graphics.loadFont(1, "data/vera.ttf", 9);
	graphics.loadFont(2, "data/vera.ttf", 11);
	graphics.loadFont(3, "data/vera.ttf", 13);
	graphics.loadFont(4, "data/vera.ttf", 15);
	
	DebuG(("Font sizes all loaded!!\n"));

	audio.loadSound(SND_CHEAT, "sound/Lock And Load!!!");
	audio.loadSound(SND_HIGHLIGHT, "sound/menu.wav");
	audio.loadSound(SND_SELECT, "sound/select.wav");

	SDL_Surface *device = graphics.loadImage("gfx/main/alienDevice.png");

	SDL_EnableKeyRepeat(350, 80);

	SDL_FreeSurface(device);
	
	if (strstr(engine.userHomeDirectory, "/root"))
	{
		graphics.showRootWarning();
	}

	if (displayLicense)
	{
		showLicense();
	}
	else
	{
		checkForLicense();
	}
		
	srand(time(NULL));
	
	engine.saveConfig = true;

	DebuG(("Init Complete...\n"));
}

/*
Removes [hopefully] all the resources that has been
loaded and created during the game. This is called by
//atexit();
*/
void cleanup()
{
	char tempPath[PATH_MAX];
	
	DebuG(("Cleaning Up...\n"));

	DebuG(("Freeing Audio...\n"));
	audio.destroy();

	DebuG(("Removing Music...\n"));
	sprintf(tempPath, "%smusic.mod", engine.userHomeDirectory);
	remove(tempPath);

	DebuG(("Freeing Game Info...\n"));
	game.destroy();

	DebuG(("Freeing Map Data...\n"));
	map.destroy();

	DebuG(("Freeing Engine Data...\n"));
	engine.destroy();

	DebuG(("Freeing Graphics...\n"));
	graphics.destroy();

	DebuG(("Saving Config...\n"));
	if (engine.saveConfig)
	{
		saveConfig();
	}

	DebuG(("Removing Font File...\n"));
	sprintf(tempPath, "%sfont.ttf", engine.userHomeDirectory);
	remove(tempPath);
	
	if (SDL_NumJoysticks() > 0)
	{
		SDL_JoystickEventState(SDL_DISABLE);
		for (int i = 0 ; i < SDL_NumJoysticks() ; i++)
		{
			DebuG(("Closing Joystick #%d - %s...\n", i, SDL_JoystickName(i)));
			SDL_JoystickClose(engine.joystick[i]);
		}
	}

	DebuG(("Closing Audio...\n"));
	if (engine.useAudio)
	{
		Mix_CloseAudio();
	}

	DebuG(("Closing TTF...\n"));
	TTF_Quit();

	DebuG(("Closing SDL Sub System...\n"));
	SDL_Quit();

	DebuG(("All Done.\n"));
}


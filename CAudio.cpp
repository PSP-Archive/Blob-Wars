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

Audio::Audio()
{
	output = 2;
	useSound = true;
	useMusic = true;

	for (int i = 0 ; i < MAX_SOUNDS ; i++)
	{
		sound[i] = NULL;
	}

	music = NULL;
	quickSound = NULL;
	
	strcpy(levelMusicName, "");
}

void Audio::setSoundVolume(int soundVolume)
{
	this->soundVolume = soundVolume;
	if (engine->useAudio)
		Mix_Volume(-1, soundVolume);
}

void Audio::setMusicVolume(int musicVolume)
{
	this->musicVolume = musicVolume;
	if (engine->useAudio)
	{
		Mix_VolumeMusic(musicVolume);
	}
}

void Audio::registerEngine(Engine *engine)
{
	this->engine = engine;
}

bool Audio::loadSound(int i, char *filename)
{
	if (!engine->useAudio)
	{
		return true;
	}
		
	if (i >= MAX_SOUNDS)
	{
		printf("ERROR: SOUND INDEX IS HIGHER THAN MAXIMUM ALLOWED %d >= %d\n", i, MAX_SOUNDS);
		exit(1);
	}

	if (sound[i] != NULL)
	{
		Mix_FreeChunk(sound[i]);
		sound[i] = NULL;
	}

	#if USEPAK
		engine->unpack(filename, PAK_SOUND);
		sound[i] = Mix_LoadWAV_RW(engine->sdlrw, 1);
	#else
		sound[i] = Mix_LoadWAV(filename);
	#endif

	if (!sound[i])
	{
		DebuG(("WARNING - Failed to load %s\n", filename));
		return false;
	}
	
	return true;
}

bool Audio::loadMusic(char *filename)
{
	char tempPath[PATH_MAX];
	
	sprintf(tempPath, "%smusic.mod", engine->userHomeDirectory);
	
	if (!engine->useAudio)
	{
		return true;
	}

	remove(tempPath);
	
	SDL_Delay(250); // wait a bit, just to be sure!

	if (music != NULL)
	{
		Mix_HaltMusic();
		SDL_Delay(5);
		Mix_FreeMusic(music);
		music = NULL;
	}

	#if USEPAK
		engine->unpack(filename, PAK_MUSIC);
		music = Mix_LoadMUS(tempPath);
	#else
		music = Mix_LoadMUS(filename);
	#endif

	if (!music)
	{
		DebuG(("WARNING - Failed to load %s\n", filename));
		return false;
	}
		
	strcpy(levelMusicName, filename);

	return true;
}

void Audio::playSound(int snd, int channel)
{
	if ((!engine->useAudio) || (soundVolume == 0))
		return;
	
	if (!output)
	{
		return;
	}

	Mix_Volume(channel, soundVolume);

	Mix_PlayChannel(channel, sound[snd], 0);
}

void Audio::playMusic()
{
	if (!engine->useAudio)
		return;
	
	if (!output)
	{
		return;
	}

	Mix_PlayMusic(music, -1);

	Mix_VolumeMusic(musicVolume);
}

void Audio::playMusicOnce()
{
	if (!engine->useAudio)
		return;
	
	if (!output)
	{
		return;
	}

	Mix_PlayMusic(music, 0);

	Mix_VolumeMusic(musicVolume);
}

bool Audio::loadGameOverMusic()
{
	char tempPath[PATH_MAX];
	
	sprintf(tempPath, "%smusic.mod", engine->userHomeDirectory);
	
	if (!engine->useAudio)
	{
		return true;
	}

	remove(tempPath);
	SDL_Delay(250); // wait a bit, just to be sure!

	if (music != NULL)
	{
		Mix_HaltMusic();
		SDL_Delay(5);
		Mix_FreeMusic(music);
		music = NULL;
	}

	#if USEPAK
		engine->unpack("music/friendDied.mod", PAK_MUSIC);
		music = Mix_LoadMUS(tempPath);
	#else
		music = Mix_LoadMUS("music/friendDied.mod");
	#endif

	if (!music)
		return false;

	return true;
}

bool Audio::reloadLevelMusic()
{
	// remove the Game Over music first...

	if (music != NULL)
	{
		Mix_HaltMusic();
		SDL_Delay(5);
		Mix_FreeMusic(music);
		music = NULL;
	}

	return loadMusic(levelMusicName);
}

void Audio::playAmbiance()
{
	if ((!engine->useAudio) || (soundVolume == 0))
		return;
	
	if (!output)
	{
		return;
	}

	Mix_PlayChannel(CH_AMBIANCE, sound[SND_AMBIANCE], -1);
}

void Audio::stopAmbiance()
{
	if ((!engine->useAudio) || (soundVolume == 0))
		return;

	Mix_HaltChannel(CH_AMBIANCE);
}

int Audio::playMenuSound(int sound)
{
	if ((!engine->useAudio) || (soundVolume == 0))
		return sound;

	if ((sound == 0) || (sound == 3))
		return sound;

	if (sound == 1)
		playSound(SND_HIGHLIGHT, CH_ANY);

	if (sound == 2)
		playSound(SND_SELECT, CH_ANY);
		
	return sound;
}

void Audio::pause()
{
	if (!engine->useAudio)
		return;

	for (int i = 0 ; i < 8 ; i++)
		Mix_Pause(i);

	Mix_PauseMusic();
}

void Audio::resume()
{
	if (!engine->useAudio)
		return;
	
	if (!output)
	{
		return;
	}

	for (int i = 0 ; i < 8 ; i++)
		Mix_Resume(i);

	Mix_ResumeMusic();
}

void Audio::stopMusic()
{
	if (!engine->useAudio)
		return;

	Mix_HaltMusic();
}

void Audio::fadeMusic()
{
	if (!engine->useAudio)
		return;

	Mix_FadeOutMusic(3500);
}

void Audio::free()
{
	for (int i = 0 ; i < MAX_SOUNDS - 3 ; i++)
	{
		if (sound[i] != NULL)
		{
			Mix_FreeChunk(sound[i]);
			sound[i] = NULL;
		}
	}

	if (music != NULL)
	{
		Mix_HaltMusic();
		SDL_Delay(5);
		Mix_FreeMusic(music);
	}

	music = NULL;

	if (quickSound != NULL)
		Mix_FreeChunk(quickSound);

	quickSound = NULL;
}

void Audio::destroy()
{
	free();

	for (int i = MAX_SOUNDS - 3 ; i < MAX_SOUNDS ; i++)
	{
		if (sound[i] != NULL)
		{
			Mix_FreeChunk(sound[i]);
			sound[i] = NULL;
		}
	}
}

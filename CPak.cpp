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
#include "SDL/SDL_endian.h"

Pak::Pak()
{
	input = NULL;
	fd = NULL;
	
	numberOfFiles = 0;
	listPos = 0;
	currentFile = NULL;

	strcpy(pakFilename, "");
	strcpy(filename, "");
}

Pak::~Pak()
{
	if (input != NULL)
		delete[] input;
		
	input = NULL;
	
	if (fd != NULL)
		delete[] fd;
	
	fd = NULL;
}

void Pak::showPakErrorAndExit()
{
	printf("\nFatal Error: The Blob Wars PAK file was either not found or was not accessable.\n");
	printf("(If you compiled from source did you forget to run make install?)\n");
	printf("The path to the file was expected to be,\n\n");
	printf(PAKFULLPATH"\n\n");
	printf("Please try running the game again. If problems persist either reinstall the game or check,\n\n");
	printf("http://www.parallelrealities.co.uk/blobWars.php\n\n");
	printf("for updates.\n\n");
	exit(1);
}

void Pak::setPakFile(char *pakFilename)
{
	#if USEPAK
	strcpy(this->pakFilename, pakFilename);
	
	//DebuG(("Pak : Filename set to %s\n", pakFilename));

	FILE *pak = fopen(pakFilename, "rb");

	if (!pak)
	{
		showPakErrorAndExit();
	}

	fseek(pak, (-sizeof(Uint32)) * 2, SEEK_END);
	fread(&listPos, sizeof(Uint32), 1, pak);
	fread(&numberOfFiles, sizeof(Uint32), 1, pak);
	
	listPos = SDL_SwapLE32(listPos);
	numberOfFiles = SDL_SwapLE32(numberOfFiles);
	
	//DebuG(("Pak : File list resides at %d\n", (int)listPos));
	//DebuG(("Pak : Number of files are %d\n", (int)numberOfFiles));
	
	fd = new FileData[numberOfFiles];
	
	fseek(pak, listPos, SEEK_SET);
	
	int result = 0;
	
	for (unsigned int i = 0 ; i < numberOfFiles ; i++)
	{
		result = fread(&fd[i], sizeof(FileData), 1, pak);
		
		if (!result)
		{
			fclose(pak);
			showPakErrorAndExit();
		}
		
	//	DebuG(("Read FileData #%d / %d : %s\n", (i + 1), numberOfFiles, fd[i].filename));
		
		fd[i].swapEndians();
	}
	
	fclose(pak);
	
	#endif
}

bool Pak::unpack(char *filename, unsigned char **buffer)
{
	DebuG(("Pak : Unpacking %s...\n", filename));
	
	currentFile = NULL;
	
	for (unsigned int i = 0 ; i < numberOfFiles ; i++)
	{
		if (strcmp(filename, fd[i].filename) == 0)
		{
			currentFile = &fd[i];
			break;
		}
	}
	
	if (currentFile == NULL)
	{
		return false;
	}
	
	FILE *pak = fopen(pakFilename, "rb");
	if (!pak)
	{
		showPakErrorAndExit();
	}
	
	fseek(pak, currentFile->location, SEEK_SET);

	if (input != NULL)
	{
		delete[] input;
	}

	input = NULL;
	
	input = new unsigned char[(int)(currentFile->cSize * 1.01) + 12];
	*buffer = new unsigned char[currentFile->fSize + 1];

	fread(input, 1, currentFile->cSize, pak);
	
	uLongf fSize = (uLongf)currentFile->fSize;
	
	uncompress(*buffer, &fSize, input, currentFile->cSize);
	(*buffer)[currentFile->fSize] = 0;

	fclose(pak);
	
	if (input != NULL)
		delete[] input;

	input = NULL;
	
	DebuG(("Pak : Unpack %s...Done\n", filename));

	return true;
}

bool Pak::fileExists(char *filename)
{
	unsigned int hashcode = 0;
	unsigned int length = strlen(filename);
	
	for (unsigned int i = 0 ; i < length ; i++)
	{
		hashcode = hashcode + (filename[i] * 31 ^ (length - i));
	}
	
	for (unsigned int i = 0 ; i < numberOfFiles ; i++)
	{
		if (strcmp(fd[i].filename, filename) == 0)
		{
			return true;
		}
	}
	
	return false;
}

unsigned int Pak::getUncompressedSize()
{
	return (unsigned int)currentFile->fSize;
}

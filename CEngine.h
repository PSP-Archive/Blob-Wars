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

class Engine {

	private:

		SDL_Event event;
		signed char frameLoop;
		int mouseX, mouseY;

		// Time Difference
		unsigned int time1;
		unsigned int time2;
		float timeDifference;
		
		// used for cheating!
		char lastKeyPressed[25];
		char lastKeyEvents[25];
		
		// Joystick config stuff
		int lastButtonPressed;
		
		Pak pak;

		// Consolidate joystick axis information.
		int joyX, joyY;

		// Used to temporarily disable the joystick (eg, in
		// option screens): the tick at which joystick
		// navigation will be re-enabled.
		//
		// Note: may break if the game runs for more than
		// 49 days, resulting in an inoperable joystick.
		Uint32 joyNavAvail;

	public:
		
		int extremeAvailable;

		char keyState[350];
		char mouseLeft, mouseRight;
		
		SDL_Joystick *joystick[2];
		Joystick joypad;
		
		Keyboard keyboard;
		bool waitForButton;
		bool waitForKey;
		bool allowJoypad;
		
		bool isKey(int key);
		void clearKey(int key);

		bool paused;
		bool saveConfig;
		bool allowQuit;

		char userHomeDirectory[PATH_MAX];

		int useAudio;
		int fullScreen;
		int skill;
		bool practice;

		// Development stuff
		bool devNoMonsters;

		SDL_RWops *sdlrw;

		unsigned char *binaryBuffer; // used for unzipping
		unsigned char *dataBuffer; // used for unzipping

		int messageTime;
		int messagePriority;
		int messageType;
		char message[100];

		char saveSlot[10][50];
		int continueSaveSlot;

		int playerPosX, playerPosY;

		List widgetList;

		Widget *highlightedWidget;

		Entity world;

		List defineList;

		bool cheats;
		int cheatHealth, cheatExtras, cheatFuel, cheatLevels, cheatBlood, cheatInvulnerable;
		int cheatReload, cheatSpeed, cheatSkipLevel;

	Engine();
	void destroy();
	void startup();
	void getInput();
	int getMouseX();
	int getMouseY();

	bool getJoyLeft();
	bool getJoyRight();
	bool getJoyUp();
	bool getJoyDown();

	// Return a value between -max and +max.
	int getJoyX(int max);
	int getJoyY(int max);
	void lockJoy(Uint32 ticks);

	void setMouse(int x, int y);
	bool userAccepts();
	
	bool loadJoystickConfig();
	bool saveJoystickConfig();
	
	bool loadKeyConfig();
	bool saveKeyConfig();
	void restoreKeyDefaults();
	
	bool getKeyState(int key);
	void setKeyState(int key);
	
	void doPause();
	void clearCheatVars();
	bool compareLastKeyInputs();
	void addKeyEvent();
	void flushInput();
	void clearInput();
	void setUserHome(char *path);
	Pak *getPak();
	bool unpack(char *filename, int fileType);
	bool loadData(char *filename);
	void reportFontFailure();
	void setPlayerPosition(int x, int y, int limitLeft, int limitRight, int limitUp, int limitDown);
	int getFrameLoop();
	void doFrameLoop();
	void doTimeDifference();
	float getTimeDifference();
	void resetTimeDifference();
	void setInfoMessage(char *message, int priority, int type);
	void deleteWidgets();
	void addWidget(Widget *widget);
	bool loadWidgets(char *filename);
	Widget *getWidgetByName(char *name);
	void showWidgetGroup(char *groupName, bool show);
	void enableWidgetGroup(char *groupName, bool show);
	void showWidget(char *name, bool show);
	void enableWidget(char *name, bool enable);
	void setWidgetVariable(char *name, int *variable);
	bool widgetChanged(char *name);
	void highlightWidget(int dir);
	void highlightWidget(char *name);
	int processWidgets();
	bool loadDefines();
	int getValueOfDefine(char *word);
	char *getDefineOfValue(char *prefix, int value);
	int getValueOfFlagTokens(char *line);

};

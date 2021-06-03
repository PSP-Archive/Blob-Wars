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

class GameData {

	public:
		
		bool completedWorld;

		List dataList;

	void clear();
	void destroy();
	
	void addCompletedObjective(char *key, char *value, int current, int target);
	void addCompletedObjective(Data *newData);
	
	void setMIARescueCount(char *key, int rescues, int total);
	bool MIARescued(char *stageName, char *name);
	
	bool objectiveCompleted(char *stageName, char *name);
	void getObjectiveValues(char *stageName, char *name, int *current, int *target);
	
	bool stagePreviouslyCleared(char *stageName);
	bool isCompleted(char *key, char *value);
	int getPercentageComplete();
	
	bool levelPrefectlyCleared(char *level);
	bool requiredLevelCleared(char *requiredLevel);
	
	void calculateWorldCompleted();
	void calculateAllCompleted();

	GameData();

};

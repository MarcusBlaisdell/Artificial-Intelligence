// Agent.h

#ifndef AGENT_H
#define AGENT_H

#include "Action.h"
#include "Percept.h"

#include "Location.h"
#include "Orientation.h"
#include "Search.h"
#include <list>
#include <vector>
#include <fstream>

class Agent
{
public:
	Agent ();
	~Agent ();
	void Initialize ();
	Action Process (Percept& percept);
	void GameOver (int score);
	int readFile ();
	void removePotentialPit (list <Location> potentialPit, Location location);

	bool agentHasGold;
	list<Action> actionList;
	SearchEngine searchEngine;

	list <Location> pitList;			// track pit locations
	list <Location> potentialPit;	// track potential pits
	Location wumpusLoc; 					// record the wumpus location
	Location curLoc; 							// track our current location
	Location prevLoc;							// track previous location
	int curDir;										// track our direction, 0=right, 1=up, 2=left, 3=down
	Location goldLoc; 						// track the location of the gold
	int normalExit;								// 1 if we exit by climbing, 0 otherwise indicating we died
	int lastPercept;							// track if last percept is breeze, 1 or stench, 2, or nothing, 0
	list <Location> possibleMoveList;
	int fileCount;								// track the number of times saved file is used
};

#endif // AGENT_H

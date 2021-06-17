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

	typedef struct locPlus
	{
		Location theLoc;
		bool theExists;
		float theProb;
	} LocPlus;

	int readFile ();
	void checkSafe (Location);
	void checkBreeze (Location, bool);
	void checkFrontier (LocPlus);
	int checkPit (Location);
	void addToFrontier (LocPlus);
	void removeFromFrontier (LocPlus);
	float calculateProbability ();
	void calculateFrontier ();
	void recursePrime (list <LocPlus>, int *, int *, int, float *, float *, float *, bool *);
	int evaluateBreeze ();
	int consistentCheck (LocPlus, bool);
	void evaluateAdjacent (LocPlus);
	void addToPossibles (LocPlus);
	void addToPossiblesHelper (LocPlus);
	void printFrontier ();
	void addToKnown (Location);
	void printKnown ();
	void printBreeze ();
	void addBreeze (LocPlus);
	void addBreezeHelper (LocPlus);
	int testSingle (LocPlus);
	float strtoflt (string);
	void printSafe ();
	void printProbabilities ();
	void removeUnvisited (Location);
	void printUnvisited ();
	int testFrontier ();
	void cornerCase (Location);
	int cornerCaseHelper (LocPlus);

	bool agentHasGold;
	list<Action> actionList;
	SearchEngine searchEngine;

	list <Location> pitList;			// track pit locations
	Location wumpusLoc; 					// record the wumpus location
	Location curLoc; 							// track our current location
	Location prevLoc;							// track previous location
	int curDir;										// track our direction, 0=right, 1=up, 2=left, 3=down
	Location goldLoc; 						// track the location of the gold
	int normalExit;								// 1 if we exit by climbing, 0 otherwise indicating we died
	int lastPercept;							// track if last percept is breeze, 1 or stench, 2, or nothing, 0
	//list <Location> possibleMoveList;	// maintain a list of possible moves adjacent to current position
	list <LocPlus> possibleMoveList;	// maintain a list of possible moves adjacent to current position
	int fileCount;								// track the number of times saved file is used
	list <LocPlus> known;					// information about pits in visited locations
	list <LocPlus> frontier;			// all locations adjacent to visited locations
	list <LocPlus> newFrontier;		// all locations in frontier, dynamically managed for recursive evaluations
	list <LocPlus> frontierPrime;			// all locations adjacent to visited locations that aren't the query Location
	list <LocPlus> breezeList;		// all locations where a breeze has been detected
	list <LocPlus> visited;			// all locations we have visited
	list <LocPlus> unvisited;		// All locations we have not visited or calculated the probability for
	LocPlus Query;							// the query location


};

#endif // AGENT_H

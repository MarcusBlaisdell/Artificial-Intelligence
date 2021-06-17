// Agent.h

#ifndef AGENT_H
#define AGENT_H

#include "Action.h"
#include "Percept.h"

class Agent
{
public:
	Agent ();
	~Agent ();
	void Initialize ();
	Action Process (Percept& percept);
	void GameOver (int score);
		// add function to track the path to the gold
		// and the path back to the entrance
		// if they are equal, the path could be optimal,
		// if the to path is greater than the return path,
		// the path was definitely not optimal
	void evalOptimal (void);

		// xPos is X-values in (x,y) coordinate system
		// yPos is Y-values in (x,y coordinate system)
		// Direction is which way we are facing:
		// 0=right, 1=up, 2=left, 3=down
		// Gold is an indicator that tells if we have acquired the gold or not
		// 0 = no gold, 1 = have gold
		// Wumpus is an indicator tells if Wumpus is alive or dead
		// 0 = Wumpus is alive, 1 = Wumpus is dead
		// return path is the column of row two we used to access the upper
		// map. If it is zero, it is unset and we have not yet moved up to row 2
	int xPos;
	int yPos;
	int Direction;
	bool Gold;
	bool Wumpus; // 0 = alive, 1 = dead
	int returnPath;
		// integer values to track the moves to get to the Gold
		// and the moves to get back to the entrance 
	int toScore;
	int retScore;
};

#endif // AGENT_H

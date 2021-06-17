// Agent.cc

#include <iostream>
#include "Agent.h"

using namespace std;

Agent::Agent ()
{
	// Agent does not create any new objects
}

Agent::~Agent ()
{
	// We did not create any objects and we did not allocate
	// any memory
	// I would call GameOver from here but it is apparently being called
	// from elsewhere so to avoid duplication, I have ommitted it
}

void Agent::Initialize ()
{
		// initialize tracking information:
	xPos = 1;
	yPos = 1;
	Direction = 0;
	Gold = 0;
	Wumpus = 0;
	returnPath = 0;
	toScore = 0;
	retScore = 0;
}

Action Agent::Process (Percept& percept)
{
	char c;
	Action action;
	bool validAction = false;

	while (! validAction)
	{
		validAction = true;
		// uncomment to debug:
		//cout << xPos << "," << yPos << " : " << Direction << " : " << Gold << " : " << Wumpus << endl;

			// if we are in a gold cell, automatically grab it

		if (percept.Glitter == 1)
		{
			Gold = 1;
			action = GRAB;
			return action;
		} // end if we are in a gold cell, automatically grab it

		// if we have returned to the home position, climb out
	if (xPos == 1 && yPos == 1 && Direction > 1)
	{
		evalOptimal();
		cout << xPos << ',' << yPos << endl;
		action = CLIMB;
		return action;
	}

			// if we have killed the wumpus, record that:

		if (percept.Scream == 1)
		{
			Wumpus = 1;
		} // end if wumpus is killed, set flag

			// if position = (1,1) and no stench or breeze, turn to up and move forward:

		if (xPos == 1 && yPos == 1 && Direction == 0 && percept.Stench == 0 && percept.Breeze == 0)
		{
			evalOptimal();
			Direction = 1;
			returnPath = 1;
			action = TURNLEFT;
			return action;
		} // end if (1,1) and no obstruction, turn left
		//if (xPos == 1 && yPos == 1 && Direction == 1 && percept.Stench == 0 && percept.Breeze == 0)
		if (yPos < 4 && Direction == 1 && Gold == 0 && percept.Stench == 0 && percept.Breeze == 0)
		{
			evalOptimal();
			yPos++;
			action = GOFORWARD;
			return action;
		} // end if (1,1) and no obstruction, turn left

		// if position 1,1 and stench, turn left, fire arrow, move forward
		if (xPos == 1 && yPos == 1 && Direction == 0 && percept.Stench == 1)
		{
			evalOptimal();
			Direction++;
			returnPath = 1;
			action = TURNLEFT;
			return action;
		} // end if 1,1, and stench, turn left
		if (xPos == 1 && yPos == 1 && Direction == 1 && percept.Stench == 1 && Wumpus == 0)
		{
			action = SHOOT;
			return action;
		} // end if 1,1, and stench, turn left
		if (xPos == 1 && yPos == 1 && Direction == 1 && percept.Stench == 1 && Wumpus == 1)
		{
			evalOptimal();
			yPos++;
			action = GOFORWARD;
			return action;
		} // end if 1,1, and stench, turn left

		if (yPos > 2 && percept.Breeze == 1 && Direction == 3) // One
		{
			evalOptimal();
			if (xPos - returnPath < 0)
			{
				Direction = 0;
				action = TURNLEFT;
			} // end if return path is to camera right
			else
			{
				Direction = 2;
				action = TURNRIGHT;
			} // end else, return path is camera left

			return action;
		} // end if moving down and breeze detected, turn to left

		if (yPos > 2 && xPos > 1 && percept.Stench == 1 && Direction == 3) // Two
		{
			evalOptimal();
			if (xPos - returnPath < 0)
			{
				Direction = 0;
				action = TURNLEFT;
			}
			else
			{
				Direction--;
				action = TURNRIGHT;
			}

			return action;
		} // end if moving down and not in first column and stench detected, turn left

		if (yPos > 1 && xPos == 1 && percept.Breeze == 0 && Direction == 2) // Four
		{
			evalOptimal();
			Direction = 3;
			action = TURNLEFT;
			return action;
		}

		if (yPos > 1 && xPos > 1 && xPos < 4 && (percept.Breeze == 1 || percept.Stench == 1) && (Direction == 0 || Direction == 2)) // Three
		{
			evalOptimal();
			if (Direction == 0)
			{
				xPos++;
			}
			else
			{
				xPos--;
			}
			action = GOFORWARD;
			return action;
		}

		if (yPos > 1 && xPos > 1 && percept.Breeze == 0 && percept.Stench == 0 && Direction == 0) // Four
		{
			evalOptimal();
			Direction = 3;
			action = TURNRIGHT;
			return action;
		}

		if (yPos == 4 && xPos == 2 && Direction == 3)
		{
			evalOptimal();
			yPos--;
			action = GOFORWARD;
			return action;
		}

		if (yPos == 1 && Direction == 3) // Five
		{
			evalOptimal();
			Direction--;
			action = TURNRIGHT;
			return action;
		}

		if (yPos == 1 && Direction == 2 && xPos > 1)
		{
			evalOptimal();
			xPos--;
			action = GOFORWARD;
			return action;
		}

			// if position = 1, 1 and there is a breeze, move right until no breeze or stench
		if (xPos == 1 && yPos == 1 && percept.Breeze == 1)
		{
			evalOptimal();
			xPos++;
			action = GOFORWARD;
			return action;
		} // end if
		if (Direction == 0 && (percept.Breeze == 1 || percept.Stench == 1))
		{
			evalOptimal();
			xPos++;
			action = GOFORWARD;
			return action;
		} // end if stench or breeze, move forward if facing right
		if (xPos > 1 && yPos == 1 && Direction == 0) // Six
		{
			evalOptimal();
			Direction++;
			returnPath = xPos;
			action=TURNLEFT;
			return action;
		} // end if all the way to right and facing right, turn left

		if (Direction == 1 && Gold == 1)
		{
			evalOptimal();
			Direction = 2;
			action = TURNLEFT;
			return action;
		}

			// if not at last row, and no gold, move to last row
		if (yPos > 1 && yPos < 4 && Direction == 1 && Gold == 0)
		{
			evalOptimal();
			yPos++;
			action = GOFORWARD;
			return action;
		} // end move to last row

		if (yPos == 4 && xPos > 1 && Direction == 1)
		{
			evalOptimal();
			Direction++;
			action = TURNLEFT;
			return action;
		} // end if facing up and at last row, turn left

		if (yPos == 4 && xPos == 1 && Gold == 0 && Direction == 1)
		{
			evalOptimal();
			Direction--;
			action = TURNRIGHT;
			return action;
		} // end if facing up and at last row, turn left

		if (xPos < 4 && yPos > 1 && Gold == 0 and Direction == 0) // Seven
		{
			evalOptimal();
			xPos++;
			action = GOFORWARD;
			return action;
		}

		if (xPos > 1 && Gold == 0 && Direction == 2)
		{
			evalOptimal();
			xPos--;
			action = GOFORWARD;
			return action;
		} // end if not at x = 1 and facing left, move forward

		if (xPos == 1 && yPos == 4 && Gold == 0 && Direction == 2)
		{
			evalOptimal();
			Direction++;
			action = TURNLEFT;
			return action;
		}

		if (xPos == 1 && yPos == 4 && Gold == 0 && Direction == 3)
		{
			evalOptimal();
			yPos--;
			action = GOFORWARD;
			return action;
		}

		if (xPos == 1 && yPos == 3 && Gold == 0 && Direction == 3)
		{
			evalOptimal();
			Direction = 0;
			action = TURNLEFT;
			return action;
		}

		if (xPos == 1 && yPos == 3 && Gold == 0 and Direction == 0)
		{
			evalOptimal();
			xPos++;
			action = GOFORWARD;
			return action;
		}

		// if we have the gold, face down:
	if (yPos > 1 && Gold == 1 && Direction == 0)
	{
		evalOptimal ();
		Direction = 3;
		action = TURNRIGHT;
		return action;
	} // end if we have the gold, turn down
	if (yPos > 1 && xPos > 1 && Gold == 1 && Direction == 2)
	{
		evalOptimal ();
		if (xPos - returnPath > 0)
		{
			xPos--;
			action = GOFORWARD;
		}
		else
		{
			Direction++;
			action = TURNLEFT;
		}
		return action;
	} // end if we have the gold, turn down
	if (yPos > 1 && Gold == 1 && Direction == 1)
	{
		evalOptimal ();
		Direction++;
		action = TURNLEFT;
		return action;
	} // end if we have the gold, turn down

		// if not at first row, and have gold, move to first row
	if (yPos > 1 && Direction == 3 && Gold == 1)
	{
		evalOptimal ();
		yPos--;
		action = GOFORWARD;
		return action;
	} // end move to last row

		cout << "Action? ";
		cin >> c;
		if (c == 'f') {
			action = GOFORWARD;
		}
		else if (c == 'l')
		{
			Direction++;
			if (Direction == 4)
			{
				Direction == 0;
			}
			action = TURNLEFT;
		}
		else if (c == 'r')
		{
			Direction--;
			if (Direction == -1)
			{
				Direction = 0;
			}
			action = TURNRIGHT;
		} else if (c == 'g') {
			action = GRAB;
		} else if (c == 's') {
			action = SHOOT;
		} else if (c == 'c') {
			action = CLIMB;
		} else {
			cout << "Huh?" << endl;
			validAction = false;
		}
	}
	return action;
}

void Agent::GameOver (int score)
{
	int totalScore = 1000;

	if (Gold == 1)
	{
		totalScore--; // subtract one move for grabbing the gold
		cout << "You have successfully navigated the cave" << endl;
		if (toScore > retScore)
		{
			cout << "You took " << toScore << " moves to get the gold" << endl;
			cout << "and " << retScore << " moves to return to the entrance" << endl;
			cout << "your path could have been improved by " << toScore - retScore << " moves" << endl;
		} // end if not optimal path
		else
		{
			cout << "You appear to have found the optimal path" << endl;
		} // end if optimal path
	} // end if success
	else
	{
		cout << "You survived the cave but you did not locate the gold" << endl;
		cout << "I wish you better luck in your next attempt" << endl;
	} // end if not successful

	if (Wumpus == 1)
	{
			// we the Wumpus was killed, subtract 10
		totalScore = totalScore - 10;
	} // end subtract Wumpus kill penalty

	totalScore = totalScore - toScore - retScore;

	cout << "Your total score is: " << totalScore << endl;

}

void Agent::evalOptimal (void)
{
	if (Gold == 0)
	{
		toScore++;
	}
	else
	{
		retScore++;
	}
} // end evalOptimal function

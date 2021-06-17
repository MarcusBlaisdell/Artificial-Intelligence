// Agent.cc

#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include <string.h>
#include "Agent.h"

using namespace std;

Agent::Agent ()
{

} // end Agent constructor

Agent::~Agent ()
{
	ofstream outputFile;

	//cout << "X: " << curLoc.X << " Y: " << curLoc.Y << endl;
	if (normalExit == 0)
	{
		//cout << "normalExit is zero, lastPercept = " << lastPercept << "\n";
		if (lastPercept == 1)
		{
			//cout << "lastPercept is 1\n";
			//cout << "pit at this location" << endl;
			pitList.push_back (curLoc);
			searchEngine.RemoveSafeLocation(curLoc.X,curLoc.Y);
		} // end if
		else if (lastPercept == 2)
		{
			//cout << "lastPercept is 2\n";
			//cout << "wumpus at this location" << endl;
			wumpusLoc = curLoc;
			searchEngine.RemoveSafeLocation(curLoc.X,curLoc.Y);
		} // end else
	} // end if not normalExit

	outputFile.open ("saved.txt", ios::out);
	fileCount++;
	fileCount %= 10; // reset every ten iterations
	outputFile << fileCount << "\n";
	//cout << "fileCount: " << fileCount << endl;
	outputFile << goldLoc.X << "," << goldLoc.Y << "\n";
	outputFile << wumpusLoc.X << "," << wumpusLoc.Y << "\n";
	for (list<Location>::iterator it = pitList.begin (); it != pitList.end (); it++)
	{
		outputFile << it->X << "," << it->Y << "\n";
	} // end output all known pit locations

	outputFile << "safe\n";

	for (list<Location>::iterator it = searchEngine.safeLocations.begin (); it != searchEngine.safeLocations.end (); it++)
	{
		outputFile << it->X << "," << it->Y << "\n";
	} // end output all known pit locations

	outputFile << "possible\n";

	for (list<Location>::iterator it = possibleMoveList.begin (); it != possibleMoveList.end (); it++)
	{
		outputFile << it->X << "," << it->Y << "\n";
	} // end output all known pit locations

	outputFile << "EOF\n";
	outputFile.close ();

} // end Agent deconstructor

void Agent::Initialize ()
{
	ifstream inputFile;
	string line;
	list <Action> actionList2;
	int x, y;
	int a = 1, b = 1;

	/*
		// initialize all locations as safe locations:
	for (y = 1; y <= 4; y++)
	{
		for (x = 1; x <= 4; x++)
		{
			searchEngine.AddSafeLocation(x,y);
		} // end inner for loop
	} // end outer for loop
	*/

	/*
	inputFile.open ("saved.txt", ios::in);
	if (inputFile.is_open())
	{
		readFile (inputFile);
	} // if inputFile exists, read it

	inputFile.close ();
	*/

		// initialize what we know about locations:

	curLoc.X    = 1;
	curLoc.Y    = 1;
	prevLoc.X   = 0;
	prevLoc.Y   = 0;
	curDir      = 0;
	wumpusLoc.X = 0;
	wumpusLoc.Y = 0;
	goldLoc.X   = 0;
	goldLoc.Y   = 0;
	lastPercept = 0;
	fileCount   = 0;

	//cout << "calling readfile:\n";
	fileCount = readFile ();

	//cout << "finished readfile\n";

	//cout << "Safe locations List: \n";
	for (list<Location>::iterator it = searchEngine.safeLocations.begin (); it != searchEngine.safeLocations.end (); it++)
	{
		//cout << it->X << "," << it->Y << "\n";
	} // end output all known pit locations

	agentHasGold = false;
	actionList.clear();

}

Action Agent::Process (Percept& percept)
{
	Action action;
		// vector list of Actions
	list<Action> actionList2;
	Location theGoto;
		// Flag variable to test if an element exists in a list
	int exists = 0;
	int safeFlag = 1;
		// variables to store possible moves:
	Location p_up, p_down, p_left, p_right, possibleMove;

		/*****************************BEGIN****************************/

		// if we are here, this is a safe location
		// if it has not been added to safeLocations, add it:
		// flag will be set to 1 if the location is in the safeLocations list

	for (list<Location>::iterator it = searchEngine.safeLocations.begin (); it != searchEngine.safeLocations.end (); it++)
	{
		if (curLoc == *it)
		{
			exists = 1;
			//cout << curLoc.X << "," << curLoc.Y << " already exists\n";
		} // end if location is already in safe list, set exists flag so we don't re-add it

	} // end iterate through safelist to test if location is already in safeLocations

	if (exists == 0)
	{
		searchEngine.AddSafeLocation(curLoc.X, curLoc.Y);
		//cout << "Added: " << curLoc.X << "," << curLoc.Y << endl;
		removePotentialPit (potentialPit, curLoc);
	} // end if location isn't already in safeLocations, add it

		/***   ***   ***   ***   *** END Add safe location ***   ***   ***  ***   ***/

		/*****************************BEGIN****************************/

			// if we are at home position with the gold, exit the game: (c)
		if (agentHasGold && curLoc.X == 1 && curLoc.Y == 1 && (curDir == 2 || curDir == 3))
		{
		action = CLIMB;
		normalExit = 1;
		return action;
		} // end if home with gold, climb

		/***   ***   ***   ***   *** END if home with gold, exit ***   ***   ***  ***   ***/

		/***************************BEGIN******************************/

			// if we have found the gold, pick it up and set the agentHasGold flag: (b)

		if (!agentHasGold && percept.Glitter == 1)
		{
			//cout << "Glitter\n";
			action = GRAB;
			agentHasGold = 1;
			goldLoc.X = curLoc.X;
			goldLoc.Y = curLoc.Y;
			return action;
		} // if glitter, grab

		/***   ***   ***   ***   *** END if Glitter, Grab ***   ***   ***  ***   ***/

		/*****************************BEGIN****************************/

	if (actionList.empty())
	{
		//cout << "actionList is empty\n";

																/*** Begin convoluted move evaluations ***/

			/*****************************BEGIN****************************/

			// If we know the location of the gold, get to it, get it, get back, get out (d)

		if (goldLoc.X != 0)
		{
			//cout << "Gold location is known\n";
			//cout << goldLoc.X << "," << goldLoc.Y << endl;
			if (!agentHasGold)
			{
				//cout << "Plot path to gold\n";
				actionList2 = searchEngine.FindPath (Location(curLoc.X,curLoc.Y), (Orientation) curDir, Location(goldLoc.X,goldLoc.Y), (Orientation) curDir);
				actionList.splice (actionList.end(), actionList2);
				actionList.push_back(GRAB);
				agentHasGold = 1;
			} // end if we don't yet have the gold, get to it and get it
			else
			{
				//cout << "plot the return trip\n";
				actionList2 = searchEngine.FindPath (Location(curLoc.X,curLoc.Y), (Orientation) curDir, Location(1,1), LEFT);
				actionList.splice (actionList.end(), actionList2);
				actionList.push_back(CLIMB);
			} // if we do already have the gold, get back and get out:
		} // end findPath to gold

			/***   ***   ***   ***   *** END ***   ***   ***  ***   ***/

					/* end if we know the location of the gold, go to it, get it, get back, get out  */

			/*****************************BEGIN****************************/

			// get a list of possible moves:

		// evaluate up

		if (curLoc.Y < 4 && curLoc.Y > 0) // between 1 and 3
		{
			possibleMove.X = (curLoc.X);
			possibleMove.Y = (curLoc.Y + 1);

			// check if location contains a wumpus:

			if (possibleMove.X == wumpusLoc.X && possibleMove.Y == wumpusLoc.Y)
			{
				safeFlag = 0;
			} // end check if location contains a wumpus

			// check if location contains a pit:
			for (list<Location>::iterator it = pitList.begin (); it != pitList.end (); it++)
			{
				if (it->X == possibleMove.X and it->Y == possibleMove.Y)
				{
					safeFlag = 0;
				} // end if location is a pit, set safeFlag to zero
			} // end iterate through pit list to check if move location is a pit

			if (safeFlag == 1)
			{
				// only add if it's not already in the list:
				int alreadyFlag = 0;
				for (list <Location>::iterator it = possibleMoveList.begin (); it != possibleMoveList.end (); it++)
				{
					if (possibleMove.X == it->X && possibleMove.Y == it->Y)
					{
						alreadyFlag = 1;
					} // end if already in list, set flag
				} // end iterate through list to check if postion is already in it
				if (alreadyFlag == 0)
				{
					possibleMoveList.push_back (possibleMove);
				} // end only add if not already in list

				//cout << "added: " << possibleMove.X << "," << possibleMove.Y << endl;
			} // end if move is not a pit or wumpus, add to list of possible moves
		} // end evaluate up

		// evaluate down
		if (curLoc.Y > 1 && curLoc.Y < 5)
		{
			possibleMove.X = (curLoc.X);
			possibleMove.Y = (curLoc.Y - 1);

			// check if location contains a wumpus:

			if (possibleMove.X == wumpusLoc.X && possibleMove.Y == wumpusLoc.Y)
			{
				safeFlag = 0;
			} // end check if location contains a wumpus

			// check if location contains a pit:
			for (list<Location>::iterator it = pitList.begin (); it != pitList.end (); it++)
			{
				if (it->X == possibleMove.X and it->Y == possibleMove.Y)
				{
					safeFlag = 0;
				} // end if location is a pit, set safeFlag to zero
			} // end iterate through pit list to check if move location is a pit

			if (safeFlag == 1)
			{
				// only add if it's not already in the list:
				int alreadyFlag = 0;
				for (list <Location>::iterator it = possibleMoveList.begin (); it != possibleMoveList.end (); it++)
				{
					if (possibleMove.X == it->X && possibleMove.Y == it->Y)
					{
						alreadyFlag = 1;
					} // end if already in list, set flag
				} // end iterate through list to check if postion is already in it
				if (alreadyFlag == 0)
				{
					possibleMoveList.push_back (possibleMove);
				} // end only add if not already in list
				//cout << "added: " << possibleMove.X << "," << possibleMove.Y << endl;
			} // end if move is not a pit or wumpus, add to list of possible moves
		} // end evaluate down

		// evaluate left
		if (curLoc.X > 1 && curLoc.X < 5)
		{
			possibleMove.X = (curLoc.X - 1);
			possibleMove.Y = (curLoc.Y);

			// check if location contains a wumpus:

			if (possibleMove.X == wumpusLoc.X && possibleMove.Y == wumpusLoc.Y)
			{
				safeFlag = 0;
			} // end check if location contains a wumpus

			// check if location contains a pit:

			for (list<Location>::iterator it = pitList.begin (); it != pitList.end (); it++)
			{
				if (it->X == possibleMove.X and it->Y == possibleMove.Y)
				{
					safeFlag = 0;
				} // end if location is a pit, set safeFlag to zero
			} // end iterate through pit list to check if move location is a pit

			if (safeFlag == 1)
			{
				// only add if it's not already in the list:
				int alreadyFlag = 0;
				for (list <Location>::iterator it = possibleMoveList.begin (); it != possibleMoveList.end (); it++)
				{
					if (possibleMove.X == it->X && possibleMove.Y == it->Y)
					{
						alreadyFlag = 1;
					} // end if already in list, set flag
				} // end iterate through list to check if postion is already in it
				if (alreadyFlag == 0)
				{
					possibleMoveList.push_back (possibleMove);
				} // end only add if not already in list
				//cout << "added: " << possibleMove.X << "," << possibleMove.Y << endl;
			} // end if move is not a pit or wumpus, add to list of possible moves
		} // end evaluate left

		// evaluate right
		safeFlag = 1;
		if (curLoc.X < 4 && curLoc.X > 0)
		{
			possibleMove.X = (curLoc.X + 1);
			possibleMove.Y = (curLoc.Y);
			//cout << "evaluating: " << possibleMove.X << "," << possibleMove.Y << endl;

			// check if location contains a wumpus:

			if (possibleMove.X == wumpusLoc.X && possibleMove.Y == wumpusLoc.Y)
			{
				safeFlag = 0;
			} // end check if location contains a wumpus

			// check if location contains a pit:
			for (list<Location>::iterator it = pitList.begin (); it != pitList.end (); it++)
			{
				if (it->X == possibleMove.X and it->Y == possibleMove.Y)
				{
					safeFlag = 0;
				} // end if location is a pit, set safeFlag to zero
			} // end iterate through pit list to check if move location is a pit

			if (safeFlag == 1)
			{
				// only add if it's not already in the list:
				int alreadyFlag = 0;
				for (list <Location>::iterator it = possibleMoveList.begin (); it != possibleMoveList.end (); it++)
				{
					if (possibleMove.X == it->X && possibleMove.Y == it->Y)
					{
						alreadyFlag = 1;
					} // end if already in list, set flag
				} // end iterate through list to check if postion is already in it
				if (alreadyFlag == 0)
				{
					possibleMoveList.push_back (possibleMove);
				} // end only add if not already in list
				//cout << "added: " << possibleMove.X << "," << possibleMove.Y << endl;
			} // end if move is not a pit or wumpus, add to list of possible moves
		} // end evaluate right

			/***   ***   ***   ***   *** END add to list of possible moves ***   ***   ***  ***   ***/


		for (list <Location>::iterator it = possibleMoveList.begin (); it != possibleMoveList.end (); it++)
		{
			//cout << "possible: " << it->X << "," << it->Y << endl;
		}
		// we now have a list of potential moves, search list for latest unexplored loation and go to it
		int visited = 0;
		Location nextLocation, nextUnexploredLocation;
		nextLocation.X = 0;
		nextLocation.Y = 0;
		nextUnexploredLocation.X = 0;
		nextUnexploredLocation.Y = 0;

		for (list <Location>::iterator it = possibleMoveList.begin (); it != possibleMoveList.end (); it++)
		{
			for (list <Location>::iterator it2 = searchEngine.safeLocations.begin (); it2 != searchEngine.safeLocations.end (); it2++)
			{
				if (*it == *it2)
				{
					visited = 1;
				} // end if location is in safe list, it has been visited

			} // end iterate through safe list looking for location

			if (visited == 1)
			{
				nextLocation.X = it->X;
				nextLocation.Y = it->Y;
			} // if the location has been visited, add as nextLocation
			else
			{
				nextUnexploredLocation.X = it->X;
				nextUnexploredLocation.Y = it->Y;
				//cout << "next unexplored: " << it->X << "," << it->Y << endl;
			} // if the location has not been visited, add as nextUnexploredLocation
			visited = 0;
		} // end check each location in possible list for visited/unexplored

		// favor unexplored locations over explored:
		if (nextUnexploredLocation.X != 0)
		{
			nextLocation.X = nextUnexploredLocation.X;
			nextLocation.Y = nextUnexploredLocation.Y;
		} // end if there is an unexplored location, set it to nextLocation

		// we need to remove the location we're going to from the possible Move list
		// but we have to handle the last element differently than not-last elements

		list <Location>::iterator it = possibleMoveList.end ();
		if (it->X == nextLocation.X && it->Y == nextLocation.Y)
		{
			possibleMoveList.pop_back ();
		} // end if it's at the end, do pop_back
		else
		{
			for (list <Location>::iterator it = possibleMoveList.begin (); it != possibleMoveList.end (); it++)
			{
				if (it->X == nextLocation.X && it->Y == nextLocation.Y)
				{
					possibleMoveList.erase (it);
					// once we delete our record, no reason to continue iterating through list, get out
					break;
				} // end if it's at the end, do erase

			} // end iterate through list to find element to erase

		} // end else, iterate to location and erase


		//cout << "Goto: " << nextLocation.X << "," << nextLocation.Y << endl;
		searchEngine.AddSafeLocation (nextLocation.X, nextLocation.Y);

		actionList2 = searchEngine.FindPath (Location(curLoc.X,curLoc.Y), (Orientation) curDir, Location(nextLocation.X,nextLocation.Y), (Orientation) curDir);
		actionList.splice (actionList.end(), actionList2);
		if (goldLoc.X == 0 && percept.Stench == 1)
		lastPercept = 2;
		if (goldLoc.X == 0 && percept.Breeze == 1)
		lastPercept = 1;
		if (goldLoc.X == 0 && percept.Breeze == 0 && percept.Stench == 0)
		lastPercept = 0;

	} // end if action list is empty

	action = actionList.front();
	actionList.pop_front();
	if (goldLoc.X == 0 && percept.Stench == 1)
	lastPercept = 2;
	if (goldLoc.X == 0 && percept.Breeze == 1)
	lastPercept = 1;
	if (goldLoc.X == 0 && percept.Breeze == 0 && percept.Stench == 0)
	lastPercept = 0;
		// adjust curLoc appropriately:

	if (action == GOFORWARD)
	{
		if (curDir == 0)
		{
			if (curLoc.X < 4)
			{
				curLoc.X++;
			}
		}
		if (curDir == 1)
		{
			if (curLoc.Y < 4)
			{
				curLoc.Y++;
			}
		}
		if (curDir == 2)
		{
			if (curLoc.X > 1)
			{
				curLoc.X--;
			}
		}
		if (curDir == 3)
		{
			if (curLoc.Y > 1)
			{
				curLoc.Y--;
			}
		}
	} // end if action == GOFORWARD
	else if (action == TURNLEFT)
	{
		curDir++;
		curDir = curDir % 4;
	}
	else if (action == TURNRIGHT)
	{
		curDir--;
		if (curDir == -1)
		{
			curDir = 3;
		}
	}

	for (list<Location>::iterator it = searchEngine.safeLocations.begin (); it != searchEngine.safeLocations.end (); it++)
	{
		//cout << it->X << "," << it->Y << "\n";
	} // end output all known pit locations

	return action;


} // end Process function

void Agent::GameOver (int score)
{

}

	// readFile function
int Agent::readFile ()
{
	string line;
	Location loc;

	// initialize fileCount to 0
	fileCount = 0;

	ifstream inputFile;
	inputFile.open ("saved.txt", ios::in);
	if (inputFile.is_open())
	{
		// first line is count:
		// if the file has been used 10 times, disregard it
	getline (inputFile, line);
	fileCount = int (line[0] - 48);
	if (fileCount < 9)
	{
			// second line is the gold location:
		getline (inputFile, line);
		goldLoc.X = int (line[0] - 48);
		goldLoc.Y = int (line[2] - 48);
		//cout << "Gold\n";

			// second line is the wumpus location:
		getline (inputFile, line);
		wumpusLoc.X = int (line[0] - 48);
		wumpusLoc.Y = int (line[2] - 48);
		//cout << "wumpus\n";

			// remaining locations until "Safe" are pit locations
		int flag = 0;
		while (flag == 0)
		{
			getline (inputFile, line);
			if (line[0] == 's')
			{
				flag = 1;
				getline (inputFile, line);
			} // end if we encounter the letter s, stop reading
			else
			{
				loc.X = int (line[0] - 48);
				loc.Y = int (line[2] - 48);
				//cout << "pit\n";

				pitList.push_back (loc);
			} // if not "safe" keyword, add to pit list

		} // end add known pit locations to pitList

			// locations until "possible" are known safe locations

		flag = 0;
		while (flag == 0)
		{
			if (line[0] == 'p')
			{
				flag = 1;
			} // end if possible found
			if (flag == 0)
			{
				loc.X = int (line[0] - 48);
				loc.Y = int (line[2] - 48);
				//cout << "safe\n";

				searchEngine.safeLocations.push_back (loc);
			} // end if not possible flag
			getline (inputFile, line);

			} // end add known safe locations

			// locations until "EOF" are possible locations

		flag = 0;
		while (flag == 0)
		{
			if (line[0] == 'E')
			{
				flag = 1;
			} // end if EOF found
			if (flag == 0)
			{
				loc.X = int (line[0] - 48);
				loc.Y = int (line[2] - 48);
				//cout << "possible\n";

				possibleMoveList.push_back (loc);
			} // end if not EOF flag

			getline (inputFile, line);

		} // end add known safe locations
	} // end only read the rest of the file if count is less than 9

	} // end if there is a save file, read from it:

	inputFile.close ();

	return fileCount;

} // end function readFile

// Remove given location from safeLocations list
void Agent::removePotentialPit (list <Location> potentialPit, Location location)
{
	//for (list<Location>::iterator it = searchEngine.safeLocations.begin (); it != searchEngine.safeLocations.end (); it++)
	list<Location>::iterator itr = findLocation (potentialPit.begin(), potentialPit.end(), location);
	if (itr != potentialPit.end())
	{
		potentialPit.erase(itr);
		////cout << "Removed safe location (" << x << "," << y << ")\n";
	}
}

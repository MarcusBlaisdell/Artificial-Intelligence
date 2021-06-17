// Agent.cc

#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include <string.h>
#include <math.h>
#include "Agent.h"

using namespace std;

Agent::Agent ()
{

} // end Agent constructor

Agent::~Agent ()
{
	ofstream outputFile;
	LocPlus storeVal;

	cout << "in ~, normalExit = " << normalExit << " - curLoc = " << curLoc.X << "," << curLoc.Y << endl;
	//cout << "X: " << curLoc.X << " Y: " << curLoc.Y << endl;
	if (normalExit == 0)
	{
		cout << "normalExit == 0 : Location: " << curLoc.X << "," << curLoc.Y << " - lastPercept = : " << lastPercept << endl;
		//cout << "normalExit is zero, lastPercept = " << lastPercept << "\n";
			// if we died by pit:
		if (lastPercept == 1)
		{
			storeVal.theLoc.X = curLoc.X;
			storeVal.theLoc.Y = curLoc.Y;
			storeVal.theExists = true;
			storeVal.theProb = 1.0;
			int storeFlag = 0;

			for (list<LocPlus>::iterator itl = known.begin (); itl != known.end(); itl++)
			{
				if (itl->theLoc.X == storeVal.theLoc.X && itl->theLoc.Y == storeVal.theLoc.Y)
				{
					itl->theExists = true;
					itl->theProb = 1.0;
					storeFlag = 1;
					break;
				} // end if already in known, change probability to 1.0
			} // end iterate through known to check if location is already in known
			if (storeFlag == 0)
			{
				known.push_back (storeVal);
				removeUnvisited (curLoc);
			} // end if not already in known, add it

			removeFromFrontier (storeVal);

			//checkFrontier (storeVal);
			//addBreeze (storeVal);

			searchEngine.RemoveSafeLocation(curLoc.X,curLoc.Y);
		} // end if
			// if we died by wumpus:
		else if (lastPercept == 2)
		{

			storeVal.theLoc.X = curLoc.X;
			storeVal.theLoc.Y = curLoc.Y;
			storeVal.theExists = false;
			storeVal.theProb = 0.0;
			int storeFlag = 0;

			for (list<LocPlus>::iterator itl = known.begin (); itl != known.end(); itl++)
			{
				if (itl->theLoc.X == storeVal.theLoc.X && itl->theLoc.Y == storeVal.theLoc.Y)
				{
					itl->theExists = false;
					itl->theProb = 0.0;
					storeFlag = 1;
					break;
				} // end if already in known, change probability to 1.0
			} // end iterate through known to check if location is already in known
			if (storeFlag == 0)
			{
				known.push_back (storeVal);
				removeUnvisited (curLoc);
				cout << "added wumpusLoc as no pit" << endl;
			} // end if not already in known, add it

			//cout << "wumpus at this location" << endl;
			wumpusLoc = curLoc;
			searchEngine.RemoveSafeLocation(curLoc.X,curLoc.Y);

				// remove from the frontier
			removeFromFrontier (storeVal);

		} // end else
	} // end if not normalExit

	// Output knowledge base to file:
	// format:
	// Line 1: file count, number of times file has been used
	// Line 2: Gold location, 0,0 if unknown
	// Line 3: Wumpus location, 0,0 if unknown
	// Line 4 until "safe" are known safe locations
	// Line after "safe" to "known",  pit information
	// Line after "known" to "breeze", breeze information
	// Line after "breeze" to "frontier", frontier information
	// Line after "frontier" to "possible", possible moves
	// "EOF"

	outputFile.open ("saved.txt", ios::out);
	fileCount++;
	cout << "fileCount: " << fileCount << endl;
	fileCount %= 11; // reset every ten iterations
	cout << "fileCount: " << fileCount << endl;
		// line 1:
	outputFile << fileCount << "\n";
	//cout << "fileCount: " << fileCount << endl;
		// line 2:
	outputFile << goldLoc.X << "," << goldLoc.Y << "\n";
		// line 3:
	outputFile << wumpusLoc.X << "," << wumpusLoc.Y << "\n";

		// line 4 - "safe"

	for (list<Location>::iterator it = searchEngine.safeLocations.begin (); it != searchEngine.safeLocations.end (); it++)
	{
		outputFile << it->X << "," << it->Y << "\n";
	} // end output all known pit locations

	outputFile << "safe\n";

	for (list<LocPlus>::iterator it = known.begin (); it != known.end (); it++)
	{
		outputFile << it->theLoc.X << "," << it->theLoc.Y << "," << it->theExists << "," << it->theProb << "\n";

	} // end output all known pit locations

	outputFile << "known\n";

	for (list<LocPlus>::iterator it = breezeList.begin (); it != breezeList.end (); it++)
	{
		outputFile << it->theLoc.X << "," << it->theLoc.Y << "," << it->theExists << "," << it->theProb << "\n";

	} // end output all breeze information

	outputFile << "breeze\n";

	for (list<LocPlus>::iterator it = frontier.begin (); it != frontier.end (); it++)
	{
		outputFile << it->theLoc.X << "," << it->theLoc.Y << "," << it->theExists << "," << it->theProb << "\n";

	} // end output all frontier locations

	outputFile << "frontier\n";

	for (list<LocPlus>::iterator it = possibleMoveList.begin (); it != possibleMoveList.end (); it++)
	{
		outputFile << it->theLoc.X << "," << it->theLoc.Y  << "," << it->theExists << "," << it->theProb << "\n";
	} // end output all known pit locations

	outputFile << "possible\n";

	outputFile << "EOF\n";
	outputFile.close ();

		// print probabilities for all locations:

	printProbabilities ();

	/*
	cout << "Probabilities:" << endl;
	cout << "x, y, P(x,y)" << endl;
	for (list<LocPlus>::iterator itv = known.begin (); itv != known.end (); itv++)
	{
		cout << itv->theLoc.X << " " << itv->theLoc.Y << " " << itv->theProb << endl;
	} // end print out probability of all locations in known

	for (list<LocPlus>::iterator itv = unvisited.begin (); itv != unvisited.end (); itv++)
	{
		cout << itv->theLoc.X << " " << itv->theLoc.Y << " " << itv->theProb << endl;
	} // end print out probability of all locations in unvisited
	*/

} // end Agent deconstructor

void Agent::Initialize ()
{
	ifstream inputFile;
	string line;
	list <Action> actionList2;
	int x, y;
	int a = 1, b = 1;
	LocPlus unVisited;

		// initialize all locations as unvisited
		// with initial pit probability = 0.2:

	for (y = 1; y <= 4; y++)
	{
		for (x = 1; x <= 4; x++)
		{
			unVisited.theLoc.X = x;
			unVisited.theLoc.Y = y;
			unVisited.theExists = true;
			unVisited.theProb = 0.2;

			unvisited.push_back(unVisited);
		} // end inner for loop
	} // end outer for loop

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
	agentHasGold = false;
	actionList.clear();

		// read stored data from file:
	//cout << "calling readfile:\n";
	fileCount = readFile ();

	printKnown ();
	printFrontier ();
	printUnvisited ();

} // end initizize

Action Agent::Process (Percept& percept)
{
	Action action;
		// vector list of Actions
	list<Action> actionList2;
	Location theGoto;
		// Flag variable to test if an element exists in a list
	int exists = 0;
	int safeFlag = 1;

		// reset normalExit flag to zero
	normalExit = 0;
		// variables to store possible moves:
	Location possibleMove;
	LocPlus p_up, p_down, p_left, p_right;
	LocPlus theValue;
	LocPlus curLocRemove;
	LocPlus curLocPlus;
	int leave = 0; 	// if 1, leave game
	Location leaveLoc;
		// if the leave flag sets to 1, we will update this to the home location
	leaveLoc.X = 0;
	leaveLoc.Y = 0;

		/*****************************BEGIN****************************/

		// if we are here, this is a safe location
		// if it has not been added to safeLocations, add it:

	checkSafe (curLoc);
	removeUnvisited (curLoc);
	printKnown ();


		// add location to breeze list, if we detect a breeze,
		// add it to the breeze list as true
		// if we don't detect a breeze, add as false

	if (percept.Breeze == 1)
	{
		checkBreeze (curLoc, true);
	} // end if we detect a breeze
	else
	{
		checkBreeze (curLoc, false);
	} // end if we don't detect a breeze


		/***   ***   ***   ***   *** END Add safe location ***   ***   ***  ***   ***/

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

			// if we are at home position with the gold, exit the game: (c)
			// ***** I don't think I need to evaluate direction, if we have the gold and are not at 1,1, direction is irrelevant
		//if (agentHasGold && curLoc.X == 1 && curLoc.Y == 1 && (curDir == 2 || curDir == 3))
		if (agentHasGold && curLoc.X == 1 && curLoc.Y == 1)
		{
			action = CLIMB;
			normalExit = 1;
			return action;
		} // end if home with gold, climb

		/***   ***   ***   ***   *** END if home with gold, exit ***   ***   ***  ***   ***/

		/*****************************BEGIN****************************/

	if (actionList.empty())
	{
		cout << "actionList is empty\n";

																/*** Begin convoluted move evaluations ***/

			/*****************************BEGIN****************************/

			// If we know the location of the gold, get to it, get it, get back, get out (d)

		if (goldLoc.X != 0)
		{
			cout << "Gold location is known\n";
			//cout << goldLoc.X << "," << goldLoc.Y << endl;
			//cout << curLoc.X << "," << curLoc.Y << endl;
			//printSafe ();
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
				cout << "plot the return trip\n";
				actionList2 = searchEngine.FindPath (Location(curLoc.X,curLoc.Y), (Orientation) curDir, Location(1,1), LEFT);
				actionList.splice (actionList.end(), actionList2);
				actionList.push_back(CLIMB);
			} // if we do already have the gold, get back and get out:
		} // end findPath to gold

			/***   ***   ***   ***   *** END ***   ***   ***  ***   ***/

					/* end if we know the location of the gold, go to it, get it, get back, get out  */

			/*****************************BEGIN****************************/

					/* If we don't know the location of the gold, plan our search: */


			// get a list of possible moves:


			// calculate probabilities on the Frontier:
		curLocPlus.theLoc.X = curLoc.X;
		curLocPlus.theLoc.Y = curLoc.Y;
		curLocPlus.theExists = false;
		curLocPlus.theProb = 0.0;

		checkFrontier (curLocPlus);
		removeFromFrontier (curLocPlus);
		printFrontier ();

		calculateFrontier ();
			// if all probabilities in the frontier are greater than 0.5:
			// leave the game

		leave = testFrontier ();
		if (leave == 1)
		{
			actionList2 = searchEngine.FindPath (Location(curLoc.X,curLoc.Y), (Orientation) curDir, Location(1,1), (Orientation) curDir);
			actionList.splice (actionList.end(), actionList2);
			actionList.push_back(CLIMB);
		} // end if all frontier probabilities > 0.5

		addToPossibles (curLocPlus);

			/***   ***   ***   ***   *** END add to list of possible moves ***   ***   ***  ***   ***/

		if (possibleMoveList.size () > 0)
		{
			for (list <LocPlus>::iterator it = possibleMoveList.begin (); it != possibleMoveList.end (); it++)
			{
				cout << "possible: " << it->theLoc.X << "," << it->theLoc.Y << " : " << it->theProb << endl;
			} // print list of possible moves

		} // end only iterate through possibleMoveList if it's not empty
		else
		{
				// the only way this should happen is if there are pits in 1,2 and 2,1, I think
			cout << "possibleMoveList is empty" << endl;
				// so:
			action = CLIMB;
			normalExit = 1;
			return action;
		} // end if possibleMoveList is empty


			// we now have a list of potential moves, search list for latest unexplored loation and go to it
		int visited = 0;
		LocPlus nextLocation, nextUnexploredLocation;

		nextLocation.theLoc.X = 0;
		nextLocation.theLoc.Y = 0;
		nextUnexploredLocation.theLoc.X = 0;
		nextUnexploredLocation.theLoc.Y = 0;
			//initialize probability to 1 so we will only choose values lower than
			// the last unexplored location
		nextUnexploredLocation.theProb = 1.0;

		if (possibleMoveList.size () > 0)
		{
			for (list <LocPlus>::iterator it = possibleMoveList.begin (); it != possibleMoveList.end (); it++)
			{
				for (list <Location>::iterator it2 = searchEngine.safeLocations.begin (); it2 != searchEngine.safeLocations.end (); it2++)
				{
					if (it->theLoc.X == it2->X && it->theLoc.Y == it2->Y)
					{
						visited = 1;
					} // end if location is in safe list, it has been visited

				} // end iterate through safe list looking for location

				if (visited == 1)
				{
					nextLocation.theLoc.X = it->theLoc.X;
					nextLocation.theLoc.Y = it->theLoc.Y;
				} // if the location has been visited, add as nextLocation
				else
				{
						// only choose as nextUnexploredLocation if probability is lower than
						// what we already selected
						// that is, prefer the lowest probability unexplored location:

					if (it->theProb < nextUnexploredLocation.theProb)
					{
						nextUnexploredLocation.theLoc.X = it->theLoc.X;
						nextUnexploredLocation.theLoc.Y = it->theLoc.Y;
						nextUnexploredLocation.theProb = it->theProb;
					}

					//cout << "next unexplored: " << it->X << "," << it->Y << endl;
				} // if the location has not been visited, add as nextUnexploredLocation
				visited = 0;
			} // end check each location in possible list for visited/unexplored
		} // end only evaluate if not empty


		cout << "nextLocation: " << nextLocation.theLoc.X << "," << nextLocation.theLoc.Y << endl;
		cout << "nextUnexploredLocation: " << nextUnexploredLocation.theLoc.X << "," << nextUnexploredLocation.theLoc.Y << endl;
		// favor unexplored locations over explored:
		if (nextUnexploredLocation.theLoc.X != 0 && nextUnexploredLocation.theProb < 0.5)
		{
			nextLocation.theLoc.X = nextUnexploredLocation.theLoc.X;
			nextLocation.theLoc.Y = nextUnexploredLocation.theLoc.Y;
		} // end if there is an unexplored location, set it to nextLocation

		// we need to remove the location we're going to from the possible Move list
		// but we have to handle the last element differently than not-last elements

		if (possibleMoveList.size () > 0)
		{
			list <LocPlus>::iterator it = possibleMoveList.end ();
			if (it->theLoc.X == nextLocation.theLoc.X && it->theLoc.Y == nextLocation.theLoc.Y)
			{
				possibleMoveList.pop_back ();
			} // end if it's at the end, do pop_back
			else
			{
				for (list <LocPlus>::iterator it = possibleMoveList.begin (); it != possibleMoveList.end (); it++)
				{
					if (it->theLoc.X == nextLocation.theLoc.X && it->theLoc.Y == nextLocation.theLoc.Y)
					{
						possibleMoveList.erase (it);
						// once we delete our record, no reason to continue iterating through list, get out
						break;
					} // end if it's at the end, do erase

				} // end iterate through list to find element to erase

			} // end else, iterate to location and erase
		} // end only run if list is not empty

		//cout << "Goto: " << nextLocation.X << "," << nextLocation.Y << endl;
		searchEngine.AddSafeLocation (nextLocation.theLoc.X, nextLocation.theLoc.Y);

		Location Destination;
		Destination.X = nextLocation.theLoc.X;
		Destination.Y = nextLocation.theLoc.Y;
		cout << "Destination: " << Destination.X << "," << Destination.Y << endl;

		if (leave == 0)
		{
			actionList2 = searchEngine.FindPath (Location(curLoc.X,curLoc.Y), (Orientation) curDir, Location(Destination.X,Destination.Y), (Orientation) curDir);
		}

		actionList.splice (actionList.end(), actionList2);

			// I can't recall why I am checking if goldLoc is not known
		/*
		if (goldLoc.X == 0 && percept.Stench == 1)
		lastPercept = 2;
		if (goldLoc.X == 0 && percept.Breeze == 1)
		lastPercept = 1;
		if (goldLoc.X == 0 && percept.Breeze == 0 && percept.Stench == 0)
		lastPercept = 0;
		*/

		if (percept.Stench == 1)
		lastPercept = 2;
		if (percept.Breeze == 1)
		lastPercept = 1;
		if (percept.Breeze == 0 && percept.Stench == 0)
		lastPercept = 0;

	} // end if action list is empty
	else
	{
		cout << "actionList is not empty" << endl;
	}


	action = actionList.front();
	actionList.pop_front();

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
		cout << "current position: " << curLoc.X << "," << curLoc.Y << endl;
		//LocPlus addCurrent;
		//addCurrent.theLoc.X = curLoc.X;
		//addCurrent.theLoc.Y = curLoc.Y;
		//addCurrent.theExists = false;
		//addCurrent.theProb = 0.0;

		//known.push_back (addCurrent);
		addToKnown (curLoc);
		removeUnvisited (curLoc);

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
	// end adjust curLoc appropriately

	return action;

} // end Process function

void Agent::GameOver (int score)
{

}

// readFile function

int Agent::readFile ()
{
	string line;
	char word[32];
	Location sLoc;
	LocPlus loc;
	LocPlus theValue;
	int i, j;
	string theFloat;
	float theProb;
	int flag;

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
		if (line[1] == '0')
		{
			fileCount = 10;
		}

		if (fileCount < 10)
		{
				// second line is the gold location:
			getline (inputFile, line);
			goldLoc.X = int (line[0] - 48);
			goldLoc.Y = int (line[2] - 48);
			//cout << "Gold\n";

				// third line is the wumpus location:
			getline (inputFile, line);
			wumpusLoc.X = int (line[0] - 48);
			wumpusLoc.Y = int (line[2] - 48);
			//cout << "wumpus\n";

/////
				// remaining locations until "safe" are safe locations
			flag = 0;
			while (flag == 0)
			{
				getline (inputFile, line);
				if (line[0] == 's')
				{
					flag = 1;
				} // end if we encounter the letter s, stop reading
				else
				{
					sLoc.X = int (line[0] - 48);
					sLoc.Y = int (line[2] - 48);

					searchEngine.AddSafeLocation (sLoc.X, sLoc.Y);

				} // if not "safe" keyword, add to safeLocations list

		} // end add safe locations to safeLocations
/////

				// remaining locations until "known" are pit locations
			flag = 0;
			while (flag == 0)
			{
				getline (inputFile, line);
				if (line[0] == 'k')
				{
					flag = 1;
				} // end if we encounter the letter k, stop reading
				else
				{
					loc.theLoc.X = int (line[0] - 48);
					loc.theLoc.Y = int (line[2] - 48);
					loc.theExists = int (line[4] - 48);
					if (line[6] == '0')
					{
						loc.theProb = 0;
					}
					if (line[6] == '1')
					{
						loc.theProb = 1;
					}
					/*
					i = 6;
					j = 0;
					while (line[i] != 0)
					{
						theFloat[j++] = line[i++];
					} // end while to get string
					theFloat[j] = 0;

					loc.theProb = strtoflt (theFloat);
					cout << "theFloat[0]: " << theFloat[0] << endl;
					cout << "theFloat: " << theFloat << endl;
					cout << "theProb: " << loc.theProb << endl;
					*/

					//pitList.push_back (loc);
					theValue.theLoc.X = loc.theLoc.X;
					theValue.theLoc.Y = loc.theLoc.Y;
					theValue.theExists = loc.theExists;
					theValue.theProb = loc.theProb;

					known.push_back (theValue);
					removeUnvisited (curLoc);

				} // if not "known" keyword, add to known list

			} // end add known locations to known


				// locations until "breeze" are breeze information

				flag = 0;
				while (flag == 0)
				{
					getline (inputFile, line);
					if (line[0] == 'b')
					{
						flag = 1;
					} // end if we encounter the letter b, stop reading
					else
					{
						loc.theLoc.X = int (line[0] - 48);
						loc.theLoc.Y = int (line[2] - 48);
						loc.theExists = int (line[4] = 48);
						i = 6;
						j = 0;
						while (line[i] != 0)
						{
							theFloat[j++] = line[i++];
						} // end while to get string

						loc.theProb = strtoflt (theFloat);

						theValue.theLoc.X = loc.theLoc.X;
						theValue.theLoc.Y = loc.theLoc.Y;
						theValue.theExists = loc.theExists;
						theValue.theProb = loc.theProb;

						breezeList.push_back (theValue);

					} // if not "breeze" keyword, add to known list

				} // end add breeze information


				flag = 0;
				while (flag == 0)
				{
					getline (inputFile, line);
					if (line[0] == 'f')
					{
						flag = 1;
					} // end if we encounter the letter f, stop reading
					else
					{
						loc.theLoc.X = int (line[0] - 48);
						loc.theLoc.Y = int (line[2] - 48);
						loc.theExists = int (line[4] = 48);
						i = 6;
						j = 0;
						while (line[i] != 0)
						{
							theFloat[j++] = line[i++];
						} // end while to get string

						loc.theProb = strtoflt (theFloat);

						//pitList.push_back (loc);
						theValue.theLoc.X = loc.theLoc.X;
						theValue.theLoc.Y = loc.theLoc.Y;
						theValue.theExists = loc.theExists;
						theValue.theProb = loc.theProb;

						frontier.push_back (theValue);

					} // if not "frontier" keyword, add to frontier list

				} // end add frontier information


					// add possibleMove information:

				flag = 0;
				while (flag == 0)
				{
					getline (inputFile, line);
					if (line[0] == 'p')
					{
						flag = 1;
					} // end if we encounter the letter p, stop reading
					else
					{
						loc.theLoc.X = int (line[0] - 48);
						loc.theLoc.Y = int (line[2] - 48);
						loc.theExists = int (line[4] = 48);
						i = 6;
						j = 0;
						while (line[i] != 0)
						{
							theFloat[j++] = line[i++];
						} // end while to get string

						loc.theProb = strtoflt (theFloat);

						theValue.theLoc.X = loc.theLoc.X;
						theValue.theLoc.Y = loc.theLoc.Y;
						theValue.theExists = loc.theExists;
						theValue.theProb = loc.theProb;

						possibleMoveList.push_back (theValue);

					} // if not "known" keyword, add to known list

				} // end add possibleMove information


		} // end only read the rest of the file if count is less than 10
		else
		{
			cout << "\t***" << endl;
			cout << "ignoring saved.txt" << endl;
			cout << "\t***" << endl;
		}

		} // end if there is a save file, read from it:

	inputFile.close ();

	return fileCount;

} // end function readFile


		// checkSafe function evaluates if a location is already in the safeLocations list
void Agent::checkSafe (Location curLoc)
{
	int exists = 0;
	LocPlus theValue;

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

			// add to known list:

		theValue.theLoc.X = curLoc.X;
		theValue.theLoc.Y = curLoc.Y;
		theValue.theExists = false;
		theValue.theProb = 0.0;
		known.push_back (theValue);
		removeUnvisited (curLoc);
		searchEngine.AddSafeLocation(curLoc.X, curLoc.Y);
		//cout << "Added: " << curLoc.X << "," << curLoc.Y << endl;

			// if location was not in safe list, this is the first time we have
			// visited it so run checkFrontier
		//checkFrontier (theValue);
		addToFrontier (theValue);


			// this location is now visited (if it wasn't already)
			// make sure to remove it from the frontier
		removeFromFrontier (theValue);
	} // end if location isn't already in safeLocations, add it (and check frontier)

	//return exists;

} // end checksafe function

// check if location contains a pit:

int Agent::checkPit (Location possibleMove)
{
	int safeFlag = 1;
	LocPlus theValue;

	theValue.theLoc.X = possibleMove.X;
	theValue.theLoc.Y = possibleMove.Y;
	theValue.theExists = true;

	for (list<LocPlus>::iterator it = known.begin (); it != known.end (); it++)
	{
		if (it->theLoc.X == theValue.theLoc.X && it->theLoc.Y == theValue.theLoc.Y && it->theExists == theValue.theExists)
		{
			safeFlag = 0;
		} // end if location is a pit, set safeFlag to zero

	} // end iterate through pit list to check if move location is a pit

	return safeFlag;

} // end checkPit function

	// checkBreeze function adds breeze information in visited locations to the breeze list
void Agent::checkBreeze (Location curLoc, bool truthness)
{
	int exists = 0;
	LocPlus theValue;
	theValue.theLoc.X = curLoc.X;
	theValue.theLoc.Y = curLoc.Y;
	theValue.theExists = truthness;

		// check if it's already in the list:

	for (list<LocPlus>::iterator it = breezeList.begin (); it != breezeList.end (); it++)
	{
	if (theValue.theLoc.X == it->theLoc.X && theValue.theLoc.Y == it->theLoc.Y && theValue.theExists == it->theExists)
	{
		exists = 1;
		//cout << curLoc.X << "," << curLoc.Y << " already exists\n";
	} // end if location is already in breezes list, set exists flag so we don't re-add it

	} // end iterate through breezeList to test if location is already in breezeList

	if (exists == 0)
	{
		breezeList.push_back (theValue);
	} // end if location isn't already in breezeList, add it

} // end checkBreeze function

	// evaluateAdjacent function

void Agent::evaluateAdjacent (LocPlus locEval)
{
	int exists = 0;

	for (list<LocPlus>::iterator it = known.begin (); it != known.end (); it++)
	{
		if (locEval.theLoc.X == it->theLoc.X && locEval.theLoc.Y == it->theLoc.Y)
		{
			exists = 1;
			return;
			//cout << curLoc.X << "," << curLoc.Y << " already exists\n";
		} // end if location is already in known list, set exists flag so we don't re-add it

	} // end iterate through known to test if location is already in known

	if (exists == 0)
	{
		for (list<LocPlus>::iterator it = frontier.begin (); it != frontier.end (); it++)
		{
			if (locEval.theLoc.X == it->theLoc.X && locEval.theLoc.Y == it->theLoc.Y && locEval.theExists == it->theExists)
			{
				exists = 1;
				return;
				//cout << curLoc.X << "," << curLoc.Y << " already exists\n";
			} // end if location is already in frontier list, set exists flag so we don't re-add it

		} // end iterate through frontier to test if location is already in frontier

	} // end check Frontier

	if (exists == 0)
	{
		locEval.theExists = true;
		locEval.theProb = 1.0;
		frontier.push_back (locEval);
	} // end if location isn't already in frontier, add it

} // end evaluateAdjacent function

	// checkFrontier function adds unvisited locations adjacent to visited locations

void Agent::checkFrontier (LocPlus theValue)
{
	LocPlus locEval;

	// check all possible moves from theValue,
	// if they are in the known list, we have already visited them, do nothing
	// if they are not in the known list but in the frontier, do nothing
	// if they are not in known, and not in the frontier, add to frontier:

	// up:
	if (theValue.theLoc.Y < 4)
	{
		locEval.theLoc.X = theValue.theLoc.X;
		locEval.theLoc.Y = theValue.theLoc.Y + 1;
		locEval.theExists = true;

		evaluateAdjacent (locEval);
	} // end if up

	// down:
	if (theValue.theLoc.Y > 1)
	{
		locEval.theLoc.X = theValue.theLoc.X;
		locEval.theLoc.Y = theValue.theLoc.Y - 1;
		locEval.theExists = true;

		evaluateAdjacent (locEval);
	} // end if down

	// left:
	if (theValue.theLoc.X > 1)
	{
		locEval.theLoc.X = theValue.theLoc.X - 1;
		locEval.theLoc.Y = theValue.theLoc.Y;
		locEval.theExists = true;

		evaluateAdjacent (locEval);
	} // end if left

	// right:
	if (theValue.theLoc.X < 4)
	{
		locEval.theLoc.X = theValue.theLoc.X + 1;
		locEval.theLoc.Y = theValue.theLoc.Y;
		locEval.theExists = true;

		evaluateAdjacent (locEval);
	} // end if right

} // end checkFrontier function

// Remove given location from frontier list
void Agent::removeFromFrontier (LocPlus theValue)
{
	list <LocPlus>::iterator it = frontier.end ();
	if (it->theLoc.X == theValue.theLoc.X && it->theLoc.Y == theValue.theLoc.Y)
	{
	  frontier.pop_back ();
		return;
	} // end if it's at the end, do pop_back
	else
	{
		for (list<LocPlus>::iterator itr = frontier.begin(); itr != frontier.end(); itr++)
		{
			if (theValue.theLoc.X == itr->theLoc.X && theValue.theLoc.Y == itr->theLoc.Y)
			{
				frontier.erase(itr);
				break;
				////cout << "Removed frontier location (" << x << "," << y << ")\n";
			} // end erase from not-the-end
		} // end for to iterate to the element we want to remove

	} // end if itr is not at the end

} // end removeFromFrontier function

	// addToFrontier function evaluates all possible moves from current location
	// and adds any unvisited locations

void Agent::addToFrontier (LocPlus curLoc)
{
	LocPlus theEvaluate;

	// check up from current location:
	if (curLoc.theLoc.Y < 4)
	{
		theEvaluate.theLoc.X = curLoc.theLoc.X;
		theEvaluate.theLoc.Y = curLoc.theLoc.Y + 1;
		theEvaluate.theExists = true;

		//checkFrontier (theEvaluate);
		evaluateAdjacent (theEvaluate);

	} // end test up from current location

	// check down from current location:
	if (curLoc.theLoc.Y > 1)
	{
		theEvaluate.theLoc.X = curLoc.theLoc.X;
		theEvaluate.theLoc.Y = curLoc.theLoc.Y - 1;
		theEvaluate.theExists = true;

		//checkFrontier (theEvaluate);
		evaluateAdjacent (theEvaluate);

	} // end test down from current location

	// check up from current location:
	if (curLoc.theLoc.X > 1)
	{
		theEvaluate.theLoc.X = curLoc.theLoc.X - 1;
		theEvaluate.theLoc.Y = curLoc.theLoc.Y;
		theEvaluate.theExists = true;

		//checkFrontier (theEvaluate);
		evaluateAdjacent (theEvaluate);

	} // end test left from current location

	// check right from current location:
	if (curLoc.theLoc.X < 4)
	{
		theEvaluate.theLoc.X = curLoc.theLoc.X + 1;
		theEvaluate.theLoc.Y = curLoc.theLoc.Y;
		theEvaluate.theExists = true;

		//checkFrontier (theEvaluate);
		evaluateAdjacent (theEvaluate);

	} // end test right from current location

	return;

} // end addToFrontier function

	// calculateFrontier function
	// calculate the probability of a pit in location

void Agent::calculateFrontier ()
{
	float P_True, P_False;			// the probability of true and probability of false
	int TrueCount, FalseCount;	// number of Trues and Falses in a combination
	float P_Prime = 0.0;				// The probability of frontierPrime
	int goodFlag = 1;						// if a combination is valid, goodFlag == 1 when it reaches the end
	bool QueryTruth;						// store the value of the query truthness

		// if there is only one element in our frontier,
		// this can only happen one way, we have visited
		// every position except one corner
		// run cornerCase evaluation:




	if (frontier.size () == 1)
	{
		cornerCase (curLoc);
		//list<LocPlus>::iterator itx = frontier.begin();
    //itx->theProb = 0.2;
    return;

	} // end if there is only one elment, just set to 0.2 and return


		// for each location (x,y) in frontier:

	for (list<LocPlus>::iterator it = frontier.begin (); it != frontier.end (); it++)
	{
		P_True = 0.0;
		P_False = 0.0;
		TrueCount = 0;
		FalseCount = 0;

		Query.theLoc.X = it->theLoc.X;
		Query.theLoc.Y = it->theLoc.Y;
		Query.theExists = true;

		frontierPrime = frontier;

			// use newFrontier to track the combination:
			// reset on each query:

		//newFrontier = frontier;

		/*
		cout << "\t\t\tQuery Location: " << Query.theLoc.X << "," <<Query.theLoc.Y << endl;
		cout  << "breeze list:" << endl;
		printBreeze ();
		*/

			// remove query location from frontierPrime:

			list <LocPlus>::iterator it2 = frontierPrime.end ();
			if (it2->theLoc.X == Query.theLoc.X && it2->theLoc.Y == Query.theLoc.Y)
			{
			  frontierPrime.pop_back ();
			} // end if it's at the end, do pop_back
			else
			{
				for (list<LocPlus>::iterator itr = frontierPrime.begin(); itr != frontierPrime.end(); itr++)
				{
					if (Query.theLoc.X == itr->theLoc.X && Query.theLoc.Y == itr->theLoc.Y)
					{
						if (itr != frontierPrime.end() )
						{
							frontierPrime.erase(itr);
							break;
						} // end only delete if not at end

					} // end remove query location from frontierPrime

				} // end for loop to find element to remove

			} // end element not at the end of the list


			// for each possible combination C of pit = true and pit = false in frontierPrime:
			// calculate P(frontierPrime)
			// tabulate P(Query=true) and P(Query=false)

			// Use a recursive call to get all combinations of frontierPrime:

			// do recursive call here for Query = true:

		QueryTruth = true;

		recursePrime (frontierPrime, &TrueCount, &FalseCount, goodFlag, &P_True, &P_False, &P_Prime, &QueryTruth);

			// repeat for Query == false:
		Query.theExists = false;
		QueryTruth = false;
		//TrueCount--;
		//FalseCount++;

		recursePrime (frontierPrime, &TrueCount, &FalseCount, goodFlag, &P_True, &P_False, &P_Prime, &QueryTruth);

		P_True = P_True * 0.2;
		P_False = P_False * 0.8;

			// Don't divide by zero

		cout << "P_True = " << P_True << endl;
		cout << "P_False = " << P_False << endl;
		if ((P_True + P_False) > 0)
		{
			P_True = (P_True / (P_True + P_False) );
		} // end only update P_True if we won't divide by zero
		cout << "P_True = " << P_True << endl;

		//cout << "P_True: " << it->theLoc.X << "," << it->theLoc.Y << " = " << P_True << endl;

		it->theProb = P_True;
		//it->theProb = P_True * 10;


	} // end for each location in frontier

} // end calculateFrontier function


// recursePrime function

void Agent::recursePrime (list <LocPlus> doublePrime, int *TrueCount, int *FalseCount,
													int goodFlag, float *P_True, float *P_False, float *P_Prime, bool *QueryTruth)
{
	LocPlus theElement;
	int consistent = 0, localFlag = goodFlag;

	//cout << "recursePrime called" << endl;

		// base case, there is only one element in the list:

	if (doublePrime.size () == 1)
	{
			// first, evaluate as true:

		list<LocPlus>::iterator it = doublePrime.begin ();
		it->theExists = true;
		*TrueCount = *TrueCount + 1;
		theElement.theLoc.X = it->theLoc.X;
		theElement.theLoc.Y = it->theLoc.Y;
		//cout << "newFrontier.size (): " << newFrontier.size () << "  : TruCount + FalseCount: " << *TrueCount + *FalseCount << endl;

			// update frontierPrime to have the correct truth value for this element:

		//for (list<LocPlus>::iterator itu = newFrontier.begin(); itu != newFrontier.end();itu++)
		for (list<LocPlus>::iterator itu = frontierPrime.begin(); itu != frontierPrime.end();itu++)
		{
			if (itu->theLoc.X == theElement.theLoc.X && itu->theLoc.Y == theElement.theLoc.Y)
			{
				itu->theExists = true;
				break;
			} // end update value to true

		} // end iterate through frontierPrime to update element

		consistent = evaluateBreeze ();

			// if both goodFlag and consistent are 1, we calculate P_Prime,
			// if either is zero, we don't

		if (consistent == 1)
		{
			cout << "TRUE: *TrueCount: " << *TrueCount << "  *FalseCount: " << *FalseCount << endl;
			cout << "TRUE-1: *P_True: " << *P_True << "  *P_Prime: " << *P_Prime << endl;
			*P_Prime = pow(0.2, *TrueCount) * pow(0.8, *FalseCount);
			*P_True = *P_True + *P_Prime;
			cout << "TRUE-2: *P_True: " << *P_True << "  *P_Prime: " << *P_Prime << endl;

		} // end for when last element is true

			// repeat for last element is false:

		it->theExists = false;
		*TrueCount = *TrueCount - 1;
		*FalseCount = *FalseCount + 1;

		for (list<LocPlus>::iterator itu = frontierPrime.begin(); itu != frontierPrime.end();itu++)
		{
			if (itu->theLoc.X == theElement.theLoc.X && itu->theLoc.Y == theElement.theLoc.Y)
			{
				itu->theExists = false;
			} // end update value to false

		} // end iterate through newFrontier to update element

		consistent = evaluateBreeze ();

		localFlag = consistent * goodFlag;

		if (consistent == 1)
		{
			cout << "FALSE: *TrueCount: " << *TrueCount << "  *FalseCount: " << *FalseCount << endl;
			cout << "FALSE-1: *P_False: " << *P_False << "  *P_Prime: " << *P_Prime << endl;
			*P_Prime = pow(0.2, *TrueCount) * pow(0.8, *FalseCount);
			*P_False = *P_False + *P_Prime;
			cout << "FALSE-2: *P_False: " << *P_False << "  *P_Prime: " << *P_Prime << endl;

		} // end for when last element is false

		*FalseCount = *FalseCount - 1;

	} // end if only one element in list

	// if this is not the last element in our list:
	// dequeue one element, process it, call the recursive function
	// with the remaining list

	else
	{
			// remove first element from list:

		list<LocPlus>::iterator it = doublePrime.begin();
		theElement.theLoc.X = it->theLoc.X;
		theElement.theLoc.Y = it->theLoc.Y;
		doublePrime.erase (it);

			// run for true:

			//for (list<LocPlus>::iterator itu = newFrontier.begin(); itu != newFrontier.end();itu++)
			for (list<LocPlus>::iterator itu = frontierPrime.begin(); itu != frontierPrime.end();itu++)
			{
				if (itu->theLoc.X == theElement.theLoc.X && itu->theLoc.Y == theElement.theLoc.Y)
				{
					itu->theExists = true;
					*TrueCount = *TrueCount + 1;
				} // end update value to true
			} // end iterate through newFrontier to update element

		recursePrime (doublePrime, TrueCount, FalseCount, goodFlag, P_True, P_False, P_Prime, QueryTruth);

			// repeat for false:

		for (list<LocPlus>::iterator itu = frontierPrime.begin(); itu != frontierPrime.end();itu++)
		{
			if (itu->theLoc.X == theElement.theLoc.X && itu->theLoc.Y == theElement.theLoc.Y)
			{
				itu->theExists = false;
				*TrueCount = *TrueCount - 1;
				*FalseCount = *FalseCount + 1;
			} // end update value to true
		} // end iterate through newFrontier to update element

		recursePrime (doublePrime, TrueCount, FalseCount, goodFlag, P_True, P_False, P_Prime, QueryTruth);

		*FalseCount = *FalseCount - 1;
	} // end else, not last element in list

} // end recursePrime function

	// evaluateBreeze function
int Agent::evaluateBreeze ()
{
	LocPlus b_loc, testLoc;
	int consistentFlag = 0, persistantFlag = 0, consistentCount = 0, checkCount = 0;

	//cout << "\t\t\tEvaluteBreeze called" << endl;

		// This will evaluate the Breeze information against the known data, frontier data, and Query location

		// for each location in the breezeList:

	for (list<LocPlus>::iterator itb = breezeList.begin(); itb != breezeList.end(); itb++)
	{
		b_loc.theLoc.X = itb->theLoc.X;
		b_loc.theLoc.Y = itb->theLoc.Y;
		b_loc.theExists = itb->theExists;
		testLoc.theExists = itb->theExists;

		if (b_loc.theLoc.Y < 4)
		{
			testLoc.theLoc.X = b_loc.theLoc.X;
			testLoc.theLoc.Y = b_loc.theLoc.Y + 1;

			consistentFlag = consistentCheck (testLoc, b_loc.theExists);
			consistentCount++;
			persistantFlag = persistantFlag + consistentFlag;

		} // end check up

		if (b_loc.theLoc.Y > 1)
		{
			testLoc.theLoc.X = b_loc.theLoc.X;
			testLoc.theLoc.Y = b_loc.theLoc.Y - 1;

			consistentFlag = consistentCheck (testLoc, b_loc.theExists);
			consistentCount++;
			persistantFlag = persistantFlag + consistentFlag;

		} // end check down

		if (b_loc.theLoc.X > 1)
		{
			testLoc.theLoc.X = b_loc.theLoc.X - 1;
			testLoc.theLoc.Y = b_loc.theLoc.Y;

			consistentFlag = consistentCheck (testLoc, b_loc.theExists);
			consistentCount++;
			persistantFlag = persistantFlag + consistentFlag;

		} // end check left

		if (b_loc.theLoc.X < 4)
		{
			testLoc.theLoc.X = b_loc.theLoc.X + 1;
			testLoc.theLoc.Y = b_loc.theLoc.Y;

			consistentFlag = consistentCheck (testLoc, b_loc.theExists);
			consistentCount++;
			persistantFlag = persistantFlag + consistentFlag;

		} // end check right

	} // end iterate through all breeze data points

		// if breeze is true, then any pit in any adjacent position
		// is consistent,
		// if breeze is false, then all pits in all adjacent positions
		// must not contain pits to be consistent
		// if the persistantFlag > 0, then at least one location
		// supported the test that there is a breeze,
		// persistantFlag must equal the number of checks
		// to support the test there is no breeze:

	if (b_loc.theExists == 1)
	{
		if (persistantFlag > 0)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}  // end if breeze is in this location
	else
	{
		if (persistantFlag == consistentCount)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	} // end if there is no breeze in this location



} // end evaluateBreeze function


	// consistentCheck function :
int Agent::consistentCheck (LocPlus testLoc, bool theTruth)
{
		// return 1 for consistent:
		// return 0 for non-consistent:

		// testLoc is a location adjacent to a known breeze location
		// theTruth is the truth value of the location we are evaluating

			//cout << endl << "Query: " << Query.theLoc.X << "," << Query.theLoc.Y << " : " << Query.theExists << endl;
			//cout << "test: " << testLoc.theLoc.X << "," << testLoc.theLoc.Y << " : " << testLoc.theExists << endl << endl;
		// if the test location is the query location, evaluate it:

	if (testLoc.theLoc.X == Query.theLoc.X && testLoc.theLoc.Y == Query.theLoc.Y)
	{
		if (theTruth == Query.theExists)
		{
			return 1;
		}	// end if Query location is consistent
		else
		{
			return 0;
		} // end if Query location is not consistent
	} // end if test location is the query location


		// Check Known:

	for (list<LocPlus>::iterator itc = known.begin(); itc != known.end(); itc++)
	{
			// if we found the location:
		if (itc->theLoc.X == testLoc.theLoc.X && itc->theLoc.Y == testLoc.theLoc.Y)
		{
				// test its consistency:
			if (itc->theExists == theTruth)
			{
				return 1;
			}
			else
			{
				// if not consistent, return 0:
				return 0;
			}

		} // if location is consistent with truthvalue, return consistent = 1

	} // end iterate through known

		// check Frontier Prime:

	for (list<LocPlus>::iterator itc = frontierPrime.begin(); itc != frontierPrime.end(); itc++)
	{
			// if we found the location:
		if (itc->theLoc.X == testLoc.theLoc.X && itc->theLoc.Y == testLoc.theLoc.Y)
		{
					// test its consistency
			if (itc->theExists == theTruth)
			{
				return 1;
			}
			else
			{
				// if not consistent, return 0:
				return 0;
			}

		} // if location is consistent with truthvalue, return consistent = 1

	} // end iterate through frontierPrime

	// if we get here, no matching location was found, this shouldn't happen:

	cout << "ERROR: evaluation location" << testLoc.theLoc.X << "," << testLoc.theLoc.Y << " is not in knowledge base" << endl;
		// if no position was consistent, return 0:

	return 0;

} // end consistentCheck function

// addToPossiblesHelper

void Agent::addToPossiblesHelper (LocPlus possibleMove)
{
		// test for wumpus first:
		// if the location is a known wumpus, return to caller,
		// do not add to possibleMoveList

	if (possibleMove.theLoc.X == wumpusLoc.X && possibleMove.theLoc.Y == wumpusLoc.Y)
	{
		return;
	} // end test for wumpus location

		// if location is in known list and is not a pit, add to possibleMoveList:

	for (list<LocPlus>::iterator itp = known.begin (); itp != known.end (); itp++)
	{
		if (itp->theLoc.X == possibleMove.theLoc.X && itp->theLoc.Y == possibleMove.theLoc.Y)
		{
			if (itp->theExists == false)
			{
				possibleMove.theProb = 0.0;
				possibleMoveList.push_back (possibleMove);

					// if it's in known, no point in evaluating frontier so just
					// return to where we were called:
				return;

			} // end if not a pit, add to possibleMoveList
			// if in known and is a pit, don't add to possibleMoveList:
			else
			{
				return;
			} // end if it is a pit, don't add to possibles, just return

		} // end if in known, check if pit

	} // end iterate through known list

	// if not in known, it must be in the frontier:

	for (list<LocPlus>::iterator itp = frontier.begin (); itp != frontier.end (); itp++)
	{
		if (itp->theLoc.X == possibleMove.theLoc.X && itp->theLoc.Y == possibleMove.theLoc.Y)
		{
			possibleMove.theProb = itp->theProb;
			possibleMoveList.push_back (possibleMove);

				// if it's in known, no point in evaluating frontier so just
				// return to where we were called:
			return;

		} // end if in known, check if pit

	} // end iterate through frontier

		// we should not get here, if we did, display an error:

	cout << "known: " << endl;
	printKnown ();
	cout << "frontier: " << endl;
	printFrontier ();
	cout << "ERROR: Not a wumpus location, not in known, not in frontier" << endl;
	cout << "possibleMove: " << possibleMove.theLoc.X << "," << possibleMove.theLoc.Y << endl << endl;

} // end addToPossiblesHelper function

// addToPossibles function

void Agent::addToPossibles (LocPlus curLocPlus)
{
	LocPlus possibleMove;

	/*
	Possible moves list

	for each direction from current location:

	if location is valid:
			if location is not a known wumpus location:
		    if location is known and not a pit:
		        add location to possibleMoveList with pit probability of 0.0
		    else, location is known and is a pit:
		        return to caller, do not add to possibleMoveList

      get location information from frontier:
          add location to possibleMoveList with pit probability from frontier
	*/

	if (curLocPlus.theLoc.Y < 4)
	{
		possibleMove.theLoc.X = curLocPlus.theLoc.X;
		possibleMove.theLoc.Y = curLocPlus.theLoc.Y + 1;

		addToPossiblesHelper (possibleMove);

	} // end evaluate up

	if (curLocPlus.theLoc.Y > 1)
	{
		possibleMove.theLoc.X = curLocPlus.theLoc.X;
		possibleMove.theLoc.Y = curLocPlus.theLoc.Y - 1;

		addToPossiblesHelper (possibleMove);

	} // end evaluate down

	if (curLocPlus.theLoc.X > 1)
	{
		possibleMove.theLoc.X = curLocPlus.theLoc.X - 1;
		possibleMove.theLoc.Y = curLocPlus.theLoc.Y;

		addToPossiblesHelper (possibleMove);

	} // end evaluate left

	if (curLocPlus.theLoc.X < 4)
	{
		possibleMove.theLoc.X = curLocPlus.theLoc.X + 1;
		possibleMove.theLoc.Y = curLocPlus.theLoc.Y;

		addToPossiblesHelper (possibleMove);

	} // end evaluate right

	return;

} // end addToPossibles function

// printFrontier function
void Agent::printFrontier ()
{
	cout << endl << "\t*** Frontier ***" << endl << endl;
		// iterate through frontier list and print all elements:

	for (list<LocPlus>::iterator itcf = frontier.begin (); itcf != frontier.end (); itcf++)
	{
		cout << "frontier: " << itcf->theLoc.X << "," << itcf->theLoc.Y << " : " << itcf->theProb << endl;
	} // end print out all items in the frontier

	cout << "end Print Frontier" << endl;

} // end printFronter function


// addToKnown testFunction

void Agent::addToKnown (Location curLoc)
{
	LocPlus curLocPlus;

	curLocPlus.theLoc.X = curLoc.X;
	curLocPlus.theLoc.Y = curLoc.Y;
	curLocPlus.theExists = false;
	curLocPlus.theProb = 0.0;

	for (list<LocPlus>::iterator ita = known.begin (); ita != known.end (); ita++)
	{
		if (ita->theLoc.X == curLocPlus.theLoc.X && ita->theLoc.Y == curLocPlus.theLoc.Y)
		{
			ita->theExists = false;
			ita->theProb = 0.0;
			return;
		} // end if we found the location in known, stop looking

	} // end check if location is already in known

		// if location was not already in known, add it:
	known.push_back (curLocPlus);
	removeUnvisited (curLoc);
	if (curLocPlus.theExists == false)
	{
		searchEngine.AddSafeLocation(curLoc.X, curLoc.Y);
	} // add to safe locations list if not a pit

} // end addToKnown function

	// printKnown function
void Agent::printKnown ()
{
	cout << "\t*** Known list ***" << endl;

	for (list<LocPlus>::iterator itp = known.begin (); itp != known.end (); itp++)
	{
		cout << "known: " << itp->theLoc.X << "," << itp->theLoc.Y << " : " << itp->theExists << " : " << itp->theProb << endl;
	}
} // end printKnown function

// printBreeze function
void Agent::printBreeze ()
{
cout << "\t*** Breeze list ***" << endl;

for (list<LocPlus>::iterator itp = breezeList.begin (); itp != breezeList.end (); itp++)
{
	cout << "breezeList: " << itp->theLoc.X << "," << itp->theLoc.Y << " : " << itp->theExists << " : " << itp->theProb << endl;
}
} // end printBreeze function

// addBreezeHelper function
void Agent::addBreezeHelper (LocPlus breezeLoc)
{
	for (list<LocPlus>::iterator itb = breezeList.begin(); itb != breezeList.end(); itb++)
	{
		if (itb->theLoc.X == breezeLoc.theLoc.X && itb->theLoc.Y == breezeLoc.theLoc.Y)
		{
				// if breeze location is already in list, just return
				// nothing new to add
			return;

		} // end check if location is already in breezeList

	} // end iterate through all locaitons already in breezeList

	// if we get here, location is not in breeze list, so add it:
	breezeLoc.theExists = 1;
	breezeLoc.theProb = 1.0;

	breezeList.push_back (breezeLoc);

} // end addBreezeHelper function

	// addBreeze function
void Agent::addBreeze (LocPlus pitLoc)
{
	LocPlus breezeLoc;

	breezeLoc.theExists = 1;
	breezeLoc.theProb = 1.0;

	// evaluate all four locations around pitLoc:

	// up
	if (pitLoc.theLoc.Y < 4)
	{
		breezeLoc.theLoc.X = pitLoc.theLoc.X;
		breezeLoc.theLoc.Y = pitLoc.theLoc.Y + 1;

		addBreezeHelper (breezeLoc);
	} // end up

	// down
	if (pitLoc.theLoc.Y > 1)
	{
		breezeLoc.theLoc.X = pitLoc.theLoc.X;
		breezeLoc.theLoc.Y = pitLoc.theLoc.Y - 1;

		addBreezeHelper (breezeLoc);
	} // end down

	// left
	if (pitLoc.theLoc.X > 1)
	{
		breezeLoc.theLoc.X = pitLoc.theLoc.X - 1;
		breezeLoc.theLoc.Y = pitLoc.theLoc.Y;

		addBreezeHelper (breezeLoc);
	} // end left

	// right
	if (pitLoc.theLoc.X < 4)
	{
		breezeLoc.theLoc.X = pitLoc.theLoc.X + 1;
		breezeLoc.theLoc.Y = pitLoc.theLoc.Y;

		addBreezeHelper (breezeLoc);
	} // end right

} // end addBreeze function

// testSingle function
int Agent::testSingle (LocPlus testOne)
{
	for (list <LocPlus>::iterator ito = known.begin (); ito != known.end (); ito++)
	{
		if (ito->theLoc.X == testOne.theLoc.X && ito->theLoc.Y == testOne.theLoc.Y && ito->theExists == true)
		{
			return 1;
		} // if location is in known and has a pit, return 1

	} // end iterate through known to check for pit

	// if location does not contain a pit, return 0

	return 0;

} // end testSingle function

	// strtoflt function
	// convert a string to a float

float Agent::strtoflt (string theString)
{
	int i = 0, j = 1, decFlag = 0;
	float number = 0, theFloat = 0;

	while (theString[i] != 0)
	{
		if (theString[i] == '.')
		{
			decFlag = 1;
			i++;
		}
		number = ((float) theString[i]) - 48;
		if (decFlag == 0)
		{
			theFloat = (theFloat * 10.0) + number;
		} // end if before decimal
		else
		{
			theFloat = theFloat + (number / pow(10,j));

			j++;
		} // end if after decimal

		i++;

	} // end while


	return theFloat;

} // end strtoflt function

// printSafe function

void Agent::printSafe ()
{
	cout << "\t ***safeLocations List ***" << endl;

	for (list<Location>::iterator its = searchEngine.safeLocations.begin (); its != searchEngine.safeLocations.end (); its++)
	{
		cout << its->X << "," << its->Y << endl;
	} // end print out all locations in safeLocations list

} // end printSafe function

	// removeUnvisited
	// remove a visted location from the unvisited list

void Agent::removeUnvisited (Location theLoc)
{
		// if location is at the end of the list:
	//cout << "unvisitedRemoval of " << theLoc.X << "," << theLoc.Y << endl;

	list <LocPlus>::iterator itr = unvisited.end();
	if (itr->theLoc.X == theLoc.X && itr->theLoc.Y == theLoc.Y)
	{
		unvisited.pop_back ();
		return;
	} // end if it's at the end of the list

		// if location is not at the end of the list:

	for (list <LocPlus> ::iterator itr = unvisited.begin (); itr != unvisited.end (); itr++)
	{
		if (itr->theLoc.X == theLoc.X && itr->theLoc.Y == theLoc.Y)
		{
			unvisited.erase (itr);
			return;
		} // end if found, erase and return
	} // end iterate through unvisited to removed the location

	// if the locatioi is not in the list, nothing to remove:

	return;

} // end removeUnvisited function

// printProbabilities funtion
// print the probabilities of all locations:

void Agent::printProbabilities ()
{
	// print the probabilitis from the Known list first,
	// then from the frontier,
	// then from unvisited:

	cout << endl;
	cout << "\t~~~ Probabilities of all locations ~~~" << endl;
	cout << endl;

		// known:
		cout << "known" << endl;
	for (list<LocPlus>::iterator itk = known.begin (); itk != known.end (); itk++)
	{
		cout << itk->theLoc.X << "," << itk->theLoc.Y << " : " << itk->theProb << endl;
	} // end print from known list

		// frontier:
		cout << "frontier" << endl;
	for (list<LocPlus>::iterator itf = frontier.begin (); itf != frontier.end (); itf++)
	{
		cout << itf->theLoc.X << "," << itf->theLoc.Y << " : " << itf->theProb << endl;
	} // end print from frontier list

		// unvisited
		cout << "unvisited" << endl;
	int uFlag = 0;
	for (list<LocPlus>::iterator itu = unvisited.begin (); itu != unvisited.end (); itu++)
	{
		for (list<LocPlus>::iterator itk = known.begin (); itk != known.end (); itk++)
		{
			if (itk->theLoc.X == itu->theLoc.X && itk->theLoc.Y == itu->theLoc.Y)
			{
				uFlag = 1;
				break;
			} // end if in known list, set flag

		} // end ignore if in known

		for (list<LocPlus>::iterator itf = frontier.begin (); itf != frontier.end (); itf++)
		{
			if (itf->theLoc.X == itu->theLoc.X && itf->theLoc.Y == itu->theLoc.Y)
			{
				uFlag = 1;
				break;
			} // end if in frontier list, set flag

		} // end ignore if in frontier

		if (uFlag == 0)
		{
			cout << itu->theLoc.X << "," << itu->theLoc.Y << " : " << itu->theProb << endl;
		}

			// reset uFlag:
		uFlag = 0;
	} // end print from unvisited list

	cout << endl;

} // end printProbabilities

// printUnvisited function

void Agent::printUnvisited ()
{
	cout << "\t*** Unvisited ***" << endl;

	for (list<LocPlus>::iterator itu = unvisited.begin (); itu != unvisited.end (); itu++)
	{
		cout << itu->theLoc.X << "," << itu->theLoc.Y << " : " << itu->theProb << endl;
	} // end iterate through all unVisited and print them

} // end printUnvisited function

// testFrontier function
int Agent::testFrontier ()
{
	int leaveFlag = 1;

		// check frontier for any location with probability < 0.5
		// if any location is < 0.5, we stay and play,
		// if we don't find any location < 0.5, no location is < 0.5
		// so we leave

	for (list<LocPlus>::iterator itf = frontier.begin (); itf != frontier.end (); itf++)
	{
		if (itf->theProb < 0.5)
		{
			leaveFlag = 0;
		} // end if we found a < 0.5
	} // end iterate through frontier list

	return leaveFlag;

}  // end testFrontier function

// cornerCaseHelper

int Agent::cornerCaseHelper (LocPlus testLoc)
{
	for (list<LocPlus>::iterator itk = known.begin(); itk != known.end(); itk++)
	{
		if (itk->theLoc.X == testLoc.theLoc.X && itk->theLoc.Y == testLoc.theLoc.Y)
		{
			if (itk->theExists == true)
			{
				return 1;
			} // end if
			else
			{
				return 0;
			} // end if don't match
		} // end if we found the location in known
	} // end iterate
} // end cornerCaseHelper function

// cornerCase function

void Agent::cornerCase (Location curLoc)
{
		// if only one in frontier,
		// if we detect a breeze,
		// if there is a known pit adjacent to us, P(pit=true) = 0.2
		// if there is not a known pit adjacent, P(pit=true) = 1

	LocPlus testLoc, breezeLoc;
	int testFlag = 0, totalFlag = 0;
	bool curBreeze;

	for (list<LocPlus>::iterator itb = breezeList.begin (); itb != breezeList.end (); itb++)
	{
		if (itb->theLoc.X == curLoc.X && itb->theLoc.Y == curLoc.Y)
		{
			curBreeze = itb->theExists;
		}
	}

		// get the frontier element:

	list <LocPlus>::iterator itf = frontier.begin ();

	if (curBreeze == 1)
	{
			// check adjacent locatons to current to explain breeze

		if (curLoc.Y < 4)
		{
			testLoc.theLoc.X = curLoc.X;
			testLoc.theLoc.Y = curLoc.Y + 1;
			testFlag = cornerCaseHelper (testLoc);
			totalFlag += testFlag;
		}
		if (curLoc.Y > 1)
		{
			testLoc.theLoc.X = curLoc.X;
			testLoc.theLoc.Y = curLoc.Y - 1;
			testFlag = cornerCaseHelper (testLoc);
			totalFlag += testFlag;
		}
		if (curLoc.X > 1)
		{
			testLoc.theLoc.X = curLoc.X - 1;
			testLoc.theLoc.Y = curLoc.Y;
			testFlag = cornerCaseHelper (testLoc);
			totalFlag += testFlag;
		}
		if (curLoc.X < 4)
		{
			testLoc.theLoc.X = curLoc.X + 1;
			testLoc.theLoc.Y = curLoc.Y;
			testFlag = cornerCaseHelper (testLoc);
			totalFlag += testFlag;
		}

		if (totalFlag > 0)
		{
			itf->theExists = true;
			itf->theProb = 0.2;
		}
		else
		{
			itf->theExists = true;
			itf->theProb = 1;
		}
	} // end if there is a breeze:

	else
	{
		itf->theExists = false;
		itf->theProb = 0;
	}

} // end cornerCase function

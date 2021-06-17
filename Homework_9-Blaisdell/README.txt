Marcus Blaisdell
Homework #9 

No special instructions.

I am using a "saved.txt" text file to save the knowledge base between runs.
It will not load after the tenth run to create a new save file every tenth time the file has been read.

I have tested this using:
./wumpsim -world test1.txt -trials 10
./wumpsim -world test2.txt -trials 10
and it is functioning as expected.

I am using a recursive algorithm to create all of the possible combinations of the frontier.

I created a struct to hold:
Location
Truth-ness
Probability

of each location in: known, breezeList, frontier, unvisited, and many other helper lists. 

The struct allowed me to store the probability associated with a location and whether or not it exists for that location. 

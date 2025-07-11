/*
Game: Snake(ASCII)
Author: Slate Thompson
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <conio.h>
#include <windows.h>

#define WIDTH 10*2
#define HEIGHT 10
#define REFRESH_RATE 60
#define LOGIC_RATE 8


//Structure that holds an x and y value. used to define points on a grid
struct coordinate {
	int x;
	int y;
}; typedef struct coordinate coord;

//compares two coordinates to see if they overlap. Returns true if they do
int clsnDetect(coord coord1, coord coord2) {
    //collision detected
	if(coord1.x == coord2.x && coord1.y == coord2.y) {
		return 1;
	}
	//no collision detected
	else {
		return 0;
	}
}


//Generates random location for apple to placed
coord applesGen(coord snakeBody[], int snakeLen) {
    //creates random coordinate within the confines of the game area
	coord appleLoc = {(rand() % ((WIDTH-2)/2))*2+2,(rand() % (HEIGHT-1))+1};
	
	//checks if it overlaps with the snake. if it does it recursively generates until it doesn't
	for (int i = 0; i < snakeLen; i++){
	    if (clsnDetect(snakeBody[i], appleLoc)){
	        appleLoc = applesGen(snakeBody, snakeLen);
	    }
	}
	
	return appleLoc;
}



//Evaluates what objects lies at the given position
int posEval(coord curPos, coord appleLoc, coord snakeBody[], int snakeLen) {
    //Border
    if (curPos.x == 0 || curPos.x == WIDTH+2 ||curPos.y == 0 || curPos.y == HEIGHT+1){
	    return 3;
	}
	//Apple
	if (clsnDetect(appleLoc, curPos)) {
		return 2;
	}
	//Snake Body
	for(int i = 0; i < snakeLen; i++) {
		if (clsnDetect(snakeBody[i], curPos)) {
			return 1;
		}
	}
	//Empty
	return 0; 
}


//Renders out the game state in ASCII
void frame(int y, int x, coord appleLoc, coord snakeBody[], int score, int snakeLen) {
    system("clear"); // Clear screen using Windows command
    
    //grid to be output and score string
    char outStr[((WIDTH*(HEIGHT+2))*(16))] = "";
    char scoreStr[50] = "";
    //used to alternate between printing spaces and game info for visual asthetic
    int gapCount = 0;

	//Brightens colors & displays score
	sprintf(scoreStr, "\033[1mScore: %d\n\n", score);
	strcat(outStr, scoreStr);

    //iterates through grid
	for(int i = 0; i < x+2; i++) {
		for(int j = 0; j < y+4; j++) {
		    
		    coord curPos = {j, i};
		    
		    //alternates between space and game info using gapCount
		    //prints approriate char and color given the posEval return value.
		    if(gapCount%2 == 0){
    			switch(posEval(curPos,appleLoc,snakeBody,snakeLen)) {
    			case 1: strcat(outStr,"\033[36m=\033[32m"); break; //snake body
    			case 2: strcat(outStr,"\033[31m@\033[32m"); break; //apple
    			case 3: strcat(outStr,"\033[36m#\033[32m"); break; //border
    			default: strcat(outStr, "^"); break; //background
    			}
		    }
		    else{
		        strcat(outStr," ");
		    }
			gapCount++;
		}
	    strcat(outStr, "\n");
	}
	
	//updates game screen
	printf("%s",outStr); //prints outStr
	
	//Possibly more optimized than the printf statment: write(STDOUT_FILENO, outStr, strlen(outStr));
}


//Runs primary game logic
int main()
{
    //Basic game setup
    system("cls");
    srand(time(NULL)); //sets seed for random
	
	//fundamental variables
	int frameLogicBuffer = 0; //allows there to be multiple frames per game state
	int running = 1; //game loop on/off var
	int snakeLen = 1; 
	char key;
	int direction = 0;
	int prevDirection;
	
	//tracks stats data
	int score = 0;
	int turns = 0;
	int distance = 0;

    //creates snake body
	coord snakeBody[WIDTH+1*HEIGHT+1];
	snakeBody[0].x = WIDTH/2;
    snakeBody[0].y = HEIGHT/2;
	
    //holds apple location and initializes it with randomly generate position
	coord appleLoc = applesGen(snakeBody, snakeLen);

    //MAIN GAME LOOP
	while(running == 1) {
	    
	    //Game logic
	    if(frameLogicBuffer % LOGIC_RATE == 0){

            //Keyboard was hit
    		if(_kbhit()) {
                //Gets key that was hit
    			key = getch();
    
                //changes direction based off key
    			switch(key) {
        			case 'w': direction = 1; break;
        			case 'a': direction = 2; break;
        			case 's': direction = 3; break;
        			case 'd': direction = 4; break;
        			
        			case 'r': main(); break; //Restart
    			}
    			turns++;
    		}
    		//moves snake based on direction
    		if (abs(prevDirection-direction) == 2){
    		    direction = prevDirection;    
    		}
    		switch(direction) {
        		case 1: snakeBody[0].y--; distance++; break;
        		case 2: snakeBody[0].x-=2; distance++; break;
        		case 3: snakeBody[0].y++; distance++; break;
        		case 4: snakeBody[0].x+=2; distance++; break;
    		}
    		
            
            //Border collision detection
    		if(snakeBody[0].x == 0 || snakeBody[0].x == WIDTH+2 || snakeBody[0].y == 0 || snakeBody[0].y == HEIGHT+1) {
    			running = 0;
    		}
    		
            //snake eating apple detection
    		if(clsnDetect(snakeBody[0],appleLoc)) {
    			score++;
    			appleLoc = applesGen(snakeBody, snakeLen); //generates new apple loc
    			snakeLen++;
    		}
    		
            //self collision detection
    		for(int i = snakeLen-1; i > 0; i--) {
    			if (clsnDetect(snakeBody[0], snakeBody[i])) {
    				running = 0;
    				i = 0;
    			}
    		}
            
            //tracks snake movement to create "slither" effect. moves value at each index to the next index
    		coord prev = snakeBody[0];
    		coord temp;
    		for (int i = snakeLen; i > 0; i--) {
    			temp = snakeBody[i];
    			snakeBody[i] = prev;
    			prev = temp;
    		}
	    } 
	    //frame call & timing
        else{
            Sleep(1000/REFRESH_RATE); //Windows sleep function (takes milliseconds)
    		frame(WIDTH,HEIGHT, appleLoc, snakeBody, score, snakeLen);
        }
        
        prevDirection = direction; 
        frameLogicBuffer++;
	}
	
	//game over menu variables
	char menu1;
	char statCh;
	
	//Game over menu
	printf("Game Over!\nRestart Game (r)\nSee Stats (t)\nClose Game (e)\n");
	
	//duplication to prevent accidental key press
	scanf("%c", &menu1);
    scanf("%c", &menu1);
	switch(menu1){
	    case 'r': main(); break; //restart
	    case 'e': break; //exit
	    case 't': //stats
    	    printf("\n\n\nScore: %d\nDistance Traveled: %d\nTurns Made: %d\n\npress (r) to restart\nPress any other key to exit", score, distance, turns);
    	    scanf("%c", &statCh);
    	    if(statCh == 'r'){
    	        main();
    	    }
    	    else{
        	    break;   
    	    }
	}

	//end of program
	return 0;
}


/*

This is a simple 2048 game for arduino on 128x64 OLED display.
 
*/


#include "U8glib.h"


#include "Arduino.h"

#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

U8GLIB_SH1106_128X64 u8g(5, 6, 10, 8, 7);  // SW SPI Com: SCK = 13, MOSI = 11, CS = 10, A0 = 9

// Initialise the grid and counter variables.
int **grid;
int score = 0;
int occupiedTiles = 0;

int leftButtonPin = 9;
int downButtonPin = 13;
int upButtonPin = 12;
int rightButtonPin = 11;

void str_replace(char *src, char *oldchars, char *newchars) { // utility string function
  char *p = strstr(src, oldchars);
  char buf[30];
  do {
    if (p) {
      memset(buf, '\0', strlen(buf));
      if (src == p) {
        strcpy(buf, newchars);
        strcat(buf, p + strlen(oldchars));
      } else {
        strncpy(buf, src, strlen(src) - strlen(p));
        strcat(buf, newchars);
        strcat(buf, p + strlen(oldchars));
      }
      memset(src, '\0', strlen(src));
      strcpy(src, buf);
    }
  } while (p && (p = strstr(src, oldchars)));
}

void draw() {
  // graphic commands to redraw the complete screen should be placed here  
  u8g.setFont(u8g_font_6x10);
  u8g_uint_t yOffset = 7;
  char buffer[21];
  sprintf(buffer, "Score: %d", score);
  u8g.drawStr(5, 7, buffer);
  u8g.drawStr(0,yOffset + 10, " -------------------");
  for (u8g_uint_t i = 0; i < 4; i++) {
    char buffer[21];
    sprintf(buffer,"|%4d|%4d|%4d|%4d|", grid[i][0], grid[i][1], grid[i][2], grid[i][3]);
    str_replace(buffer, "   0", "    ");
    u8g.drawStr(0, yOffset + 22+12*i -6, buffer);
    if(i != 3) {
      u8g.drawStr(0, yOffset + 22+12*i, " -------------------");
    }
  }
}
void drawGameOver() {
  // graphic commands to redraw the complete screen should be placed here  
  u8g.setFont(u8g_font_6x10);
  u8g_uint_t yOffset = 7;
  char buffer[21];
  sprintf(buffer, "Score: %d", score);
  u8g.drawStr(5, 10, buffer);
  u8g.drawStr(30,yOffset + 20, "Game Over!");
}

int generateNewTileValue() {
	return 2 + 2 * (int) random(2);
}

int getRandomCoordinate() {
	return random (100) % 4;
}

void spawnTile() {
	bool success = false;
	while(!success) {
		int x = getRandomCoordinate();
		int y = getRandomCoordinate();

		if(grid[x][y] == 0) {
			grid[x][y] = generateNewTileValue();
			success = true;
		}
	}
	occupiedTiles++;
}

bool isEmptyRow(int *row) {
	boolean isEmpty = true;
	for (int i = 0; i < 4; i++) {
		isEmpty &= (row[i] == 0);
	}
	return isEmpty;
}

void transpose() {
  int **transposed = allocateGrid();
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
				transposed[j][i] = grid[i][j];
		}
	}
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
				grid[i][j] = transposed[i][j];
		}
	}
	freeGrid(transposed);
}

void merge(int direction) {
	if (direction == UP || direction == DOWN) {
		transpose();
	}
	
	for(int i = 0; i < 4; i++) {
		mergeRow(i, direction);	
	}

	if (direction == UP || direction == DOWN) {
		transpose();
	}
}

int getSuccessorIndex(int i, int currentIndex) {
	int succ = currentIndex + 1;
	while (succ < 4 && grid[i][succ] == 0) {
		succ++;
	}
	return succ;
}

void reverse(int *row) {
	int clone[4];
	for (int i = 0; i < 4; i++){
		clone[i] = row[i];
	}
	for (int i = 0; i < 4; i++) {
		row[3-i] = clone[i];
	}
}

void mergeRow(int i, int direction) {
	int currentIndex = 0;
	int mergedRow[4] = {0, 0, 0, 0};
	int mergedNum = 0; 

	if (direction == DOWN || direction == RIGHT) {
		reverse(grid[i]);
	}

	currentIndex = getSuccessorIndex(i, -1);
  
	if (currentIndex == 4) {
		// All tiles are empty.
		return;
	}

	// Now the current tile must be non-empty.
	while(currentIndex < 4) {
		int successorIndex = getSuccessorIndex(i, currentIndex);
		if (successorIndex < 4 && grid[i][currentIndex] == grid[i][successorIndex]) {
			// Two matching tiles found, we perform a merge.
			int sum = grid[i][currentIndex] + grid[i][successorIndex];
			score += sum;
			occupiedTiles--;
			mergedRow[mergedNum] = sum;
			mergedNum++;	
			currentIndex = getSuccessorIndex(i, successorIndex);
		} else {
			mergedRow[mergedNum] = grid[i][currentIndex];
			mergedNum++;
			currentIndex = successorIndex;
		}
	}

	for(int j = 0; j < 4; j++) {
		if (direction == DOWN || direction == RIGHT) {
			grid[i][3-j] = mergedRow[j];
		} else {
			grid[i][j] = mergedRow[j];
		}
	}
}

void freeGrid(int **g) {
	for (int i = 0; i < 4; i++) {
		free(g[i]);
	}
	free(g);
}
void takeTurn(int direction) {
	int **oldGrid = allocateGrid();
	copy(grid, oldGrid);
	merge(direction);

	if (theGridChangedFrom(oldGrid)) {
		spawnTile();
	}
	freeGrid(oldGrid);
}

bool isBoardFull() {
	return occupiedTiles >= 16;
}

bool isGameOver() {
	return isBoardFull() && noMovePossible();
}

bool theGridChangedFrom(int **oldGrid) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (grid[i][j] != oldGrid[i][j]) {
				return true;
			}
		}
	}
	return false;
}

bool noMovePossible() {
	// Preserve the state
	int currentState = allocateGrid();
	copy(grid, currentState);
	int currentScore = score;
	int currentOccupied = occupiedTiles;

	boolean noMoves = true;
	for (int direction = 0; direction < 4; direction++) {
		merge(direction);
		noMoves &= !theGridChangedFrom(currentState);
		copy(currentState, grid);
	}
	freeGrid(currentState);

	// Restore the state
	score = currentScore;
	occupiedTiles = currentOccupied;
	return noMoves;
}

void copy(int **source, int **destination) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			destination[i][j] = source[i][j];
		}
	}
}

int **allocateGrid() {
	int **g = malloc (4 * sizeof(int));
  for (int i = 0; i < 4; i++) {
		g[i] = malloc (4 * sizeof(int));
  }
	return g;
}


void setup(void) {
  randomSeed(analogRead(0));
  // flip screen, if required
  u8g.setRot180();
  
  // set SPI backup if required
  //u8g.setHardwareBackup(u8g_backup_avr_spi);

  // assign default color value
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255);     // white
  }
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3);         // max intensity
  }
  else if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);         // pixel on
  }
  else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
    u8g.setHiColorByRGB(255,255,255);
  }
  
  pinMode(8, OUTPUT);
  pinMode(leftButtonPin, INPUT);
  pinMode(downButtonPin, INPUT);
  pinMode(upButtonPin, INPUT);
  pinMode(rightButtonPin, INPUT);
	
	grid = allocateGrid();	

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      grid[i][j] = 0;
    }
  }
}

int leftButton;
int downButton;
int upButton;
int rightButton;

void loop(void) {
  // picture loop
  spawnTile();
	u8g.firstPage();  
	do {
		draw();
	} while( u8g.nextPage() );

  while(!isGameOver()) {
		leftButton=digitalRead(leftButtonPin);
		downButton=digitalRead(downButtonPin);
		upButton=digitalRead(upButtonPin);
		rightButton=digitalRead(rightButtonPin);
    u8g.firstPage();  
    do {
      draw();
    } while( u8g.nextPage() );
		//Serial.print(leftButton);
		//Serial.print(downButton);
		//Serial.print(upButton);
		//Serial.println(rightButton);
		int turn;
		bool inputRegistered = false;


		if (!leftButton) {
			turn = LEFT;
			inputRegistered = true;
		}
		if (!downButton) {
			turn = DOWN;
			inputRegistered = true;
		}
		if (!upButton) {
			turn = UP;
			inputRegistered = true;
		}
		if (!rightButton) {
			turn = RIGHT;
			inputRegistered = true;
		}

		if (inputRegistered) {
      takeTurn(turn);
			u8g.firstPage();  
			do {
				draw();
			} while( u8g.nextPage() );
			delay(150);
		}
   delay(50);
  }
		u8g.firstPage();  
		do {
			drawGameOver();
		} while( u8g.nextPage() );
}

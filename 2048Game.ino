/*

This is a simple 2048 game for arduino on 128x64 OLED display.
 
*/


#include "U8glib.h"


#include "Arduino.h"

#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

U8GLIB_SH1106_128X64 u8g(13, 11, 10, 9, 8);  // SW SPI Com: SCK = 13, MOSI = 11, CS = 10, A0 = 9

// Initialise the grid and counter variables.
int **grid;
int score = 0;
int occupiedTiles = 0;

void draw(int grid[4][4]) {
  // graphic commands to redraw the complete screen should be placed here  
  u8g.setFont(u8g_font_6x10);
  //u8g.setFont(u8g_font_5x7);
  //u8g.setFont(u8g_font_osb21);
  u8g_uint_t yOffset = 7;
  u8g.drawStr(0, 10, "Score: 123456");
  u8g.drawStr(0,yOffset + 10, " -------------------");
  for (u8g_uint_t i = 0; i < 4; i++) {
    u8g.drawStr(0, yOffset + 22+12*i -6, draw_grid_row(grid[i]).c_str());
    u8g.drawStr(0, yOffset + 22+12*i, " -------------------");
  }
}

String draw_grid_row(int row[]) {
  char buffer[21];
  sprintf(buffer,"|%4d|%4d|%4d|%4d|", row[0], row[1], row[2], row[3]);
  return buffer;
}

int generateNewTileValue() {
	return 2 + 2 * (int) random(1);
}

int getRandomCoordinate() {
	return random (4);
}

void spawnTile(int **grid) {
	bool success = false;
	while(!success) {
		int x = getRandomCoordinate();
		int y = getRandomCoordinate();

		if(grid[x][y] == 0) {
			grid[x][y] = getNewTileValue();
			success = true;
		}
	}
	occupiedTiles++;
}

bool isEmptyRow(int[] row) {
	boolean isEmpty = true;
	for (int i = 0; i < 4; i++) {
		isEmpty &= (row[i] == 0);
	}
	return isEmpty;
}

void transpose(int **grid) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (i != j) {
				int temp = grid[i][j];
				grid[i][j] = grid[j][i];
				grid[j][i] = temp;
			}
		}
	}
}

void merge(int **grid, int direction) {
	if (direction == UP || direction == DOWN) {
		transpose(grid);
	}
	
	for(int i = 0; i < 4; i++) {
		mergeRow(grid[i], int direction);
	}

	if (direction == UP || direction == DOWN) {
		transposa(grid);
	}
}

void getSuccessorIndex(int *row, int currentIndex) {
	int succ = currentIndex + 1;
	while (succ < 4 && row[succ] == 0) {
		succ++;
	}
	return succ;
}

void reverse(int *row) {
	int temp = row[0];
	for (int i = 1; i < 4; i++) {
		row[i-1] = row[i];
	}
	row[3] = temp;
}

void mergeRow(int *row, int direction) {
	int currentIndex = 0;
	int mergedRow[4] = {0, 0, 0, 0};
	int mergedNum = 0;

	if (direction == DOWN || direction == RIGHT) {
		reverse(row);
	}

	while (currentIndex < 4 && row[currentIndex] == 0) {
		currentIndex++;
	}
	if (currentIdex == 4) {
		// All tiles are empty.
		return;
	}

	// Now the current tile must be non-empty.
	while(currentIndex < 4) {
		if(currentIndex = 3) {
			// If we have reached the last tile, we add it to the list of merged tiles.
			mergedRow[mergedNum] = row[currentIndex];
			break;
		}
		int successorIndex = getSuccessorIndex(row, currentIndex);
		if (row[currentIndex] == row[successorIndex]) {
			// Two matchint tiles found, we perform a merge.
			int sum = row[currentIndex] + row[successorIndex];
			score += sum;
			occupiedTiles--;
			mergedRow[mergedNum] = sum;
			mergedNum++;	
			currentIndex = getSuccessorIndex(row, successor);
		} else {
			mergedRow[mergedNum] = row[currentIndex];
			mergedNum++;
			currentIndex = successorIndex;
		}
	}

	if (direction == DOWN || direction == RIGHT) {
		for(int i = 0; i < 4; i++) {
			row[3-i] = mergedRow[i];
		}
	}
}


void setup(void) {
  
  // flip screen, if required
  // u8g.setRot180();
  
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


	grid = malloc(4 * sizeof(int));
  for (int i = 0; i < 4; i++) {
		grid[i] = malloc(4 *sizeof(int));
  }
}

void loop(void) {
  // picture loop
  u8g.firstPage();  
  do {
    draw(grid);
  } while( u8g.nextPage() );
  
  // rebuild the picture after some delay
  delay(50);
}

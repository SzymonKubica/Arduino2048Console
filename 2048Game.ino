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
int grid[4][4];
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

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      grid[i][j] = 1;
    }
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

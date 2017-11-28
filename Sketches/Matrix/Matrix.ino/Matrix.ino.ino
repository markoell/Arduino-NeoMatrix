// Adafruit_NeoMatrix example for tiled NeoPixel matrices.  Scrolls
// 'Howdy' across three 10x8 NeoPixel grids that were created using
// NeoPixel 60 LEDs per meter flex strip.

#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

#ifndef PSTR
 #define PSTR // Make Arduino Due happy
#endif

#define PIN 6

const int NEO_MATRIX_HIGHT = 16;
const int NEO_MATRIX_WIDTH = 28;

// MATRIX DECLARATION:
// Parameter 1 = width of EACH NEOPIXEL MATRIX (not total display)
// Parameter 2 = height of each matrix
// Parameter 3 = number of matrices arranged horizontally
// Parameter 4 = number of matrices arranged vertically
// Parameter 5 = pin number (most are valid)
// Parameter 6 = matrix layout flags, add together as needed:
//   NEO_MATRIX_TOP, NEO_MATRIX_BOTTOM, NEO_MATRIX_LEFT, NEO_MATRIX_RIGHT:
//     Position of the FIRST LED in the FIRST MATRIX; pick two, e.g.
//     NEO_MATRIX_TOP + NEO_MATRIX_LEFT for the top-left corner.
//   NEO_MATRIX_ROWS, NEO_MATRIX_COLUMNS: LEDs WITHIN EACH MATRIX are
//     arranged in horizontal rows or in vertical columns, respectively;
//     pick one or the other.
//   NEO_MATRIX_PROGRESSIVE, NEO_MATRIX_ZIGZAG: all rows/columns WITHIN
//     EACH MATRIX proceed in the same order, or alternate lines reverse
//     direction; pick one.
//   NEO_TILE_TOP, NEO_TILE_BOTTOM, NEO_TILE_LEFT, NEO_TILE_RIGHT:
//     Position of the FIRST MATRIX (tile) in the OVERALL DISPLAY; pick
//     two, e.g. NEO_TILE_TOP + NEO_TILE_LEFT for the top-left corner.
//   NEO_TILE_ROWS, NEO_TILE_COLUMNS: the matrices in the OVERALL DISPLAY
//     are arranged in horizontal rows or in vertical columns, respectively;
//     pick one or the other.
//   NEO_TILE_PROGRESSIVE, NEO_TILE_ZIGZAG: the ROWS/COLUMS OF MATRICES
//     (tiles) in the OVERALL DISPLAY proceed in the same order for every
//     line, or alternate lines reverse direction; pick one.  When using
//     zig-zag order, the orientation of the matrices in alternate rows
//     will be rotated 180 degrees (this is normal -- simplifies wiring).
//   See example below for these values in action.
// Parameter 7 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 pixels)
//   NEO_GRB     Pixels are wired for GRB bitstream (v2 pixels)
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA v1 pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)

// Example with three 10x8 matrices (created using NeoPixel flex strip --
// these grids are not a ready-made product).  In this application we'd
// like to arrange the three matrices side-by-side in a wide display.
// The first matrix (tile) will be at the left, and the first pixel within
// that matrix is at the top left.  The matrices use zig-zag line ordering.
// There's only one row here, so it doesn't matter if we declare it in row
// or column order.  The matrices use 800 KHz (v2) pixels that expect GRB
// color data.
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(NEO_MATRIX_WIDTH, NEO_MATRIX_HIGHT, PIN,
  NEO_MATRIX_BOTTOM + NEO_MATRIX_RIGHT + NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG,
  NEO_GRB + NEO_KHZ800);

const uint16_t colors[] = {
  matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(0, 0, 255) };

int maxCounter = 50; //Dauer / Wiederholungen Krisseln
//int[] jahre = 2017, 1539,...


//byte (*blocks[2])[28] = { arr };  //TODO blocks[2] vergr��ern

int test = 0;  //TODO nur zum Test

void setup() {
  matrix.begin();
  //matrix.setTextWrap(false);
  matrix.setBrightness(40);
  matrix.setTextColor(colors[0]);
  matrix.fillScreen(0);
  matrix.show();
}

void loop() {

  if(test == 1)  //Sp�ter Taster abfragen
    return;
    
    //TODO unterscheidung Schalter Gruppen (1 = Rebl�use; 2 = Altstadtjodler; ...)
  drawPixel();  //TODO eine Schleife �ber alle Elemente auf Tastendruck (counter)

  test = 1; //Kann weg, sobald Taster
}

void drawPixel(){

    byte arr[17][28] = {
{0,0,0,255,0,0,0,255,255,255,255,255,0,0,0,255,255,255,255,0,0,0,255,255,0,255,255,0},
{0,0,255,255,0,0,255,255,255,255,255,255,255,0,255,255,255,255,255,255,255,0,255,255,0,255,255,0},
{0,255,255,255,0,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,255,255,0},
{255,255,255,255,0,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,255,255,0},
{0,0,255,255,0,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,0,255,255,0},
{0,0,255,255,0,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,255,255,255,255,255,255},
{0,0,255,255,0,0,255,255,255,255,255,255,255,0,255,255,255,255,255,255,255,0,255,255,255,255,255,255},
{0,0,255,255,0,0,0,255,255,255,255,255,0,0,0,255,255,255,255,255,0,0,0,0,0,255,255,0},
{0,0,255,255,0,0,255,255,255,255,255,255,255,0,255,255,255,255,255,255,255,0,0,0,0,255,255,0},
{0,0,255,255,0,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,0,0,255,255,0},
{0,0,255,255,0,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,0,0,255,255,0},
{0,0,255,255,0,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,0,0,255,255,0},
{0,0,255,255,0,0,255,255,0,0,0,255,255,0,255,255,0,0,0,255,255,0,0,0,0,255,255,0},
{0,0,255,255,0,0,255,255,255,255,255,255,255,0,255,255,255,255,255,255,255,0,0,0,0,255,255,0},
{0,255,255,255,255,0,0,255,255,255,255,255,0,0,0,255,255,255,255,255,0,0,0,0,0,255,255,0},
{0,255,255,255,255,0,0,255,255,255,255,255,0,0,0,255,255,255,255,255,0,0,0,0,0,255,255,0}};

  for (int i = 0; i < NEO_MATRIX_HIGHT; i++){
    for (int j = 0; j < NEO_MATRIX_WIDTH; j++){
      matrix.drawPixel(j,i,matrix.Color(arr[i][j],0,0));
    }
  }
  matrix.show();
}

void clr(){
  matrix.fillScreen(0);
}


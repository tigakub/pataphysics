#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

// Import custom font
#include "pataneo_5x8.h"

#ifndef PSTR
 #define PSTR // For Due
#endif

#define PIN 6

// MATRIX DECLARATION:
// Parameter 1 = width of NeoPixel matrix
// Parameter 2 = height of matrix
// Parameter 3 = pin number (most are valid)
// Parameter 4 = matrix layout flags, add together as needed:
//   NEO_MATRIX_TOP, NEO_MATRIX_BOTTOM, NEO_MATRIX_LEFT, NEO_MATRIX_RIGHT:
//     Position of the FIRST LED in the matrix; pick two, e.g.
//     NEO_MATRIX_TOP + NEO_MATRIX_LEFT for the top-left corner.
//   NEO_MATRIX_ROWS, NEO_MATRIX_COLUMNS: LEDs are arranged in horizontal
//     rows or in vertical columns, respectively; pick one or the other.
//   NEO_MATRIX_PROGRESSIVE, NEO_MATRIX_ZIGZAG: all rows/columns proceed
//     in the same order, or alternate lines reverse direction; pick one.
//   See example below for these values in action.
// Parameter 5 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)

// Instantiate a NeoMatrix Object at Global Scope
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(
  160,  // Five 32-pixel wide panels across
  8,    // 8 pixels high
  PIN,  // The data pin
  NEO_MATRIX_BOTTOM  + NEO_MATRIX_RIGHT + // Use NEO_MATRIX_TOP + NEO_MATRIX_LEFT to rotate 180º
  NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG, // NeoMatirx panels are laid out in zig-zag columns
  NEO_GRB            + NEO_KHZ800);       // NeoPixel library internal component format

// Starting position of the text display (start off screen)
int x = matrix.width();

// Message to scroll
String msg("IMPEACH TRUMP and his CABINET of DEPLORABLES!!!  Make America CIVIL Again!!!  RESIST!!!  ");

// Rainbow offset
uint32_t colorIndex = 0;

// Variables to store text bounds
int16_t tx, ty;
uint16_t tw, th;

// Function to cycle through color Gamut
uint32_t wheel(uint32_t iPos) {
  iPos = 255 - iPos;
  if(iPos < 85) {
    return matrix.Color(255 - iPos * 3, 0, iPos * 3);
  }
  if(iPos < 170) {
    iPos -= 85;
    return matrix.Color(0, iPos * 3, 255 - iPos * 3);
  }
  iPos -= 170;
  return matrix.Color(iPos * 3, 255 - iPos * 3, 0);
}

void setup() {
  // Initialize NeoMatrix
  matrix.begin();

  // Set font
  matrix.setFont(&pataneo_5x88pt7b);

  // The font is 8 pixels high, so a scale of 1 is fine.
  matrix.setTextSize(1);

  // Do NOT wrap the ext.
  matrix.setTextWrap(false);

  // Quarter intensity.
  matrix.setBrightness(64);

  // Get the bounding rectangle of the rendered text in pixels
  // (we're only interested in the width)
  matrix.getTextBounds((char *) msg.c_str(), 0, 0, &tx, &ty, &tw, &th);
}

void loop() {
  // Clear the display buffer
  matrix.fillScreen(0);

  // Set starting position to render the message
  matrix.setCursor(x, 5); // The font is 8 pixels high, but 
                          // true type fonts are rendered
                          // at the base line which for
                          // this font is the third pixel
                          // from the bottom. So we have
                          // to shift the base line down 
                          // 5 pixels to show the upper 
                          // part of the font.

  // Loop updating the color before rendering each character
  for(int i = 0; i < msg.length(); i++) {
    // Create shifting rainbow effect
    matrix.setTextColor(wheel((colorIndex + (i << 3)) % 256));
    // Render the next character
    matrix.print(msg.charAt(i));
  }

  // As the message shifts off the screen, we need to
  // render a second copy to maintain a
  // continuous ticker.
  if(x < 0) {
    for(int i = 0; i < msg.length(); i++) {
      matrix.setTextColor(wheel((colorIndex + (i << 3)) % 256));
      matrix.print(msg.charAt(i));
    }
  }

  // When the message has completely scrolled off screen,
  // we can wrap around.
  if(--x < -tw - 10) {
    x = -6;
  }

  // Blit the display buffer to the matrix array.
  matrix.show();

  // Step the color offset
  colorIndex -= 4;
}

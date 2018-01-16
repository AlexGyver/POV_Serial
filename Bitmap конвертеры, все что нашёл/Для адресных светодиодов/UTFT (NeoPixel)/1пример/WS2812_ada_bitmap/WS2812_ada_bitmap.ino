// ---------- НАСТРОЙКИ -----------
#define BRIGHTNESS 20
#define mw 16
#define mh 16
#define PIN 6
// ---------- НАСТРОЙКИ -----------

#include "bitmaps.h"  // файл с картинками

// ---------- БИБЛИОТЕКИ -----------
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
// ---------- БИБЛИОТЕКИ -----------

// ---------- СОЗДАЁМ МАТРИЦУ -----------
Adafruit_NeoMatrix *matrix = new Adafruit_NeoMatrix(mw, mh, PIN,
    NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
    NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG,
    NEO_GRB            + NEO_KHZ800);
// ---------- СОЗДАЁМ МАТРИЦУ -----------

void setup() {
  Serial.begin(9600);
  matrix->begin();
  matrix->setTextWrap(false);
  matrix->setBrightness(BRIGHTNESS);

  matrix->drawRGBBitmap(0, 0, (const uint16_t *) logoGyv, 16, 16);  
  matrix->show();
}

void loop() {


}

void fixdrawRGBBitmap(int16_t x, int16_t y, const uint16_t *bitmap, int16_t w, int16_t h) {
  uint16_t RGB_bmp_fixed[w * h];
  for (uint16_t pixel = 0; pixel < w * h; pixel++) {
    uint8_t r, g, b;
    uint16_t color = pgm_read_word(bitmap + pixel);

    b = (color & 0xF00) >> 8;
    g = (color & 0x0F0) >> 4;
    r = color & 0x00F;

    b = map(b, 0, 15, 0, 31);
    g = map(g, 0, 15, 0, 63);
    r = map(r, 0, 15, 0, 31);
    RGB_bmp_fixed[pixel] = (r << 11) + (g << 5) + b;
  }
  matrix->drawRGBBitmap(x, y, RGB_bmp_fixed, w, h);
}

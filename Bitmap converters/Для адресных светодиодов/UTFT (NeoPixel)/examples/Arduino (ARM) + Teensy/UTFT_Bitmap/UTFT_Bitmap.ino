// UTFT_Bitmap 
// Copyright (C)2015 Rinky-Dink Electronics, Henning Karlsen. All right reserved
// web: http://www.RinkyDinkElectronics.com/
//
// This program is a demo of the drawBitmap()-function.
//
// This demo was made to work on the 320x240 modules.
// Any other size displays may cause strange behaviour.
//
// This program requires the UTFT library.
//

#include <UTFT.h>

// Declare which fonts we will be using
extern uint8_t SmallFont[];

// Set the pins to the correct ones for your development shield
// ------------------------------------------------------------
// Standard Arduino Mega/Due shield            : <display model>,38,39,40,41
// CTE TFT LCD/SD Shield for Arduino Due       : <display model>,25,26,27,28
// Teensy 3.x TFT Test Board                   : <display model>,23,22, 3, 4
// ElecHouse TFT LCD/SD Shield for Arduino Due : <display model>,22,23,31,33
//
// Remember to change the model parameter to suit your display module!
UTFT myGLCD(ITDB32S,38,39,40,41);

extern unsigned short info[0x400];
extern unsigned short icon[0x400];
extern unsigned short tux[0x400];

void setup()
{
  myGLCD.InitLCD();
  myGLCD.setFont(SmallFont);
}

void loop()
{
  myGLCD.fillScr(255, 255, 255);
  myGLCD.setColor(255, 255, 255);
  myGLCD.print(" *** A 10 by 7 grid of a 32x32 icon *** ", CENTER, 228);
  for (int x=0; x<10; x++)
    for (int y=0; y<7; y++)
      myGLCD.drawBitmap (x*32, y*32, 32, 32, info);

  delay(5000);
  
  myGLCD.fillScr(255, 255, 255);
  myGLCD.setColor(255, 255, 255);
  myGLCD.print("   Two different icons in scale 1 to 4  ", CENTER, 228);
  int x=0;
  for (int s=0; s<4; s++)
  {
    x+=(s*32);
    myGLCD.drawBitmap (x, 0, 32, 32, tux, s+1);
  }
  x=0;
  for (int s=4; s>0; s--)
  {
    myGLCD.drawBitmap (x, 224-(s*32), 32, 32, icon, s);
    x+=(s*32);
  }

  delay(5000);
}


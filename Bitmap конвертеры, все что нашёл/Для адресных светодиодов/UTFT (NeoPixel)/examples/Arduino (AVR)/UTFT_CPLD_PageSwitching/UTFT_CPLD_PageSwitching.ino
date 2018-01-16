// UTFT_CPLD_PageSwitching 
// Copyright (C)2015 Rinky-Dink Electronics, Henning Karlsen. All right reserved
// web: http://www.RinkyDinkElectronics.com/
//
// This program is a demo of page switching on CPLD-based display modules..
//
// This demo was made for modules with a screen resolution 
// of 800x480 pixels.
//
// This program requires the UTFT library.
//
// NOTE: The display will be black for 10-15 seconds during the start
//

#include <UTFT.h>

// Declare which fonts we will be using
extern uint8_t BigFont[];
extern uint8_t SevenSegNumFont[];

// Set the pins to the correct ones for your development shield
// ------------------------------------------------------------
// Arduino Mega:
// -------------------
// Standard Arduino Mega/Due shield            : <display model>,38,39,40,41
// CTE TFT LCD/SD Shield for Arduino Mega      : <display model>,38,39,40,41
//
// Remember to change the model parameter to suit your display module!
UTFT myGLCD(CPLD,38,39,40,41);

void setup()
{
  myGLCD.InitLCD();
}

void loop()
{
  myGLCD.setBackColor(VGA_TRANSPARENT);

  myGLCD.setBrightness(0);  
  for (byte pg=0; pg<8; pg++)
  {
    myGLCD.setWritePage(pg);
    myGLCD.clrScr();
    for (int ln=0; ln<480; ln+=2)
    {
      switch (pg)
      {
        case 0:
          myGLCD.setColor(ln/2, 0, 0);
          break;
        case 1:
          myGLCD.setColor(0, ln/2, 0);
          break;
        case 2:
          myGLCD.setColor(0, 0, ln/2);
          break;
        case 3:
          myGLCD.setColor(ln/4, ln/2, 0);
          break;
        case 4:
          myGLCD.setColor(0, ln/2, ln/2);
          break;
        case 5:
          myGLCD.setColor(ln/2, 0, ln/2);
          break;
        case 6:
          myGLCD.setColor(ln/2, ln/2, 0);
          break;
        case 7:
          myGLCD.setColor(0, ln/2, ln/4);
          break;
      }
      myGLCD.drawLine(0, ln, 799, ln);
      myGLCD.drawLine(0, ln+1, 799, ln+1);
    }
    myGLCD.setColor(VGA_WHITE);
    myGLCD.setFont(BigFont);
    myGLCD.print("This is page:", CENTER, 200);
    myGLCD.setFont(SevenSegNumFont);
    myGLCD.printNumI(pg, CENTER, 240);
  }
  myGLCD.setBrightness(16);  

  while(1)
  {
    for (byte pg=0; pg<8; pg++)
    {
      myGLCD.setDisplayPage(pg);
      delay(500);
    }
  }
}


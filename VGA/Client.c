
// Video text sender
// Author: Nick Gammon
// Date:    20th April 2012
// License: Released to public domain.

#include <Wire.h>

const byte i2cAddress = 42;

enum SEND_COMMANDS { CLRSCR = 1, CLREOL, GOTOXY, ESC = 27 };
int count = 0;

char clearScreen [] = { ESC, CLRSCR };
char clearToEndOfLine [] = { ESC, CLREOL };
char gotoXY [] = { ESC, GOTOXY };  // followed by x and y

void setup () 
{
  Wire.begin ();
  TWBR = 4;   // fast .. fast .. I2C : 16000000 / (16 + 2 * 4) = 666666 Hz
}  // end of setup

void sendString (const char * s)
  {
  Wire.beginTransmission (i2cAddress);
  Wire.write (s);
  Wire.endTransmission ();
  }  // end of sendString
 
char buf [20];
   
void loop () 
{
  sendString (clearScreen);
  sendString ("Hi there\n");
  sendString ("Everyone\n");
  
  delay (2000);
 
  // clear screen test
  sendString (clearScreen);
  sendString ("After CLRSCR\n");

  // display a counter  
  sprintf (buf, "%i\n", count++);
  
  sendString (buf);

  delay (2000);  
}  // end of loop
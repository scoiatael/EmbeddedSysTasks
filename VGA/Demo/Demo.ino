// vim: syntax=c:
/*
 VGA video generation

 Author:   Nick Gammon
 Date:     20th April 2012

 Example code without I2C communications.

 Connections:

 D1 : Pixel output (180 ohms in series) (connect to R, G, B tied together)   --> Pins 1, 2, 3 on DB15 socket
 D3 : Horizontal Sync (68 ohms in series) --> Pin 13 on DB15 socket
 D10 : Vertical Sync (68 ohms in series) --> Pin 14 on DB15 socket

 Gnd : --> Pins 5, 6, 7, 8, 10 on DB15 socket

*/

#include <TimerHelpers.h>
#include <avr/pgmspace.h>
#include "screenFont.h"
#include <avr/sleep.h>

#define BETA_ARDUINO ARDUINO < 100

const byte pixelPin = 1;     // <------- Pixel data
const byte hSyncPin = 3;     // <------- HSYNC
const byte MSPIM_SCK = 4;    // <-- we aren't using it directly
const byte vSyncPin = 10;    // <------- VSYNC

const int horizontalBytes = 20;  // 160 pixels wide
const int verticalPixels = 480;  // 480 pixels high

const byte i2cAddress = 42;
const byte screenFontHeight = 8;
const byte screenFontWidth = 8;

const int verticalLines = verticalPixels / screenFontHeight / 2;  // double-height characters
const int horizontalPixels = horizontalBytes * screenFontWidth;

const byte verticalBackPorchLines = 35;  // includes sync pulse?
const int verticalFrontPorchLines = 525 - verticalBackPorchLines;

volatile int vLine;
volatile int messageLine;
volatile byte backPorchLinesToGo;

char message [verticalLines]  [horizontalBytes];

int timer;
byte next_screen;

enum STATE {WARMUP, START, P1} state;

//WARMUP vars
const int WARMUPTime = 64;

//START vars
int iter;
const char* Logo = "        ___   _   /\\   /\\/ _ \\ /_\\  \\ \\ / / /_\\///_\\\\  \\ V / /_\\\\/  _  \\  \\_/\\____/\\_/ \\_/   a r d u i n o   ";
/*   
        ___   _   
/\   /\/ _ \ /_\  
\ \ / / /_\///_\\ 
 \ V / /_\\/  _  \
  \_/\____/\_/ \_/
*/

const int LogoX = 18;
const int LogoY = 6;
const int LogoSize = LogoX * LogoY;
const int LogoOffsetX = (horizontalBytes - LogoX ) / 2;
const int LogoOffsetY = (verticalLines - LogoY ) / 2;

#define M(X,Y) (message[X][Y] & 1)
#define R(X,Y) ( M(X,Y+1) + M(X,Y) + M(X,Y-1))
#define COUNT(X,Y) (R(X,Y-1) + M(X+1,Y) + M(X-1,Y) + R(X,Y+1))

#define BIT(X) ((byte)((X) & 1))
inline byte Count(byte x, byte y) {
  char *r1 = message[x-1], *r2 = message[x], *r3 = message[x+1];
  return BIT(r1[y-1]) + BIT(r1[y]) + BIT(r1[y+1]) + BIT(r2[y-1]) + BIT(r2[y+1]) + BIT(r3[y-1]) + BIT(r3[y]) + BIT(r3[y+1]);
}
 
inline void LogicStep()
{
  int temp = 36;
  switch(state) {
  case WARMUP:
    if(next_screen == 0 && timer > WARMUPTime) {
      for(int i = 0; i < verticalLines; i++)
        for(int y = 0; y < horizontalBytes; y++) {
          message[i][y] = temp;
          temp = temp + 129;
          temp = temp & 255;
        }
      state = START;
      timer = 0;
    }
    break;
  case START:
    if(next_screen == 0 && (timer & 31) == 0) {
      message[iter / LogoX + LogoOffsetY][iter % LogoX + LogoOffsetX] = Logo[iter];
      iter++;
      if(iter >= LogoSize) {
        state = P1;
        timer = 0;
        iter = 0;
      }
    }
    break;
  case P1:
    if(next_screen == 0) {
      byte x = iter / (verticalLines - 1) + 1, y = iter % (verticalLines - 1) + 1;
      byte c = COUNT(x, y);
      switch(c) {
      case 2:
        if(message[x][y] == 0)
          break;
      case 3:
        message[x][y] = 1;
        break;
      default:
        message[x][y] = 0;
      }
      iter = (iter + 1) % ((verticalLines - 1) * (horizontalBytes - 1));
   }
   break;
  }
  next_screen = 1;
}

// ISR: Vsync pulse
ISR (TIMER1_OVF_vect)
{
  timer++;
  next_screen = 0;
//  LogicStep();

  vLine = 0;
  messageLine = 0;
  backPorchLinesToGo = verticalBackPorchLines;
} // end of TIMER1_OVF_vect

// ISR: Hsync pulse ... this interrupt merely wakes us up
ISR (TIMER2_OVF_vect)
{
} // end of TIMER2_OVF_vect

void setup()
{
  state = WARMUP;

  // initial message ... change to suit
  strcpy(message[5], "       Init..      ");
  strcpy(message[6], "    Please wait    ");

  // disable Timer 0
  TIMSK0 = 0;  // no interrupts on Timer 0
  OCR0A = 0;   // and turn it off
  OCR0B = 0;

  // Timer 1 - vertical sync pulses
  pinMode (vSyncPin, OUTPUT);
  Timer1::setMode (15, Timer1::PRESCALE_1024, Timer1::CLEAR_B_ON_COMPARE);
  OCR1A = 259;  // 16666 / 64 uS = 260 (less one)
  OCR1B = 0;    // 64 / 64 uS = 1 (less one)
  TIFR1 = bit (TOV1);   // clear overflow flag
  TIMSK1 = bit (TOIE1);  // interrupt on overflow on timer 1

  // Timer 2 - horizontal sync pulses
  pinMode (hSyncPin, OUTPUT);
  Timer2::setMode (7, Timer2::PRESCALE_8, Timer2::SET_B_ON_COMPARE);
  OCR2A = 63;   // 32 / 0.5 uS = 64 (less one)
  OCR2B = 7;    // 4 / 0.5 uS = 8 (less one)
  TIFR2 = bit (TOV2);   // clear overflow flag
  TIMSK2 = bit (TOIE2);  // interrupt on overflow on timer 2

  // Set up USART in SPI mode (MSPIM)

  // baud rate must be zero before enabling the transmitter
  UBRR0 = 0;  // USART Baud Rate Register
  pinMode (MSPIM_SCK, OUTPUT);   // set XCK pin as output to enable master mode
  UCSR0B = 0;
  UCSR0C = bit (UMSEL00) | bit (UMSEL01) | bit (UCPHA0) | bit (UCPOL0);  // Master SPI mode

  // prepare to sleep between horizontal sync pulses
  set_sleep_mode (SLEEP_MODE_IDLE);

}  // end of setup

// draw a single scan line
void doOneScanLine ()
{

  // after vsync we do the back porch
  if (backPorchLinesToGo) {
    LogicStep();
    backPorchLinesToGo--;
    return;
  }  // end still doing back porch

  // if all lines done, do the front porch
  if (vLine >= verticalPixels) {
    LogicStep();
    return;
  }

  // pre-load pointer for speed
  const register byte * linePtr = &screen_font [ (vLine >> 1) & 0x07 ] [0];
  register char * messagePtr =  & (message [messageLine] [0] );

  // how many pixels to send
  register byte i = horizontalBytes;

  // turn transmitter on
  UCSR0B = bit (TXEN0);  // transmit enable (starts transmitting white)

  // blit pixel data to screen
  while (i--)
    UDR0 = pgm_read_byte (linePtr + (* messagePtr++));

  // wait till done
  while (!(UCSR0A & bit(TXC0))) {
    LogicStep();
  }

  // disable transmit
  UCSR0B = 0;   // drop back to black

  // this line done
  vLine++;

  // every 16 pixels it is time to move to a new line in our text
  //  (because we double up the characters vertically)
  if ((vLine & 0xF) == 0)
    messageLine++;

}  // end of doOneScanLine

void loop()
{
  // sleep to ensure we start up in a predictable way
  sleep_mode ();
  doOneScanLine ();
}  // end of loop

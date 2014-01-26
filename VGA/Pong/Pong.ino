
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

// Timer 1 - Vertical sync

// output    OC1B   pin 16  (D10) <------- VSYNC

//   Period: 16.64 mS (60 Hz)
//      1/60 * 1e6 = 16666.66 uS
//   Pulse for 64 uS  (2 x HSync width of 32 uS)
//    Sync pulse: 2 lines
//    Back porch: 33 lines
//    Active video: 480 lines
//    Front porch: 10 lines
//       Total: 525 lines

// Timer 2 - Horizontal sync

// output    OC2B   pin 5  (D3)   <------- HSYNC

//   Period: 32 uS (31.25 kHz)
//      (1/60) / 525 * 1e6 = 31.74 uS
//   Pulse for 4 uS (96 times 39.68 nS)
//    Sync pulse: 96 pixels
//    Back porch: 48 pixels
//    Active video: 640 pixels
//    Front porch: 16 pixels
//       Total: 800 pixels

// Pixel time =  ((1/60) / 525 * 1e9) / 800 = 39.68  nS
//  frequency =  1 / (((1/60) / 525 * 1e6) / 800) = 25.2 MHz

// However in practice, it is the SPI speed, namely a period of 125 nS
//     (that is 2 x system clock speed)
//   giving an 8 MHz pixel frequency. Thus the characters are about 3 times too wide.
// Thus we fit 160 of "our" pixels on the screen in what usually takes 3 x 160 = 480

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

int count;

char facingx;
char facingy;
unsigned char x;
unsigned char y;
#define printByte(B,X,Y) {message[X][Y] = ((B)/(8*8)) % 8 + '0'; message[X][Y+1] = ((B)/8) % 8 + '0'; message[X][Y+2] = (B) % 8 + '0';}

const byte p1PINUp = 5;
const byte p1PINDo = 6;

const byte p2PINUp = 11;
const byte p2PINDo = 12;

unsigned char p1y,p2y;
unsigned int p1p,p2p;

const int BallSpeed = 2;
byte ball;

// ISR: Vsync pulse
ISR (TIMER1_OVF_vect)
  {
    //Pileczka
  switch(ball) {
    case 0:
      ball = BallSpeed;
      message[x][y] = 0;
      switch(x) {
        case verticalLines-1:
        case 0:
          facingx = -facingx;
        default:
          x = (x + facingx) % (verticalLines); 
          break;
      }
      switch(y) {
        case 0:
          switch(p1y - x) {
            case (-1):
              facingx = 1;
              facingy = 1;
              y = 1;
              break;
            case 1:
              facingx = -1;
              facingy = 1;
              y = 1;
              break;
            case 0:
              facingy = 1;
              y = 1;
              break;
            default:
              p2p++;
              facingx = 0;
              facingy = 1;
              y = y + 1;
              x = p1y;
              ball = 30;
          }
          break;
        case horizontalBytes-1:
          switch(p2y - x) {
            case (-1):
              facingx = 1;
              facingy = -1;
              y = y-1;
              break;
            case 1:
              facingx = -1;
              facingy = -1;
              y = y-1;
              break;
            case 0:
              facingy = -1;
              y = y-1;
              break;
            default:
              p1p++;
              facingx = 0;
              facingy = -1;
              y = y-1;
              x = p2y;
              ball = 30;
          } 
          break;     
          //facingy = -facingy;
        default:
          y = (y + facingy) % horizontalBytes;
          break;
      }
      message[x][y] = 'O';
      break;
    default:
      ball = ball - 1;
  }
  
  message[p1y-1][0] = message[p1y][0] = message[p1y+1][0] = 0;
  message[p2y-1][horizontalBytes-1] = message[p2y][horizontalBytes-1] = message[p2y+1][horizontalBytes-1] = 0;

  //Ruch p1:
  if(digitalRead(p1PINUp) == HIGH) {
      if(p1y > 1)  
        p1y = p1y - 1;
  }
  if(digitalRead(p1PINDo) == HIGH) {
      if(p1y < verticalLines-2)
        p1y = p1y + 1;
  }
  //Ruch p2:
  if(digitalRead(p2PINUp) == HIGH) {
      if(p2y > 1)  
        p2y = p2y - 1;
  }
  if(digitalRead(p2PINDo) == HIGH) {
      if(p2y < verticalLines-2)
        p2y = p2y + 1;
  }
  
  message[p1y-1][0] = message[p1y][0] = message[p1y+1][0] = 'H';
  message[p2y-1][horizontalBytes-1] = message[p2y][horizontalBytes-1] = message[p2y+1][horizontalBytes-1] = 'H';
  
  //Tekst
  count++;
  count = count % (8*8*8*8);
  message[0][0] = (count / (8*8*8)) % 8 + '0'; printByte(count%(8*8*8), 0, 1);
  const int pointOffset = 6;
  printByte(p1p, 0, pointOffset); message[0][pointOffset + 3] = ':'; printByte(p2p, 0, pointOffset + 4);
  
  //Display
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
  
  // initial message ... change to suit
  
  y = horizontalBytes / 2;
  x = verticalLines / 2;
  facingx = facingy = 1;
  
  p1y = p2y = verticalLines / 2;
  message[p1y-1][0] = message[p1y][0] = message[p1y+1][0] = 'H';
  message[p2y-1][horizontalBytes-1] = message[p2y][horizontalBytes-1] = message[p2y+1][horizontalBytes-1] = 'H';
  
  
  pinMode(p1PINUp,INPUT);
  pinMode(p1PINDo,INPUT);
  pinMode(p2PINUp,INPUT);
  pinMode(p2PINDo,INPUT);
  
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

#define nop asm volatile("nop\n\t")

// draw a single scan line
void doOneScanLine ()
  {
    
  // after vsync we do the back porch
  if (backPorchLinesToGo)
    {
    backPorchLinesToGo--;
    return;   
    }  // end still doing back porch
    
  // if all lines done, do the front porch
  if (vLine >= verticalPixels) {
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
  while (!(UCSR0A & bit(TXC0))) 
    {}
  
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

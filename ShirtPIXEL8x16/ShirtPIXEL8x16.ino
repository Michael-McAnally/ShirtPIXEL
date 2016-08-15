/*=======================================================================*/
// PIXELWearables.co Free code following and respecting all licenses below
// By: Michael McAnally Aug 15, 2016 Purpose: Prototype ShirtPIXEL 1.0
// These code comments should not be removed as well.
/*=======================================================================*/
/*********************************************************************
 This is an example for our nRF51822 based Bluefruit LE modules

 Pick one up today in the adafruit shop!

 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/

#include <Arduino.h>
#include <SPI.h>
#if not defined (_VARIANT_ARDUINO_DUE_X_) && not defined (_VARIANT_ARDUINO_ZERO_)
  #include <SoftwareSerial.h>
#endif

#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

#include "BluefruitConfig.h"


// Adafruit_NeoMatrix example for single NeoPixel Shield.
// Scrolls 'Text' across the matrix in a portrait (vertical) orientation.

#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>


/*=========================================================================
    APPLICATION SETTINGS

    FACTORYRESET_ENABLE       Perform a factory reset when running this sketch
   
                              Enabling this will put your Bluefruit LE module
                              in a 'known good' state and clear any config
                              data set in previous sketches or projects, so
                              running this at least once is a good idea.
   
                              When deploying your project, however, you will
                              want to disable factory reset by setting this
                              value to 0.  If you are making changes to your
                              Bluefruit LE device via AT commands, and those
                              changes aren't persisting across resets, this
                              is the reason why.  Factory reset will erase
                              the non-volatile memory where config data is
                              stored, setting it back to factory default
                              values.
       
                              Some sketches that require you to bond to a
                              central device (HID mouse, keyboard, etc.)
                              won't work at all with this feature enabled
                              since the factory reset will clear all of the
                              bonding data stored on the chip, meaning the
                              central device won't be able to reconnect.
    MINIMUM_FIRMWARE_VERSION  Minimum firmware version to have some new features
    MODE_LED_BEHAVIOUR        LED activity, valid options are
                              "DISABLE" or "MODE" or "BLEUART" or
                              "HWUART"  or "SPI"  or "MANUAL"
    -----------------------------------------------------------------------*/
    #define FACTORYRESET_ENABLE         1
    #define MINIMUM_FIRMWARE_VERSION    "0.6.6"
    #define MODE_LED_BEHAVIOUR          "MODE"
/*=========================================================================*/

// Create the bluefruit object, either software serial...uncomment these lines
/*
SoftwareSerial bluefruitSS = SoftwareSerial(BLUEFRUIT_SWUART_TXD_PIN, BLUEFRUIT_SWUART_RXD_PIN);

Adafruit_BluefruitLE_UART ble(bluefruitSS, BLUEFRUIT_UART_MODE_PIN,
                      BLUEFRUIT_UART_CTS_PIN, BLUEFRUIT_UART_RTS_PIN);
*/

/* ...or hardware serial, which does not need the RTS/CTS pins. Uncomment this line */
// Adafruit_BluefruitLE_UART ble(BLUEFRUIT_HWSERIAL_NAME, BLUEFRUIT_UART_MODE_PIN);

/* ...hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST */
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

/* ...software SPI, using SCK/MOSI/MISO user-defined SPI pins and then user selected CS/IRQ/RST */
//Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_SCK, BLUEFRUIT_SPI_MISO,
//                             BLUEFRUIT_SPI_MOSI, BLUEFRUIT_SPI_CS,
//                             BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);


// Adafriut defines . . .

#define PIN 6


// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      128

Adafruit_NeoPixel pixel = Adafruit_NeoPixel(NUMPIXELS, PIN);

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


// Example for NeoPixel Shield.  In this application we'd like to use it
// as a 5x8 tall matrix, with the USB port positioned at the top of the
// Arduino.  When held that way, the first pixel is at the top right, and
// lines are arranged in columns, progressive order.  The shield uses
// 800 KHz (v2) pixels that expect GRB color data.
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(16, 8, PIN,
  NEO_MATRIX_TOP     + NEO_MATRIX_RIGHT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE,
  NEO_GRB            + NEO_KHZ800);

const uint16_t colors[] = {
  matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(0, 0, 255) };



int pos = 0;  // pos in array
int x = 8;  // begining position of text scrolling in matrix
int spaces = -160;
int pass = 0;  // number of passes for txt color switching
int swapanimation = 0;  // used to swap between 2 different animations
int gaypride = 0;  // used swithch to show scrolling and alternating animations


int R1[] = {25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35};
int R2[] = {24, 23, 22, 21, 20, 19, 18, 17, 16, 15};
int R3[] = {7, 8, 9, 10, 11, 12, 13};
int R4[] = {6, 5, 4, 3, 2, 1, 0};
int L1[] = {46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36};
int L2[] = {47, 48, 49, 50, 51, 52, 53, 54, 55, 56};
int L3[] = {64, 63, 62, 61, 60, 59, 58, 57};
int L4[] = {65, 66, 67, 68, 69, 70, 71};

//Color

//Color
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t compRed;
    uint8_t compGreen;
    uint8_t compBlue;
    uint8_t triadRed1;
    uint8_t triadRed2;
    uint8_t triadGreen1;
    uint8_t triadGreen2;
    uint8_t triadBlue1;
    uint8_t triadBlue2;
    uint8_t animationState = 1;

char BLE_arr[] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};  //char array for txting display


////////////////////////////////////////////////// Animation Data Area Below
//  This structure of the animation data for animations is as follows:
//
//
//   There are four (4) basic arrays which make up an 8 frame animation
//
//   The first array is a binary array, this defines the on/off shape of the 8 individual frames of the animation, example: a smiley face, then a frown face, etc.
//   you can create the binary array easily using the following program in github:
//
//                 
//   https://github.com/g-mu/8x8-Animator
//
//
//   remove the delay data, since we don't use it that way...
//
//
//   Now the remaining arrays are color value arrays for Red, Green and Blue. RGB.  By mixing the RGB values you can create many colors.
//   Here is a link to a color table with values... there are many tables out there, I like this one.
//
//
//   https://forums.adafruit.com/viewtopic.php?f=8&t=80684
//
//
//
//   RULE: so basically remember, while designing animations a pixel has to be "1" in the binary table, and have a non-zero value in at least one of the RGB tables.
//
//
//
//   Example call:
//
//          for (int loop = 0 ; loop < 10 ; loop++ ) {
//                AnimateFrames(BeatingHeart, RedBeatingHeart, GreenBeatingHeart, BlueBeatingHeart);       }
//
//
//
//
//
//
//
//   void AnimateFrames( int Bits[64], int Red[][8], int  Green[][8], int Blue[][8])
//       {
//         int offset = 0;
//            
//         for (int idx=0; idx<8; ++idx) {
//            
//            offset = idx * 8;
//            
//        for (byte x=0; x<8; ++x) {
//
//         byte data = pgm_read_byte (&Bits[x + offset]);   // fetch data from program memory
//
//            for (byte y=0; y<8; ++y) {
//    
//                if (data & (1<<y)) {
// 
//                    
//                    matrix.drawPixel(y, x, matrix.Color(Red[x + offset][y], Green[x + offset][y], Blue[x + offset][y])); // x, y, color
//                } else {
//
//                    
//                    matrix.drawPixel(y, x, 0); // x, y, color
//                }
//            }
//        }
//       
//        matrix.show();
//       
//        delay(400);
//      }
//      
//   }
//
//       
//
//
//

 

// Yellow Emoji Faces

int Emoji[64] = {

  // 1. Tongue
  B00111100,
  B01000010,
  B10100101,
  B10000001,
  B11111111,
  B10011001,
  B01000010,
  B00111100,  
  
  // 2. smile big
  B00111100,
  B01111110,
  B11111111,
  B11111111,
  B11111111,
  B11111111,
  B01111110,
  B00111100,

  // 3. Oh!
  B00111100,
  B01111110,
  B11111111,
  B11111111,
  B11111111,
  B11100111,
  B01111110,
  B00111100,

  // 4. Big Oh!
  B00111100,
  B01111110,
  B11111111,
  B11111111,
  B11100111,
  B11100111,
  B01111110,
  B00111100,

  // 5. Shock!
  B00111100,
  B01111110,
  B11111111,
  B11111111,
  B11100111,
  B11100111,
  B01111110,
  B00111100,

  // 6. Frown
  B00111100,
  B01111110,
  B11111111,
  B11111111,
  B11111111,
  B11111111,
  B01111110,
  B00111100,
 
  // 7. Not Happy
  B00111100,
  B01111110,
  B11111111,
  B11111111,
  B11111111,
  B11111111,
  B01111110,
  B00111100,

  // 8. Sad
  B00111100,
  B01111110,
  B11111111,
  B11111111,
  B11111111,
  B11111111,
  B01111110,
  B00111100
  

};

int RedEmoji[64][8] = {   // store in program memory to save RAM

  // Red Animation bitmap.  Each number represents the value of Red in RGB image in the BitMap.
  // if the Bit map has a 1 in the posion the color will light up.  if not, it will still not light up...
  // 8 lines of color data for 8 frames (there is no error checking for
  // length).


    // 1. Tongue
  {255,255,255,255,255,255,255,255},
  {255,255,255,255,255,255,255,255},
  {255,255,0,255,255,0,255,255},
  {255,255,255,255,255,255,255,255},
  {255,255,255,255,255,255,255,255},
  {255,255,255,255,255,255,255,255},
  {255,255,255,255,255,255,255,255},
  {255,255,255,255,255,255,255,255},

  // 2. Smile Big
  {255,255,255,255,255,255,255,255},
  {255,255,255,255,255,255,255,255},
  {255,255,0,255,255,0,255,255},
  {255,255,255,255,255,255,255,255},
  {255,255,255,255,255,255,255,255},
  {255,255,255,0,0,255,255,255},
  {255,255,255,255,255,255,255,255},
  {255,255,255,255,255,255,255,255},

  // 3. Oh!
  {255,255,255,255,255,255,255,255},
  {255,255,255,255,255,255,255,255},
  {255,255,0,255,255,0,255,255},
  {255,255,255,255,255,255,255,255},
  {255,255,255,255,255,255,255,255},
  {255,255,255,255,255,255,255,255},
  {255,255,255,255,255,255,255,255},
  {255,255,255,255,255,255,255,255},

  // 4.Big Oh!
  {255,255,255,255,255,255,255,255},
  {255,255,255,255,255,255,255,255},
  {255,255,0,255,255,0,255,255},
  {255,255,255,255,255,255,255,255},
  {255,255,255,255,255,255,255,255},
  {255,255,255,255,255,255,255,255},
  {255,255,255,255,255,255,255,255},
  {255,255,255,255,255,255,255,255},

    // 5. Shock!
  {255,255,255,255,255,255,255,255},
  {255,255,255,255,255,255,255,255},
  {255,255,0,255,255,0,255,255},
  {255,255,255,255,255,255,255,255},
  {255,255,255,255,255,255,255,255},
  {255,255,255,255,255,255,255,255},
  {255,255,255,255,255,255,255,255},
  {255,255,255,255,255,255,255,255},

  // 6. Frown
  {255,255,255,255,255,255,255,255},
  {255,255,255,255,255,255,255,255},
  {255,255,0,255,255,0,255,255},
  {255,255,255,255,255,255,255,255},
  {255,255,255,255,255,255,255,255},
  {255,255,255,255,255,255,255,255},
  {255,255,255,255,255,255,255,255},
  {255,255,255,255,255,255,255,255},

  // 7. Not Happy
  {255,255,255,255,255,255,255,255},
  {255,255,255,255,255,255,255,255},
  {255,255,0,255,255,0,255,255},
  {255,255,255,255,255,255,255,255},
  {255,255,102,102,102,102,255,255},
  {255,102,102,255,255,102,102,255},
  {255,255,255,255,255,255,255,255},
  {255,255,255,255,255,255,255,255}, 

  // 8. Sad
  {255,255,255,255,255,255,255,255},
  {255,255,255,255,255,255,255,255},
  {255,255,0,255,255,0,255,255},
  {255,51,255,255,255,255,51,255},
  {255,255,102,102,102,102,255,255},
  {255,102,102,255,255,102,102,255},
  {255,255,255,255,255,255,255,255},
  {255,255,255,255,255,255,255,255}    //no comma on last entry
        
    };

int GreenEmoji[64][8] = {   // store in program memory to save RAM

  // Green Animation bitmap.  Each number represents the value of Green in RGB image in the BitMap.
  // if the Bit map has a 1 in the posion the color will light up.  if not, it will still not light up...
  // 8 lines of color data for 8 frames (there is no error checking for
  // length).

    // 1. Tongue
  {204,204,204,204,204,204,204,204},
  {204,204,204,204,204,204,204,204},
  {204,204,0,204,204,0,204,204},
  {204,204,204,204,204,204,204,204},
  {204,204,204,204,204,204,204,204},
  {204,204,51,51,51,51,204,204},
  {204,204,204,51,51,204,204,204},
  {204,204,204,51,51,204,204,204},
        
  // 2. Smile Big
  {204,204,204,204,204,204,204,204},
  {204,204,204,204,204,204,204,204},
  {204,204,0,204,204,0,204,204},
  {204,204,204,204,204,204,204,204},
  {204,51,51,51,51,51,51,204},
  {204,204,51,0,0,51,204,204},
  {204,204,204,51,51,204,204,204},
  {204,204,204,204,204,204,204,204},

  // 3. Oh!
  {204,204,204,204,204,204,204,204},
  {204,204,204,204,204,204,204,204},
  {204,204,0,204,204,0,204,204},
  {204,204,204,204,204,204,204,204},
  {204,204,204,51,51,204,204,204},
  {204,204,51,0,0,51,204,204},
  {204,204,204,51,51,204,204,204},
  {204,204,204,204,204,204,204,204},

    // 4. Big Oh!
  {204,204,204,204,204,204,204,204},
  {204,204,204,204,204,204,204,204},
  {204,204,0,204,204,0,204,204},
  {204,204,204,51,51,204,204,204},
  {204,204,51,0,0,51,204,204},
  {204,204,51,0,0,51,204,204},
  {204,204,204,51,51,204,204,204},
  {204,204,204,204,204,204,204,204},


  // 5. Shock!
  {204,204,204,204,204,204,204,204},
  {204,204,255,204,204,255,204,204},
  {204,255,0,255,255,0,255,204},
  {204,204,255,51,51,255,204,204},
  {204,204,51,0,0,51,204,204},
  {204,204,51,0,0,51,204,204},
  {204,204,204,51,51,204,204,204},
  {204,204,204,204,204,204,204,204},
 
  // 6. Frown
  {204,204,204,204,204,204,204,204},
  {204,204,204,204,204,204,204,204},
  {204,204,0,204,204,0,204,204},
  {204,204,204,204,204,204,204,204},
  {204,204,204,51,51,204,204,204},
  {204,204,51,51,51,51,204,204},
  {204,204,204,204,204,204,204,204},
  {204,204,204,204,204,204,204,204},

  // 7. Not Happy
  {204,204,204,204,204,204,204,204},
  {204,204,204,204,204,204,204,204},
  {204,204,0,204,204,0,204,204},
  {204,204,204,204,204,204,204,204},
  {204,204,51,51,51,51,204,204},
  {204,51,51,204,204,51,51,204},
  {204,204,204,204,204,204,204,204},
  {204,204,204,204,204,204,204,204},

  // 8. Sad
  {204,204,204,204,204,204,204,204},
  {204,204,204,204,204,204,204,204},
  {204,204,0,204,204,0,204,204},
  {204,102,204,204,204,204,102,204},
  {204,204,51,51,51,51,204,204},
  {204,51,51,204,204,51,51,204},
  {204,204,204,204,204,204,204,204},
  {204,204,204,204,204,204,204,204}   // no comma on last entry
        
    }; 



int BlueEmoji[64][8] = {   // store in program memory to save RAM

  // Blue Animation bitmap.  Each number represents the value of Blue in RGB image in the BitMap.
  // if the Bit map has a 1 in the posion the color will light up.  if not, it will still not light up...
  // 8 lines of color data for 8 frames (there is no error checking for
  // length).



    // 1. Tongue
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,255,0,0,255,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,51,51,51,51,0,0},
  {0,0,0,51,51,0,0,0},
  {0,0,0,51,51,0,0,0},
     
  // 2. Smile Big
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,255,0,0,255,0,0},
  {0,0,0,0,0,0,0,0},
  {0,51,51,51,51,51,51,0},
  {0,0,51,0,0,51,0,0},
  {0,0,0,51,51,0,0,0},
  {0,0,0,0,0,0,0,0},
 
   // 3. Oh!
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,255,0,0,255,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,51,51,0,0,0},
  {0,0,51,0,0,51,0,0},
  {0,0,0,51,51,0,0,0},
  {0,0,0,0,0,0,0,0},

     // 4. Big Oh!
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,255,0,0,255,0,0},
  {0,0,0,51,51,0,0,0},
  {0,0,51,0,0,51,0,0},
  {0,0,51,0,0,51,0,0},
  {0,0,0,51,51,0,0,0},
  {0,0,0,0,0,0,0,0},

   // 5. Shock!
  {0,0,0,0,0,0,0,0},
  {0,0,255,0,0,255,0,0},
  {0,255,255,255,255,255,255,0},
  {0,0,255,51,51,255,0,0},
  {0,0,51,0,0,51,0,0},
  {0,0,51,0,0,51,0,0},
  {0,0,0,51,51,0,0,0},
  {0,0,0,0,0,0,0,0},

   // 6. Frown
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,255,0,0,255,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,51,51,0,0,0},
  {0,0,51,51,51,51,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},

  // 7. Not Happy
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,255,0,0,255,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,51,51,51,51,0,0},
  {0,51,51,0,0,51,51,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},

  
  // 8. Sad
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,255,0,0,255,0,0},
  {0,204,0,0,0,0,204,0},
  {0,0,51,51,51,51,0,0},
  {0,51,51,0,0,51,51,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0}  // no comma on last entry
        
    };


// Beating Heart

int BeatingHeart[64] = {

  // Frame 1
  B01100110,
  B11111111,
  B11111111,
  B11111111,
  B11111111,
  B01111110,
  B00111100,
  B00011000,
  

  // Frame 2
  B00000000,
  B00100100,
  B01111110,
  B01111110,
  B01111110,
  B00111100,
  B00011000,
  B00000000,
  
  // Frame 3
  B01100110,
  B11111111,
  B11111111,
  B11111111,
  B11111111,
  B01111110,
  B00111100,
  B00011000,
  

  // Frame 4
  B00000000,
  B00100100,
  B01111110,
  B01111110,
  B01111110,
  B00111100,
  B00011000,
  B00000000,

  // Frame 5
  B01100110,
  B11111111,
  B11111111,
  B11111111,
  B11111111,
  B01111110,
  B00111100,
  B00011000,
  

  // Frame 6
  B00000000,
  B00100100,
  B01111110,
  B01111110,
  B01111110,
  B00111100,
  B00011000,
  B00000000,

  // Frame 7
  B01100110,
  B11111111,
  B11111111,
  B11111111,
  B11111111,
  B01111110,
  B00111100,
  B00011000,
  

  // Frame 8
  B00000000,
  B00100100,
  B01111110,
  B01111110,
  B01111110,
  B00111100,
  B00011000,
  B00000000

};


int RedBeatingHeart[64][8] = {   // store in program memory to save RAM

  // Red Animation bitmap.  Each number represents the value of Red in RGB image in the BitMap.
  // if the Bit map has a 1 in the posion the color will light up.  if not, it will still not light up...
  // 8 lines of color data for 8 frames (there is no error checking for
  // length).

  // Frame 1
  {0,100,100,0,0,100,100,0},
  {100,100,100,100,100,100,100,100},
  {100,100,100,100,100,100,100,100},
  {100,100,100,100,100,100,100,100},
  {100,100,100,100,100,100,100,100},
  {0,100,100,100,100,100,100,0},
  {0,0,100,100,100,100,0,0},
  {0,0,0,100,100,0,0,0},
  

  // Frame 2
  {0,0,0,0,0,0,0,0},
  {0,0,100,0,0,100,0,0},
  {0,100,100,100,100,100,100,0},
  {0,100,100,100,100,100,100,0},
  {0,100,100,100,100,100,100,0},
  {0,0,100,100,100,100,0,0},
  {0,0,0,100,100,0,0,0},
  {0,0,0,0,0,0,0,0},
  
  // Frame 3
  {0,100,100,0,0,100,100,0},
  {100,100,100,100,100,100,100,100},
  {100,100,100,100,100,100,100,100},
  {100,100,100,100,100,100,100,100},
  {100,100,100,100,100,100,100,100},
  {0,100,100,100,100,100,100,0},
  {0,0,100,100,100,100,0,0},
  {0,0,0,100,100,0,0,0},
  

  // Frame 4
  {0,0,0,0,0,0,0,0},
  {0,0,100,0,0,100,0,0},
  {0,100,100,100,100,100,100,0},
  {0,100,100,100,100,100,100,0},
  {0,100,100,100,100,100,100,0},
  {0,0,100,100,100,100,0,0},
  {0,0,0,100,100,0,0,0},
  {0,0,0,0,0,0,0,0},

  // Frame 5
  {0,100,100,0,0,100,100,0},
  {100,100,100,100,100,100,100,100},
  {100,100,100,100,100,100,100,100},
  {100,100,100,100,100,100,100,100},
  {100,100,100,100,100,100,100,100},
  {0,100,100,100,100,100,100,0},
  {0,0,100,100,100,100,0,0},
  {0,0,0,100,100,0,0,0},
  

  // Frame 6
  {0,0,0,0,0,0,0,0},
  {0,0,100,0,0,100,0,0},
  {0,100,100,100,100,100,100,0},
  {0,100,100,100,100,100,100,0},
  {0,100,100,100,100,100,100,0},
  {0,0,100,100,100,100,0,0},
  {0,0,0,100,100,0,0,0},
  {0,0,0,0,0,0,0,0},

  // Frame 7
  {0,100,100,0,0,100,100,0},
  {100,100,100,100,100,100,100,100},
  {100,100,100,100,100,100,100,100},
  {100,100,100,100,100,100,100,100},
  {100,100,100,100,100,100,100,100},
  {0,100,100,100,100,100,100,0},
  {0,0,100,100,100,100,0,0},
  {0,0,0,100,100,0,0,0},
  

  // Frame 8
  {0,0,0,0,0,0,0,0},
  {0,0,100,0,0,100,0,0},
  {0,100,100,100,100,100,100,0},
  {0,100,100,100,100,100,100,0},
  {0,100,100,100,100,100,100,0},
  {0,0,100,100,100,100,0,0},
  {0,0,0,100,100,0,0,0},
  {0,0,0,0,0,0,0,0}   // no comma on last entry
        
    };

int GreenBeatingHeart[64][8] = {   // store in program memory to save RAM

  // Green Animation bitmap.  Each number represents the value of Green in RGB image in the BitMap.
  // if the Bit map has a 1 in the posion the color will light up.  if not, it will still not light up...
  // 8 lines of color data for 8 frames (there is no error checking for
  // length).

        
        {0,0,0,0,0,0,0,0}, 
        {0,0,0,0,0,0,0,0}, 
        {0,0,0,0,0,0,0,0}, 
        {0,0,0,0,0,0,0,0},    
        {0,0,0,0,0,0,0,0}, 
        {0,0,0,0,0,0,0,0}, 
        {0,0,0,0,0,0,0,0}, 
        {0,0,0,0,0,0,0,0}, 
  
        {0,0,0,0,0,0,0,0}, 
        {0,0,0,0,0,0,0,0}, 
        {0,0,0,0,0,0,0,0}, 
        {0,0,0,0,0,0,0,0},    
        {0,0,0,0,0,0,0,0}, 
        {0,0,0,0,0,0,0,0}, 
        {0,0,0,0,0,0,0,0}, 
        {0,0,0,0,0,0,0,0}, 
        
        {0,0,0,0,0,0,0,0}, 
        {0,0,0,0,0,0,0,0}, 
        {0,0,0,0,0,0,0,0}, 
        {0,0,0,0,0,0,0,0},    
        {0,0,0,0,0,0,0,0}, 
        {0,0,0,0,0,0,0,0}, 
        {0,0,0,0,0,0,0,0}, 
        {0,0,0,0,0,0,0,0}, 
  
        {0,0,0,0,0,0,0,0}, 
        {0,0,0,0,0,0,0,0}, 
        {0,0,0,0,0,0,0,0}, 
        {0,0,0,0,0,0,0,0},    
        {0,0,0,0,0,0,0,0}, 
        {0,0,0,0,0,0,0,0}, 
        {0,0,0,0,0,0,0,0}, 
        {0,0,0,0,0,0,0,0}, 

        {0,0,0,0,0,0,0,0}, 
        {0,0,0,0,0,0,0,0}, 
        {0,0,0,0,0,0,0,0}, 
        {0,0,0,0,0,0,0,0},    
        {0,0,0,0,0,0,0,0}, 
        {0,0,0,0,0,0,0,0}, 
        {0,0,0,0,0,0,0,0}, 
        {0,0,0,0,0,0,0,0}, 
  
        {0,0,0,0,0,0,0,0}, 
        {0,0,0,0,0,0,0,0}, 
        {0,0,0,0,0,0,0,0}, 
        {0,0,0,0,0,0,0,0},    
        {0,0,0,0,0,0,0,0}, 
        {0,0,0,0,0,0,0,0}, 
        {0,0,0,0,0,0,0,0}, 
        {0,0,0,0,0,0,0,0}, 
        
        {0,0,0,0,0,0,0,0}, 
        {0,0,0,0,0,0,0,0}, 
        {0,0,0,0,0,0,0,0}, 
        {0,0,0,0,0,0,0,0},    
        {0,0,0,0,0,0,0,0}, 
        {0,0,0,0,0,0,0,0}, 
        {0,0,0,0,0,0,0,0}, 
        {0,0,0,0,0,0,0,0}, 
  
        {0,0,0,0,0,0,0,0}, 
        {0,0,0,0,0,0,0,0}, 
        {0,0,0,0,0,0,0,0}, 
        {0,0,0,0,0,0,0,0},    
        {0,0,0,0,0,0,0,0}, 
        {0,0,0,0,0,0,0,0}, 
        {0,0,0,0,0,0,0,0}, 
        {0,0,0,0,0,0,0,0}, 
        
    };


int BlueBeatingHeart[64][8] = {   // store in program memory to save RAM

  // Blue Animation bitmap.  Each number represents the value of Blue in RGB image in the BitMap.
  // if the Bit map has a 1 in the posion the color will light up.  if not, it will still not light up...
  // 8 lines of color data for 8 frames (there is no error checking for
  // length).

        
  // Frame 1
  {0,0,0,0,0,0,0,0},
  {0,0,100,0,0,100,0,0},
  {0,100,100,100,100,100,100,0},
  {0,100,100,100,100,100,100,0},
  {0,100,100,100,100,100,100,0},
  {0,0,100,100,100,100,0,0},
  {0,0,0,100,100,0,0,0},
  {0,0,0,0,0,0,0,0},
  

  // Frame 2
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,100,0,0,100,0,0},
  {0,0,100,100,100,100,0,0},
  {0,0,100,100,100,100,0,0},
  {0,0,0,100,100,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  
  
  // Frame 3
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,1,1,1,1,0,0},
  {0,0,0,1,1,0,0,0},
  {0,0,0,1,1,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  

  // Frame 4
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,100,0,0,100,0,0},
  {0,0,100,100,100,100,0,0},
  {0,0,100,100,100,100,0,0},
  {0,0,0,100,100,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},

  // Frame 5
  {0,0,0,0,0,0,0,0},
  {0,0,100,0,0,100,0,0},
  {0,100,100,100,100,100,100,0},
  {0,100,100,100,100,100,100,0},
  {0,100,100,100,100,100,100,0},
  {0,0,100,100,100,100,0,0},
  {0,0,0,100,100,0,0,0},
  {0,0,0,0,0,0,0,0},
  

  // Frame 6
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,100,0,0,100,0,0},
  {0,0,100,100,100,100,0,0},
  {0,0,100,100,100,100,0,0},
  {0,0,0,100,100,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},

  // Frame 7
  {0,0,0,0,0,0,0,0},
  {0,0,100,0,0,100,0,0},
  {0,100,100,100,100,100,100,0},
  {0,100,100,100,100,100,100,0},
  {0,100,100,100,100,100,100,0},
  {0,0,100,100,100,100,0,0},
  {0,0,0,100,100,0,0,0},
  {0,0,0,0,0,0,0,0},
  

  // Frame 8
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,100,0,0,100,0,0},
  {0,0,100,100,100,100,0,0},
  {0,0,100,100,100,100,0,0},
  {0,0,0,100,100,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0}   // no comma on last entry
        
    };


int Alien[64] = {   // store in program memory to save RAM

 
  // Animation bitmaps.  Each frame of animation MUST contain
  // 8 lines of graphics data (there is no error checking for
  // length).  Each line should be prefixed with the letter 'B',
  // followed by exactly 8 binary digits (0 or 1), no more,
  // no less (again, no error checking).  '0' represents an
  // 'off' pixel, '1' an 'on' pixel. Note: There are currently 8 frames of animation available.
 
  B00100100, // First frame for alien #3
  B00100100,
  B01111110,
  B11111111,
  B11111111,
  B11111111,
  B10100101,
  B00100100,
  
  B00100100, // Second frame for alien #3
  B10100101,
  B11111111,
  B11111111,
  B11111111,
  B01111110,
  B00100100,
  B01000010,
   
  B00100100, // First frame for alien #3
  B00100100,
  B01111110,
  B11111111,
  B11111111,
  B11111111,
  B10100101,
  B00100100,
  
  B00100100, // Second frame for alien #3
  B10100101,
  B11111111,
  B11111111,
  B11111111,
  B01111110,
  B00100100,
  B01000010,
 
  B00100100, // First frame for alien #3
  B00100100,
  B01111110,
  B11111111,
  B11111111,
  B11111111,
  B10100101,
  B00100100,

  B00100100, // Second frame for alien #3
  B10100101,
  B11111111,
  B11111111,
  B11111111,
  B01111110,
  B00100100,
  B01000010,
  
  B00100100, // First frame for alien #3
  B00100100,
  B01111110,
  B11111111,
  B11111111,
  B11111111,
  B10100101,
  B00100100,
  
  B00100100, // Second frame for alien #3
  B10100101,
  B11111111,
  B11111111,
  B11111111,
  B01111110,
  B00100100,
  B01000010
  
};



 


int RedAlien[64][8] = {   // store in program memory to save RAM

  // Red Animation bitmap.  Each number represents the value of Red in RGB image in the BitMap.
  // if the Bit map has a 1 in the posion the color will light up.  if not, it will still not light up...
  // 8 lines of color data for 8 frames (there is no error checking for
  // length).

        {0,0,0,0,0,0,0,0}, // Frame 1
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,100,0,0,100,0,0},   //Red Eyes
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
  
        {0,0,0,0,0,0,0,0}, // Frame 2 
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        
        {0,0,0,0,0,0,0,0}, // Frame 3 
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,100,0,0,100,0,0},   //Red Eyes
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
  
        {0,0,0,0,0,0,0,0}, // Frame 4 
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},

        {0,0,0,0,0,0,0,0}, // Frame 5 
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,100,0,0,100,0,0},   //Red Eyes
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
  
        {0,0,0,0,0,0,0,0}, // Frame 6 
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},

        {0,0,0,0,0,0,0,0}, // Frame 7 
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,100,0,0,100,0,0},   //Red Eyes
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
  
        {0,0,0,0,0,0,0,0}, // Frame 8 
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0}  // no comma on last entry
        
    };

int GreenAlien[64][8] = {   // store in program memory to save RAM

  // Green Animation bitmap.  Each number represents the value of Green in RGB image in the BitMap.
  // if the Bit map has a 1 in the posion the color will light up.  if not, it will still not light up...
  // 8 lines of color data for 8 frames (there is no error checking for
  // length).

        
        {0,0,100,0,0,100,0,0}, // First frame for alien #3
        {0,0,100,0,0,100,0,0},
        {0,100,100,100,100,100,100,0},
        {100,100,0,100,100,0,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,0,100,0,0,100,0,100},
        {0,0,100,0,0,100,0,0},
  
        {0,0,100,0,0,100,0,0}, // Second frame for alien #3
        {100,0,100,0,0,100,0,100},
        {100,100,100,100,100,100,100,100},
        {100,100,0,100,100,0,100,100},
        {100,100,100,100,100,100,100,100},
        {0,100,100,100,100,100,100,0},
        {0,0,100,0,0,100,0,0},
        {0,100,0,0,0,0,100,0},
        
        {0,0,100,0,0,100,0,0}, // First frame for alien #3
        {0,0,100,0,0,100,0,0},
        {0,100,100,100,100,100,100,0},
        {100,100,0,100,100,0,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,0,100,0,0,100,0,100},
        {0,0,100,0,0,100,0,0},
  
        {0,0,100,0,0,100,0,0}, // Second frame for alien #3
        {100,0,100,0,0,100,0,100},
        {100,100,100,100,100,100,100,100},
        {100,100,0,100,100,0,100,100},
        {100,100,100,100,100,100,100,100},
        {0,100,100,100,100,100,100,0},
        {0,0,100,0,0,100,0,0},
        {0,100,0,0,0,0,100,0},

        {0,0,100,0,0,100,0,0}, // First frame for alien #3
        {0,0,100,0,0,100,0,0},
        {0,100,100,100,100,100,100,0},
        {100,100,0,100,100,0,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,0,100,0,0,100,0,100},
        {0,0,100,0,0,100,0,0},
  
        {0,0,100,0,0,100,0,0}, // Second frame for alien #3
        {100,0,100,0,0,100,0,100},
        {100,100,100,100,100,100,100,100},
        {100,100,0,100,100,0,100,100},
        {100,100,100,100,100,100,100,100},
        {0,100,100,100,100,100,100,0},
        {0,0,100,0,0,100,0,0},
        {0,100,0,0,0,0,100,0},

        {0,0,100,0,0,100,0,0}, // First frame for alien #3
        {0,0,100,0,0,100,0,0},
        {0,100,100,100,100,100,100,0},
        {100,100,0,100,100,0,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,0,100,0,0,100,0,100},
        {0,0,100,0,0,100,0,0},
  
        {0,0,100,0,0,100,0,0}, // Second frame for alien #3
        {100,0,100,0,0,100,0,100},
        {100,100,100,100,100,100,100,100},
        {100,100,0,100,100,0,100,100},
        {100,100,100,100,100,100,100,100},
        {0,100,100,100,100,100,100,0},
        {0,0,100,0,0,100,0,0},
        {0,100,0,0,0,0,100,0}
        
    };


int BlueAlien[64][8] = {   // store in program memory to save RAM

  // Blue Animation bitmap.  Each number represents the value of Blue in RGB image in the BitMap.
  // if the Bit map has a 1 in the posion the color will light up.  if not, it will still not light up...
  // 8 lines of color data for 8 frames (there is no error checking for
  // length).

        
        {0,0,0,0,0,0,0,0}, // Frame 1
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
  
        {0,0,0,0,0,0,0,0}, // Frame 2 
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,100,0,0,100,0,0},   //Blue Eyes
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        
        {0,0,0,0,0,0,0,0}, // Frame 3 
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
  
        {0,0,0,0,0,0,0,0}, // Frame 4 
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,100,0,0,100,0,0},   //Blue Eyes
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},

        {0,0,0,0,0,0,0,0}, // Frame 5 
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
  
        {0,0,0,0,0,0,0,0}, // Frame 6 
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,100,0,0,100,0,0},   //Blue Eyes
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},

        {0,0,0,0,0,0,0,0}, // Frame 7 
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
  
        {0,0,0,0,0,0,0,0}, // Frame 8 
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,100,0,0,100,0,0},   //Blue Eyes
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0}  // no comma on last entry
        
    };


int Face[64] = {
  // Frame 0
  B00111100,
  B01000010,
  B10100101,
  B10000001,
  B10100101,
  B10011001,
  B01000010,
  B00111100,


  // Frame 1
  B00111100,
  B01000010,
  B10100101,
  B10000001,
  B10111101,
  B10011001,
  B01000010,
  B00111100,


  // Frame 2
  B00111100,
  B01000010,
  B10100101,
  B10000001,
  B10111101,
  B10000001,
  B01000010,
  B00111100,


  // Frame 3
  B00111100,
  B01000010,
  B10100101,
  B10000001,
  B10011001,
  B10100101,
  B01000010,
  B00111100,


  // Frame 4
  B00111100,
  B01000010,
  B10100101,
  B10000001,
  B10011001,
  B10111101,
  B01000010,
  B00111100,


  // Frame 5
  B00111100,
  B01000010,
  B10100101,
  B10000001,
  B10000001,
  B10111101,
  B01000010,
  B00111100,


  // Frame 6
  B00111100,
  B01000010,
  B10100101,
  B10000001,
  B10100101,
  B10111101,
  B01000010,
  B00111100,


  // Frame 7
  B00111100,
  B01000010,
  B10100101,
  B10000001,
  B10100101,
  B10011001,
  B01000010,
  B00111100

};


int RedFace[64][8] = {   // store in program memory to save RAM

  // Red Animation bitmap.  Each number represents the value of Red in RGB image in the BitMap.
  // if the Bit map has a 1 in the posion the color will light up.  if not, it will still not light up...
  // 8 lines of color data for 8 frames (there is no error checking for
  // length).

        {100,100,100,100,100,100,100,100}, // Frame 1
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},   
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
  
        {100,100,100,100,100,100,100,100}, // Frame 2
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},   
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        
        {100,100,100,100,100,100,100,100}, // Frame 3
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},   
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
  
        {100,100,100,100,100,100,100,100}, // Frame 4
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},   
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},

        {100,100,100,100,100,100,100,100}, // Frame 5
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},   
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
  
        {100,100,100,100,100,100,100,100}, // Frame 6
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},   
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},

        {100,100,100,100,100,100,100,100}, // Frame 7
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},   
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
  
        {100,100,100,100,100,100,100,100}, // Frame 8
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},   
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100}   // no comma on last entry
        
    };

int GreenFace[64][8] = {   // store in program memory to save RAM

  // Green Animation bitmap.  Each number represents the value of Green in RGB image in the BitMap.
  // if the Bit map has a 1 in the posion the color will light up.  if not, it will still not light up...
  // 8 lines of color data for 8 frames (there is no error checking for
  // length).

        
        {100,100,100,100,100,100,100,100}, // Frame 1
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},   
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
  
        {100,100,100,100,100,100,100,100}, // Frame 2
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},   
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        
        {100,100,100,100,100,100,100,100}, // Frame 3
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},   
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
  
        {100,100,100,100,100,100,100,100}, // Frame 4
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},   
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},

        {100,100,100,100,100,100,100,100}, // Frame 5
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},   
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
  
        {100,100,100,100,100,100,100,100}, // Frame 6
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},   
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},

        {100,100,100,100,100,100,100,100}, // Frame 7
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},   
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
  
        {100,100,100,100,100,100,100,100}, // Frame 8
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},   
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100}   // no comma on last entry
        
    };


int BlueFace[64][8] = {   // store in program memory to save RAM

  // Blue Animation bitmap.  Each number represents the value of Blue in RGB image in the BitMap.
  // if the Bit map has a 1 in the posion the color will light up.  if not, it will still not light up...
  // 8 lines of color data for 8 frames (there is no error checking for
  // length).

        
        {100,100,100,100,100,100,100,100}, // Frame 1
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},   
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
  
        {100,100,100,100,100,100,100,100}, // Frame 2
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},   
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        
        {100,100,100,100,100,100,100,100}, // Frame 3
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},   
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
  
        {100,100,100,100,100,100,100,100}, // Frame 4
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},   
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},

        {100,100,100,100,100,100,100,100}, // Frame 5
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},   
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
  
        {100,100,100,100,100,100,100,100}, // Frame 6
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},   
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},

        {100,100,100,100,100,100,100,100}, // Frame 7
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},   
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
  
        {100,100,100,100,100,100,100,100}, // Frame 8
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},   
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100},
        {100,100,100,100,100,100,100,100}   // no comma on last entry
        
    };



// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}




/**************************************************************************/
/*!
    @brief  Sets up the HW an the BLE module (this function is called
            automatically on startup)
*/
/**************************************************************************/
void setup(void)
{




// matrix setup
     matrix.begin();
     matrix.setTextWrap(false);
     matrix.setBrightness(40);
     matrix.setTextColor(colors[0]);


  
  //while (!Serial);  // required for Flora & Micro
  delay(500);

  Serial.begin(115200);
  Serial.println(F("Adafruit Bluefruit Command <-> Data Mode Example"));
  Serial.println(F("------------------------------------------------"));

  /* Initialise the module */
  Serial.print(F("Initialising the Bluefruit LE module: "));

  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  Serial.println( F("OK!") );

  if ( FACTORYRESET_ENABLE )
  {
    /* Perform a factory reset to make sure everything is in a known state */
    Serial.println(F("Performing a factory reset: "));
    if ( ! ble.factoryReset() ){
      error(F("Couldn't factory reset"));
    }
  }

  /* Disable command echo from Bluefruit */
  ble.echo(false);

  Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  ble.info();

  Serial.println(F("Please use Adafruit Bluefruit LE app to connect in UART mode"));
  Serial.println(F("Then Enter characters to send to Bluefruit"));
  Serial.println();

  ble.verbose(false);  // debug info is a little annoying after this point!

  /* Wait for connection */
  while (! ble.isConnected()) {
      delay(500);
  }

  Serial.println(F("******************************"));

  // LED Activity command is only supported from 0.6.6
  if ( ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION) )
  {
    // Change Mode LED Activity
    Serial.println(F("Change LED activity to " MODE_LED_BEHAVIOUR));
    ble.sendCommandCheckOK("AT+HWModeLED=" MODE_LED_BEHAVIOUR);
  }

  // Set module to DATA mode
  Serial.println( F("Switching to DATA mode!") );
  ble.setMode(BLUEFRUIT_MODE_DATA);

  Serial.println(F("******************************"));
}

/**************************************************************************/
/*!
    @brief  Constantly poll for new command or response data
*/
/**************************************************************************/
void loop(void)
{


  
  
  // Check for user input
  char n, inputs[BUFSIZE+1];

  if (Serial.available())
  {
    n = Serial.readBytes(inputs, BUFSIZE);
    inputs[n] = 0;
    // Send characters to Bluefruit
    Serial.print("Sending: ");
    Serial.println(inputs);

    // Send input data to host via Bluefruit
    ble.print(inputs);
  }

//***************** SCROLLING CLOCKWORK CODE SECTION **************************************
// code by Michael McAnally, July 2, 2016
//
//  VERY IMPORTANT NOTES: 
//
//
//          --------------                          -----------------
//         |  Phone App   |                        |  "pocketPIXEL"  |
//         |              |  20 bluetooth chars    |  PIXEL          |
//         |  rFtUART     | =====================> |  Wearables      |
//         |              |                        |  Device         |
//         |              |  Command Char (see Legend)               |
//         |              | =====================> |                 |
//         |              |                        |                 |
//          --------------                          -----------------
//          iOS or Android                          This Code
//
//
//  This is the beginning of the looping scroll code  (data driven logic)
//  It is like "clockwork" looping around and around checking for 20 chars coming
//  in over bluetooth channel (limitations of chip), then dispatching to specific
//  animations and single char controls inside the switch statement.
//
//  NOTE: "One rule" is you must recieve 20 chars of something
//  to scroll and reach the control switch statements logic.  It can be 20 chars of "spaces",
//  nothing basically, which displays nothing on the screen. Or text if you like.
//  This rule can be masked from the user later, knowing this in the next coding exercise the
//  rFtUART controller in Android & iOS.
//
//  EXAMPLE:
//
//  The numbers represent counting characters
//
//  After connection with rFtUART app sends 20 chars (ignoring the quotes) spaces padding inside quotes:
//
//             1         2
//   012345678901234567890
//
//  "Hello World          "
//
//  this results in a scrolling Hello World with 10 blank scrolling spaces afterward.
//  it is now ready to receive a command char...
//
//
//  LEGEND OF CONTROL SWITCH COMMAND CHARS
//                           -------------
//
//                                ~   = erase the scrolling text and returns to waiting for another 20 chars
//                                |   = adds another 19 chars to the current 20, giving 39 chars total, "|" is not displayed
//                                {   = adds another 19 chars after "|" command is issued giving a total of 58 chars to be display,
//                                      at this time I don't believe more than 58 continuous scolling chars should be necessary.
//                                      It's difficult enough to read 20 chars.
//
//                                _   = (underscore) shows the 20 scolling chars, which is the default the first time
//                                -   = (dash or minus) shows the 39 chars scrolling after executing the "|" command
//                                +   = shows the 58 chars scrolling after executing both "|" and "{" commands
//
//                                !   = beatiful rainbowCycle(10) animation, borrowed from Adafruit
//                                @   = theaterChaseRainbow(20) animation, borrowed from Adafruit
//                                #   = Red, White, Blue wipe animation
//                                $   = single Alien animation
//                                %   = Beating heart animation
//                                /   = Face outline animation
//                                ^   = Yellow face simple Emoji animations (experimental, needs to be redone)
//                                
//                                &   = Experimental, used at gay pride to display single animation after scrolling text
//                                *   = Experimental, used at gay pride to display two animations after scrolling text
//                                      this code is to be modified shorly, see lines 1821 - 1838  possible to allow 1,2
//                                      animation in-line with scrolling 
//
//
//
//*******************************************************************************

  // Echo received data
  if ( ble.available() && pos < 20)
  {
    
    int c = ble.read();
    Serial.print((char)c);

                 if (pos < 20)
                   {
                      BLE_arr[pos] = (char)c;  //load array with chars we get up to 20
                      
                      pos = pos + 1;
                   }
                 
    
  }
  else   //SCROLL OUR TEXT MSG BELOW . . . 
  {
  

                matrix.fillScreen(0);

                x = x - 1;

                

                if ( x > spaces )
                  {

                      Serial.print(x);
                      Serial.print(spaces);
                      
                      matrix.clear();
              
                      matrix.setCursor( x, 0 );
                                
                      matrix.print(BLE_arr);
                            
                      matrix.show(); 
                  }
                else 
                  {

                      // for Gay Pride cycle animation 1/3 of the time 

                      if (gaypride) {
                      
                         if(++swapanimation >= 3) {
                            swapanimation = 0;
                         }

                         if(swapanimation = 1) {
                            rainbowCycle(10);
                         }

                         if(swapanimation = 2) {
                            theaterChaseRainbow(20);
                         }
                      }
                      
                      x = 8;
                      
                  }
         
                
                delay(120);

                // Flashing color text effect
                if(++pass >= 3) {
                  pass = 0;
                }

                matrix.setTextColor(colors[pass]);


                if (pos >= 20)

                   {

                     int c = ble.read();
                     //Serial.print((char)c);
                                               
                           switch ((char)c) {

                            case '~':
                                Serial.println(F(" Case ~ detected."));

                                // wipe everything and reset
                                
                                pos = 0;
                     
                                for (int clr = 0 ; clr < 60 ; clr++ ) {
                                      BLE_arr[clr] = 0;  }  // clear out BLE display array buffer
                            break;

                            case '|':
                                Serial.println(F(" Case | detected."));

                                while (pos < 39)   {
                                      
                                      int c = ble.read();

                                      Serial.print((char)c);
                      
                                      BLE_arr[pos] = (char)c;  //load array with chars we get BEYOND 20
                      
                                      pos = pos + 1;

                                      x = 0;
                                }

                            break;


                            case '-':
                            
                                Serial.println(F(" Case - detected."));
                              
                                    spaces = -240;
                                    x = 8;
                                    
                                
                            break;

                            case '_':
                            
                                Serial.println(F(" Case _ detected."));

                                                    
                                   spaces = -120;
                                   x = 8;


                            break;

                            case '+':
                            
                                Serial.println(F(" Case + detected."));

                                                    
                                    spaces = -360;
                                    x = 8;


                            break;

                            case '{':
                                Serial.println(F(" Case { detected."));

                                while (pos < 58)   {
                                      
                                      int c = ble.read();

                                      Serial.print((char)c);
                      
                                      BLE_arr[pos] = (char)c;  //load array with chars we get BEYOND 20
                      
                                      pos = pos + 1;

                                      x = 0;
                                }

                            break;

                            
                            case '!':
                                Serial.println(F(" Case ! detected."));
                                rainbowCycle(10);
                            break;


                            case '@':
                                Serial.println(F(" Case @ detected."));
                                theaterChaseRainbow(20);
                            break;

                            case '#':
                                Serial.println(F(" Case # detected."));
                                colorWipe(pixel.Color(0, 0, 0), 20);
                                colorWipe(pixel.Color(255, 0, 0), 20);
                                colorWipe(pixel.Color(255, 255, 255), 20);
                                colorWipe(pixel.Color(0, 0, 255), 20);
                                colorWipe(pixel.Color(0, 0, 0), 20);
                                pixel.show(); // This sends the updated pixel color to the hardware.                            
                                
                             break;

                             case '$':

                        
                                Serial.println(F(" Case $ detected."));
         
                                for (int loop = 0 ; loop < 5 ; loop++ ) {
                                   AnimateFrames(Alien, RedAlien, GreenAlien, BlueAlien);    }
                               
                                
                             break;


                             case '%':

                        
                                Serial.println(F(" Case % detected."));

                                for (int loop = 0 ; loop < 10 ; loop++ ) {
                                   AnimateFrames(BeatingHeart, RedBeatingHeart, GreenBeatingHeart, BlueBeatingHeart);       }

                                                               
                                
                             break;


                             case '/':

                        
                                Serial.println(F(" Case / detected."));

                                for (int loop = 0 ; loop < 10 ; loop++ ) {
                                   AnimateFrames(Face, RedFace, GreenFace, BlueFace);       }

                                                               
                                
                             break;


                             case '^':

                        
                                Serial.println(F(" Case ^ detected."));

                                for (int loop = 0 ; loop < 10 ; loop++ ) {
                                   AnimateFrames(Emoji, RedEmoji, GreenEmoji, BlueEmoji);       }

                                                               
                                
                             break;


                             case '&':

                        
                                Serial.println(F(" Case & detected."));

                                gaypride = 0;

                                                               
                                
                             break;

                             case '*':

                        
                                Serial.println(F(" Case * detected."));

                                gaypride = 1;

                                                               
                                
                             break;

                           }

                       
                   }
         
  }
}





///////////// END OF SCROLLING CLOCKWORK CODE (SEE NOTES ABOVE) //////////////////////////////////////////////


//******************Animation Code Notes to be added soon****************************************************

void AnimateFrames( int Bits[64], int Red[][8], int  Green[][8], int Blue[][8])
    {
      int offset = 0;
         
         for (int idx=0; idx<8; ++idx) {
            
            offset = idx * 8;
            
        for (byte x=0; x<8; ++x) {

            byte data = pgm_read_byte (&Bits[x + offset]);   // fetch data from program memory

            for (byte y=0; y<8; ++y) {
    
                if (data & (1<<y)) {
 
                    
                    matrix.drawPixel(y, x, matrix.Color(Red[x + offset][y], Green[x + offset][y], Blue[x + offset][y])); // x, y, color
                } else {

                    
                    matrix.drawPixel(y, x, 0); // x, y, color
                }
            }
        }
       
        matrix.show();
       
        delay(400);
      }
      
   }



//*******************code below this point are aminations borrowed from Adafruit***************************************************
 
     

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<pixel.numPixels(); i++) {
      pixel.setPixelColor(i, c);
      pixel.show();
      delay(wait);
  }
}

void flowingStripes(uint32_t c, uint8_t wait) {
for(uint16_t i=0; i<11; i++) {
      pixel.setPixelColor(R1[i], c);
      pixel.setPixelColor(R2[i], c);
      pixel.setPixelColor(R3[i], c);
      pixel.setPixelColor(R4[i], c);
      pixel.setPixelColor(L1[i], c);
      pixel.setPixelColor(L2[i], c);
      pixel.setPixelColor(L3[i], c);
      pixel.setPixelColor(L4[i], c);
      pixel.show();
      delay(30);
  }
}


void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<pixel.numPixels(); i++) {
      pixel.setPixelColor(i, Wheel((i+j) & 255));
    }
    pixel.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< pixel.numPixels(); i++) {
      pixel.setPixelColor(i, Wheel(((i * 256 / pixel.numPixels()) + j) & 255));
    }
    pixel.show();
    delay(wait);
  }
}


//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (int i=0; i < pixel.numPixels(); i=i+3) {
        pixel.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on

                
      }
      pixel.show();

      delay(wait);

      for (int i=0; i < pixel.numPixels(); i=i+3) {
        pixel.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}




// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return pixel.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return pixel.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return pixel.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

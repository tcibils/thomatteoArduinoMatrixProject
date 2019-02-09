/* 
 *  
 *  Creator : Thomas Cibils
 *  Last update : 05.02.2019
 *  FastLED tuto : https://github.com/FastLED/FastLED/wiki/Basic-usage - used for WS2812B 5050 RGB LED Strip 5M 150 300 Leds 144 60LED/M Individual Addressable 5V
 *  */

#include <TimerOne.h>
#include "FastLED.h"
#include <avr/pgmspace.h>

// LED MATRIX CODE
#define displayNumberOfRows 10                          // Number of rows
#define displayNumberOfColumns 6                       // Number of coumns
#define NUM_LEDS displayNumberOfRows * displayNumberOfColumns // Number of LEDs

CRGB leds[NUM_LEDS];                                          // Defining leds table for FastLed
#define DATA_PIN 6                                            // Output pin for FastLed

// LED Matrix
// top column is from 0 to 7, bottom one from 56 to 63 (for a 8x8 matrix)
byte LEDMatrix[displayNumberOfRows][displayNumberOfColumns];


// Original colours for leds.
const byte Black = 0;
const byte White = 1;
const byte Blue = 2;
const byte Red = 3;
const byte Green = 4;
const byte Purple = 5;


// Pin used from the arduino
#define leftButton A5        // Input pin for button Left
#define upButton A4          // Input pin for button Up
#define rightButton A3       // Input pin for button Right
#define downButton A2        // Input pin for button Down
#define aButton A0           // Input pin for button A
#define bButton A1           // Input pin for button B


struct pointOnMatrix {
  byte lineCoordinate;
  byte columnCoordinate;
};

unsigned long lastMillis = 0;
unsigned const int screenMoves = 1000;  // In miliseconds, every how much will the menace grow

unsigned int leftButtonValue = LOW;
unsigned int rightButtonValue = LOW;
unsigned int upButtonValue = LOW;
unsigned int downButtonValue = LOW;

unsigned int lastLeftButtonValue = LOW;
unsigned int lastRightButtonValue = LOW;
unsigned int lastUpButtonValue = LOW;
unsigned int lastDownButtonValue = LOW;



unsigned byte leftButtonPushed = 0;
unsigned byte rightButtonPushed = 0;



void setup() {

  Serial.begin(9600);

  // Set matrix pins to output
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

  // Set button pins to input
  pinMode(leftButton, INPUT);
  pinMode(upButton, INPUT);
  pinMode(downButton, INPUT);
  pinMode(rightButton, INPUT);
  pinMode(aButton, INPUT);
  pinMode(bButton, INPUT);
}

void loop() {

if(millis() - lastMillis > screenMoves) {

  if(leftButtonPushed == 1) {
    
  }

  if(rightButtonPushed == 1) {
    
  }

  lastMillis = millis();
  leftButtonPushed = 0;
  rightButtonPushed = 0;
  outputDisplay();
}

    // ----------------------------------------------------------
    // Checking if a button has been pushed, reacting accordingly
    // ----------------------------------------------------------
    
    leftButtonValue = analogRead(leftButton);
    if (leftButtonValue < 200 && lastLeftButtonValue > 800) {
      leftButtonPushed = 1;
      rightButtonPushed = 0;
    }
    lastLeftButtonValue = leftButtonValue; // And we update what we read just after

    rightButtonValue = analogRead(rightButton);
    if (rightButtonValue < 200 && lastRightButtonValue > 800) { 
      leftButtonPushed = 0;
      rightButtonPushed = 1;
    }
    lastRightButtonValue = rightButtonValue; // And we update what we read just after

    /*
    upButtonValue = analogRead(upButton);
    if (upButtonValue < 200 && lastUpButtonValue > 800) { 
      
    }
    lastUpButtonValue = upButtonValue; // And we update what we read just after
    */
    
    /*
    downButtonValue = analogRead(downButton);
    if (downButtonValue < 200 && lastDownButtonValue > 800) { 
      
    }
    lastDownButtonValue = downButtonValue; // And we update what we read just after
    */
    
  delay(1);
}

// Makes the whole "LEDMatrix" equals to 0, i.e. each LED is off
void clearLEDMatrix() {
  // Just seting le LEDmatrix to Wall
  for (byte i = 0; i < displayNumberOfRows; i++)  {
    for (byte j = 0; j < displayNumberOfColumns; j++) {
      LEDMatrix[i][j] = Black;
    }
  }
}


// We update the physical display of the LED matrix, based on the LEDMatrix
void outputDisplay() {
  for(byte rowIndex = 0; rowIndex < displayNumberOfRows; rowIndex++) {
    for(byte columnIndex = 0; columnIndex < displayNumberOfColumns; columnIndex++) {
      // Useful because of the way my matrix is soldered.
      // So we'll invert one column every two compared to our digital matrix
      // If we're on an even column, we're fine, everything is straightfoward
      if(columnIndex%2 == 0) {
        
        if(LEDMatrix[rowIndex][columnIndex] == Black) {leds[(columnIndex + 1)*displayNumberOfRows - rowIndex - 1] = CRGB::Black;}
        if(LEDMatrix[rowIndex][columnIndex] == White) {leds[(columnIndex + 1)*displayNumberOfRows - rowIndex - 1] = CRGB::White;}
        if(LEDMatrix[rowIndex][columnIndex] == Green) {leds[(columnIndex + 1)*displayNumberOfRows - rowIndex - 1] = CRGB::Green;}
        if(LEDMatrix[rowIndex][columnIndex] == Blue) {leds[(columnIndex + 1)*displayNumberOfRows - rowIndex - 1] = CRGB::Blue;}
        if(LEDMatrix[rowIndex][columnIndex] == Red) {leds[(columnIndex + 1)*displayNumberOfRows - rowIndex - 1] = CRGB::Red;}
        if(LEDMatrix[rowIndex][columnIndex] == Purple) {leds[(columnIndex + 1)*displayNumberOfRows - rowIndex - 1] = CRGB::Purple;}
      }
      // If we're on an uneven column, we do a mathematical trick to invert it
      else if(columnIndex%2 == 1) {
        if(LEDMatrix[rowIndex][columnIndex] == Black) {leds[columnIndex*displayNumberOfRows + rowIndex] = CRGB::Black;}
        if(LEDMatrix[rowIndex][columnIndex] == White) {leds[columnIndex*displayNumberOfRows + rowIndex] = CRGB::White;}
        if(LEDMatrix[rowIndex][columnIndex] == Green) {leds[columnIndex*displayNumberOfRows + rowIndex] = CRGB::Green;}
        if(LEDMatrix[rowIndex][columnIndex] == Blue) {leds[columnIndex*displayNumberOfRows + rowIndex] = CRGB::Blue;}
        if(LEDMatrix[rowIndex][columnIndex] == Red) {leds[columnIndex*displayNumberOfRows + rowIndex] = CRGB::Red;}
        if(LEDMatrix[rowIndex][columnIndex] == Purple) {leds[columnIndex*displayNumberOfRows + rowIndex] = CRGB::Purple;}
      }
    }
  }
  
  // Display the matrix physically
  FastLED.show(); 
}

// We update the digital display of the LED matrix
void digitalOutputDisplay() {
  Serial.print("\n We print digitally the current theoritical state of the LED Matrix : \n");
  for (byte i = 0; i < displayNumberOfRows; i++) {
    for (byte j = 0; j < displayNumberOfColumns; j++) {
      Serial.print(LEDMatrix[i][j]);
      if (j < displayNumberOfColumns - 1) {
        Serial.print(", ");
      }
      else {
        Serial.print("\n");
      }
    }
  }
}

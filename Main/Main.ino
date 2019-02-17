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
// Position 0, 0 is on top left
byte LEDMatrix[displayNumberOfRows][displayNumberOfColumns] = {
  {0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0}
};


// Original colours for leds.
const byte Black = 0;
const byte Wall = 1;
const byte Blue = 2;
const byte Red = 3;
const byte Car = 3;
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

unsigned int leftButtonValue = LOW;
unsigned int rightButtonValue = LOW;
unsigned int upButtonValue = LOW;
unsigned int downButtonValue = LOW;

unsigned int lastLeftButtonValue = LOW;
unsigned int lastRightButtonValue = LOW;
unsigned int lastUpButtonValue = LOW;
unsigned int lastDownButtonValue = LOW;

byte leftButtonPushed = 0;
byte rightButtonPushed = 0;


byte numberTable[10][5][3] {
{
  {1,1,1},
  {1,0,1},
  {1,0,1},
  {1,0,1},
  {1,1,1}
},
{
  {0,1,0},
  {0,1,0},
  {0,1,0},
  {0,1,0},
  {0,1,0}
},
{
  {1,1,1},
  {0,0,1},
  {1,1,1},
  {1,0,0},
  {1,1,1}
},
{
  {1,1,1},
  {0,0,1},
  {1,1,1},
  {0,0,1},
  {1,1,1}
},
{
  {1,0,1},
  {1,0,1},
  {1,1,1},
  {0,0,1},
  {0,0,1}
},
{
  {1,1,1},
  {1,0,0},
  {1,1,1},
  {0,0,1},
  {1,1,1}
},
{
  {1,1,1},
  {1,0,0},
  {1,1,1},
  {1,0,1},
  {1,1,1}
},
{
  {1,1,1},
  {0,0,1},
  {0,1,1},
  {0,0,1},
  {0,0,1}
},
{
  {1,1,1},
  {1,0,1},
  {1,1,1},
  {1,0,1},
  {1,1,1}
},
{
  {1,1,1},
  {1,0,1},
  {1,1,1},
  {0,0,1},
  {1,1,1}
}
};


// Will count the number of lines the player passed.
unsigned int playerScore = 0;

// ---------------------------------------------
// ---------------- GAME VARIABLES -------------
// ---------------------------------------------
byte carPosition = 3;                       // Car position on the bootom line of the LED matrix (so the column)
byte ticker = 5;                            // Once the ticker is 0, we generate a new line randomly on top of the matrix. It dicreases every "turn".
byte probaApparitionLigne = 90;             // Probability of a new line appearing when possible
byte probaApparitionBlock = 80;             // For a new line, for each block, the probability of it being a "wall"
unsigned const int startScreenMoves = 300;  // Speed of the car at start of the game
unsigned int screenMoves = 500;             // In miliseconds, how fast will the car go. That will be updated during the game.
unsigned const int screenMovesMini = 100;   // Maximum speed of the game
byte speedIncreaseMode = 0;                 // If it's 1, the player jumps levels and the car speeds at defined point in time. If it's 0, the car speed augments lineraly.

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

  // Initializing randomness
  randomSeed(analogRead(0));
}



/* 
Star
Wall colours
End-game / restart (=> code cleaning)
Speed up quicker ?
*/




void loop() {

if(screenMoves > screenMovesMini) {
  if(speedIncreaseMode == 0) {
    screenMoves = startScreenMoves - playerScore;
  }

  if(speedIncreaseMode == 1) {
    screenMoves = startScreenMoves - (50*(playerScore/50));
  }
  Serial.print("Speed :");
  Serial.print(screenMoves);
  Serial.print("\n");
}

if(millis() - lastMillis > screenMoves) {

  if(leftButtonPushed == 1) {
    if(carPosition > 0) {
      carPosition--;
    }
  }

  if(rightButtonPushed == 1) {
    if(carPosition <5) {
      carPosition++;
    }
  }

  
  byte newLine[6] = {0,0,0,0,0,0};
  // If the ticker is 0, meaning we can create a new line
  if(ticker == 0) {
    byte blockedSpacesCounter = 0;
    // We have a proba to pop a new line or not
    if(random(100) < probaApparitionLigne) {
      byte freePassage = random(6);
      // And we try to generate blocks
      for(byte counter = 0; counter < 6; counter++) {
        // Each having a random proba of appearing
          if(random(100) < probaApparitionBlock && freePassage != counter) {
            newLine[counter] = Wall;
            blockedSpacesCounter++;
        }
      }
    }
    ticker = blockedSpacesCounter + 1;
  }

  
  

  if(ticker > 0) {
    ticker--;
  }
  
  lastMillis = millis();
  leftButtonPushed = 0;
  rightButtonPushed = 0;
  updateLEDmatrix(newLine);
  playerScore++;
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

void updateLEDmatrix(byte newLine[6]) {
  // Getting each line down by one bit
  for(byte rowIterator = 0; rowIterator < displayNumberOfRows - 2; rowIterator++) {
    for(byte columnIterator = 0; columnIterator < displayNumberOfColumns; columnIterator++) {
      LEDMatrix[9-rowIterator][columnIterator] = LEDMatrix[9-rowIterator-1][columnIterator];
    }
  }

  // We check if the car hit something
  if(LEDMatrix[9][carPosition] != Black) {
    gameOver();
  }
  else {
    LEDMatrix[9][carPosition] = Car;
  }

  // Create a new line in top of the existing matrix, with the passed argument.
  for(byte columnIterator = 0; columnIterator < displayNumberOfColumns; columnIterator++) {
    LEDMatrix[1][columnIterator] = newLine[columnIterator];
  }

  // On top of the top, we create a line for the score
  byte LEDSon = playerScore/100;
  byte lastLed = playerScore%100;
  
  for(byte columnIterator = 0; columnIterator < displayNumberOfColumns; columnIterator++) {
    // We plot 1 blue dot for each 100 points the player has
    if(columnIterator < LEDSon) {
      LEDMatrix[0][columnIterator] = Red;
    }
    // And we get the last dot to get more and more blue, as the score goes up
    if(columnIterator == LEDSon) {
      LEDMatrix[0][columnIterator] = lastLed;
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
        
        if(LEDMatrix[rowIndex][columnIndex] == Black)  {leds[(columnIndex + 1)*displayNumberOfRows - rowIndex - 1] = CRGB::Black;}
        if(LEDMatrix[rowIndex][columnIndex] == Wall)   {leds[(columnIndex + 1)*displayNumberOfRows - rowIndex - 1] = CRGB::White;}
        if(LEDMatrix[rowIndex][columnIndex] == Green)  {leds[(columnIndex + 1)*displayNumberOfRows - rowIndex - 1] = CRGB::Green;}
        if(LEDMatrix[rowIndex][columnIndex] == Blue)   {leds[(columnIndex + 1)*displayNumberOfRows - rowIndex - 1] = CRGB::Blue;}
        if(LEDMatrix[rowIndex][columnIndex] == Car)    {leds[(columnIndex + 1)*displayNumberOfRows - rowIndex - 1] = CRGB::Red;}
        if(LEDMatrix[rowIndex][columnIndex] == Red)    {leds[(columnIndex + 1)*displayNumberOfRows - rowIndex - 1] = CRGB::Red;}
        if(LEDMatrix[rowIndex][columnIndex] == Purple) {leds[(columnIndex + 1)*displayNumberOfRows - rowIndex - 1] = CRGB::Purple;}
        // If we're above it, then we make the diod goes up and up in blue for the score. The rainbow before it is wanted as a "checkpoint"
        if(LEDMatrix[rowIndex][columnIndex] > 5)     {leds[(columnIndex + 1)*displayNumberOfRows - rowIndex - 1].setRGB(2*LEDMatrix[rowIndex][columnIndex]+30,0,0);}
      }
      // If we're on an uneven column, we do a mathematical trick to invert it
      else if(columnIndex%2 == 1) {
        if(LEDMatrix[rowIndex][columnIndex] == Black) {leds[columnIndex*displayNumberOfRows + rowIndex] = CRGB::Black;}
        if(LEDMatrix[rowIndex][columnIndex] == Wall) {leds[columnIndex*displayNumberOfRows + rowIndex] = CRGB::White;}
        if(LEDMatrix[rowIndex][columnIndex] == Green) {leds[columnIndex*displayNumberOfRows + rowIndex] = CRGB::Green;}
        if(LEDMatrix[rowIndex][columnIndex] == Blue) {leds[columnIndex*displayNumberOfRows + rowIndex] = CRGB::Blue;}
        if(LEDMatrix[rowIndex][columnIndex] == Car) {leds[columnIndex*displayNumberOfRows + rowIndex] = CRGB::Red;}
        if(LEDMatrix[rowIndex][columnIndex] == Red) {leds[columnIndex*displayNumberOfRows + rowIndex] = CRGB::Red;}
        if(LEDMatrix[rowIndex][columnIndex] == Purple) {leds[columnIndex*displayNumberOfRows + rowIndex] = CRGB::Purple;}
        // If we're above it, then we make the diod goes up and up in blue for the score. The rainbow before it is wanted as a "checkpoint"
        if(LEDMatrix[rowIndex][columnIndex] > 5)     {leds[columnIndex*displayNumberOfRows + rowIndex].setRGB(2*LEDMatrix[rowIndex][columnIndex]+30,0,0);}
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

void displayScore() {
  byte thousands = playerScore/1000;
  byte hundreds = (playerScore - (thousands*1000))/100;
  byte tens = (playerScore - (thousands*1000) - (hundreds*100))/10;
  byte units = (playerScore - thousands*1000 - hundreds*100 - tens*10);
  
  clearLEDMatrix();

  for(byte rowIterator = 0; rowIterator < 5; rowIterator++) {
    for(byte columnIterator = 0; columnIterator < 3; columnIterator++) {
         LEDMatrix[rowIterator][columnIterator] = numberTable[thousands][rowIterator][columnIterator];  
    }
  }
  for(byte rowIterator = 0; rowIterator < 5; rowIterator++) {
    for(byte columnIterator = 3; columnIterator < 6; columnIterator++) {
         LEDMatrix[rowIterator][columnIterator] = Red * numberTable[hundreds][rowIterator][columnIterator-3];  
    }
  }
  for(byte rowIterator = 5; rowIterator < 10; rowIterator++) {
    for(byte columnIterator = 0; columnIterator < 3; columnIterator++) {
         LEDMatrix[rowIterator][columnIterator] = Red * numberTable[tens][rowIterator-5][columnIterator];  
    }
  }
  for(byte rowIterator = 5; rowIterator < 10; rowIterator++) {
    for(byte columnIterator = 3; columnIterator < 6; columnIterator++) {
         LEDMatrix[rowIterator][columnIterator] = numberTable[units][rowIterator-5][columnIterator-3];  
    }
  }

  outputDisplay();
}

void gameOver() {
  Serial.print("game over");
  delay(2000);
  displayScore();
  delay(10000); 
}

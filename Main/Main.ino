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

/* Constantes des bits de chaque bouton */
#define BTN_A 256
#define BTN_B 1
#define BTN_X 512
#define BTN_Y 2
#define BTN_SELECT 4
#define BTN_START 8
#define BTN_UP 16
#define BTN_DOWN 32
#define BTN_LEFT 64
#define BTN_RIGHT 128
#define BTN_L 1024
#define BTN_R 2048
#define NO_GAMEPAD 61440

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
const byte White = 1;
const byte Blue = 2;
const byte Red = 3;
const byte Green = 4;
const byte Purple = 5;
const byte Car = 6;


// Pin used from the arduino
#define leftButton A5        // Input pin for button Left
#define upButton A4          // Input pin for button Up
#define rightButton A3       // Input pin for button Right
#define downButton A2        // Input pin for button Down
#define aButton A0           // Input pin for button A
#define bButton A1           // Input pin for button B

static const byte PIN_LATCH = 2;
static const byte PIN_CLOCK = 3;
static const byte PIN_DATA = 4;

struct pointOnMatrix {
  byte lineCoordinate;
  byte columnCoordinate;
};


unsigned long lastMillis = 0;

unsigned int leftButtonValue = LOW;
unsigned int rightButtonValue = LOW;
unsigned int upButtonValue = LOW;
unsigned int downButtonValue = LOW;
unsigned int aButtonValue = LOW;
unsigned int bButtonValue = LOW;

unsigned int lastLeftButtonValue = LOW;
unsigned int lastRightButtonValue = LOW;
unsigned int lastUpButtonValue = LOW;
unsigned int lastDownButtonValue = LOW;
unsigned int lastAButtonValue = LOW;
unsigned int lastBButtonValue = LOW;

byte leftButtonPushed = 0;
byte rightButtonPushed = 0;
byte aButtonPushed = 0;
byte bButtonPushed = 0;

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


const byte gamePadMode = 0;                 // 0 means using my home-made gamepad, 1 means using an SNES gamepad
byte gameStatus = 2;                        // 0 means we're playing, 1 means it's game over, 2 means the player got a star

// ---------------------------------------------
// -------- NON-MODIFIED GAME VARIABLES --------
// ---------------------------------------------
const byte initialCarPosition = 3;                // Car position on the bootom line of the LED matrix (so the column)
const byte initialTicker = 5;                     // Once the ticker is 0, we generate a new line randomly on top of the matrix. It dicreases every "turn".
unsigned const int initialPlayerScore = 0;        // Will count the number of lines the player passed.
unsigned const int initialPlayerAdvancement = 0;  // See player Advancement
const byte initialStarDuration = 50;

const byte gameParametersTablesSize = 11;         // Size of tables below
unsigned const int intervalesT[gameParametersTablesSize] =   {0,   50,  100, 150, 200, 250, 300, 350, 400, 450, 500};
unsigned const int frameRateT[gameParametersTablesSize] =    {250, 200, 150, 100, 100, 100, 100, 100, 100, 100, 100};
const byte probaApparitionLigneT[gameParametersTablesSize] = {80,  70,  60,  50,  50,  60,  70,  80,  90,  90,  90};
const byte probaApparitionBlockT[gameParametersTablesSize] = {70,  60,  50,  40,  40,  40,  50,  60,  70,  80,  90};

// We will check the player advancement, and locate ourselves in the interval table defined
// Then, we will adjust the frame rate (speed of the game), proba of line apparition, and proba of block apparition
// See function setGameParameters();


// ---------------------------------------------
// ----------- MODIFIED GAME VARIABLES ---------
// ---------------------------------------------
byte carPosition = initialCarPosition;                      // Car position on the bootom line of the LED matrix (so the column)
byte ticker = initialTicker;                                // Once the ticker is 0, we generate a new line randomly on top of the matrix. It dicreases every "turn".
byte starDuration = initialStarDuration;                    // Duration left for the star.
unsigned int playerScore = initialPlayerScore;              // Will count the number of lines the player passed (points), and display it at end of the game
unsigned int playerAdvancement = initialPlayerAdvancement;  // Will be like the player score but not exactly. Will not be increased when under a star effect. We'll use this to augment game difficulty.

unsigned int screenMoves = frameRateT[0];                   // In miliseconds, how fast will the car go. That will be updated during the game.
byte probaApparitionLigne = probaApparitionLigneT[0];       // Probability of a new line appearing when possible
byte probaApparitionBlock = probaApparitionBlockT[0];       // For a new line, for each block, the probability of it being a "wall"


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
  
  /* Initialisation des broches */
  pinMode(PIN_LATCH, OUTPUT);
  pinMode(PIN_CLOCK, OUTPUT);
  pinMode(PIN_DATA, INPUT); 

  // Initializing randomness
  randomSeed(analogRead(0));
}



/* 
Star
Adapting limits score
Make the matrix size abstract
Game is impossible in certain cases : central wall, player choses the wrong path, and takes an impossible wall after it
Music ?
*/


void loop() {

  if(gameStatus == 0) {
    
    setGameParameters();
    
    // Every x miliseconds, we make an iteration.
    if(millis() - lastMillis > screenMoves) {
    
      changeCarPosition();
    
      // We create the new line we'll put on top of our game
      byte newLine[6] = {0,0,0,0,0,0};
      
      // If the ticker is 0, meaning we can create a new line with blocks
      if(ticker == 0) {
        // We'll check how many blocks we create
        byte blockedSpacesCounter = 0;
        // We have a proba to pop a new line or not
        if(random(100) < probaApparitionLigne) {
          byte freePassage = random(6); // This will enforce that there's always be at least one free passge in the line.
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

      pushLinesDown();
      checkCarCrash();

      if(gameStatus == 0) {
        showCar();
        displayScoreInGame();
        addLineTopMatrix(newLine);
      
        playerScore++;
        playerAdvancement++;
      }
    }
  }

  // The player got a star !
  if(gameStatus == 2) {
    if(starDuration > 0) {
      screenMoves = 75;
      probaApparitionLigne = 95;
      probaApparitionBlock = 95;
  
      // Every x miliseconds, we make an iteration.
      if(millis() - lastMillis > screenMoves) {
      
        changeCarPosition();
      
        // We create the new line we'll put on top of our game
        byte newLine[6] = {0,0,0,0,0,0};
        
        // If the ticker is 0, meaning we can create a new line with blocks
        if(ticker == 0) {
          // We'll check how many blocks we create
          byte blockedSpacesCounter = 0;
          // We have a proba to pop a new line or not
          if(random(100) < probaApparitionLigne) {
            byte freePassage = random(6); // This will enforce that there's always be at least one free passge in the line.
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
        
        pushLinesDown();
        displayScoreInGame();
        showCar();
        addLineTopMatrix(newLine);
        playerScore++;
        starDuration--;
      }
    }
    if(starDuration == 0) {
      gameStatus = 0;
      starDuration = initialStarDuration;
    }
  }

  // If it's game over
  if(gameStatus == 1) {
    // We update the LED matrix with the player score
    displayScore();

    // If the player has pushed A or B
    if(aButtonPushed == 1 || bButtonPushed == 1) {
      aButtonPushed = 0;
      bButtonPushed = 0;
      // We re-initialize the game.
      reinitializeGame();
      gameStatus = 0;
    }
  }
    
  outputDisplay();
  checkButtons();      
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

void changeCarPosition() {
    // If the button "left" has been pushed, then the car goes left one column
  if(leftButtonPushed == 1) {
    if(carPosition > 0) {
      carPosition--;
    }
  }

  // If the button "right" has been pushed, then the car goes right one column
  if(rightButtonPushed == 1) {
    if(carPosition <5) {
      carPosition++;
    }
  }
}

void setGameParameters() {
  // We first check in which interval we are
  for(byte i = 0; i < gameParametersTablesSize-1; i++) {
     // If the player score is above the higher than the highest interval in the table, we won't enter the if, thus the values won't change and will be at max.
    if(playerAdvancement >= intervalesT[i] && playerAdvancement < intervalesT[i+1]) {
      
      // Given the interval where we are, we set the game variable linearly between the values set.
      // The breakdown below is necessary to make it work, due to how rounding is done in Arduino (I'm not fully sure of that)

      // Setting frame rate, so speed of the game
      const int frameDivisionElementOne = (frameRateT[i+1] - frameRateT[i]);
      const int frameDivisionElementTwo = (intervalesT[i+1] - intervalesT[i]);
      const float frameGradient = (float)(frameDivisionElementOne/frameDivisionElementTwo);
      screenMoves = frameRateT[i] + (frameGradient * (playerAdvancement - intervalesT[i]));

      // Setting the proba that a new line appears
      const int ligneDivisionElementOne = (probaApparitionLigneT[i+1] - probaApparitionLigneT[i]);
      const int ligneDivisionElementTwo = (intervalesT[i+1] - intervalesT[i]);
      const float ligneGradient = (ligneDivisionElementOne / ligneDivisionElementTwo);
      probaApparitionLigne = probaApparitionLigneT[i] + (ligneGradient * (playerAdvancement - intervalesT[i]));

      // Setting the proba for the number of blocks in a new line having appeared.
      const int blockDivisionElementOne = (probaApparitionBlockT[i+1] - probaApparitionBlockT[i]);
      const int blockDivisionElementTwo = (intervalesT[i+1] - intervalesT[i]);
      const float blockGradient = (blockDivisionElementOne / blockDivisionElementTwo);
      probaApparitionBlock = probaApparitionBlockT[i] + (blockGradient * (playerAdvancement - intervalesT[i]));
     }

  }
}

void checkButtons() {
  // ----------------------------------------------------------
  // Checking if a button has been pushed, reacting accordingly
  // ----------------------------------------------------------

  if(gamePadMode == 1) {
    static uint16_t oldBtns = 0;      // Anciennes valeurs des boutons
    uint16_t btns = getSnesButtons(); // Valeurs actuelles des boutons
  
    if(btns & NO_GAMEPAD) {
      Serial.println(F("No gamepad connected"));
      return;
    }
     
    /* Affiche l'état de chaque bouton */
    if(btns & BTN_A) {
      if(gameStatus == 1) {
        aButtonPushed = 1;
      }
    }
        
    if(btns & BTN_B){
      if(gameStatus == 1) {
        bButtonPushed = 1;
      }
    }
  
    /*
    if(btns & BTN_X)
      Serial.print(F("X "));
    else
      Serial.print(F("- "));
   
    if(btns & BTN_Y)
      Serial.print(F("Y "));
    else
      Serial.print(F("- "));
   
    if(btns & BTN_SELECT)
      Serial.print(F("SELECT "));
    else
      Serial.print(F("------ "));
   
    if(btns & BTN_START)
      Serial.print(F("START "));
    else
      Serial.print(F("----- "));
   
    if(btns & BTN_UP)
      Serial.print(F("UP "));
    else
      Serial.print(F("-- "));
   
    if(btns & BTN_DOWN)
      Serial.print(F("DOWN "));
    else
      Serial.print(F("---- "));
   */
    if(btns & BTN_LEFT) {
        leftButtonPushed = 1;
        rightButtonPushed = 0;
    }
   
    if(btns & BTN_RIGHT){
        leftButtonPushed = 0;
        rightButtonPushed = 1;
    }
    /* 
    if(btns & BTN_L)
      Serial.print(F("L "));
    else
      Serial.print(F("- "));
   
    if(btns & BTN_R)
      Serial.println(F("R"));
    else
      Serial.println(F("-"));
  */
  }

  if(gamePadMode == 0) {
    
    // ----------------------------------------------------------
    // Checking if a button has been pushed, reacting accordingly
    // ----------------------------------------------------------
  
    // Left and right are only used while playing
    leftButtonValue = analogRead(leftButton);
    if (leftButtonValue < 200 && lastLeftButtonValue > 800) {
      if(gameStatus == 0) {
        leftButtonPushed = 1;
        rightButtonPushed = 0;
      }
    }
    lastLeftButtonValue = leftButtonValue; // And we update what we read just after
  
    rightButtonValue = analogRead(rightButton);
    if (rightButtonValue < 200 && lastRightButtonValue > 800) { 
      if(gameStatus == 0) {
        leftButtonPushed = 0;
        rightButtonPushed = 1;
      }
    }
    lastRightButtonValue = rightButtonValue; // And we update what we read just after
  
    // A and B buttons are only used if we're in game over, to restart the game.
    aButtonValue = analogRead(aButton);
    if (aButtonValue < 200 && lastAButtonValue > 800) { 
      if(gameStatus == 1) {
        aButtonPushed = 1;
      }
    }
    lastAButtonValue = aButtonValue; // And we update what we read just after
  
    bButtonValue = analogRead(bButton);
    if (bButtonValue < 200 && lastBButtonValue > 800) { 
      if(gameStatus == 1) {
        bButtonPushed = 1;
      }
    }
    lastBButtonValue = bButtonValue; // And we update what we read just after
  }
}

/** Retourne l'état de chaque bouton sous la forme d'un entier sur 16 bits. */
/* Only makes sense if gamePadMode == 1 */
uint16_t getSnesButtons() {
 
  /* 1 bouton = 1 bit */
  uint16_t value = 0;
 
  /* Capture de l'état courant des boutons */
  digitalWrite(PIN_LATCH, HIGH);
  digitalWrite(PIN_LATCH, LOW);
 
  /* Récupère l'état de chaque bouton (12 bits + 4 bits à "1") */
  for(byte i = 0; i < 16; ++i) {
 
    /* Lit l'état du bouton et décale le bit reçu pour former l'entier sur 16 bits final */
    value |= digitalRead(PIN_DATA) << i;
 
    /* Pulsation du signal d'horloge */
    digitalWrite(PIN_CLOCK, HIGH);
    digitalWrite(PIN_CLOCK, LOW);
  }
 
  /* Retourne le résultat sous une forme facile à manipuler (bouton appuyé = bit à "1") */
  return ~value;
}


void pushLinesDown() {
  // Getting each line down by one bit
  for(byte rowIterator = 0; rowIterator < displayNumberOfRows - 2; rowIterator++) {
    for(byte columnIterator = 0; columnIterator < displayNumberOfColumns; columnIterator++) {
      LEDMatrix[9-rowIterator][columnIterator] = LEDMatrix[9-rowIterator-1][columnIterator];
    }
  }
}

void checkCarCrash() {
  // We check if the car hit something
  if(LEDMatrix[9][carPosition] != Black) {
    gameStatus = 1;   // Meaning game over
  }
}

void showCar() {
   LEDMatrix[9][carPosition] = Car;
}

void displayScoreInGame() {
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


void addLineTopMatrix(byte newLine[6]) {
  // Create a new line in top of the existing matrix, with the passed argument.
  for(byte columnIterator = 0; columnIterator < displayNumberOfColumns; columnIterator++) {
    LEDMatrix[1][columnIterator] = newLine[columnIterator];
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
        if(LEDMatrix[rowIndex][columnIndex] == White)   {leds[(columnIndex + 1)*displayNumberOfRows - rowIndex - 1] = CRGB::White;}
        if(LEDMatrix[rowIndex][columnIndex] == Green)  {leds[(columnIndex + 1)*displayNumberOfRows - rowIndex - 1] = CRGB::Green;}
        if(LEDMatrix[rowIndex][columnIndex] == Blue)   {leds[(columnIndex + 1)*displayNumberOfRows - rowIndex - 1] = CRGB::Blue;}
        if(LEDMatrix[rowIndex][columnIndex] == Car && gameStatus == 0)    {leds[(columnIndex + 1)*displayNumberOfRows - rowIndex - 1] = CRGB::Red;}
        if(LEDMatrix[rowIndex][columnIndex] == Car && gameStatus == 2)    {leds[(columnIndex + 1)*displayNumberOfRows - rowIndex - 1] = CRGB::Yellow;}
        if(LEDMatrix[rowIndex][columnIndex] == Red)    {leds[(columnIndex + 1)*displayNumberOfRows - rowIndex - 1] = CRGB::Red;}
        if(LEDMatrix[rowIndex][columnIndex] == Purple) {leds[(columnIndex + 1)*displayNumberOfRows - rowIndex - 1] = CRGB::Purple;}
        // If we're above it, then we make the diod goes up and up in blue for the score. The rainbow before it is wanted as a "checkpoint"
        if(LEDMatrix[rowIndex][columnIndex] > 6)     {leds[(columnIndex + 1)*displayNumberOfRows - rowIndex - 1].setRGB(2*LEDMatrix[rowIndex][columnIndex]+30,0,0);}
      }
      // If we're on an uneven column, we do a mathematical trick to invert it
      else if(columnIndex%2 == 1) {
        if(LEDMatrix[rowIndex][columnIndex] == Black) {leds[columnIndex*displayNumberOfRows + rowIndex] = CRGB::Black;}
        if(LEDMatrix[rowIndex][columnIndex] == Wall) {leds[columnIndex*displayNumberOfRows + rowIndex] = CRGB::White;}
        if(LEDMatrix[rowIndex][columnIndex] == White) {leds[columnIndex*displayNumberOfRows + rowIndex] = CRGB::White;}
        if(LEDMatrix[rowIndex][columnIndex] == Green) {leds[columnIndex*displayNumberOfRows + rowIndex] = CRGB::Green;}
        if(LEDMatrix[rowIndex][columnIndex] == Blue) {leds[columnIndex*displayNumberOfRows + rowIndex] = CRGB::Blue;}
        if(LEDMatrix[rowIndex][columnIndex] == Car && gameStatus == 0) {leds[columnIndex*displayNumberOfRows + rowIndex] = CRGB::Red;}
        if(LEDMatrix[rowIndex][columnIndex] == Car && gameStatus == 2) {leds[columnIndex*displayNumberOfRows + rowIndex] = CRGB::Yellow;}
        if(LEDMatrix[rowIndex][columnIndex] == Red) {leds[columnIndex*displayNumberOfRows + rowIndex] = CRGB::Red;}
        if(LEDMatrix[rowIndex][columnIndex] == Purple) {leds[columnIndex*displayNumberOfRows + rowIndex] = CRGB::Purple;}
        // If we're above it, then we make the diod goes up and up in blue for the score. The rainbow before it is wanted as a "checkpoint"
        if(LEDMatrix[rowIndex][columnIndex] > 6)     {leds[columnIndex*displayNumberOfRows + rowIndex].setRGB(2*LEDMatrix[rowIndex][columnIndex]+30,0,0);}
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
         LEDMatrix[rowIterator][columnIterator] = White * numberTable[thousands][rowIterator][columnIterator];  
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
         LEDMatrix[rowIterator][columnIterator] = White * numberTable[units][rowIterator-5][columnIterator-3];  
    }
  }

  outputDisplay();
}

void reinitializeGame() {
  // We repass modified game variables to their initial positions
  carPosition = initialCarPosition;
  ticker = initialTicker;          
  playerScore = initialPlayerScore;
  playerAdvancement = initialPlayerAdvancement;     // Will be like the player score but not exactly. Will not be increased when under a star effect. We'll use this to augment game difficulty.

  screenMoves = frameRateT[0];               // In miliseconds, how fast will the car go. That will be updated during the game.
  probaApparitionLigne = probaApparitionLigneT[0];    // Probability of a new line appearing when possible
  probaApparitionBlock = probaApparitionBlockT[0];   // For a new line, for each block, the probability of it being a "wall"

  // We set the game status to "play"
  gameStatus = 0;

  // We clear the LED Matrix
  clearLEDMatrix(); // Digitaly
  outputDisplay();  // And physically
}

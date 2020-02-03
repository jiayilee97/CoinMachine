#include <Wire.h> 
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h> // include i/o class header
#include <Keypad.h>

// LEDs

#define RED 11
#define GREEN 12
#define ELECTROMAGNET 13

// KEYPAD

const byte numRows = 4; //number of rows on the keypad
const byte numCols = 4; //number of columns on the keypad

// keymap defines the key pressed according to the row and columns
char keymap[numRows][numCols] = { 
  {'1', '2', '3', 'A'}, 
  {'4', '5', '6', 'B'}, 
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

// keypad connections to arduino (change after connecting the rest of the components)
byte rowPins[numRows] = {9,8,7,6}; // rows 0 to 3
byte colPins[numCols]= {5,4,3,2}; // columns 0 to 3

Keypad myKeypad = Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);

// LCD
// LiquidCrystal_I2C lcd(0x3f,20,4);  // set the LCD address to 0x3F for a 20 chars and 4 line display
hd44780_I2Cexp lcd; // declare lcd object: auto locate & config display for hd44780 chip

// PROGRAM VARIABLES

// GETTING AND STORING INPUT

// input is of the format XX.XX
double inputAmt = 0.0;
boolean inputReceived = false;

// CALCULATING CURRENT AMOUNT

int valOneDollar, countOneDollar = 0;
int valFiftyCents, countFiftyCents = 0;
int valTwentyCents, countTwentyCents = 0;
int valTenCents, countTenCents = 0;
int valFiveCents, countFiveCents = 0;
int valCoinInserted = 0; // added by Jia Yi
double currAmt = 0.0;

boolean changedOneDollar = false;
boolean changedFiftyCents = false;
boolean changedTwentyCents = false;
boolean changedTenCents = false;
boolean changedFiveCents = false;

void setup() {
//  Serial.begin(250000);
  Serial.begin(9600);
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(ELECTROMAGNET, OUTPUT);
//  lcd.init();
//  lcd.backlight();
  lcd.begin(20, 4);
  lcd.clear();
  delay(1000);
}

void getInput() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Enter amount XX.XX: ");
  lcd.setCursor(0,2);
  lcd.print("C to cancel");
  lcd.setCursor(0,3);
  lcd.print("D to enter");
  
  int i = 0; 
  char inputArray[5];
  boolean cancelled = false;

  lcd.setCursor(13,1);
  while(i < 5) {
    if(i == 2) {
      inputArray[i] = '.';
      lcd.print(inputArray[i]);  
      i++;
    } 

    char keyPressed = myKeypad.getKey();
    
    if(keyPressed != NO_KEY && keyPressed != 'C') {
      inputArray[i] = keyPressed;
      lcd.print(inputArray[i]);
      i++;
    } else if(keyPressed == 'C') {
      lcd.clear();
      lcd.print("Cancelling...");
      cancelled = true;
      delay(2000);
      break;
    }
  }
  
  if(cancelled == false) {
    Serial.println("entered this");
    inputAmt = atof(inputArray);
  
    char enterKey = myKeypad.waitForKey();
    
    if(enterKey == 'D') {
      inputReceived = true;
      Serial.println("received input");
      lcd.clear();
    } 
  }
}

void incOneDollar() {
  countOneDollar = countOneDollar + 1;
}

void incFiftyCents() {
  countFiftyCents = countFiftyCents + 1;
}

void incTwentyCents() {
  countTwentyCents = countTwentyCents + 1;
}

void incTenCents() {
  countTenCents = countTenCents + 1;
}

void incFiveCents() {
  countFiveCents = countFiveCents + 1;
}

int getCurrAmtInCents() {
  return (100*countOneDollar + 50*countFiftyCents + 20*countTwentyCents + 10*countTenCents + 5*countFiveCents);
}

int getNumDollars() {
  return (getCurrAmtInCents()/100);
}

int getNumCents() {
  return (getCurrAmtInCents() % 100);
}

double getCurrAmt() { 
  double var = getNumDollars() + ((double)getNumCents())/100;
  return var;
}

void printCurrAmt() {
  lcd.setCursor(14, 1); // second row
  lcd.print(getNumDollars());
  lcd.print(".");
  lcd.print(getNumCents());
}

int compareAmts() {
  if(getCurrAmt() > inputAmt) {
    return 1;
  } else if(getCurrAmt() == inputAmt){
    return 0;
  }

  // for all other values
  return -1;
}

void flashGreenLED() {
  digitalWrite(GREEN, HIGH);
}

void flashRedLED() {
  digitalWrite(RED, HIGH);
}

void reset() {
  inputReceived = false;
  countOneDollar = 0;
  countFiftyCents = 0;
  countTwentyCents = 0;
  countTenCents = 0;
  countFiveCents = 0;
  digitalWrite(GREEN, LOW);
  digitalWrite(RED, LOW);
}

void loop() {
  
  
    // get input from keypad until it is a valid input
    while(inputReceived != true) {
      Serial.println("test");
      getInput();
      //digitalWrite(ELECTROMAGNET, LOW);  // added by Jia Yi
    }
    
    lcd.setCursor(0, 0); // first row
    lcd.print("Goal: ");
    lcd.print(inputAmt);
    
    lcd.setCursor(0, 1); // second row 
    lcd.print("Amount now: ");
    valOneDollar = analogRead(A0); // sensor for one dollar connects to A0
    valFiftyCents = analogRead(A1); // sensor for fifty cents connects to A1
    valTwentyCents = analogRead(A2); // sensor for twenty cents connects to A2.
    valTenCents = analogRead(A3); // sensor for ten cents connects to A3

    // ------added by Jia Yi------//
    // A3, A4, A5 ports are faulty
    // 2 photointerruptors are faulty
    // Baud rate differs for every computer
    // My computer can only run baud rate of 9600
    // Other computers may run baud rate of 250000
    Serial.print("ten");
    Serial.print(analogRead(A2));
    Serial.print(" ");
    Serial.print(analogRead(A4));
    Serial.print(" ");
    Serial.print(analogRead(A5));
    Serial.print("\n");
    delay(500);
    //valCoinInserted = analogRead(A4); // sensor for coinInserted connects to A4
    //digitalWrite(ELECTROMAGNET,HIGH); // added by Jia Yi
    // ------Jia Yi's addition ends------//
    
    //valFiveCents = analogRead(A5); // sensor for five cents connects to A4
    //valFiveCents = digitalRead(10); // sensor for five cents connects to 10
//
//    Serial.print("1: ");
//    Serial.print(valOneDollar);
//    Serial.print(" ");
//    Serial.print("50: ");
//    Serial.print(valFiftyCents);
//    Serial.print(" ");   
//    Serial.print("20: ");
//    Serial.print(valTwentyCents);
//    Serial.print(" ");    
//    Serial.print("10: ");
//    Serial.print(valTenCents);
//    Serial.print(" ");    
//    Serial.print("5: ");
//    Serial.print(valFiveCents);
//    Serial.println("");

    // used an if-else-if ladder because coins are put in one by one (events are mutually exclusive)
    if(valOneDollar < 300 && changedOneDollar != true) {
      // Serial.print("100");
      changedOneDollar = true;
      incOneDollar();
      delay(10);
    } 

    else if(valFiftyCents < 300 && changedFiftyCents != true) {
      //Serial.print("50");
      changedFiftyCents = true;
      incFiftyCents();
      delay(10);
    } 
    
    else if(valTwentyCents < 300 && changedTwentyCents != true) {
      //Serial.print("20");
      changedTwentyCents = true;
      incTwentyCents();
      delay(10);
    } 

    else if(valTenCents < 300 && changedTenCents != true) {
      //Serial.print("10");
      changedTenCents = true;
      incTenCents();
      delay(10);
    } 

//    else if(valFiveCents < 300 && changedFiveCents != true) {
//      //Serial.print("10");
//      changedFiveCents = true;
//      incFiveCents();
//      delay(10);
//    } 

    // for next coin initialise the boolean values again
    if(valOneDollar > 300) changedOneDollar = false;
    if(valFiftyCents > 300) changedFiftyCents = false;
    if(valTwentyCents > 300) changedTwentyCents = false;
    if(valTenCents > 300) changedTenCents = false;
    if(valFiveCents > 300) changedFiveCents = false;

    printCurrAmt(); // prints amount after every coin is added

    // compare current total amount with final value to be reached
    lcd.setCursor(0, 2); // third row
    
    // showing amount that needs to be added 
    lcd.print("Need to put: ");
    lcd.print(inputAmt - getCurrAmt());
    // delay(100); // check if delay needs to be added here
    
    if(compareAmts() == 0) {
      // flash green LED and print
      flashGreenLED();
      delay(1000);
      lcd.clear();
      lcd.print("Congratulations!");
      lcd.setCursor(0, 2); // 3rd row
      lcd.print("Press A to reset");
      char enterKey = myKeypad.waitForKey();
    
      if(enterKey == 'A') {
        lcd.clear();
        lcd.print("Resetting");
        delay(5000);
        lcd.clear();
        reset();
      }
    } else if(compareAmts() == 1){
      // flash red LED and print
      flashRedLED();
      delay(5000);
      lcd.clear();
      lcd.print("Amount exceeded.");
      lcd.setCursor(0, 2); // 3rd row
      lcd.print("Press A to reset");

      char enterKey = myKeypad.waitForKey();
    
      if(enterKey == 'A') {
        lcd.clear();
        lcd.print("Resetting");
        delay(1000);
        lcd.clear();
        reset();
      }
    }
    
    char resetKey = myKeypad.getKey();

    if(resetKey == 'A') {
      reset();
    }
}

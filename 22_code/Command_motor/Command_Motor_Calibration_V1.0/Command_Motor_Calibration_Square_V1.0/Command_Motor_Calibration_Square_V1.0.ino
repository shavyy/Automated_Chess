#include <Wire.h>
// LCD
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Motor
int microsteps=8;
const int MOTOR_WHITE_DIR = 28;
const int MOTOR_WHITE_STEP = 26;
const int MOTOR_WHITE_EN = 24;

const int MOTOR_BLACK_STEP = 36;
const int MOTOR_BLACK_DIR = 34;
const int MOTOR_BLACK_EN = 30;

const int stepsPerRev=200;
const int SQUARE_SIZE = 185*microsteps;  // Put the Steps for 1 Square. Test changing that nubmer if Trolley go more/less than 1 square
const float SQUARE_SIZE_MM = 37.0; // put your measure of 1 Square 37mm
const int POS_MAX_X = 1500; //Steps number from POS 0 to max (1510)
const int POS_MAX_Y = 1300; //Steps number from POS 0 to max (1310)
const int SPEED_SLOW (600); //3500/microsteps but check it is not Less than 150. 
const int SPEED_FAST (300); // 2000/microsteps but check it is not Less than 100
const float SQRT2 = 1.41421;
//  Button - Switch
const byte BUTTON_HA(11);
const byte BUTTON_AH (6);
const byte BUTTON_F_81 (3); 
const byte BUTTON_F_18 (2); 
const byte SEE_SERIAL (14);
const byte LIMIT_SWITCH_BLACK (4); 
const byte LIMIT_SWITCH_WHITE (5);
enum {WHITE, BLACK};
enum sequence {AH,HA,F18,F81,Result,none};
sequence mySequence;

// Add new Variables for this test
int step_H_A_count=0;
int step_F8_F1_count=0;
int step_AH_18_count=0;
int step_HA_81_count=0;
int step_AH_81_count=0;
int step_HA_18_count=0;
unsigned long previousMillis = 0;
//const long interval = 300; // Intervalo para la revisión de sensores y actualización de LED
unsigned long lastMoveTime = 0;
const long moveInterval = 50; // Intervalo de movimiento en milisegundos 

enum {A_H,H_A,F1_F8,F8_F1,AH_18, HA_81, AH_81, HA_18, calibrate_speed};	// V9.2.7.1 
const char* directionNames[] = {"A_H", "H_A", "F1_F8", "F8_F1", "AH_18", "HA_81", "AH_81", "HA_18", "calibrate_speed"}; // V9.2.3 for Serial Print the name

void setup() {
	Serial.begin(9600);
	Serial.print("Instructions");
	Serial.println(" ");
	Serial.print("With no Power, put the Trolley in the center.");
	Serial.println(" ");
	Serial.print("Power on. Press Buttons to Move. The Trolley have to move 1 Square");
	Serial.println(" ");
	Serial.print("Make with pencil on the rail marks for 1 square for check");
	Serial.println(" ");
	Serial.print("Press D14 Button for see the Serial Print on monitor");
	Serial.println(" ");

//  Motor
  pinMode (MOTOR_WHITE_STEP, OUTPUT);
  pinMode (MOTOR_WHITE_DIR, OUTPUT);
  pinMode (MOTOR_BLACK_STEP, OUTPUT);
  pinMode (MOTOR_BLACK_DIR, OUTPUT);
  pinMode (MOTOR_WHITE_EN, OUTPUT);
  pinMode (MOTOR_BLACK_EN, OUTPUT);

//  Arcade button - Limit Switch
  pinMode (BUTTON_AH, INPUT_PULLUP);
  pinMode (BUTTON_HA, INPUT_PULLUP);
  pinMode (BUTTON_F_18, INPUT_PULLUP);
  pinMode (BUTTON_F_81, INPUT_PULLUP);
  pinMode (SEE_SERIAL, INPUT_PULLUP);

  digitalWrite(MOTOR_WHITE_EN, HIGH); 
	digitalWrite(MOTOR_BLACK_EN, HIGH);
}

void loop() {
	if (digitalRead(BUTTON_AH) != HIGH)
	{
	 mySequence = AH;
	}
if (digitalRead(BUTTON_HA) != HIGH)
	{
	 mySequence= HA;
	}
if (digitalRead(BUTTON_F_18) != HIGH)
	{
	 mySequence= F18;
	}
if (digitalRead(BUTTON_F_81) != HIGH)
	{
	 mySequence= F81;
	}
if (digitalRead(SEE_SERIAL) != HIGH)
	{
		mySequence = Result;
	}

switch (mySequence){

	case AH:
		Serial.print("Button AH Pressed = ");
		Serial.println(BUTTON_AH);
    
		motor(A_H, SPEED_SLOW, 1);
		mySequence= none;
	break;

	case HA:
		Serial.print("Button HA Pressed = ");
		Serial.println(BUTTON_HA);
    
		motor(H_A, SPEED_SLOW, 1);
		mySequence= none;
	break;

	case F18:
		Serial.print("Button F18 Pressed = ");
		Serial.println(BUTTON_F_18);
    
		motor(F1_F8, SPEED_SLOW, 1);
		mySequence= none;
	break;

	case F81:
		Serial.print("Button F81 Pressed = ");
		Serial.println(BUTTON_F_81);
    
		motor(F8_F1, SPEED_SLOW, 1);
		mySequence= none;
	break;

	case Result:

		Serial.print("SQUARE_SIZE = ");
		Serial.println(SQUARE_SIZE);
		Serial.println(" microsteps");
		Serial.print("SQUARE_SIZE_MM = ");
		Serial.print(SQUARE_SIZE_MM);
		Serial.println("mm");
		Serial.print("---------------------------------");
		Serial.println();
		Serial.print("If not made that distance, change the SQUARE_SIZE on the code ");
		Serial.println("---");
   Serial.println("");
		delay(500);
		mySequence= none;
	break;

	case none:

	break;

}
}
// ****************************************  MOTOR
void motor(byte direction, int speed, float distance) { 
// for example: motor(F1_F8, SPEED_FAST, 1); 

 int step_number = 0;  
// Set Step Number

 if (direction == AH_18 || direction == HA_81 || direction == AH_81 || direction == HA_18) step_number = distance * SQUARE_SIZE * SQRT2; // **** 
 else step_number = distance * SQUARE_SIZE; // ***
Serial.print("step_number= ");
Serial.print(step_number);
Serial.println("");

digitalWrite(MOTOR_WHITE_EN, LOW);
digitalWrite(MOTOR_BLACK_EN, LOW);

  //  Direction of the motor rotation // V10 -> See the Excel

    if (direction == H_A || direction == F1_F8 || direction == HA_18 ) {
    digitalWrite(MOTOR_WHITE_DIR, HIGH);
    //printDirState("MOTOR_WHITE", MOTOR_WHITE_DIR); 
    }
    else {
      digitalWrite(MOTOR_WHITE_DIR, LOW);
      //printDirState("MOTOR_WHITE", MOTOR_WHITE_DIR); 
    }
    if (direction == A_H|| direction == F1_F8 || direction == AH_18) {
      digitalWrite(MOTOR_BLACK_DIR, HIGH);
      //printDirState("MOTOR_BLACK", MOTOR_BLACK_DIR); 
     }
    else {
      digitalWrite(MOTOR_BLACK_DIR, LOW);
      //printDirState("MOTOR_BLACK", MOTOR_BLACK_DIR)  
     }
printDirState("MOTOR_WHITE", MOTOR_WHITE_DIR);
printDirState("MOTOR_BLACK", MOTOR_BLACK_DIR);
Serial.println("");

//  Active the motors // V10 -> See the Excel
  for (int x = 0; x < step_number; x++){
    if (direction == AH_18 || direction == HA_81 ){
      digitalWrite(MOTOR_WHITE_STEP, LOW); 
    }
    else {
      digitalWrite(MOTOR_WHITE_STEP, HIGH);   
    }
    if (direction == AH_81 || direction == HA_18 ) {
      digitalWrite(MOTOR_BLACK_STEP, LOW);  
     }
    else {
      digitalWrite(MOTOR_BLACK_STEP, HIGH); 
    }
    delayMicroseconds(speed);
    digitalWrite(MOTOR_WHITE_STEP, LOW);  
    digitalWrite(MOTOR_BLACK_STEP, LOW); 
    delayMicroseconds(speed);
  }

  digitalWrite(MOTOR_WHITE_EN, HIGH);
  digitalWrite(MOTOR_BLACK_EN, HIGH);
  
}
// ****** Pin state
void printDirState(const char* motorName, byte pin) {
  if (digitalRead(pin) == HIGH) {
    Serial.print(motorName);
    Serial.print(" _DIR= HIGH");
  } else {
    Serial.print(motorName);
    Serial.print(" _DIR= LOW");
  }
  Serial.println("");
}
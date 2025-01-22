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
const byte BUTTON_AH_18 (11);
const byte BUTTON_HA_18 (6);
const byte BUTTON_REVERSE_WHITE (3); 
const byte BUTTON_REVERSE_BLACK (2); 
const byte SEE_SERIAL (14);
const byte LIMIT_SWITCH_BLACK (4); 
const byte LIMIT_SWITCH_WHITE (5);
enum {WHITE, BLACK};

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
	Serial.print("Power on. Press Buttons to Diagonal Moves");
	Serial.println(" ");
	Serial.print("Press D14 Button for Serial Print");
	Serial.println(" ");

//  Motor
  pinMode (MOTOR_WHITE_STEP, OUTPUT);
  pinMode (MOTOR_WHITE_DIR, OUTPUT);
  pinMode (MOTOR_BLACK_STEP, OUTPUT);
  pinMode (MOTOR_BLACK_DIR, OUTPUT);
  pinMode (MOTOR_WHITE_EN, OUTPUT);
  pinMode (MOTOR_BLACK_EN, OUTPUT);

//  Arcade button - Limit Switch
  pinMode (BUTTON_AH_18, INPUT_PULLUP);
  pinMode (BUTTON_HA_18, INPUT_PULLUP);
  pinMode (BUTTON_REVERSE_WHITE, INPUT_PULLUP);
  pinMode (BUTTON_REVERSE_BLACK, INPUT_PULLUP);
  pinMode (SEE_SERIAL, INPUT_PULLUP);

  digitalWrite(MOTOR_WHITE_EN, HIGH); 
	digitalWrite(MOTOR_BLACK_EN, HIGH);
}

void loop() {
	while (digitalRead(BUTTON_AH_18) != HIGH)
	{
	 motor(AH_18, SPEED_SLOW, 1);
	 step_AH_18_count++;
	}
	while (digitalRead(BUTTON_REVERSE_WHITE) != HIGH)
	{
	 motor(HA_81, SPEED_SLOW, 1);
	 step_HA_81_count++;
	}
	while (digitalRead(BUTTON_HA_18) != HIGH)
	{
	 motor(HA_18, SPEED_SLOW, 1);
	 step_HA_18_count++;
	}
	while (digitalRead(BUTTON_REVERSE_BLACK) != HIGH)
	{
	 motor(AH_81, SPEED_SLOW, 1);
	 step_AH_81_count++;
	}
	if (digitalRead(SEE_SERIAL) != HIGH)
	{

	Serial.print("Steps on HA-18 = ");
	Serial.println(step_AH_18_count);
	Serial.print("Distance HA_18 = ");
	Serial.print(float(step_AH_18_count)*SQUARE_SIZE_MM*microsteps/SQUARE_SIZE);
	Serial.println("mm");

	Serial.print("Steps on HA_81 = ");
	Serial.println(step_HA_81_count);
	Serial.print("Distance HA_81 = ");
	Serial.print(float(step_HA_81_count)*SQUARE_SIZE_MM*microsteps/SQUARE_SIZE);
	Serial.println("mm");

	Serial.print("Steps on AH_81 = ");
	Serial.println(step_AH_81_count);
	Serial.print("Distance AH_81 = ");
	Serial.print(float(step_AH_81_count)*SQUARE_SIZE_MM*microsteps/SQUARE_SIZE);
	Serial.println("mm");

	Serial.print("Steps on HA_18 = ");
	Serial.println(step_HA_18_count);
	Serial.print("Distance HA_18 = ");
	Serial.print(float(step_HA_18_count)*SQUARE_SIZE_MM*microsteps/SQUARE_SIZE);
	Serial.println("mm");

	Serial.print("---------------------------------");
	Serial.println();
	delay(1000);
	}
}
// ****************************************  MOTOR
void motor(byte direction, int speed, float distance) 
{ 
  digitalWrite(MOTOR_WHITE_EN, LOW); 
	digitalWrite(MOTOR_BLACK_EN, LOW);

// for example: motor(F1_F8, SPEED_FAST, 1); 

 int step_number = 0;  
// Set Step Number

 if (direction == AH_18 || direction == HA_81 || direction == AH_81 || direction == HA_18) step_number = distance * microsteps * SQRT2; // V9.2.7.1 
 else step_number = distance * microsteps;

//  Set Direction of the motor rotation

   if (direction == F1_F8 || direction == A_H || direction == AH_18) digitalWrite(MOTOR_WHITE_DIR, LOW); // Original: HIGH // Original F8_F1 // V9.2.6 Swap for test
    else digitalWrite(MOTOR_WHITE_DIR, HIGH);                                                               // Original: LOW // V9.2.6 Swap for test
    if (direction == H_A || direction == F1_F8 || direction == HA_18) digitalWrite(MOTOR_BLACK_DIR, LOW);  // Original: HIGH // Original F8_F1 // V9.2.6 Swap for test
    else digitalWrite(MOTOR_BLACK_DIR, HIGH);         

  //  Active the motors

  for (int x = 0; x < step_number; x++) {
    if (direction == AH_81 || direction == AH_18) digitalWrite(MOTOR_WHITE_STEP, HIGH); // v2
    else digitalWrite(MOTOR_WHITE_STEP, LOW);																						//v2
    if (direction == HA_18 || direction == HA_81) digitalWrite(MOTOR_BLACK_STEP, HIGH);	//v2
    else digitalWrite(MOTOR_BLACK_STEP, LOW);																						//v2
    delayMicroseconds(speed);
    digitalWrite(MOTOR_WHITE_STEP, LOW);   
    digitalWrite(MOTOR_BLACK_STEP, LOW);
    delayMicroseconds(speed);
  }
  
  digitalWrite(MOTOR_WHITE_EN, HIGH); 
	digitalWrite(MOTOR_BLACK_EN, HIGH);
}

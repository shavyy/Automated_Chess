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
//  Button - Switch
const byte BUTTON_WHITE_SWITCH_MOTOR_WHITE (11);
const byte BUTTON_BLACK_SWITCH_MOTOR_BLACK (6);
const byte BUTTON_REVERSE_WHITE (3); 
const byte BUTTON_REVERSE_BLACK (2); 
const byte SEE_SERIAL (14);
const byte LIMIT_SWITCH_BLACK (4); 
const byte LIMIT_SWITCH_WHITE (5);
enum {WHITE, BLACK};

//  Electromagnet H Bridge L298
int IN1 = (15);
int IN2 = (18);
int MAGNET (19);  // Enable or not the H-Bridge

// Add new Variables for this test
int step_H_A_count=0;
int step_F8_F1_count=0;
unsigned long previousMillis = 0;
//const long interval = 300; // Intervalo para la revisión de sensores y actualización de LED
unsigned long lastMoveTime = 0;
const long moveInterval = 50; // Intervalo de movimiento en milisegundos 

enum {A_H,H_A,F1_F8,F8_F1,AH_18, HA_81, AH_81, HA_18, calibrate_speed};	// V9.2.7.1 
const char* directionNames[] = {"A_H", "H_A", "F1_F8", "F8_F1", "AH_18", "HA_81", "AH_81", "HA_18", "calibrate_speed"}; // V9.2.3 for Serial Print the name

void setup() {
  Serial.begin(9600);
	Serial.print("-----------  Instructions  --------------------------------------------------------------------------");
	Serial.println(" ");
	Serial.print("Trolley will move until Touchs the Microswitchs.");
	Serial.println(" ");
	Serial.print("4 buttons, 4 different Movements");
	Serial.println(" ");
	Serial.print("Only press one button a time");
	Serial.println(" ");
	Serial.print("When press D16 button, it will Show the steps and distance made");
  Serial.println("Wait until SETUP is finished");

	//  Electromagnet
  	pinMode(IN1, OUTPUT);
  	pinMode(IN2, OUTPUT);	
	pinMode (MAGNET, OUTPUT);
	//  Motor
	pinMode (MOTOR_WHITE_STEP, OUTPUT);
	pinMode (MOTOR_WHITE_DIR, OUTPUT);
  pinMode (MOTOR_WHITE_EN, OUTPUT);
	pinMode (MOTOR_BLACK_STEP, OUTPUT);
	pinMode (MOTOR_BLACK_DIR, OUTPUT);
  pinMode (MOTOR_BLACK_EN, OUTPUT);
	//  Arcade button - Limit Switch
	pinMode (BUTTON_WHITE_SWITCH_MOTOR_WHITE, INPUT_PULLUP);
	pinMode (BUTTON_BLACK_SWITCH_MOTOR_BLACK, INPUT_PULLUP);
	pinMode (BUTTON_REVERSE_WHITE, INPUT_PULLUP);
	pinMode (BUTTON_REVERSE_BLACK, INPUT_PULLUP);
	pinMode (SEE_SERIAL, INPUT_PULLUP);
  pinMode (LIMIT_SWITCH_BLACK, INPUT_PULLUP);
  pinMode (LIMIT_SWITCH_WHITE, INPUT_PULLUP);
	//  LCD
	lcd.init();
	lcd.backlight();

	// Put the Trolley on the Switch
	//  Slow displacements up to touch the limit switches
	while (digitalRead(LIMIT_SWITCH_BLACK) == HIGH) motor(H_A, SPEED_SLOW, 1); // A_H If White Microswitch is near H1. ¡¡ RB put H_A !!
	while (digitalRead(LIMIT_SWITCH_WHITE) == HIGH) motor(F8_F1, SPEED_SLOW, 1); // F1_F8 if Black Microswitch is on Row 8

	delay(500);
	step_H_A_count=0;
	step_F8_F1_count=0;
	Serial.println();
	Serial.print("End Set Up");
	Serial.println("----------------------------------------------------------- ");

	digitalWrite(MOTOR_WHITE_EN, HIGH); 
	digitalWrite(MOTOR_BLACK_EN, HIGH);
	
	// Initial set for Black Piece
  digitalWrite(MAGNET, LOW);
  digitalWrite(IN1, LOW); // Change for invert Polarization
  digitalWrite(IN2, HIGH);  // Change for invert Polarization
}

void loop() {
	while (digitalRead(BUTTON_WHITE_SWITCH_MOTOR_WHITE) != HIGH)
	{
		motor(H_A, SPEED_SLOW, 1);
		step_H_A_count++;
	}
	while (digitalRead(BUTTON_REVERSE_WHITE) != HIGH)
	{
		motor(A_H, SPEED_SLOW, 1);
		step_H_A_count--;
	}
	while (digitalRead(BUTTON_BLACK_SWITCH_MOTOR_BLACK) != HIGH)
	{
		motor(F8_F1, SPEED_SLOW, 1);
		step_F8_F1_count++;
	}
	while (digitalRead(BUTTON_REVERSE_BLACK) != HIGH)
	{
		motor(F1_F8, SPEED_SLOW, 1);
		step_F8_F1_count--;
	}
	if (digitalRead(SEE_SERIAL) != HIGH) // Cuando presiono el boton en D16 se ejecuta lo de abajo
	{
		//electroMagnetState = !electroMagnetState; // Prendo / Apago el Electromagnet cada vez que se presiona el Boton D16
		//electromagnet(electroMagnetState);
		Serial.print("Steps on H_A = ");
		Serial.println(abs(step_H_A_count));
		Serial.print("Distance H_A = ");
		Serial.print(float(abs(step_H_A_count)*microsteps*SQUARE_SIZE_MM/SQUARE_SIZE));
		Serial.println("mm");

		Serial.print("Steps on F8_F1 = ");
		Serial.println(abs(step_F8_F1_count));
		Serial.print("Distance F8_F1 = ");
		Serial.print(float(abs(step_F8_F1_count)*microsteps*SQUARE_SIZE_MM/SQUARE_SIZE));
		Serial.println("mm");

		Serial.println("check if the Distance is correct");

			//delay(10);
			//See_Matrix();
			
		//delay(100);
	}
}
// ****************************************  MOTOR
void motor(byte direction, int speed, float distance) 
{ 
  digitalWrite(MOTOR_WHITE_EN, LOW); 
	digitalWrite(MOTOR_BLACK_EN, LOW);
  electromagnet(true);

	// for example: motor(F1_F8, SPEED_FAST, 1); 
	int step_number = distance*microsteps;  // Original: step_number = distance * SQUARE_SIZE;
	//  Direction of the motor rotation
	if (direction == F1_F8 || direction == A_H) digitalWrite(MOTOR_WHITE_DIR, LOW);
	else digitalWrite(MOTOR_WHITE_DIR, HIGH);
	if (direction == H_A || direction == F1_F8) digitalWrite(MOTOR_BLACK_DIR, LOW);
	else digitalWrite(MOTOR_BLACK_DIR, HIGH);

	//  Active the motors

	for (int x = 0; x < step_number; x++) {
		digitalWrite(MOTOR_WHITE_STEP, HIGH); 
		digitalWrite(MOTOR_BLACK_STEP, HIGH); 
		delayMicroseconds(speed);
		digitalWrite(MOTOR_WHITE_STEP, LOW);   
		digitalWrite(MOTOR_BLACK_STEP, LOW);
		delayMicroseconds(speed);
		/*if (millis() - lastMoveTime >= moveInterval) {
			lastMoveTime = millis();
			Read_Sensor (mux1);
			Read_Sensor (mux2);
			Read_Sensor (mux3);
			Read_Sensor (mux4);
			//delay(10);
			See_Matrix();
		}*/
	}
  electromagnet(false);
  digitalWrite(MOTOR_WHITE_EN, HIGH); 
	digitalWrite(MOTOR_BLACK_EN, HIGH);
}
// *******************************  ELECTROMAGNET H-Bridge 
void electromagnet(bool electroMagnetState) // For this Test
{
	if (electroMagnetState == true)  // For this Test
	{
		analogWrite(MAGNET, 255);  // V7.1 add PWM to Electromagnet. Test Value 0-256
		//delay(600);
	}
	else  
	{
		//delay(600);
		digitalWrite(MAGNET, LOW);
	}
}

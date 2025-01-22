// *** ON Going *** Test_H-Bridge_Electrom_V01_V01 August 2024
//	***
// Is just for test the Electromagnet with the H-Bridge, so can swap polarity and PWM the force of the Electromagnet
//	***
//
// Here we Test: the PWM of the Coil, the gap between the Board and the Coil, the Magnet used on the Pieces, the Weight of the piece
//
// The variables to change are:
// PWM (Only one on the code)
// Gap (Mechanical part of the board)
// Magnet Size and force (Mechanical Part of the Board)
// Weight of the Piece (Mehanical Part of the Board)
//
// The idea of this code is:
// With Power Off, put the pieces on the board.
// 
// Power On
// No Motor Coded here, so you have to move by hand the Trolley with the Electromagnet. 
// Touch D16 button for turn on the Electromagnet
// Touch D9 button for turn off the Electromagnet
// Touch White or Black button for set the polarity of the Electromagnet
// 
// 
// 


#include <Wire.h>


//  Electromagnet H Bridge L298
int IN1 = (15);
int IN2 = (18);
int MAGNET (19);  // Enable or not the H-Bridge

// **** Add new Variables for this test
// Define the range of PWM of ELECTROMAGNET
    const int pwmMax = 255;  // value max of PWM 
    const int incrementDelay = 50;  // Delay to the variation of PWM
//    const int holdTime = 500;  // Time in ms to keep the PWM to the max 
    bool state = false;

enum sequence {D11_Turn_ON,D10_Turn_OFF,D11_White_EM,D10_Black_EM,none};
sequence mySequence;


//  Button - Switch
const byte BUTTON_WHITE_SWITCH_MOTOR_WHITE (11);
const byte BUTTON_BLACK_SWITCH_MOTOR_BLACK (6);

// ****************************************  SETUP
void setup() {
  Serial.begin(9600);
Serial.print("Instructions");
Serial.println(" ");
Serial.println("With no Power, put the pieces on the board");
Serial.println(" ************** ");
Serial.println("Power on");
Serial.println(" ");
Serial.println("No Motor Coded here, so you have to move by hand the Trolley with the Electromagnet."); 
Serial.println("D11 (White Button) button fis for turn on the Electromagnet");
Serial.println(" ");
Serial.println("D10 (Black Button) button is for turn off the Electromagnet");
Serial.println(" ");
Serial.println(" ************** ");

//  Electromagnet
  	pinMode(IN1, OUTPUT);
  	pinMode(IN2, OUTPUT);	
	pinMode (MAGNET, OUTPUT);

//  Arcade button - Limit Switch
  pinMode (BUTTON_WHITE_SWITCH_MOTOR_WHITE, INPUT_PULLUP);
  pinMode (BUTTON_BLACK_SWITCH_MOTOR_BLACK, INPUT_PULLUP);

// Secuencia de Inicio

mySequence = none; // init to None
// Initial set for Black Piece
  digitalWrite(MAGNET, LOW);
  digitalWrite(IN1, LOW); // Change for invert Polarization
  digitalWrite(IN2, HIGH);  // Change for invert Polarization
   
} // Close Set Up

// *****************************************  LOOP
void loop() {


if (digitalRead(BUTTON_WHITE_SWITCH_MOTOR_WHITE) != HIGH) { 
  digitalWrite(IN1, LOW); // Change for invert Polarization
  digitalWrite(IN2, HIGH);  // Change for invert Polarization
  mySequence = D11_Turn_ON; 
  }
if (digitalRead(BUTTON_BLACK_SWITCH_MOTOR_BLACK) != HIGH) { 
  mySequence = D10_Turn_OFF;  
  }

switch (mySequence){

	case D11_Turn_ON:
		Serial.println("D11 Pressed. Electromagnet Will Turn On");
	electromagnet(true);
  break;
  
  case D10_Turn_OFF:
    Serial.println("D10 Pressed. Electromagnet Will Turn Off");
  electromagnet(false);
  break;
  
	case none:

	break;

} // close switch

// Waiting for button pressed
  /*if (mySequence != none) {
    Serial.println("**** Waiting for Button Press (D10 or D11) ****");
    while (HIGH && digitalRead(BUTTON_WHITE_SWITCH_MOTOR_WHITE) == HIGH && digitalRead(BUTTON_BLACK_SWITCH_MOTOR_BLACK) == HIGH) {
      delay(100);  // Pause to avoid to overload  processor
    }
  }*/

} // close loop



// *******************************  ELECTROMAGNET H-Bridge 
void electromagnet(bool state) // For this Test
{
	Serial.print("ElectroMagnet ");
	Serial.println(state ? "Enabled" : "Disabled");
	
	if (state == true)  // For this Test
	{
	 for (int pwmValue = 0; pwmValue <= pwmMax; pwmValue++) { // Add smooth increment, rather than going straight to the value to see if it helps prevent parts from moving out of position when turning on the EM
            analogWrite(MAGNET, 255); //pwmMax
            delay(incrementDelay);  // Wait before PWM change 
        }
	}
	else  
	{
		//delay(100);
		digitalWrite(MAGNET, LOW);
	}
}

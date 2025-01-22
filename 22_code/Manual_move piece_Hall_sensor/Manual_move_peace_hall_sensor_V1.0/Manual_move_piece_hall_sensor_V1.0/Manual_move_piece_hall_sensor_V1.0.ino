#include <C:\Users\shavy\Desktop\Automated chessboard\22_code\definition_V1.h>

//  Button - Switch
const byte BUTTON_REVERSE_WHITE (3); 
const byte BUTTON_REVERSE_BLACK (2); 
const byte SEE_SERIAL (14);
enum {WHITE, BLACK};

//  Electromagnet H Bridge L298s
bool electroMagnetState = false;

//MUX

// Add new Variables for this test
int step_H_A_count=0;
int step_F8_F1_count=0;

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
	Serial.print("When press D14 button, it will Show the steps and distance made");
	Serial.println("Wait until SETUP is finished");

	//  Electromagnet
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);	
	pinMode (MAGNET, OUTPUT);
	// Initial set for Black Piece
	digitalWrite(MAGNET, HIGH);
	digitalWrite(IN1, LOW); // Change for invert Polarization
	digitalWrite(IN2, HIGH);  // Change for invert Polarization
	
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
    // Initialisation de la matrice LED
	lc.shutdown(0, false);       // shutdown     : "true" active le mode "sommeil", tandis que "false" active le mode "normal"
	lc.setIntensity(0, 4);       // setIntensity : valeur pouvant aller de 0 à 15 (pour 16 niveaux de luminosité, donc)
	lc.clearDisplay(0);          // clearDisplay : éteint toutes les LEDs de la matrice
	
    //  Multiplexer 
     pinMode (Mux_Out, INPUT);
	  // Initialize the hall sensor memory array to 0
    hall_sensor_status_memory[8][8] = 0;
    // Initialize the hall sensor array to 0
    hall_sensor_status[8][8] = 0;

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
	
	//disable motor
	digitalWrite(MOTOR_WHITE_EN, HIGH); 
	digitalWrite(MOTOR_BLACK_EN, HIGH);
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
	if (digitalRead(SEE_SERIAL) != HIGH) // Acquisition of datas
	{
		electroMagnetState = !electroMagnetState; // Enable / disable the electromagnet
		electromagnet(electroMagnetState);

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

    Read_Sensor (mux1);
		Read_Sensor (mux2);
		Read_Sensor (mux3);
		Read_Sensor (mux4);
		delay(100);
		See_Matrix();
		delay(300);
	}
}
// ****************************************  MOTOR
void motor(byte direction, int speed, float distance) 
{ 
	digitalWrite(MOTOR_WHITE_EN, LOW); 
	digitalWrite(MOTOR_BLACK_EN, LOW);
  //electromagnet(true);

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
	}
  //electromagnet(false);
  digitalWrite(MOTOR_WHITE_EN, HIGH); 
	digitalWrite(MOTOR_BLACK_EN, HIGH);
}

void Read_Sensor (HC4067 &mux) 
{
	//  Read the hall sensor status
	mux.disable(); // Disable all mux
	// Enable MUX chosen with parameter
	mux.enable();

// Add matching row and column to the correct MUX
  if (&mux == &mux1) {
    column = 0;
    row = 0;
  } else if (&mux == &mux2) {
    column = 0;
    row = 2;
  } else if (&mux == &mux3) {
    column = 0;
    row = 4;
  } else if (&mux == &mux4) {
    column = 0;
    row = 6;
  }
 // Read the data of the activated mux 
  read_hall_values(mux);

  // disable the read
  mux.disable();
}
// ****************************************  Read Hall
void read_hall_values(HC4067 &mux)
{
  for (int j = 15; j >=0 ; j--) {
    mux.setChannel(j);
    hallMeasure = analogRead(Mux_Out);
    delay(5); // delay between read TBC
    Record_hall_measure(); // Record the value of the activated sensor
	column++;
    if (column > 7) {
      column = 0;
      row++;
    }
  }
}
// ****************************************  Record hall
void Record_hall_measure()
{
  if (hallMeasure <= hall_min) {
    hall_sensor_status[row][column] = 1;    // Depend on the polarity magnet, here is for black piece TBC
    hall_value[row][column] = hallMeasure;
  } else if (hallMeasure > hall_max) {
    hall_sensor_status[row][column] = -1;   // Depend on the polarity magnet, here is for white piece TBC
    hall_value[row][column] = hallMeasure;
  } else {
    hall_sensor_status[row][column] = 0;    // Depend on the polarity magnet, here is for no piece TBC
    hall_value[row][column] = hallMeasure;
  }
}
// ************************** See Matrix
void See_Matrix()	// Update Matrix
{
lc.clearDisplay(0);
  // hall value and matrix
  
  Serial.println("+ -    -   -   -  -   -   -   -   -+");
  for (int i = 7; i >= 0; i--) {
    Serial.print("row ");
    Serial.print(i+1); // Matrix on C++
    Serial.print("| ");
    for (int j = 7; j >=0; j--) {
      Serial.print(hall_value[i][j]); // excpected [row][column]

      Serial.print(" ");
    }
    Serial.println('|');
  }
  Serial.println("+ -    -   -   -  -   -   -   -   -+");
  Serial.println("xol: 0   1   2   3   4   5   6   7");

 
 // *** hall status and Matrix plus matrix LED
  Serial.println("+ - - - - - - - - -+");
  for (int i = 7; i >= 0; i--) {
    Serial.print("row ");
    Serial.print(i+1);
    Serial.print("| ");
    for (int j = 7; j >=0; j--) {
      Serial.print(hall_sensor_status[i][j]);
      if (hall_sensor_status[i][j] == 1 || hall_sensor_status[i][j] == -1) {
        lc.setLed(0, i, j, true); // lcd matrix column not folow the same logic 
      }
      Serial.print(" ");
    }
    Serial.println('|');
  }
  Serial.println("+ - - - - - - - - -+");
  Serial.println("  a  b c d e f g h");
}

// *******************************  ELECTROMAGNET H-Bridge 
void electromagnet(bool electroMagnetState) // For this Test
{
	Serial.print("Electroiman ");
	Serial.println(electroMagnetState ? "Encendido" : "Apagado");
	lcd.clear();
	lcd.print(electroMagnetState ? "Electroiman ON" : "Electroiman OFF");
	if (electroMagnetState == true)  // For this Test
	{
		analogWrite(MAGNET, 255);  // add PWM to Electromagnet. Test Value 0-256
		//delay(600);
	}
	else  
	{
		//delay(600);
		digitalWrite(MAGNET, LOW);
	}
}

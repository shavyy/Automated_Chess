#include <C:\Users\shavy\Desktop\Automated chessboard\22_code\definition_V1.h>

enum {WHITE, BLACK};

//  Electromagnet H Bridge L298s
bool electroMagnetState = false;

bool state = false;
enum sequence {Line1,Line5,Line8,none};
sequence mySequence=none;

const byte BUTTON_REVERSE_WHITE (3); 
const byte BUTTON_REVERSE_BLACK (2); 
const byte SEE_SERIAL (14);

void setup() {
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

	Serial.begin(9600);  
	Serial.println("----------------------------------------------------------- ");
	Serial.println("Calibration starts");
	// Put the Trolley on the Switch
	//  Slow displacements up to touch the limit switches
	while (digitalRead(LIMIT_SWITCH_BLACK) == HIGH) motor(H_A, SPEED_SLOW, calibrate_speed); 
	while (digitalRead(LIMIT_SWITCH_WHITE) == HIGH) motor(F1_F8, SPEED_SLOW, calibrate_speed); 
	delay(500);
	Serial.println("End Set Up");
	Serial.println("----------------------------------------------------------- ");
	
	//disable motor
	digitalWrite(MOTOR_WHITE_EN, HIGH); 
	digitalWrite(MOTOR_BLACK_EN, HIGH);

  digitalWrite(IN1, LOW); // Change for invert Polarization
	digitalWrite(IN2, HIGH);  // Change for invert Polarization

    electromagnet(true);
  	//  Update the hall status
	  Serial.println("**** See Matrix Update ****");
    Read_Sensor (mux1);
    Read_Sensor (mux2);
    Read_Sensor (mux3);
    Read_Sensor (mux4);
    delay(50);
    See_Matrix();
    delay(1000);
    electromagnet(false);
}

void loop() {
	if (digitalRead(BUTTON_REVERSE_WHITE) != HIGH) {
    mySequence = Line1;
  }
  if (digitalRead(BUTTON_REVERSE_BLACK) != HIGH) {
    mySequence = Line5;
  }
    if (digitalRead(SEE_SERIAL) != HIGH) {
    mySequence = Line8;
  }

switch (mySequence){

	case Line1:
	Serial.println("Trolley Will move from a8 Home to a1");
	//enable motor
	digitalWrite(MOTOR_WHITE_EN, LOW); 
	digitalWrite(MOTOR_BLACK_EN, LOW);

  electromagnet(true);
  delay(50);
	// home is a8 (position 0)
	//motor(AH_81,SPEED_FAST,0.1);
	motor(F8_F1,SPEED_FAST,7);
  electromagnet(false);
	
	// Move on the Line
	Serial.println("Trolley Will move from a1, Turn on and off the Electroagnet, then move to h1 and go up to a8");
	
	// rest of the moves
	moveTrolleyCurrentLine(); //a1 -> h1

	// Return to Home
	// h1 -> a8
  electromagnet(true);
  delay(50);
	Serial.println("**** Screw you, I'm going home ****");
	if (hall_sensor_status[0][7] == 0){
		while (digitalRead(LIMIT_SWITCH_BLACK) == HIGH) motor(H_A, SPEED_SLOW, calibrate_speed); 
		while (digitalRead(LIMIT_SWITCH_WHITE) == HIGH) motor(F1_F8, SPEED_SLOW, calibrate_speed); 
	} else {
		motor(F1_F8,SPEED_FAST,1); //h8 -> G7
		while (digitalRead(LIMIT_SWITCH_BLACK) == HIGH) motor(H_A, SPEED_SLOW, calibrate_speed); 
		while (digitalRead(LIMIT_SWITCH_WHITE) == HIGH) motor(F1_F8, SPEED_SLOW, calibrate_speed); 
	}

	  Read_Sensor (mux1);
    Read_Sensor (mux2);
    Read_Sensor (mux3);
    Read_Sensor (mux4);
    delay(50);
    See_Matrix();
    delay(50);
    electromagnet(false);
	Serial.println("**** Hey, I'm back! ****");
	
	mySequence = none; //  stop the sequence
	digitalWrite(MOTOR_WHITE_EN, HIGH); //disable white motor
	digitalWrite(MOTOR_BLACK_EN, HIGH); //disable black motor
	break;

  case Line5:
	Serial.println("Trolley Will move from a8 Home to a5");
	//enable motor
	digitalWrite(MOTOR_WHITE_EN, LOW); 
	digitalWrite(MOTOR_BLACK_EN, LOW);
  
  electromagnet(true);
  delay(50);
	// home is a8 (position 0)
	//motor(AH_81,SPEED_FAST,0.1);
	motor(F8_F1,SPEED_FAST,3);
  electromagnet(true);
	
	// Move on the Line
	Serial.println("Trolley Will move from a5, Turn on and off the Electroagnet, then move to h5 and go up to a8");
	
	// rest of the moves
	moveTrolleyCurrentLine(); //a5 -> h5

	// Return to Home
	// h1 -> a8
  electromagnet(true);
  delay(50);
	Serial.println("**** Screw you, I'm going home ****");
	if (hall_sensor_status[0][7] == 0){
		while (digitalRead(LIMIT_SWITCH_BLACK) == HIGH) motor(H_A, SPEED_SLOW, calibrate_speed); 
		while (digitalRead(LIMIT_SWITCH_WHITE) == HIGH) motor(F1_F8, SPEED_SLOW, calibrate_speed); 
	} else {
		motor(F1_F8,SPEED_FAST,1); //h8 -> G7
		while (digitalRead(LIMIT_SWITCH_BLACK) == HIGH) motor(H_A, SPEED_SLOW, calibrate_speed); 
		while (digitalRead(LIMIT_SWITCH_WHITE) == HIGH) motor(F1_F8, SPEED_SLOW, calibrate_speed); 
	}

	  Read_Sensor (mux1);
    Read_Sensor (mux2);
    Read_Sensor (mux3);
    Read_Sensor (mux4);
    delay(50);
    See_Matrix();
    delay(50);
    electromagnet(false);
	Serial.println("**** Hey, I'm back! ****");
	
	mySequence = none; //  stop the sequence
	digitalWrite(MOTOR_WHITE_EN, HIGH); //disable white motor
	digitalWrite(MOTOR_BLACK_EN, HIGH); //disable black motor
	break;

case Line8:
		Serial.println("Trolley Will remain to a8");
	//enable motor
	digitalWrite(MOTOR_WHITE_EN, LOW); 
	digitalWrite(MOTOR_BLACK_EN, LOW);

  electromagnet(true);
  delay(50);
	// home is a8 (position 0)
	//motor(AH_81,SPEED_FAST,0.1);
	motor(F8_F1,SPEED_FAST,0);
  electromagnet(false);
	
	// Move on the Line
	Serial.println("Trolley Will move from a8, Turn on and off the Electroagnet, then move to h8 and go up to a8");
	
	// rest of the moves
	moveTrolleyCurrentLine(); //a8 -> h8

	// Return to Home
	// h2 -> a8
  electromagnet(true);
  delay(50);
	Serial.println("**** Screw you, I'm going home ****");
	if (hall_sensor_status[0][7] == 0){
		while (digitalRead(LIMIT_SWITCH_BLACK) == HIGH) motor(H_A, SPEED_SLOW, calibrate_speed); 
		while (digitalRead(LIMIT_SWITCH_WHITE) == HIGH) motor(F1_F8, SPEED_SLOW, calibrate_speed); 
	} else {
		motor(F8_F1,SPEED_FAST,1); //h8 -> G7
		while (digitalRead(LIMIT_SWITCH_BLACK) == HIGH) motor(H_A, SPEED_SLOW, calibrate_speed); 
		while (digitalRead(LIMIT_SWITCH_WHITE) == HIGH) motor(F1_F8, SPEED_SLOW, calibrate_speed); 
	}

	  Read_Sensor (mux1);
    Read_Sensor (mux2);
    Read_Sensor (mux3);
    Read_Sensor (mux4);
    delay(50);
    See_Matrix();
    delay(50);
    electromagnet(false);
	Serial.println("**** Hey, I'm back! ****");
	
	mySequence = none; //  stop the sequence
	digitalWrite(MOTOR_WHITE_EN, HIGH); //disable white motor
	digitalWrite(MOTOR_BLACK_EN, HIGH); //disable black motor
break;
}
}

// ****************************************  MOTOR
void motor(byte direction, int speed, float distance) 
{ 
 int step_number = 0;  
// Set Step Number -> Calcul the distance

  if (distance == calibrate_speed) step_number = 1*microsteps ; // In order to move step by step 
else if (direction == AH_18 || direction == HA_81 || direction == AH_81 || direction == HA_18) step_number = distance * SQUARE_SIZE * SQRT2;  // diagonal move of 1 chess case
 else step_number = distance * SQUARE_SIZE; // move from chess case to another


  //  Direction of the motor rotation // V10 -> See the Excel

    if (direction == H_A|| direction == F1_F8 || direction == HA_18 ) {
    digitalWrite(MOTOR_WHITE_DIR, HIGH);
   
    }
    else {
      digitalWrite(MOTOR_WHITE_DIR, LOW);
       
    }
    if (direction == A_H|| direction == F1_F8 || direction == AH_18) {
      digitalWrite(MOTOR_BLACK_DIR, HIGH);
       
     }
    else {
      digitalWrite(MOTOR_BLACK_DIR, LOW);
        
     }

//  Active the motors 
  for (int x = 0; x < step_number; x++){
    if (direction == AH_18 || direction == HA_81 ){
      digitalWrite(MOTOR_WHITE_STEP, LOW); 
    }
    else {
      digitalWrite(MOTOR_WHITE_STEP, HIGH);   
    }
    if (direction == HA_18 || direction == AH_81 ) {
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
  
}

// ********** Move Trolley on the Current line
void moveTrolleyCurrentLine(){

for (int col = 0; col < 7; col++) {

    // set polarization for White
	  digitalWrite(IN1, LOW); // Change for invert Polarization
	  digitalWrite(IN2, HIGH);  // Change for invert Polarization
    electromagnet(true);
    delay(50);

    motor(A_H,SPEED_FAST,1);
    electromagnet(false);
    
  	//  Update the hall status
	  Serial.println("**** See Matrix Update ****");
    Read_Sensor (mux1);
    Read_Sensor (mux2);
    Read_Sensor (mux3);
    Read_Sensor (mux4);
    delay(50);
    See_Matrix();
    delay(1000);
  }
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
    Serial.print(8-i); // Matrix on C++
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
      if (hall_sensor_status[i][j] == 1){
        Serial.print(Matrix_Example [i][j]);
      } else if (hall_sensor_status[i][j] == -1){
        Serial.print("-");
        Serial.print(Matrix_Example [i][j]);
      } else Serial.print(hall_sensor_status[i][j]);
	  
      if (hall_sensor_status[i][j] == 1 || hall_sensor_status[i][j] == -1) {
        lc.setLed(0, i, j, true); // lcd matrix column not folow the same logic 
      }
      Serial.print(" ");
    }
    Serial.println('|');
  }
  Serial.println("+ - - - - - - - - -+");
  Serial.println(" H G F E D C B A");
}

// *******************************  ELECTROMAGNET H-Bridge 
void electromagnet(bool electroMagnetState) // For this Test
{
	Serial.print("Electromagnet ");
	Serial.println(electroMagnetState ? "ON" : "OFF");
	lcd.clear();
	lcd.print(electroMagnetState ? "Emagnet ON" : "Emagnet OFF");
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


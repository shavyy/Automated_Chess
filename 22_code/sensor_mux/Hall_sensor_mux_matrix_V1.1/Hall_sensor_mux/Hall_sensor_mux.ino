#include "HC4067.h"
#include <Wire.h>
#include "LedControl.h" // Added for Matrix Led based on MAX7219
#include <LiquidCrystal_I2C.h> // Old Library, replaced for a new one

LedControl lc=LedControl(51,52,53,1); // For A Mega DIN=51 , CLK=52, CS=53, 1 Matrix Led
LiquidCrystal_I2C lcd(0x27,16,2); // Old Library, replaced for a new one

const byte S0 = A5;
const byte S1 = A9;
const byte S2 = A11;
const byte S3 = A12;
const byte BUTTON_WHITE_SWITCH_MOTOR_WHITE (11);
const byte BUTTON_BLACK_SWITCH_MOTOR_BLACK (6);
enum {WHITE, BLACK};

HC4067 mux4(S0, S1, S2, S3, 40); // 
HC4067 mux3(S0, S1, S2, S3, 42); // 
HC4067 mux2(S0, S1, S2, S3, 44); // 
HC4067 mux1(S0, S1, S2, S3, 49); // 

int Mux_Out = A4;
int hallMeasure;
int hall_min = 450; // 486 measured on hall board with power supply 5.00V
int hall_max = 590; // 584 measured on hall board with power supply 5.00V

int hall_sensor_status [8][8];
int hall_sensor_record [8][8];
int hall_sensor_status_memory [8][8];
int hall_value[8][8];       //  For Debug only


int Matrix_Example [8][8]={
    { 1,  2,  3,  4,  5,  6,  7,  8},
    { 9, 10, 11, 12, 13, 14, 15, 16},
    {17, 18, 19, 20, 21, 22, 23, 24},
    {25, 26, 27, 28, 29, 30, 31, 32},
    {33, 34, 35, 36, 37, 38, 39, 40},
    {41, 42, 43, 44, 45, 46, 47, 48},
    {49, 50, 51, 52, 53, 54, 55, 56},
    {57, 58, 59, 60, 61, 62, 63, 64}
};

byte column = 0; // chessboard column
byte row = 0; // chessboard row

void setup() {
  // Initialisation de la matrice LED
  lc.shutdown(0, false);       // shutdown     : "true" active le mode "sommeil", tandis que "false" active le mode "normal"
  lc.setIntensity(0, 4);       // setIntensity : valeur pouvant aller de 0 à 15 (pour 16 niveaux de luminosité, donc)
  lc.clearDisplay(0);          // clearDisplay : éteint toutes les LEDs de la matrice

  Serial.begin(9600);
  Serial.println("Understand_Matrix_Mux_V1.0");

    // Initialize the hall sensor memory array to 0
     hall_sensor_status_memory[8][8] = 0;
     
    // Initialize the hall sensor array to 0
      hall_sensor_status[8][8] = 0;
      

   //  Multiplexer V9 
   pinMode (Mux_Out, INPUT);

  //  LCD
  lcd.init();
  lcd.backlight();

  //  Arcade button - Limit Switch
  pinMode (BUTTON_WHITE_SWITCH_MOTOR_WHITE, INPUT_PULLUP);
  pinMode (BUTTON_BLACK_SWITCH_MOTOR_BLACK, INPUT_PULLUP);

  // Show the Matrix Example


  Serial.println("+ -    -   -   -  -   -   -   -   -+");
  for (int i = 0; i < 8; i++) {
    Serial.print("row ");
    Serial.print(i); // Matrix on C++
    Serial.print("| ");
    for (int j = 0; j < 8; j++) {
      Serial.print(Matrix_Example[i][j]); // expected [row][column]

      Serial.print(" ");
    }
    Serial.println('|');
  }
  Serial.println("+ -    -   -   -  -   -   -   -   -+");
  Serial.println("col: 0   1   2   3   4   5   6   7");

  
	Serial.println("Press Any Button for run the Loop");
    lcd.setCursor(0, 0);
    lcd.print("PRESS ANY BUTTON");
    lcd.setCursor(0, 1);
    lcd.print("  FOR RUN LOOP  ");
    Serial.println(" ");

}

void loop() 
{
	if (digitalRead(BUTTON_WHITE_SWITCH_MOTOR_WHITE) != HIGH || digitalRead(BUTTON_BLACK_SWITCH_MOTOR_BLACK) != HIGH ){

		Serial.println("Will read and Show all the hall sensors");
		lcd.setCursor(0, 0);
		lcd.print("  READ THE HALL ");
		lcd.setCursor(0, 1);
		lcd.print("  SENSORS       ");
		Serial.println(" ");

		detect_human_movement();
    delay(50);
	}
}


void detect_human_movement() {

    // Perform regular readings
    Read_Sensor (mux1);
    Read_Sensor (mux2);
    Read_Sensor (mux3);
    Read_Sensor (mux4);
    delay(100);

  // Display the value
    hall_display();
    delay(300);
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

void read_hall_values(HC4067 &mux) {
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

void Record_hall_measure() {
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

void hall_display() {
  
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
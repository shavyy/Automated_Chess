#include <C:\Users\shavy\Desktop\Automated chessboard\22_code\definition_V1.h>
#include <C:\Users\shavy\Desktop\Automated chessboard\22_code\microMax_V1.cpp>

enum {WHITE, BLACK};

//  Electromagnet H Bridge L298s
bool electroMagnetState = false;

bool state = false;
enum testSequence {movePieceAuto,posZero,Line8,none};
testSequence mytestSequence=none;

const byte BUTTON_REVERSE_WHITE (3); 
const byte BUTTON_REVERSE_BLACK (2); 
const byte SEE_SERIAL (14);

int trolley_coordinate_Letter = 0+97; // 4=e, So X= Column (ASCII 97=a)
int trolley_coordinate_Number = 8+48; // 6=7, So Y= Row (ASCII 48=0)
int pin_buzzer = A3;

String incomingByte;

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

  pinMode(pin_buzzer, OUTPUT);
	
	//  Arcade button - Limit Switch
	pinMode (BUTTON_WHITE_SWITCH_MOTOR_WHITE, INPUT_PULLUP);
	pinMode (BUTTON_BLACK_SWITCH_MOTOR_BLACK, INPUT_PULLUP);
	pinMode (BUTTON_REVERSE_WHITE, INPUT_PULLUP);
	pinMode (BUTTON_REVERSE_BLACK, INPUT_PULLUP);
	pinMode (SEE_SERIAL, INPUT_PULLUP);
	pinMode (LIMIT_SWITCH_BLACK, INPUT_PULLUP);
	pinMode (LIMIT_SWITCH_WHITE, INPUT_PULLUP);

  //buzzer OFF
  digitalWrite(pin_buzzer, LOW); 
	
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

void loop() {

  if (digitalRead(BUTTON_BLACK_SWITCH_MOTOR_BLACK) != HIGH) {
    Serial.println("posZero enabled ");
    mytestSequence = posZero;
  }

	if (digitalRead(BUTTON_WHITE_SWITCH_MOTOR_WHITE) != HIGH) {
      if (Serial.available()>0) {
        // read the incoming byte:

        char chessPosConvert [3];

        incomingByte = Serial.readString();
        incomingByte.toCharArray(chessPosConvert, 3);

        lastM[2] = chessPosConvert[0]-'a';
        lastM[3] = -chessPosConvert[1]+56;

        Serial.print("FIRST, I received: ");
        Serial.println(incomingByte);
        Serial.print("Letter= ");
        Serial.println(chessPosConvert[0]);
        Serial.print("Number= ");
        Serial.println(chessPosConvert[1]);

        Serial.print("After conversion, I received: ");
        Serial.println(incomingByte);
        Serial.print("lastM[2]= ");
        Serial.println(lastM[2]-'a');
        Serial.print("lastM[3]= ");
        Serial.println(-lastM[3]+56);

        delay(1000);

        mytestSequence = movePieceAuto;
      } 
  }

switch (mytestSequence){
  case posZero:
    //enable motor
    digitalWrite(MOTOR_WHITE_EN, LOW); 
    digitalWrite(MOTOR_BLACK_EN, LOW);

    while (digitalRead(LIMIT_SWITCH_BLACK) == HIGH) motor(H_A, SPEED_SLOW, calibrate_speed); 
    while (digitalRead(LIMIT_SWITCH_WHITE) == HIGH) motor(F1_F8, SPEED_SLOW, calibrate_speed);
    Serial.println("**** Back in Position Zero ****");

    //enable motor
    digitalWrite(MOTOR_WHITE_EN, HIGH); 
    digitalWrite(MOTOR_BLACK_EN, HIGH);  
    mytestSequence = none;   
  break;

	case movePieceAuto:		
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("POSIT received");
    lcd.setCursor(0, 1);
    lcd.print("Piece move :");
    lcd.setCursor(13, 1);
    lcd.print(incomingByte);
    lcd.setCursor(15, 1);
    lcd.print("!");

		int departure_coord_Letter = lastM[0]+'a'; //Convert ASCIIs AI characters in variables 
		int departure_coord_Number = abs('8'-lastM[1]);  // Original -'8'-1 
		int arrival_coord_Letter = lastM[2]+'a';
		int arrival_coord_Number = abs('8'-lastM[3]);	// Original -'8'-1
		//  Trolley displacement to the starting position		
		int convert_table [] = {0, 1, 2, 3, 4, 5, 6, 7, 0};
		int displacement_X = 0;		
		int displacement_Y = 0;

    //Displacement Trolley
    displacement_X = abs(abs(departure_coord_Letter) - abs(trolley_coordinate_Letter));
    displacement_Y = abs(abs(departure_coord_Number) - abs(trolley_coordinate_Number));

    Serial.println("Trolley displacement to the starting position Case NO Eating");
    Serial.print("hall_sensor_status[convert_table[arrival_coord_Number]][arrival_coord_Letter]="); // V9.2.6
    Serial.println(hall_sensor_status[convert_table[arrival_coord_Number]][arrival_coord_Letter]); // V9.2.6

    Serial.print("trolley_coordinate_Letter=");     
    Serial.println(trolley_coordinate_Letter);   
    Serial.print("trolley_coordinate_Number=");     
    Serial.println(trolley_coordinate_Number);   

    Serial.print("departure_coord_Letter=");    
    Serial.println(departure_coord_Letter);     
    Serial.print("departure_coord_Number=");    
    Serial.println(departure_coord_Number);     

    Serial.print("arrival_coord_Letter=");      
    Serial.println(arrival_coord_Letter);       
    Serial.print("arrival_coord_Number=");      
    Serial.println(arrival_coord_Number);       

    Serial.print("displacement_X=");      
    Serial.println(displacement_X);       
    Serial.print("displacement_Y=");      
    Serial.println(displacement_Y);       

    if (departure_coord_Letter > trolley_coordinate_Letter) motor(A_H, SPEED_FAST, displacement_X);           
      else if (departure_coord_Letter < trolley_coordinate_Letter) motor(H_A, SPEED_FAST, displacement_X);    
      if (departure_coord_Number > trolley_coordinate_Number) motor(F1_F8, SPEED_FAST, displacement_Y);       
      else if (departure_coord_Number < trolley_coordinate_Number) motor(F8_F1, SPEED_FAST, displacement_Y);  
    //END Displacement Trolley

    //Displacement Piece
		trolley_coordinate_Letter = arrival_coord_Letter;
		trolley_coordinate_Number = arrival_coord_Number;

		//  Move the Black chess piece to the arrival position
		displacement_X = abs(abs(arrival_coord_Letter) - abs(departure_coord_Letter));
		displacement_Y = abs(abs(arrival_coord_Number) - abs(departure_coord_Number));
		digitalWrite(IN1, LOW);   // HIGH for Black piece polarizarion
		digitalWrite(IN2, HIGH);  // LOW for Black piece polarizarion
		
		Serial.println("Piece Black moving");

		Serial.print("trolley_coordinate_Letter=");     
		Serial.println(trolley_coordinate_Letter);    
		Serial.print("trolley_coordinate_Number=");      
		Serial.println(trolley_coordinate_Number);    

		Serial.print("departure_coord_Letter=");     
		Serial.println(departure_coord_Letter);      
		Serial.print("departure_coord_Number=");     
		Serial.println(departure_coord_Number);      

		Serial.print("arrival_coord_Letter=");       
		Serial.println(arrival_coord_Letter);        
		Serial.print("arrival_coord_Number=");       
		Serial.println(arrival_coord_Number);        


		Serial.print("hall_sensor_status[convert_table[arrival_coord_Number]][arrival_coord_Letter]=");  
		Serial.println(hall_sensor_status[convert_table[arrival_coord_Number]][arrival_coord_Letter]);  

		Serial.print("displacement_X=");       
		Serial.println(displacement_X);        
		Serial.print("displacement_Y=");       
		Serial.println(displacement_Y);

    lastM[0] = lastM[2];
    lastM[1] = lastM[3];

    //enable motor
    digitalWrite(MOTOR_WHITE_EN, LOW); 
    digitalWrite(MOTOR_BLACK_EN, LOW);
		electromagnet(true);
		//  Horse displacement // X= F1_F8  Y= A_H
		if (displacement_X == 1 && displacement_Y == 2 || displacement_X == 2 && displacement_Y == 1) {
      if (displacement_Y == 2) {
        if (departure_coord_Letter < arrival_coord_Letter) {
        motor(A_H, SPEED_SLOW, 0.5);
        if (departure_coord_Number < arrival_coord_Number) motor(F1_F8, SPEED_SLOW, 2);
        else motor(F8_F1, SPEED_SLOW, 2);
        motor(A_H, SPEED_SLOW, 0.5);
        }
        else if (departure_coord_Letter > arrival_coord_Letter) {
        motor(H_A, SPEED_SLOW, 0.5);
        if (departure_coord_Number < arrival_coord_Number) motor(F1_F8, SPEED_SLOW, 2);
        else motor(F8_F1, SPEED_SLOW, 2);
        motor(H_A, SPEED_SLOW, 0.5);
        }
      }
      else if (displacement_X == 2) {
        if (departure_coord_Number < arrival_coord_Number) {
        motor(F1_F8, SPEED_SLOW, 0.5);
        if (departure_coord_Letter < arrival_coord_Letter) motor(A_H, SPEED_SLOW, 2);
        else motor(H_A, SPEED_SLOW, 2);
        motor(F1_F8, SPEED_SLOW, 0.5);
        }
        else if (departure_coord_Number > arrival_coord_Number) {
        motor(F8_F1, SPEED_SLOW, 0.5);
        if (departure_coord_Letter < arrival_coord_Letter) motor(A_H, SPEED_SLOW, 2);
        else motor(H_A, SPEED_SLOW, 2);
        motor(F8_F1, SPEED_SLOW, 0.5);
        }
      }
		}
		//  Diagonal displacement - Bishop - Queen
		else if (displacement_X == displacement_Y) {
		if (departure_coord_Letter > arrival_coord_Letter && departure_coord_Number > arrival_coord_Number) motor(HA_81, SPEED_SLOW, displacement_X);
		else if (departure_coord_Letter > arrival_coord_Letter && departure_coord_Number < arrival_coord_Number) motor(HA_18, SPEED_SLOW, displacement_X);
		else if (departure_coord_Letter < arrival_coord_Letter && departure_coord_Number > arrival_coord_Number) motor(AH_81, SPEED_SLOW, displacement_X);
		else if (departure_coord_Letter < arrival_coord_Letter && departure_coord_Number < arrival_coord_Number) motor(AH_18, SPEED_SLOW, displacement_X);
		}
		//  Horizontal displacement - ROOK - KING
		else if (displacement_Y == 0) {
		if (departure_coord_Letter > arrival_coord_Letter) motor(H_A, SPEED_SLOW, displacement_X);
		else if (departure_coord_Letter < arrival_coord_Letter) motor(A_H, SPEED_SLOW, displacement_X);
		}
		//  Vertical displacement - ROOK - KING - PAWN
		else if (displacement_X == 0) {
		if (departure_coord_Number > arrival_coord_Number) motor(F8_F1, SPEED_SLOW, displacement_Y);
		else if (departure_coord_Number < arrival_coord_Number) motor(F1_F8, SPEED_SLOW, displacement_Y);
		} else {
      Serial.println("!!!!!!!!!!!!!!! ILLEGAL MOVE !!!!!!!!!!!!!!! ");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("!!ILLEGAL MOVE!!");
      lcd.setCursor(0, 1);
      lcd.print("Play other move!");
      tone(pin_buzzer, 1000, 2000); // Faire sonner le buzzer avec une fréquence de 10Hz (son grave), pendant une durée de 1915 millisecondes
      
      lastM[0] = departure_coord_Letter-'a';
      lastM[1] = '8'-departure_coord_Number ;
      trolley_coordinate_Letter = departure_coord_Letter;
      trolley_coordinate_Number = departure_coord_Number;
    }   
    //END Displacement Piece
		//Fix_Distance();// extra last black movement, for center the piece on the square
		electromagnet(false);

		//  Upadte the hall sensors states with the Balck move
    Read_Sensor (mux1);
    Read_Sensor (mux2);
    Read_Sensor (mux3);
    Read_Sensor (mux4);
    delay(50);
    See_Matrix();
    delay(50);

    Serial.print("lastM[0]=");     
    Serial.println(lastM[0]+'a');   
    Serial.print("lastM[1]=");     
    Serial.println('8'-lastM[1]);
    Serial.print("lastM[2]=");     
    Serial.println(lastM[2]+'a');   
    Serial.print("lastM[3]=");     
    Serial.println('8'-lastM[3]);

	  digitalWrite(MOTOR_WHITE_EN, HIGH); //disable white motor
	  digitalWrite(MOTOR_BLACK_EN, HIGH); //disable black motor
    mytestSequence = none; //  stop the sequence
	break;
  }
}

// ****************************************  MOTOR
void motor(byte direction, int speed, float distance) 
{ 
 int step_number = 0;  
// Set Step Number -> Calcul the distance

  if (distance == calibrate_speed) step_number = 1*microsteps ; // In order to move step by step 
else if (direction == AH_18 || direction == HA_81 || direction == AH_81 || direction == HA_18) step_number = SQRT2 * distance * SQUARE_SIZE;  // diagonal move of 1 chess case
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
    delay(5); // delay between read
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
	//lcd.clear();
	//lcd.print(electroMagnetState ? "Emagnet ON" : "Emagnet OFF");
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


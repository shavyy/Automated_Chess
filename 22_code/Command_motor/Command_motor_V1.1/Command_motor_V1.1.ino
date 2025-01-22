#include <Wire.h>
// LCD
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
String MenuItems[]={"> Motor_A+_B+ <","  Motor_A-_B+  ","  Motor_A+_B+  ","> Motor_A-_B+ <","> Motor_A+_B- <","  Motor_A-_B-  ","  Motor_A+_B-  ","> Motor_A-_B- <"};
byte MenuItemSelected=0;

// Motor
int microsteps=8;
const int MOTOR_WHITE_STEP = 36;
const int MOTOR_WHITE_DIR = 34;
const int MOTOR_WHITE_EN = 30;

const int MOTOR_BLACK_STEP = 26;
const int MOTOR_BLACK_DIR = 28;
const int MOTOR_BLACK_EN = 24;

const int stepsPerRev=200;
const int SQUARE_SIZE = 185*microsteps;  // Put the Steps for 1 Square. Test changing that nubmer if Trolley go more/less than 1 square
const int SPEED_SLOW (1000); //3500/microsteps but check it is not Less than 150. 
const int SPEED_FAST (500); // 2000/microsteps but check it is not Less than 100
enum {T_B, B_T, L_R, R_L, LR_BT, RL_TB, LR_TB, RL_BT}; // T=Top  -  B=Bottom  -  L=Left  -  R=Right
const byte SQUARES = 1;
//  Button - Switch
const byte BUTTON_WHITE_SWITCH_MOTOR_WHITE (11);
const byte BUTTON_BLACK_SWITCH_MOTOR_BLACK (6);
enum {WHITE, BLACK};

//  Game setting
enum {Stand_by,Motor_AP_BP,Motor_AN_BP,Motor_AP_BN,Motor_AN_BN};
byte sequence_Loop = Stand_by;

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

  pinMode(MOTOR_WHITE_DIR, OUTPUT);
  pinMode(MOTOR_WHITE_STEP, OUTPUT);
  pinMode(MOTOR_WHITE_EN, OUTPUT);
  
  pinMode(MOTOR_BLACK_DIR, OUTPUT);
  pinMode(MOTOR_BLACK_STEP, OUTPUT);
  pinMode(MOTOR_BLACK_EN, OUTPUT);

  digitalWrite(MOTOR_WHITE_EN, LOW);
  digitalWrite(MOTOR_BLACK_EN, LOW);

  pinMode (BUTTON_WHITE_SWITCH_MOTOR_WHITE, INPUT_PULLUP);
  pinMode (BUTTON_BLACK_SWITCH_MOTOR_BLACK, INPUT_PULLUP);
 
  Serial.println(F("8825 Initialized"));
}

void loop() {
  switch (sequence_Loop) // Stand_by,Motor_AP_BP,Motor_AN_BP,Motor_AP_BN,Motor_AN_BN
  {
  case Stand_by:     // Show choice on LCD
    lcd_display();
     if (button(WHITE) == true) 
        {                                       // White=Down Button , Black Enter Button.
          MenuItemSelected=MenuItemSelected+2;
          if (MenuItemSelected>7) 
            {
            MenuItemSelected=0;
            }
  	     lcd_display();
        }
      else if (button(BLACK) == true) 
        {                                       // Button Black is Enter
        // Display the text 
        String lcdText = MenuItems[MenuItemSelected];
        if (lcdText == "> Motor_A+_B+ <") 
        {sequence_Loop = Motor_AP_BP;
        Serial.println("Motor_AP_BP");
        }
        else if (lcdText == "  Motor_A+_B+  ")
        { sequence_Loop = Motor_AN_BP;
        Serial.println("Motor_AN_BP");
        }
        else if (lcdText == "> Motor_A+_B- <")
        { sequence_Loop = Motor_AP_BN;
        Serial.println("Motor_AP_BN");
        }
        else if (lcdText == "  Motor_A+_B-  ")
        { sequence_Loop = Motor_AN_BN;
        Serial.println("Motor_AN_BN");
        }
        }
  break;

  case Motor_AP_BP:  // Motor A turns clockwise direction and Motor B turns clockwise direction at slow speed X steps Motor y returns at fast speed X, then back to standby mode
  lcd_display();      // motor(byte direction, int speed, float distance) 
  motor(B_T, SPEED_SLOW, SQUARES);  // Motor A 
  motor(L_R, SPEED_SLOW, SQUARES);  // Motor B
  delay(500);
  Serial.println("Fast Return");
  motor(R_L, SPEED_FAST, SQUARES);  // Motor A 
  motor(T_B, SPEED_FAST, SQUARES);  // Motor B
  delay(500);
  sequence_Loop=Stand_by;
  break;

  case Motor_AN_BP:   // Motor A turns Anticlockwise direction and Motor B turns clockwise direction at slow speed X steps Motor y returns at fast speed X, then back to standby mode
  lcd_display();      // motor(byte direction, int speed, float distance) 
  motor(T_B, SPEED_SLOW, SQUARES);  // Motor A 
  motor(L_R, SPEED_SLOW, SQUARES);  // Motor B
  delay(500);
  Serial.println("Fast Return");
  motor(R_L, SPEED_FAST, SQUARES);  // Motor A 
  motor(B_T, SPEED_FAST, SQUARES);  // Motor B
  delay(500);
  sequence_Loop=Stand_by;
  break;

  case Motor_AP_BN:   // Motor A turns clockwise direction and Motor B turns anticlockwise direction at slow speed X steps Motor y returns at fast speed X, then back to standby mode
  lcd_display();      // motor(byte direction, int speed, float distance) 
  motor(B_T, SPEED_SLOW, SQUARES);  // Motor A 
  motor(R_L, SPEED_SLOW, SQUARES);  // Motor B
  delay(500);
  Serial.println("Fast Return");
  motor(L_R, SPEED_FAST, SQUARES);  // Motor A 
  motor(T_B, SPEED_FAST, SQUARES);  // Motor B
  delay(500);
  sequence_Loop=Stand_by;
  break;

  case Motor_AN_BN:   // Motor A turns anticlockwise direction and Motor B turns anticlockwise direction at slow speed X steps Motor y returns at fast speed X, then back to standby mode
  lcd_display();      // motor(byte direction, int speed, float distance) 
  motor(T_B, SPEED_SLOW, SQUARES);  // Motor A 
  motor(R_L, SPEED_SLOW, SQUARES);  // Motor B
  delay(500);
  Serial.println("Fast Return");
  motor(L_R, SPEED_FAST, SQUARES);  // Motor A 
  motor(B_T, SPEED_FAST, SQUARES);  // Motor B
  delay(500);
  sequence_Loop=Stand_by;
  break;

  } // Close Switch

}

boolean button(byte type) 
{

  if (type == WHITE && digitalRead(BUTTON_WHITE_SWITCH_MOTOR_WHITE) != HIGH) {
    delay(250);
    return true;
  }
  if (type == BLACK && digitalRead(BUTTON_BLACK_SWITCH_MOTOR_BLACK) != HIGH) {
    delay(250);
    return true;
  }
  return false;
}

// **********************************  LCD DISPLAY
void lcd_display() 
{
 switch (sequence_Loop)  // Stand_by,Motor_AP_BP,Motor_AN_BP,Motor_AP_BN,Motor_AN_BN
  {             
  case Stand_by:     
    lcd.setCursor(0, 0);
    lcd.print(MenuItems[MenuItemSelected]); 
	  lcd.setCursor(0, 1);
    lcd.print(MenuItems[MenuItemSelected+1]); 
	  return;
  case Motor_AP_BP:
    lcd.setCursor(0, 0);
    lcd.print("    Motor A  +  ");
    lcd.setCursor(0, 1);
    lcd.print("    Motor B  +  ");
    return;
  case Motor_AN_BP:
    lcd.setCursor(0, 0);
    lcd.print("    Motor A  -  ");
    lcd.setCursor(0, 1);
    lcd.print("    Motor B  +  ");
    return;
  case Motor_AP_BN:
    lcd.setCursor(0, 0);
    lcd.print("    Motor A  +  ");
    lcd.setCursor(0, 1);
    lcd.print("    Motor B  -  ");
    return;
  case Motor_AN_BN:
    lcd.setCursor(0, 0);
    lcd.print("    Motor A  -  ");
    lcd.setCursor(0, 1);
    lcd.print("    Motor B  -  ");
    return;  
  }                                     // close wsitch
}                                      // close lcd_display 

void motor(byte direction, int speed, float distance) 
{

  float step_number = 0;

  step_number = distance * SQUARE_SIZE;

  //  Direction of the motor rotation
  if (direction == R_L || direction == T_B || direction == RL_TB) digitalWrite(MOTOR_WHITE_DIR, HIGH);
  else digitalWrite(MOTOR_WHITE_DIR, LOW);
  if (direction == B_T || direction == R_L || direction == RL_BT) digitalWrite(MOTOR_BLACK_DIR, HIGH);
  else digitalWrite(MOTOR_BLACK_DIR, LOW);

  //  Active the motors
  for (int x = 0; x < step_number; x++) 
  {
    if (direction == LR_TB || direction == RL_BT) digitalWrite(MOTOR_WHITE_STEP, LOW);
    else digitalWrite(MOTOR_WHITE_STEP, HIGH);
    if (direction == LR_BT || direction == RL_TB) digitalWrite(MOTOR_BLACK_STEP, LOW);
    else digitalWrite(MOTOR_BLACK_STEP, HIGH);
    delayMicroseconds(speed);
    digitalWrite(MOTOR_WHITE_STEP, LOW);   
    digitalWrite(MOTOR_BLACK_STEP, LOW);
    delayMicroseconds(speed);
  }
}

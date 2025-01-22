// Test_LCD_V1.0
// lcd 16X2 Hardware with I2C
// Just for check the LCD works as expected

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2); // Check the datasheet of your LCD. "0x27" works for me, but maybe not for you

const byte BUTTON_WHITE_SWITCH_MOTOR_WHITE (11);
const byte BUTTON_BLACK_SWITCH_MOTOR_BLACK (6);
int state  = 0;

void setup () {
Serial.begin(9600);
//  LCD
  lcd.init();
  lcd.backlight();

  //  Arcade button - Limit Switch
  pinMode (BUTTON_WHITE_SWITCH_MOTOR_WHITE, INPUT_PULLUP);
  pinMode (BUTTON_BLACK_SWITCH_MOTOR_BLACK, INPUT_PULLUP);

  Serial.println("This 2 Lines must appear on LCD: ");
  Serial.println(" HELLO AUTOMATED ");
  Serial.println("   CHESSBOARD   ");
}

void loop(){	
	lcd.setCursor(0, 0);
  if (digitalRead(BUTTON_WHITE_SWITCH_MOTOR_WHITE) == LOW){
    lcd.print("HELLO AUTOMATED ");
    lcd.setCursor(0, 1);
    lcd.print("   White pressed   ");
  } 
    if (digitalRead(BUTTON_BLACK_SWITCH_MOTOR_BLACK) == LOW){
    lcd.print("HELLO AUTOMATED ");
    lcd.setCursor(0, 1);
    lcd.print("   Black pressed   ");
  } 

}

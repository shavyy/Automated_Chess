#include <Wire.h>
#include <LiquidCrystal_I2C.h> // Old Library, replaced for a new one
LiquidCrystal_I2C lcd(0x27,16,2); // Old Library, replaced for a new one

// variable declarations for time
// Change them to whatever number of hours, minutes, and seconds you wish.
int whiteHours    = 0;
int whiteMinutes  = 1;
int whiteSeconds  = 0;
int blackHours   = 0;
int blackMinutes = 1;
int blackSeconds = 0;

bool chessTimerState = false;

enum {start_up, start, calibration, player_Time};
byte sequence = start_up;

byte game_mode = 0;
enum {HvsH, HvsC};

// other variables for timekeeping
unsigned long lastTimeUpdate = 0; // micros() value when the clock was last updated
int whiteClockStep = -1;  // 0 for paused; 1 for counting up; -1 for counting down
int blackClockStep = -1; // 0 for paused; 1 for counting up; -1 for counting down

char buf[20]; // buffer for output

const byte BUTTON_WHITE_SWITCH_MOTOR_WHITE (11);
const byte BUTTON_BLACK_SWITCH_MOTOR_BLACK (6);
const byte BUTTON_REVERSE_WHITE (3); 
const byte BUTTON_REVERSE_BLACK (2); 
const byte SEE_SERIAL (14);

void setup() {
  Serial.begin(9600); // set baud rate for Serial
  pinMode (BUTTON_WHITE_SWITCH_MOTOR_WHITE, INPUT_PULLUP);
	pinMode (BUTTON_BLACK_SWITCH_MOTOR_BLACK, INPUT_PULLUP);
  pinMode (SEE_SERIAL, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();
  lcd_display();
  delay(2000);
  lcd.clear();

  game_mode=HvsH;
}

void loop() {

    if (digitalRead(BUTTON_WHITE_SWITCH_MOTOR_WHITE) != HIGH){
        chessTimerState = true;
    }
    if (digitalRead(BUTTON_BLACK_SWITCH_MOTOR_BLACK) != HIGH){
        chessTimerState = false;
    }
  
  // This next part is what makes our clock keep time.
  // (We only update the clock once per second.)
  if ((micros() - lastTimeUpdate) >= 1000000UL) {

    // We add time to, or subtract time from, each clock as necessary.
    if (chessTimerState == false){
        whiteSeconds  += whiteClockStep;
    } else if (chessTimerState == true){
        blackSeconds += blackClockStep;
    }

    countdown();
  
    // And we schedule the next update for one second later.
    lastTimeUpdate += 1000000UL;  
  }
  
  // A clock isn't much use if you have no way of seeing the time.
  // So...
  // This next line composes a string showing the time remaining for each player:
  sprintf(buf,"%d:%02d%02d  %d:%02d:%02d",whiteHours,whiteMinutes,whiteSeconds,blackHours,blackMinutes,blackSeconds);
  // (That string should look something like this: "0:00:00  0:00:00")
  // And this next line writes that string to the Serial monitor:
  //Serial.println(buf);
  
  sequence = player_Time;
  lcd_display();
  
  // You can also write that string to an LCD display if you wish.
  // But, I'm not sure how you have your display wired up,
  // so I'll skip that part for now.  
}

void lcd_display() { 

  /*if (no_valid_move == true) {
    lcd.setCursor(0, 0);
    lcd.print("  NO VALID MOVE  ");
    lcd.setCursor(0, 1);
    lcd.print("                ");
    delay(500);  // V10.2
    Set_The_Board();         
    no_valid_move = false;
    return;
  }*/

  switch (sequence) {
    case start_up:
      lcd.setCursor(0, 0);
      lcd.print("V1.0 AUTOMATIC"); 
      lcd.setCursor(0, 1);
      lcd.print("   CHESSBOARD   ");
      sequence = start;
      delay(800); 
    case start:
      lcd.setCursor(0, 0);
      lcd.print(" PRESS W - HvsH ");  
      lcd.setCursor(0, 1);
      lcd.print(" PRESS B - HvsC ");
      break;
    case calibration:
      lcd.setCursor(0, 0);
      lcd.print("  CALIBRATION   ");
      lcd.setCursor(0, 1);
      lcd.print("                ");
      break;
    case player_Time:
      lcd.setCursor(0, 0);
      lcd.print(" WHITE    BLACK");
      lcd.setCursor(0, 1);
      lcd.print("" + String(whiteHours) + ":" + String(whiteMinutes) + ":" + String(whiteSeconds) + "  " + String(blackHours) + ":" + String(blackMinutes) + ":" + String(blackSeconds));
      break;
  }
}

// ***********************************  COUNTDONW
void countdown() {

  // We do any necessary arithmetic for the minutes and hours.
    if (whiteSeconds < 0) {
      lcd.clear();
      whiteSeconds += 60;
      whiteMinutes -= 1;
    }
    if (whiteSeconds >= 60) {
      lcd.clear();
      whiteSeconds -= 60;
      whiteMinutes += 1;
    }    
    if (whiteMinutes < 0) {
      lcd.clear();
      whiteMinutes += 60;
      whiteHours -= 1;
    }
    if (whiteMinutes >= 60) {
      lcd.clear();
      whiteMinutes -= 60;
      whiteHours += 1;
    }
    if (whiteHours < 0) {
      lcd.clear();
      // if it goes below zero, then just set it to zero
      whiteHours = 0;
      whiteMinutes = 0;
      whiteSeconds = 0;
    }
    if (blackSeconds < 0) {
      lcd.clear();
      blackSeconds += 60;
      blackMinutes -= 1;
    }
    if (blackSeconds >= 60) {
      lcd.clear();
      blackSeconds -= 60;
      blackMinutes += 1;
    }    
    if (blackMinutes < 0) {
      lcd.clear();
      blackMinutes += 60;
      blackHours -= 1;
    }
    if (blackMinutes >= 60) {
      lcd.clear();
      blackMinutes -= 60;
      blackHours += 1;
    }
    if (blackHours < 0) {
      lcd.clear();
      // if it goes below zero, then just set it to zero
      blackHours = 0;
      blackMinutes = 0;
      blackSeconds = 0;
    }


// Verify if Times up. For GameOver()
if (game_mode==HvsH){
  if(sequence== player_Time){
    if(whiteMinutes==0 && whiteSeconds==0){
    Serial.println("White get out of time. Black wins!"); 
    lcd.setCursor(0, 0);
    lcd.print(" PLAYER BLACK   ");
    lcd.setCursor(0, 1);
    lcd.print("      WINS      ");
    for (;;); 
    } else if (blackMinutes==0 && blackSeconds==0){
      Serial.println("Black get out of time. White wins!"); 
      lcd.setCursor(0, 0);
      lcd.print(" PLAYER White   ");
      lcd.setCursor(0, 1);
      lcd.print("      WINS      ");
      for (;;); 
    }
  }
}
else if(game_mode== HvsC){
  if(sequence== player_Time){
    if(whiteMinutes<0){
    Serial.println("White get out of time. Black wins!"); 
    lcd.setCursor(0, 0);
    lcd.print(" PLAYER BLACK   ");
    lcd.setCursor(0, 1);
    lcd.print("      WINS      ");
    for (;;); 
    } else if (blackMinutes<0){
      Serial.println("Black get out of time. White wins!"); 
      lcd.setCursor(0, 0);
      lcd.print(" PLAYER White   ");
      lcd.setCursor(0, 1);
      lcd.print("      WINS      ");
      for (;;); 
    }
  }
}
}
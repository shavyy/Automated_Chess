const int MOTOR_WHITE_DIR = 28;
const int MOTOR_WHITE_STEP = 26;
const int MOTOR_WHITE_EN = 24;

const int MOTOR_BLACK_STEP = 36;
const int MOTOR_BLACK_DIR = 34;
const int MOTOR_BLACK_EN = 30;

const byte BUTTON_WHITE_SWITCH_MOTOR_WHITE (11);
const byte BUTTON_BLACK_SWITCH_MOTOR_BLACK (6);

const int stepsPerRev=200;
int millisBtwnSteps = 10;
int microsteps=32; //TBC

void setup() {
  Serial.begin(9600);
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
  if (digitalRead(BUTTON_WHITE_SWITCH_MOTOR_WHITE) == LOW){
      Serial.println(F("White Running clockwise"));
  digitalWrite(MOTOR_WHITE_DIR, HIGH); // Enables the motor to move in a particular direction
  // Makes 200 pulses for making one full cycle rotation
  for (int i = 0; i < stepsPerRev; i++) {
    digitalWrite(MOTOR_WHITE_STEP, HIGH);
    delay(millisBtwnSteps);
    digitalWrite(MOTOR_WHITE_STEP, LOW);
    delay(millisBtwnSteps);
  }
  delay(1000); // One second delay

  Serial.println(F("White Running counter-clockwise"));
  digitalWrite(MOTOR_WHITE_DIR, LOW); //Changes the rotations direction
  // Makes 400 pulses for making two full cycle rotation
  for (int i = 0; i < 2*stepsPerRev; i++) {
    digitalWrite(MOTOR_WHITE_STEP, HIGH);
    delay(millisBtwnSteps);
    digitalWrite(MOTOR_WHITE_STEP, LOW);
    delay(millisBtwnSteps);
  }
  delay(1000);
  }

    if (digitalRead(BUTTON_BLACK_SWITCH_MOTOR_BLACK) == LOW){
      Serial.println(F("Black Running clockwise"));
  digitalWrite(MOTOR_BLACK_DIR, HIGH);
  // Makes 200 pulses for making one full cycle rotation
  for (int i = 0; i < stepsPerRev; i++) {
	digitalWrite(MOTOR_BLACK_STEP, HIGH);
    delay(millisBtwnSteps);
	digitalWrite(MOTOR_BLACK_STEP, LOW);
    delay(millisBtwnSteps);
  }
  delay(1000); // One second delay

  Serial.println(F("Black Running counter-clockwise"));
  digitalWrite(MOTOR_BLACK_DIR, LOW);
  // Makes 400 pulses for making two full cycle rotation
  for (int i = 0; i < 2*stepsPerRev; i++) {
	digitalWrite(MOTOR_BLACK_STEP, HIGH);
    delay(millisBtwnSteps);
	digitalWrite(MOTOR_BLACK_STEP, LOW);
    delay(millisBtwnSteps);
  }
  delay(1000);
  }

}

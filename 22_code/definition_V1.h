#include <Wire.h>
#include "HC4067.h"
#include "LedControl.h" // Added for Matrix Led based on MAX7219
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

//MUX
const byte S0 = A5;
const byte S1 = A9;
const byte S2 = A11;
const byte S3 = A12;
HC4067 mux4(S0, S1, S2, S3, 40); // 
HC4067 mux3(S0, S1, S2, S3, 42); // 
HC4067 mux2(S0, S1, S2, S3, 44); // 
HC4067 mux1(S0, S1, S2, S3, 49); // 

int Mux_Out = A4;
int hallMeasure;
int hall_min = 480; // 486 measured on hall board with power supply 5.00V
int hall_max = 650; // 584 measured on hall board with power supply 5.00V

int hall_sensor_status [8][8];
int hall_sensor_record [8][8];
int hall_sensor_status_memory [8][8];
int hall_value[8][8];

int Matrix_Example2 [8][8]={
    { 1,  2,  3,  4,  5,  6,  7,  8},
    { 9, 10, 11, 12, 13, 14, 15, 16},
    {17, 18, 19, 20, 21, 22, 23, 24},
    {25, 26, 27, 28, 29, 30, 31, 32},
    {33, 34, 35, 36, 37, 38, 39, 40},
    {41, 42, 43, 44, 45, 46, 47, 48},
    {49, 50, 51, 52, 53, 54, 55, 56},
    {57, 58, 59, 60, 61, 62, 63, 64}
};

char* Matrix_Example [8][8]={
      "A8",  "B8",  "C8",  "D8",  "E8",  "F8",  "G8", "H8",
      "A7",  "B7",  "C7",  "D7",  "E7",  "F7",  "G7", "H7",
      "A6",  "B6",  "C6",  "D6",  "E6",  "F6",  "G6", "H6",
      "A5",  "B5",  "C5",  "D5",  "E5",  "F5",  "G5", "H5",
      "A4",  "B4",  "C4",  "D4",  "E4",  "F4",  "G4", "H4",
      "A3",  "B3",  "C3",  "D3",  "E3",  "F3",  "G3", "H3",
      "A2",  "B2",  "C2",  "D2",  "E2",  "F2",  "G2", "H2",
      "A1",  "B1",  "C1",  "D1",  "E1",  "F1",  "G1", "H1"};

byte column = 0; // chessboard column
byte row = 0; // chessboard row

//Matrix LED
LedControl lc=LedControl(51,52,53,1); // DIN=51 , CLK=52, CS=53, 1 Matrix Led

//  Electromagnet H Bridge L298
int IN1 = (15);
int IN2 = (18);
int MAGNET (19);  // Enable or not the H-Bridge

//  Button - Switch
const byte BUTTON_WHITE_SWITCH_MOTOR_WHITE (11);
const byte BUTTON_BLACK_SWITCH_MOTOR_BLACK (6);
const byte LIMIT_SWITCH_BLACK (4); 
const byte LIMIT_SWITCH_WHITE (5);

// Motor
const int MOTOR_WHITE_DIR = 28;
const int MOTOR_WHITE_STEP = 26;
const int MOTOR_WHITE_EN = 24;

const int MOTOR_BLACK_STEP = 36;
const int MOTOR_BLACK_DIR = 34;
const int MOTOR_BLACK_EN = 30;

const int stepsPerRev=200;
const int microsteps=8;
const int SQUARE_SIZE = 189*microsteps;  // Put the Steps for 1 Square. Test changing that nubmer if Trolley go more/less than 1 square
const int SPEED_SLOW (800); //3500/microsteps but check it is not Less than 150. 
const int SPEED_FAST (500); // 2000/microsteps but check it is not Less than 100
const float SQUARE_SIZE_MM = 37.0; // measure of 1 Square 37mm
const float SQRT2=2; //1.41421 theoritical calculated 

enum {A_H,H_A,F1_F8,F8_F1,AH_18, HA_81, AH_81, HA_18, calibrate_speed};	
const char* directionNames[] = {"A_H", "H_A", "F1_F8", "F8_F1", "AH_18", "HA_81", "AH_81", "HA_18", "calibrate_speed"}; 
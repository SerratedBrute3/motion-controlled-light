#include "Timer.h"

int a = 2;
int b = 3;
int c = 4;
int d = 5;
int e = 6;
int f = 7;
int g = 8;
int dp = 9;
int sw = 10;

int LEDS[7] {a, b, c, d, e, f, g};
int nums[11][7] {
  {1, 1, 1, 1, 1, 1, 0}, //0
  {0, 1, 1, 0, 0, 0, 0}, //1
  {1, 1, 0, 1, 1, 0, 1}, //2
  {1, 1, 1, 1, 0, 0, 1}, //3
  {0, 1, 1, 0, 0, 1, 1}, //4
  {1, 0, 1, 1, 0, 1, 1}, //5
  {1, 0, 1, 1, 1, 1, 1}, //6
  {1, 1, 1, 0, 0, 0, 0}, //7
  {1, 1, 1, 1, 1, 1, 1}, //8
  {1, 1, 1, 1, 0, 1, 1}, //9
  {0, 0, 0, 0, 0, 0, 0}, //off
};



void outNum(int x) { //outputs number x on the display
  for (int i = 0; i < 7; i++) {
    if (nums[x][i] == 1) {
      digitalWrite(LEDS[i], HIGH);
    }
    else {
      digitalWrite(LEDS[i], LOW);
    }
  }
}

void LEDOFF(int x) { //outputs number x on the display
  for (int i = 0; i < 7; i++) {
    if (nums[x][i] == 1) {
      digitalWrite(LEDS[i], LOW);
    }
    else {
      digitalWrite(LEDS[i], LOW);
    }
  }
}

int readStick() { //returns 1 if the joystick was up, 2 if it is down, 0 for anything else
  // you may have to read from A0 instead of A1 depending on how you orient the joystick
  if (analogRead(A1) > 800) {
    return 2; // down
  }
  else if (analogRead(A1) < 200) {
    return 1; // up
  }
  else {
    return 0;
  }

}
unsigned int white_pin = 13;
unsigned int green_pin = 12;
unsigned char cflag = 0; // count flag
unsigned int count = 9;  // display count
unsigned long tcnt = 0; // tick count
unsigned int speed[3] = {4,2,1}; // array of speeds
unsigned int val = 1; // value to determine speed
unsigned int otherCount = 5;

// write state machine here

enum DM_States {DM_Init, DM_On, DM_Off, DM_Button1, DM_Button2} DM_State;
// enum BL_States {BL_Init, BL_On} BL_State;
enum CS_States {CS_Init, CS_Nine, CS_Five, CS_Reset, CS_Hold} CS_State;

void DetectMotion(){
  switch(DM_State){
    case DM_Init:
      if(analogRead(A1) > 200 && analogRead(A1) < 800 && digitalRead(sw) == 1){
        tcnt++;
        DM_State = DM_Init;
      }
      else if (analogRead(A1) < 200 || analogRead(A1) > 800){
        DM_State = DM_On;        
      }
      else if (digitalRead(sw) == 0){
        DM_State = DM_Button1;
      }
      break;
    
    case DM_On:
      if(analogRead(A1) < 200 || analogRead(A1) > 800){
        DM_State = DM_On;
      }
      if(count == -1){
        DM_State = DM_Init;
      }
      if(digitalRead(sw) == 0 && count != -1){        
        DM_State = DM_Button2;
      }
    break;

    case DM_Button1:
      if(digitalRead(sw) == 1){
        DM_State = DM_On;
      }
    break;

    case DM_Button2:
        
      if (otherCount == -1){
        DM_State = DM_Init;
      }
      if (digitalRead(sw) == 0){
        DM_State = DM_On;
      }
    break;
  }

  switch(DM_State){
    case DM_Init:
      digitalWrite(white_pin, LOW);
      digitalWrite(green_pin, LOW);
      if(tcnt >= 10){
        tcnt = 0;
        if (tcnt == 0){
          digitalWrite(green_pin, HIGH);
        }        
      }
    break;

    case DM_On:
      digitalWrite(white_pin, HIGH);
      digitalWrite(green_pin, LOW);
    break;
    
    case DM_Button1:
      digitalWrite(white_pin, HIGH);
      digitalWrite(green_pin, LOW);
    break;

    case DM_Button2:
      digitalWrite(white_pin, LOW);
      digitalWrite(green_pin, LOW);
    break;
  }
}

void Counter(){
  switch(CS_State){
    case CS_Init:
      if(analogRead(A1) < 200 || analogRead(A1) > 800){
        CS_State = CS_Nine;
      }
      if(digitalRead(sw) == 0){
        CS_State = CS_Hold;
      }
    break;

    case CS_Nine:
      if(analogRead(A1) < 200 || analogRead(A1) > 800){
        count = 9;
        CS_State = CS_Nine;
      }
      if(digitalRead(sw) == 0 && count != -1){        
        otherCount = 5;
        while(digitalRead(sw) == 0){
          LEDOFF(1);
        }
        CS_State = CS_Five;
      }
      if(count == -1){
        LEDOFF(1);
        CS_State = CS_Init;
      }

    break;

    case CS_Five:

      if(digitalRead(sw) == 1 && otherCount != -1){        
        tcnt++;
      }
      if (otherCount == -1){
        CS_State = CS_Init;
      }
      if (digitalRead(sw) == 0){
        count = 9;
        CS_State = CS_Nine;
      }
    break;

    case CS_Hold:
      if(digitalRead(sw) == 1){
        count = 9;
        CS_State = CS_Nine;
      }
    break;

  }

  switch (CS_State){
    case CS_Init:
      LEDOFF(1);
    break;

    case CS_Nine:
      outNum(count);
      tcnt++;
      if(tcnt >= 10){
        tcnt = 0;
        count--;
      }
    break;

    case CS_Five:
      outNum(otherCount);
      if(tcnt >= 10){
        tcnt = 0;
        if (tcnt == 0){
          digitalWrite(green_pin, HIGH);
          otherCount--;
        }        
      }
    break;

    case CS_Hold:
    break;

  }
}


void setup() {
  // put your setup code here, to run once:
  pinMode(a, OUTPUT);
  pinMode(b, OUTPUT);
  pinMode(c, OUTPUT);
  pinMode(d, OUTPUT);
  pinMode(e, OUTPUT);
  pinMode(f, OUTPUT);
  pinMode(g, OUTPUT);
  pinMode(dp, OUTPUT);

  pinMode(sw, INPUT_PULLUP);

  pinMode(white_pin, OUTPUT);
  pinMode(green_pin, OUTPUT);
  TimerSet(100);
  TimerOn();
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  // call tick function here

  Serial.println(analogRead(A1));
  // Serial.println("shjkdfbksdjbf");

  DetectMotion();
  Counter();
  //Blink_LED();

  while (!TimerFlag){}  // Wait for BL's period
  TimerFlag = 0;  
  
}

#include <Servo.h>

#define DATA_PIN 8
#define BIT_TIME 1000
#define LEFT_MOTOR_PIN 9
#define RIGHT_MOTOR_PIN 10

#define LEFT_CENTER 85
#define RIGHT_CENTER 89

#define MAX_POWER 1

const uint16_t id = 0xabcd;

uint16_t buffer = 0x0000;

Servo leftMotor;
Servo rightMotor;

int maxRange(int center)
{
  return 90-abs(center-90);
}

const int leftMaxRange = maxRange(LEFT_CENTER);
const int rightMaxRange = maxRange(RIGHT_CENTER);


void setup(){
  Serial.begin(9600);
  pinMode(DATA_PIN, INPUT);
  pinMode(LEFT_MOTOR_PIN, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  leftMotor.attach(LEFT_MOTOR_PIN, 1000, 2000);
  
  rightMotor.attach(RIGHT_MOTOR_PIN, 1000, 2000);
}

void loop(){
  read();
  if (buffer == id)
  {
    for (int i = 0; i < 16; ++i) {
      short bit = readBit();
      if (bit > 1)
      {
        return;
      }
      buffer = (buffer << 1) | (bit & 0x01);
    }

    //                                 _                                     _ 
    //  ___ ___  _ __  _ __   ___  ___| |_    __ _ _ __ ___  _   _ _ __   __| |
    // / __/ _ \| '_ \| '_ \ / _ \/ __| __|  / _` | '__/ _ \| | | | '_ \ / _` |
    //| (_| (_) | | | | | | |  __/ (__| |_  | (_| | | | (_) | |_| | | | | (_| |
    // \___\___/|_| |_|_| |_|\___|\___|\__|  \__, |_|  \___/ \__,_|_| |_|\__,_|
    //                                        |___/                             
    Serial.println(mapInput((buffer & (0xff00)) >> 8, LEFT_CENTER, leftMaxRange));
    leftMotor.write(mapInput((buffer & (0xff00)) >> 8, LEFT_CENTER, leftMaxRange));
    rightMotor.write(mapInput(buffer & (0x00ff), RIGHT_CENTER, rightMaxRange)); 
  }
 
}

String toHex(uint16_t bits)
{
  String strn;
  for (int i = 0; i < 4; ++i) {
    uint8_t bit = (48+((bits >> (i*4)) & 0x000f));
    if(bit > 57)
    {
      bit += 39;
    }
    strn += (char)bit;
  }
  return strn;
}

void read(){
  buffer = (buffer << 1) | (readBit() & 0x01);
}

int mapInput(short input, int center, int range)
{
  double x = ((double)input)/255.0d;

  return constrain(map(((3*pow((1.0d-x),2.0d)*x+pow(x, 3.0d))*255.0d), 4, 251, center - (range * MAX_POWER), center + (range * MAX_POWER)), center - (range * MAX_POWER), center + (range * MAX_POWER));
}

short readBit(){
  bool first = digitalRead(DATA_PIN);
  delayMicroseconds(BIT_TIME/2);
  bool second = digitalRead(DATA_PIN);
  delayMicroseconds(BIT_TIME/2);

  if (first && !second) return 1;
  if (!first && second) return 0;
  return 2; // fallback
}



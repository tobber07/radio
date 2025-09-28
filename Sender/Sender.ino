#define DATA_PIN 4
#define LEFT_PIN A0
#define RIGHT_PIN A1
#define BIT_TIME 1000


const uint16_t id = 0xabcd;

uint16_t input = 0xcccc;

void setup() {

  pinMode(DATA_PIN, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  readInput();
  write(id);
  write(input);
}


void readInput()
{
  //shift the bits by 8 to make space for both inputs in one variable.
  input = (input << 8) | (map(analogRead(LEFT_PIN), 0, 1023 , 0, 255) & 0x00ff);
  input = (input << 8) | (map(analogRead(RIGHT_PIN), 0, 1023, 0, 255) & 0x00ff);
}

void write(uint16_t data)
{
  for (int i = 0; i < 16; ++i) {
    sendBit(((data >> (15-i)) & 0x0001));
  }
}


void sendBit(bool bit)
{
  //encrypt the bits to prevent dataloss
  //high = 1,0  low = 0,1
  digitalWrite(DATA_PIN, bit);
  delayMicroseconds(BIT_TIME/2);
  digitalWrite(DATA_PIN, !bit);
  delayMicroseconds(BIT_TIME/2);
}
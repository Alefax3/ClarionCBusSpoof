/* Shift Reader app for Arduino to test shift reading theory against Clarion data bus */

volatile byte dataIn[8];

int dt_pin = 2;
int cl_pin = 7;

volatile int counter = 0;
volatile bool receiving = false;

void setup() {
  pinMode(dt_pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(cl_pin), shift_dt, FALLING);
  Serial.begin(9600);
}

void loop() {
  
}

void shift_dt() {
  receiving = true;
  if (counter < 8) {
    dataIn[counter] = digitalRead(dt_pin);
  } else if (counter == 8) {
    for (int i = 0; i < 8; i++) {
      Serial.println(bitsToByte(dataIn));
    }
  } else if (counter > 140 && counter < 144) {
    digitalWrite(dt_pin, LOW);
  } else if (counter >= 144) {
    counter = 0;
    receiving = false;
  } else {
    digitalWrite(dt_pin, HIGH);
  }
  counter++;
}

byte bitsToByte(byte bits[8]) {
  int bitmask[8] = {128,64,32,16,8,4,2,1};
  int result = 0;
  for (int i = 0; i < 8; i++) {
    if (bits[i]) result += bitmask[i];
  }
  return result;
}

void processByte() {
  if (dataIn == 0xF7) {
    shiftOut(dt_pin, cl_pin, 
  }
}

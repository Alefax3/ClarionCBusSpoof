/* Shift Reader app for Arduino to test shift reading theory against Clarion data bus */

volatile in dataIn[8];

int dt_pin = 2;
int cl_pin = 7;

volatile int counter = 0;
volatile bool receiving = false;
volatile bool sending = false;

void setup() {
  pinMode(dt_pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(cl_pin), shift_dt, FALLING);
  Serial.begin(9600);
}

void send(byte data) {
  detachInterrupt(digitalPinToInterrupt(cl_pin));
  shiftOut(dt_pin, cl_pin, MSBFIRST, data);
  attachInterrupt(digitalPinToInterrupt(cl_pin), shift_dt, FALLING);
}

void loop() {
  if (!receiving && sending) {
    sending = false;
    byte value = bitsToByte(dataIn);
    if (value == 0xF7) {
      send(value);
    } else {
      if (value == 0x00) {
        send(0xF7);
      } else {
        send(value);
      }
    }
  }
}

void shift_dt() {
  receiving = true;
  if (counter < 8) {
    dataIn[counter] = digitalRead(dt_pin);
  } else if (counter == 8) {
    Serial.println(bitsToByte(dataIn));
  } else if (counter > 140 && counter < 144) {
    digitalWrite(dt_pin, LOW);
  } else if (counter >= 144) {
    counter = 0;
    receiving = false;
    sending = true;
  } else {
    digitalWrite(dt_pin, HIGH);
  }
  counter++;
}

byte bitsToByte(int bits[8]) {
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

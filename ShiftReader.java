/* Shift Reader app for Arduino to test shift reading theory against Clarion data bus */

volatile int dataIn[8];

int dt_pin = 2;
int cl_pin = 7;

volatile int counter = 0;
volatile bool receiving = false;
volatile bool sending = false;

byte lastmessage = 0x00;
bool readysend = false;

byte dataMessage[6] = { 0x03, 0x00, 0x01, 0x08, 0x00, 0x00 }; // Right now the message to send is just to request audio.

void setup() {
  pinMode(dt_pin, INPUT_PULLUP);
  pinMode(cl_pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(cl_pin), shift_dt, FALLING);
  Serial.begin(57600);
}

void send(byte data) {
  detachInterrupt(digitalPinToInterrupt(cl_pin));
  shiftOut(dt_pin, cl_pin, MSBFIRST, data);
  lastmessage = data;
  attachInterrupt(digitalPinToInterrupt(cl_pin), shift_dt, FALLING);
}

void loop() {
  delayMicroseconds(50);
  if (!receiving && sending) {
    sending = false;
    byte value = bitsToByte(dataIn);
    if (value == 0xF7) {
      send(value);
    } else {
      if (value == 0x00) {
        send(0xF7);
      } else if (value == 0x11) {
        send(0x11);
        readysend = true;
        delay(1);
      } else {
        if (counter <= dataMessage[0] && readysend) {
          send(dataMessage[counter]);
          counter++;
          delay(1);
        } else {
          counter = 0;
          readysend = false;
        }
      }
    }
  }
}

void shift_dt() {
  receiving = true;
  sending = false;
  if (counter < 8) {
    dataIn[counter] = digitalRead(dt_pin);
  } else if (counter == 8) {
    Serial.println(bitsToByte(dataIn));
  } else if (counter > 140 && counter < 144) {
    digitalWrite(dt_pin, LOW);
    Serial.println("Ending Transmission...");
  } else if (counter >= 144) {
    Serial.println("--------");
    counter = 0;
    receiving = false;
    sending = true;
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
  return (byte)result;
}

/* Shift Reader app for Arduino to test shift reading theory against Clarion data bus */

volatile int dataIn[8];

int dt_pin = 2;
int cl_pin = 3;

volatile int counter = 0;
volatile byte command = 0xFF;

byte lastmessage = 0x00;

byte dataMessage[6] = { 0x03, 0x00, 0x01, 0x08, 0x00, 0x00 }; // Right now the message to send is just to request audio.

void setup() {
  pinMode(dt_pin, INPUT_PULLUP);
  pinMode(cl_pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(cl_pin), shift_dt, FALLING);
  Serial.begin(9600);
}

void send(byte data) {
  detachInterrupt(digitalPinToInterrupt(cl_pin));
  shiftDataOut(dt_pin, cl_pin, MSBFIRST, data);
  lastmessage = data;
  attachInterrupt(digitalPinToInterrupt(cl_pin), shift_dt, RISING);
}

void loop() {
  delay(2);
  if (command != 0xFF) {
    send(command);
    counter = 0;
    Serial.println(command);
    command = 0xFF;
  }
}

void shift_dt() {
  if (counter < 8) {
    if (command == 0xFF) {
      command = digitalRead(dt_pin);
    } else {
      command = digitalRead(dt_pin) << 1;
    }
  } else if (counter > 20) {
    digitalWrite(dt_pin, LOW);
    delayMicroseconds(1);
    digitalWrite(dt_pin, HIGH);
  }
  counter++;
}

// Shift the bits out to the Head according to protocol.
void shiftDataOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, int val)
{
  uint8_t i;
  for (i = 0; i < 8; i++)  {
    digitalWrite(clockPin, LOW);
    if (bitOrder == LSBFIRST)
      digitalWrite(dataPin, !!(val & (1 << i)));
    else    
      digitalWrite(dataPin, !!(val & (1 << ((8 - 1 - i)))));
    delayMicroseconds(1);
    digitalWrite(clockPin, HIGH);
    delayMicroseconds(1);
  }
}

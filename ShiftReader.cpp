/* Shift Reader app for Arduino to test shift reading theory against Clarion data bus */

volatile int dataIn[8];

int dt_pin = 2;
int cl_pin = 3;

volatile bool byteDone = false;
volatile int counter = 0;
volatile byte lastbytein = 0xFF;

volatile bool initialized = false;

int messageStep = -1;

#define S_INIT -1;
#define S_COMMAND_ECHO 0;
#define S_BYTE_SEND 1;

byte lastbyteout = 0xFF;
byte nextbyteout = 0xFF;

byte messageToSend[6] = { 0x03, 0x00, 0x01, 0x08, 0x00, 0x00 }; // Right now the message to send is just to request audio.
int messageIndex = 1;

void setup() {
  pinMode(dt_pin, INPUT_PULLUP);
  pinMode(cl_pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(cl_pin), shift_dt, RISING);
  //attachInterrupt(digitalPinToInterrupt(dt_pin), init_mod, FALLING);
  Serial.begin(115200);
}

void send(byte data) {
  noInterrupts();
  shiftDataOut(dt_pin, cl_pin, MSBFIRST, data);
  lastbyteout = data;
  interrupts();
}

void loop() {
  delay(2);
  if (lastbytein != 0xFF && byteDone) {
    counter = 0;
    noInterrupts();
    Serial.println(lastbytein);
    switch(messageStep) {
      case S_INIT:
      case S_COMMAND_ECHO:
        send(lastbytein);
        if (messageStep == S_COMMAND_ECHO) {
          executeCommand(lastbytein);
          send(messageToSend[0]);
        }
        messageStep++;
        messageIndex = 1;
        break;
      case S_BYTE_SEND:
        if (lastbyteout != ~lastbytein) {
          resetMessage();
          break;
        }
        send(messageToSend[messageIndex]);
        messageIndex++;
        messageStep++;
        break;
    }
    interrupts();
  }
}

void resetMessage() {
  lastbytein = 0xFF;
  lastbyteout = 0xFF;
  nextbyteout = 0xFF;
  send(nextbyteout);
  messageStep = S_INIT;
}

void shift_dt() {
  if (counter < 8) {
    if (lastbytein == 0xFF) {
      lastbytein = digitalRead(dt_pin);
    } else {
      lastbytein = digitalRead(dt_pin) << 1;
    }
  } else if (counter == 8) {
    byteDone = true;
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

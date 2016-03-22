/* Shift Reader app for Arduino to test shift reading theory against Clarion data bus */

#define dt_pin 2
#define cl_pin 3
#define S_INIT -1
#define S_COMMAND_ECHO 0
#define S_BYTE_SEND 1
#define DOUT true
#define DIN false

volatile bool dataIO = false;
volatile int counter = 0;

volatile byte lastbytein = 0xFF;
volatile byte lastbyteout = 0xFF;
volatile byte nextbyteout = 0xFF;

volatile bool initialized = false;

int messageStep = -1;

byte messageToSend[6] = { 0x03, 0x00, 0x01, 0x08, 0x00, 0x00 }; // Right now the message to send is just to request audio.
int messageIndex = 1;

void setup() {
  pinMode(dt_pin, INPUT_PULLUP);
  pinMode(cl_pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(cl_pin), shift_dt, RISING);
  Serial.begin(115200);
}

void send(byte data) {
  /*noInterrupts();
  shiftDataOut(dt_pin, cl_pin, MSBFIRST, data);
  lastbyteout = data;
  interrupts();*/
  nextbyteout = data;
  lastbyteout = data;
}

void loop() {
  if (lastbytein != 0xFF && dataIO == DOUT) {
    counter = 0;
    noInterrupts();
    switch(messageStep) { // Starts at -1
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
        if (messageIndex == messageToSend[0]) { // Checks if at last bite, and resets
          resetMessage();
          break;
        }
        messageIndex++;
        messageStep++;
        break;
    }
    interrupts();
    Serial.println(lastbytein);
  }
}

void executeCommand(byte command) {
  switch(command) {
    case 0x11:
      messageToSend[0] = 0x03;
      messageToSend[1] = 0x00;
      messageToSend[2] = 0x01;
      messageToSend[3] = 0x08;
      break;
    case 0x09:
      messageToSend[0] = 0x03;
      messageToSend[1] = 0x00;
      messageToSend[2] = 0x01;
      messageToSend[3] = 0x00;
      break;
  }
}

void resetMessage() {
  lastbytein = 0xFF;
  lastbyteout = 0xFF;
  nextbyteout = 0xFF;
  messageStep = S_INIT;
}

void shift_dt() {
  if (counter < 8 && dataIO == DIN) {
    if (lastbytein == 0xFF) {
      lastbytein = digitalRead(dt_pin);
    } else {
      lastbytein = digitalRead(dt_pin) << 1;
    }
  } else if (counter < 8 && dataIO == DOUT) {
    digitalWrite(dt_pin, !!(nextbyteout & (1 << ((8 - 1 - counter)))));
  } else if (counter == 8) {
    dataIO = !dataIO;
  } else if (counter > 15) {
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
    digitalWrite(clockPin, LOW); // Bring clock low
    if (bitOrder == LSBFIRST) // Set data pin on the falling edge of clock...
      digitalWrite(dataPin, !!(val & (1 << i)));
    else    
      digitalWrite(dataPin, !!(val & (1 << ((8 - 1 - i)))));
    delayMicroseconds(4);
    digitalWrite(clockPin, HIGH); // Bring clock high, leaving data where it is to be read on the rising edge...
    delayMicroseconds(4);
  }
}

/*
  Clarion C-Bus Driver
  Prototyped off 1995 Saab 900 Clarion stereo.
  Read the Readme for more information
  Rob Lester
  GNU General Public License, version 2.0 (GPL-2.0)
*/

#define dt_pin 7 // Data Pin
#define cl_pin 2 // Clock pin on INT0
#define rs_pin 6 // RSQ pin
#define bt_pl_pin 5 // Playback: Play
#define bt_nx_pin 4 // Playback: Next
#define by_pr_pin 3 // Playback: Prev
#define S_INIT -1 // Initial transmission step
#define S_COMMAND_ECHO 0
#define S_RESP_LEN 1
#define S_BYTE_SEND 2
#define DOUT true
#define DIN false

volatile bool dataIO = false;
volatile int counter = 0;

volatile byte lastbytein = 0xFF;
volatile byte lastbyteout = 0xFF;
volatile byte nextbyteout = 0xFF;

volatile bool initialized = false;

int messageStep = -1;

byte messageToSend[6] = { 0x03, 0x00, 0x01, 0x00, 0x00, 0x00 }; // Initialize the standard response message. This will change frequently.
int messageIndex = 1;

void setup() {
  pinMode(dt_pin, INPUT_PULLUP);
  pinMode(cl_pin, INPUT);
  attachInterrupt(digitalPinToInterrupt(cl_pin), shift_dt, RISING);
  Serial.begin(115200);
}

void send(byte data) {
  nextbyteout = data;
  lastbyteout = data;
}

void loop() {
  if (lastbytein != 0xFF && dataIO == DOUT) {
    counter = 0;
    noInterrupts();
    Serial.println(lastbytein, HEX);
    delay(1);
    switch(messageStep) { // Starts at -1
      case S_INIT:
      case S_COMMAND_ECHO:
        send(lastbytein);
        if (messageStep == S_COMMAND_ECHO) {
          executeCommand(lastbytein);
        }
        messageStep++;
        break;
      case S_RESP_LEN:
        send(messageToSend[0]);
        messageIndex = 1;
        messageStep++;
        break;
      case S_BYTE_SEND:
        if (lastbyteout != ~lastbytein) {
          resetMessage();
          break;
        }
        send(messageToSend[messageIndex]);
        if (messageIndex == messageToSend[0]) { // Checks if at last bite, and resets
          resetMessage();
          messageStep = S_COMMAND_ECHO;
          break;
        }
        messageIndex++;
        messageStep++;
        break;
    }
    interrupts();
  }
}

void executeCommand(byte command) {
  switch(command) {
    case 0x11: // Request Audio
      messageToSend[0] = 0x03;
      messageToSend[1] = 0x00;
      messageToSend[2] = 0x01;
      messageToSend[3] = 0x08;
      break;
    default: // Respond to anything not mentioned.
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

void shift_dt() { // Interrupt function. Fired on every clock pulse from HEAD.
  if (counter < 8 && dataIO == DIN) {
    if (lastbytein == 0xFF) {
      lastbytein = digitalRead(dt_pin); // First bit in
    } else {
      lastbytein = digitalRead(dt_pin) << 1; // Shift in next bit.
    }
  } else if (counter < 8 && dataIO == DOUT) {
    digitalWrite(dt_pin, !!(nextbyteout & (1 << ((8 - 1 - counter))))); // Shift out bit
  } else if (counter == 8) {
    if (dataIO == DIN && messageStep != S_RESP_LEN) dataIO = !dataIO;
  } else if (counter > 50) {
    digitalWrite(dt_pin, LOW);
    delayMicroseconds(1);
    digitalWrite(dt_pin, HIGH);
  }
  counter++;
}

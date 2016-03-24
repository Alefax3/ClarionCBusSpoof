/*
	Clarion C-Bus Driver
	Prototyped off 1995 Saab 900 Clarion stereo.
	Read the Readme for more information
	Rob Lester
	GNU General Public License, version 2.0 (GPL-2.0)
*/

#define dt_pin 7 // Data Pin
#define cl_pin 2 // Clock pin on INT0


volatile bool dataIO = false;
volatile int counter = 0;

volatile byte lastbytein = 0x00;

volatile int bitsin[8];

volatile int lastInterruptTime = 0;

void setup() {
	pinMode(dt_pin, INPUT_PULLUP);
	pinMode(cl_pin, INPUT);
	attachInterrupt(digitalPinToInterrupt(cl_pin), shift_dt, RISING);
	Serial.begin(115200);
}

void loop() {
	registerState = PIND;
	if (micros() > (lastInterruptTime + 20)) {
		counter = 0;
		for (int i = 0; i < 8; i++) {
			lastbytein = (lastbytein << 1) | bitsin[i];
		}
		Serial.println(lastbytein, HEX);
	}
}

void shift_dt() {
	bitsin[counter] = bitRead(registerState, dt_pin);
	lastInterruptTime = micros();
	counter++;
}

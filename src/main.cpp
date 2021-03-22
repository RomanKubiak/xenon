#include "MIDIFilter.h"

elapsedMillis statusBlink;
MIDIFilter midiFilter;

void setup()
{
    Serial.begin(115200);
	midiFilter.begin();

	pinMode(13, OUTPUT); // LED pin

	digitalWriteFast(13, HIGH); // LED on
	delay(500);
	digitalWriteFast(13, LOW);
}

void loop()
{
	if (statusBlink > 345) {
		digitalWrite(13, !digitalRead(13));
		statusBlink = 0;
	}
	midiFilter.update();
}


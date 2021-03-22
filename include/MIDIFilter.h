//
// Created by atom on 17.03.2021.
//

#ifndef XENON_MIDIFILTER_H
#define XENON_MIDIFILTER_H

#define SYSEX_LENGTH 128

#include "MIDIPortWrapper.h"
#include <WS2812Serial.h>

#define RED         0xa00000
#define GREEN       0x000a00
#define BLACK       0x000000
#define BLUE        0x00000a

#define DIMBLUE     0x00000a
#define DIMRED      0x0a0000
#define DIMGREEN    0x000a00
#define DIMYELLOW   0x0a0a00
#define DIMPINK     0x0a000a
#define DIMVIOLET   0x33004D
#define DIMBROWN    0x0a0001
#define LED_COUNT   12
#define LED_PIN     24

struct MIDIFilterConnection {
    MIDIPortWrapper *src;
    std::vector<MIDIPortWrapper *> dst;
};

class MIDIFilter
{
public:
	MIDIFilter();
	void begin();
	void update();
	bool recieveFromPort(MIDIPortWrapper *port);
    void sendToPort(MIDIPortWrapper *port, const MIDIMessage &m);
private:
    void blink(const uint8_t led, const byte type);
    uint32_t typeToColor(uint8_t type);

    USBHost usbHost;
    USBHub usbHub1;
    USBHub usbHub2;
    USBHub usbHub3;
    MIDIDevice usbMIDIDevice;

    MIDIPortHardware port1;
    MIDIPortHardware port2;
    MIDIPortHardware port3;
    MIDIPortHardware port4;
    MIDIPortHardware port5;
    MIDIPortUSBClient port6;
    MIDIPortUSBHost port7;

    WS2812Serial leds;
	bool ledState[LED_COUNT];
	elapsedMillis ledBlink[LED_COUNT];
    std::vector<MIDIPortWrapper *> ports;
    std::vector<MIDIFilterConnection> connections;
    elapsedMillis ledsShow;
    MIDIMessage currentMessage;
    bool ignoreRealtime = false;
};


#endif //XENON_MIDIFILTER_H

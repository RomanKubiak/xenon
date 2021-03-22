//
// Created by atom on 17.03.2021.
//

#include "MIDIFilter.h"

static byte drawingMemory[LED_COUNT * 3];         //  3 bytes per LED
static DMAMEM byte displayMemory[LED_COUNT * 12]; // 12 bytes per LED

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI1);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI2);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial3, MIDI3);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial4, MIDI4);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial5, MIDI5);

MIDIFilter::MIDIFilter() : usbHub1(usbHost),
                           usbHub2(usbHost),
                           usbHub3(usbHost),
                           usbMIDIDevice(usbHost),
                           port1(MIDI1, "DIN1", 6,4),
                           port2(MIDI2, "DIN2", 7,3),
                           port3(MIDI3, "DIN3", 8,2),
                           port4(MIDI4, "DIN4", 9,1),
                           port5(MIDI5, "DIN5", 10, 0),
                           port6(usbMIDI, "USB Client", 11,11),
                           port7(usbMIDIDevice, "USB Host", 5,5),
                           leds{LED_COUNT, displayMemory, drawingMemory, LED_PIN, WS2812_GRB}, ledState{false} {
    ports.push_back(&port1);
    ports.push_back(&port2);
    ports.push_back(&port3);
    ports.push_back(&port4);
    ports.push_back(&port5);
    ports.push_back(&port6);
    ports.push_back(&port7);

    /* default set of connections, all inputs map to all outputs */
    connections = std::vector<MIDIFilterConnection>{
            {
                    {ports[0], {ports[1], ports[2], ports[3], ports[4], ports[5], ports[6]}},
                    {ports[1], {ports[0], ports[2], ports[3], ports[4], ports[5], ports[6]}},
                    {ports[2], {ports[0], ports[1], ports[3], ports[4], ports[5], ports[6]}},
                    {ports[3], {ports[0], ports[1], ports[2], ports[4], ports[5], ports[6]}},
                    {ports[4], {ports[0], ports[1], ports[2], ports[3], ports[5], ports[6]}},
                    {ports[5], {ports[0], ports[1], ports[2], ports[3], ports[4], ports[6]}},
                    {ports[6], {ports[0], ports[1], ports[2], ports[3], ports[4], ports[5]}},
            }
    };
}

void MIDIFilter::begin() {
    for (auto &port : ports) {
        port->begin(MIDI_CHANNEL_OMNI);
        port->turnThruOff();
    }
    leds.begin();
    leds.clear();
    leds.show();
}

void MIDIFilter::blink(const uint8_t led, const byte type) {
    if (ignoreRealtime && (
            (type == midi::Clock) || (type == midi::Start) || (type == midi::Stop)
                          )) {
        return;
    }

    if (ledBlink[led] > 5) {
        leds.setPixel(led, ledState[led] ? BLACK : typeToColor(type));
        ledState[led] = !ledState[led];
        ledBlink[led] = 0;
    }
}

uint32_t MIDIFilter::typeToColor(uint8_t type) {
    uint32_t color = DIMRED;
    switch(type) {
        case midi::NoteOn:
            color = DIMGREEN;
            break;

        case midi::NoteOff:
            color = DIMBLUE;
            break;

        case midi::ControlChange:
            color = DIMRED;
            break;

        case midi::PitchBend:
            color = DIMYELLOW;
            break;

        case midi::AfterTouchPoly:
            color = DIMPINK;
            break;
        case midi::AfterTouchChannel:
            color = DIMVIOLET;
            break;
        case midi::ModulationWheel:
            color = DIMBROWN;
            break;

        default:
            break;
    }

    return color;
}

bool MIDIFilter::recieveFromPort(MIDIPortWrapper *port) {
    if (port->read()) {
        blink(port->getInLed(), port->getType());
        return true;
    }
    return false;
}

void MIDIFilter::sendToPort(MIDIPortWrapper *port, const MIDIMessage &m) {
    blink(port->getOutLed(), m.type);
    port->send(m);
}

void MIDIFilter::update() {
    usbHost.Task();
    for (auto &port : ports) {
        if (recieveFromPort(port)) {
            currentMessage.type = port->getType();
            currentMessage.data1 = port->getData1();
            currentMessage.data2 = port->getData2();
            currentMessage.channel = port->getChannel();
            currentMessage.sysexData = port->getSysExArray();
            currentMessage.sysexDataSize = port->getSysExArrayLength();

            for (auto &conn: connections) {
                if (conn.src == port) {
                    if (conn.dst.size() > 0) {
                        for (auto &destination: conn.dst) {
                            sendToPort(destination, currentMessage);
                        }
                    }
                }
            }
        }
    }

    if (ledsShow > 5) {
        leds.show();
    }
}
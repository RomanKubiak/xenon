//
// Created by atom on 19.03.2021.
//

#ifndef XENON_MIDIPORTWRAPPER_H
#define XENON_MIDIPORTWRAPPER_H

#include <usb_midi.h>
#include <MIDI.h>
#include <USBHost_t36.h>
#include <vector>
#include <string>

struct MIDIMessage {
    byte type;
    byte data1;
    byte data2;
    byte channel;
    const byte *sysexData;
    uint16_t sysexDataSize;
};

class MIDIPortWrapper {
public:
    MIDIPortWrapper(const std::string &_portName, const uint8_t _portInLed = 0, const uint8_t _portOutLed = 0)
        : portName(_portName), portInLed(_portInLed), portOutLed(_portOutLed) {}

    virtual ~MIDIPortWrapper() {}

    virtual void begin(byte channel) = 0;

    virtual void turnThruOff() = 0;

    virtual const bool read() = 0;
    virtual inline const byte* getSysExArray() const { return nullptr; }
    virtual const byte getType() = 0;
    virtual const byte getChannel() = 0;
    virtual const byte getData1() = 0;
    virtual const byte getData2() = 0;

    virtual void send(byte type, byte data1, byte data2, byte channel) = 0;
    virtual void send(MIDI_NAMESPACE::MidiType type, byte data1, byte data2, byte channel) = 0;
    virtual void send(const MIDIMessage &m) = 0;

    virtual void sendSysEx(unsigned int length, const byte *inArray, bool inArrayContainsBoundries = false) = 0;
    virtual inline const unsigned getSysExArrayLength() { return 0; }
    const std::string getName() { return portName; }
    const uint8_t getInLed() { return portInLed; }
    const uint8_t getOutLed() { return portOutLed; }
private:
    std::string portName;
    uint8_t portInLed;
    uint8_t portOutLed;
};

class MIDIPortHardware : public MIDIPortWrapper {
public:
    MIDIPortHardware(midi::MidiInterface<midi::SerialMIDI<HardwareSerial>> &_midiPort, const std::string &portName,
                     const uint8_t portInLed, const uint8_t portOutLed)
            : MIDIPortWrapper(portName, portInLed, portOutLed), midiPort(_midiPort) {}

    ~MIDIPortHardware() {}

    void begin(byte channel) override { midiPort.begin(channel); }

    void turnThruOff() override { midiPort.turnThruOff(); }

    const bool read() override { return midiPort.read(); }
    inline const byte* getSysExArray() const { return midiPort.getSysExArray(); }
    const byte getType() override { return midiPort.getType(); }
    const byte getChannel() override { return midiPort.getChannel(); }
    const byte getData1() override { return midiPort.getData1(); }
    const byte getData2() override { return midiPort.getData2(); }

    void send(byte type, byte data1, byte data2, byte channel) override {
        midiPort.send((MIDI_NAMESPACE::MidiType)type, data1, data2, channel);
    }

    void send(const MIDIMessage &m) {}

    void send(MIDI_NAMESPACE::MidiType type, byte data1, byte data2, byte channel) override{
        midiPort.send((MIDI_NAMESPACE::MidiType)type, data1, data2, channel);
    }

    void sendSysEx(unsigned int length, const byte *inArray, bool inArrayContainsBoundries = false) override {}
    inline const unsigned getSysExArrayLength() { return midiPort.getSysExArrayLength(); }
private:
    midi::MidiInterface<midi::SerialMIDI<HardwareSerial>> &midiPort;
};


class MIDIPortUSBClient : public MIDIPortWrapper {
public:
    MIDIPortUSBClient(usb_midi_class &_midiPort, const std::string &portName, const uint8_t portInLed, const uint8_t portOutLed)
            : MIDIPortWrapper(portName, portInLed, portOutLed), midiPort(_midiPort) {}

    ~MIDIPortUSBClient() {}

    void begin(byte channel) override { midiPort.begin(); }

    void turnThruOff() override {}

    const bool read() override { return midiPort.read(); }
    inline const byte* getSysExArray() const { return midiPort.getSysExArray(); }
    const byte getType() override { return midiPort.getType(); }
    const byte getChannel() override { return midiPort.getChannel(); }
    const byte getData1() override { return midiPort.getData1(); }
    const byte getData2() override { return midiPort.getData2(); }

    void send(byte type, byte data1, byte data2, byte channel) override {
        midiPort.send(type, data1, data2, channel, 0);
    }

    void send(const MIDIMessage &m) {
    }

    void send(MIDI_NAMESPACE::MidiType type, byte data1, byte data2, byte channel) override {}

    void sendSysEx(unsigned int length, const byte *inArray, bool inArrayContainsBoundries = false) override {}
    inline const unsigned getSysExArrayLength() { return midiPort.getSysExArrayLength(); }
private:
    usb_midi_class &midiPort;
};


class MIDIPortUSBHost : public MIDIPortWrapper {
public:
    MIDIPortUSBHost(MIDIDevice &_midiPort, const std::string &portName, const uint8_t portInLed, const uint8_t portOutLed)
            : MIDIPortWrapper(portName, portInLed, portOutLed), midiPort(_midiPort) {}

    ~MIDIPortUSBHost() {}

    void begin(byte channel) override { midiPort.begin(); }

    void turnThruOff() override {}

    const bool read() override { return midiPort.read(); }
    inline const byte* getSysExArray() const { return midiPort.getSysExArray(); }
    const byte getType() override { return midiPort.getType(); }
    const byte getChannel() override { return midiPort.getChannel(); }
    const byte getData1() override { return midiPort.getData1(); }
    const byte getData2() override { return midiPort.getData2(); }

    void send(byte type, byte data1, byte data2, byte channel) override {
        midiPort.send(type, data1, data2, channel);
    }

    void send(const MIDIMessage &m) {
    }

    void send(MIDI_NAMESPACE::MidiType type, byte data1, byte data2, byte channel) override {}

    void sendSysEx(unsigned int length, const byte *inArray, bool inArrayContainsBoundries = false) override {}
    inline const unsigned getSysExArrayLength() { return midiPort.getSysExArrayLength(); }
private:
    MIDIDevice &midiPort;
};

#endif //XENON_MIDIPORTWRAPPER_H

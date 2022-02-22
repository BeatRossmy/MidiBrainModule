#ifndef HARDWARE_H
#define HARDWARE_H

#include "LED.h"
#include <MIDI.h>
#include <USBHost_t36.h>

LED led_in,led_usb,led_1,led_2,led_3;

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI_1);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI_2);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial3, MIDI_3);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial4, MIDI_4);
midi::MidiInterface<HardwareSerial> * midi_outs [4] = {&MIDI_1, &MIDI_2, &MIDI_3, &MIDI_4};

USBHost myusb;
USBHub hub1(myusb);
MIDIDevice midi01(myusb);
MIDIDevice midi02(myusb);
MIDIDevice midi03(myusb);
MIDIDevice midi04(myusb);
MIDIDevice * midilist[4] = {&midi01, &midi02, &midi03, &midi04};

/*const int8_t LP1 = 32; //31+id -> 32-39
const int8_t LP2 = 33;
const int8_t LP3 = 34;
const int8_t LP4 = 35;
const int8_t KBOARD = 26;*/

const int16_t LPX = 259;

void sendMidiToPort () {}

void sendNoteToDevice (int8_t note, int8_t velocity, int8_t channel, int16_t idProduct) {
  if (note > 127) note = 127;
  if (note < 0) note = 0;
  for (int8_t port=0; port < 4; port++) {
    if (midilist[port]->idProduct() == idProduct) {
      midilist[port]->sendNoteOn(note,velocity,channel);
    }
  }
}

void sendCCToDevice (int8_t note, int8_t velocity, int8_t channel, int16_t idProduct) {
  for (int8_t port=0; port < 4; port++) {
    if (midilist[port]->idProduct() == idProduct) {
      midilist[port]->sendControlChange(note,velocity,channel);
    }
  }
}

void initHardware () {
  MIDI_1.begin(MIDI_CHANNEL_OMNI);
  MIDI_1.turnThruOff();
  MIDI_2.begin();
  MIDI_3.begin();
  MIDI_4.begin();

  myusb.begin();
  //usbMIDI.setHandleRealTimeSystem(RealTimeSystem);

  led_in = LED(20);
  led_1 = LED(21);
  led_2 = LED(22);
  led_3 = LED(23);
  led_usb = LED(29);
}

void handleHardware () {
  led_in.handle();
  led_usb.handle();

  led_1.handle();
  led_2.handle();
  led_3.handle();
}

#endif

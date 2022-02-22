#include <EEPROM.h>
#include "MIDI_BRAIN_LPX.h"

elapsedMicros kTimer;

void setup () {
  Serial.begin(115200);

  initHardware();
  delay(600);
  usbMIDI.setHandleRealTimeSystem(RealTimeSystem);
  // stop all midi messages
  for (int n = 0; n < 128; n++) {
    MIDI_1.sendNoteOff(n, 0, 1);
    MIDI_2.sendNoteOff(n, 0, 1);
    MIDI_3.sendNoteOff(n, 0, 1);
    MIDI_4.sendNoteOff(n, 0, 1);
  }

  // TEST CODE LAUNCHPAD X
  for (int8_t port=0; port < 4; port++) {
    Serial.println(midilist[port]->idProduct());
  }

  static uint8_t msg [9] = {240, 0, 32, 41, 2, 12, 14, 127, 247};
  midilist[0]->sendSysEx(9,msg); // Address hardcoded !!!!! CHANGE
  Serial.println("switched to programmer's mode");

  static uint8_t msg2 [] = {240, 0, 32, 41, 2, 12, 3, 2, 99, 3, 247};
  midilist[0]->sendSysEx(11,msg2);
}

void loop () {
  while (usbMIDI.read()) {}
  handleExtClock();
  handleUSBdevices();
  kRate(kTimer);
}

void RealTimeSystem(byte realtimebyte) {
  midi_brain.handleClock(realtimebyte);
}

void kRate (elapsedMicros & timer) {
  if (timer < 500) return;
  timer -= 500;
  midi_brain.kRate();
  handleHardware();
}

void handleUSBdevices () {
  for (int port = 0; port < 4; port++) {
    if (midilist[port]->read()) {

      led_usb.setOn();

      uint8_t type =       midilist[port]->getType();
      uint8_t data1 =      midilist[port]->getData1();
      uint8_t data2 =      midilist[port]->getData2();
      uint8_t channel =    midilist[port]->getChannel();
      const uint8_t *sys = midilist[port]->getSysExArray();
      int idProduct = midilist[port]->idProduct();

      midi_brain.handleMIDI(idProduct, type, data1, data2, channel);
    }
  }
}

void handleExtClock () {
  if (MIDI_1.read()) {
    byte type = MIDI_1.getType();
    if (type == midi::ControlChange) {
      uint8_t data1 = MIDI_1.getData1();
      uint8_t data2 = MIDI_1.getData2();
      uint8_t channel = MIDI_1.getChannel();
      midi_brain.handleCC(data1, data2, channel);
      return;
    }
    else if (type == midi::NoteOn) {}
    else if (type == midi::NoteOff) {}
    else {
      midi_brain.handleClock(type);
    }
  }
}

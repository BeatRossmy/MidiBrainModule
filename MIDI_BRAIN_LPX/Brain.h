#ifndef BRAIN_H
#define BRAIN_H

/*
EEPROM:
m1:
  output: 0
  class:  1
  ...
m2:
  output: 10
  class:  11
  ...
...
*/

class BRAIN : GRID_LISTENER {
private:
  CLOCK clock = CLOCK();

  LP_X lp = LP_X(LPX);

  USB_SENDER usbPort_XXX = USB_SENDER();
  KEYBOARD kboard= KEYBOARD(&lp);

  STEP_SEQ s1 = STEP_SEQ(0, &clock, &kboard, &lp);
  MIDI_LOOPER s2 = MIDI_LOOPER(1, &clock, &kboard, &lp);

  const int8_t seqLength = 2;
  MACHINE * sequencer [2] = {&s1,&s2};
  int8_t mode = 0;

  //STEP_SEQ testMachine = STEP_SEQ(0, &clock, &kboard, &lp);
  //MACHINE * activeMachine = & testMachine;

  /*POLY_SEQ m1 = POLY_SEQ(0,&clock, &lp, &grid, &keys);
  GRAF_SEQ m2 = GRAF_SEQ(1,&clock, &lp, &grid, &keys);
  POLY_SEQ m3 = POLY_SEQ(2,&clock, &lp, &grid, &keys);
  GRAF_SEQ m4 = GRAF_SEQ(3,&clock, &lp, &grid, &keys);
  MACHINE * machines [4] = {&m1, &m2, &m3, &m4};*/

public:
  BRAIN () {
    //m1.setVisible(true);

    lp.addListener(this);

    kboard.addListener(&usbPort_XXX);
    ////kboard.addListener(&seq); // PROBLEM??? not informing currentlistener???

    sequencer[mode]->activate();

    //testMachine.activate();

    /*for (int i=0; i<4; i++) {
      machines[i]->setMidiOutput(EEPROM.read(i*10+0));
      machines[i]->setOutputChannel(EEPROM.read(i*10+1));
      machines[i]->setBypassCC(EEPROM.read(i*10+2));
      machines[i]->setInputChannel(EEPROM.read(i*10+3));
    }*/
  }

  void display () {
    for (int i=0; i<seqLength; i++) lp.set(64+i,(mode==i)?GreenFull:GreenLow);
    //lp.set(0,GreenFull);
  }

  void kRate () {
    //for (int i=0; i<4; i++) machines[i]->kRate();
    display();

    kboard.kRate();
    lp.kRate();

    sequencer[mode]->display();

    //activeMachine->kRate();
  }

  void handleMIDI (int16_t device, uint8_t type, uint8_t data1, uint8_t data2, uint8_t channel) {
    lp.handleMIDI(device,type,data1,data2,channel);
  }

  void handleCC (uint8_t data1, uint8_t data2, uint8_t channel) {
    /*for (int i=0; i<4; i++) {
      if (machines[i]->receivingCC() && machines[i]->getInputChannel()==channel) machines[i]->handleCC(data1,data2,channel);
    }*/
  }

  void handleClock (byte type) {
    clock.handleMIDI(type);
    lp.sendClock(type);
  }

  virtual void grid_event (GRID_EVENT e) {
    if (e.type == PRE && e.pos>=64 && e.pos<64+seqLength) {
      // deactivate active mode
      sequencer[mode]->deactivate();
      mode = e.pos-64;
      sequencer[mode]->activate();
      // activate new mode
    }
  }

};

#endif

#ifndef MACHINE_H
#define MACHINE_H

/*
[] [] [] [] [] [] [] []  ()
[] [] [] [] [] [] [] []  ()
[] [] [] [] [] [] [] []  ()
[] [] [] [] [] [] [] []  ()
[] [] [] [] [] [] [] []  ()
[] [] [] [] [] [] [] []  ()
[] [] [] [] [] [] [] []  ()
[] [] [] [] [] [] [] []  ()
*/

/*struct NOTE {
  int32_t timestamp = -1;
  int8_t pitch = -1;
  int8_t velocity = -1;
  int16_t duration = -1;
  inline bool operator==(NOTE e) {return (e.pitch==pitch);}
};*/

struct NOTE {
  int8_t pitch = -1;
  int8_t velocity = -1;
  int8_t channel = -1;
  int16_t duration = -1;
  int32_t timestamp = -1;
  inline bool operator==(NOTE e) {return (e.pitch==pitch);}
};

class MIDI_OUTPUT : public CLOCK_LISTENER {
  bool midiFork = false;
  int midi_output = 0;
  int8_t output_channel = 1;
  STACK<int8_t,8,true> out_channels;
  STACK<NOTE,4,false> activeNotes;
  
  public:
  MIDI_OUTPUT () : CLOCK_LISTENER(1) {
    //out_channels.add(1);
    //out_channels.add(2);
    //out_channels.add(3);
  }
  
  void stop_all_notes () {
    for (int i=activeNotes.size()-1; i>-1; i--) {
      usbMIDI.sendNoteOff(activeNotes[i].pitch,0,activeNotes[i].channel);
      activeNotes.remove(activeNotes[i]);
    }
  }
  
  void setOutput (int out) {
    stop_all_notes();
    midi_output = out;
  }
  
  void clock_event (CLOCK_EVENT e) {
    if (e.type == TICK) {
      for (int i=activeNotes.size()-1; i>-1; i--) {
        activeNotes[i].duration--;
        if (activeNotes[i].duration <= 0) {
          usbMIDI.sendNoteOff(activeNotes[i].pitch,0,activeNotes[i].channel);
          activeNotes.remove(activeNotes[i]);
        }
      }
    }
    
    if (e.type == STOP) stop_all_notes();
  }
  
  void playNote (NOTE n) {
    if (activeNotes.size()<activeNotes.capacity()) {
      n.channel = output_channel;
      if (midiFork) {
        output_channel = (output_channel+1)%out_channels.size();
        n.channel = out_channels[output_channel];
        Serial.println(n.channel);
      }
      //sendNoteOn(n.pitch,n.velocity);
      usbMIDI.sendNoteOn(n.pitch,n.velocity,n.channel);
      activeNotes.add(n);
    }
  }
};

class MACHINE : public CLOCK_LISTENER, public KEYBOARD_LISTENER, public GRID_LISTENER {
protected:
  CLOCK * clock;
  LP_X * lp;
  KEYBOARD * keys;

  int8_t id;
  bool visible;
  bool settings;
  bool bypassCC;
  MIDI_OUTPUT output;

  int8_t input_channel = 1;

  //int32_t step = -1;
  //std::vector <NOTE> activeNotes = std::vector<NOTE>(8);

public:
  MACHINE (int8_t i, CLOCK * c, KEYBOARD * k, LP_X * l) : id(i), clock(c), keys(k), lp(l) {clock->addListener(this);
    clock->addListener(&output);
    
    lp->addListener(this);
  }

  void activate () {
    visible = true;
    keys->setCurrentListener(this);
    //keys->setExtNoteStack(&activeNotes);
  }
  void deactivate () {
    visible = false;
    rec = false;
  }
  bool isVisible () {return visible;}

  virtual void display () {
    for (int i=56; i<64; i++) lp->set(i,AmberFull);
    /*for (int i=0; i<64; i++) outputDevice->set(i,Off);
    for (int i=0; i<4; i++) outputDevice->set(i,(i==midi_output)?AmberFull:AmberLow);
    for (int i=0; i<16; i++) outputDevice->set(8+i,(i==input_channel)?AmberFull:AmberLow);
    for (int i=0; i<16; i++) outputDevice->set(24+i,(i==output_channel)?AmberFull:AmberLow);
    for (int i=0; i<2; i++) outputDevice->set(40+i,(bypassCC)?AmberFull:AmberLow);*/
  }

  void kRate () {
    if (!visible) return;
    if (settings) MACHINE::display();
    display();
  }

  virtual void clock_event (CLOCK_EVENT e) = 0;

  virtual void keyboardNoteMessage (MIDI_NOTE_MESSAGE msg) = 0;

  virtual void grid_event (GRID_EVENT e) = 0;

  void setSettings (bool b) {
    settings = b;
  }
  void setMidiOutput (int8_t out) {
    //midi_output = out;
    //EEPROM.write(id*10+0,out);
  }
  void setInputChannel (int8_t c) {
    //input_channel = c;
    //EEPROM.write(id*10+3,c);
  }
  void setOutputChannel (int8_t c) {
    //output_channel = c;
    //EEPROM.write(id*10+1,c);
  }
  void setBypassCC (bool b) {
    //bypassCC = b;
    //EEPROM.write(id*10+2,b);
  }

  void playNote (NOTE n) {
    output.playNote(n);
  }

  bool inSettings () {return settings;}

  bool receivingCC () {return !bypassCC;}

  int8_t getInputChannel () {return input_channel;}

  void handleCC (uint8_t data1, uint8_t data2, uint8_t channel) {/*sendCC(data1,data2);*/}
};

#endif

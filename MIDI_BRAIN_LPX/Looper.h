#ifndef LOOPER_H
#define LOOPER_H

class MIDI_LOOP {
  STACK<MIDI_NOTE_MESSAGE,4,false> tape [128];
  const int maxLoopLength = 128;
  int length = 128;
  public:
  MIDI_LOOP () {}
  
  void play (int32_t pos, MACHINE * parent) {
    for (int n=0; n<tape[pos].size(); n++) {
      MIDI_NOTE_MESSAGE msg = tape[pos].get(n);
      parent->playNote({msg.pitch,msg.velocity,1,6,millis()});
    }
  }
  
  void write (int pos, MIDI_NOTE_MESSAGE msg) {
    tape[pos].add(msg);
  }
  
  bool emptyAt (int pos) {
    return (tape[pos].size()==0);
  }
};

class MIDI_LOOPER : public MACHINE {

  //STACK<int8_t,12,false> stepStack;
  //bool editFlag = false;
  int selectedLoop = 0;
  int maxLoopLength = 128;
  
  MIDI_LOOP l1;
  MIDI_LOOP l2;
  MIDI_LOOP l3;
  MIDI_LOOP l4;
  
  MIDI_LOOP * loops [4] = {&l1,&l2,&l3,&l4};

public:
  MIDI_LOOPER (int id, CLOCK* c, KEYBOARD* k, LP_X* l) : MACHINE(id,c,k,l) {}

  void clock_event (CLOCK_EVENT e) {
    if (e.type != TICK) return;
    pos = e.pos%128;
    for (int l=0; l<4; l++) loops[l]->play(pos,this);
  }

  virtual void display () {
    lp->clear(32,32);
    for (int l=0; l<4; l++) {
      for (int s=0; s<128; s+=4) if (loops[l]->emptyAt(s)==false) lp->set((56-8*l)+(s/16),AmberFull);
    }
    for (int i=0; i<4; i++) lp->set(72+i,(i==selectedLoop)?3:1);
    lp->set(timeToPos[pos/16],5);
  }

  void keyboardNoteMessage (MIDI_NOTE_MESSAGE msg) {
    if (!rec) return;
    int p = clock->getPos(this)%128;
    loops[selectedLoop]->write(p,msg);
  }

  void grid_event (GRID_EVENT e) {
    if (e.pos>=32 && e.pos<64) {}
    if (e.pos<76 && e.pos>=72) {
      selectedLoop = e.pos-72;
    }
  }
};

#endif

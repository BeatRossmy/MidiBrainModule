#ifndef SIMPLESEQ_H
#define SIMPLESEQ_H

const int8_t timeToPos [] = {
    56,57,58,59,60,61,62,63,
    48,49,50,51,52,53,54,55,
    40,41,42,43,44,45,46,47,
    32,33,34,35,36,37,38,39,
    24,25,26,27,28,29,30,31,
    16,17,18,19,20,21,22,23,
     8, 9,10,11,12,13,14,15,
     0, 1, 2, 3, 4, 5, 6, 7
};

class USB_SENDER : public KEYBOARD_LISTENER {
public:
  USB_SENDER () {}
  void keyboardNoteMessage (MIDI_NOTE_MESSAGE msg) {
    if (msg.type == NOTE_ON) usbMIDI.sendNoteOn(msg.pitch,msg.velocity,1);
    else if (msg.type == NOTE_OFF) usbMIDI.sendNoteOff(msg.pitch,msg.velocity,1);
  }
};

class STEP_SEQ : public MACHINE {

  STACK<int8_t,12,false> stepStack;
  bool editFlag = false;
  STACK<MIDI_NOTE_MESSAGE,4,false> pattern [32];

public:
  STEP_SEQ (int id, CLOCK* c, KEYBOARD* k, LP_X* l) : MACHINE(id,c,k,l) {}

  void clock_event (CLOCK_EVENT e) {
    if (e.type != TICK) return;

    pos = e.pos%32;

    for (int n=0; n<pattern[pos].size(); n++) {
      MIDI_NOTE_MESSAGE msg = pattern[pos].get(n);
      playNote({msg.pitch,msg.velocity,1,6,millis()});
    }
  }

  virtual void display () {
    lp->clear(32,32);
    for (int n=0; n<32; n++) if (pattern[n].size()>0) lp->set(timeToPos[n],AmberFull);
    for (int i=0; i<stepStack.size(); i++) lp->set(stepStack[i],48);
    lp->set(timeToPos[pos],5);
  }

  void keyboardNoteMessage (MIDI_NOTE_MESSAGE msg) {
    if (stepStack.size()>0 && msg.type == NOTE_ON) {
      for (int i=0; i<stepStack.size(); i++) {
        pattern[timeToPos[stepStack[i]]].addOrRemove(msg);
        editFlag = true;
      }
    }
    if (!rec) return;
    int p = clock->getPos(this)%32;
    if (msg.type == NOTE_ON) pattern[p].add(msg);
  }

  void grid_event (GRID_EVENT e) {
    if (e.pos>=32 && e.pos<64) {
      STACK<MIDI_NOTE_MESSAGE,12,false>* noteStack = keys->getNotes();
      if (noteStack->size()>0 &&  e.type == PRE) {
        pattern[timeToPos[e.pos]].clear();
        for (int i=0; i<noteStack->size(); i++) {
          pattern[timeToPos[e.pos]].add(noteStack->get(i));
        }
      }
      else if (noteStack->size()==0 && e.type == PRE) {
        if (stepStack.size()==0) editFlag = false;
        stepStack.add(e.pos);
        //keys->setExtNoteStack(&pattern[timeToPos[e.pos]]);
      }
      else if (noteStack->size()==0 && e.type == REL) {
        stepStack.remove(e.pos);
        //keys->setExtNoteStack(&activeNotes);
        if (!editFlag) pattern[timeToPos[e.pos]].clear();
      }
    }
  }
};

#endif

#ifndef KEYBOARD_H
#define KEYBOARD_H

enum MESSAGE_TYPE {NOTE_ON,NOTE_OFF};

struct MIDI_NOTE_MESSAGE {
  MESSAGE_TYPE type;
  int8_t pitch;
  int8_t velocity;

  bool operator==(MIDI_NOTE_MESSAGE b) {
     return (pitch == b.pitch) && (type == b.type);
  }
  
  void print () {
    Serial.print("{");
    Serial.print(type);
    Serial.print(", ");
    Serial.print(pitch);
    Serial.print(", ");
    Serial.print(velocity);
    Serial.println("}");
  }
};

class KEYBOARD_LISTENER {
  public:
  bool mute, rec;
  virtual void keyboardNoteMessage (MIDI_NOTE_MESSAGE msg) = 0;
};

class KEYBOARD : public GRID_LISTENER{
  LP_X* lp;
  int8_t offset = 60;
  int8_t rootNote;
  STACK<MIDI_NOTE_MESSAGE,12,false> noteStack;
  STACK<MIDI_NOTE_MESSAGE,4,false>* extNoteStack;
  STACK<KEYBOARD_LISTENER*,8,false> listener;
  KEYBOARD_LISTENER* currentListener = NULL;

public:
  KEYBOARD (LP_X* ptr) {
    lp = ptr;
    lp->addListener(this);
  }

  STACK<MIDI_NOTE_MESSAGE,12,false>* getNotes () {return &noteStack;}

  void addListener (KEYBOARD_LISTENER* ptr) {listener.add(ptr);}

  void setCurrentListener (KEYBOARD_LISTENER* ptr) {currentListener = ptr;}
  void setExtNoteStack (STACK<MIDI_NOTE_MESSAGE,4,false>* ptr) { extNoteStack = ptr;}

  void handleGridEvent (GRID_EVENT e) {
    int8_t pitch = posToNote(e.pos);
    MIDI_NOTE_MESSAGE msg = {NOTE_ON,pitch,e.vel};
    
    if (e.type==PRE && noteStack.size()<noteStack.capacity()) {
      noteStack.add(msg);
      for (int i=0; i<listener.size(); i++) listener[i]->keyboardNoteMessage(msg);
      if (currentListener != NULL) currentListener->keyboardNoteMessage(msg);
    }
    else if (e.type==REL && noteStack.contains(msg)>=0) {
      noteStack.remove(msg);
      msg = {NOTE_OFF,pitch,0};
      for (int i=0; i<listener.size(); i++) listener[i]->keyboardNoteMessage(msg);
      if (currentListener != NULL) currentListener->keyboardNoteMessage(msg);
    }
  }

  void endAllNotes () {
    for (int i=0; i<noteStack.size(); i++) {
      int8_t pitch = noteStack.get(i).pitch;
      MIDI_NOTE_MESSAGE msg = {NOTE_OFF,pitch,0};
      for (int i=0; i<listener.size(); i++) listener[i]->keyboardNoteMessage(msg);
      if (currentListener != NULL) currentListener->keyboardNoteMessage(msg);
    }
    noteStack.clear();
  }

  void pitchDown () {offset -= 5; endAllNotes();}
  void pitchUp () {offset += 5; endAllNotes();}

  int8_t posToNote (int8_t pos) {
    int8_t row = pos/8;
    int8_t column = pos%8;
    int8_t note = row*5 + column + offset;
    return note;
  }

  void display () {
    lp->set(76,AmberLow);
    lp->set(77,AmberLow);
    lp->set(78,(currentListener->mute)?12:13);
    lp->set(79,(currentListener->rec)?5:4);

    for (int i=0; i<32; i++) {
      bool r = false;
      MIDI_NOTE_MESSAGE msg = {NOTE_ON,posToNote(i),100};
      if (msg.pitch%12==0) r = true;
      if (noteStack.contains(msg)>=0  || (extNoteStack != NULL && extNoteStack->contains(msg)>=0)) lp->set(i,(r)?GreenFull:GreenLow);
      else lp->set(i,(r)?AmberLow:1);
    }
  }

  void kRate () {display();}

  void grid_event (GRID_EVENT e) {
    if ((e.pos>=0 && e.pos<32)) handleGridEvent(e);
    if (e.pos == 76 && e.type == PRE) pitchUp();
    if (e.pos == 77 && e.type == PRE) pitchDown();
    if (e.pos == 78 && e.type == PRE) currentListener->mute = !currentListener->mute;
    if (e.pos == 79 && e.type == PRE) currentListener->rec = !currentListener->rec;
  }
};

#endif

#ifndef POLY_SEQ_H
#define POLY_SEQ_H

template <int maxLoopLength, int maxNotes>
class LOOP {
private:
  LIST<NOTE,maxLoopLength,maxNotes> loop;
  uint16_t loopLength = maxLoopLength/2;
  bool mute = false;

public:
  void read (int pos, MACHINE * parent) {
    pos %= loopLength;
    for (int i=0; i<loop.getNumberOfElements(pos); i++) {
      NOTE n = loop.get(pos,i);
      if (!mute) parent->playNote(n);
    }
  }

  void write (int pos, NOTE n) {loop.add(n,pos);}
  void clear () {loop.clear();}
  bool isMuted () {return mute;}
  void toggleMute () {mute = !mute;}
  uint16_t getLoopLength () {return loopLength;}
  void setLoopLength (uint16_t length) {
    if (length<=maxLoopLength) loopLength = length;
    else loopLength = maxLoopLength;
  }

  void display (LP * outputDevice, int start, int length) {
    for (int i=0; i<loopLength; i++) {
      if (loop.getNumberOfElements(i)>0) {
        outputDevice->set(start+(i/(loopLength/length)),AmberLow);
        //i =
      }
    }
  }
};

class POLY_SEQ : public MACHINE {
private:
  static const int maxNotes = 300;
  static const int maxLoopLength = 256; //128

  LOOP<maxLoopLength,maxNotes> loopA;
  LOOP<maxLoopLength,maxNotes> loopB;
  LOOP<maxLoopLength,maxNotes> loopC;
  LOOP<maxLoopLength,maxNotes> loopD;

  int8_t loopSettings = -1;

  LOOP<maxLoopLength,maxNotes> * loops [4] = {&loopA,&loopB,&loopC,&loopD};
  uint8_t activeLoop = 0;
  bool rec = false;

  std::vector <NOTE> buffer = std::vector<NOTE>(8);

public:

  POLY_SEQ (int8_t id, CLOCK * c, LP * lp,  GRID * grid, KEYS * keys) : MACHINE(id,c,lp,grid,keys) {
    buffer.clear();
  }

  virtual void onTick () {
    step = clock->getPos(divisor);
    for (int l =0; l<4; l++) loops[l]->read(step,this);
  }

  virtual void display () {
    if (!visible) return;

    if (settings) {
      MACHINE::display();
      return;
    }

    if (loopSettings>=0) {
      for (int l=0; l<4; l++) {
        for (int i=0; i<16; i++) outputDevice->set(i+16*l,(i*16<loops[l]->getLoopLength())?AmberLow:Off);
      }
      return;
    }

    for (int i=0; i<4; i++) outputDevice->set(68+i,Off);

    for (int l=0; l<4; l++) {
      for (int i=0; i<16; i++) outputDevice->set(i+16*l,(rec&&l==activeLoop)?RedLow:Off);
      loops[l]->display(outputDevice,l*16,16);
      //for (int i=0; i<16; i++) outputDevice->set(i+16*l,(i==(step%loops[l]->getLoopLength())/(loops[l]->getLoopLength()/16))?AmberLow:Off);
      for (int i=0; i<16; i++) if (i==(step%loops[l]->getLoopLength())/(loops[l]->getLoopLength()/16)) outputDevice->set(i+16*l,GreenFull);
      outputDevice->set(72+2*l,(loops[l]->isMuted())?AmberLow:AmberFull);
      if (l==activeLoop) outputDevice->set(73+2*l,(rec)?RedFull:GreenLow);
      else outputDevice->set(73+2*l,RedLow);
    }
  }










  /*
  virtual void matrix_ShortPress (int8_t id, int8_t pos) {}

  virtual void matrix_Press (int8_t id, int8_t pos) {
    if (!visible) return;
    if (loopSettings==-1) return;
    int target = pos/16;
    if (loopSettings==0) loops[target]->setLoopLength(((pos%16)+1)*16);
    if (loopSettings==1) {}
    if (loopSettings==2) {}
    if (loopSettings==3) {}
  }
  virtual void matrix_Release (int8_t id, int8_t pos) {}

  virtual void top_Hold (int8_t id, int8_t pos) {
    if (!visible) return;
    if (pos>67 && pos<72) loopSettings = pos-68;
  }
  virtual void top_Release (int8_t id, int8_t pos) {
    if (!visible) return;
    if (pos>67 && pos<72) loopSettings = -1;
  }

  virtual void side_Press (int8_t id, int8_t pos) {
    if (!visible) return;
    for (int l =0; l<4; l++) if (pos==72+2*l) loops[l]->toggleMute();
    if (pos==73 || pos==75 || pos==77 || pos==79) {
      if (rec) rec = !rec;
      activeLoop = (pos-73)/2;
      //buffer.clear();
    }
  }
  virtual void side_DoublePress (int8_t id, int8_t pos) {
    if (!visible) return;
    for (int l =0; l<4; l++) if (pos==73+2*l) loops[l]->clear();
  }
  virtual void side_Hold (int8_t id, int8_t pos) {
    if (!visible) return;
    if (pos==73 || pos==75 || pos==77 || pos==79) rec = !rec;
  }
  */

  /*----------------------------------------*/
  /*----------------------------------------*/
  /*----------------------------------------*/
  /*----------------------------------------*/

  virtual void grid_event (GRID_EVENT e) {
    if (!visible) return;
    switch (e.location) {
      case MATRIX: {
        if (e.type==FIRST_PRESS) {
          if (loopSettings==-1) return;
          int target = e.id/16;
          if (loopSettings==0) loops[target]->setLoopLength(((e.id%16)+1)*16);
          if (loopSettings==1) {}
          if (loopSettings==2) {}
          if (loopSettings==3) {}
        }
        return;
      }
      case TOP: {
        if (e.type==HOLD) {
          if (e.id>67 && e.id<72) loopSettings = e.id-68;
        }
        if (e.type==RELEASE) {
          if (e.id>67 && e.id<72) loopSettings = -1;
        }
        return;
      }
      case SIDE: {
        if (e.type==FIRST_PRESS) {
          for (int l =0; l<4; l++) if (e.id==72+2*l) loops[l]->toggleMute();
          if (e.id==73 || e.id==75 || e.id==77 || e.id==79) {
            if (rec) rec = !rec;
            activeLoop = (e.id-73)/2;
          }
        }
        if (e.type==DOUBLE_PRESS) {
          for (int l =0; l<4; l++) {
            if (e.id==73+2*l) {
              loops[l]->clear();
            }
          }
        }
        if (e.type==HOLD || e.type==DOUBLE_HOLD) {
          if (e.id==73 || e.id==75 || e.id==77 || e.id==79) rec = !rec;
        }
        return;
      }
    }
  }

  /*----------------------------------------*/
  /*----------------------------------------*/
  /*----------------------------------------*/
  /*----------------------------------------*/





  virtual void keys_Press (int8_t id, int8_t pitch, int8_t velocity) {
    if (!visible) return;
    if (buffer.size()<buffer.capacity()) {
      sendNoteOn(pitch, velocity);
      buffer.push_back({clock->getQuantizedPos(6,2),pitch,velocity,clock->getPos(1)});
    }
  }

  virtual void keys_Release (int8_t id, int8_t pitch, int8_t velocity) {
    if (!visible) return;
    sendNoteOff(pitch, velocity);
    int index = -1;
    NOTE note;
    for (int i=0; i<buffer.size(); i++) {
      if (buffer[i].pitch == pitch) {
        index = i;
        note = buffer[index];
        i = buffer.size();
      }
    }
    note.timestamp = note.timestamp%loops[activeLoop]->getLoopLength();
    note.duration = clock->getPos(1)-note.duration;
    if (rec) loops[activeLoop]->write(note.timestamp,note);
    buffer.erase(buffer.begin()+index,buffer.begin()+index+1);
  }
};

#endif

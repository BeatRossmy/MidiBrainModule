#ifndef GRID_H
#define GRID_H

enum GRID_EVENT_TYPE {PRE,REL};
struct GRID_EVENT {
  int8_t pos;
  GRID_EVENT_TYPE type;
  int8_t vel;
  uint32_t time;
  bool evaluated = false;

  void clear () {pos=0; type=REL; time=0;}
  bool olderThan (uint32_t t) {return ((millis()-time)>t);}
  static uint32_t isPress (GRID_EVENT a, GRID_EVENT b) {
    if (a.pos!=b.pos || a.type==REL || b.type==PRE) return 0x7FFFFFFF;
    else return b.time-a.time;
  }
  static uint32_t timeBetweenPress (GRID_EVENT a, GRID_EVENT b) {
    if (a.pos!=b.pos || a.type==PRE || b.type==REL) return 0x7FFFFFFF;
    else return b.time-a.time;
  }
  bool operator==(GRID_EVENT a) const{
    return (a.pos==pos && a.time==time && a.type==type);
  }
  void print () {
    Serial.print("{");
    Serial.print(pos);
    Serial.print(", ");
    Serial.print((type==REL)?"REL":"PRE");
    Serial.print(", ");
    Serial.print(time);
    Serial.print("}");
    Serial.println("");
  }
};

class GRID_LISTENER {
public:
  virtual void grid_event (GRID_EVENT e) = 0;
  //virtual void kRate () = 0;
};

class GRID {
protected:
  int8_t id;

  STACK<GRID_LISTENER*,128,false> listener;

public:
  GRID (int8_t i) : id(i){}
  void addListener (GRID_LISTENER * ptr) {listener.add(ptr);}

  virtual void handleMIDI (int8_t device, uint8_t type, uint8_t data1, uint8_t data2, uint8_t channel) {
    if (device!=id) return;

    GRID_EVENT e = {0,PRE,100,millis()};

    if (type==144) e.pos = lpToPos[data1];
    else if (type==128) {
      e.pos = lpToPos[data1];
      e.type = REL;
    }
    else if (type==176) {
      e.pos = data1-104+64;
      e.type = (data2==127)?PRE:REL;
    }

    for(uint8_t i = 0; i < listener.size(); i++) listener[i]->grid_event(e);
  }

  void kRate () {
    //for(uint8_t i = 0; i < listener.size(); i++) listener[i]->kRate();
  }

};

#endif

#ifndef LP_X_H
#define LP_X_H

static const int8_t lpToPos [128] = {
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, // 11
    0, 1, 2, 3, 4, 5, 6, 7, 79, 127,                       // 10
    8, 9, 10, 11, 12, 13, 14, 15, 78, 127,                 // 10
    16, 17, 18, 19, 20, 21, 22, 23, 77, 127,               // 10
    24, 25, 26, 27, 28, 29, 30, 31, 76, 127,               // 10
    32, 33, 34, 35, 36, 37, 38, 39, 75, 127,               // 10
    40, 41, 42, 43, 44, 45, 46, 47, 74, 127,               // 10
    48, 49, 50, 51, 52, 53, 54, 55, 73, 127,               // 10
    56, 57, 58, 59, 60, 61, 62, 63, 72, 127,               // 10
    64, 65, 66, 67, 68, 69, 70, 71, 127, 127,              // 10
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127,      // 10
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127,      // 10
    127, 127, 127, 127, 127, 127                           // 6
};

static const int8_t posToLp [80] = {
    11,12,13,14,15,16,17,18,
    21,22,23,24,25,26,27,28,
    31,32,33,34,35,36,37,38,
    41,42,43,44,45,46,47,48,
    51,52,53,54,55,56,57,58,
    61,62,63,64,65,66,67,68,
    71,72,73,74,75,76,77,78,
    81,82,83,84,85,86,87,88,
    91,92,93,94,95,96,97,98,
    89,79,69,59,49,39,29,19
};

//enum MIDI {ON=127, OFF=0};

enum COLOR {
  Off       = 12,
  RedLow    = 13, RedMedium = 14, RedFull = 15,
  AmberLow  = 29, AmberFull = 63,
  YellowLow = 62,
  GreenLow  = 28, GreenFull = 60
};

enum GRID_EVENT_TYPE {PRE,REL,AFTER};

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
  bool listensToGrid = true;
  virtual void grid_event (GRID_EVENT e) = 0;
};

class LP_X {
private:
  int8_t pixel [80];
  int8_t buffer [80];
  bool change [80];
  const int8_t length = 80;
  int8_t ptr = 0;
  
  const int16_t device;
  
  STACK<GRID_LISTENER*,128,false> listener;

  void next () {ptr = (ptr+1)%length;}

  bool hasChanged (int8_t p) {
    if (change[p]){
      change[p] = false;
      return true;
    }
    return false;
  }

public:
  LP_X (int16_t d) : device(d) {
    for (int8_t i=0; i<length; i++) {
      pixel[i] = 0;
      buffer[i] = 0;
      change[i] = true;
    }
  }
  
  void addListener (GRID_LISTENER * l) {listener.add(l);}
  
  void handleMIDI (int16_t dev, uint8_t type, uint8_t data1, uint8_t data2, uint8_t channel) {
    if (device!=dev) return;
    // Serial.print("{"); Serial.print(type); Serial.print(","); Serial.print(data1); Serial.print(","); Serial.print(data2); Serial.print(","); Serial.print(channel); Serial.print(","); Serial.println("}");
    GRID_EVENT e = {0,PRE,data2,millis()};

    if (type==144) e.pos = lpToPos[data1];
    else if (type==128) {
      e.pos = lpToPos[data1];
      e.type = REL;
    }
    else if (type==176) {
      e.pos = lpToPos[data1];
      e.type = (data2==127)?PRE:REL;
    }
    
    if (type==160) return; // AFTERTOUCH

    for(uint8_t i = 0; i < listener.size(); i++) listener[i]->grid_event(e);
  }
  
  void callListener () {
    //for(uint8_t i = 0; i < listener.size(); i++) listener[i]->kRate();
  }
  
  void updateChangedPixel (int8_t amount) {
    swap();
    for (int8_t c=0; c<amount; c++) {
      next();
      if (hasChanged(ptr)) {
        uint8_t msg2 [] = {240, 0, 32, 41, 2, 12, 3, 0, posToLp[ptr], pixel[ptr], 247};
        midilist[0]->sendSysEx(11,msg2,true); // TODO
        return;
      }
    }
  }

  void kRate () {
    updateChangedPixel(20);
    // callListener();
  }

  void set (int8_t pos, int8_t color) {
    if (pos<0 || pos>=length) return;
    buffer[pos] = color;
  }

  void swap () {
    for (int8_t p=0; p<length; p++) {
      if (buffer[p]!=pixel[p]) {
        pixel[p] = buffer[p];
        change[p] = true;
      }
    }
  }

  void clear (int8_t start, int8_t area) {
    for (int8_t p = start; p<start+area; p++) set(p,0);
  }

  void clear (int8_t start, int8_t area, bool c) {
    for (int8_t p = start; p<start+area; p++) {
      set(p,Off);
      change[p] = c;
    }
  }
  
  void sendClock (byte type) {
    midilist[0]->sendRealTime(type); // TODO
  }
  
  /*static bool isSide (int8_t p) {return (p>=72 && p<80);}
  static bool isTop (int8_t p) {return (p>=64 && p<72);}
  static bool isMatrix (int8_t p) {return (p>=0 && p<64);}
  static bool isTopHalf (int8_t p) {return (p>=0 && p<32);}
  static bool isBottomHalf (int8_t p) {return (p>=32 && p<64);}*/
};

#endif

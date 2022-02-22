#ifndef CLOCK_H
#define CLOCK_H

enum CLOCK_EVENT_TYPE {TICK,PLAY,PAUSE,STOP,PROCEED};

struct CLOCK_EVENT {
  CLOCK_EVENT_TYPE type;
  int32_t pos;
};

class CLOCK_LISTENER {
public:
  int8_t divisor = 6;
  bool quantize = true;
  int32_t pos;
  
  CLOCK_LISTENER () {}
  CLOCK_LISTENER (int8_t d) : divisor(d) {}

  virtual void clock_event (CLOCK_EVENT e) = 0;
};

class CLOCK {
private:
  bool play_flag = false;
  int32_t pos = -1;
  STACK<CLOCK_LISTENER*,128,false>  listener;

public:
  CLOCK () {}

  void addListener (CLOCK_LISTENER * ptr) {listener.add(ptr);}

  int32_t getNewPos (int32_t last, int32_t divisor) {return (divisor==0 || last==pos/divisor)?-1:getPos(divisor);}

  int32_t getPos (int32_t divisor) {return pos/divisor;}
  int32_t getPos () {return pos;}
  int32_t getPos (CLOCK_LISTENER * ptr) {
    return (ptr->quantize)?(((pos%ptr->divisor>=4)?pos+1:pos)/ptr->divisor):(pos/ptr->divisor);
  }

  int32_t getQuantizedPos (int32_t divisor, int32_t q) {return getQuantizedPos(divisor, q, pos);}

  int32_t getQuantizedPos (int32_t divisor, int32_t q, int32_t timestamp) {
    q %= divisor;
    return (timestamp+q)/divisor;
  }

  void tick () {
    pos++;
    if (pos%24==0) led_in.setOn();
    if (!play_flag) return;
    for(uint8_t i = 0; i < listener.size(); i++) if (pos%listener[i]->divisor == 0) listener[i]->clock_event({TICK,pos/listener[i]->divisor});

  }

  void play () {
    pos = -1;
    play_flag = true;
    for(uint8_t i = 0; i < listener.size(); i++) listener[i]->clock_event({PLAY,pos});
  }

  /*void proceed () {
    play_flag = true;
    for(uint8_t i = 0; i < listener.size(); i++) listener[i]->clock_event({PROCEED,pos});
  }*/

  void stop () {
    play_flag = false;
    for(uint8_t i = 0; i < listener.size(); i++) listener[i]->clock_event({STOP,pos});
  }

  void reset () {
    pos = -1;
  }

  void handleMIDI (byte realtimebyte) {
    if (realtimebyte == 248) tick();
    if (realtimebyte == 250 || realtimebyte == 251) play();
    if (realtimebyte == 252) stop();
  }
};

#endif

#ifndef LED_H
#define LED_H

#define DEFAULT_DURATION 500

class LED {
private:
  int8_t pin = 13;
  int32_t counter = DEFAULT_DURATION;
  int32_t duration = DEFAULT_DURATION;

public:
  LED () {}

  LED (int8_t p) : pin(p) {
    pinMode(p,OUTPUT);
  }

  void handle () {
    if (counter>0) counter--;
    analogWrite(pin,map(counter,0,duration,0,255));
  }

  void setOn (int32_t d) {
    duration = d;
    counter = d;
  }

  void setOn () {
    setOn(DEFAULT_DURATION);
  }
};

#endif

#ifndef STACK_H
#define STACK_H

template <typename T, uint16_t l, bool overwrite>

class STACK {
private:
  uint16_t s = 0;
  T stack [l];

public:
  STACK () {
    clear();
  }
  
  int contains (T element) {
    for (int i=0; i<s; i++) if (get(i)==element) return i;
    return -1;
  }

  void clear () {
    for (int i=0; i<l; i++) stack[i] = T();
    s = 0;
  }
  
  void moveElementsLeft (int from, int slots) {
    for (int i=from; i<s-slots; i++) stack[i] = stack[i+slots];
    for (int i=s-slots; i<s; i++) stack[i] = T();
    s-=slots;
    if (s<0) s=0;
  }
  
  void append (T element) {
    if (s>=l) return;
    stack[s] = element;
    s++;
  }
  
  void replace (T element, int index) {
    if (index<s) stack[index] = element;
  }

  void add (T element) {
    int index = contains(element);
    if (index>=0) return;
    if (s<l) append(element);
    else if (overwrite) {
      moveElementsLeft(0,1);
      replace(element,l-1);
    }
  }

  void remove (T element) {
    int index = contains(element);
    if (index<0) return;
    moveElementsLeft(index,1);
  }

  void addOrRemove (T element) {
    int index = contains(element);
    if (index<0) append(element);
    else moveElementsLeft(index,1);
  }

  T getRandom () {
    return (s==0)? T():get((uint16_t)random(s));
  }

  T getLast () {
    return (s==0)? T():stack[s-1];
  }

  T operator [](uint16_t i) const {return stack[i%l];}
  T & operator [](uint16_t i) {return stack[i%l];}
  T get (uint16_t i) {return stack[i%l];}
  
  int32_t size () {return s;}
  int32_t capacity () {return l;}

  void print () {
    Serial.print("{");
    for (int i=0; i<l; i++) {
      //Serial.print(stack[i]);
      stack[i].print();
      if (i!=l-1) Serial.print(",");
    }
    Serial.println("}\n---");
  }
};

#endif

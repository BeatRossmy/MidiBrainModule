template <typename T, int numberOfLabels, int max>

class LIST {
private:
  std::vector<T> list = std::vector<T>(max);
  int labels [numberOfLabels];

public:
  LIST () {
    //list.reserve(max);
    list.clear();
  }

  void add (T element, int label) {
    if (label<numberOfLabels && !containsAt(element,label) && list.size()<list.capacity()) {
      list.insert(list.begin()+labels[label],element);
      for (int i=label+1; i<numberOfLabels; i++) labels[i] = labels[i]+1;
    }
  }

  T get (int label, int index) {
    if (index<getNumberOfElements(label)) return list[labels[label]+index];
    return T();
  }

  int getNumberOfElements (int label) {
    if (label<numberOfLabels-1) return labels[label+1]-labels[label];
    return list.size()-labels[label];
  }

  void remove (T element, int label) {
    if (containsAt(element,label)) {
      list.erase(list.begin()+indexAt(element,label));
      for (int i=label+1; i<numberOfLabels; i++) labels[i] = labels[i]-1;
    }
  }

  void clear () {
    list.clear();
    for (int i=0; i<numberOfLabels; i++) labels[i] = 0;
  }

  int indexAt (T element, int label) {
    if (label<numberOfLabels) {
      for (int i=labels[label]; i<labels[label]+getNumberOfElements(label); i++) if (list[i]==element) return i;
    }
    return -1;
  }

  bool containsAt (T element, int label) {return (indexAt(element,label)>=0);}

  int size () {return list.size();}

  void print () {
    Serial.print("[");
    for (int i=0; i<list.size(); i++) {
      Serial.print(list[i]);
      if(i<list.size()-1)Serial.print(",");
    }
    Serial.println("]");
    Serial.println();
  }
};

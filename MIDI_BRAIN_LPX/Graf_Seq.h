#ifndef GRAF_SEQ_H
#define GRAF_SEQ_H

class TAPE_HEAD {
private:
  int32_t step = -1;
  int32_t nextStep = -1;
  int32_t root = 0;
  bool active = true;
public:
  TAPE_HEAD (int32_t r, bool a) : root(r), nextStep(r), active(a) {}
  void read (NOTE * notes, STACK<8> * path, MACHINE * parent) {
    //step = (step==-1)?0:path[step].getRandom();
    step = nextStep;
    nextStep = (step==-1)?0:path[step].getRandom();
    if (step==-1) step = 0;
    if (!active) return;
    if (notes[step].pitch!=-1) parent->playNote(notes[step]);
  }
  void display (LP * outputDevice) {
    if (!active) return;
    outputDevice->set((step>-1)?step:0,AmberFull);
  }
  bool isActive () {return active;}
  void reset () {nextStep = root;}
  void toggle () {active = !active;}
  void setNext (int32_t next) {nextStep = next%64;}
};

class GRAF_SEQ : public MACHINE {
private:
  NOTE notes [64];
  //STACK<8> edges [64];
  STACK<8> edges [4][64];
  int path = 0;

  TAPE_HEAD head1 = TAPE_HEAD(0,true);
  TAPE_HEAD head2 = TAPE_HEAD(16,false);
  TAPE_HEAD head3 = TAPE_HEAD(32,false);
  TAPE_HEAD head4 = TAPE_HEAD(48,false);

  TAPE_HEAD * heads [4] = {&head1,&head2,&head3,&head4};

public:
  GRAF_SEQ (int8_t id, CLOCK * c, LP * lp,  GRID * grid, KEYS * keys) : MACHINE(id,c,lp,grid,keys) {
    //for (int i=0; i<64; i++) edges[i].add((i+1)%64);
    for (int i=0; i<64; i++) edges[0][i].add((i+1)%64);
    for (int i=0; i<64; i++) {
      int x = i%8;
      int y = i/8;
      if (x<7) edges[1][i].add(i+1);
      if (x>0) edges[1][i].add(i-1);
      if (y<7) edges[1][i].add(i+8);
      if (y>0) edges[1][i].add(i-8);
      if (x<7 && y<7) edges[1][i].add(i+9);
      if (x<7 && y>0)edges[1][i].add(i-7);
      if (x>0 && y<7) edges[1][i].add(i+7);
      if (x>0 && y>0) edges[1][i].add(i-9);
    }
    for (int i=0; i<64; i++) edges[2][i].add((i+1)%64);
    for (int i=0; i<64; i++) edges[3][i].add((i+1)%64);
  }

  virtual void onTick () {
    //step = (step==-1)?0:edges[step].getRandom();

    //step = (step==-1)?0:edges[path][step].getRandom();
    //if (step==-1) step = 0;
    //if (notes[step].pitch!=-1) playNote(notes[step]);

    for (int h=0; h<4; h++) {
      heads[h]->read(&notes[0],&edges[path][0],this);
    }
  }

  virtual void display () {
    if (!visible) return;

    if (settings) {
      MACHINE::display();
      return;
    }

    for (int i=0; i<64; i++) {
      outputDevice->set(i,(notes[i].pitch!=-1)?AmberLow:Off);
    }
    for (int i=0; i<4; i++) outputDevice->set(68+i,(i==path)?AmberFull:AmberLow);
    for (int i=0; i<8; i++) outputDevice->set(72+i,Off);
    int8_t m_sel = grid->getMatrixSelection();
    if(m_sel>=0) {
      //for (int i=0; i<edges[m_sel].size(); i++) outputDevice->set(edges[m_sel][i],GreenLow);
      for (int i=0; i<edges[path][m_sel].size(); i++) outputDevice->set(edges[path][m_sel][i],GreenLow);
    }
    //outputDevice->set((step>-1)?step:0,AmberFull);

    for (int h=0; h<4; h++) {
      outputDevice->set(72+h,(heads[h]->isActive())?AmberFull:AmberLow);
      heads[h]->display(outputDevice);
    }
  }









  /*
  virtual void top_Press (int8_t id, int8_t pos) {
    if (!visible) return;
    if (pos>67 && pos<72) path = pos-68;
  }

  virtual void matrix_ShortPress (int8_t id, int8_t pos) {
    if (!visible || grid->getSideSelection()>=72) return;
    if (!keys->isPressed()) notes[pos] = NOTE{};
  }

  virtual void matrix_Press (int8_t id, int8_t pos) {
    if (!visible) return;
    int8_t sel = grid->getSideSelection();
    if (sel>=72 && sel<76) {
      heads[sel-72]->setNext(pos);
      return;
    }
    if (keys->isPressed()) notes[pos] = NOTE{-1,keys->getSelectedKey(),100,5};
    int8_t m_sel = grid->getMatrixSelection();
    //if (m_sel>=0 && pos!=m_sel) edges[m_sel].addOrRemove(pos);
    if (m_sel>=0 && pos!=m_sel) edges[path][m_sel].addOrRemove(pos);
  }

  virtual void matrix_Release (int8_t id, int8_t pos) {}

  virtual void side_ShortPress (int8_t id, int8_t pos) {
    if (!visible) return;
    if (pos>=72 && pos<76) heads[pos-72]->toggle();
  }
  */

  virtual void keys_Press (int8_t id, int8_t pitch, int8_t velocity) {
    if (!visible) return;
    int8_t m_sel = grid->getMatrixSelection();
    if (m_sel>=0) notes[m_sel] = NOTE{-1,pitch,velocity,5};
  }
  virtual void keys_Release (int8_t id, int8_t pitch, int8_t velocity) {
    if (!visible) return;
  }

  /*----------------------------------------*/
  /*----------------------------------------*/
  /*----------------------------------------*/
  /*----------------------------------------*/

  virtual void grid_event (GRID_EVENT e) {
    if (!visible) return;
    switch (e.location) {
      case MATRIX:
        if (e.type==SHORT_PRESS && !keys->isPressed() && grid->getSideSelection()==-1 ) notes[e.id] = NOTE{};
        else if (e.type==FIRST_PRESS) {
          int8_t sel = grid->getSideSelection();
          if (sel>=72 && sel<76) {
            heads[sel-72]->setNext(e.id);
            return;
          }
          if (keys->isPressed()) notes[e.id] = NOTE{-1,keys->getSelectedKey(),100,5};
        }
        else if (e.type==SECOND_PRESS) {
          int8_t m_sel = grid->getMatrixSelection();
          if (m_sel>=0 && e.id!=m_sel) edges[path][m_sel].addOrRemove(e.id);
        }
        break;
      case TOP:
        if (e.type==FIRST_PRESS && e.id>67 && e.id<72) path = e.id-68;
        break;
      case SIDE:
        if (e.type==SHORT_PRESS && e.id>=72 && e.id<76) heads[e.id-72]->toggle();
        break;
    }
  }
};

#endif

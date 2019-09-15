#ifndef PUSH_SWHITCH_H
#define PUSH_SWHITCH_H

class PushSwitch {
 public:
  PushSwitch();
  ~PushSwitch() {}

  void compute();
  bool getState() const;  // filtered
  bool detectEdgeRise() const;
  bool detectEdgeFall() const;
  void setPortNum(const int port);

 private:
  bool detectSwitchEdge(const bool sw_input);
  void filterSwitchInput();

  bool sw_state_;
  bool pre_sw_state_;
  int port_;
  bool port_is_set_;
  
};


#endif

#ifndef TRUN_OUT_DRIVER_H
#define TRUN_OUT_DRIVER_H

#include "constant_value.h"

class TurnOutDriver {
 public:
  TurnOutDriver();
  ~TurnOutDriver() {}
  void setPortNum(const int port_p, const int port_n);
  void compute();
  void setStatus(const TurnOutState turn_out_status);
  void requestForceDrive();
  TurnOutState getState() {return turn_out_status_;}

 private:
  void drive();
  void nodrive();

  bool port_is_set_;
  int port_p_;
  int port_n_;
  TurnOutState turn_out_status_;
  bool detect_edge_;
  unsigned long start_time_msec_;

};

#endif

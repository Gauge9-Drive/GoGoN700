#include "push_switch.h"

PushSwitch::PushSwitch() {
  sw_state_ = false;
  pre_sw_state_ = false;
  port_ = false;
  port_is_set_ = false;

}

void PushSwitch::compute() {
  if(port_is_set_) {
    pre_sw_state_ = sw_state_;
    filterSwitchInput();
  } else {
    sw_state_ = false;
    pre_sw_state_ = false;
  }
}

bool PushSwitch::getState() const {
  return sw_state_;
}

bool PushSwitch::detectEdgeRise() const {
  return (!pre_sw_state_ && sw_state_);
}

bool PushSwitch::detectEdgeFall() const {
  return (pre_sw_state_ && !sw_state_);
}

void PushSwitch::filterSwitchInput() {
  static unsigned long msec_at_last_edge = 0UL;
  static bool pre_switch_input = false;
  const bool switch_input = (digitalRead(port_) == HIGH);
  
  if(pre_switch_input != switch_input) {
    msec_at_last_edge = millis();
  } else if(millis() > msec_at_last_edge + kSwitchFiltTimeMsec) {
    sw_state_ = switch_input;
  } else {
    // do nothing
  }
  pre_switch_input = switch_input;
}

void PushSwitch::setPortNum(const int port) {
  port_ = port;
  port_is_set_ = true;
  pinMode(port_, INPUT);
}

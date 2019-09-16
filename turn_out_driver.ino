#include "turn_out_driver.h"

TurnOutDriver::TurnOutDriver() {
  port_is_set_ = false;
  turn_out_status_ = kTurnOutStraight;
  detect_edge_ = false;
  start_time_msec_ = 0UL;
}

void TurnOutDriver::drive() {
  if(turn_out_status_ == kTurnOutStraight) {
    digitalWrite(port_p_, HIGH);
    digitalWrite(port_n_, LOW);
  } else if(turn_out_status_ == kTurnOutCurve) {
    digitalWrite(port_p_, LOW);
    digitalWrite(port_n_, HIGH);
  } else {
    digitalWrite(port_p_, LOW);
    digitalWrite(port_n_, LOW);
  }
}

void TurnOutDriver::nodrive() {
  digitalWrite(port_p_, LOW);
  digitalWrite(port_n_, LOW);
}

void TurnOutDriver::setPortNum(const int port_p, const int port_n) {
  port_p_ = port_p;
  port_n_ = port_n;
  pinMode(port_p_, OUTPUT);
  pinMode(port_n_, OUTPUT);
  port_is_set_ = true;
}

void TurnOutDriver::compute() {
  if(detect_edge_ && (start_time_msec_ + kTurnOutDriveIntervalMsec <= millis())) {
    start_time_msec_ = millis();
    detect_edge_ = false;
  }
  if(port_is_set_) {
    if(start_time_msec_ + kTurnOutDriveDurationMsec > millis()) {
      drive();
    } else {
      nodrive();
    }
  }
}

void TurnOutDriver::setStatus(const TurnOutState turn_out_status) {
  detect_edge_ = (turn_out_status_ != turn_out_status);
  turn_out_status_ = turn_out_status;
}

void TurnOutDriver::requestForceDrive() {
    detect_edge_ = true;
}

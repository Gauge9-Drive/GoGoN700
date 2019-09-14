#include "motor_driver.h"

MotorDriver::MotorDriver() {
  port_is_set_ = false;
  voltage_ = 0;
}

MotorDriver::~MotorDriver() {
  digitalWrite(port_a_, LOW);
  digitalWrite(port_b_, LOW);
  digitalWrite(port_en_, LOW);
}

void MotorDriver::setPortNum(const int port_a, const int port_b, const int port_en) {
  port_a_ = port_a;
  port_b_ = port_b;
  port_en_ = port_en;
  port_is_set_ = true;
  pinMode(port_a_, OUTPUT);
  pinMode(port_b_, OUTPUT);
  pinMode(port_en_, OUTPUT);
  digitalWrite(port_en_, HIGH);
}

void MotorDriver::compute() {
  if(port_is_set_) {
    driveMotor(voltage_);
  }
}

void MotorDriver::setDriveMode(const MotorDriveMode drive_mode) {
  switch(drive_mode) {
    case kMtDrvModeHalt :
      voltage_ = 0;
      break;
    case kMtDrvModeFwdSlow :
      voltage_ = kVoltSlow;
      break;
    case kMtDrvModeFwdFast :
      voltage_ = kVoltFast;
      break;
    case kMtDrvModeBwdSlow :
      voltage_ = -kVoltSlow;
      break;
    case kMtDrvModeBwdFast :
      voltage_ = -kVoltFast;
      break;
    default :
      voltage_ = 0;
      break;
  }
}

void MotorDriver::driveMotor(const int volt) {
  int duty_A = 0; // 0 to 255
  int duty_B = 0; // 0 to 255
  const int volt_after_smoothing = smoothVolt(volt);
  calculateDutyOfPortAandB(volt_after_smoothing, &duty_A, &duty_B);
  outputToPortAandB(duty_A, duty_B);
}

int MotorDriver::smoothVolt(const int volt) {
  static int pre_volt = 0;
  int volt_range_lower, volt_range_upper;
  calcVoltRange(pre_volt, &volt_range_lower, &volt_range_upper);
  const int volt_output = limitVoltInRange(volt, volt_range_lower, volt_range_upper);
  pre_volt = volt_output;
  return volt_output;
}

void MotorDriver::calcVoltRange(const int pre_volt, int* lower_out, int* upper_out) {
  int lower = 0;
  int upper = 0;
  if(pre_volt == 0) {
    upper = kVoltRiseRate100ms;
    lower = -kVoltRiseRate100ms;
  } else if(pre_volt > 0) {
    upper = pre_volt + kVoltRiseRate100ms;
    if(upper > kMaxVolt) upper = kMaxVolt;
    lower = pre_volt - kVoltFallRate100ms;
    if(lower < 0) lower = 0;
  } else { // pre_volt < 0
    upper = pre_volt + kVoltFallRate100ms;
    if(upper > 0) upper = 0;
    lower = pre_volt - kVoltRiseRate100ms;
    if(lower < kMinVolt) lower = kMinVolt;
  }
  *lower_out = lower;
  *upper_out = upper;
}

int MotorDriver::limitVoltInRange(const int volt, const int lower, const int upper) {
  int volt_output = 0;
  if(volt > upper) {
    volt_output = upper;
  } else if(volt < lower) {
    volt_output = lower;
  } else {
    volt_output = volt;
  }
  return volt_output;
}

void MotorDriver::outputToPortAandB(const int duty_A, const int duty_B) {
  outputAnalogPort(port_a_, duty_A);
  outputAnalogPort(port_b_, duty_B);
}

void MotorDriver::outputAnalogPort(const int port_number, const int duty) {
  if(duty == 0) {
    digitalWrite(port_number, LOW);
  } else if(duty >= kMaxValOfAnalogWrite) {
    digitalWrite(port_number, HIGH);
  } else {
    analogWrite(port_number, duty);
  }
}

void MotorDriver::calculateDutyOfPortAandB(const int volt, int* duty_A, int* duty_B) {
  if(volt == 0) {
    *duty_A = 0;
    *duty_B = 0;
  } else if(volt > 0) {
    *duty_A = (volt<kMaxVolt)?volt:kMaxVolt;
    *duty_B = 0;
  } else {
    *duty_A = 0;
    *duty_B = (volt>kMinVolt)?(-volt):(-kMinVolt);
  }
}

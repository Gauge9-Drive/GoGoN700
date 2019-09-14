#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H

#include "constant_value.h"

class MotorDriver {
 public:
  MotorDriver();
  ~MotorDriver();

  void setPortNum(const int port_a, const int port_b, const int port_en);
  void compute();
  void setDriveMode(const MotorDriveMode drive_mode);
  int getVoltage() {return voltage_;}

 private:
  void driveMotor(const int volt);
  int limitVoltInRange(const int volt, const int lower, const int upper);
  void calcVoltRange(const int pre_volt, int* lower_out, int* upper_out);
  int smoothVolt(const int volt);
  void calculateDutyOfPortAandB(const int volt, int* duty_A, int* duty_B);
  void outputToPortAandB(const int duty_A, const int duty_B);
  void outputAnalogPort(const int port_number, const int duty);

  bool port_is_set_;
  int port_a_;
  int port_b_;
  int port_en_;
  int voltage_;
};
#endif

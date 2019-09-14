#ifndef IO_DEVICE_DEF_H
#define IO_DEVICE_DEF_H

struct InputDevices {
  PhotoInterrupterLedDriver photo_int_led;
  PhotoInterrupter photo_int_1;
  PhotoInterrupter photo_int_2;
  bool sw_status;
  bool sw_edge;
};

struct OutputDevices {
//  TurnOutDriver turn_out_driver_1;
//  MotorDriver motor_driver_1;
  int val;
};

#endif

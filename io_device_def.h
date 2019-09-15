#ifndef IO_DEVICE_DEF_H
#define IO_DEVICE_DEF_H

const int kPortPushSwitch = 2;
const int kPortPhotoIntLed = 7; // drive IR-LED
const int kPortPhotoInt01 = 0; // analog
const int kPortPhotoInt02 = 1; // analog

class InputDevices {  // TODO クラスにして、駆動処理をメソッドにする
 public:
  InputDevices() {}
  ~InputDevices() {}
  void initialize() {
    push_sw_1.setPortNum(kPortPushSwitch);
    photo_int_led.setPortNum(kPortPhotoIntLed);
    photo_int_1.setPortNum(kPortPhotoInt01);
    photo_int_1.setThresholdHigh(kThreasholdOfPhotoInt);
    photo_int_1.setThresholdLow(kThreasholdOfPhotoInt - 200UL);
    photo_int_1.setHoldTime(1000UL);
    photo_int_2.setPortNum(kPortPhotoInt02);
    photo_int_2.setThresholdHigh(kThreasholdOfPhotoInt);
    photo_int_2.setThresholdLow(kThreasholdOfPhotoInt - 200UL);
    photo_int_2.setHoldTime(1000UL);
  }
  void compute() {
    photo_int_led.compute();
    photo_int_1.compute();
    photo_int_2.compute();
    push_sw_1.compute();
  }
  PhotoInterrupterLedDriver photo_int_led;
  PhotoInterrupter photo_int_1;
  PhotoInterrupter photo_int_2;
  PushSwitch push_sw_1;
};

struct OutputDevices {
  TurnOutDriver turn_out_driver_1;
  MotorDriver motor_driver_1;
  int val;
};

#endif

#ifndef IO_DEVICE_DEF_H
#define IO_DEVICE_DEF_H

#include "constant_value.h"
#include "photo_interrupter.h"
#include "push_switch.h"
#include "turn_out_driver.h"
#include "motor_driver.h"
#include "led_control.h"

// input device
const int kPortPushSwitch = 2;
const int kPortPhotoIntLed = 7; // drive IR-LED
const int kPortPhotoInt01 = 0; // analog
const int kPortPhotoInt02 = 1; // analog

// output device
const int kPortEnableDriver = 8;
const int kPortMotorA = 9; // Use OC1A as MotorA(1A of L293)
const int kPortMotorB = 10; // OC1B - MotorB(2A)
const int kPortLed1 = 11;
const int kPortLed2 = 12;
const int kPortTurnOut1P = 4;
const int kPortTurnOut1N = 3;
const int kPortTurnOut2P = 5;
const int kPortTurnOut2N = 6;

class InputDevices {  // TODO クラスにして、駆動処理をメソッドにする
 public:
  InputDevices() {}
  ~InputDevices() {}
  void initialize() {
    push_sw_1.setPortNum(kPortPushSwitch);
    photo_int_led.setPortNum(kPortPhotoIntLed);
    photo_int_1.setPortNum(kPortPhotoInt01);
    photo_int_1.setThresholdHigh(kThresholdOfPhotoIntOn);
    photo_int_1.setThresholdLow(kThresholdOfPhotoIntOff);
    photo_int_1.setHoldTime(500UL);
    photo_int_2.setPortNum(kPortPhotoInt02);
    photo_int_2.setThresholdHigh(kThresholdOfPhotoIntOn);
    photo_int_2.setThresholdLow(kThresholdOfPhotoIntOff);
    photo_int_2.setHoldTime(500UL);
  }
  void compute() {
    photo_int_led.compute();
    photo_int_1.compute();
    photo_int_2.compute();
    push_sw_1.compute();
  }

 private:
  PhotoInterrupterLedDriver photo_int_led;
 public:
  PhotoInterrupter photo_int_1{&photo_int_led};
  PhotoInterrupter photo_int_2{&photo_int_led};
  PushSwitch push_sw_1;
};

class OutputDevices {
 public:
  OutputDevices() {}
  ~OutputDevices() {}

  void initialize() {
    turn_out_driver_1.setPortNum(kPortTurnOut1P, kPortTurnOut1N);
    motor_driver_1.setPortNum(kPortMotorA, kPortMotorB, kPortEnableDriver);
    led_1.setPortNum(kPortLed1);
    led_2.setPortNum(kPortLed2);
    led_built_in.setPortNum(LED_BUILTIN);
    led_built_in.ledFlash(100U);
    val = 0;
  }

  void control_100ms() {  // 100ms
    motor_driver_1.compute();
    turn_out_driver_1.compute();
  }

  void compute(const InputDevices& input) { // LED driver should be called every loop() cycle because LED flashes < 100ms
    led_1.setLed(input.photo_int_1.getState());
    led_2.setLed(input.photo_int_2.getState());
    led_1.compute();
    led_2.compute();
    led_built_in.compute();
  }

  TurnOutDriver turn_out_driver_1;
  MotorDriver motor_driver_1;
  LedController led_1;
  LedController led_2;
  LedController led_built_in;
  int val;
};

#endif

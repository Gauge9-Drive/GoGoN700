/* Motor Driver
 * Schematics
 * 09 --MotorA(1A)--M--(2A)MotorB-- 10
 * Gauge9-Drive / 時致
 * TODO 入出力デバイスをクラスにする
 * TODO センサのLED駆動をシングルトンにする
 * TODO ステート遷移条件の共通化
 * TODO 入力・出力クラスでそれぞれインターフェースを統一できないか
 */

#include "constant_value.h"
#include "io_device_def.h"
#include "state_machine.h"

const int kPortEnableDriver = 8;
const int kPortMotorA = 9; // Use OC1A as MotorA(1A of L293)
const int kPortMotorB = 10; // OC1B - MotorB(2A)
const int kPortLed1 = 11;
const int kPortLed2 = 12;
const int kPortTurnOut1P = 4;
const int kPortTurnOut1N = 3;
const int kPortTurnOut2P = 5;
const int kPortTurnOut2N = 6;

InputDevices input_devices;
OutputDevices output_devices;
StateManager state_manager;

void controlMotor() {
  output_devices.motor_driver_1.compute();
}

void controlTurnOut() {
  output_devices.turn_out_driver_1.compute();
}

bool start100msTasks(const unsigned long time_msec) { // check if 100ms elapsed since the last trigger
  static unsigned long pre_time_msec = 0UL;
  bool ret = false;
  if(time_msec >= pre_time_msec + 100UL) {
    ret = true;
    pre_time_msec = time_msec;
  }
  return ret;
}

// =============Setup=============

void setup() {
  Serial.begin(57600);
  Serial.println("start");

  input_devices.initialize();
  
  output_devices.turn_out_driver_1.setPortNum(kPortTurnOut1P, kPortTurnOut1N);
  output_devices.motor_driver_1.setPortNum(kPortMotorA, kPortMotorB, kPortEnableDriver);
  output_devices.led_1.setPortNum(kPortLed1);
  output_devices.led_2.setPortNum(kPortLed2);
  output_devices.led_built_in.setPortNum(LED_BUILTIN);
  output_devices.led_built_in.ledFlash(100U);
  output_devices.val = 0;

  state_manager.initialize();

}

// =============Loop=============

void loop() {
  // put your main code here, to run repeatedly:

  input_devices.compute();
  state_manager.transit(input_devices);
  output_devices.led_1.setLed(input_devices.photo_int_1.getSensorState());
  output_devices.led_1.compute();
  output_devices.led_2.setLed(input_devices.photo_int_2.getSensorState());
  output_devices.led_2.compute();
  output_devices.led_built_in.compute();
/*
  static unsigned int last_ts = 0U;
  unsigned int ts = (unsigned int)millis() % 50;
  if(last_ts != ts) {
    int v = analogRead(0);
    Serial.print(ts);
    Serial.print(" ");
    Serial.print((int)(input_devices.photo_int_led.getStatus()));
    Serial.print(" ");
    Serial.println(v);
    last_ts = ts;
  }
  */

  if(start100msTasks(millis())) { // run the control sequence every 100ms
    state_manager.execute(output_devices);

    controlTurnOut();
    controlMotor();

    Serial.print("state = ");
    Serial.print(state_manager.getStateNumber());
    Serial.print(", sw 1 = ");
    Serial.print(input_devices.push_sw_1.getState());
    Serial.print(", loop = ");
    Serial.print(output_devices.val);
    Serial.print(", LED = ");
    Serial.print((int)(input_devices.photo_int_led.getStatus()));
    Serial.print(", 1D = ");
    Serial.print(input_devices.photo_int_1.getSensorOnHold() - input_devices.photo_int_1.getSensorOffHold());
    Serial.print(", 2D = ");
    Serial.print(input_devices.photo_int_2.getSensorOnHold() - input_devices.photo_int_2.getSensorOffHold());
    Serial.print(", volt = ");
    Serial.print(output_devices.motor_driver_1.getVoltage());
    Serial.print(", turn out = ");
    Serial.println(output_devices.turn_out_driver_1.getStatus());
  }
}

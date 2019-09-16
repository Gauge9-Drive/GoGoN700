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

InputDevices input_devices;
OutputDevices output_devices;
StateManager state_manager;

bool startControlTask(const unsigned long time_msec) {
  static unsigned long pre_time_msec = 0UL;
  const unsigned long rem = pre_time_msec % kControlPeriod;
  bool ret = false;
  if(time_msec >= pre_time_msec + kControlPeriod - rem) {
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
  output_devices.initialize();
  state_manager.initialize();
}

// =============Loop=============

void loop() {
  // put your main code here, to run repeatedly:

  input_devices.compute();
  state_manager.transit(input_devices);
  output_devices.compute(input_devices);

  if(startControlTask(millis())) { // run the control sequence every 100ms
    state_manager.execute(output_devices);
    output_devices.control_100ms();

    Serial.print("state = ");
    Serial.print(state_manager.getStateNumber());
    Serial.print(", sw 1 = ");
    Serial.print(input_devices.push_sw_1.getState());
    Serial.print(", loop = ");
    Serial.print(output_devices.val);
    Serial.print(", 1D = ");
    Serial.print(input_devices.photo_int_1.getOnHold() - input_devices.photo_int_1.getOffHold());
    Serial.print(", 2D = ");
    Serial.print(input_devices.photo_int_2.getOnHold() - input_devices.photo_int_2.getOffHold());
    Serial.print(", volt = ");
    Serial.print(output_devices.motor_driver_1.getVoltage());
    Serial.print(", turn out = ");
    Serial.println(output_devices.turn_out_driver_1.getState());
  }
}

/* Motor Driver
 * Schematics
 * 09 --MotorA(1A)--M--(2A)MotorB-- 10
 * Gauge9-Drive / 時致
 * TODO スイッチをクラスにする
 * TODO 入出力デバイスをクラスにする
 * TODO タイマの復活
 * TODO センサのLED駆動をシングルトンにする
 * TODO センサの判定条件をロバストにする
 * TODO ステート遷移条件の共通化
 * TODO 入力・出力クラスでそれぞれインターフェースを統一できないか
 */

#include "constant_value.h"
#include "count_down_timer.h"
#include "photo_interrupter.h"
#include "push_switch.h"
#include "turn_out_driver.h"
#include "motor_driver.h"
#include "io_device_def.h"
#include "state_machine.h"

const int kPortEnableDriver = 8;
const int kPortMotorA = 9; // Use OC1A as MotorA(1A of L293)
const int kPortMotorB = 10; // OC1B - MotorB(2A)
const int kPortPushSwitch = 2;
const int kPortGreenLed = 11;
const int kPortRedLed = 12;
const int kPortTurnOut1P = 4;
const int kPortTurnOut1N = 3;
const int kPortTurnOut2P = 5;
const int kPortTurnOut2N = 6;
const int kPortPhotoIntLed = 7; // drive IR-LED
const int kPortPhotoInt01 = 0; // analog
const int kPortPhotoInt02 = 1; // analog

bool builtin_led_status;

InputDevices input_devices;
OutputDevices output_devices;
StateManager state_manager;

CountDownTimer timer_1;

void controlMotor() {
  output_devices.motor_driver_1.compute();
}

void controlTurnOut() {
  output_devices.turn_out_driver_1.compute();
}

void drivePhotoInterrupter() {
  input_devices.photo_int_led.compute();
  input_devices.photo_int_1.compute();
  input_devices.photo_int_2.compute();
}

void controlLed() {
  if(input_devices.photo_int_1.getSensorState()) {
    digitalWrite(kPortGreenLed, HIGH);
  } else {
    digitalWrite(kPortGreenLed, LOW);
  }
  if(input_devices.photo_int_2.getSensorState()) {
    digitalWrite(kPortRedLed, HIGH);
  } else {
    digitalWrite(kPortRedLed, LOW);
  }
  // on borad LED to blink every control cycle
  builtin_led_status = !builtin_led_status;
  digitalWrite(LED_BUILTIN, (builtin_led_status?HIGH:LOW));
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
  Serial.begin(9600);

  pinMode(kPortGreenLed, OUTPUT);
  pinMode(kPortRedLed, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(kPortGreenLed, HIGH);
  digitalWrite(kPortRedLed, LOW);

  input_devices.push_sw_1.setPortNum(kPortPushSwitch);
  input_devices.photo_int_led.setPortNum(kPortPhotoIntLed);
  input_devices.photo_int_1.setPortNum(kPortPhotoInt01);
  input_devices.photo_int_1.setThresholdHigh(kThreasholdOfPhotoInt);
  input_devices.photo_int_1.setThresholdLow(kThreasholdOfPhotoInt - 200UL);
  input_devices.photo_int_1.setHoldTime(1000UL);
  input_devices.photo_int_2.setPortNum(kPortPhotoInt02);
  input_devices.photo_int_2.setThresholdHigh(kThreasholdOfPhotoInt);
  input_devices.photo_int_2.setThresholdLow(kThreasholdOfPhotoInt - 200UL);
  input_devices.photo_int_2.setHoldTime(1000UL);
  
  output_devices.turn_out_driver_1.setPortNum(kPortTurnOut1P, kPortTurnOut1N);
  output_devices.motor_driver_1.setPortNum(kPortMotorA, kPortMotorB, kPortEnableDriver);
  output_devices.val = 0;

  state_manager.initialize();
  builtin_led_status = true;

  Serial.println("start");
}

// =============Loop=============

void loop() {
  // put your main code here, to run repeatedly:
  drivePhotoInterrupter();
  input_devices.push_sw_1.compute();

  state_manager.transit(input_devices);

  if(start100msTasks(millis())) { // run the control sequence every 100ms
    state_manager.execute(output_devices);

    controlTurnOut();
    controlMotor();
    controlLed();
    Serial.print("state = ");
    Serial.print(state_manager.getStateNumber());
    Serial.print(", sw 1 = ");
    Serial.print(input_devices.push_sw_1.getState());
    Serial.print(", loop = ");
    Serial.print(output_devices.val);
    Serial.print(", LED = ");
    Serial.print((int)(input_devices.photo_int_led.getStatus()));
    Serial.print(", sensor 1 = ");
    Serial.print(input_devices.photo_int_1.getSensorRaw());
    Serial.print(", sensor 2 = ");
    Serial.print(input_devices.photo_int_2.getSensorRaw());
    Serial.print(", volt = ");
    Serial.print(output_devices.motor_driver_1.getVoltage());
    Serial.print(", turn out = ");
    Serial.println(output_devices.turn_out_driver_1.getStatus());
  }
}

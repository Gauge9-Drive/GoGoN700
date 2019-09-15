#include "led_control.h"

LedController::LedController() {
  led_is_on_ = false;
  flash_period_ = 0U;
  port_is_set_ = false;
}

void LedController::setPortNum(const int port) {
  port_ = port;
  port_is_set_ = true;
  pinMode(port_, OUTPUT);
  digitalWrite(port_, LOW);
}

void LedController::ledOn() {
  led_is_on_ = true;
  flash_period_ = 0U;
}

void LedController::ledOff() {
  led_is_on_ = false;
  flash_period_ = 0U;
}

void LedController::setLed(const bool val) {
  if(val) {
    ledOn();
  } else {
    ledOff();
  }
}

void LedController::ledFlash(const unsigned int period) {
  led_is_on_ = true;
  flash_period_ = period;
}

void LedController::compute() {
  if(port_is_set_) {
    bool led_status = false;
    if(led_is_on_ && flash_period_ == 0U) {
      led_status = true;
    } else if(led_is_on_ && flash_period_ > 0U) {
      const unsigned int ts = (unsigned int)millis();
      led_status = ((ts % flash_period_) >= (flash_period_ / 2U));
    } else {
      led_status = false;
    }
    if(led_status) {
      digitalWrite(port_, HIGH);
    } else {
      digitalWrite(port_, LOW);
    }
  }
}

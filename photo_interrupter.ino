#include "photo_interrupter.h"


PhotoInterrupterLedDriver::PhotoInterrupterLedDriver() {
  status_ = kTransitPeriod;
  port_is_set_ = false;
  t1_ = kLedRiseTime;
  t2_ = kLedRiseTime + kLedOnDuration;
  t3_ = kLedRiseTime + kLedOnDuration + kLedFallTime;
}

void PhotoInterrupterLedDriver::compute() {
  const unsigned int count = ((unsigned int)millis() % kLedCycleTime);

  if (port_is_set_ == false) {
    status_ = kTransitPeriod;
  } else if (count < t1_) {
    status_ = kTransitPeriod;
    digitalWrite(port_, HIGH);
  } else if (count < t2_) {
    status_ = kLedIsOn;
  } else if (count < t3_) {
    status_ = kTransitPeriod;
    digitalWrite(port_, LOW);
  } else {
    status_ = kLedIsOff;
  }
}

PhotoInterrupterLedDriver::PhotoIntLedStatus PhotoInterrupterLedDriver::getState() const {
  return status_;
}

void PhotoInterrupterLedDriver::setPortNum(int port) {
  port_ = port;
  pinMode(port_, OUTPUT);
  port_is_set_ = true;
}


PhotoInterrupter::PhotoInterrupter(PhotoInterrupterLedDriver* const led_driver) {
  led_driver_ = led_driver;
  sensor_state_ = false;
  pre_led_status_ = PhotoInterrupterLedDriver::kTransitPeriod;
  sensor_raw_ = 0;
  sensor_on_hold_ = 0;
  sensor_off_hold_ = 0;
  edge_rise_ = false;
  edge_rise_time_ms_ = 0UL;
  edge_fall_ = false;
  edge_fall_time_ms_ = 0UL;
  threshold_high_ = 0;
  threshold_low_ = 0;
  hold_time_ms_ = 0UL;
  last_active_time_ms_ = 0UL;
  port_is_set_ = false;
}

void PhotoInterrupter::compute() {
  if (port_is_set_) {
    const PhotoInterrupterLedDriver::PhotoIntLedStatus led_status = led_driver_->getState();
    if (led_status != pre_led_status_) { // led_status が切替わったときのみ処理を行う
      sensor_raw_ = analogRead(port_);
      if (led_status == PhotoInterrupterLedDriver::kLedIsOn) {
        sensor_on_hold_ = sensor_raw_;
      } else if (led_status == PhotoInterrupterLedDriver::kLedIsOff) {
        sensor_off_hold_ = sensor_raw_;
      }
    }
    pre_led_status_ = led_status;

    const int sensor_diff = sensor_on_hold_ - sensor_off_hold_;

    if (sensor_state_ == false) {
      if (sensor_diff > threshold_high_) {
        sensor_state_ = true;
        edge_fall_ = false;
        edge_rise_ = true;
        edge_rise_time_ms_ = millis();
        last_active_time_ms_ = millis();
      }
    } else {
      if (sensor_diff >= threshold_low_) {
        last_active_time_ms_ = millis();
      } else if (last_active_time_ms_ + hold_time_ms_ < millis()) {
        sensor_state_ = false;
        edge_rise_ = false;
        edge_fall_ = true;
        edge_fall_time_ms_ = millis();
      } else {
        // do nothing
      }
    }
    if (edge_rise_ && (edge_rise_time_ms_ + 250UL < millis())) {
      edge_rise_ = false;
    }
    if (edge_fall_ && (edge_fall_time_ms_ + 250UL < millis())) {
      edge_fall_ = false;
    }
  }
}

int PhotoInterrupter::getOnHold() const {
  return sensor_on_hold_;
}

int PhotoInterrupter::getOffHold() const {
  return sensor_off_hold_;
}

bool PhotoInterrupter::detectEdgeRise() const {
  return edge_rise_;
}

bool PhotoInterrupter::detectEdgeFall() const {
  return edge_fall_;
}

void PhotoInterrupter::setPortNum(const int port) {
  port_ = port;
  port_is_set_ = true;
}

bool PhotoInterrupter::getState() const {
  return sensor_state_;
}

void PhotoInterrupter::setThresholdHigh(const int th) {
  threshold_high_ = th;
}

void PhotoInterrupter::setThresholdLow(const int th) {
  threshold_low_ = th;
}

void PhotoInterrupter::setHoldTime(const unsigned long hold_time_ms) {
  hold_time_ms_ = hold_time_ms;
}

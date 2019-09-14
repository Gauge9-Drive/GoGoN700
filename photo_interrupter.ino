#include "photo_interrupter.h"

PhotoInterrupter::PhotoInterrupter() {
  sensor_state_ = false;
  pre_led_status_ = PhotoInterrupterLedDriver::TransitTime;
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
  if(port_is_set_) {
    const int sensor_raw = analogRead(port_);
    sensor_raw_ = sensor_raw;
    const PhotoInterrupterLedDriver::PhotoIntLedStatus led_status = photo_int_led.getStatus();
    if(led_status == PhotoInterrupterLedDriver::LedIsOn && led_status != pre_led_status_) {
      sensor_on_hold_ = sensor_raw;
    } else if(led_status == PhotoInterrupterLedDriver::LedIsOff && led_status != pre_led_status_) {
      sensor_off_hold_ = sensor_raw;
    }
    pre_led_status_ = photo_int_led.getStatus();
    
    if(sensor_state_ == false) {
      if(sensor_raw > threshold_high_) {
        sensor_state_ = true;
        edge_fall_ = false;
        edge_rise_ = true;
        edge_rise_time_ms_ = millis();
        last_active_time_ms_ = millis();
      }
    } else {
      if(sensor_raw >= threshold_low_) {
        last_active_time_ms_ = millis();
      } else if(last_active_time_ms_ + hold_time_ms_ < millis()) {
        sensor_state_ = false;
        edge_rise_ = false;
        edge_fall_ = true;
        edge_fall_time_ms_ = millis();
      } else {
        // do nothing
      }
    }
    if(edge_rise_ && (edge_rise_time_ms_ + 250UL < millis())) {
      edge_rise_ = false;
    }
    if(edge_fall_ && (edge_fall_time_ms_ + 250UL < millis())) {
      edge_fall_ = false;
    }
  }
}

int PhotoInterrupter::getSensorRaw() {
  return sensor_raw_;
}

int PhotoInterrupter::getSensorOnHold() {
  return sensor_on_hold_;
}

int PhotoInterrupter::getSensorOffHold() {
  return sensor_off_hold_;
}

bool PhotoInterrupter::detectEdgeRise() {
  const bool ret = edge_rise_;
  edge_rise_ = false;
  return ret;
}

bool PhotoInterrupter::detectEdgeFall() {
  const bool ret = edge_fall_;
  edge_fall_ = false;
  return ret;
}

void PhotoInterrupter::setPortNum(const int port) {
  port_ = port;
  port_is_set_ = true;
}

bool PhotoInterrupter::getSensorState() {
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
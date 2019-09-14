#ifndef PHOTO_INTERRUPTER_H
#define PHOTO_INTERRUPTER_H

class PhotoInterrupterLedDriver {
 public:
  enum PhotoIntLedStatus {
    LedIsOn = 0,
    LedIsOff,
    TransitTime
  };
  
  PhotoInterrupterLedDriver() {
    status_ = TransitTime;
    port_is_set_ = false;
  }
  ~PhotoInterrupterLedDriver() {}

  void compute() {
    const unsigned int mask = 0x0300;
    const unsigned int count = (mask & ((unsigned int)millis()));
    if(port_is_set_ == false) {
      status_ = TransitTime;
    } else if(count < 0x0100) {
      status_ = TransitTime;
      digitalWrite(port_, HIGH);
    } else if(count < 0x0200) {
      status_ = LedIsOn;
    } else if(count < 0x0300) {
      status_ = TransitTime;
      digitalWrite(port_, LOW);
    } else {
      status_ = LedIsOff;
    }
 digitalWrite(port_, HIGH); // for DEBUG
  }
  
  PhotoIntLedStatus getStatus() {
    return status_;
  }

  void setPortNum(int port) {
    port_ = port;
    pinMode(port_, OUTPUT);
    port_is_set_ = true;
  }

 private:
  PhotoIntLedStatus status_;
  int port_;
  bool port_is_set_;
};

class PhotoInterrupter {
 public:
  PhotoInterrupter();
  ~PhotoInterrupter() {}

  void compute();
  void setPortNum(const int port);
  bool getSensorState() const;
  int getSensorRaw() const;
  int getSensorOnHold() const;
  int getSensorOffHold() const;
  bool detectEdgeRise();
  bool detectEdgeFall();
  void setThresholdHigh(const int th);
  void setThresholdLow(const int th);
  void setHoldTime(const unsigned long hold_time_ms);

 private:
  bool sensor_state_;
  PhotoInterrupterLedDriver::PhotoIntLedStatus pre_led_status_;
  int sensor_raw_;
  int sensor_on_hold_;
  int sensor_off_hold_;
  bool edge_rise_;
  unsigned long edge_rise_time_ms_;
  bool edge_fall_;
  unsigned long edge_fall_time_ms_;
  int threshold_high_;
  int threshold_low_;
  unsigned long hold_time_ms_;
  unsigned long last_active_time_ms_;
  int port_;
  bool port_is_set_;
  
};

#endif

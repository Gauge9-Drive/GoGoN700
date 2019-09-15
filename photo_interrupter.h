#ifndef PHOTO_INTERRUPTER_H
#define PHOTO_INTERRUPTER_H

class PhotoInterrupterLedDriver {
 private:
  const unsigned int kLedRiseTime = 10U;  // ms
  const unsigned int kLedOnDuration = 10U;
  const unsigned int kLedFallTime = 20U;  // ms
  const unsigned int kLedCycleTime = 50U;

 public:
  enum PhotoIntLedStatus {
    kLedIsOn = 0,
    kLedIsOff,
    kTransitPeriod
  };
  
  PhotoInterrupterLedDriver() {
    status_ = kTransitPeriod;
    port_is_set_ = false;
    t1_ = kLedRiseTime;
    t2_ = kLedRiseTime + kLedOnDuration;
    t3_ = kLedRiseTime + kLedOnDuration + kLedFallTime;
  }
  ~PhotoInterrupterLedDriver() {}

  void compute() {
    const unsigned int count = ((unsigned int)millis() % kLedCycleTime);
    
    if(port_is_set_ == false) {
      status_ = kTransitPeriod;
    } else if(count < t1_) {
      status_ = kTransitPeriod;
      digitalWrite(port_, HIGH);
    } else if(count < t2_) {
      status_ = kLedIsOn;
    } else if(count < t3_) {
      status_ = kTransitPeriod;
      digitalWrite(port_, LOW);
    } else {
      status_ = kLedIsOff;
    }
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
  unsigned int t1_;
  unsigned int t2_;
  unsigned int t3_;
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

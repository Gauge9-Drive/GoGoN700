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
  
  PhotoInterrupterLedDriver();
  ~PhotoInterrupterLedDriver() {}

  void compute();
  PhotoIntLedStatus getState() const;
  void setPortNum(int port);

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
  PhotoInterrupter() = delete;
  PhotoInterrupter(PhotoInterrupterLedDriver* const led_driver);
  ~PhotoInterrupter() {}

  void compute();
  bool getState() const;
  bool detectEdgeRise() const;
  bool detectEdgeFall() const;
  void setPortNum(const int port);

  int getOnHold() const;
  int getOffHold() const;
  void setThresholdHigh(const int th);
  void setThresholdLow(const int th);
  void setHoldTime(const unsigned long hold_time_ms);

 private:
  bool sensor_state_;
  PhotoInterrupterLedDriver::PhotoIntLedStatus pre_led_status_;
  PhotoInterrupterLedDriver* led_driver_;
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

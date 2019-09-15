#ifndef LED_CONTROL_H
#define LED_CONTROL_H

class LedController {
 public:
  LedController();
  ~LedController() {}
  void compute();
  void setLed(const bool val);
  void ledOn();
  void ledOff();
  void ledFlash(const unsigned int period);

  void setPortNum(const int port);

 private:
  int port_;
  bool port_is_set_;
  bool led_is_on_;
  unsigned int flash_period_;
};

#endif

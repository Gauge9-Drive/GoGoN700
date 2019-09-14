#ifndef COUNT_DOWN_TIMER_H
#define COUNT_DOWN_TIMER_H

class CountDownTimer {
 public:
  CountDownTimer();
  ~CountDownTimer() {}

  void startTimer(const unsigned long duration_ms);
  void resetTimer();
  bool isTimerEnd();
  bool isActive();

 private:
  unsigned long end_time_;
  bool is_active_;
  
};

#endif

#include "count_down_timer.h"

CountDownTimer::CountDownTimer() {
  end_time_ = 0UL;
  is_active_ = false;
}

void CountDownTimer::startTimer(const unsigned long duration_ms) {
  end_time_ = millis() + duration_ms;
  is_active_ = true;
}

bool CountDownTimer::isActive() {
  return is_active_;
}

void CountDownTimer::resetTimer() {
  end_time_ = 0UL;
  is_active_ = false;
}

bool CountDownTimer::isTimerEnd() {
  bool ret = false;
  if(end_time_ <= millis()) {
    resetTimer();
    ret = true;
  } else {
    ret = false;
  }
  return ret;
}

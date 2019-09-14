#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include "io_device_def.h"

class StateBase {
 public:
  ~StateBase() {}
  virtual StateBase* transit(const InputDevices& input) = 0;
  virtual void execute(OutputDevices& output) = 0;
  virtual int getStateNumber() = 0;
 protected:
  StateBase() {}
};

class StateManager {
 public:
  StateManager();
  ~StateManager();
  void initialize();
  void transit(const InputDevices& input_devices);
  void execute(OutputDevices& output_devices);
  int getStateNumber() {return current_state_->getStateNumber();}
 private:
  StateBase* current_state_;
};

class StateHalt1 : public StateBase {
 public:
  ~StateHalt1() {delete instance_;}
  static StateHalt1* getInstance();
  StateBase* transit(const InputDevices& input);
  void execute(OutputDevices& output);
  int getStateNumber() {return 0;}
 private:
  static StateHalt1* instance_;
};
StateHalt1* StateHalt1::instance_ = 0;

class StateFwdSlow : public StateBase {
 public:
  ~StateFwdSlow() {delete instance_;}
  static StateFwdSlow* getInstance();
  StateBase* transit(const InputDevices& input);
  void execute(OutputDevices& output);
  int getStateNumber() {return 1;}
 private:
  static StateFwdSlow* instance_;
};
StateFwdSlow* StateFwdSlow::instance_ = 0;

class StateFwdFast : public StateBase {
 public:
  ~StateFwdFast() {delete instance_; delete loop_count_;}
  static StateFwdFast* getInstance();
  StateBase* transit(const InputDevices& input);
  void execute(OutputDevices& output);
  int getStateNumber() {return 2;}
 private:
  static StateFwdFast* instance_;
  static int* loop_count_;
};
StateFwdFast* StateFwdFast::instance_ = 0;
int* StateFwdFast::loop_count_ = 0;

class StateHalt2 : public StateBase {
 public:
  ~StateHalt2() {delete instance_;}
  static StateHalt2* getInstance();
  StateBase* transit(const InputDevices& input);
  void execute(OutputDevices& output);
  int getStateNumber() {return 3;}
 private:
  static StateHalt2* instance_;
};
StateHalt2* StateHalt2::instance_ = 0;

class StateBwdSlow : public StateBase {
 public:
  ~StateBwdSlow() {delete instance_;}
  static StateBwdSlow* getInstance();
  StateBase* transit(const InputDevices& input);
  void execute(OutputDevices& output);
  int getStateNumber() {return 4;}
 private:
  static StateBwdSlow* instance_;
};
StateBwdSlow* StateBwdSlow::instance_ = 0;

class StateBwdFast : public StateBase {
 public:
  ~StateBwdFast() {delete instance_; delete loop_count_;}
  static StateBwdFast* getInstance();
  StateBase* transit(const InputDevices& input);
  void execute(OutputDevices& output);
  int getStateNumber() {return 5;}
 private:
  static StateBwdFast* instance_;
  static int* loop_count_;
};
StateBwdFast* StateBwdFast::instance_ = 0;
int* StateBwdFast::loop_count_ = 0;

#endif

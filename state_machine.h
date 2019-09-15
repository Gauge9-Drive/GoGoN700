#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include "io_device_def.h"
#include "constant_value.h"

class StateBase {
 public:
  ~StateBase() {}
  virtual StateBase* transit(const InputDevices& input, const unsigned int time_elapsed) = 0;
  virtual void execute(OutputDevices& output, const unsigned int time_elapsed) = 0;
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
  unsigned long start_time_;
};

/* ===================================================================== */
class StateReady : public StateBase {
 public:
  ~StateReady() {delete instance_;}
  static StateReady* getInstance();
  StateBase* transit(const InputDevices& input, const unsigned int time_elapsed);
  void execute(OutputDevices& output, const unsigned int time_elapsed);
  int getStateNumber() {return 0;}
 private:
  static StateReady* instance_;
};
StateReady* StateReady::instance_ = 0;

class StatePrep : public StateBase {
 public:
  ~StatePrep() {delete instance_;}
  static StatePrep* getInstance();
  StateBase* transit(const InputDevices& input, const unsigned int time_elapsed);
  void execute(OutputDevices& output, const unsigned int time_elapsed);
  int getStateNumber() {return 1;}
 private:
  static StatePrep* instance_;
  static int* loop_count_;
};
StatePrep* StatePrep::instance_ = 0;

class StatePosFast : public StateBase {
 public:
  ~StatePosFast() {delete instance_; delete loop_count_;}
  static StatePosFast* getInstance();
  StateBase* transit(const InputDevices& input, const unsigned int time_elapsed);
  void execute(OutputDevices& output, const unsigned int time_elapsed);
  int getStateNumber() {return 11;}
 private:
  static StatePosFast* instance_;
  static int* loop_count_;
};
StatePosFast* StatePosFast::instance_ = 0;
int* StatePosFast::loop_count_ = 0;

class StatePosSlow : public StateBase {
 public:
  ~StatePosSlow() {delete instance_;}
  static StatePosSlow* getInstance();
  StateBase* transit(const InputDevices& input, const unsigned int time_elapsed);
  void execute(OutputDevices& output, const unsigned int time_elapsed);
  int getStateNumber() {return 12;}
 private:
  static StatePosSlow* instance_;
};
StatePosSlow* StatePosSlow::instance_ = 0;

class StatePosStop : public StateBase {
 public:
  ~StatePosStop() {delete instance_;}
  static StatePosStop* getInstance();
  StateBase* transit(const InputDevices& input, const unsigned int time_elapsed);
  void execute(OutputDevices& output, const unsigned int time_elapsed);
  int getStateNumber() {return 13;}
 private:
  static StatePosStop* instance_;
};
StatePosStop* StatePosStop::instance_ = 0;

class StateLeaveBranchA : public StateBase {
 public:
  ~StateLeaveBranchA() {delete instance_;}
  static StateLeaveBranchA* getInstance();
  StateBase* transit(const InputDevices& input, const unsigned int time_elapsed);
  void execute(OutputDevices& output, const unsigned int time_elapsed);
  int getStateNumber() {return 14;}
 private:
  static StateLeaveBranchA* instance_;
};
StateLeaveBranchA* StateLeaveBranchA::instance_ = 0;

class StateNegFast : public StateBase {
 public:
  ~StateNegFast() {delete instance_; delete loop_count_;}
  static StateNegFast* getInstance();
  StateBase* transit(const InputDevices& input, const unsigned int time_elapsed);
  void execute(OutputDevices& output, const unsigned int time_elapsed);
  int getStateNumber() {return 21;}
 private:
  static StateNegFast* instance_;
  static int* loop_count_;
};
StateNegFast* StateNegFast::instance_ = 0;
int* StateNegFast::loop_count_ = 0;

class StateNegSlow : public StateBase {
 public:
  ~StateNegSlow() {delete instance_;}
  static StateNegSlow* getInstance();
  StateBase* transit(const InputDevices& input, const unsigned int time_elapsed);
  void execute(OutputDevices& output, const unsigned int time_elapsed);
  int getStateNumber() {return 22;}
 private:
  static StateNegSlow* instance_;
};
StateNegSlow* StateNegSlow::instance_ = 0;

class StateNegStop : public StateBase {
 public:
  ~StateNegStop() {delete instance_;}
  static StateNegStop* getInstance();
  StateBase* transit(const InputDevices& input, const unsigned int time_elapsed);
  void execute(OutputDevices& output, const unsigned int time_elapsed);
  int getStateNumber() {return 23;}
 private:
  static StateNegStop* instance_;
};
StateNegStop* StateNegStop::instance_ = 0;

/**
 * 引込み線Bから、正方向に低速で走行し、センサBの立下り検知したら次へ
 */
class StateLeaveBranchB : public StateBase {
 public:
  ~StateLeaveBranchB() {delete instance_;}
  static StateLeaveBranchB* getInstance();
  StateBase* transit(const InputDevices& input, const unsigned int time_elapsed);
  void execute(OutputDevices& output, const unsigned int time_elapsed);
  int getStateNumber() {return 24;}
 private:
  static StateLeaveBranchB* instance_;
};
StateLeaveBranchB* StateLeaveBranchB::instance_ = 0;







/* ===================================================================== */
class StateHalt1 : public StateBase {
 public:
  ~StateHalt1() {delete instance_;}
  static StateHalt1* getInstance();
  StateBase* transit(const InputDevices& input, const unsigned int time_elapsed);
  void execute(OutputDevices& output, const unsigned int time_elapsed);
  int getStateNumber() {return 0;}
 private:
  static StateHalt1* instance_;
};
StateHalt1* StateHalt1::instance_ = 0;

class StateFwdSlow : public StateBase {
 public:
  ~StateFwdSlow() {delete instance_;}
  static StateFwdSlow* getInstance();
  StateBase* transit(const InputDevices& input, const unsigned int time_elapsed);
  void execute(OutputDevices& output, const unsigned int time_elapsed);
  int getStateNumber() {return 1;}
 private:
  static StateFwdSlow* instance_;
};
StateFwdSlow* StateFwdSlow::instance_ = 0;

class StateFwdFast : public StateBase {
 public:
  ~StateFwdFast() {delete instance_; delete loop_count_;}
  static StateFwdFast* getInstance();
  StateBase* transit(const InputDevices& input, const unsigned int time_elapsed);
  void execute(OutputDevices& output, const unsigned int time_elapsed);
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
  StateBase* transit(const InputDevices& input, const unsigned int time_elapsed);
  void execute(OutputDevices& output, const unsigned int time_elapsed);
  int getStateNumber() {return 3;}
 private:
  static StateHalt2* instance_;
};
StateHalt2* StateHalt2::instance_ = 0;

class StateBwdSlow : public StateBase {
 public:
  ~StateBwdSlow() {delete instance_;}
  static StateBwdSlow* getInstance();
  StateBase* transit(const InputDevices& input, const unsigned int time_elapsed);
  void execute(OutputDevices& output, const unsigned int time_elapsed);
  int getStateNumber() {return 4;}
 private:
  static StateBwdSlow* instance_;
};
StateBwdSlow* StateBwdSlow::instance_ = 0;

class StateBwdFast : public StateBase {
 public:
  ~StateBwdFast() {delete instance_; delete loop_count_;}
  static StateBwdFast* getInstance();
  StateBase* transit(const InputDevices& input, const unsigned int time_elapsed);
  void execute(OutputDevices& output, const unsigned int time_elapsed);
  int getStateNumber() {return 5;}
 private:
  static StateBwdFast* instance_;
  static int* loop_count_;
};
StateBwdFast* StateBwdFast::instance_ = 0;
int* StateBwdFast::loop_count_ = 0;

#endif

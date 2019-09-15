#include "state_machine.h"

StateManager::StateManager() {
  current_state_ = 0;
}

StateManager::~StateManager() {
  delete current_state_;
}

void StateManager::initialize() {
  Serial.println("initilize");
  current_state_ = StateHalt1::getInstance();
  start_time_ = millis();
}

void StateManager::transit(const InputDevices& input_devices) {
  if(current_state_ != 0) {
    const StateBase* pre_state = current_state_;
    const unsigned int time_elapsed = (unsigned int)(millis() - start_time_);
    current_state_ = current_state_->transit(input_devices, time_elapsed);
    if(current_state_ != pre_state) {
      start_time_ = millis();
    }
  }
}

void StateManager::execute(OutputDevices& output_devices) {
  if(current_state_ != 0) {
    const unsigned int time_elapsed = (unsigned int)(millis() - start_time_);
    current_state_->execute(output_devices, time_elapsed);
  }
}

//===========================================================
StateHalt1* StateHalt1::getInstance() {
  if(!instance_) {
    instance_ = new StateHalt1;
    Serial.println("Create Instance : StateHalt1");
  } else {
    // NOP
  }
  return instance_;
}

StateBase* StateHalt1::transit(const InputDevices& input, const unsigned int time_elapsed) {
  if(input.push_sw_1.detectEdgeRise()) {
    return StateFwdSlow::getInstance();
  } else {
    return instance_;
  }
}

void StateHalt1::execute(OutputDevices& output, const unsigned int time_elapsed) {
  output.turn_out_driver_1.setStatus(kTurnOutStraight);
  output.motor_driver_1.setDriveMode(kMtDrvModeHalt);
  output.val = 0;
}

//===========================================================
StateFwdSlow* StateFwdSlow::getInstance() {
  if(!instance_) {
    instance_ = new StateFwdSlow;
    Serial.println("Create Instance : StateFwdSlow");
  } else {
    // NOP
  }
  return instance_;
}

StateBase* StateFwdSlow::transit(const InputDevices& input, const unsigned int time_elapsed) {
  if(input.push_sw_1.detectEdgeRise() || input.photo_int_1.getSensorState()) {
    return StateFwdFast::getInstance();
  } else {
    return instance_;
  }
}

void StateFwdSlow::execute(OutputDevices& output, const unsigned int time_elapsed) {
  output.turn_out_driver_1.setStatus(kTurnOutStraight);
  output.motor_driver_1.setDriveMode(kMtDrvModeFwdSlow);
  output.val = 1;
}

//===========================================================
StateFwdFast* StateFwdFast::getInstance() {
  if(!instance_) {
    instance_ = new StateFwdFast;
    loop_count_ = new int;
    *loop_count_ = 0;
    Serial.println("Create Instance : StateFwdFast");
  } else {
    // NOP
  }
  return instance_;
}

StateBase* StateFwdFast::transit(const InputDevices& input, const unsigned int time_elapsed) {
  if((*loop_count_ % 2) == 0 && input.photo_int_2.getSensorState() 
                             && !input.photo_int_1.getSensorState()) {
    (*loop_count_)++;
  } else if((*loop_count_ % 2) == 1 && input.photo_int_1.getSensorState() 
                                    && !input.photo_int_2.getSensorState()) {
    (*loop_count_)++;
  } else {
    // NOP
  }
  if(input.push_sw_1.detectEdgeRise()) {
    *loop_count_ = 0;
    return StateHalt2::getInstance();
  } else if(*loop_count_ >= 5) {
    *loop_count_ = 0;
    return StateHalt2::getInstance();
  } else {
    return instance_;
  }
}

void StateFwdFast::execute(OutputDevices& output, const unsigned int time_elapsed) {
  output.turn_out_driver_1.setStatus(kTurnOutStraight);
  output.motor_driver_1.setDriveMode(kMtDrvModeFwdFast);
  output.val = *loop_count_;
}

//===========================================================
StateHalt2* StateHalt2::getInstance() {
  if(!instance_) {
    instance_ = new StateHalt2;
    Serial.println("Create Instance : StateHalt2");
  } else {
    // NOP
  }
  return instance_;
}

StateBase* StateHalt2::transit(const InputDevices& input, const unsigned int time_elapsed) {
  const bool timer_end = time_elapsed > 6000UL;
/*  if(!timer_1.isActive()) {
    timer_1.startTimer(6000UL);
  } else {
    timer_end = timer_1.isTimerEnd();
  }*/
  if(input.push_sw_1.detectEdgeRise() || timer_end) {
    return StateBwdSlow::getInstance();
  } else {
    return instance_;
  }
}

void StateHalt2::execute(OutputDevices& output, const unsigned int time_elapsed) {
  output.turn_out_driver_1.setStatus(kTurnOutCurve);
  output.motor_driver_1.setDriveMode(kMtDrvModeHalt);
  output.val = 0;
}

//===========================================================
StateBwdSlow* StateBwdSlow::getInstance() {
  if(!instance_) {
    instance_ = new StateBwdSlow;
    Serial.println("Create Instance : StateBwdSlow");
  } else {
    // NOP
  }
  return instance_;
}

StateBase* StateBwdSlow::transit(const InputDevices& input, const unsigned int time_elapsed) {
  if(input.push_sw_1.detectEdgeRise() || input.photo_int_1.getSensorState()) {
    return StateBwdFast::getInstance();
  } else {
    return instance_;
  }
}

void StateBwdSlow::execute(OutputDevices& output, const unsigned int time_elapsed) {
  output.turn_out_driver_1.setStatus(kTurnOutCurve);
  output.motor_driver_1.setDriveMode(kMtDrvModeBwdSlow);
  output.val = 1;
}

//===========================================================
StateBwdFast* StateBwdFast::getInstance() {
  if(!instance_) {
    instance_ = new StateBwdFast;
    loop_count_ = new int;
    *loop_count_ = 0;
    Serial.println("Create Instance : StateBwdFast");
  } else {
    // NOP
  }
  return instance_;
}

StateBase* StateBwdFast::transit(const InputDevices& input, const unsigned int time_elapsed) {
  if((*loop_count_ % 2) == 0 && input.photo_int_2.getSensorState() && !input.photo_int_1.getSensorState()) {
    (*loop_count_)++;
  } else if((*loop_count_ % 2) == 1 && input.photo_int_1.getSensorState() && !input.photo_int_2.getSensorState()) {
    (*loop_count_)++;
  } else {
    // NOP
  }
  if(input.push_sw_1.detectEdgeRise()) {
    *loop_count_ = 0;
    return StateHalt1::getInstance();
  } else if(*loop_count_ >= 5) {
    *loop_count_ = 0;
    return StateFwdSlow::getInstance();
  } else {
    return instance_;
  }
}

void StateBwdFast::execute(OutputDevices& output, const unsigned int time_elapsed) {
  output.turn_out_driver_1.setStatus(kTurnOutCurve);
  output.motor_driver_1.setDriveMode(kMtDrvModeBwdFast);
  output.val = *loop_count_;
}

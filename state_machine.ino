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
}

void StateManager::transit(const InputDevices& input_devices) {
  if(current_state_ != 0) {
    current_state_ = current_state_->transit(input_devices);
  }
}

void StateManager::execute(OutputDevices& output_devices) {
  if(current_state_ != 0) {
    current_state_->execute(output_devices);
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

StateBase* StateHalt1::transit(const InputDevices& input) {
  if(input.sw_edge) {
    return StateFwdSlow::getInstance();
  } else {
    return instance_;
  }
}

void StateHalt1::execute(OutputDevices& output) {
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

StateBase* StateFwdSlow::transit(const InputDevices& input) {
  if(input.sw_edge || input.photo_int_1.getSensorState()) {
    return StateFwdFast::getInstance();
  } else {
    return instance_;
  }
}

void StateFwdSlow::execute(OutputDevices& output) {
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

StateBase* StateFwdFast::transit(const InputDevices& input) {
  if((*loop_count_ % 2) == 0 && input.photo_int_2.getSensorState()) {
    (*loop_count_)++;
  } else if((*loop_count_ % 2) == 1 && input.photo_int_1.getSensorState()) {
    (*loop_count_)++;
  } else {
    // NOP
  }
  if(input.sw_edge) {
    *loop_count_ = 0;
    return StateHalt2::getInstance();
  } else if(*loop_count_ >= 5) {
    *loop_count_ = 0;
    return StateBwdSlow::getInstance();
  } else {
    return instance_;
  }
}

void StateFwdFast::execute(OutputDevices& output) {
  output.turn_out_driver_1.setStatus(kTurnOutStraight);
  output.motor_driver_1.setDriveMode(kMtDrvModeFwdFast);
  output.val = 1;
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

StateBase* StateHalt2::transit(const InputDevices& input) {
  if(input.sw_edge) {
    return StateBwdSlow::getInstance();
  } else {
    return instance_;
  }
}

void StateHalt2::execute(OutputDevices& output) {
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

StateBase* StateBwdSlow::transit(const InputDevices& input) {
  if(input.sw_edge || input.photo_int_1.getSensorState()) {
    return StateBwdFast::getInstance();
  } else {
    return instance_;
  }
}

void StateBwdSlow::execute(OutputDevices& output) {
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

StateBase* StateBwdFast::transit(const InputDevices& input) {
  if((*loop_count_ % 2) == 0 && input.photo_int_2.getSensorState()) {
    (*loop_count_)++;
  } else if((*loop_count_ % 2) == 1 && input.photo_int_1.getSensorState()) {
    (*loop_count_)++;
  } else {
    // NOP
  }
  if(input.sw_edge) {
    *loop_count_ = 0;
    return StateHalt1::getInstance();
  } else if(*loop_count_ >= 5) {
    *loop_count_ = 0;
    return StateFwdSlow::getInstance();
  } else {
    return instance_;
  }
}

void StateBwdFast::execute(OutputDevices& output) {
  output.turn_out_driver_1.setStatus(kTurnOutCurve);
  output.motor_driver_1.setDriveMode(kMtDrvModeBwdFast);
  output.val = 1;
}

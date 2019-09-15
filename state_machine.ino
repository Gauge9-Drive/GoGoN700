#include "state_machine.h"

StateManager::StateManager() {
  current_state_ = 0;
}

StateManager::~StateManager() {
  delete current_state_;
}

void StateManager::initialize() {
  Serial.println("initilize");
  current_state_ = StateReady::getInstance();
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

/**=====================================================================
 * 電源投入時の待機状態、スイッチを押すと次に進む
 * 分岐A：正方向の電圧で周回しているときに、分岐可能な分岐地点
 * センサA：分岐Aの手前にあるセンサ
 * 分岐B：正方向の電圧で周回しているときに、分岐可能な分岐地点
 * センサB：分岐Bの手前にあるセンサ
 */
StateReady* StateReady::getInstance() {
  if(!instance_) {
    instance_ = new StateReady;
    Serial.println("Create Instance : StateReady");
  }
  return instance_;
}

StateBase* StateReady::transit(const InputDevices& input, const unsigned int time_elapsed) {
  if(input.push_sw_1.detectEdgeRise()) {
    return StatePrep::getInstance();
  } else {
    return instance_;
  }
}

void StateReady::execute(OutputDevices& output, const unsigned int time_elapsed) {
  output.turn_out_driver_1.setStatus(kTurnOutStraight);
  output.motor_driver_1.setDriveMode(kMtDrvModeHalt);
  output.val = 0;
}

/**=====================================================================
 * 分岐を直進側にする
 * 正方向に低速で走行して、初期位置を合わせる
 * センサBの立下がりを通過したら次のステートへ
 */
StatePrep* StatePrep::getInstance() {
  if(!instance_) {
    instance_ = new StatePrep;
    Serial.println("Create Instance : StateReady");
  }
  return instance_;
}

StateBase* StatePrep::transit(const InputDevices& input, const unsigned int time_elapsed) {
  if(input.push_sw_1.detectEdgeRise()) {
    return StateReady::getInstance();
  } else if(input.photo_int_2.detectEdgeFall()) {
    return StatePosFast::getInstance();
  } else {
    return instance_;
  }
}

void StatePrep::execute(OutputDevices& output, const unsigned int time_elapsed) {
  output.turn_out_driver_1.setStatus(kTurnOutStraight);
  if(time_elapsed < 100U) {
    output.turn_out_driver_1.requestForceDrive();
  }
  output.motor_driver_1.setDriveMode(kMtDrvModeFwdSlow);
  output.val = 0;
}

/**=====================================================================
 * 高速で正方向に走行
 * センサA・センサB・センサA・センサBの立下がり検知したら次へ
 */
StatePosFast* StatePosFast::getInstance() {
  if(!instance_) {
    instance_ = new StatePosFast;
    loop_count_ = new int;
    *loop_count_ = 0;
    Serial.println("Create Instance : StatePosFast");
  } else {
    // NOP
  }
  return instance_;
}

StateBase* StatePosFast::transit(const InputDevices& input, const unsigned int time_elapsed) {
  if((*loop_count_ % 2) == 0 && input.photo_int_1.getSensorState()) {
    (*loop_count_)++;
  } else if((*loop_count_ % 2) == 1 && input.photo_int_2.getSensorState()) {
    (*loop_count_)++;
  } else {
    // NOP
  }
  if(input.push_sw_1.detectEdgeRise()) {
    return StateReady::getInstance();
  } else if((*loop_count_ >= 4 && input.photo_int_2.detectEdgeFall())) {
    *loop_count_ = 0;
    return StatePosSlow::getInstance();
  } else {
    return instance_;
  }
}

void StatePosFast::execute(OutputDevices& output, const unsigned int time_elapsed) {
  output.turn_out_driver_1.setStatus(kTurnOutStraight);
  output.motor_driver_1.setDriveMode(kMtDrvModeFwdFast);
  output.val = *loop_count_;
}

/**=====================================================================
 * 分岐を曲線方向へ転換
 * 低速で正方向に走行、センサAの立下り検知したら次へ
 */
StatePosSlow* StatePosSlow::getInstance() {
  if(!instance_) {
    instance_ = new StatePosSlow;
    Serial.println("Create Instance : StatePosSlow");
  }
  return instance_;
}

StateBase* StatePosSlow::transit(const InputDevices& input, const unsigned int time_elapsed) {
  if(input.push_sw_1.detectEdgeRise()) {
    return StateReady::getInstance();
  } else if(input.photo_int_1.detectEdgeFall()) {
    return StatePosStop::getInstance();
  } else {
    return instance_;
  }
}

void StatePosSlow::execute(OutputDevices& output, const unsigned int time_elapsed) {
  output.turn_out_driver_1.setStatus(kTurnOutCurve);
  output.motor_driver_1.setDriveMode(kMtDrvModeFwdSlow);
  output.val = 0;
}

/**=====================================================================
 * 低速で正方向に走行、遷移後２秒で停止し、さらに５秒後に次へ
 */
StatePosStop* StatePosStop::getInstance() {
  if(!instance_) {
    instance_ = new StatePosStop;
    Serial.println("Create Instance : StatePosStop");
  }
  return instance_;
}

StateBase* StatePosStop::transit(const InputDevices& input, const unsigned int time_elapsed) {
  if(input.push_sw_1.detectEdgeRise()) {
    return StateReady::getInstance();
  } else if(time_elapsed >= 7000U) {
    return StateLeaveBranchA::getInstance();
  } else {
    return instance_;
  }
}

void StatePosStop::execute(OutputDevices& output, const unsigned int time_elapsed) {
  output.turn_out_driver_1.setStatus(kTurnOutCurve);
  if(time_elapsed <= 500U) {
    output.motor_driver_1.setDriveMode(kMtDrvModeFwdSlow);
  } else {
    output.motor_driver_1.setDriveMode(kMtDrvModeHalt);
  }
  output.val = (int)time_elapsed;
}

/**=====================================================================
 * 引込み線Aから、逆方向に低速で走行し、センサAの立下り検知したら次へ
 */
StateLeaveBranchA* StateLeaveBranchA::getInstance() {
  if(!instance_) {
    instance_ = new StateLeaveBranchA;
    Serial.println("Create Instance : StateLeaveBranchA");
  }
  return instance_;
}

StateBase* StateLeaveBranchA::transit(const InputDevices& input, const unsigned int time_elapsed) {
  if(input.push_sw_1.detectEdgeRise()) {
    return StateReady::getInstance();
  } else if(input.photo_int_1.detectEdgeFall()) {
    return StateNegFast::getInstance();
  } else {
    return instance_;
  }
}

void StateLeaveBranchA::execute(OutputDevices& output, const unsigned int time_elapsed) {
  output.turn_out_driver_1.setStatus(kTurnOutCurve);
//  if(time_elapsed < 100U) {
//    output.turn_out_driver_1.requestForceDrive();
//  }
  output.motor_driver_1.setDriveMode(kMtDrvModeBwdSlow);
  output.val = 0;
}

/**=====================================================================
 * 分岐を直線側へ転換
 * 高速でセンサB・センサA・センサB・センサAと通過したら次へ
 */
StateNegFast* StateNegFast::getInstance() {
  if(!instance_) {
    instance_ = new StateNegFast;
    loop_count_ = new int;
    *loop_count_ = 0;
    Serial.println("Create Instance : StateNegFast");
  } else {
    // NOP
  }
  return instance_;
}

StateBase* StateNegFast::transit(const InputDevices& input, const unsigned int time_elapsed) {
  if((*loop_count_ % 2) == 0 && input.photo_int_2.getSensorState()) {
    (*loop_count_)++;
  } else if((*loop_count_ % 2) == 1 && input.photo_int_1.getSensorState()) {
    (*loop_count_)++;
  } else {
    // NOP
  }
  if(input.push_sw_1.detectEdgeRise()) {
    return StateReady::getInstance();
  } else if((*loop_count_ >= 4 && input.photo_int_1.detectEdgeFall())) {
    *loop_count_ = 0;
    return StateNegSlow::getInstance();
  } else {
    return instance_;
  }
}

void StateNegFast::execute(OutputDevices& output, const unsigned int time_elapsed) {
  output.turn_out_driver_1.setStatus(kTurnOutStraight);
  output.motor_driver_1.setDriveMode(kMtDrvModeBwdFast);
  output.val = *loop_count_;
}

/**=====================================================================
 * 分岐を曲線方向へ転換
 * 低速で逆方向に走行、センサBの立下り検知したら次へ
 */
StateNegSlow* StateNegSlow::getInstance() {
  if(!instance_) {
    instance_ = new StateNegSlow;
    Serial.println("Create Instance : StateNegSlow");
  }
  return instance_;
}

StateBase* StateNegSlow::transit(const InputDevices& input, const unsigned int time_elapsed) {
  if(input.push_sw_1.detectEdgeRise()) {
    return StateReady::getInstance();
  } else if(input.photo_int_2.detectEdgeFall()) {
    return StateNegStop::getInstance();
  } else {
    return instance_;
  }
}

void StateNegSlow::execute(OutputDevices& output, const unsigned int time_elapsed) {
  output.turn_out_driver_1.setStatus(kTurnOutCurve);
  output.motor_driver_1.setDriveMode(kMtDrvModeBwdSlow);
  output.val = 0;
}

/**=====================================================================
 * 低速で逆方向に走行、遷移後２秒で停止し、さらに５秒後に次へ
 */
StateNegStop* StateNegStop::getInstance() {
  if(!instance_) {
    instance_ = new StateNegStop;
    Serial.println("Create Instance : StateNegStop");
  }
  return instance_;
}

StateBase* StateNegStop::transit(const InputDevices& input, const unsigned int time_elapsed) {
  if(input.push_sw_1.detectEdgeRise()) {
    return StateReady::getInstance();
  } else if(time_elapsed >= 7000U) {
    return StateLeaveBranchB::getInstance();
  } else {
    return instance_;
  }
}

void StateNegStop::execute(OutputDevices& output, const unsigned int time_elapsed) {
  output.turn_out_driver_1.setStatus(kTurnOutCurve);
  if(time_elapsed <= 500U) {
    output.motor_driver_1.setDriveMode(kMtDrvModeBwdSlow);
  } else {
    output.motor_driver_1.setDriveMode(kMtDrvModeHalt);
  }
  output.val = (int)time_elapsed;
}

/**=====================================================================
 * 引込み線Bから、正方向に低速で走行し、センサBの立下り検知したら次へ
 */
StateLeaveBranchB* StateLeaveBranchB::getInstance() {
  if(!instance_) {
    instance_ = new StateLeaveBranchB;
    Serial.println("Create Instance : StateLeaveBranchB");
  }
  return instance_;
}

StateBase* StateLeaveBranchB::transit(const InputDevices& input, const unsigned int time_elapsed) {
  if(input.push_sw_1.detectEdgeRise()) {
    return StateReady::getInstance();
  } else if(input.photo_int_2.detectEdgeFall()) {
    return StatePosFast::getInstance();
  } else {
    return instance_;
  }
}

void StateLeaveBranchB::execute(OutputDevices& output, const unsigned int time_elapsed) {
  output.turn_out_driver_1.setStatus(kTurnOutCurve);
  if(time_elapsed < 100U) {
    output.turn_out_driver_1.requestForceDrive();
  }
  output.motor_driver_1.setDriveMode(kMtDrvModeFwdSlow);
  output.val = 0;
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

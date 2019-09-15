#ifndef CONSTANT_VALUE_H
#define CONSTANT_VALUE_H


const int kMaxVolt = 255; // fixed
const int kMinVolt = -255; // fixed
const int kVoltSlow = 60;
const int kVoltFast = 95;

const int kMaxValOfAnalogWrite = 0xFF;
const int kMaxvalOfAnalogRead = 1023;
const int kThresholdOfPhotoIntOn = 300;
const int kThresholdOfPhotoIntOff = 150;
const unsigned long kSwitchFiltTimeMsec = 100UL; // Time constant for filtering switch chattering in msec

const int kVoltRiseRate100ms = 1; // duty LSB per 100ms
const int kVoltFallRate100ms = 2; // duty LSB per 100ms

const unsigned long kTurnOutDriveDurationMsec = 100UL;
const unsigned long kTurnOutDriveIntervalMsec = 1000UL;

enum MotorDriveMode {
  kMtDrvModeHalt = 0,
  kMtDrvModeFwdSlow,
  kMtDrvModeFwdFast,
  kMtDrvModeBwdSlow,
  kMtDrvModeBwdFast
};

enum TurnOutStatus {
  kTurnOutStraight = 0,
  kTurnOutCurve
}; 

#endif

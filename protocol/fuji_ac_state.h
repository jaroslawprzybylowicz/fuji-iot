// Copyright 2020 Fuji-Iot authors. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef FUJI_AC_STATE_H_
#define FUJI_AC_STATE_H_

#include "fuji_register.h"
#include "fuji_types.h"

namespace fuji_iot {
// This class represents local state of the AC unit.
class FujiAcState {
 public:
  mode_t Mode() const;
  void SetMode(mode_t mode);
  bool Enabled() const;
  void SetEnabled(bool enabled);
  fan_t Fan() const;
  void SetFan(fan_t fan);
  bool ErrorFlag() const;
  bool Economy() const;
  void SetEconomy(bool economy);
  uint8_t Temperature() const;
  void SetTemperature(uint8_t temp);
  // Swing flap operation.
  bool Swing() const;
  void SetSwing(bool swing);
  // Single step in fan flap operation.
  bool SwingStep() const;
  void SetSwingStep(bool swing_step);
  // Will return true if user modified local state and it needs to be written to
  // main unit.
  bool Merged() const;
  void SetMerged(bool merged);

  // Returns true if wired-controller was registered with the main unit.
  bool ControllerPresent() const;

  // Fills status object with data based on this object.
  void BuildStatusRegisterResponse(FujiStatusRegister *status) const;
  // Merges main unit data with local object.
  void MergeFromMasterStatusRegister(const FujiStatusRegister &status);

 private:
  void SetControllerPresent(bool controller_present);
  void SetErrorFlag(bool error_flag);

  mode_t mode_ = mode_t::AUTO;
  bool enabled_ = false;
  fan_t fan_ = fan_t::AUTO;
  bool error_flag_ = false;
  bool controller_present_ = false;
  bool economy_ = false;
  uint8_t temperature_ = 20;
  bool swing_ = false;
  bool swing_step_ = false;
  bool merged_ = true;
};

}  // namespace fuji_iot

#endif
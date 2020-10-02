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

#ifndef FUJI_REGISTER_H_
#define FUJI_REGISTER_H_

#include <bits/stdint-uintn.h>

#include <array>

#include "fuji_types.h"

namespace fuji_iot {
// Controller communicated with AC unit using 8-byte frames (see fuji_frame.h)
// This is a helper class to manage payload within those frames.
class FujiRegister {
 public:
  FujiRegister(uint8_t *data);
  std::array<uint8_t, 5> Payload() const;

 protected:
  uint8_t *bytes_;
};

// Helper for accessing payload of STATUS register.
class FujiStatusRegister : public FujiRegister {
 public:
  FujiStatusRegister(uint8_t *data);

  void SetMode(mode_t mode);
  const mode_t Mode() const;
  void SetEnabled(bool enabled);
  const bool Enabled() const;
  void SetFan(fan_t fan);
  const fan_t Fan() const;
  void SetError(bool error);
  const bool Error() const;
  void SetEconomy(bool economy);
  const bool Economy() const;
  void SetTemperature(uint8_t temp);
  const uint8_t Temperature() const;
  void SetSwing(bool swing);
  const bool Swing() const;
  void SetSwingStep(bool swing);
  const bool SwingStep() const;
  void SetControllerPresent(bool swing);
  const bool ControllerPresent() const;
  void SetUpdateMagic(uint8_t update_magic);
  const uint8_t UpdateMagic() const;
  const std::string DebugInfo() const;
};

}  // namespace fuji_iot

#endif

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

#ifndef FUJI_AC_UNIT_SIM_H_
#define FUJI_AC_UNIT_SIM_H_

#include <bits/stdint-uintn.h>
#include <gtest/gtest_prod.h>

#include <array>
#include <memory>

#include "protocol/fuji_frame.h"

namespace fuji_iot {
namespace sim {
// Simulates real AC unit.
// May be used in server application to verify code correctness and create
// integration tests.
class FujiAcUnitSim {
 public:
  FujiAcUnitSim();
  const FujiMasterFrame GetNextMasterFrame();
  void PushControllerFrame(const FujiControllerFrame &frame);

  // Methods below are intended to real AC unit side state of the universe.
  // Changing values is simulated as if IR controller was used.
  bool Enabled() const;
  void SetEnabled(bool enabled);
  mode_t Mode() const;
  void SetMode(mode_t mode);
  fan_t Fan() const;
  void SetFan(fan_t fan);
  uint8_t Temperature() const;
  void SetTemperature(uint8_t temp);
  bool Economy() const;
  void SetEconomy(bool economy);
  bool Swing() const;
  void SetSwing(bool swing);
  bool SwingStep() const;
  void SetSwingStep(bool swing);
  bool ControllerPresent() const;

 protected:
  FRIEND_TEST(FujiAcUnitSimTest, TestReconnect);
  FRIEND_TEST(FujiAcUnitSimTest, RemoteTurnOnTurnOff);
  FRIEND_TEST(FujiAcUnitSimTest, TestGolden);
  FRIEND_TEST(FujiAcUnitSimTest, TurnOn);
  FRIEND_TEST(FujiAcUnitSimTest, WiredControlGolden);
  std::unique_ptr<FujiStatusRegister> status_;

 private:
  void Update();
  std::array<uint8_t, 8> status_register_;
  std::array<uint8_t, 8> error_register_;
  std::array<uint8_t, 8> login_register_;
  RegisterType next_query_register_;
};
}  // namespace sim
}  // namespace fuji_iot

#endif
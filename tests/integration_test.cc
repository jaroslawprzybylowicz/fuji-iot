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

#include "absl/synchronization/mutex.h"
#include "absl/types/optional.h"
#include "controller/fuji_ac_controller.h"
#include "controller/fuji_ac_serial_interface.h"
#include "glog/logging.h"
#include "gtest/gtest.h"
#include "protocol/fuji_ac_protocol_handler.h"
#include "protocol/fuji_ac_state.h"
#include "sim/fuji_ac_unit_sim.h"

namespace fuji_iot {
namespace tests {

proto::Mode ModeToProtoMode(const mode_t mode) {
  switch (mode) {
    case mode_t::AUTO:
      return proto::MODE_AUTO;
    case mode_t::COOL:
      return proto::MODE_COOL;
    case mode_t::DRY:
      return proto::MODE_DRY;
    case mode_t::FAN:
      return proto::MODE_FAN;
    case mode_t::HEAT:
      return proto::MODE_HEAT;
      break;
    default:
      LOG(FATAL) << "Unknown mode";
  }
}

proto::Fan FanToProtoFan(const fan_t fan) {
  switch (fan) {
    case fan_t::AUTO:
      return proto::FAN_AUTO;
    case fan_t::HIGH:
      return proto::FAN_HIGH;
    case fan_t::MEDIUM:
      return proto::FAN_MEDIUM;
    case fan_t::LOW:
      return proto::FAN_LOW;
    case fan_t::MAX:
      return proto::FAN_MAX;
    default:
      LOG(FATAL) << "Unknown fan mode";
      break;
  }
}

// Tests for FujiAcController class. Verifies correctness of protocol logic and
// serial interface integration.
class FujiAcServerTest : public testing::Test, FujiAcSerialInterface {
 public:
  virtual void WriteControllerFrame(const FujiControllerFrame &frame) {
    sim_->PushControllerFrame(frame);
    same_ = (last_frame_ == frame);
    last_frame_ = frame;
  }

  virtual absl::optional<FujiMasterFrame> ReadMasterFrame() override {
    absl::MutexLock l(&mu_);
    frame_count_++;
    // absl::SleepFor(absl::Milliseconds(10));
    return sim_->GetNextMasterFrame();
  }

 protected:
  FujiAcServerTest() {
    sim_ = std::unique_ptr<sim::FujiAcUnitSim>(new sim::FujiAcUnitSim());
  }

  void SetUp() override {
    controller_ = std::move(FujiAcController::MakeFujiAcController(this));
    controller_->GetStatus();
  }

  void TearDown() override { controller_->Shutdown(); }

  bool Enabled() {
    absl::MutexLock l(&mu_);
    return sim_->Enabled();
  }

  void SetEnabled(bool enabled) {
    absl::MutexLock l(&mu_);
    sim_->SetEnabled(enabled);
  }

  mode_t Mode() {
    absl::MutexLock l(&mu_);
    return sim_->Mode();
  }

  void SetMode(mode_t mode) {
    absl::MutexLock l(&mu_);
    sim_->SetMode(mode);
  }

  fan_t Fan() {
    absl::MutexLock l(&mu_);
    return sim_->Fan();
  }

  void SetFan(fan_t fan) {
    absl::MutexLock l(&mu_);
    sim_->SetFan(fan);
  }

  uint8_t Temperature() {
    absl::MutexLock l(&mu_);
    return sim_->Temperature();
  }

  void SetTemperature(uint8_t temp) {
    absl::MutexLock l(&mu_);
    sim_->SetTemperature(temp);
  }

  void AwaitRead() {
    absl::MutexLock l(&mu_);
    int current = frame_count_;
    auto reached = [this, current]() { return frame_count_ >= current + 2; };
    mu_.Await(absl::Condition(&reached));
  }

  std::unique_ptr<FujiAcController> controller_;

 private:
  absl::Mutex mu_;
  int frame_count_ ABSL_GUARDED_BY(&mu_);
  std::unique_ptr<sim::FujiAcUnitSim> sim_ ABSL_GUARDED_BY(&mu_);
  absl::optional<FujiControllerFrame> last_frame_;
  bool same_;
};  // namespace tests

TEST_F(FujiAcServerTest, GetStatus) {
  auto state = controller_->GetStatus();
  EXPECT_EQ(proto::MODE_OFF, state.mode());
  EXPECT_EQ(proto::FAN_MAX, state.fan());
}

TEST_F(FujiAcServerTest, TurnOnViaRemote) {
  EXPECT_EQ(proto::MODE_OFF, controller_->GetStatus().mode());
  SetEnabled(true);
  AwaitRead();
  EXPECT_EQ(proto::MODE_COOL, controller_->GetStatus().mode());
}

TEST_F(FujiAcServerTest, TurnOnViaController) {
  EXPECT_FALSE(Enabled());
  proto::ACUnitState state;
  state.set_mode(proto::MODE_COOL);
  EXPECT_TRUE(controller_->Update(state).ok());
  EXPECT_TRUE(Enabled());
}

TEST_F(FujiAcServerTest, SwitchModeViaRemote) {
  SetEnabled(true);
  AwaitRead();
  for (mode_t mode :
       {mode_t::AUTO, mode_t::COOL, mode_t::DRY, mode_t::FAN, mode_t::HEAT}) {
    SetMode(mode);
    AwaitRead();
    EXPECT_EQ(ModeToProtoMode(mode), controller_->GetStatus().mode());
  }
}

TEST_F(FujiAcServerTest, SwitchModeViaController) {
  SetEnabled(true);
  controller_->GetStatus();
  for (mode_t mode :
       {mode_t::AUTO, mode_t::COOL, mode_t::DRY, mode_t::FAN, mode_t::HEAT}) {
    proto::ACUnitState state;
    state.set_mode(ModeToProtoMode(mode));
    EXPECT_TRUE(controller_->Update(state).ok());
    EXPECT_EQ(mode, Mode());
  }
}

TEST_F(FujiAcServerTest, SwitchFanViaRemote) {
  SetEnabled(true);
  AwaitRead();
  controller_->GetStatus();
  for (fan_t fan :
       {fan_t::AUTO, fan_t::HIGH, fan_t::LOW, fan_t::MAX, fan_t::MEDIUM}) {
    SetFan(fan);
    AwaitRead();
    EXPECT_EQ(FanToProtoFan(fan), controller_->GetStatus().fan());
  }
}

TEST_F(FujiAcServerTest, SwitchFanViaController) {
  SetEnabled(true);
  controller_->GetStatus();
  for (fan_t fan :
       {fan_t::AUTO, fan_t::HIGH, fan_t::LOW, fan_t::MAX, fan_t::MEDIUM}) {
    proto::ACUnitState state;
    state.set_fan(FanToProtoFan(fan));
    EXPECT_TRUE(controller_->Update(state).ok());
    EXPECT_EQ(fan, Fan());
  }
}

TEST_F(FujiAcServerTest, SwitchTemperatureViaRemote) {
  SetEnabled(true);
  AwaitRead();
  controller_->GetStatus();
  for (uint8_t temperature = 18; temperature < 30; temperature++) {
    SetTemperature(temperature);
    AwaitRead();
    EXPECT_EQ(temperature, controller_->GetStatus().setpoint_temperature());
  }
}

TEST_F(FujiAcServerTest, SwitchTemperatureViaController) {
  SetEnabled(true);
  controller_->GetStatus();
  for (uint8_t temperature = 18; temperature < 30; temperature++) {
    proto::ACUnitState state;
    state.set_setpoint_temperature(temperature);
    EXPECT_TRUE(controller_->Update(state).ok());
    EXPECT_EQ(temperature, Temperature());
  }
}

TEST_F(FujiAcServerTest, RaceModeChange) {
  SetEnabled(true);
  SetMode(mode_t::COOL);
  controller_->GetStatus();
  SetMode(mode_t::DRY);
  proto::ACUnitState state;
  state.set_mode(proto::MODE_HEAT);
  EXPECT_TRUE(controller_->Update(state).ok());
  // Controller takes precedence.
  EXPECT_EQ(mode_t::HEAT, Mode());
  EXPECT_EQ(proto::MODE_HEAT, controller_->GetStatus().mode());
}

TEST_F(FujiAcServerTest, RaceTempChange) {
  SetEnabled(true);
  SetFan(fan_t::LOW);
  controller_->GetStatus();
  SetFan(fan_t::MAX);
  proto::ACUnitState state;
  state.set_fan(proto::FAN_MEDIUM);
  EXPECT_TRUE(controller_->Update(state).ok());
  // Controller takes precedence.
  EXPECT_EQ(fan_t::MEDIUM, Fan());
  EXPECT_EQ(proto::FAN_MEDIUM, controller_->GetStatus().fan());
}

}  // namespace tests
}  // namespace fuji_iot

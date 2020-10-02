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

#include "fuji_ac_unit_sim.h"

#include <glog/logging.h>

#include "absl/types/optional.h"
#include "protocol/fuji_frame.h"

namespace fuji_iot {
namespace sim {

FujiAcUnitSim::FujiAcUnitSim() {
  status_register_ = {0x00, 0x00, 0x00, 0x46, 0x12, 0xa0, 0x00, 0x20};
  error_register_ = {0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00};
  login_register_ = {0x00, 0x00, 0x20, 0x1F, 0x1F, 0x05, 0x01, 0x00};
  next_query_register_ = RegisterType::STATUS;
  status_ = std::unique_ptr<FujiStatusRegister>(
      new FujiStatusRegister(status_register_.data() + 3));
}

const FujiMasterFrame FujiAcUnitSim::GetNextMasterFrame() {
  absl::optional<FujiMasterFrame> resp;
  switch (next_query_register_) {
    case RegisterType::UNKNOWN:
    case RegisterType::STATUS:
      if (status_->SwingStep()) {
        // master unit 0'es swing step immediately after use.
        status_->SetSwingStep(false);
      }
      resp.emplace(FujiMasterFrame(status_register_));
      resp->WithType(RegisterType::STATUS);
      break;
    case RegisterType::ERROR:
      resp.emplace(FujiMasterFrame(error_register_));
      resp->WithType(RegisterType::ERROR);
      break;
    case RegisterType::LOGIN:
      resp.emplace(FujiMasterFrame(login_register_));
      resp->WithType(RegisterType::LOGIN);
      break;
    default:
      LOG(FATAL) << "Not a query register";
      break;
  }
  resp->WithUnknownBit(true);
  resp->WithDestination(DestinationAddr::WIRED_CONTROLLER_ADDR);
  status_->SetUpdateMagic(10);
  VLOG(3) << "Master frame: " << resp.value();
  return resp.value();
}

void FujiAcUnitSim::PushControllerFrame(const FujiControllerFrame &frame) {
  VLOG(3) << "Controller frame: " << frame.DebugInfo();
  if (frame.QueryRegister() != RegisterType::UNKNOWN) {
    next_query_register_ = frame.QueryRegister();
  }
  if (frame.LoginBit()) {
    status_->SetControllerPresent(true);
  }
  if (frame.QueryRegister() == RegisterType::STATUS && frame.WriteBit()) {
    std::array<uint8_t, 5> payload = frame.Payload();
    FujiStatusRegister status(payload.data());
    status_->SetEnabled(status.Enabled());
    status_->SetMode(status.Mode());
    status_->SetFan(status.Fan());
    status_->SetEconomy(status.Economy());
    status_->SetSwing(status.Swing());
    status_->SetSwingStep(status.SwingStep());
    if (status.Mode() != mode_t::FAN) {
      // Ignore temperature in fan mode.
      status_->SetTemperature(status.Temperature());
      status_->SetUpdateMagic(0);
    } else {
      status_->SetTemperature(18);
      status_->SetUpdateMagic(4);
    }
  }
}

void FujiAcUnitSim::Update() { status_->SetUpdateMagic(4); }

bool FujiAcUnitSim::Enabled() const { return status_->Enabled(); }

void FujiAcUnitSim::SetEnabled(bool enabled) {
  status_->SetEnabled(enabled);
  Update();
}

uint8_t FujiAcUnitSim::Temperature() const { return status_->Temperature(); }

void FujiAcUnitSim::SetTemperature(uint8_t temp) {
  status_->SetTemperature(temp);
  Update();
}

fan_t FujiAcUnitSim::Fan() const { return status_->Fan(); }

void FujiAcUnitSim::SetFan(fan_t fan) {
  status_->SetFan(fan);
  Update();
}

mode_t FujiAcUnitSim::Mode() const { return status_->Mode(); }

void FujiAcUnitSim::SetMode(mode_t mode) {
  status_->SetMode(mode);
  Update();
}

bool FujiAcUnitSim::Economy() const { return status_->Economy(); }

void FujiAcUnitSim::SetEconomy(bool economy) {
  status_->SetEconomy(economy);
  Update();
}

bool FujiAcUnitSim::Swing() const { return status_->Swing(); }

void FujiAcUnitSim::SetSwing(bool swing) {
  status_->SetSwing(swing);
  Update();
}

bool FujiAcUnitSim::SwingStep() const { return status_->SwingStep(); }

void FujiAcUnitSim::SetSwingStep(bool swing_step) {
  status_->SetSwingStep(swing_step);
  Update();
}

bool FujiAcUnitSim::ControllerPresent() const {
  return status_->ControllerPresent();
}

}  // namespace sim

}  // namespace fuji_iot
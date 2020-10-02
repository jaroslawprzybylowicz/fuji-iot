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

#include "controller/fuji_ac_controller.h"

#include "absl/status/status.h"
#include "glog/logging.h"

namespace fuji_iot {

absl::Status FujiAcController::Update(const proto::ACUnitState &new_state) {
  absl::MutexLock l(&mu_);
  ready_ = false;
  switch (new_state.mode()) {
    case proto::MODE_OFF:
      state_->SetEnabled(false);
      break;
    case proto::MODE_AUTO:
      state_->SetEnabled(true);
      state_->SetMode(mode_t::AUTO);
      break;
    case proto::MODE_COOL:
      state_->SetEnabled(true);
      state_->SetMode(mode_t::COOL);
      break;
    case proto::MODE_HEAT:
      state_->SetEnabled(true);
      state_->SetMode(mode_t::HEAT);
      break;
    case proto::MODE_DRY:
      state_->SetEnabled(true);
      state_->SetMode(mode_t::DRY);
      break;
    case proto::MODE_FAN:
      state_->SetEnabled(true);
      state_->SetMode(mode_t::FAN);
      break;
    default:
      break;
  }
  switch (new_state.fan()) {
    case proto::FAN_AUTO:
      state_->SetFan(fan_t::AUTO);
      break;
    case proto::FAN_MAX:
      state_->SetFan(fan_t::MAX);
      break;
    case proto::FAN_HIGH:
      state_->SetFan(fan_t::HIGH);
      break;
    case proto::FAN_MEDIUM:
      state_->SetFan(fan_t::MEDIUM);
      break;
    case proto::FAN_LOW:
      state_->SetFan(fan_t::LOW);
      break;
    default:
      break;
  }
  if (new_state.setpoint_temperature() != 0) {
    state_->SetTemperature(new_state.setpoint_temperature());
  }
  mu_.Await(absl::Condition(&ready_));
  return absl::OkStatus();
}

const proto::ACUnitState FujiAcController::GetStatus() {
  absl::MutexLock l(&mu_);
  mu_.Await(absl::Condition(&ready_));
  proto::ACUnitState ret;
  if (state_->Enabled()) {
    switch (state_->Mode()) {
      case mode_t::AUTO:
        ret.set_mode(proto::MODE_AUTO);
        break;
      case mode_t::COOL:
        ret.set_mode(proto::MODE_COOL);
        break;
      case mode_t::DRY:
        ret.set_mode(proto::MODE_DRY);
        break;
      case mode_t::FAN:
        ret.set_mode(proto::MODE_FAN);
        break;
      case mode_t::HEAT:
        ret.set_mode(proto::MODE_HEAT);
        break;
      default:
        LOG(ERROR)
            << "Mode is unknown, this should happen only on initialization";
        break;
    }
  } else {
    ret.set_mode(proto::MODE_OFF);
  }
  switch (state_->Fan()) {
    case fan_t::AUTO:
      ret.set_fan(proto::FAN_AUTO);
      break;
    case fan_t::MAX:
      ret.set_fan(proto::FAN_MAX);
      break;
    case fan_t::HIGH:
      ret.set_fan(proto::FAN_HIGH);
      break;
    case fan_t::MEDIUM:
      ret.set_fan(proto::FAN_MEDIUM);
      break;
    case fan_t::LOW:
      ret.set_fan(proto::FAN_LOW);
      break;
    default:
      break;
  }
  ret.set_setpoint_temperature(state_->Temperature());

  return ret;
}

void FujiAcController::DoLoop() {
  while (!shutdown_) {
    auto mf = serial_->ReadMasterFrame();
    if (!mf.has_value()) {
      continue;
    }
    {
      absl::MutexLock l(&mu_);
      auto cf = client_->HandleMasterFrame(mf.value());
      if (cf.has_value()) {
        if (cf == last_frame_) ready_ = true;
        serial_->WriteControllerFrame(cf.value());
        last_frame_ = cf.value();
      }
    }
  }
}

FujiAcController::FujiAcController(
    std::unique_ptr<FujiAcProtocolHandler> handler,
    FujiAcSerialInterface *serial, FujiAcState *state)
    : client_(std::move(handler)),
      serial_(serial),
      state_(state),
      shutdown_(false),
      ready_(false) {
  loop_thread_ = std::unique_ptr<std::thread>(
      new std::thread(&FujiAcController::DoLoop, this));
}

void FujiAcController::Shutdown() {
  if (shutdown_) {
    LOG(FATAL) << "already shut down.";
  }
  shutdown_ = true;
  loop_thread_->join();
}

FujiAcController::~FujiAcController() {
  if (!shutdown_) {
    LOG(ERROR) << "Destroying controller object before shutting down.";
    Shutdown();
  }
}

std::unique_ptr<FujiAcController> FujiAcController::MakeFujiAcController(
    FujiAcSerialInterface *serial) {
  FujiAcState *state = new FujiAcState();
  std::unique_ptr<FujiAcProtocolHandler> handler =
      std::unique_ptr<FujiAcProtocolHandler>(
          new FujiAcProtocolHandler(std::unique_ptr<FujiAcState>(state)));
  return std::unique_ptr<FujiAcController>(
      new FujiAcController(std::move(handler), serial, state));
}

}  // namespace fuji_iot
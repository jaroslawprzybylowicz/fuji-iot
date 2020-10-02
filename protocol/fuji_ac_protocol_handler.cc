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

#include "fuji_ac_protocol_handler.h"

#include <memory>

#include "absl/types/optional.h"
#include "fuji_ac_state.h"

namespace fuji_iot {

FujiAcProtocolHandler::FujiAcProtocolHandler(
    std::unique_ptr<FujiAcState> state) {
  ac_state_ = std::move(state);
}

absl::optional<FujiControllerFrame> FujiAcProtocolHandler::HandleMasterFrame(
    const FujiMasterFrame &master_frame) {
  // It seems that some of the frames sent over the wire are not intended for
  // wired controller and are simply ignored
  if (master_frame.Destination() != DestinationAddr::WIRED_CONTROLLER_ADDR) {
    return absl::nullopt;
  }

  // There are several types of frames, they represent separate registers that
  // hold diffrent type of information.
  switch (master_frame.Type()) {
    case RegisterType::STATUS:
      // Status register is most commonly used (it is repeated over and over if
      // no special actions are taken). We want to make sure that local
      // ac_state_ represents current state of the main unit, but if user made
      // changes to it, local state takes precedence and we ignore incoming
      // data.
      if (ac_state_->Merged()) UpdateFromMasterStatusRegister(master_frame);
      break;
    case RegisterType::LOGIN:
      // At the very beggining of the communication, wired-controller
      // "registers" itself with the main unit. This frame is sent only once
      // upon successful registration.
      return SendLoggedInFrame();
    case RegisterType::ERROR:
      // Error register holds information about recent errors in the system.
      UpdateFromMasterErrorRegister(master_frame);
    default:
      // This should log an error.
      return absl::nullopt;
  }

  if (login_read_) {
    // Login frame will "register" wired-controller with the main unit.
    // This happens only at the begging of communication, so we clear the flag.
    // During the next cycle, we should receive RegisterType::LOGIN frame.
    login_read_ = false;
    return SendLoginFrame();
  }
  if (error_read_) {
    // If status frame error bit is set, error_read will be true and we will
    // send query for error register. During the next cycle, we should receive
    // RegisterType::ERROR frame.
    error_read_ = false;
    return SendErrorQueryFrame();
  }
  return SendStatusFrame();
}

FujiControllerFrame FujiAcProtocolHandler::SendLoginFrame() {
  FujiControllerFrame f;
  // Next master_frame should be of LOGIN type.
  f.WithQueryRegister(RegisterType::LOGIN);
  return f;
}

FujiControllerFrame FujiAcProtocolHandler::SendErrorQueryFrame() {
  FujiControllerFrame f;
  // Next master_frame should be of ERROR type.
  f.WithQueryRegister(RegisterType::ERROR);
  return f;
}

FujiControllerFrame FujiAcProtocolHandler::SendStatusFrame() {
  // First, we initialize empty controller frame.
  FujiControllerFrame f;
  std::array<uint8_t, 5> payload;
  payload.fill(0);
  FujiStatusRegister status(payload.data());
  // Third byte of controller status frame is value from
  // wired-controller temperature sensor (I think it's in Celsius multiplied by
  // 2). AC unit ignores this value unless it's configured to use external
  // sensor.
  // TODO: remove this magic and implement temp. sensor one day.
  payload[3] = 47;
  // Next, let's build response frame based on our local-state.
  ac_state_->BuildStatusRegisterResponse(&status);
  f.WithPayload(payload);
  // We want next frame to be of type STATUS too.
  f.WithQueryRegister(RegisterType::STATUS);
  if (!ac_state_->Merged()) {
    // If data was not merged, we want AC unit to use our local
    // version.
    f.WithWriteBit(true);
    if (ac_state_->SwingStep()) {
      // If swing step is used, main unit will automatically clear it
      // next cycle.
      ac_state_->SetSwingStep(false);
      ac_state_->SetMerged(false);
    } else {
      // If not, after a write we can consider local state to be merged.
      ac_state_->SetMerged(true);
    }
  }
  return f;
}

FujiControllerFrame FujiAcProtocolHandler::SendLoggedInFrame() {
  // This is effectively same as above, but no state change occurs.
  FujiControllerFrame f;
  std::array<uint8_t, 5> payload;
  payload.fill(0);
  f.WithLoginBit(true);
  FujiStatusRegister status(payload.data());
  // TODO: remove this magic.
  payload[3] = 47;
  ac_state_->BuildStatusRegisterResponse(&status);
  f.WithPayload(payload);
  f.WithQueryRegister(RegisterType::STATUS);
  return f;
}

void FujiAcProtocolHandler::UpdateFromMasterStatusRegister(
    const FujiMasterFrame &master_frame) {
  std::array<uint8_t, 5> payload = master_frame.Payload();
  FujiStatusRegister status(payload.data());
  if (ac_state_->ErrorFlag() != status.Error()) {
    // If error bit is set, set the flag and query for Error register next
    // cycle.
    error_read_ = true;
  }
  ac_state_->MergeFromMasterStatusRegister(status);
  if (!ac_state_->ControllerPresent()) {
    // If controller is not registered with main unit, set flag and query for
    // Login register next cycle.
    login_read_ = true;
  }
}

void FujiAcProtocolHandler::UpdateFromMasterErrorRegister(
    const FujiMasterFrame &master_frame) {
  // TODO: It would be nice to actually implement error codes someday.
}

}  // namespace fuji_iot
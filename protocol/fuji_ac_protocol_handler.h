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

#ifndef FUJI_AC_PROTOCOL_HANDLER_H_
#define FUJI_AC_PROTOCOL_HANDLER_H_

#include <gtest/gtest_prod.h>

#include <memory>

#include "absl/types/optional.h"
#include "fuji_ac_state.h"
#include "fuji_frame.h"

namespace fuji_iot {
// This class is entry point for protocol handling. It is virtual equivalent of
// wired controller. AC unit communicates with the wired controller via serial
// interface. On hardware layer it is implemented using single wire connection
// that is shared for Tx/Rx.
//
// [master_frame -- wired_ctrl_frame(opt) -------]
//
// Single communication cycle is started with single master frame sent. Wired
// controller process it and (optionally) responds with wire_ctrl_frame.
// Remainder of the cycle is silent. The process resumes with next master_frame
// and is periodic. Main unit will repeat master_frame indefinately (even if
// there is not state change) and will treat lack of responses as an error.
class FujiAcProtocolHandler {
 public:
  // Takes ownership of state object, but caller should keep it reference to
  // read and modify AC unit state.
  FujiAcProtocolHandler(std::unique_ptr<FujiAcState> state);
  // Processes data from main unit, updates state object with master_frame data
  // and optionally returns a controller frame that should be sent back to main
  // unit. If state was modified in between the calls (for example to change AC
  // unit parameters), it will take precendece and that modification will be
  // reflected in returned controller frame.
  absl::optional<FujiControllerFrame> HandleMasterFrame(
      const FujiMasterFrame &master_frame);

 private:
  FujiControllerFrame SendLoginFrame();
  FujiControllerFrame SendErrorQueryFrame();
  FujiControllerFrame SendStatusFrame();
  FujiControllerFrame SendLoggedInFrame();
  void UpdateFromMasterStatusRegister(const FujiMasterFrame &master_frame);
  void UpdateFromMasterErrorRegister(const FujiMasterFrame &master_frame);

  std::unique_ptr<FujiAcState> ac_state_;
  bool error_read_ = false;
  bool login_read_ = true;
  FRIEND_TEST(FujiAcProtocolHandlerTest, RemoteTurnOnTurnOff);
  FRIEND_TEST(FujiAcProtocolHandlerTest, TestGolden);
  FRIEND_TEST(FujiAcProtocolHandlerTest, TurnOn);
  FRIEND_TEST(FujiAcProtocolHandlerTest, WiredControlGolden);
};

}  // namespace fuji_iot

#endif
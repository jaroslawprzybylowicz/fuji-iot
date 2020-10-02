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

#ifndef FUJI_AC_CONTROLLER_H_
#define FUJI_AC_CONTROLLER_H_

#include <atomic>
#include <memory>
#include <thread>

#include "absl/status/status.h"
#include "absl/synchronization/mutex.h"
#include "controller/fuji_ac_controller.pb.h"
#include "controller/fuji_ac_serial_interface.h"
#include "protocol/fuji_ac_protocol_handler.h"
#include "protocol/fuji_ac_state.h"
#include "protocol/fuji_types.h"

namespace fuji_iot {
// This class combines protocol logic with hardware interface and provides an
// abstraction of a wired-controller. Given the serial interface, this object
// will run separate thead in the background that will handle periodic
// communication. Class is thread-safe.
class FujiAcController {
 public:
  ~FujiAcController();
  // Call to change AC unit parameters. This method will block until values are
  // changed.
  absl::Status Update(const proto::ACUnitState &new_state);
  // Call to obtain last known state of the AC unit. This method will usually
  // return immediately, unless there was no state obtained from the controller
  // yet.
  const proto::ACUnitState GetStatus();
  // Will construct FujiAcController and start underlying thread for protocol
  // handling.
  static std::unique_ptr<FujiAcController> MakeFujiAcController(
      FujiAcSerialInterface *serial);
  // Should be called prior to destruction to stop underlying thread.
  void Shutdown();

 private:
  void DoLoop();

  absl::Mutex mu_;
  FujiAcController(std::unique_ptr<FujiAcProtocolHandler> handler,
                   FujiAcSerialInterface *serial, FujiAcState *state);
  std::unique_ptr<FujiAcProtocolHandler> client_;
  std::unique_ptr<std::thread> loop_thread_;

  FujiAcSerialInterface *serial_;
  FujiAcState *state_;

  FujiControllerFrame last_frame_;

  // looping vars
  std::atomic<bool> shutdown_;
  // indicates whether controller reached a stable state.
  bool ready_;
};

}  // namespace fuji_iot

#endif
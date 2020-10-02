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

#include <iostream>
#include <memory>
#include <string>

#include "absl/strings/str_format.h"
#include "controller/fuji_ac_controller.grpc.pb.h"
#include "gflags/gflags.h"
#include "glog/logging.h"
#include "grpcpp/grpcpp.h"

DEFINE_string(address, "", "Specifies bind address, all interfaces by default");
DEFINE_int32(port, 12345, "Specifies bind port");
DEFINE_string(mode, "", "New mode setting");
DEFINE_string(fan, "", "New fan setting");
DEFINE_int32(setpoint, 0, "New setpoint temperature");

namespace fuji_iot {

// Very simple client. If mode/fan/setpoint flags are not specified, will query
// for status. If present, will change that property to flag value.
void RunClient() {
  auto stub = proto::FujiACControllerService::NewStub(
      ::grpc::CreateChannel(absl::StrFormat("%s:%d", FLAGS_address, FLAGS_port),
                            grpc::InsecureChannelCredentials()));
  grpc::ClientContext context;
  proto::UpdateRequest request;
  proto::StatusResponse response;
  auto new_state = request.mutable_new_state();
  if (FLAGS_mode == "off") {
    new_state->set_mode(proto::Mode::MODE_OFF);
  } else if (FLAGS_mode == "cool") {
    new_state->set_mode(proto::Mode::MODE_COOL);
  } else if (FLAGS_mode == "dry") {
    new_state->set_mode(proto::Mode::MODE_DRY);
  } else if (FLAGS_mode == "fan") {
    new_state->set_mode(proto::Mode::MODE_FAN);
  } else if (FLAGS_mode == "heat") {
    new_state->set_mode(proto::Mode::MODE_HEAT);
  }

  if (FLAGS_fan == "low") {
    new_state->set_fan(proto::FAN_LOW);
  } else if (FLAGS_fan == "medium") {
    new_state->set_fan(proto::FAN_MEDIUM);
  } else if (FLAGS_fan == "high") {
    new_state->set_fan(proto::FAN_HIGH);
  } else if (FLAGS_fan == "max") {
    new_state->set_fan(proto::FAN_MAX);
  }

  if (FLAGS_setpoint >= 18 && FLAGS_setpoint <= 28) {
    new_state->set_setpoint_temperature(FLAGS_setpoint);
  }

  auto status = stub->Update(&context, request, &response);
  if (status.ok()) {
    LOG(INFO) << "Reponse Status:\n" << response.DebugString();
  } else {
    LOG(ERROR) << "Failed to send RPC";
  }
}

}  // namespace fuji_iot

int main(int argc, char **argv) {
  google::ParseCommandLineFlags(&argc, &argv, false);
  google::InitGoogleLogging(argv[0]);
  fuji_iot::RunClient();
  return 0;
}
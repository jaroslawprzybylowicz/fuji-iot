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
#include "controller/fuji_ac_controller.h"
#include "controller/fuji_ac_serial_reader.h"
#include "gflags/gflags.h"
#include "glog/logging.h"
#include "grpcpp/grpcpp.h"
#include "sim/fuji_ac_unit_sim.h"

DEFINE_string(serial_port, "/dev/ttyAMA0",
              "Port to use with AC Unit communication");
DEFINE_bool(sim, false, "If true uses simulated AC Unit.");
DEFINE_string(bind_address, "",
              "Specifies bind address, all interfaces by default");
DEFINE_int32(bind_port, 12345, "Specifies bind port");

namespace fuji_iot {

class FujiACControllerSimServiceImpl final
    : public proto::FujiACControllerService::Service,
      FujiAcSerialInterface {
 public:
  FujiACControllerSimServiceImpl() {
    sim_ = std::unique_ptr<sim::FujiAcUnitSim>(new sim::FujiAcUnitSim());
    controller_ = std::move(FujiAcController::MakeFujiAcController(this));
  }

  virtual void WriteControllerFrame(const FujiControllerFrame &frame) {
    sim_->PushControllerFrame(frame);
  }

  virtual absl::optional<FujiMasterFrame> ReadMasterFrame() override {
    absl::SleepFor(absl::Seconds(1));
    return sim_->GetNextMasterFrame();
  }

  ::grpc::Status GetStatus(::grpc::ServerContext *context,
                           const proto::StatusRequest *request,
                           proto::StatusResponse *response) {
    VLOG(3) << "GetStatus query";
    auto state = controller_->GetStatus();
    VLOG(3) << "Responding with state: " << state.DebugString();
    *response->mutable_state() = state;
    return ::grpc::Status::OK;
  }

  ::grpc::Status Update(::grpc::ServerContext *context,
                        const proto::UpdateRequest *request,
                        proto::StatusResponse *response) {
    LOG(INFO) << "Update query request: " << request->DebugString();
    auto status = controller_->Update(request->new_state());
    if (status.ok()) {
      LOG(INFO) << "Update successful";
      *response->mutable_state() = controller_->GetStatus();
      return ::grpc::Status::OK;
    }
    LOG(ERROR) << "Update failed: " << status;
    return ::grpc::Status(grpc::StatusCode::INTERNAL,
                          "Failed to update the controller.");
  }

 private:
  std::unique_ptr<FujiAcController> controller_;
  std::unique_ptr<sim::FujiAcUnitSim> sim_;
};

class FujiACControllerRealServiceImpl final
    : public proto::FujiACControllerService::Service,
      FujiAcSerialInterface {
 public:
  FujiACControllerRealServiceImpl() {
    reader_ = FujiAcSerialReader::Build(FLAGS_serial_port);
    controller_ = std::move(FujiAcController::MakeFujiAcController(this));
  }

  virtual void WriteControllerFrame(const FujiControllerFrame &frame) {
    reader_->WriteControllerFrame(frame);
  }

  virtual absl::optional<FujiMasterFrame> ReadMasterFrame() override {
    return reader_->ReadMasterFrame();
  }

  ::grpc::Status GetStatus(::grpc::ServerContext *context,
                           const proto::StatusRequest *request,
                           proto::StatusResponse *response) {
    VLOG(3) << "GetStatus query";
    auto state = controller_->GetStatus();
    VLOG(3) << "Responding with state: " << state.DebugString();
    *response->mutable_state() = state;
    return ::grpc::Status::OK;
  }

  ::grpc::Status Update(::grpc::ServerContext *context,
                        const proto::UpdateRequest *request,
                        proto::StatusResponse *response) {
    LOG(INFO) << "Update query request: " << request->DebugString();
    auto status = controller_->Update(request->new_state());
    if (status.ok()) {
      LOG(INFO) << "Update successful";
      *response->mutable_state() = controller_->GetStatus();
      return ::grpc::Status::OK;
    }
    LOG(ERROR) << "Update failed: " << status;
    return ::grpc::Status(grpc::StatusCode::INTERNAL,
                          "Failed to update the controller.");
  }

 private:
  std::unique_ptr<FujiAcController> controller_;
  std::unique_ptr<FujiAcSerialReader> reader_;
};

// Will run server with either real controller or simulated one based on --sim
// flag.
void RunServer() {
  std::string server_address =
      absl::StrFormat("%s:%d", FLAGS_bind_address, FLAGS_bind_port);
  std::unique_ptr<proto::FujiACControllerService::Service> service;
  if (FLAGS_sim) {
    service = std::unique_ptr<FujiACControllerSimServiceImpl>(
        new FujiACControllerSimServiceImpl());
  } else {
    service = std::unique_ptr<FujiACControllerRealServiceImpl>(
        new FujiACControllerRealServiceImpl());
  }

  grpc::ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(service.get());
  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  if (server.get() != nullptr) {
    server->Wait();
  } else {
    LOG(FATAL) << "Failed to create server";
  }
}

}  // namespace fuji_iot

int main(int argc, char **argv) {
  google::ParseCommandLineFlags(&argc, &argv, false);
  google::InitGoogleLogging(argv[0]);
  fuji_iot::RunServer();
  return 0;
}

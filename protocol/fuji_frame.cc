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

#include "fuji_frame.h"

namespace fuji_iot {

const std::string ToString(const DestinationAddr &da) {
  switch (da) {
    case DestinationAddr::OTHER:
      return "OTHER";
    case DestinationAddr::WIRED_CONTROLLER_ADDR:
      return "WIRED_CONTROLLER_ADDR";
    case DestinationAddr::UNKNOWN:
      return "UNKNOWN";
    default:
      return "invalid value";
  }
}

std::ostream &operator<<(std::ostream &os, const DestinationAddr &da) {
  os << ToString(da);
  return os;
}

const std::string ToString(const RegisterType &rt) {
  switch (rt) {
    case RegisterType::STATUS:
      return "STATUS";
    case RegisterType::ERROR:
      return "ERROR";
    case RegisterType::LOGIN:
      return "LOGIN";
    case RegisterType::UNKNOWN:
      return "UNKNOWN";
    default:
      return "invalid value";
  }
}

std::ostream &operator<<(std::ostream &os, const RegisterType &rt) {
  os << ToString(rt);
  return os;
}

FujiMasterFrame::FujiMasterFrame(std::array<uint8_t, 8> data) { data_ = data; }

bool FujiMasterFrame::UnknownBit() const { return (data_[1] & 0b10000000) > 0; }

void FujiMasterFrame::WithUnknownBit(bool bit) {
  data_[1] &= 0b01111111;
  if (bit) {
    data_[1] |= 0b10000000;
  }
}

DestinationAddr FujiMasterFrame::Destination() const {
  switch (data_[1] & 0b01111111) {
    case static_cast<uint8_t>(DestinationAddr::WIRED_CONTROLLER_ADDR):
      return DestinationAddr::WIRED_CONTROLLER_ADDR;
    case static_cast<uint8_t>(DestinationAddr::OTHER):
      return DestinationAddr::OTHER;
    default:
      return DestinationAddr::UNKNOWN;
  }
}

void FujiMasterFrame::WithDestination(DestinationAddr dest) {
  data_[1] &= 0b10000000;
  data_[1] |= static_cast<uint8_t>(dest) & 0b01111111;
}

RegisterType FujiMasterFrame::Type() const {
  switch (data_[2] >> 4 & 0b11) {
    case static_cast<uint8_t>(RegisterType::STATUS):
      return RegisterType::STATUS;
    case static_cast<uint8_t>(RegisterType::ERROR):
      return RegisterType::ERROR;
    case static_cast<uint8_t>(RegisterType::LOGIN):
      return RegisterType::LOGIN;
    default:
      return RegisterType::UNKNOWN;
  }
}

void FujiMasterFrame::WithType(RegisterType type) {
  data_[2] &= 0b11001111;
  data_[2] |= static_cast<uint8_t>(type) << 4;
}

std::array<uint8_t, 5> FujiMasterFrame::Payload() const {
  std::array<uint8_t, 5> ret;
  std::copy(data_.begin() + 3, data_.end(), ret.begin());
  return ret;
}

std::array<uint8_t, 8> FujiMasterFrame::FullFrame() const { return data_; }

const std::string FujiMasterFrame::DebugInfo() const {
  std::string str;
  str.append("Register type: ");
  str.append(ToString(Type()));
  str.append(" Unknown bit: ");
  if (UnknownBit()) {
    str.append("true");
  } else {
    str.append("false");
  }
  str.append(" Destination: ");
  str.append(ToString(Destination()));

  switch (Type()) {
    case RegisterType::STATUS: {
      auto payload = Payload();
      FujiStatusRegister status(payload.data());
      str.append(" Status register: ");
      str.append(status.DebugInfo());
    }
    default:
      str.append(" Payload: ");
      str.append(std::to_string(Payload()[0]));
      str.append(" ");
      str.append(std::to_string(Payload()[1]));
      str.append(" ");
      str.append(std::to_string(Payload()[2]));
      str.append(" ");
      str.append(std::to_string(Payload()[3]));
      str.append(" ");
      str.append(std::to_string(Payload()[4]));
      break;
  }

  return str;
}

std::ostream &operator<<(std::ostream &os, const FujiMasterFrame &mf) {
  os << mf.DebugInfo();
  return os;
}

bool operator==(const FujiMasterFrame &a, const FujiMasterFrame &b) {
  return a.FullFrame() == b.FullFrame();
}

FujiControllerFrame::FujiControllerFrame()
    : FujiControllerFrame({0, 129, 0, 0, 0, 0, 0, 0}) {
  data_[0] = static_cast<uint8_t>(DestinationAddr::WIRED_CONTROLLER_ADDR);
}

FujiControllerFrame::FujiControllerFrame(std::array<uint8_t, 8> data) {
  data_ = data;
}

DestinationAddr FujiControllerFrame::ControllerAddress() const {
  return DestinationAddr::WIRED_CONTROLLER_ADDR;
}

void FujiControllerFrame::WithQueryRegister(RegisterType register_type) {
  data_[2] &= 0b11001111;
  data_[2] |= static_cast<uint8_t>(register_type) << 4;
}

RegisterType FujiControllerFrame::QueryRegister() const {
  switch ((data_[2] & 0b00110000) >> 4) {
    case 0:
      return RegisterType::STATUS;
    case 1:
      return RegisterType::ERROR;
    case 2:
      return RegisterType::LOGIN;
    default:
      return RegisterType::UNKNOWN;
  }
}

void FujiControllerFrame::WithLoginBit(bool login) {
  if (login) {
    data_[1] |= 0b00100000;
  } else {
    data_[1] &= 0b11011111;
  }
}

bool FujiControllerFrame::LoginBit() const {
  return (data_[1] & 0b00100000) != 0;
}

void FujiControllerFrame::WithWriteBit(bool write) {
  if (write) {
    data_[2] |= 0b00001000;
  } else {
    data_[2] &= 0b11110111;
  }
}

bool FujiControllerFrame::WriteBit() const {
  return (data_[2] & 0b00001000) != 0;
}

std::array<uint8_t, 5> FujiControllerFrame::Payload() const {
  std::array<uint8_t, 5> payload;
  std::copy(data_.begin() + 3, data_.end(), payload.begin());
  return payload;
}

std::array<uint8_t, 8> FujiControllerFrame::FullFrame() const { return data_; }

void FujiControllerFrame::WithPayload(const std::array<uint8_t, 5> &payload) {
  std::copy(payload.begin(), payload.end(), data_.begin() + 3);
}

std::array<uint8_t, 8> FujiControllerFrame::BuildFrame() { return data_; }

const std::string FujiControllerFrame::DebugInfo() const {
  std::string str;
  str.append("Query register: ");
  str.append(ToString(QueryRegister()));
  str.append(" Login bit: ");
  if (LoginBit()) {
    str.append("true");
  } else {
    str.append("false");
  }
  str.append(" Write bit: ");
  if (WriteBit()) {
    str.append("true");
  } else {
    str.append("false");
  }
  str.append(" Payload: ");
  str.append(std::to_string(Payload()[0]));
  str.append(" ");
  str.append(std::to_string(Payload()[1]));
  str.append(" ");
  str.append(std::to_string(Payload()[2]));
  str.append(" ");
  str.append(std::to_string(Payload()[3]));
  str.append(" ");
  str.append(std::to_string(Payload()[4]));

  return str;
}

std::ostream &operator<<(std::ostream &os, const FujiControllerFrame &cf) {
  os << cf.DebugInfo();
  return os;
}

bool operator==(const FujiControllerFrame &a, const FujiControllerFrame &b) {
  return a.FullFrame() == b.FullFrame();
}

bool operator!=(const FujiControllerFrame &a, const FujiControllerFrame &b) {
  return !(a == b);
}

}  // namespace fuji_iot
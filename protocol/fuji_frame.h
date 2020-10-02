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

#ifndef FUJI_FRAME_H_
#define FUJI_FRAME_H_

#include <bits/stdint-uintn.h>

#include <array>
#include <iostream>
#include <string>

#include "fuji_register.h"

namespace fuji_iot {
// Frames used for communication are 8-byte long. They consist of magic,
// destination address, type and payload. [magic, destination, register_type,
// register content (5-byte)]
enum class DestinationAddr : uint8_t {
  UNKNOWN = 0,
  // This device.
  WIRED_CONTROLLER_ADDR = 32,
  // Something else that we are not.
  OTHER = 1,
};

const std::string ToString(const DestinationAddr &da);

std::ostream &operator<<(std::ostream &os, const DestinationAddr &da);

enum class RegisterType {
  STATUS = 0,
  ERROR = 1,
  LOGIN = 2,
  UNKNOWN = 3,
};

const std::string ToString(const RegisterType &rt);

std::ostream &operator<<(std::ostream &os, const RegisterType &rt);

// This class is helper for representing frames incoming from the main unit.
class FujiMasterFrame {
 public:
  FujiMasterFrame(std::array<uint8_t, 8> data);
  // We don't know meaning of this bit. But it is set when dest address is
  // OTHER.
  bool UnknownBit() const;
  void WithUnknownBit(bool bit);
  // Returns destination device.
  DestinationAddr Destination() const;
  void WithDestination(DestinationAddr dest);
  RegisterType Type() const;
  void WithType(RegisterType type);
  // Just the payload (without magic and destination).
  std::array<uint8_t, 5> Payload() const;
  // Returns entire frame content.
  std::array<uint8_t, 8> FullFrame() const;
  // Print data in this frame in human readable form.
  const std::string DebugInfo() const;

 private:
  std::array<uint8_t, 8> data_;
};

std::ostream &operator<<(std::ostream &os, const FujiMasterFrame &mf);

bool operator==(const FujiMasterFrame &a, const FujiMasterFrame &b);

class FujiControllerFrame {
 public:
  FujiControllerFrame();
  FujiControllerFrame(std::array<uint8_t, 8> data);
  DestinationAddr ControllerAddress() const;
  RegisterType QueryRegister() const;
  void WithQueryRegister(RegisterType register_type);
  bool LoginBit() const;
  void WithLoginBit(bool login);
  bool WriteBit() const;
  void WithWriteBit(bool write);
  std::array<uint8_t, 5> Payload() const;
  std::array<uint8_t, 8> FullFrame() const;
  void WithPayload(const std::array<uint8_t, 5> &payload);
  std::array<uint8_t, 8> BuildFrame();
  const std::string DebugInfo() const;

 private:
  std::array<uint8_t, 8> data_;
};

std::ostream &operator<<(std::ostream &os, const FujiControllerFrame &cf);
bool operator==(const FujiControllerFrame &a, const FujiControllerFrame &b);
bool operator!=(const FujiControllerFrame &a, const FujiControllerFrame &b);

}  // namespace fuji_iot

#endif
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

#ifndef FUJI_TYPES_H_
#define FUJI_TYPES_H_

#include <bits/stdint-uintn.h>

#include <initializer_list>
#include <string>

namespace fuji_iot {
// Defines various operation modes of the AC unit.
enum class mode_t : uint8_t {
  UNKNOWN = 0,
  FAN = 1,
  DRY = 2,
  COOL = 3,
  HEAT = 4,
  AUTO = 5,
};

// Returns human-readable string for AC unit mode.
const std::string ToString(const mode_t &mode);

std::ostream &operator<<(std::ostream &os, const mode_t &mode);

constexpr std::initializer_list<mode_t> all_mode_t = {
    mode_t::UNKNOWN, mode_t::FAN,  mode_t::DRY,
    mode_t::COOL,    mode_t::HEAT, mode_t::AUTO,
};

// Defines fan speed of the AC unit.
enum class fan_t : uint8_t {
  AUTO = 0,
  LOW = 1,
  MEDIUM = 2,
  HIGH = 3,
  MAX = 4,
  UNKNOWN = 5,
};

constexpr std::initializer_list<fan_t> all_fan_t = {
    fan_t::AUTO, fan_t::LOW, fan_t::MEDIUM,
    fan_t::HIGH, fan_t::MAX, fan_t::UNKNOWN,
};

// Returns human-readable string for fan speed.
const std::string ToString(const fan_t &fan);

std::ostream &operator<<(std::ostream &os, const fan_t &fan);

}  // namespace fuji_iot

#endif

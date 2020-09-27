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

#include "protocol/fuji_types.h"

namespace fuji_iot
{

    const std::string ToString(const mode_t &mode)
    {
        switch (mode)
        {
        case mode_t::UNKNOWN:
            return "UNKNOWN";
        case mode_t::FAN:
            return "FAN";
        case mode_t::DRY:
            return "DRY";
        case mode_t::COOL:
            return "COOL";
        case mode_t::HEAT:
            return "HEAT";
        case mode_t::AUTO:
            return "AUTO";

        default:
            return "invalid value";
        }
    }

    std::ostream &operator<<(std::ostream &os, const mode_t &mode)
    {
        os << ToString(mode);
        return os;
    }

    const std::string ToString(const fan_t &fan)
    {
        switch (fan)
        {
        case fan_t::AUTO:
            return "AUTO";
        case fan_t::LOW:
            return "LOW";
        case fan_t::MEDIUM:
            return "MEDIUM";
        case fan_t::HIGH:
            return "HIGH";
        case fan_t::MAX:
            return "MAX";
        case fan_t::UNKNOWN:
            return "UNKNOWN";
        default:
            return "invalid value";
        }
    }

    std::ostream &operator<<(std::ostream &os, const fan_t &fan)
    {
        os << ToString(fan);
        return os;
    }

} // namespace fuji_iot

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

#include "fuji_register.h"
#include <sys/types.h>

namespace fuji_iot
{

    const static uint8_t kModeIndex = 0;
    const static uint8_t kModeMask = 0b00001110;
    const static uint8_t kModeOffset = 1;

    const static uint8_t kFanIndex = 0;
    const static uint8_t kFanMask = 0b01110000;
    const static uint8_t kFanOffset = 4;

    const static uint8_t kEnabledIndex = 0;
    const static uint8_t kEnabledMask = 0b00000001;
    const static uint8_t kEnabledOffset = 0;

    const static uint8_t kErrorIndex = 0;
    const static uint8_t kErrorMask = 0b10000000;
    const static uint8_t kErrorOffset = 7;

    const static uint8_t kEconomyIndex = 1;
    const static uint8_t kEconomyMask = 0b10000000;
    const static uint8_t kEconomyOffset = 7;

    const static uint8_t kTemperatureIndex = 1;
    const static uint8_t kTemperatureMask = 0b01111111;
    const static uint8_t kTemperatureOffset = 0;

    const static uint8_t kUpdateMagicIndex = 2;
    const static uint8_t kUpdateMagicMask = 0b11110000;
    const static uint8_t kUpdateMagicOffset = 4;

    const static uint8_t kSwingIndex = 2;
    const static uint8_t kSwingMask = 0b00000100;
    const static uint8_t kSwingOffset = 2;

    const static uint8_t kSwingStepIndex = 2;
    const static uint8_t kSwingStepMask = 0b00000010;
    const static uint8_t kSwingStepOffset = 1;

    const static uint8_t kControllerPresentIndex = 3;
    const static uint8_t kControllerPresentMask = 0b00000001;
    const static uint8_t kControllerPresentOffset = 0;

    FujiRegister::FujiRegister(uint8_t *data)
    {
        bytes_ = data;
    }

    std::array<uint8_t, 5> FujiRegister::Payload() const
    {
        std::array<uint8_t, 5> ret;
        std::copy(bytes_, bytes_ + 5, ret.begin());
        return ret;
    }

    FujiStatusRegister::FujiStatusRegister(uint8_t *data) : FujiRegister(data) {}

    void FujiStatusRegister::SetMode(mode_t mode)
    {
        bytes_[kModeIndex] = (bytes_[kModeIndex] & ~kModeMask) |
                             (static_cast<uint8_t>(mode) << kModeOffset);
    }

    const mode_t FujiStatusRegister::Mode() const
    {
        switch ((bytes_[kModeIndex] & kModeMask) >> kModeOffset)
        {
        case static_cast<uint8_t>(mode_t::AUTO):
            return mode_t::AUTO;
        case static_cast<uint8_t>(mode_t::COOL):
            return mode_t::COOL;
        case static_cast<uint8_t>(mode_t::DRY):
            return mode_t::DRY;
        case static_cast<uint8_t>(mode_t::FAN):
            return mode_t::FAN;
        case static_cast<uint8_t>(mode_t::HEAT):
            return mode_t::HEAT;
        default:
            return mode_t::UNKNOWN;
        }
    }

    void FujiStatusRegister::SetEnabled(bool enabled)
    {
        bytes_[kModeIndex] = (bytes_[kEnabledIndex] & ~kEnabledMask) |
                             ((uint8_t)enabled << kEnabledOffset);
    }

    const bool FujiStatusRegister::Enabled() const
    {
        return (bytes_[kEnabledIndex] & kEnabledMask) >> kEnabledOffset;
    }

    void FujiStatusRegister::SetFan(fan_t fan)
    {
        bytes_[kFanIndex] = (bytes_[kFanIndex] & ~kFanMask) |
                            (static_cast<uint8_t>(fan) << kFanOffset);
    }

    const fan_t FujiStatusRegister::Fan() const
    {
        switch ((bytes_[kFanIndex] & kFanMask) >> kFanOffset)
        {
        case static_cast<uint8_t>(fan_t::AUTO):
            return fan_t::AUTO;
        case static_cast<uint8_t>(fan_t::MAX):
            return fan_t::MAX;
        case static_cast<uint8_t>(fan_t::HIGH):
            return fan_t::HIGH;
        case static_cast<uint8_t>(fan_t::MEDIUM):
            return fan_t::MEDIUM;
        case static_cast<uint8_t>(fan_t::LOW):
            return fan_t::LOW;
        default:
            // TODO: Log an error.
            return fan_t::UNKNOWN;
        }
    }

    void FujiStatusRegister::SetError(bool error)
    {
        bytes_[kErrorIndex] = (bytes_[kErrorIndex] & ~kErrorMask) |
                              (static_cast<uint8_t>(error) << kErrorOffset);
    }

    const bool FujiStatusRegister::Error() const
    {
        return (bytes_[kErrorIndex] & kErrorMask) >> kErrorOffset;
    }

    void FujiStatusRegister::SetEconomy(bool economy)
    {
        bytes_[kEconomyIndex] = (bytes_[kEconomyIndex] & ~kEconomyMask) |
                                (static_cast<uint8_t>(economy) << kEconomyOffset);
    }

    const bool FujiStatusRegister::Economy() const
    {
        return (bytes_[kEconomyIndex] & kEconomyMask) >> kEconomyOffset;
    }

    void FujiStatusRegister::SetTemperature(uint8_t temp)
    {
        bytes_[kTemperatureIndex] = (bytes_[kTemperatureIndex] & ~kTemperatureMask) |
                                    (temp << kTemperatureOffset);
    }

    const uint8_t FujiStatusRegister::Temperature() const
    {
        return (bytes_[kTemperatureIndex] & kTemperatureMask) >> kTemperatureOffset;
    }

    void FujiStatusRegister::SetSwing(bool swing)
    {
        bytes_[kSwingIndex] = (bytes_[kSwingIndex] & ~kSwingMask) |
                              (swing << kSwingOffset);
    }

    const bool FujiStatusRegister::Swing() const
    {
        return (bytes_[kSwingIndex] & kSwingMask) >> kSwingOffset;
    }

    void FujiStatusRegister::SetSwingStep(bool swing)
    {
        bytes_[kSwingStepIndex] = (bytes_[kSwingStepIndex] & ~kSwingStepMask) |
                                  (swing << kSwingStepOffset);
    }

    const bool FujiStatusRegister::SwingStep() const
    {
        return (bytes_[kSwingStepIndex] & kSwingStepMask) >> kSwingStepOffset;
    }

    void FujiStatusRegister::SetControllerPresent(bool controller_present)
    {
        bytes_[kControllerPresentIndex] = (bytes_[kControllerPresentIndex] & ~kControllerPresentMask) |
                                          (controller_present << kControllerPresentOffset);
    }

    const bool FujiStatusRegister::ControllerPresent() const
    {
        return (bytes_[kControllerPresentIndex] & kControllerPresentMask) >> kControllerPresentOffset;
    }

    void FujiStatusRegister::SetUpdateMagic(uint8_t update_magic)
    {
        bytes_[kUpdateMagicIndex] = (bytes_[kUpdateMagicIndex] & ~kUpdateMagicMask) |
                                    (update_magic << kUpdateMagicOffset);
    }

    const uint8_t FujiStatusRegister::UpdateMagic() const
    {
        return (bytes_[kUpdateMagicIndex] & kUpdateMagicMask) >> kUpdateMagicOffset;
    }

    const std::string FujiStatusRegister::DebugInfo() const
    {
        std::string str;
        str.append(" Mode: ");
        str.append(ToString(Mode()));
        str.append(" Fan: ");
        str.append(ToString(Fan()));
        str.append(" Enabled: ");
        str.append(std::to_string(Enabled()));
        str.append(" Error: ");
        str.append(std::to_string(Error()));
        str.append(" Economy: ");
        str.append(std::to_string(Economy()));
        str.append(" Temperature: ");
        str.append(std::to_string(Temperature()));
        str.append(" Swing: ");
        str.append(std::to_string(Swing()));
        str.append(" Swing step: ");
        str.append(std::to_string(SwingStep()));
        str.append(" Controller present: ");
        str.append(std::to_string(ControllerPresent()));
        str.append(" Update magic: ");
        str.append(std::to_string(UpdateMagic()));
        return str;
    }

} // namespace fuji
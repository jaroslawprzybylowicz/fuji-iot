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

#include "fuji_ac_state.h"
#include "fuji_register.h"

namespace fuji_iot
{

    mode_t FujiAcState::Mode() const
    {
        return mode_;
    }

    void FujiAcState::SetMode(mode_t mode)
    {
        mode_ = mode;
        merged_ = false;
    }

    bool FujiAcState::Enabled() const
    {
        return enabled_;
    }

    void FujiAcState::SetEnabled(bool enabled)
    {
        enabled_ = enabled;
        merged_ = false;
    }

    fan_t FujiAcState::Fan() const
    {
        return fan_;
    }

    void FujiAcState::SetFan(fan_t fan)
    {
        fan_ = fan;
        merged_ = false;
    }

    bool FujiAcState::ErrorFlag() const
    {
        return error_flag_;
    }

    void FujiAcState::SetErrorFlag(bool error_flag)
    {
        error_flag_ = error_flag;
        merged_ = false;
    }

    bool FujiAcState::Economy() const
    {
        return economy_;
    }
    void FujiAcState::SetEconomy(bool economy)
    {
        economy_ = economy;
        merged_ = false;
    }

    uint8_t FujiAcState::Temperature() const
    {
        return temperature_;
    }

    void FujiAcState::SetTemperature(uint8_t temp)
    {
        temperature_ = temp;
        merged_ = false;
    }

    bool FujiAcState::Swing() const
    {
        return swing_;
    }

    void FujiAcState::SetSwing(bool swing)
    {
        swing_ = swing;
        merged_ = false;
    }

    bool FujiAcState::FujiAcState::SwingStep() const
    {
        return swing_step_;
    }

    void FujiAcState::SetSwingStep(bool swing_step)
    {
        swing_step_ = swing_step;
        merged_ = false;
    }

    bool FujiAcState::ControllerPresent() const
    {
        return controller_present_;
    }

    void FujiAcState::SetControllerPresent(bool controller_present)
    {
        controller_present_ = controller_present;
        merged_ = false;
    }

    bool FujiAcState::Merged() const
    {
        return merged_;
    }

    void FujiAcState::SetMerged(bool merged)
    {
        merged_ = merged;
    }

    void FujiAcState::BuildStatusRegisterResponse(FujiStatusRegister *status) const
    {
        status->SetEnabled(enabled_);
        status->SetMode(mode_);
        status->SetFan(fan_);
        if (mode_ != mode_t::FAN)
        {
            status->SetEconomy(economy_);
            status->SetTemperature(temperature_);
        }
        status->SetSwing(swing_);
        status->SetSwingStep(swing_step_);
    }

    void FujiAcState::MergeFromMasterStatusRegister(const FujiStatusRegister &status)
    {
        SetEnabled(status.Enabled());
        SetMode(status.Mode());
        SetFan(status.Fan());
        SetEconomy(status.Economy());
        SetTemperature(status.Temperature());
        SetSwing(status.Swing());
        SetErrorFlag(status.Error());
        SetControllerPresent(status.ControllerPresent());
        merged_ = true;
    }

} // namespace fuji_iot
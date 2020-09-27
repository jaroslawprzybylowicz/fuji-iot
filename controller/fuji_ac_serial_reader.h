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

#ifndef FUJI_AC_SERIAL_READER_H_
#define FUJI_AC_SERIAL_READER_H_

#include <memory>
#include <string>

#include "absl/types/optional.h"
#include "controller/fuji_ac_serial_interface.h"

namespace fuji_iot
{
    // Implements FujiAcSerialInterface over tty device.
    class FujiAcSerialReader : public FujiAcSerialInterface
    {
    public:
        // Creates the interface over device_name tty and configures communication parameters.
        static std::unique_ptr<FujiAcSerialReader> Build(const std::string &device_name);
        ~FujiAcSerialReader();
        virtual void WriteControllerFrame(const FujiControllerFrame &frame) override;
        virtual absl::optional<FujiMasterFrame> ReadMasterFrame() override;

    private:
        FujiAcSerialReader(const int fd);
        int fd_;
    };

} // namespace fuji_iot

#endif
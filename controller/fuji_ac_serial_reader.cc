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

#include "controller/fuji_ac_serial_reader.h"

// Ugly hack to deal with symbol conflicts.
#define termios asmtermios
#define termio asmtermio
#define winsize asmwinsize
#include <asm/termios.h>
#undef termios
#undef termio
#undef winsize

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <string>

#include "absl/time/clock.h"
#include "glog/logging.h"

namespace fuji_iot
{

    FujiAcSerialReader::FujiAcSerialReader(const int fd)
    {
        fd_ = fd;
    }

    FujiAcSerialReader::~FujiAcSerialReader()
    {
        close(fd_);
    }

    std::unique_ptr<FujiAcSerialReader> FujiAcSerialReader::Build(const std::string &device_name)
    {
        VLOG(3) << "Attempting to open " << device_name;
        int fd = open(device_name.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
        if (fd < 0)
        {
            PLOG(FATAL) << "Failed to open device: " << device_name;
        }
        std::unique_ptr<FujiAcSerialReader> reader(new FujiAcSerialReader(fd));
        VLOG(3) << "Exclusively locking " << device_name;
        if (flock(fd, LOCK_EX | LOCK_NB) < 0)
        {
            PLOG(FATAL) << "Failed to exclisively lock tty device";
        }
        struct termios tty;
        memset(&tty, 0, sizeof tty);
        VLOG(3) << "Getting attributes for tty device";
        if (tcgetattr(fd, &tty) < 0)
        {
            PLOG(FATAL) << "Error getting attributes from: " << device_name;
        }
        // Raw mode, no echo, binary
        tty.c_cflag |= (CLOCAL | CREAD);
        tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ISIG | IEXTEN);
        tty.c_oflag &= ~(OPOST | ONLCR | OCRNL);
        tty.c_iflag &= ~(INLCR | IGNCR | ICRNL | IGNBRK);
        tty.c_iflag &= ~IUCLC;
        tty.c_iflag &= ~PARMRK;

        // 8 byte word
        tty.c_cflag &= ~CSIZE;
        tty.c_cflag |= CS8;
        // One stop bit.
        tty.c_cflag &= ~(CSTOPB);
        // Parity even.
        tty.c_iflag &= ~(INPCK | ISTRIP);
        tty.c_cflag &= ~(PARODD | CMSPAR);
        tty.c_cflag |= (PARENB);
        // No flow control
        tty.c_iflag &= ~(IXON | IXOFF | IXANY);
        // No rtscts
        tty.c_cflag &= ~(CRTSCTS);
        // vmim set to zero to control byte count with read() call.
        tty.c_cc[VMIN] = 0;
        // vtime 0.3s
        tty.c_cc[VTIME] = 3;
        VLOG(3) << "Setting tty device attriubutes.";
        if (tcsetattr(fd, TCSANOW, &tty) < 0)
        {
            PLOG(FATAL) << "Error setting attributes on: " << device_name;
        }
        // Setting custom baud rate for 500 bps
        struct termios2 tio;
        VLOG(3) << "Getting tty device baud rate.";
        if (ioctl(fd, TCGETS2, &tio) < 0)
        {
            PLOG(FATAL) << "Failed TCGETS2 ioctl to device: " << device_name;
        }
        tio.c_cflag &= ~CBAUD;
        tio.c_cflag |= BOTHER;
        tio.c_ispeed = 500;
        tio.c_ospeed = 500;
        VLOG(3) << "Setting tty device baud rate.";
        if (ioctl(fd, TCSETS2, &tio) < 0)
        {
            PLOG(FATAL) << "Failed TCSETS2 ioctl to device: " << device_name;
        }
        if (ioctl(fd, TCGETS2, &tio) < 0)
        {
            PLOG(FATAL) << "Failed TCGETS2 ioctl to device: " << device_name;
        }
        LOG(INFO) << "Finished device setup. Baud rate is: " << tio.c_ospeed;
        VLOG(3) << "Enabling blocking mode";
        if (fcntl(fd, F_SETFL, 0) < 0)
        {
            PLOG(FATAL) << "Failed to set blocking mode: " << device_name;
        }
        return reader;
    }

    void FujiAcSerialReader::WriteControllerFrame(const FujiControllerFrame &frame)
    {
        std::array<uint8_t, 8> data = frame.FullFrame();
        for (int i = 0; i < 8; i++)
        {
            data[i] ^= 0xFF;
        }
        VLOG(3) << "Writing to device";
        if (write(fd_, data.data(), 8) != 8)
        {
            PLOG(FATAL) << "Failed to write to device";
        }
        VLOG(3) << "Succesfully wrote frame to device";
    }

    absl::optional<FujiMasterFrame> FujiAcSerialReader::ReadMasterFrame()
    {
        std::array<uint8_t, 8> data;
        VLOG(3) << "reading from tty device";
        int bytes = read(fd_, data.data(), 8);
        if (bytes < 0)
        {
            PLOG(FATAL) << "Failed to read from device";
        }
        VLOG(3) << "read " << bytes << " bytes";
        absl::SleepFor(absl::Milliseconds(30));
        if (bytes < 8)
        {
            VLOG(3) << "Skipping incomplete frame: " << bytes;
            return absl::optional<FujiMasterFrame>();
        }
        else
        {
            for (int i = 0; i < 8; i++)
            {
                data[i] ^= 0xFF;
            }
            FujiMasterFrame f(data);
            VLOG(3) << "Got master frame: " << f;
            return f;
        }
    }
} // namespace fuji_iot

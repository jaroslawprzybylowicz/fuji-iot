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

#include "gtest/gtest.h"

namespace fuji_iot {
namespace test {

class FujiFrameTest : public testing::Test {
 protected:
  void MakeMasterFrame(std::array<uint8_t, 8> data) {
    frame_ = std::unique_ptr<FujiMasterFrame>(new FujiMasterFrame(data));
  }

  void SetUp() {
    std::array<uint8_t, 8> data = {0x00, 0x00, 0x00, 0x00,
                                   0x00, 0x00, 0x00, 0x00};
    frame_ = std::unique_ptr<FujiMasterFrame>(new FujiMasterFrame(data));
  }

  std::unique_ptr<FujiMasterFrame> frame_;
};

TEST_F(FujiFrameTest, Address) {
  MakeMasterFrame({0x00, 0x81, 0x00, 0x46, 0x12, 0xa0, 0x00, 0x20});
  EXPECT_EQ(frame_->Destination(), DestinationAddr::OTHER);
  MakeMasterFrame({0x00, 0xa0, 0x00, 0x46, 0x12, 0xa0, 0x00, 0x20});
  EXPECT_EQ(frame_->Destination(), DestinationAddr::WIRED_CONTROLLER_ADDR);
  MakeMasterFrame({0x00, 0xf0, 0x00, 0x46, 0x12, 0xa0, 0x00, 0x20});
  EXPECT_EQ(frame_->Destination(), DestinationAddr::UNKNOWN);
}

TEST_F(FujiFrameTest, Type) {
  MakeMasterFrame({0x00, 0x81, 0x00, 0x46, 0x12, 0xa0, 0x00, 0x20});
  EXPECT_EQ(frame_->Type(), RegisterType::STATUS);
  MakeMasterFrame({0x00, 0xa0, 0x20, 0x46, 0x12, 0xa0, 0x00, 0x20});
  EXPECT_EQ(frame_->Type(), RegisterType::LOGIN);
  MakeMasterFrame({0x00, 0xf0, 0x10, 0x46, 0x12, 0xa0, 0x00, 0x20});
  EXPECT_EQ(frame_->Type(), RegisterType::ERROR);
  MakeMasterFrame({0x00, 0xa0, 0x70, 0x46, 0x12, 0xa0, 0x00, 0x20});
  EXPECT_EQ(frame_->Type(), RegisterType::UNKNOWN);
}

TEST_F(FujiFrameTest, Payload) {
  MakeMasterFrame({0x00, 0x81, 0x00, 0x01, 0x23, 0x34, 0x56, 0x78});
  EXPECT_EQ(frame_->Payload(),
            (std::array<uint8_t, 5>({0x01, 0x23, 0x34, 0x56, 0x78})));
}

TEST_F(FujiFrameTest, TypeAccessor) {
  frame_->WithType(RegisterType::STATUS);
  EXPECT_EQ(RegisterType::STATUS, frame_->Type());
  frame_->WithType(RegisterType::ERROR);
  EXPECT_EQ(RegisterType::ERROR, frame_->Type());
  frame_->WithType(RegisterType::LOGIN);
  EXPECT_EQ(RegisterType::LOGIN, frame_->Type());
}

}  // namespace test
}  // namespace fuji_iot
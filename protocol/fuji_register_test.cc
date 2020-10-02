#include "fuji_register.h"

#include <algorithm>

#include "gtest/gtest.h"

namespace fuji_iot {
namespace test {

class FujiStatusRegisterTest : public testing::Test {
 protected:
  void SetUp() override {
    data_.fill(0);
    reg_ = std::unique_ptr<FujiStatusRegister>(
        new FujiStatusRegister(data_.data()));
  }

  std::array<uint8_t, 5> data_;
  std::unique_ptr<FujiStatusRegister> reg_;
};

TEST_F(FujiStatusRegisterTest, InitializesCorrectly) {
  EXPECT_EQ(reg_->Mode(), mode_t::UNKNOWN);
  EXPECT_EQ(reg_->Swing(), false);
  EXPECT_EQ(reg_->SwingStep(), false);
  EXPECT_EQ(reg_->Temperature(), 0);
  EXPECT_EQ(reg_->Fan(), fan_t::AUTO);
  EXPECT_EQ(reg_->Error(), false);
  EXPECT_EQ(reg_->Enabled(), false);
  EXPECT_EQ(reg_->Economy(), false);
  EXPECT_EQ(reg_->ControllerPresent(), false);
}

TEST_F(FujiStatusRegisterTest, ModeSwitches) {
  for (auto v : all_mode_t) {
    reg_->SetMode(v);
    EXPECT_EQ(reg_->Mode(), v);
    uint8_t val = static_cast<uint8_t>(v) << 1;
    std::array<uint8_t, 5> expected = {val, 0, 0, 0, 0};
    EXPECT_EQ(data_, expected);
  }
}

TEST_F(FujiStatusRegisterTest, FanSwitches) {
  for (auto v : all_fan_t) {
    reg_->SetFan(v);
    EXPECT_EQ(reg_->Fan(), v);
    uint8_t val = static_cast<uint8_t>(v) << 4;
    std::array<uint8_t, 5> expected = {val, 0, 0, 0, 0};
    EXPECT_EQ(data_, expected);
  }
}

TEST_F(FujiStatusRegisterTest, ControllerPresent) {
  reg_->SetControllerPresent(true);
  EXPECT_EQ(reg_->ControllerPresent(), true);
  std::array<uint8_t, 5> expected = {0, 0, 0, 0b1, 0};
  EXPECT_EQ(data_, expected);
}

TEST_F(FujiStatusRegisterTest, Economy) {
  reg_->SetEconomy(true);
  EXPECT_EQ(reg_->Economy(), true);
  std::array<uint8_t, 5> expected = {0, 0b10000000, 0, 0, 0};
  EXPECT_EQ(data_, expected);
}

TEST_F(FujiStatusRegisterTest, Enabled) {
  reg_->SetEnabled(true);
  EXPECT_EQ(reg_->Enabled(), true);
  std::array<uint8_t, 5> expected = {0b1, 0, 0, 0, 0};
  EXPECT_EQ(data_, expected);
}

TEST_F(FujiStatusRegisterTest, Error) {
  reg_->SetError(true);
  EXPECT_EQ(reg_->Error(), true);
  std::array<uint8_t, 5> expected = {0b10000000, 0, 0, 0, 0};
  EXPECT_EQ(data_, expected);
}

TEST_F(FujiStatusRegisterTest, Swing) {
  reg_->SetSwing(true);
  EXPECT_EQ(reg_->Swing(), true);
  std::array<uint8_t, 5> expected = {0, 0, 0b100, 0, 0};
  EXPECT_EQ(data_, expected);
}

TEST_F(FujiStatusRegisterTest, SwingStep) {
  reg_->SetSwingStep(true);
  EXPECT_EQ(reg_->SwingStep(), true);
  std::array<uint8_t, 5> expected = {0, 0, 0b10, 0, 0};
  EXPECT_EQ(data_, expected);
}

TEST_F(FujiStatusRegisterTest, Temperature) {
  reg_->SetTemperature(10);
  EXPECT_EQ(reg_->Temperature(), 10);
  std::array<uint8_t, 5> expected = {0, 10, 0, 0, 0};
  EXPECT_EQ(data_, expected);
  reg_->SetTemperature(20);
  EXPECT_EQ(reg_->Temperature(), 20);
  expected = {0, 20, 0, 0, 0};
  EXPECT_EQ(data_, expected);
  reg_->SetTemperature(30);
  EXPECT_EQ(reg_->Temperature(), 30);
  expected = {0, 30, 0, 0, 0};
  EXPECT_EQ(data_, expected);
}

}  // namespace test
}  // namespace fuji_iot

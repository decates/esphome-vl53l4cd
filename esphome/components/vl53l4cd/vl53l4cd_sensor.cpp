#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/i2c/i2c.h"
#include "vl53l4cd_class.h"


namespace esphome {
namespace vl53l4cd {

static const char *const TAG = "vl53l4cd";

//class VL53L4CDSensor : public sensor::Sensor, public PollingComponent, public i2c::I2CDevice {
// public:
void VL53L4CDSensor::setup() {
  ESP_LOGCONFIG(TAG, "Setting up VL53L4CD Sensor...");
  this->trigger_pin_->setup();
  this->trigger_pin_->digital_write(false);
  this->echo_pin_->setup();
  // isr is faster to access
  echo_isr_ = echo_pin_->to_isr();

//    DEV_I2C.begin(); // Assume covered by base class
//
    // Configure VL53L4CD satellite component.
    this->sensor_.begin();
//
//    // Switch off VL53L4CD satellite component.
    this->sensor_.VL53L4CD_Off();
//
//    //Initialize VL53L4CD satellite component.
    this->sensor_.InitSensor();
//
//    // Program the highest possible TimingBudget, without enabling the
//    // low power mode. This should give the best accuracy
    this->sensor_.VL53L4CD_SetRangeTiming(this->timing_budget_, 0);
//
//    // Start Measurements
    this->sensor_.VL53L4CD_StartRanging();

}

void VL53L4CDSensor::dump_config() {

  // GPIOPin *enable_pin_{nullptr};
  // uint32_t timing_budget_{33};  // Default timing budget in ms
  LOG_SENSOR("", "VL53L4CD", this);
  LOG_UPDATE_INTERVAL(this);
  LOG_I2C_DEVICE(this);
  if (this->enable_pin_ != nullptr) {
    LOG_PIN("  Enable Pin: ", this->enable_pin_);
  }
  ESP_LOGCONFIG(TAG, "  Timing budget: %u%s", this->timing_budget_, this->timing_budget_ > 0 ? "us" : " (no timing budget)");
}



void VL53L4CDSensor::update() {
  uint8_t NewDataReady = 0;
  VL53L4CD_Result_t results;
  uint8_t status;
  char report[64];

  do {
    status = this->sensor_.VL53L4CD_CheckForDataReady(&NewDataReady);
  } while (!NewDataReady);

  if ((!status) && (NewDataReady != 0)) {
    // (Mandatory) Clear HW interrupt to restart measurements
    this->sensor_.VL53L4CD_ClearInterrupt();

    // Read measured distance. RangeStatus = 0 means valid data
    this->sensor_.VL53L4CD_GetResult(&results);
    snprintf(report, sizeof(report), "Status = %3u, Distance = %5u mm, Signal = %6u kcps/spad\r\n",
             results.range_status,
             results.distance_mm,
             results.signal_per_spad_kcps);
    // SerialPort.print(report);
    ESP_LOGD(TAG, report);
    this->publish_state(results.distance_mm);
  } else {
    this->publish_state(NAN);
  }
}

void VL53L4CDSensor::set_enable_pin(GPIOPin *pin) { enable_pin_ = pin; }
void VL53L4CDSensor::set_timing_budget(uint32_t timing_budget_ms) { timing_budget_ = timing_budget_ms; }

}  // namespace vl53l4cd
}  // namespace esphome

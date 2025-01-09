#pragma once

#include <list>

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/i2c/i2c.h"
#include "stm32duino_VL53L4CD/vl53l4cd_class.h"

namespace esphome {
namespace vl53l4cd {

class VL53L4CDSensor : public sensor::Sensor, public PollingComponent, public i2c::I2CDevice {
 public:
  void setup() override;
  void dump_config() override;
  void update() override;

  void set_enable_pin(GPIOPin *pin) { enable_pin_ = pin; }
  void set_timing_budget(uint32_t timing_budget_ms) { timing_budget_ = timing_budget_ms; }
  // void set_measurement_interval(uint32_t interval_ms) { measurement_interval_ = interval_ms; }

 protected:
  VL53L4CD sensor_{&Wire, -1};  // Wire object and default pin
  GPIOPin *enable_pin_{nullptr};
  uint32_t timing_budget_{33};  // Default timing budget in ms
  // uint32_t measurement_interval_{0};  // Measurement interval (0 = use update_interval)
};

}  // namespace vl53l4cd
}  // namespace esphome

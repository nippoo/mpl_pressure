#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace mpl_pressure {

class MPLComponent : public PollingComponent, public i2c::I2CDevice {
 public:
  void set_temperature(sensor::Sensor *temperature) { temperature_ = temperature; }
  void set_pressure(sensor::Sensor *pressure) { pressure_ = pressure; }

  /// Schedule temperature+pressure readings.
  void update() override;
  /// Setup the sensor and test for a connection.
  void setup() override;
  void dump_config() override;

  float get_setup_priority() const override;

 protected:
  /// Internal method to read the temperature and pressure from the component after it has been scheduled.
  void read_temp_press_();

  sensor::Sensor *temperature_{nullptr};
  sensor::Sensor *pressure_{nullptr};
};

}  // namespace mpl_pressure
}  // namespace esphome

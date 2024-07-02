#include "mpl_pressure.h"
#include "esphome/core/log.h"

namespace esphome {
namespace mpl_pressure {

static const char *const TAG = "mpl_pressure.sensor";

static const uint8_t MPL_ADDRESS = 0x18;
static const double outputmax = 15099494; // output at maximum pressure [counts]
static const double outputmin = 1677722; // output at minimum pressure [counts]
static const double pmax = 25; // maximum value of pressure range [bar, psi, kPa, etc.]

void MPLComponent::update() {
  uint8_t cmd[3] = {0xAA, 0x00, 0x00};
  if (!this->write(cmd, 4, true)) {
    this->mark_failed();
    return;
  }
  this->set_timeout("temperature", 10, [this]() { this->read_temp_press_(); });
}

void MPLComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up MPL...");
  uint8_t cmd[3] = {0xAA, 0x00, 0x00};
  if (!this->write(cmd, 4, true)) {
    this->mark_failed();
    return;
  }
}

void MPLComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "MPL:");
  LOG_I2C_DEVICE(this);
  if (this->is_failed()) {
    ESP_LOGE(TAG, "Connection with MPL failed!");
  }
  LOG_UPDATE_INTERVAL(this);

  LOG_SENSOR("  ", "Temperature", this->temperature_);
  LOG_SENSOR("  ", "Pressure", this->pressure_);
}

void MPLComponent::read_temp_press_() {
  uint8_t data[7];

  if (!this->read_bytes_raw(data, 7)) {
    this->status_set_warning();
    return;
  }

  double press_counts = data[3] + data[2] * 256 + data[1] * 65536; // calculate digital pressure counts
  double temp_counts = data[6] + data[5] * 256 + data[4] * 65536; // calculate digital temperature counts
  double temperature = (temp_counts * 200 / 16777215) - 50; // calculate temperature in deg c
  double percentage = (press_counts / 16777215) * 100; // calculate pressure as percentage of full scale
  //calculation of pressure value according to equation 2 of datasheet
  double pressure = 68.9476 * ((press_counts - outputmin) * (pmax)) / (outputmax - outputmin);

  ESP_LOGD(TAG, "Got Temperature=%.1f °C", temperature);
  ESP_LOGD(TAG, "Got Pressure=%.1f hPa", pressure);

  if (this->temperature_ != nullptr)
    this->temperature_->publish_state(temperature);
  this->status_clear_warning();

  if (this->pressure_ != nullptr)
    this->pressure_->publish_state(pressure);
  this->status_clear_warning();
}

float MPLComponent::get_setup_priority() const { return setup_priority::DATA; }

}  // namespace mpl_pressure
}  // namespace esphome

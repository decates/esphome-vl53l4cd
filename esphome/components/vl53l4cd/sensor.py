import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, sensor
from esphome.const import (
    STATE_CLASS_MEASUREMENT,
    UNIT_METER,
    ICON_ARROW_EXPAND_VERTICAL,
    CONF_ADDRESS,
    CONF_TIMEOUT,
    CONF_ENABLE_PIN,
)
from esphome import pins

DEPENDENCIES = ["i2c"]
AUTO_LOAD = ["sensor"]

vl53l4cd_ns = cg.esphome_ns.namespace("vl53l4cd")
VL53L4CDSensor = vl53l4cd_ns.class_(
    "VL53L4CDSensor", sensor.Sensor, cg.PollingComponent, i2c.I2CDevice
)

CONF_TIMING_BUDGET = "timing_budget"

def check_keys(obj):
    if obj[CONF_ADDRESS] != 0x29 and CONF_ENABLE_PIN not in obj:
        msg = "Address other then 0x29 requires enable_pin definition to allow sensor\r"
        msg += "re-addressing. Also if you have more then one VL53 device on the same\r"
        msg += "i2c bus, then all VL53 devices must have enable_pin defined."
        raise cv.Invalid(msg)
    return obj

# def check_keys(config):
#     # Ensure measurement_interval is greater than timing_budget if both are set
#     timing_budget = int(config[CONF_TIMING_BUDGET].strip("ms"))
#
#     if measurement_interval is not None and measurement_interval < timing_budget:
#         raise cv.Invalid(
#             f"measurement_interval ({measurement_interval}ms) must be greater than or equal to "
#             f"timing_budget ({timing_budget}ms)."
#         )
#
#     return config


CONFIG_SCHEMA = cv.All(
    sensor.sensor_schema(
        VL53L4CDSensor,
        unit_of_measurement=UNIT_METER,
        icon=ICON_ARROW_EXPAND_VERTICAL,
        accuracy_decimals=3,
        state_class=STATE_CLASS_MEASUREMENT,
    )
    .extend(
        {
            cv.Optional(CONF_TIMING_BUDGET, default="33ms"): cv.All(
                cv.string_strict, cv.one_of("15ms", "20ms", "33ms", "50ms", upper=False)
            ),
            # cv.Optional(CONF_MEASUREMENT_INTERVAL): cv.positive_time_period_milliseconds,
            cv.Optional(CONF_ENABLE_PIN): pins.gpio_output_pin_schema,
        }
    )
    .extend(cv.polling_component_schema("60s"))
    .extend(i2c.i2c_device_schema(0x29)),
)

async def to_code(config):
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)
    cg.add(var.set_timing_budget(config[CONF_TIMING_BUDGET]))

    if CONF_ENABLE_PIN in config:
        enable = await cg.gpio_pin_expression(config[CONF_ENABLE_PIN])
        cg.add(var.set_enable_pin(enable))

    # if CONF_MEASUREMENT_INTERVAL in config:
    #     cg.add(var.set_measurement_interval(config[CONF_MEASUREMENT_INTERVAL]))
    # else:
    #     cg.add(var.set_measurement_interval(cg.literal("default")))

    await i2c.register_i2c_device(var, config)

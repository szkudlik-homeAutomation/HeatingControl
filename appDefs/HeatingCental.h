#pragma once

#define CONFIG_HEATING_CIRCLE_CONTROL_SERVLET 1
#define CONFIG_HEATING_CIRCLE_CONTROL 1
#define CONFIG_HEATING_CIRCLE_CONTROL_STATUS_SENSOR 1

// stop the pumps if heating water supply temp drops below target_temp + PUMP_DISABLE_THOLD_SHIFT
#define PUMP_DISABLE_THOLD_SHIFT -4

// never stop pumps if heating water supply temp is higher than PUMP_DISABLE_MAX_TEMP
#define PUMP_DISABLE_MAX_TEMP 36

// restart pump if heating storage temp raise higer than target_temp + PUMP_ENABLE_THOLD_SHIFT
#define PUMP_ENABLE_THOLD_SHIFT 0

// always restart pumps if heating water supply temp is higher than PUMP_ENABLE_PUMPS_FORCE
#define PUMP_FORCE_ENABLE_TEMP 36

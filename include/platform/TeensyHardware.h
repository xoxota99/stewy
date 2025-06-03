#pragma once
/**
 * @file TeensyHardware.h
 * @brief Teensy-specific hardware functionality
 * 
 * This file contains Teensy-specific hardware functionality
 * for the Stewart platform.
 * 
 * @author Philippe Desrosiers
 * @copyright Copyright (C) 2018 Philippe Desrosiers
 * @license GPL-3.0
 */

#include <Arduino.h>

namespace stewy {
namespace platform {

/**
 * @class TeensyHardware
 * @brief Teensy-specific hardware functionality
 * 
 * This class provides Teensy-specific hardware functionality
 * for the Stewart platform.
 */
class TeensyHardware {
public:
  /**
   * @brief Restart the Teensy
   */
  static void restart() {
#ifdef CORE_TEENSY
    // Software reset macros / MMap FOR TEENSY ONLY
    #define CPU_RESTART_VAL 0x5FA0004                         // write this magic number...
    #define CPU_RESTART_ADDR (uint32_t *)0xE000ED0C           // to this memory location...
    #define CPU_RESTART (*CPU_RESTART_ADDR = CPU_RESTART_VAL) // presto!
    CPU_RESTART;
#else
    // For other platforms, just reset the Arduino
    asm volatile ("jmp 0");
#endif
  }
  
  /**
   * @brief Check if running on USB power
   * 
   * @return true if running on USB power
   * @return false if running on battery power
   */
  static bool isUsbPowered() {
    // This is a placeholder - actual implementation would depend on hardware
    // For example, reading an analog pin connected to a voltage divider
    return true;
  }
};

} // namespace platform
} // namespace stewy

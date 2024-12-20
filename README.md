# BarLED

A versatile Arduino library for driving LED bar displays and arrays using common shift registers and LED driver chips. Features automatic cascading, built-in animations, and brightness control.

## Overview

BarLED simplifies LED bar display control by providing a unified interface for popular LED drivers like 74HC595, MBI5027, and WS2803. Whether you're building a VU meter, progress bar, or animated display, BarLED handles the complexity of chip communication and pattern generation.

### Key Features

- 🔌 Multi-chip support: 74HC595, MBI5027, WS2803
- 🔗 Automatic cascading for larger displays
- ✨ Built-in animation patterns
- 🎚️ Brightness control (supported chips)
- 🚀 Efficient memory usage
- 📚 Simple, consistent API
- ⚡ Hardware-optimized communication

### Perfect For

- Audio VU meters
- Progress indicators
- Status displays
- Light animations
- LED bar graphs
- Sequential indicators
- Loading animations
- Level meters

## Getting Started

```cpp
#include <BarLED.h>

BarLED leds;

void setup() {
  // Setup for a 10-segment display using 74HC595
  leds.begin(11, 12, 8, BarLED::TYPE_74HC595, 8, 2);
  
  // Turn on segments 1,3,5
  leds.setBars(135);
  
  // Or start a chase pattern
  leds.setBars(0, BarLED::PATTERN_CHASE);
}

void loop() {
  leds.update();  // Update any active patterns
}
```

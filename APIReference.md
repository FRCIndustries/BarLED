# BarLED API Documentation

## Table of Contents
- [Installation](#installation)
- [Basic Usage](#basic-usage)
- [Hardware Setup](#hardware-setup)
- [Class Reference](#class-reference)
- [Pattern System](#pattern-system)
- [Examples](#examples)
- [Troubleshooting](#troubleshooting)

## Installation

### Using Arduino Library Manager
1. Open Arduino IDE
2. Go to `Sketch > Include Library > Manage Libraries`
3. Search for "BarLED"
4. Click Install

### Manual Installation
1. Download the latest release
2. In Arduino IDE: `Sketch > Include Library > Add .ZIP Library`
3. Select the downloaded ZIP file

## Basic Usage

```cpp
#include <BarLED.h>

BarLED leds;

void setup() {
  // Initialize with data, clock, and latch pins
  leds.begin(11, 12, 8, BarLED::TYPE_74HC595, 8, 2);
}

void loop() {
  // Must be called in loop when using patterns
  leds.update();
}
```

## Hardware Setup

### 74HC595 Configuration
```
Arduino Pin -> 74HC595
Data  (11) -> DS   (14)
Clock (12) -> SHCP (11)
Latch (8)  -> STCP (12)
             VCC  (16) -> 5V
             GND  (8)  -> GND
             MR   (10) -> 5V
             OE   (13) -> GND
```

### MBI5027 Configuration
```
Arduino Pin -> MBI5027
Data  (11) -> SDI  (2)
Clock (12) -> CLK  (3)
Latch (8)  -> LE   (4)
             VDD  (1)  -> 5V
             GND  (16) -> GND
             OE   (13) -> GND/PWM Pin
```

### WS2803 Configuration
```
Arduino Pin -> WS2803
Data  (11) -> SDI  (1)
Clock (12) -> CLK  (3)
Latch (8)  -> LAT  (4)
             VDD  (2)  -> 5V
             GND  (8)  -> GND
```

## Class Reference

### Initialization

```cpp
void begin(uint8_t dataPin, uint8_t clockPin, uint8_t latchPin, 
           uint8_t type, uint8_t pinsPerChip, uint8_t numberOfChips)
```
- `dataPin`: Arduino pin connected to data input
- `clockPin`: Arduino pin connected to clock input
- `latchPin`: Arduino pin connected to latch input
- `type`: Chip type (TYPE_74HC595, TYPE_MBI5027, or TYPE_WS2803)
- `pinsPerChip`: Number of outputs per chip (8 for 595, 16 for MBI5027, 18 for WS2803)
- `numberOfChips`: Number of cascaded chips

### Core Functions

```cpp
void setBars(uint32_t pinCombination, uint8_t pattern = PATTERN_NONE)
```
Set specific LEDs or start a pattern
- `pinCombination`: Decimal number representing LED combination (e.g., 135 turns on LEDs 1,3,5)
- `pattern`: Optional pattern to run (default: PATTERN_NONE)

```cpp
void clearAll()
```
Turn off all LEDs

```cpp
void setAll()
```
Turn on all LEDs

```cpp
void update()
```
Update pattern animation (must be called in loop)

### Pattern Control

```cpp
void setPatternSpeed(uint16_t speedMs)
```
Set pattern animation speed in milliseconds

```cpp
void stopPattern()
```
Stop current pattern

```cpp
void pausePattern()
```
Pause current pattern

```cpp
void resumePattern()
```
Resume paused pattern

```cpp
void setPatternRepeat(uint8_t times)
```
Set number of pattern repetitions (0 = infinite)

```cpp
bool isPatternComplete()
```
Check if pattern has completed all repetitions

```cpp
uint8_t getCurrentPattern()
```
Get currently running pattern

### Brightness Control

```cpp
void setBrightness(uint8_t brightness)
```
Set LED brightness (0-255, MBI5027 and WS2803 only)

## Pattern System

Available patterns:
- `PATTERN_NONE`: Direct LED control
- `PATTERN_INCREMENT`: Sequential light-up
- `PATTERN_DECREMENT`: Sequential light-down
- `PATTERN_CHASE`: Moving dot
- `PATTERN_GROW`: Expanding bar
- `PATTERN_SHRINK`: Contracting bar
- `PATTERN_HEARTBEAT`: Pulsing effect
- `PATTERN_BOUNCE`: Bouncing dot
- `PATTERN_RANDOM`: Random LED activation

## Examples

### Basic LED Control
```cpp
// Turn on LEDs 1, 3, and 5
leds.setBars(135);

// Turn on all LEDs
leds.setAll();

// Clear display
leds.clearAll();
```

### Pattern Usage
```cpp
// Start chase pattern
leds.setBars(0, BarLED::PATTERN_CHASE);

// Set pattern speed to 200ms
leds.setPatternSpeed(200);

// Run pattern 5 times
leds.setPatternRepeat(5);

// Wait for pattern completion
while(!leds.isPatternComplete()) {
  leds.update();
}
```

### Brightness Control
```cpp
// For MBI5027 and WS2803
leds.setBrightness(128);  // 50% brightness
```

### Cascaded Setup
```cpp
// Setup for 3 cascaded 74HC595s (24 LEDs total)
leds.begin(11, 12, 8, BarLED::TYPE_74HC595, 8, 3);
```

## Troubleshooting

### Common Issues

1. **No LEDs Light Up**
   - Check power connections
   - Verify pin connections
   - Ensure OE/Enable pins are properly connected
   - Check if brightness is not set to 0

2. **Erratic Behavior**
   - Verify clock and data timing
   - Check for proper grounding
   - Ensure update() is called in loop

3. **Cascading Issues**
   - Verify chip-to-chip connections
   - Check number of chips matches initialization
   - Ensure consistent power supply

### Best Practices

1. **Power Supply**
   - Use adequate power supply for number of LEDs
   - Keep wire lengths short
   - Use capacitors for stability

2. **Signal Integrity**
   - Keep data/clock lines short
   - Use pull-up resistors if needed
   - Consider logic level conversion for 3.3V systems

3. **Code Structure**
   - Always call update() in loop
   - Check pattern completion before starting new ones
   - Clear display when switching modes

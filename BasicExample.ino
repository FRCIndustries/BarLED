/******************************************************************************
===============================================================================
===============================================================================
  BarLED Basic Example
  
  Shows how to use the BarLED library with a 10-segment LED bar
  using two cascaded 74HC595 shift registers.

  BasicExample.ino
  Version 1.0.0

  by Chris Formeister

  COPYRIGHT (c) 2024, FRC Industries & Chris Formeister. All Rights Reserved.
==============================================================================
==============================================================================
******************************************************************************/

/*
IMPORTANT:  
  Circuit:
  - Data pin:  11
  - Clock pin: 12
  - Latch pin: 8
  - First 74HC595 outputs (Q0-Q7) connected to LEDs 1-8
  - Second 74HC595 Q0-Q1 connected to LEDs 9-10
*/

#include <BarLED.h>

// Create BarLED instance
BarLED leds;

void setup() {
  // Initialize BarLED
  // Parameters: data pin, clock pin, latch pin, chip type, pins per chip, number of chips
  leds.begin(11, 12, 8, BarLED::TYPE_74HC595, 8, 2);
  
  // Optional: set pattern speed (default is 100ms)
  leds.setPatternSpeed(150);
}

void loop() {
  // Light up segments one by one
  for(int i = 1; i <= 10; i++) {
    leds.setBars(i);
    delay(200);
  }
  delay(1000);
  
  // Turn all segments off
  leds.clearAll();
  delay(500);
  
  // Turn on combination of segments
  leds.setBars(13579);  // Turn on segments 1,3,5,7,9
  delay(1000);
  
  // Run chase pattern
  leds.setBars(0, BarLED::PATTERN_CHASE);
  
  // Let pattern run for 3 seconds
  // Note: must call update() in loop for patterns to work
  unsigned long startTime = millis();
  while(millis() - startTime < 3000) {
    leds.update();
  }
  
  // Stop pattern and clear display
  leds.clearAll();
  delay(500);
}

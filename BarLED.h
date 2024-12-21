// BarLED.h
#ifndef BARLED_H
#define BARLED_H

#include <Arduino.h>

class BarLED {
  public:
    // Device types
    static const uint8_t TYPE_74HC595 = 0;
    static const uint8_t TYPE_MBI5027 = 1;
    static const uint8_t TYPE_WS2803 = 2;
    
    // Pattern types
    static const uint8_t PATTERN_NONE = 0;
    static const uint8_t PATTERN_INCREMENT = 1;
    static const uint8_t PATTERN_DECREMENT = 2;
    static const uint8_t PATTERN_CHASE = 3;
    static const uint8_t PATTERN_GROW = 4;
    static const uint8_t PATTERN_SHRINK = 5;
    static const uint8_t PATTERN_HEARTBEAT = 6;
    static const uint8_t PATTERN_BOUNCE = 7;
    static const uint8_t PATTERN_RANDOM = 8;
    static const uint8_t PATTERN_WAVE = 9;
    static const uint8_t PATTERN_ALTERNATING = 10;
    
    // Error codes
    static const uint8_t ERROR_NONE = 0;
    static const uint8_t ERROR_INVALID_CHIP = 1;
    static const uint8_t ERROR_INVALID_PINS = 2;
    static const uint8_t ERROR_INVALID_PATTERN = 3;
    
    // Constructor/Destructor
    BarLED();
    ~BarLED();
    
    // Core functions
    uint8_t begin(uint8_t dataPin, uint8_t clockPin, uint8_t latchPin, 
                 uint8_t type, uint8_t pinsPerChip, uint8_t numberOfChips);
    void setBars(uint32_t pinCombination, uint8_t pattern = PATTERN_NONE);
    void clearAll();
    void setAll();
    void update();
    
    // Pattern control
    void setPatternSpeed(uint16_t speedMs);
    void stopPattern();
    void pausePattern();
    void resumePattern();
    void setPatternRepeat(uint8_t times);   // 0 = infinite
    bool isPatternComplete();
    uint8_t getCurrentPattern();
    
    // Brightness control (MBI5027/WS2803 only)
    void setBrightness(uint8_t brightness);
    
    // Status and debug
    uint8_t getLastError();
    void setDebugMode(bool enable);
    
  private:
    // Configuration structure
    struct ChipConfig {
      uint8_t dataPin;
      uint8_t clockPin;
      uint8_t latchPin;
      uint8_t deviceType;
      uint8_t numPins;
      uint16_t* states;
      uint8_t numChips;
      uint8_t currentPattern;
      uint32_t lastUpdate;
      uint8_t patternStep;
      uint16_t patternSpeed;
      bool patternPaused;
      uint8_t repeatCount;
      uint8_t maxRepeats;
      uint8_t brightness;
      uint8_t lastError;
      bool debugMode;
    };
    
    ChipConfig _config;
    
    // Private helper functions
    void sendData();
    void updatePattern();
    void executePattern(uint8_t pattern);
    bool validateConfig();
    void debugPrint(const char* message);
    void setError(uint8_t error);
    uint16_t generatePatternState(uint8_t pattern, uint8_t step);
    void shiftBits(uint16_t data, uint8_t bits);
};

#endif

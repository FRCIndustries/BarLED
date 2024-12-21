// BarLED.cpp
#include "BarLED.h"

// Constructor
BarLED::BarLED() {
  memset(&_config, 0, sizeof(ChipConfig));
  _config.patternSpeed = 100;  // Default 100ms
  _config.brightness = 255;     // Full brightness
  _config.debugMode = false;
}

// Destructor
BarLED::~BarLED() {
  if(_config.states) {
    free(_config.states);
    _config.states = nullptr;
  }
}

// Initialize the LED driver
uint8_t BarLED::begin(uint8_t dataPin, uint8_t clockPin, uint8_t latchPin, 
                      uint8_t type, uint8_t pinsPerChip, uint8_t numberOfChips) {
  // Validate parameters
  if(type > TYPE_WS2803) {
    setError(ERROR_INVALID_CHIP);
    return ERROR_INVALID_CHIP;
  }
  
  if(pinsPerChip == 0 || numberOfChips == 0) {
    setError(ERROR_INVALID_PINS);
    return ERROR_INVALID_PINS;
  }
  
  // Store configuration
  _config.dataPin = dataPin;
  _config.clockPin = clockPin;
  _config.latchPin = latchPin;
  _config.deviceType = type;
  _config.numPins = pinsPerChip;
  _config.numChips = numberOfChips;
  
  // Allocate state memory
  if(_config.states) {
    free(_config.states);
  }
  _config.states = (uint16_t*)malloc(numberOfChips * sizeof(uint16_t));
  if(!_config.states) {
    return ERROR_INVALID_PINS;
  }
  memset(_config.states, 0, numberOfChips * sizeof(uint16_t));
  
  // Configure pins
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  
  digitalWrite(dataPin, LOW);
  digitalWrite(clockPin, LOW);
  digitalWrite(latchPin, LOW);
  
  // Initial chip setup based on type
  switch(type) {
    case TYPE_WS2803:
      // WS2803 needs a reset pulse
      digitalWrite(latchPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(latchPin, LOW);
      break;
  }
  
  return ERROR_NONE;
}

// Set LED states or pattern
void BarLED::setBars(uint32_t pinCombination, uint8_t pattern) {
  if(pattern > PATTERN_ALTERNATING) {
    setError(ERROR_INVALID_PATTERN);
    return;
  }
  
  if(pattern != PATTERN_NONE) {
    _config.currentPattern = pattern;
    _config.patternStep = 0;
    _config.repeatCount = 0;
    _config.lastUpdate = millis();
    memset(_config.states, 0, _config.numChips * sizeof(uint16_t));
  } else {
    // Process direct LED control
    _config.currentPattern = PATTERN_NONE;
    memset(_config.states, 0, _config.numChips * sizeof(uint16_t));
    
    // Handle single pin
    if(pinCombination < (_config.numPins * _config.numChips + 1)) {
      if(pinCombination > 0) {
        uint8_t chip = (pinCombination - 1) / _config.numPins;
        uint8_t pin = (pinCombination - 1) % _config.numPins;
        _config.states[chip] |= (1 << pin);
      }
    } 
    // Handle combinations
    else {
      while(pinCombination > 0) {
        uint8_t digit = pinCombination % 10;
        if(digit > 0 && digit <= (_config.numPins * _config.numChips)) {
          uint8_t chip = (digit - 1) / _config.numPins;
          uint8_t pin = (digit - 1) % _config.numPins;
          _config.states[chip] |= (1 << pin);
        }
        pinCombination /= 10;
      }
    }
  }
  
  sendData();
}

// Clear all LEDs
void BarLED::clearAll() {
  _config.currentPattern = PATTERN_NONE;
  memset(_config.states, 0, _config.numChips * sizeof(uint16_t));
  sendData();
}

// Set all LEDs
void BarLED::setAll() {
  _config.currentPattern = PATTERN_NONE;
  for(uint8_t i = 0; i < _config.numChips; i++) {
    _config.states[i] = 0xFFFF;
  }
  sendData();
}

// Update pattern animation
void BarLED::update() {
  if(_config.currentPattern == PATTERN_NONE || _config.patternPaused) {
    return;
  }
  
  uint32_t now = millis();
  if(now - _config.lastUpdate < _config.patternSpeed) {
    return;
  }
  
  _config.lastUpdate = now;
  updatePattern();
}

// Set pattern animation speed
void BarLED::setPatternSpeed(uint16_t speedMs) {
  _config.patternSpeed = speedMs;
}

// Stop current pattern
void BarLED::stopPattern() {
  _config.currentPattern = PATTERN_NONE;
  memset(_config.states, 0, _config.numChips * sizeof(uint16_t));
  sendData();
}

// Pause pattern
void BarLED::pausePattern() {
  _config.patternPaused = true;
}

// Resume pattern
void BarLED::resumePattern() {
  _config.patternPaused = false;
  _config.lastUpdate = millis();
}

// Set pattern repeat count
void BarLED::setPatternRepeat(uint8_t times) {
  _config.maxRepeats = times;
  _config.repeatCount = 0;
}

// Check if pattern is complete
bool BarLED::isPatternComplete() {
  return _config.currentPattern == PATTERN_NONE || 
         (_config.maxRepeats > 0 && _config.repeatCount >= _config.maxRepeats);
}

// Get current pattern
uint8_t BarLED::getCurrentPattern() {
  return _config.currentPattern;
}

// Set LED brightness
void BarLED::setBrightness(uint8_t brightness) {
  _config.brightness = brightness;
  if(_config.deviceType == TYPE_MBI5027 || _config.deviceType == TYPE_WS2803) {
    sendData();
  }
}

// Get last error
uint8_t BarLED::getLastError() {
  return _config.lastError;
}

// Set debug mode
void BarLED::setDebugMode(bool enable) {
  _config.debugMode = enable;
}

// Private: Send data to chips
void BarLED::sendData() {
  digitalWrite(_config.latchPin, LOW);
  
  for(int chip = _config.numChips - 1; chip >= 0; chip--) {
    switch(_config.deviceType) {
      case TYPE_74HC595:
        shiftOut(_config.dataPin, _config.clockPin, MSBFIRST, (_config.states[chip] >> 8) & 0xFF);
        shiftOut(_config.dataPin, _config.clockPin, MSBFIRST, _config.states[chip] & 0xFF);
        break;
        
      case TYPE_MBI5027:
        shiftBits(_config.states[chip], 16);
        break;
        
      case TYPE_WS2803:
        // WS2803 needs specific timing
        for(int i = 17; i >= 0; i--) {
          digitalWrite(_config.dataPin, (_config.states[chip] >> i) & 1);
          digitalWrite(_config.clockPin, HIGH);
          delayMicroseconds(1);  // WS2803 timing requirement
          digitalWrite(_config.clockPin, LOW);
        }
        break;
    }
  }
  
  // Handle chip-specific latch timing
  switch(_config.deviceType) {
    case TYPE_WS2803:
      delayMicroseconds(500);
      break;
  }
  
  digitalWrite(_config.latchPin, HIGH);
}

// Private: Update pattern animation
void BarLED::updatePattern() {
  if(_config.currentPattern == PATTERN_NONE) return;
  
  memset(_config.states, 0, _config.numChips * sizeof(uint16_t));
  
  uint16_t patternState = generatePatternState(_config.currentPattern, _config.patternStep);
  
  // Distribute pattern state across chips
  for(uint8_t chip = 0; chip < _config.numChips; chip++) {
    _config.states[chip] = (patternState >> (chip * _config.numPins)) & ((1 << _config.numPins) - 1);
  }
  
  sendData();
  
  _config.patternStep++;
  uint16_t maxSteps = _config.numPins * _config.numChips;
  
  if(_config.patternStep >= maxSteps) {
    _config.patternStep = 0;
    _config.repeatCount++;
    
    if(_config.maxRepeats > 0 && _config.repeatCount >= _config.maxRepeats) {
      stopPattern();
    }
  }
}

// Private: Generate pattern state
uint16_t BarLED::generatePatternState(uint8_t pattern, uint8_t step) {
  uint16_t state = 0;
  uint16_t totalPins = _config.numPins * _config.numChips;
  
  switch(pattern) {
    case PATTERN_INCREMENT:
      state = 1 << step;
      break;
      
    case PATTERN_DECREMENT:
      state = 1 << (totalPins - 1 - step);
      break;
      
    case PATTERN_CHASE:
      state = 1 << (step % totalPins);
      break;
      
    case PATTERN_GROW:
      for(uint8_t i = 0; i <= step; i++) {
        state |= (1 << i);
      }
      break;
      
    case PATTERN_SHRINK:
      for(uint8_t i = step; i < totalPins; i++) {
        state |= (1 << i);
      }
      break;
      
    case PATTERN_HEARTBEAT: {
      uint8_t halfStep = totalPins / 2;
      if(step < halfStep) {
        for(uint8_t i = 0; i <= step; i++) {
          state |= (1 << i);
          state |= (1 << (totalPins - 1 - i));
        }
      } else {
        for(uint8_t i = 0; i < (totalPins - step + halfStep); i++) {
          state |= (1 << i);
          state |= (1 << (totalPins - 1 - i));
        }
      }
      break;
    }
    
    case PATTERN_BOUNCE: {
      uint8_t bouncePos;
      if(step < totalPins) {
        bouncePos = step;
      }
	  else {
        bouncePos = (2 * totalPins - step - 2);
      }
      state = 1 << bouncePos;
      break;
    }
    
    case PATTERN_RANDOM:
      state = random(1 << totalPins);
      break;
      
    case PATTERN_WAVE: {
      for(uint8_t i = 0; i < totalPins; i++) {
        if((i + step) % 3 == 0) {
          state |= (1 << i);
        }
      }
      break;
    }
    
    case PATTERN_ALTERNATING: {
      state = (step % 2) ? 0x5555 : 0xAAAA;
      state &= ((1 << totalPins) - 1);
      break;
    }
  }
  
  return state;
}

// Private: Shift out bits with timing control
void BarLED::shiftBits(uint16_t data, uint8_t bits) {
  for(int8_t i = bits - 1; i >= 0; i--) {
    digitalWrite(_config.dataPin, (data >> i) & 1);
    digitalWrite(_config.clockPin, HIGH);
    
    // Add device-specific timing delays if needed
    switch(_config.deviceType) {
      case TYPE_MBI5027:
        delayMicroseconds(1);
        break;
    }
    
    digitalWrite(_config.clockPin, LOW);
  }
}

// Private: Set error code and debug print
void BarLED::setError(uint8_t error) {
  _config.lastError = error;
  if(_config.debugMode) {
    debugPrint("Error: ");
    switch(error) {
      case ERROR_INVALID_CHIP:
        debugPrint("Invalid chip type");
        break;
      case ERROR_INVALID_PINS:
        debugPrint("Invalid pin configuration");
        break;
      case ERROR_INVALID_PATTERN:
        debugPrint("Invalid pattern");
        break;
    }
  }
}

// Private: Debug print helper
void BarLED::debugPrint(const char* message) {
  if(_config.debugMode) {
    Serial.println(message);
  }
}

// Private: Validate configuration
bool BarLED::validateConfig() {
  if(_config.deviceType > TYPE_WS2803) {
    setError(ERROR_INVALID_CHIP);
    return false;
  }
  
  if(!_config.states) {
    setError(ERROR_INVALID_PINS);
    return false;
  }
  
  return true;
}
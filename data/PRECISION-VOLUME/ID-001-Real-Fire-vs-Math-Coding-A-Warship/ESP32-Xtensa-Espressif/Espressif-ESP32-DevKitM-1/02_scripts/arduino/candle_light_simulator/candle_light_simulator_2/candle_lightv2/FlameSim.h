#pragma once
#include <Arduino.h>

class FlameSim {
public:
    FlameSim(uint8_t pin, uint8_t channel = 0);

    void begin(uint16_t freq = 5000, uint8_t resolution = 10);

    void update();

    void triggerDisturbance();

    void setBaseLevel(float level);
    void setAmplitude(float amp);

private:
    uint8_t _pin;
    uint8_t _channel;

    uint16_t _maxPWM;

    float _baseLevel;
    float _amplitude;

    enum State {
        IDLE,
        DROP,
        CHAOTIC,
        HARMONIC
    };

    State _state;

    unsigned long _lastUpdate;
    float _t;

    // internal helpers
    float applyGamma(float x);
    void writePWM(float value);

    float generateDrop();
    float generateChaos();
    float generateHarmonic();
};
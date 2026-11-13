#include "FlameSim.h"

FlameSim::FlameSim(uint8_t pin, uint8_t channel)
    : _pin(pin), _channel(channel), _state(IDLE), _t(0) {}

void FlameSim::begin(uint16_t freq, uint8_t resolution) {
    ledcAttach(_pin, freq, resolution);

    _maxPWM = (1 << resolution) - 1;

    _baseLevel = 0.6 * _maxPWM;
    _amplitude = 0.12 * _maxPWM;

    _lastUpdate = millis();
}

void FlameSim::setBaseLevel(float level) {
    _baseLevel = level;
}

void FlameSim::setAmplitude(float amp) {
    _amplitude = amp;
}

void FlameSim::triggerDisturbance() {
    _state = DROP;
    _t = 0;
}

void FlameSim::update() {
    if (millis() - _lastUpdate < 10) return;
    _lastUpdate = millis();

    float value = _baseLevel;

    switch (_state) {
        case IDLE:
            value += generateChaos() * 0.3; // subtle idle flicker
            break;

        case DROP:
            value = generateDrop();
            if (_t > 1.0) {
                _state = CHAOTIC;
                _t = 0;
            }
            break;

        case CHAOTIC:
            value = _baseLevel + generateChaos();
            if (_t > 3.0) {
                _state = HARMONIC;
                _t = 0;
            }
            break;

        case HARMONIC:
            value = _baseLevel + generateHarmonic();
            if (_t > 4.0) {
                _state = IDLE;
                _t = 0;
            }
            break;
    }

    writePWM(value);
    _t += 0.01;
}

// ---------- SIGNAL GENERATORS ----------

float FlameSim::generateDrop() {
    // Fast exponential drop then recovery
    float drop = exp(-5 * _t);
    return _baseLevel * (0.5 + 0.5 * drop);
}

float FlameSim::generateChaos() {
    static float noise = 0;
    float raw = random(-100, 100) / 100.0;
    float alpha = 0.2;

    noise = alpha * raw + (1 - alpha) * noise;

    return noise * _amplitude;
}

float FlameSim::generateHarmonic() {
    float envelope = exp(-0.5 * _t);
    float freq = 8.0; // Hz

    return sin(2 * PI * freq * _t) * envelope * _amplitude;
}

// ---------- OUTPUT ----------

float FlameSim::applyGamma(float x) {
    float normalized = x / _maxPWM;
    normalized = constrain(normalized, 0.0, 1.0);

    float gamma = 2.2;
    return pow(normalized, gamma) * _maxPWM;
}

void FlameSim::writePWM(float value) {
    value = applyGamma(value);
    value = constrain(value, 0, _maxPWM);

    ledcWrite(_channel, (uint32_t)value);
}
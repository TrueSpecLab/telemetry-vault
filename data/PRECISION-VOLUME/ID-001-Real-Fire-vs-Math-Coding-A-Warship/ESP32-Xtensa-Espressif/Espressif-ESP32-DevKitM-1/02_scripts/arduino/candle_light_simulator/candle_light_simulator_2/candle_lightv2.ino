#include "FlameSim.h"

FlameSim flame(21); // LED pin

void setup() {
    flame.begin();
}

void loop() {
    flame.update();

    // Trigger disturbance occasionally
    //if (random(0, 1000) > 995) {
    //    flame.triggerDisturbance();
    //}
}
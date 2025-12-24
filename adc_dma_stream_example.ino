#include <Arduino.h>
#include <memory>
#include "include/ADCDMAStream.h"
#include "include/ADCDriver.h"
#include "include/ADCSampler.h"
#include "include/SharedArrayGuard.h"

namespace {
    constexpr std::size_t kBufferSize = 32;
    constexpr uint8_t kADCIndex = 0;
    constexpr uint8_t kPin = A9;
    constexpr uint32_t kSampleRateHz = 64U;
    constexpr uint8_t kAveraging = 0U;
    constexpr uint8_t kResolutionBits = 12U;
    ADC adc{};
    ADCDriver driver{adc, kADCIndex, kPin, kSampleRateHz, kAveraging, kResolutionBits};
    ADCDMAStream<kBufferSize> stream{};
    ADCSampler sampler{driver, stream};
}  // namespace


[[noreturn]] void fail_loop(const char* msg);

[[noreturn]] void fail_loop(const char* msg) {

    while (true) {
        delay(1000);
        Serial.println(msg);
    }
}

void setup() {
    Serial.begin(115200);
    while (!Serial && millis() < 5000) {
        // wait for USB Serial
    }

    pinMode(LED_BUILTIN, OUTPUT);

    if (!sampler.begin()) {
        fail_loop("Failed to begin ADC sampler");
    }

    Serial.println("ADC DMA stream initialized successfully");


}

void loop() {
    static uint32_t last_data_ms = millis();
    static bool led_state = false;
    if (const SharedArrayGuard guard{sampler}; guard.isValid()) {
        const auto block = guard.get();
        const uint32_t now = millis();
        // Example: compute simple min/max on this block and print.
        uint16_t min_val = 0xFFFFu;
        uint16_t max_val = 0u;

        for (const auto v : block) {
            if (v < min_val) {
                min_val = v;
            }
            if (v > max_val) {
                max_val = v;
            }
        }

        Serial.printf("Block size: %u, min: %u, max: %u,time to acquire: %u ms.\n",
                      block.size(),
                      min_val,
                      max_val,
                      static_cast<unsigned>(now - last_data_ms));

        last_data_ms = now;
        // stream.releaseBuffer();
    }

    // Optional: lightweight "heartbeat" if nothing for a while, etc.
    // Heartbeat: 1 Hz flash only if no data for >= 1000 ms
    const uint32_t now = millis();

    if (const uint32_t silence = now - last_data_ms; silence >= 2000) {
        // toggle LED every 1000 ms
        if (silence % 1000 == 0) {
            led_state = !led_state;
            digitalWriteFast(LED_BUILTIN, led_state ? HIGH : LOW);
        }
    }
}

// AdcDriver.h
#pragma once

#include <cstdint>

class ADC;
class ADC_Module;

class ADCDriver final {
public:
    ADCDriver(ADC& adc,
              int8_t adcIndex,
              uint8_t pin,
              uint32_t sampleRateHz,
              uint8_t averaging,
              uint8_t resolutionBits) noexcept;

    // Returns false if ADC module is invalid (misconfigured adcIndex)
    [[nodiscard]] bool begin() noexcept;

    [[nodiscard]] ADC& adc() const noexcept { return m_adc; }
    [[nodiscard]] int8_t adcIndex() const noexcept { return m_adcIndex; }

private:
    ADC& m_adc;
    int8_t m_adcIndex;
    uint8_t m_pin;
    uint32_t m_sampleRateHz;
    uint8_t m_averaging;
    uint8_t m_resolutionBits;
    bool m_isInitialised { false };

    [[nodiscard]] ADC_Module* module() const noexcept;
};

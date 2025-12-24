// AdcDriver.h
#pragma once

#include <cstdint>

class ADC;
class ADC_Module;

/**
 * @class ADCDriver
 * @brief Configuration and control for an ADC hardware module.
 * 
 * This class encapsulates the hardware settings for an ADC, including
 * sampling rate, resolution, and averaging.
 */
class ADCDriver final {
public:
    /**
     * @brief Constructs an ADCDriver configuration.
     * 
     * @param adc Reference to the ADC hardware object.
     * @param adcIndex Hardware index (0 or 1).
     * @param pin The analog pin to sample.
     * @param sampleRateHz Desired sampling rate in Hertz.
     * @param averaging Number of samples to average (hardware feature).
     * @param resolutionBits Bits of resolution (e.g., 10, 12).
     */
    ADCDriver(ADC& adc,
              int8_t adcIndex,
              uint8_t pin,
              uint32_t sampleRateHz,
              uint8_t averaging,
              uint8_t resolutionBits) noexcept;

    /**
     * @brief Configures the hardware with the stored settings.
     * 
     * @return false if the ADC module index is invalid or configuration fails.
     */
    [[nodiscard]] bool begin() noexcept;

    /** @brief Returns the ADC hardware reference. */
    [[nodiscard]] ADC& adc() const noexcept { return m_adc; }

    /** @brief Returns the ADC module index. */
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

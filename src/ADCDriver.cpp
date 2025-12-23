// AdcDriver.cpp
#include "../include/ADCDriver.h"
#include <Arduino.h>
#include "ADC.h"

ADCDriver::ADCDriver(ADC& adc,
    const int8_t adcIndex,
    const uint8_t pin,
    const uint32_t sampleRateHz,
    const uint8_t averaging,
    const uint8_t resolutionBits) noexcept
    : m_adc(adc)
    , m_adcIndex(adcIndex)
    , m_pin(pin)
    , m_sampleRateHz(sampleRateHz)
    , m_averaging(averaging)
    , m_resolutionBits(resolutionBits)
{
}

ADC_Module* ADCDriver::module() const noexcept
{
    if (m_adcIndex == ADC_0)
        return m_adc.adc0;
#if defined(ADC_DUAL_ADCS)
    if (m_adcIndex == ADC_1)
        return m_adc.adc1;
#endif
    return nullptr;
}

bool ADCDriver::begin() noexcept
{
    if (m_isInitialised)
        return true;
    pinMode(m_pin, INPUT_DISABLE);

    auto* mod = module();
    if (!mod)
        return false;

    mod->setAveraging(m_averaging);
    mod->setResolution(m_resolutionBits);
    mod->setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_HIGH_SPEED);
    mod->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_HIGH_SPEED);

    mod->startSingleRead(m_pin);
    mod->startTimer(m_sampleRateHz);

    m_isInitialised = true;

    return true;
}

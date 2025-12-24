// AdcSampler.h
#pragma once

#include "ADCDriver.h"
#include "ADCDMAStream.h"

/**
 * @class ADCSampler
 * @brief High-level interface for ADC sampling using DMA.
 * 
 * This class coordinates the ADCDriver (hardware config) and 
 * ADCDMAStream (data transport) to provide a simple interface for 
 * acquiring blocks of ADC data.
 * 
 * @tparam BufferSize Number of samples per buffer.
 */
template <std::size_t BufferSize>
class ADCSampler final {
public:
    /**
     * @brief Constructs a sampler from a driver and a DMA stream.
     * 
     * @param driver Configured ADC driver.
     * @param dmaStream Configured DMA stream.
     */
    ADCSampler(ADCDriver& driver, ADCDMAStream<BufferSize>& dmaStream) noexcept;

    /**
     * @brief Starts the ADC sampling process.
     * @return true if successfully started.
     */
    [[nodiscard]] bool begin() noexcept;

    /**
     * @brief Attempts to acquire a full buffer of samples.
     * 
     * @param[out] out Span to be populated with buffer location and size.
     * @return true if data is available and acquired.
     */
    [[nodiscard]] bool tryAcquire(Span<const uint16_t>& out) noexcept;

    /**
     * @brief Releases the acquired buffer.
     */
    void releaseBuffer() noexcept;

private:
    ADCDriver& m_driver;
    ADCDMAStream<BufferSize>& m_dmaStream;
    bool m_isInitialised { false };
};

// AdcSampler.h
#pragma once

#include "ADCDriver.h"
#include "ADCDMAStream.h"

template <std::size_t BufferSize>
class ADCSampler final {
public:
    ADCSampler(ADCDriver& driver, ADCDMAStream<BufferSize>& dmaStream) noexcept;

    [[nodiscard]] bool begin() noexcept;

    [[nodiscard]] bool tryAcquire(Span<const uint16_t>& out) noexcept;
    void releaseBuffer() noexcept;

private:
    ADCDriver& m_driver;
    ADCDMAStream<BufferSize>& m_dmaStream;
    bool m_isInitialised { false };
};

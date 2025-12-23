#pragma once

#include <cstddef>
#include "AnalogBufferDMA.h"
#include "Span.h"


/*
 *NOTE: Buffers are static and are shared across all instances of a given size!! Be careful not to double instantiate stuff.
 *NOTE: AnalogBufferDMA uses millis() to detect triggers. Make sure the buffer size is large enough to hold at least one ms of data!
 */

template <std::size_t BufferSize>
class ADCDMAStream final {
public:
    static_assert(BufferSize > 0, "BufferSize must be greater than zero");
    static_assert(BufferSize * sizeof(uint16_t) % 32 == 0, "BufferSize must be a multiple of 32 for DMA alignment");
    ADCDMAStream() noexcept;

    // Initialize with ADC driver pointer; returns success/failure
    [[nodiscard]] bool initialize(ADC* adc, int8_t adcIndex) noexcept;

    [[nodiscard]] bool hasFullBuffer() noexcept;
    [[nodiscard]] bool tryAcquire(Span<const uint16_t>& out) noexcept;
    void releaseBuffer() noexcept;

    [[nodiscard]] static constexpr std::size_t bufferSize() noexcept { return BufferSize; }

private:
    AnalogBufferDMA m_dma;
    bool m_hasPending { false };
    bool m_isInitialised { false };

    // Header-defined static storage per template instantiation
    static inline DMAMEM __attribute__((aligned(32)))
        volatile uint16_t s_buffer1[BufferSize];

    static inline DMAMEM __attribute__((aligned(32)))
        volatile uint16_t s_buffer2[BufferSize];
};

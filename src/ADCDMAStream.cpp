// AdcDmaStream.cpp
#include "../include/ADCDMAStream.h"
#include <Arduino.h>

template <std::size_t BufferSize>
ADCDMAStream<BufferSize>::ADCDMAStream() noexcept
    : m_dma(s_buffer1, BufferSize, s_buffer2, BufferSize)
{
}

template <std::size_t BufferSize>
bool ADCDMAStream<BufferSize>::initialize(ADC* adc, int8_t adcIndex) noexcept
{
    if (m_isInitialised)
        return true;
    if (adcIndex < 0)
        return false;

    // AnalogBufferDMA::init does not return a status, so assume success
    // unless adc pointer is invalid
    if (!adc)
        return false;

    m_dma.init(adc, adcIndex);
    m_isInitialised = true;
    return true;
}

template <std::size_t BufferSize>
bool ADCDMAStream<BufferSize>::hasFullBuffer() noexcept
{
    return m_dma.interrupted();
}

template <std::size_t BufferSize>
bool ADCDMAStream<BufferSize>::tryAcquire(Span<const uint16_t>& out) noexcept
{
    if (!m_dma.interrupted())
        return false;

    volatile uint16_t* raw = m_dma.bufferLastISRFilled();
    const std::size_t count = m_dma.bufferCountLastISRFilled();

#if defined(__IMXRT1062__)
    if (reinterpret_cast<std::uintptr_t>(raw) >= 0x20200000u) {
        void* p = const_cast<uint16_t*>(raw);
        arm_dcache_delete(p, BufferSize * sizeof(uint16_t));
    }
#endif
    //Casting away the volatile here is fine since the original buffer wasn't declared volatile.
    out = Span(const_cast<const uint16_t*>(raw), count);
    m_hasPending = true;

    return true;
}

template <std::size_t BufferSize>
void ADCDMAStream<BufferSize>::releaseBuffer() noexcept
{
    if (m_hasPending) {
        m_dma.clearInterrupt();
        m_hasPending = false;
    }
}

// Explicit instantiations (optional)
template class ADCDMAStream<32>;
template class ADCDMAStream<64>;
template class ADCDMAStream<128>;
template class ADCDMAStream<256>;
template class ADCDMAStream<512>;
template class ADCDMAStream<1024>;
template class ADCDMAStream<2048>;
template class ADCDMAStream<4096>;
template class ADCDMAStream<8192>;
template class ADCDMAStream<16384>;
template class ADCDMAStream<32256>;
template class ADCDMAStream<32736>;
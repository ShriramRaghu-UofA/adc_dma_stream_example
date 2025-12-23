// AdcSampler.cpp
#include "../include/ADCSampler.h"

template <std::size_t BufferSize>
ADCSampler<BufferSize>::ADCSampler(ADCDriver& driver,
                                   ADCDMAStream<BufferSize>& dmaStream) noexcept
    : m_driver(driver)
    , m_dmaStream(dmaStream)
{
}

template <std::size_t BufferSize>
bool ADCSampler<BufferSize>::begin() noexcept
{
    if (m_isInitialised)
        return true;
    if (!m_dmaStream.initialize(&m_driver.adc(), m_driver.adcIndex()))
        return false;
    if (!m_driver.begin())
        return false;

    m_isInitialised = true;
    return true;
}

template <std::size_t BufferSize>
bool ADCSampler<BufferSize>::tryAcquire(Span<const uint16_t>& out) noexcept
{
    return m_dmaStream.tryAcquire(out);
}

template <std::size_t BufferSize>
void ADCSampler<BufferSize>::releaseBuffer() noexcept
{
    m_dmaStream.releaseBuffer();
}

template class ADCSampler<32>;
template class ADCSampler<64>;
template class ADCSampler<1024>;
template class ADCSampler<2048>;

#pragma once

#include <cstddef>
#include "AnalogBufferDMA.h"
#include "Span.h"


/**
 * @class ADCDMAStream
 * @brief Manages DMA-backed double buffering for ADC data.
 * 
 * This class handles the low-level DMA configuration and buffer management. 
 * It uses two static buffers to allow the ADC to write to one while the application 
 * processes the other.
 * 
 * @note Buffers are static and are shared across all instances of a given size.
 * @note AnalogBufferDMA uses millis() to detect triggers. Ensure BufferSize is large 
 *       enough to hold at least 1ms of data.
 * 
 * @tparam BufferSize Number of samples (uint16_t) per buffer. 
 *                    Must be > 0, < 32768, and a multiple of 32 for DMA alignment.
 */
template <std::size_t BufferSize>
class ADCDMAStream final {
public:
    static_assert(BufferSize > 0, "BufferSize must be greater than zero");
    static_assert(BufferSize * sizeof(uint16_t) % 32 == 0, "BufferSize must be a multiple of 32 for DMA alignment");
    static_assert(BufferSize < 32768, "BufferSize must be less than 32768 to avoid overflow");

    
    /**
     * @brief Constructs an ADCDMAStream.
     */
    ADCDMAStream() noexcept;

    /**
     * @brief Initializes the DMA stream for a specific ADC module.
     * 
     * @param adc Pointer to the ADC hardware instance.
     * @param adcIndex Index of the ADC (usually 0 or 1).
     * @return true if initialization succeeded, false otherwise.
     */
    [[nodiscard]] bool initialize(ADC* adc, int8_t adcIndex) noexcept;

    /**
     * @brief Checks if a buffer is full and ready for processing.
     * @return true if a buffer is ready.
     */
    [[nodiscard]] bool hasFullBuffer() noexcept;

    /**
     * @brief Attempts to acquire the current full buffer.
     * 
     * @param[out] out A Span pointing to the acquired buffer data if successful.
     * @return true if a buffer was acquired, false if none was available.
     */
    [[nodiscard]] bool tryAcquire(Span<const uint16_t>& out) noexcept;

    /**
     * @brief Releases the currently held buffer back to the DMA system.
     * 
     * Must be called after processing the buffer acquired via tryAcquire().
     */
    void releaseBuffer() noexcept;

    /**
     * @brief Returns the size of the buffers.
     */
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

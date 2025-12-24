#pragma once

#include "Span.h"
#include "ADCSampler.h"


/**
 * @class SharedArrayGuard
 * @brief RAII guard for managing ADC buffer acquisition and release.
 * 
 * Automatically attempts to acquire a buffer from an ADCSampler on construction 
 * and ensures it is released on destruction.
 * 
 * @tparam BufferSize The buffer size matching the sampler.
 */
template <std::size_t BufferSize>
class SharedArrayGuard {

public:
    /**
     * @brief Constructs the guard and attempts to acquire a buffer.
     * @param sampler The sampler to acquire from.
     */
    explicit SharedArrayGuard(ADCSampler<BufferSize>& sampler) noexcept
        : m_sampler(sampler),
          m_buffer(Span<const uint16_t> {})
    {
        m_acquired = sampler.tryAcquire(m_buffer);
    }

    ~SharedArrayGuard() {
        if (m_acquired) m_sampler.releaseBuffer();
    }

    // Non-copyable
    SharedArrayGuard(const SharedArrayGuard&) = delete;
    SharedArrayGuard& operator=(const SharedArrayGuard&) = delete;

    // Movable
    SharedArrayGuard(SharedArrayGuard&& other) noexcept
        : m_sampler(other.m_sampler),
          m_buffer(std::exchange(other.m_buffer, {})),
          m_acquired(std::exchange(other.m_acquired, false))
    {}

    SharedArrayGuard& operator=(SharedArrayGuard&& other) noexcept {
        if (this != &other) {
            if (m_acquired) m_sampler.releaseBuffer();
            m_sampler = other.m_sampler;
            m_buffer = std::exchange(other.m_buffer, {});
            m_acquired = std::exchange(other.m_acquired, false);
        }
        return *this;
    }

    /**
     * @brief Returns a Span to the acquired data.
     * @return A valid Span if data was acquired, otherwise an empty Span.
     */
    [[nodiscard]] Span<const uint16_t> get() const noexcept {
        return m_acquired ? m_buffer : Span<const uint16_t>{};
    }

    /**
     * @brief Checks if the buffer was successfully acquired.
     */
    [[nodiscard]] bool isValid() const noexcept {
        return m_acquired;
    }

    /**
     * @brief Manually releases the buffer before the guard goes out of scope.
     */
    void release_early() noexcept {
        if (m_acquired) {
            m_sampler.releaseBuffer();
            m_acquired = false;
            m_buffer = Span<const uint16_t>{};
        }
    }

private:
    ADCSampler<BufferSize>& m_sampler;
    Span<const uint16_t> m_buffer = {};
    bool m_acquired = false;
};
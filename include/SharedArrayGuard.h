#pragma once

#include "Span.h"
#include "ADCSampler.h"


// RAII guard class
template <std::size_t BufferSize>
class SharedArrayGuard {

public:
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

    // Movable (optional but recommended)
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

    // Accessors
    [[nodiscard]] Span<const uint16_t> get() const noexcept {
        return m_acquired ? m_buffer : Span<const uint16_t>{};
    }

    [[nodiscard]] bool isValid() const noexcept {
        return m_acquired;
    }

    // Optional: Early release
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
#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <type_traits>


#if defined(__cpp_lib_span) || __cplusplus >= 202002L
#include <span>
/**
 * @typedef Span
 * @brief Alias for std::span in C++20 or later.
 */
template <typename T>
using Span = std::span<T>;
#else
/**
 * @class Span
 * @brief A lightweight non-owning view over a contiguous sequence of objects.
 * 
 * Provides a subset of C++20 std::span functionality for C++17 projects.
 * Useful for passing around array references without losing size information.
 * 
 * @tparam T The type of elements in the span.
 */
// -----------------------------------------------------------------------------
// Enhanced C++17 Span<T>
// - forward iterator
// - subspan(offset, count)
// - first(n), last(n)
// - size(), data(), operator[]
// - noexcept and constexpr where appropriate
//
// This is NOT a full C++20 span, but it captures the essentials needed for
// lightweight data slicing, iteration, and streaming to storage.
// -----------------------------------------------------------------------------

template <typename T>
class Span {
public:
    using element_type = T;
    using value_type   = std::remove_cv_t<T>;
    using pointer      = T*;
    using reference    = T&;
    using size_type    = std::size_t;

    // -------------------------------------------------------------------------
    // Iterator (simple forward iterator)
    // -------------------------------------------------------------------------
    class iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = T;
        using pointer           = T*;
        using reference         = T&;

        constexpr iterator() noexcept : ptr_{nullptr} {}
        constexpr explicit iterator(pointer p) noexcept : ptr_{p} {}

        constexpr reference operator*()  const noexcept { return *ptr_; }
        constexpr pointer   operator->() const noexcept { return ptr_; }

        // pre-increment
        constexpr iterator& operator++() noexcept {
            ++ptr_;
            return *this;
        }
        // post-increment
        constexpr iterator operator++(int) noexcept {
            iterator tmp{*this};
            ++ptr_;
            return tmp;
        }

        constexpr bool operator==(const iterator& other) const noexcept {
            return ptr_ == other.ptr_;
        }
        constexpr bool operator!=(const iterator& other) const noexcept {
            return ptr_ != other.ptr_;
        }

    private:
        pointer ptr_;
    };

    /** @brief Constructs an empty Span. */
    constexpr Span() noexcept : data_{nullptr}, size_{0} {}

    /**
     * @brief Constructs a Span from a pointer and a size.
     * @param data Pointer to the start of the sequence.
     * @param size Number of elements.
     */
    constexpr Span(pointer data, const size_type size) noexcept
        : data_{data}, size_{size} {}

    /** @brief Returns a pointer to the beginning of the sequence. */
    [[nodiscard]] constexpr pointer    data()  const noexcept { return data_; }

    /** @brief Returns the number of elements in the Span. */
    [[nodiscard]] constexpr size_type size()   const noexcept { return size_; }

    /** @brief Checks if the Span is empty. */
    [[nodiscard]] constexpr bool      empty()  const noexcept { return size_ == 0; }

    /** @brief Accesses an element by index. */
    [[nodiscard]] constexpr reference operator[](size_type idx) const {
        return data_[idx];
    }

    /** @brief Returns an iterator to the beginning. */
    [[nodiscard]] constexpr iterator begin()  const noexcept { return iterator{data_}; }

    /** @brief Returns an iterator to the end. */
    [[nodiscard]] constexpr iterator end()    const noexcept { return iterator{data_ + size_}; }

    /**
     * @brief Returns a sub-span.
     * @param offset Starting position.
     * @param count Number of elements.
     */
    [[nodiscard]] constexpr Span subspan(size_type offset, size_type count) const noexcept {
        if (offset >= size_) {
            return Span{};  // empty span
        }
        const size_type r = size_ - offset;
        const size_type n = (count <= r) ? count : r;
        return Span{data_ + offset, n};
    }

    /**
     * @brief Returns a Span containing the first @p count elements.
     */
    [[nodiscard]] constexpr Span first(size_type count) const noexcept {
        if (count >= size_) {
            return *this;
        }
        return Span{data_, count};
    }

    /**
     * @brief Returns a Span containing the last @p count elements.
     */
    [[nodiscard]] constexpr Span last(size_type count) const noexcept {
        if (count >= size_) {
            return *this;
        }
        return Span{data_ + (size_ - count), count};
    }

private:
    pointer    data_;
    size_type  size_;
};

#endif

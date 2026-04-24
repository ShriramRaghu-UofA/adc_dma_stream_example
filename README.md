# adc_dma_stream_example

A clean, idiomatic C++17 wrappers for high-performance ADC sampling using DMA on the Teensy 4.1. This project demonstrates how to bypass the overhead of traditional polling or interrupt-driven ADC reads by streaming data directly into memory buffers.

## Features

- **Idiomatic C++17**: Utilizes RAII, templates, and `Span` for memory safety and zero-cost abstractions.
- **DMA-Powered**: High-frequency sampling without CPU intervention for data transport (powered by the ADC library).
- **Buffer Safety**: Uses a `SharedArrayGuard` (RAII) to ensure acquired DMA buffers are automatically released back to the hardware.
- **Decoupled Architecture**: Separate drivers for hardware configuration (`ADCDriver`), DMA transport (`ADCDMAStream`), and high-level orchestration (`ADCSampler`).

## Getting Started

### Hardware Configuration

The example is configured by default for:
- **Pin**: `A9`
- **Resolution**: 12-bit
- **Sampling Rate**: 64 Hz (Configurable)
- **Buffer Size**: 32 samples per block

### Basic Usage

The core logic resides in the orchestration of the `ADCDriver` and the `ADCSampler`. Here is the minimal setup based on the provided example:

```c++
#include "include/ADCDMAStream.h"
#include "include/ADCDriver.h"
#include "include/ADCSampler.h"
#include "include/SharedArrayGuard.h"

// Hardware and Stream setup
ADC adc{};
ADCDriver driver{adc, 0 /* ADC Index */, A9, 64 /* Hz */, 0 /* Averaging */, 12 /* Bits */};
ADCDMAStream<32> stream{};
ADCSampler sampler{driver, stream};

void setup() {
    Serial.begin(115200);
    if (!sampler.begin()) {
        while(1) Serial.println("Init Failed");
    }
}

void loop() {
    // SharedArrayGuard handles the tryAcquire/release logic automatically
    if (const SharedArrayGuard guard{sampler}; guard.isValid()) {
        const auto block = guard.get(); // Returns a Span<const uint16_t>
        
        // Process your data block here, example:
        // C++17 Iterator access via range-based for loop
        for (const auto& sample : block) {
            // Process each sample
            Serial.println(sample);
        }
    }
}
```


## Project Structure

- `ADCDriver`: Manages the Teensy-specific ADC hardware registers and timing.
- `ADCDMAStream`: Handles the DMA circular buffer logic and hardware triggers.
- `ADCSampler`: Provides the top-level API to acquire and release data blocks.
- `SharedArrayGuard`: A RAII wrapper that ensures data blocks are never "leaked" (kept locked in DMA) if an exception or early return occurs.
- `Span`: A C++17 implementation of a non-owning view over a contiguous sequence of objects (backport of C++20 `std::span`).

## Requirements

- **Platform**: Teensy 4.1 (or compatible)
- **Framework**: Teensyduino
- **Build Tool**: PlatformIO (either the VSCode version or the CLion extension also works). 
  - If you wish to compile the project using Arduino IDE, then:
    - Move `main.cpp` out of src to the root directory
    - Rename it to `adc_dma_stream_example.ino`
    - Open it in Arduino IDE as usual.
    - Adjust include paths and compile away!

## Gotchas
Note that ADC library assumes that the buffer size can hold at least 1 millisecond worth of data as it uses millisecond-based timing for DMA triggers. This is to ensure that the DMA buffer does not overflow and data is not lost. If you need to sample at higher rates, consider increasing the buffer size or using a different sampling strategy.

## License

This project is licensed under the Apache 2.0 License - see the `LICENSE` file for details.
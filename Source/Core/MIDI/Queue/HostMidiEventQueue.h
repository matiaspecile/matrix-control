#pragma once

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include <juce_core/juce_core.h>

namespace Core
{
    /**
     * Fixed-size SPSC handoff for MIDI events crossing the audio callback boundary.
     *
     * The callback-to-worker instance carries completed host SysEx input. The worker-to-callback
     * instance carries MIDI events the DAW should route to its selected MIDI output.
     * Payload storage is preallocated so enqueueing host input never allocates or locks.
     */
    class HostMidiEventQueue
    {
    public:
        static constexpr std::size_t kCapacity = 64;
        static constexpr int kMaxMessageBytes = 4096;

        bool enqueue(const juce::uint8* data, int size, int samplePosition = 0) noexcept
        {
            if (data == nullptr || size <= 0 || size > kMaxMessageBytes)
            {
                droppedEventCount_.fetch_add(1, std::memory_order_relaxed);
                return false;
            }

            const auto write = writeIndex_.load(std::memory_order_relaxed);
            const auto read = readIndex_.load(std::memory_order_acquire);
            if (write - read >= kCapacity)
            {
                droppedEventCount_.fetch_add(1, std::memory_order_relaxed);
                return false;
            }

            auto& slot = slots_[static_cast<std::size_t>(write % kCapacity)];
            std::memcpy(slot.bytes.data(), data, static_cast<std::size_t>(size));
            slot.size = size;
            slot.samplePosition = samplePosition;
            writeIndex_.store(write + 1, std::memory_order_release);
            return true;
        }

        template <typename Consumer>
        bool tryConsumeNext(Consumer&& consumer)
        {
            const auto read = readIndex_.load(std::memory_order_relaxed);
            if (read == writeIndex_.load(std::memory_order_acquire))
                return false;

            const auto& slot = slots_[static_cast<std::size_t>(read % kCapacity)];
            consumer(slot.bytes.data(), slot.size, slot.samplePosition);
            readIndex_.store(read + 1, std::memory_order_release);
            return true;
        }

        bool isEmpty() const noexcept
        {
            return readIndex_.load(std::memory_order_acquire)
                == writeIndex_.load(std::memory_order_acquire);
        }

        bool isFull() const noexcept
        {
            const auto write = writeIndex_.load(std::memory_order_acquire);
            const auto read = readIndex_.load(std::memory_order_acquire);
            return write - read >= kCapacity;
        }

        std::uint32_t takeDroppedEventCount() noexcept
        {
            return droppedEventCount_.exchange(0, std::memory_order_acq_rel);
        }

    private:
        struct Slot
        {
            std::array<juce::uint8, static_cast<std::size_t>(kMaxMessageBytes)> bytes {};
            int size = 0;
            int samplePosition = 0;
        };

        std::array<Slot, kCapacity> slots_ {};
        alignas(64) std::atomic<std::uint64_t> writeIndex_ { 0 };
        alignas(64) std::atomic<std::uint64_t> readIndex_ { 0 };
        std::atomic<std::uint32_t> droppedEventCount_ { 0 };
    };
}

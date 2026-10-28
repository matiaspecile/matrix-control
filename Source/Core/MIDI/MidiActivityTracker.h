#pragma once

#include <array>
#include <atomic>
#include <cmath>
#include <cstddef>
#include <cstdint>

#include <juce_core/juce_core.h>

namespace Core
{
    enum class HostMidiInquiryStage : std::size_t
    {
        kGenerated = 0,
        kOutboundQueue = 1,
        kHostEventQueue = 2,
        kOutputMidiBuffer = 3
    };

    struct HostMidiInquiryDiagnostic
    {
        static constexpr std::size_t kMaxCapturedBytes = 4096;

        std::uint64_t sequence = 0;
        int totalSize = 0;
        int capturedSize = 0;
        std::array<juce::uint8, kMaxCapturedBytes> bytes {};
    };

    /// Ephemeral MIDI activity edges for instrument, editor, and host output paths (FR-9).
    /// Returns a short pulse after each notify so the ~30 Hz UI poll can catch it.
    /// Visual hold/decay is owned by Led (kReleaseTimeMs_).
    /// Writers: any producer/consumer thread (lock-free atomics). Readers: message thread only.
    class MidiActivityTracker
    {
    public:
        enum class Path
        {
            kInstrument,
            kEditor,
            kOutbound,
            kMidiFromInbound,
            kHostMidiOutput
        };

        void notifyActivity(Path path) noexcept;
        float getActivityLevel(Path path) const noexcept;

        /// Lock-free checkpoints for tracing the manual Universal Device Inquiry through host MIDI output.
        void recordHostMidiInquiry(HostMidiInquiryStage stage, const juce::uint8* data, int size) noexcept;
        HostMidiInquiryDiagnostic getHostMidiInquiryDiagnostic(HostMidiInquiryStage stage) const noexcept;

        /// Test seam — pulse math with explicit timestamps (message-thread reads only).
        static float computeLevelFromTimestamp(juce::int64 lastNotifyMs, juce::int64 nowMs) noexcept;

    private:
        struct AtomicHostMidiInquiryDiagnostic
        {
            std::atomic<std::uint64_t> version { 0 };
            std::atomic<int> totalSize { 0 };
            std::array<std::atomic<juce::uint8>, HostMidiInquiryDiagnostic::kMaxCapturedBytes> bytes {};
        };

        static constexpr std::size_t kHostMidiInquiryStageCount = 4;
        std::array<AtomicHostMidiInquiryDiagnostic, kHostMidiInquiryStageCount> hostMidiInquiryDiagnostics_ {};

        /// Coalesces bursts between UI polls; not a visual hold constant.
        inline constexpr static juce::int64 kActivityPulseMs_ = 34;

        std::atomic<juce::int64> instrumentLastNotifyMs_{ 0 };
        std::atomic<juce::int64> editorLastNotifyMs_{ 0 };
        std::atomic<juce::int64> outboundLastNotifyMs_{ 0 };
        std::atomic<juce::int64> midiFromInboundLastNotifyMs_{ 0 };
        std::atomic<juce::int64> hostMidiOutputLastNotifyMs_{ 0 };
    };
}

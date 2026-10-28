#include "Core/MIDI/MidiActivityTracker.h"

namespace Core
{
    void MidiActivityTracker::recordHostMidiInquiry(HostMidiInquiryStage stage,
                                                    const juce::uint8* data,
                                                    int size) noexcept
    {
        if (data == nullptr || size <= 0)
            return;

        const auto index = static_cast<std::size_t>(stage);
        if (index >= hostMidiInquiryDiagnostics_.size())
            return;

        auto& diagnostic = hostMidiInquiryDiagnostics_[index];
        const auto version = diagnostic.version.load(std::memory_order_seq_cst);
        diagnostic.version.store(version + 1, std::memory_order_seq_cst); // odd while writing

        const auto capturedSize = juce::jmin(size,
            static_cast<int>(HostMidiInquiryDiagnostic::kMaxCapturedBytes));
        for (int i = 0; i < capturedSize; ++i)
            diagnostic.bytes[static_cast<std::size_t>(i)].store(data[i], std::memory_order_seq_cst);
        diagnostic.totalSize.store(size, std::memory_order_seq_cst);

        diagnostic.version.store(version + 2, std::memory_order_seq_cst);
    }

    HostMidiInquiryDiagnostic MidiActivityTracker::getHostMidiInquiryDiagnostic(
        HostMidiInquiryStage stage) const noexcept
    {
        HostMidiInquiryDiagnostic result;
        const auto index = static_cast<std::size_t>(stage);
        if (index >= hostMidiInquiryDiagnostics_.size())
            return result;

        const auto& diagnostic = hostMidiInquiryDiagnostics_[index];
        for (int attempt = 0; attempt < 3; ++attempt)
        {
            const auto versionBefore = diagnostic.version.load(std::memory_order_seq_cst);
            if ((versionBefore & 1u) != 0)
                continue;

            const int totalSize = diagnostic.totalSize.load(std::memory_order_seq_cst);
            const int capturedSize = juce::jmin(totalSize,
                static_cast<int>(HostMidiInquiryDiagnostic::kMaxCapturedBytes));
            std::array<juce::uint8, HostMidiInquiryDiagnostic::kMaxCapturedBytes> bytes {};
            for (int i = 0; i < capturedSize; ++i)
                bytes[static_cast<std::size_t>(i)] =
                    diagnostic.bytes[static_cast<std::size_t>(i)].load(std::memory_order_seq_cst);

            const auto versionAfter = diagnostic.version.load(std::memory_order_seq_cst);
            if (versionBefore != versionAfter)
                continue;

            result.sequence = versionAfter / 2;
            result.totalSize = totalSize;
            result.capturedSize = capturedSize;
            result.bytes = bytes;
            return result;
        }

        return result;
    }

    float MidiActivityTracker::computeLevelFromTimestamp(juce::int64 lastNotifyMs,
                                                         juce::int64 nowMs) noexcept
    {
        if (lastNotifyMs <= 0)
            return 0.0f;

        const juce::int64 elapsed = nowMs - lastNotifyMs;

        if (elapsed <= kActivityPulseMs_)
            return 1.0f;

        return 0.0f;
    }

    void MidiActivityTracker::notifyActivity(Path path) noexcept
    {
        const auto now = static_cast<juce::int64>(juce::Time::getMillisecondCounterHiRes());

        switch (path)
        {
            case Path::kInstrument:
                instrumentLastNotifyMs_.store(now, std::memory_order_relaxed);
                break;

            case Path::kEditor:
                editorLastNotifyMs_.store(now, std::memory_order_relaxed);
                break;

            case Path::kOutbound:
                outboundLastNotifyMs_.store(now, std::memory_order_relaxed);
                break;

            case Path::kMidiFromInbound:
                midiFromInboundLastNotifyMs_.store(now, std::memory_order_relaxed);
                break;

            case Path::kHostMidiOutput:
                hostMidiOutputLastNotifyMs_.store(now, std::memory_order_relaxed);
                break;
        }
    }

    float MidiActivityTracker::getActivityLevel(Path path) const noexcept
    {
        const auto now = static_cast<juce::int64>(juce::Time::getMillisecondCounterHiRes());

        juce::int64 lastNotify = 0;

        switch (path)
        {
            case Path::kInstrument:
                lastNotify = instrumentLastNotifyMs_.load(std::memory_order_relaxed);
                break;

            case Path::kEditor:
                lastNotify = editorLastNotifyMs_.load(std::memory_order_relaxed);
                break;

            case Path::kOutbound:
                lastNotify = outboundLastNotifyMs_.load(std::memory_order_relaxed);
                break;

            case Path::kMidiFromInbound:
                lastNotify = midiFromInboundLastNotifyMs_.load(std::memory_order_relaxed);
                break;

            case Path::kHostMidiOutput:
                lastNotify = hostMidiOutputLastNotifyMs_.load(std::memory_order_relaxed);
                break;
        }

        return computeLevelFromTimestamp(lastNotify, now);
    }
}

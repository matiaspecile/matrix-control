#include "SettingsMidiPage.h"

#include <array>
#include <cstddef>

#include "Core/MIDI/MidiActivityTracker.h"

namespace
{
    constexpr std::size_t kHostMidiInquiryTraceStageCount = 4;
}

void SettingsMidiPage::updateHostMidiInquiryDiagnostics(
    const Core::MidiActivityTracker& activityTracker)
{
    static constexpr std::array<Core::HostMidiInquiryStage, kHostMidiInquiryTraceStageCount> stages {
        Core::HostMidiInquiryStage::kGenerated,
        Core::HostMidiInquiryStage::kOutboundQueue,
        Core::HostMidiInquiryStage::kHostEventQueue,
        Core::HostMidiInquiryStage::kOutputMidiBuffer
    };
    static constexpr std::array<const char*, kHostMidiInquiryTraceStageCount> stageNames {
        "GENERATED",
        "OUTBOUND QUEUE",
        "HOST EVENT QUEUE",
        "OUTPUT MIDI BUFFER"
    };

    for (std::size_t stageIndex = 0; stageIndex < stages.size(); ++stageIndex)
    {
        const auto diagnostic = activityTracker.getHostMidiInquiryDiagnostic(stages[stageIndex]);
        if (diagnostic.sequence <= displayedInquiryDiagnosticSequences_[stageIndex])
            continue;

        displayedInquiryDiagnosticSequences_[stageIndex] = diagnostic.sequence;
        juce::String line(stageNames[stageIndex]);
        line << " (" << diagnostic.totalSize << " bytes): ";

        for (int byteIndex = 0; byteIndex < diagnostic.capturedSize; ++byteIndex)
        {
            if (byteIndex > 0)
                line << ((byteIndex % 16 == 0) ? "\n  " : " ");
            line << juce::String::toHexString(
                        static_cast<int>(diagnostic.bytes[static_cast<std::size_t>(byteIndex)]))
                        .paddedLeft('0', 2)
                        .toUpperCase();
        }

        if (diagnostic.capturedSize < diagnostic.totalSize)
            line << "\n  [trace capped at " << diagnostic.capturedSize << " bytes]";

        if (waitingForInquiryTrace_)
        {
            hostMidiDiagnosticsConsole_->setText(juce::String(), juce::dontSendNotification);
            waitingForInquiryTrace_ = false;
        }

        line << "\n";
        hostMidiDiagnosticsConsole_->setText(
            hostMidiDiagnosticsConsole_->getText() + line,
            juce::dontSendNotification);
        hostMidiDiagnosticsConsole_->moveCaretToEnd();
    }
}


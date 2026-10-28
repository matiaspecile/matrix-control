// Extracted from MidiManager.cpp for modular maintenance.
// Outbound queue consumer thread and SysEx pacing.

#include <cstring>

#include "MidiManager.h"

#include "Core/Loggers/MidiLogger.h"
#include "Core/MIDI/EditorOutboundGate.h"
#include "Core/Services/DeviceTypeRegistry.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"

namespace
{
    bool isDeviceInquiryPrefix(const juce::uint8* data, int size) noexcept
    {
        if (data == nullptr || size < 2)
            return false;

        constexpr auto& expected = SysExConstants::DeviceInquiry::kRequestMessage;
        const int bytesToCompare = juce::jmin(
            size, static_cast<int>(SysExConstants::DeviceInquiry::kRequestMessageLength));
        return std::memcmp(data, expected, static_cast<std::size_t>(bytesToCompare)) == 0;
    }

    Core::MidiActivityTracker::Path pathForOutboundMessage(
        const Core::MidiOutboundQueue::Message& msg) noexcept
    {
        if (msg.category == Core::MidiOutboundQueue::MessageCategory::kSysEx)
            return Core::MidiActivityTracker::Path::kEditor;

        if (msg.midiMessage.isProgramChange())
            return Core::MidiActivityTracker::Path::kEditor;

        return Core::MidiActivityTracker::Path::kInstrument;
    }
}

void MidiManager::wakeConsumer() noexcept
{
    notify();
}

bool MidiManager::canSendSysExNow() const noexcept
{
    const auto nowMs = static_cast<juce::int64>(juce::Time::getMillisecondCounterHiRes());
    return sysExDelay_.millisUntilNextAllowed(nowMs) == 0;
}

bool MidiManager::isOutboundQueueIdle() const noexcept
{
    return outboundQueue_.isEmpty()
        && ! hasPendingSysEx_.load(std::memory_order_acquire)
        && (! usesHostMidiRouting() || hostMidiOutputQueue_.isEmpty());
}

bool MidiManager::hasOutboundIdleTimedOut(const OutboundIdlePollArgs& args) const noexcept
{
    return juce::Time::getMillisecondCounter() - args.idleStartMs
           >= static_cast<juce::uint32>(args.outboundIdleTimeoutMs);
}

void MidiManager::sendSysExWithDelay(const juce::MemoryBlock& sysExMessage,
                                     const juce::String& description,
                                     bool bypassEditorGate)
{
    if (usesHostMidiRouting())
    {
        outboundQueue_.enqueueSysEx(sysExMessage, bypassEditorGate);
        if (description == "Device Inquiry")
            activityTracker_.recordHostMidiInquiry(
                Core::HostMidiInquiryStage::kOutboundQueue,
                static_cast<const juce::uint8*>(sysExMessage.getData()),
                static_cast<int>(sysExMessage.getSize()));
        MidiLogger::getInstance().logInfo("Queued host-routed SysEx: " + description);
        return;
    }

    sysExDelay_.waitUntilReady();
    midiSender->sendSysEx(sysExMessage);
    MidiLogger::getInstance().logSysExSent(sysExMessage, description);
    sysExDelay_.recordSysExSent(static_cast<juce::int64>(juce::Time::getMillisecondCounterHiRes()));
}

void MidiManager::sendQueuedSysEx(const juce::MemoryBlock& sysExMessage, const juce::String& description)
{
    if (usesHostMidiRouting())
    {
        if (! queueHostMidiOutput(static_cast<const juce::uint8*>(sysExMessage.getData()),
                                  static_cast<int>(sysExMessage.getSize())))
        {
            updateErrorState("Host MIDI output queue is full; a MIDI event was dropped", "MIDI");
            return;
        }

        const auto* data = static_cast<const juce::uint8*>(sysExMessage.getData());
        const int size = static_cast<int>(sysExMessage.getSize());
        if (isDeviceInquiryPrefix(data, size))
            activityTracker_.recordHostMidiInquiry(
                Core::HostMidiInquiryStage::kHostEventQueue, data, size);

        MidiLogger::getInstance().logSysExSent(sysExMessage, description);
    }
    else
    {
        // MidiSender::sendSysEx logs the physical-device transmission.
        midiSender->sendSysEx(sysExMessage);
    }

    sysExDelay_.recordSysExSent(static_cast<juce::int64>(juce::Time::getMillisecondCounterHiRes()));
}

void MidiManager::dispatchRealtimeMessage(const Core::MidiOutboundQueue::Message& msg)
{
    // Program Change is editor outbound — drop if freeze/gate flipped after enqueue.
    // Panic CC / notes remain ungated (instrument path).
    if (msg.midiMessage.isProgramChange() && ! isEditorOutboundAllowed())
        return;

    if (usesHostMidiRouting())
    {
        if (! queueHostMidiOutput(msg.midiMessage.getRawData(),
                                  msg.midiMessage.getRawDataSize()))
            updateErrorState("Host MIDI output queue is full; a MIDI event was dropped", "MIDI");
    }
    else
    {
        midiSender->sendMidiMessage(msg.midiMessage);
    }

    activityTracker_.notifyActivity(pathForOutboundMessage(msg));
    activityTracker_.notifyActivity(Core::MidiActivityTracker::Path::kOutbound);
}

void MidiManager::notifyOutboundActivity(const Core::MidiOutboundQueue::Message& msg)
{
    activityTracker_.notifyActivity(pathForOutboundMessage(msg));
    activityTracker_.notifyActivity(Core::MidiActivityTracker::Path::kOutbound);
}

bool MidiManager::isEditorSysExAllowed(const juce::MemoryBlock& sysEx) const
{
    const bool deviceDetected = static_cast<bool>(apvts.state.getProperty("deviceDetected", false));
    const bool deviceMidiUnresponsive = static_cast<bool>(
        apvts.state.getProperty(Core::kDeviceMidiUnresponsiveProperty, false));
    const auto deviceType = Core::DeviceTypeRegistry::fromApvtsProperty(
        apvts.state.getProperty(MatrixDeviceTypes::kApvtsPropertyName));
    return Core::maySendEditorSysEx(deviceDetected, deviceType, sysEx, deviceMidiUnresponsive);
}

bool MidiManager::handleOutboundMessage(Core::MidiOutboundQueue::Message& msg)
{
    if (msg.category == Core::MidiOutboundQueue::MessageCategory::kRealtime)
    {
        dispatchRealtimeMessage(msg);
        return true;
    }

    if (msg.sysExData.getSize() == 0)
        return true;

    if (! msg.bypassEditorGate && ! isEditorSysExAllowed(msg.sysExData))
        return true;

    if (pendingSysEx_.has_value())
    {
        outboundQueue_.enqueueSysEx(std::move(msg.sysExData), msg.bypassEditorGate);
        return false;
    }

    if (canSendSysExNow())
    {
        sendQueuedSysEx(msg.sysExData, "QUEUED");
        notifyOutboundActivity(msg);
        return true;
    }

    pendingSysEx_ = std::move(msg);
    return false;
}

bool MidiManager::tryDispatchPendingSysEx()
{
    if (! pendingSysEx_.has_value() || ! canSendSysExNow()
        || (usesHostMidiRouting() && hostMidiOutputQueue_.isFull()))
        return false;

    try
    {
        if (! pendingSysEx_->bypassEditorGate
            && ! isEditorSysExAllowed(pendingSysEx_->sysExData))
        {
            pendingSysEx_.reset();
            return false;
        }

        sendQueuedSysEx(pendingSysEx_->sysExData, "QUEUED");
        notifyOutboundActivity(*pendingSysEx_);
        pendingSysEx_.reset();
        return true;
    }
    catch (const MidiConnectionException& e)
    {
        updateErrorState(e.getMessage(), "Connection");
    }
    catch (const std::exception& e)
    {
        updateErrorState(e.what(), "MIDI");
    }

    return false;
}

void MidiManager::drainQueuedRealtimeOnly()
{
    while (! usesHostMidiRouting() || ! hostMidiOutputQueue_.isFull())
    {
        auto message = outboundQueue_.tryDequeueRealtime();
        if (! message.has_value())
            break;

        dispatchRealtimeMessage({ Core::MidiOutboundQueue::MessageCategory::kRealtime,
                                  std::move(*message),
                                  {} });
    }
}

bool MidiManager::processDequeuedOutboundMessage(Core::MidiOutboundQueue::Message& msg)
{
    try
    {
        if (! handleOutboundMessage(msg))
        {
            // Pending SysEx parked — flush realtime queued behind it before the gate sleeps.
            drainQueuedRealtimeOnly();
            return false;
        }
    }
    catch (const MidiConnectionException& e)
    {
        updateErrorState(e.getMessage(), "Connection");
        return false;
    }
    catch (const std::exception& e)
    {
        updateErrorState(e.what(), "MIDI");
        return false;
    }

    return true;
}

bool MidiManager::processOutboundQueue()
{
    bool didWork = false;

    while (true)
    {
        if (usesHostMidiRouting() && hostMidiOutputQueue_.isFull())
            break;

        auto msg = outboundQueue_.dequeue();
        if (! msg.has_value())
            break;

        didWork = true;

        if (! processDequeuedOutboundMessage(*msg))
            break;
    }

    if ((! usesHostMidiRouting() || ! hostMidiOutputQueue_.isFull())
        && tryDispatchPendingSysEx())
        didWork = true;

    hasPendingSysEx_.store(pendingSysEx_.has_value(), std::memory_order_release);
    return didWork;
}

bool MidiManager::queueHostMidiOutput(const juce::uint8* data,
                                      int size,
                                      int samplePosition) noexcept
{
    return usesHostMidiRouting()
        && hostMidiOutputQueue_.enqueue(data, size, samplePosition);
}

void MidiManager::drainHostMidiInput()
{
    while (hostMidiInputQueue_.tryConsumeNext(
        [this](const juce::uint8* data, int size, int)
        {
            if (midiReceiver == nullptr || data == nullptr || size <= 0)
                return;

            juce::MemoryBlock completeSysEx(data, static_cast<std::size_t>(size));
            MidiLogger::getInstance().logSysExReceived(completeSysEx);
            activityTracker_.notifyActivity(Core::MidiActivityTracker::Path::kMidiFromInbound);
            midiReceiver->feedCompleteSysEx(completeSysEx);
        }))
    {
    }

    if (hostMidiInputQueue_.takeDroppedEventCount() > 0)
        updateErrorState("Host MIDI input queue is full; incoming SysEx was dropped", "MIDI");
}

void MidiManager::run()
{
    while (! threadShouldExit())
    {
        if (usesHostMidiRouting())
            drainHostMidiInput();
        else if (! midiSender->isOutputAvailable())
        {
            wait(1);
            continue;
        }

        if (processOutboundQueue())
            continue;

        int sleepMs = 1;
        if (pendingSysEx_.has_value())
        {
            const auto nowMs = static_cast<juce::int64>(juce::Time::getMillisecondCounterHiRes());
            sleepMs = juce::jmax(1, sysExDelay_.millisUntilNextAllowed(nowMs));
        }

        wait(sleepMs);
    }
}

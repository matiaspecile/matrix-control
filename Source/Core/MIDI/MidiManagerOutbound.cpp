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

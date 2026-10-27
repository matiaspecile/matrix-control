        return;

    lastInquiryInputId_ = inputId;
    lastInquiryOutputId_ = outputId;
    performDeviceInquiry();
}

void MidiManager::refreshDeviceInquiryAfterPortSync()
{
    if (! isDeviceDumpAvailable())
    {
        clearDeviceDetectionAfterPortLoss();
        return;
    }

    if (usesHostMidiRouting())
    {
        startDeviceInquiryForPortPair(juce::String("HOST MIDI INPUT"),
                                      juce::String("HOST MIDI OUTPUT"));
        updateDevicePresenceMonitoring();
        return;
    }

    const auto inputId = inputMidiPort != nullptr ? inputMidiPort->getOpenDeviceId() : juce::String();
    const auto outputId = outputMidiPort != nullptr ? outputMidiPort->getOpenDeviceId() : juce::String();

    if (inputId.isEmpty() || outputId.isEmpty())
    {
        clearDeviceDetectionAfterPortLoss();
        return;
    }

    startDeviceInquiryForPortPair(inputId, outputId);
    updateDevicePresenceMonitoring();
}

void MidiManager::startDeviceInquiryForPortPair(const juce::String& inputId,
                                                const juce::String& outputId)
{
    if (! Core::shouldStartDeviceInquiry(true,
                                         inputId,
                                         outputId,
                                         lastInquiryInputId_,
                                         lastInquiryOutputId_))
        return;

    // A changed port pair may still report the device as connected. Force a fresh Master pull
    // when the inquiry succeeds so MIDI TO reconnect mirrors a fresh detect.
    forceMasterPullOnNextInquirySuccess_ = true;
    lastInquiryInputId_ = inputId;
    lastInquiryOutputId_ = outputId;
    performDeviceInquiry();
}

void MidiManager::handleAsyncDeviceInquiryResponse(std::uint64_t token,
                                                   const juce::MemoryBlock& response)
{
    if (token != asyncRequestToken_.load(std::memory_order_acquire))
        return;

    if (sysExParser == nullptr)
    {
        finishAsyncDeviceInquiryFailure(token, "SysEx parser unavailable", "SysEx");
        return;
    }

    const auto validation = sysExParser->validateSysEx(response);
    if (! validation.isValid || validation.messageType != SysExParser::MessageType::kDeviceId)
    {
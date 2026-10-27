        midiManager->startThread();
    }
}

void PluginProcessor::releaseResources()
{
    if (shouldUseDevelopmentLogging())
    {
        disableApvtsLogging();
        closeLogFileForSession();
        developmentLoggingStarted_ = false;
    }
}

void PluginProcessor::stopMidiThread()
{
    if (midiManager != nullptr && midiManager->isThreadRunning())
    {
        midiManager->stopThread(kThreadStopTimeoutMs_);
    }
}

void PluginProcessor::processMidiMessages(juce::MidiBuffer& midiMessages)
{
    if (midiManager->usesHostMidiRouting())
    {
        auto& hostMidiScratch = hostMidiScratchBuffer_;
        hostMidiScratch.clear();

        const bool instrumentPathEnabled = getInstrumentPathEnabled(midiMessages);
        for (const auto metadata : midiMessages)
        {
            if (metadata.numBytes > 0 && metadata.data[0] == 0xF0)
                midiManager->enqueueHostMidiInputSysEx(metadata.data, metadata.numBytes);
            else
                hostMidiScratch.addEvent(metadata.data,
                                         metadata.numBytes,
                                         metadata.samplePosition);
        }

        instrumentForwarder_->observe(hostMidiScratch,
                                      instrumentPathEnabled,
                                      *midiActivityTracker_);

        midiManager->appendHostMidiOutput(hostMidiScratch);
        midiMessages.swapWith(hostMidiScratch);
        hostMidiScratch.clear();
    }
    else
    {
        instrumentForwarder_->forward(midiMessages,
                                      getInstrumentPathEnabled(midiMessages),
                                      *outboundQueue_,
                                      *midiActivityTracker_);
    }
}

void PluginProcessor::processBlock(juce::AudioBuffer<float>& audioBuffer,
                                   juce::MidiBuffer& midiMessages)
{
    processMidiMessages(midiMessages);

    auto inputBusBuffer = getBusBuffer(audioBuffer, true, 0);
    auto outputBusBuffer = getBusBuffer(audioBuffer, false, 0);

    const bool inputEnabled = getTotalNumInputChannels() > 0
                              && inputBusBuffer.getNumChannels() > 0;

    audioPassthroughProcessor_->updateChannelLayout(inputBusBuffer.getNumChannels(),
                                                    outputBusBuffer.getNumChannels(),
                                                    inputEnabled);
    audioPassthroughProcessor_->process(inputBusBuffer,
                                        outputBusBuffer,
                                        inputGainLinear_.load(std::memory_order_relaxed));
}

juce::AudioProcessorEditor* PluginProcessor::createEditor()
{
    return new PluginEditor(*this);
}

juce::AudioProcessorValueTreeState::ParameterLayout PluginProcessor::createParameterLayout()
{
    return ApvtsFactory::createParameterLayout();
}

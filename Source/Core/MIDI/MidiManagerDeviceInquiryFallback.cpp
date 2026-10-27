// Matrix-6/6R Master-parameter fallback for devices that ignore Universal Inquiry.

#include "MidiManager.h"

#include "Core/Loggers/MidiLogger.h"
#include "Core/MIDI/Matrix6MasterResponseFilter.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"

void MidiManager::sendArmedMatrix6MasterFallback(std::uint64_t token)
{
    if (token != asyncRequestToken_.load(std::memory_order_acquire))
        return;

    if (! isDeviceDumpAvailable() || midiSender == nullptr || ! midiSender->isOutputAvailable())
    {
        finishAsyncDeviceInquiryFailure(token, "MIDI ports unavailable for Matrix-6/6R fallback", "Connection");
        return;
    }

    try
    {
        if (! armAsyncMatrix6MasterFallbackCapture(token))
            return;

        const auto request = sysExEncoder->encodeRequestMessage(
            SysExConstants::RequestType::kRequestMasterParameters, 0);
        sendSysExWithDelay(request, "Matrix-6/6R discovery fallback: request Master parameters");
        scheduleMatrix6MasterFallbackTimeout(token);
    }
    catch (const MidiConnectionException& e)
    {
        finishAsyncDeviceInquiryFailure(token, e.getMessage(), "Connection");
    }
    catch (const std::exception& e)
    {
        finishAsyncDeviceInquiryFailure(token, e.what(), "SysEx");
    }
}

bool MidiManager::armAsyncMatrix6MasterFallbackCapture(std::uint64_t token)
{
    if (token != asyncRequestToken_.load(std::memory_order_acquire))
        return false;

    if (midiReceiver == nullptr)
    {
        finishAsyncDeviceInquiryFailure(token, "MIDI input unavailable for Matrix-6/6R fallback", "Connection");
        return false;
    }

    juce::WeakReference<MidiManager> weakThis(this);
    midiReceiver->armOneShotSysExCapture(
        [weakThis, token](const juce::MemoryBlock& response)
        {
            juce::MessageManager::callAsync(
                [weakThis, token, response]
                {
                    if (auto* self = weakThis.get())
                        self->handleAsyncMatrix6MasterFallbackResponse(token, response);
                });
        },
        [](const juce::MemoryBlock& sysex)
        {
            return Core::isMatrix6MasterParameterDump(sysex);
        });

    return true;
}

void MidiManager::handleAsyncMatrix6MasterFallbackResponse(std::uint64_t token,
                                                           const juce::MemoryBlock& response)
{
    if (token != asyncRequestToken_.load(std::memory_order_acquire))
        return;

    if (! Core::isMatrix6MasterParameterDump(response))
    {
        armAsyncMatrix6MasterFallbackCapture(token);
        return;
    }

    MidiLogger::getInstance().logSysExReceived(response, "Matrix-6/6R Master fallback response");

    DeviceIdInfo info {};
    info.manufacturerId = SysExConstants::DeviceInquiry::kExpectedManufacturer;
    info.familyLow = SysExConstants::DeviceInquiry::kExpectedFamily;
    info.familyHigh = SysExConstants::DeviceInquiry::kExpectedFamilyHigh;
    info.memberLow = SysExConstants::DeviceInquiry::kMatrix6MemberLow;
    info.memberHigh = SysExConstants::DeviceInquiry::kMatrix6MemberHigh;
    info.isValid = true;

    // The legacy response proves the shared Matrix-6/6R family, not the keyboard/rack form.
    finishAsyncDeviceInquirySuccess(token, info, MatrixDeviceTypes::Type::kMatrix6, true);
}

void MidiManager::scheduleMatrix6MasterFallbackTimeout(std::uint64_t token)
{
    juce::WeakReference<MidiManager> weakThis(this);
    juce::Timer::callAfterDelay(
        SysExConstants::kDefaultTimeoutMs,
        [weakThis, token]
        {
            if (auto* self = weakThis.get())
            {
                if (token != self->asyncRequestToken_.load(std::memory_order_acquire))
                    return;

                self->finishAsyncDeviceInquiryFailure(
                    token,
                    "Timeout waiting for Matrix-6/6R Master parameter response",
                    "Timeout");
            }
        });
}

// Async Master Parameter Data request (Request Data type=3) — companion to MidiManagerAsyncPatch.

#include "MidiManager.h"

#include "Core/Loggers/MidiLogger.h"
#include "Core/MIDI/Matrix6MasterResponseFilter.h"
#include "Core/MIDI/MasterPullOnConnectPolicy.h"
#include "Core/Models/Matrix6MasterModel.h"
#include "Core/Services/DeviceTypeRegistry.h"
#include "Shared/Definitions/PluginDisplayNames.h"

std::vector<juce::uint8> MidiManager::tryDecodeAsyncMasterResponse(const juce::MemoryBlock& response)
{
    if (response.getSize() == 0 || sysExParser == nullptr || sysExDecoder == nullptr)
        return {};

    if (Core::isMatrix6MasterParameterDump(response))
    {
        std::vector<juce::uint8> matrix6Data;
        if (! Core::Matrix6MasterModel::decodeSysEx(response, matrix6Data))
            return {};

        MidiLogger::getInstance().logSysExReceived(response, "Matrix-6/6R master response");
        return matrix6Data;
    }

    const auto validation = sysExParser->validateSysEx(response);
    if (! validation.isValid || validation.messageType != SysExParser::MessageType::kMaster)
        return {};

    std::vector<juce::uint8> packedData(SysExConstants::kMasterPackedDataSize);
    if (! sysExDecoder->decodeMasterSysEx(response, packedData.data()))
        return {};

    MidiLogger::getInstance().logSysExReceived(response, "master response");
    return packedData;
}

void MidiManager::armAsyncMasterCapture(std::uint64_t token)
{
    if (token != asyncRequestToken_.load(std::memory_order_acquire))
        return;

    if (midiReceiver == nullptr)
    {
        finishAsyncPackedPatch(token, {});
        return;
    }

    juce::WeakReference<MidiManager> weakThis(this);
    midiReceiver->armOneShotSysExCapture(
        [weakThis, token](const juce::MemoryBlock& response)
        {
            juce::MessageManager::callAsync(
                [weakThis, token, response]
                {
                    if (auto* self = weakThis.get())
                    {
                        if (token != self->asyncRequestToken_.load(std::memory_order_acquire))
                            return;

                        auto packed = self->tryDecodeAsyncMasterResponse(response);
                        if (! packed.empty())
                        {
                            self->finishAsyncPackedPatch(token, std::move(packed));
                            return;
                        }

                        self->armAsyncMasterCapture(token);
                    }
                });
        });
}

void MidiManager::scheduleAsyncMasterTimeout(std::uint64_t token)
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

                MidiLogger::getInstance().logWarning(
                    "Timeout waiting for SysEx response ("
                    + juce::String(SysExConstants::kDefaultTimeoutMs) + "ms)");
                self->updateErrorState("Timeout waiting for master response", "Timeout");
                self->finishAsyncPackedPatch(token, {});
            }
        });
}

void MidiManager::sendArmedMasterRequest(std::uint64_t token)
{
    if (token != asyncRequestToken_.load(std::memory_order_acquire))
        return;

    if (! isMasterParameterDataOutboundAllowed()
        || midiReceiver == nullptr
        || midiSender == nullptr
        || ! midiSender->isOutputAvailable())
    {
        finishAsyncPackedPatch(token, {});
        return;
    }

    try
    {
        auto requestMessage = sysExEncoder->encodeRequestMessage(
            SysExConstants::RequestType::kRequestMasterParameters, 0);

        armAsyncMasterCapture(token);
        sendSysExWithDelay(requestMessage, "master request");
        scheduleAsyncMasterTimeout(token);
    }
    catch (const MidiConnectionException& e)
    {
        updateErrorState(e.getMessage(), "Connection");
        finishAsyncPackedPatch(token, {});
    }
    catch (const std::exception& e)
    {
        updateErrorState(e.what(), "SysEx");
        finishAsyncPackedPatch(token, {});
    }
}

void MidiManager::scheduleOrSendArmedMasterRequest(const OutboundIdlePollArgs& args)
{
    if (args.settleMs <= 0)
    {
        sendArmedMasterRequest(args.token);
        return;
    }

    juce::WeakReference<MidiManager> weakThis(this);
    juce::Timer::callAfterDelay(args.settleMs,
                                [weakThis, token = args.token]
                                {
                                    if (auto* self = weakThis.get())
                                        self->sendArmedMasterRequest(token);
                                });
}

void MidiManager::pollOutboundIdleThenMasterRequest(OutboundIdlePollArgs args)
{
    if (args.token != asyncRequestToken_.load(std::memory_order_acquire))
        return;

    if (isOutboundQueueIdle())
    {
        scheduleOrSendArmedMasterRequest(args);
        return;
    }

    if (hasOutboundIdleTimedOut(args))
    {
        MidiLogger::getInstance().logWarning("Timeout waiting for outbound MIDI queue to go idle");
        updateErrorState("Timeout waiting for outbound MIDI queue to go idle", "Timeout");
        finishAsyncPackedPatch(args.token, {});
        return;
    }

    wakeConsumer();
    juce::WeakReference<MidiManager> weakThis(this);
    juce::Timer::callAfterDelay(1,
                                [weakThis, args]
                                {
                                    if (auto* self = weakThis.get())
                                        self->pollOutboundIdleThenMasterRequest(args);
                                });
}

void MidiManager::requestMasterDataAsync(PackedPatchCallback callback,
                                         int settleMs,
                                         int outboundIdleTimeoutMs)
{
    if (! isMasterParameterDataOutboundAllowed())
    {
        if (callback)
            callback({});
        return;
    }

    cancelPendingSysExRequest();

    const auto token = asyncRequestToken_.load(std::memory_order_acquire);
    const auto deviceType = Core::DeviceTypeRegistry::fromApvtsProperty(
        apvts.state.getProperty(MatrixDeviceTypes::kApvtsPropertyName));
    if (MatrixDeviceTypes::isMatrix6Family(deviceType))
        apvts.state.setProperty(Core::kMatrix6MasterDataReadyProperty, false, nullptr);
    pendingAsyncCallback_ = std::move(callback);
    asyncSysExCaptureActive_.store(true, std::memory_order_release);

    wakeConsumer();
    pollOutboundIdleThenMasterRequest({ token,
                                        juce::jmax(0, settleMs),
                                        juce::Time::getMillisecondCounter(),
                                        juce::jmax(0, outboundIdleTimeoutMs) });
}

void MidiManager::publishMasterPullFailureFooter()
{
    apvts.state.setProperty(
        "uiMessageText",
        juce::String(PluginDisplayNames::Settings::FooterMessages::kMasterPullFailed),
        nullptr);
    apvts.state.setProperty("uiMessageSeverity", "warning", nullptr);
}

void MidiManager::clearMasterPullFailureFooterIfPresent()
{
    if (apvts.state.getProperty("uiMessageText").toString()
        != PluginDisplayNames::Settings::FooterMessages::kMasterPullFailed)
        return;

    apvts.state.setProperty("uiMessageText", juce::String(), nullptr);
    apvts.state.setProperty("uiMessageSeverity", juce::String(), nullptr);
}

void MidiManager::deliverMasterPullResult(std::vector<juce::uint8> packed)
{
    if (packed.size() != SysExConstants::kMasterPackedDataSize
        && packed.size() != SysExConstants::kMatrix6MasterPackedDataSize)
    {
        if (suppressMasterPullFailureFooterForEmptyResult_)
        {
            MidiLogger::getInstance().logMessage(
                MidiLogger::LogLevel::kDebug,
                "Master pull on connect: aborted (pending request cancelled)");
            return;
        }

        MidiLogger::getInstance().logWarning("Master pull on connect: invalid or empty dump");
        publishMasterPullFailureFooter();
        return;
    }

    clearMasterPullFailureFooterIfPresent();
    MidiLogger::getInstance().logInfo("Master pull on connect: applying Master dump");

    if (masterPullApplyHandler_)
        masterPullApplyHandler_(std::move(packed));
}

void MidiManager::setMasterPullApplyHandler(PackedPatchCallback handler)
{
    masterPullApplyHandler_ = std::move(handler);
}

bool MidiManager::maybePullMasterAfterInquirySuccess(bool wasDetectedBeforeSuccess,
                                                     MatrixDeviceTypes::Type previousType,
                                                     MatrixDeviceTypes::Type newType,
                                                     bool forceBecausePortPairChanged)
{
    if (! Core::shouldPullMasterAfterDeviceInquirySuccess(wasDetectedBeforeSuccess,
                                                          previousType,
                                                          newType,
                                                          forceBecausePortPairChanged))
    {
        MidiLogger::getInstance().logMessage(
            MidiLogger::LogLevel::kDebug,
            "Master pull on connect: skipped by policy");
        return false;
    }

    MidiLogger::getInstance().logInfo("Master pull on connect: requesting Master Parameter Data");
    triggerMasterPullOnConnectIfAllowed();
    return asyncSysExCaptureActive_.load(std::memory_order_acquire);
}

void MidiManager::triggerMasterPullOnConnectIfAllowed()
{
    if (! isMasterParameterDataOutboundAllowed())
        return;

    const int profileDelayMs = getRequiredSysExDelayMs();
    juce::WeakReference<MidiManager> weakThis(this);
    requestMasterDataAsync(
        [weakThis](std::vector<juce::uint8> packed)
        {
            if (auto* self = weakThis.get())
                self->deliverMasterPullResult(std::move(packed));
        },
        Core::MidiRequestTiming::deviceSettleMs(profileDelayMs),
        Core::MidiRequestTiming::outboundIdleTimeoutMs(profileDelayMs));
}

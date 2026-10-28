#include "MasterParameterSysExDispatcher.h"

#include "Core/Models/ApvtsMasterMapper.h"

namespace Core
{

MasterParameterSysExDispatcher::MasterParameterSysExDispatcher(const MasterModel& masterModel,
                                                               EnqueueMasterFn enqueueMaster,
                                                               RequestDebouncedSendFn requestDebouncedSend)
    : masterModel_(masterModel)
    , enqueueMaster_(std::move(enqueueMaster))
    , requestDebouncedSend_(std::move(requestDebouncedSend))
    , masterParameterIds_(buildMasterParameterIdSet())
{
    jassert(enqueueMaster_ != nullptr);
}

void MasterParameterSysExDispatcher::dispatch(const juce::String& parameterId) const
{
    if (masterParameterIds_.count(parameterId) == 0)
        return;

    requestSend();
}

void MasterParameterSysExDispatcher::dispatchFull() const
{
    requestSend();
}

void MasterParameterSysExDispatcher::dispatchFullNow() const
{
    if (enqueueMaster_ != nullptr)
        enqueueMaster_(masterModel_.data());
}

void MasterParameterSysExDispatcher::requestSend() const
{
    if (requestDebouncedSend_ != nullptr)
    {
        requestDebouncedSend_();
        return;
    }

    dispatchFullNow();
}

std::unordered_set<juce::String> MasterParameterSysExDispatcher::buildMasterParameterIdSet()
{
    std::unordered_set<juce::String> ids;

    for (const auto& d : ApvtsMasterMapper::buildIntDescriptors())
        ids.insert(d.parameterId);

    for (const auto& d : ApvtsMasterMapper::buildChoiceDescriptors())
        ids.insert(d.parameterId);

    return ids;
}

} // namespace Core

#include "Core/Util/ComboboxPatchSendDebouncer.h"

namespace Core
{

ComboboxPatchSendDebouncer::ComboboxPatchSendDebouncer(int debounceMsOverride)
    : debounceMs_(debounceMsOverride)
{
}

ComboboxPatchSendDebouncer::~ComboboxPatchSendDebouncer()
{
    stopTimer();
}

void ComboboxPatchSendDebouncer::schedule(std::function<void()> callback)
{
    pending_ = std::move(callback);
    startTimer(debounceMs_);
}

void ComboboxPatchSendDebouncer::cancel()
{
    stopTimer();
    pending_ = nullptr;
}

void ComboboxPatchSendDebouncer::flushPendingSynchronouslyForTests()
{
    // Headless Release builds may leave pending_ set while isTimerRunning() is false
    // (Timer needs a message thread). Always drain the callback when tests ask to flush.
    stopTimer();

    if (pending_ != nullptr)
        timerCallback();
}

void ComboboxPatchSendDebouncer::timerCallback()
{
    stopTimer();

    if (pending_)
    {
        auto callback = std::move(pending_);
        pending_ = nullptr;
        callback();
    }
}

} // namespace Core

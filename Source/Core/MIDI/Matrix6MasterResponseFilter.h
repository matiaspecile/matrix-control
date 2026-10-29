#pragma once

#include <juce_core/juce_core.h>

#include "Core/MIDI/SysEx/SysExConstants.h"

namespace Core
{
// The Matrix-6/6R reply has a 4-byte header and 236 packed data bytes encoded
// as low/high nibbles. Its full length distinguishes it from Matrix-1000's
// shorter, versioned Master dump. Discovery does not consume the payload, so
// only the frame, opcode, and model-specific length are checked here.
inline bool isMatrix6MasterParameterDump(const juce::MemoryBlock& sysex) noexcept
{
    if (sysex.getSize() != SysExConstants::kMatrix6MasterMessageLength
        && sysex.getSize() != SysExConstants::kMatrix6MasterUnversionedMessageLength)
        return false;

    const auto* data = static_cast<const juce::uint8*>(sysex.getData());
    if (data[0] != SysExConstants::kSysExStart
        || data[1] != SysExConstants::kManufacturerIdOberheim
        || data[2] != SysExConstants::kDeviceIdMatrix1000
        || data[3] != SysExConstants::Opcode::kMasterParameterData
        || data[sysex.getSize() - 1] != SysExConstants::kSysExEnd)
        return false;

    return true;
}
} // namespace Core

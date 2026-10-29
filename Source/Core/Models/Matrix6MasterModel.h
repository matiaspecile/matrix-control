#pragma once

#include <algorithm>
#include <array>
#include <utility>
#include <vector>

#include <juce_core/juce_core.h>

#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/Models/MasterModel.h"

namespace Core
{
    inline constexpr const char* kMatrix6MasterDataReadyProperty = "matrix6MasterDataReady";

    // Matrix-6/6R Global Parameters block. Keep the full buffer so a write preserves
    // parameters and patch maps that the current Master UI does not expose.
    class Matrix6MasterModel
    {
    public:
        static constexpr size_t kBufferSize = SysExConstants::kMatrix6MasterPackedDataSize;

        const juce::uint8* data() const noexcept { return buffer_.data(); }
        juce::uint8* data() noexcept { return buffer_.data(); }

        void loadFrom(const juce::uint8* packedData) noexcept
        {
            jassert(packedData != nullptr);
            if (packedData != nullptr)
                std::copy_n(packedData, kBufferSize, buffer_.begin());
        }

        // Overlay only fields represented by the shared Master UI.
        void copySupportedParametersFrom(const MasterModel& source) noexcept
        {
            const auto* master = source.data();

            buffer_[1] = master[1]; // Vibrato speed
            buffer_[2] = master[4]; // Vibrato waveform
            buffer_[3] = master[5]; // Vibrato amplitude
            buffer_[4] = master[2]; // Vibrato speed modulation source
            buffer_[5] = static_cast<juce::uint8>(decodeSigned7(master[3]));
            buffer_[6] = master[6]; // Vibrato amplitude modulation source
            buffer_[7] = static_cast<juce::uint8>(decodeSigned7(master[7]));
            buffer_[8] = static_cast<juce::uint8>(master[8] & 0x3F); // Signed 6-bit tune

            constexpr std::array<size_t, 10> sharedMidiOffsets {
                11, 12, 13, 14, 17, 18, 19, 20, 32, 35
            };
            for (const auto offset : sharedMidiOffsets)
                buffer_[offset] = master[offset];
        }

        // Populate the shared M-1000 model fields for the selective APVTS mapper.
        void copySupportedParametersTo(MasterModel& target) const noexcept
        {
            auto* master = target.data();

            master[1] = buffer_[1];
            master[2] = buffer_[4];
            master[3] = static_cast<juce::uint8>(buffer_[5] & 0x3F);
            master[4] = buffer_[2];
            master[5] = buffer_[3];
            master[6] = buffer_[6];
            master[7] = static_cast<juce::uint8>(buffer_[7] & 0x3F);
            master[8] = static_cast<juce::uint8>(buffer_[8] & 0x3F);

            constexpr std::array<size_t, 10> sharedMidiOffsets {
                11, 12, 13, 14, 17, 18, 19, 20, 32, 35
            };
            for (const auto offset : sharedMidiOffsets)
                master[offset] = buffer_[offset];
        }

        static bool decodeSysEx(const juce::MemoryBlock& sysex,
                                std::vector<juce::uint8>& packedData)
        {
            const auto length = sysex.getSize();
            size_t dataStart = 0;
            if (! getDataStart(sysex, dataStart))
                return false;

            const auto* bytes = static_cast<const juce::uint8*>(sysex.getData());
            const size_t checksumIndex = length - 2;
            return decodePackedData(bytes, dataStart, checksumIndex, packedData);
        }

    private:
        static bool getDataStart(const juce::MemoryBlock& sysex, size_t& dataStart) noexcept
        {
            const auto length = sysex.getSize();
            if (length == SysExConstants::kMatrix6MasterMessageLength)
                dataStart = 5;
            else if (length == SysExConstants::kMatrix6MasterUnversionedMessageLength)
                dataStart = 4;
            else
                return false;

            const auto* bytes = static_cast<const juce::uint8*>(sysex.getData());
            if (bytes[0] != SysExConstants::kSysExStart)
                return false;
            if (bytes[1] != SysExConstants::kManufacturerIdOberheim)
                return false;
            if (bytes[2] != SysExConstants::kDeviceIdMatrix1000)
                return false;
            if (bytes[3] != SysExConstants::Opcode::kMasterParameterData)
                return false;
            if (bytes[length - 1] != SysExConstants::kSysExEnd)
                return false;
            if (dataStart == 5 && bytes[4] > 0x7F)
                return false;

            const size_t checksumIndex = length - 2;
            return checksumIndex - dataStart == kBufferSize * 2;
        }

        static bool decodePackedData(const juce::uint8* bytes,
                                     size_t dataStart,
                                     size_t checksumIndex,
                                     std::vector<juce::uint8>& packedData)
        {
            std::vector<juce::uint8> decoded(kBufferSize);
            juce::uint8 checksum = 0;
            for (size_t i = 0; i < kBufferSize; ++i)
            {
                const auto low = bytes[dataStart + (i * 2)];
                const auto high = bytes[dataStart + (i * 2) + 1];
                if (low > 0x0F || high > 0x0F)
                    return false;

                decoded[i] = static_cast<juce::uint8>(low | (high << 4));
                checksum = static_cast<juce::uint8>(checksum + decoded[i]);
            }

            if (bytes[checksumIndex] > 0x7F || (checksum & 0x7F) != bytes[checksumIndex])
                return false;

            packedData = std::move(decoded);
            return true;
        }

        static int decodeSigned7(juce::uint8 value) noexcept
        {
            int signedValue = value & 0x7F;
            if ((signedValue & 0x40) != 0)
                signedValue -= 0x80;
            return juce::jlimit(0, 63, signedValue);
        }

        std::array<juce::uint8, kBufferSize> buffer_ {};
    };
} // namespace Core

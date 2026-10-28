#pragma once

#include "Core/MIDI/MasterEditGate.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"

namespace Core
{
    /** Snapshot taken on Device Inquiry success before (or independent of) status update:
        detection/type seen before success, plus consumed port-pair force flag. */
    struct MasterPullInquirySnapshot
    {
        bool wasDetectedBeforeSuccess = false;
        MatrixDeviceTypes::Type previousType = MatrixDeviceTypes::Type::kUnknown;
        bool forceBecausePortPairChanged = false;
    };

    /** Consumes `forceMasterPullOnNextInquirySuccess` into the snapshot (clears the flag). */
    inline MasterPullInquirySnapshot consumeMasterPullInquirySnapshot(
        bool wasDetectedBeforeSuccess,
        MatrixDeviceTypes::Type previousType,
        bool& forceMasterPullOnNextInquirySuccess) noexcept
    {
        const MasterPullInquirySnapshot snapshot {
            wasDetectedBeforeSuccess,
            previousType,
            forceMasterPullOnNextInquirySuccess
        };
        forceMasterPullOnNextInquirySuccess = false;
        return snapshot;
    }

    /** True when Device Inquiry success should silently request Master Parameter Data.
        Pulls on newly detected devices, on port-pair reconnect while still flagged detected,
        and on a device-type transition; not on same-type presence heartbeats. */
    inline bool shouldPullMasterAfterDeviceInquirySuccess(
        bool wasDetectedBeforeSuccess,
        MatrixDeviceTypes::Type previousType,
        MatrixDeviceTypes::Type newType,
        bool forceBecausePortPairChanged = false) noexcept
    {
        if (! isMasterParameterDataAllowed(true, newType))
            return false;

        if (forceBecausePortPairChanged)
            return true;

        if (! wasDetectedBeforeSuccess)
            return true;

        return previousType != newType;
    }

    inline bool shouldPullMasterAfterDeviceInquirySuccess(
        const MasterPullInquirySnapshot& snapshot,
        MatrixDeviceTypes::Type newType) noexcept
    {
        return shouldPullMasterAfterDeviceInquirySuccess(snapshot.wasDetectedBeforeSuccess,
                                                         snapshot.previousType,
                                                         newType,
                                                         snapshot.forceBecausePortPairChanged);
    }
}

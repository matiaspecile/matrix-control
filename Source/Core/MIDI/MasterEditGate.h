#pragma once

#include "Shared/Definitions/MatrixDeviceTypes.h"

namespace Core
{
    /** M-1000's 172-byte Master block is only valid for a detected Matrix-1000. */
    inline bool isMasterEditAllowed(bool deviceDetected,
                                    MatrixDeviceTypes::Type deviceType) noexcept
    {
        return deviceDetected && deviceType == MatrixDeviceTypes::Type::kMatrix1000;
    }

    /** Keep MASTER EDIT content visible; unsupported controls are disabled in each module. */
    inline bool shouldHideMasterEditContent(bool,
                                            bool,
                                            MatrixDeviceTypes::Type) noexcept
    {
        return false;
    }

    /** Full Master Parameter Data is supported on each detected Matrix device type. */
    inline bool isMasterParameterDataAllowed(bool deviceDetected,
                                             MatrixDeviceTypes::Type deviceType) noexcept
    {
        return deviceDetected && MatrixDeviceTypes::isSupportedMatrixDevice(deviceType);
    }
}
